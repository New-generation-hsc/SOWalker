#include <omp.h>
#include "api/constants.hpp"
#include "engine/config.hpp"
#include "engine/cache.hpp"
#include "engine/schedule.hpp"
#include "engine/walk.hpp"
#include "engine/engine.hpp"
#include "engine/sample.hpp"
#include "logger/logger.hpp"
#include "util/io.hpp"
#include "util/util.hpp"
#include "metrics/metrics.hpp"
#include "metrics/reporter.hpp"
#include "apps/secondorder.hpp"


int main(int argc, const char *argv[])
{
    assert(argc >= 2);
    set_argc(argc, argv);
    logstream(LOG_INFO) << "app : " << argv[0] << ", dataset : " << argv[1] << std::endl;
    std::string input = remove_extension(argv[1]);
    std::string base_name = randgraph_output_filename(get_path_name(input), get_file_name(input), BLOCK_SIZE);

    /* graph meta info */
    vid_t nvertices;
    eid_t nedges;
    bool weighted = get_option_bool("weighted");
    bool reordered = get_option_bool("reordered");
    size_t max_iter = get_option_int("iter", 30);

    size_t cache_size = get_option_int("cache", MEMORY_CACHE / (1024LL * 1024 * 1024));
    wid_t walks = (wid_t)get_option_int("walksource", 100000);
    hid_t steps = (hid_t)get_option_int("length", 25);
    real_t alpha = (real_t)get_option_float("alpha", 0.2);

    load_graph_meta(base_name, &nvertices, &nedges, weighted);

    graph_config conf = {
        base_name,
        0,
        cache_size * 1024LL * 1024 * 1024,
        BLOCK_SIZE,
        (tid_t)omp_get_max_threads(),
        nvertices,
        nedges,
        weighted,
        reordered
    };

    graph_block blocks(&conf);
    bid_t nmblocks = get_option_int("nmblocks", blocks.nblocks);

    metrics m("autoregressive_walksource_" + std::to_string(walks) + "_steps_" + std::to_string(steps) + "_dataset_" + argv[1] + "_iter_" + std::to_string(max_iter));
    graph_driver driver(&conf, m);

    graph_walk walk_mangager(conf, driver, blocks);

    graph_cache cache(min_value(nmblocks, blocks.nblocks), &conf);

    transit_func_t app_func;
    app_func.query_equal_func = [&alpha](const transit_context_t &prev_vertex, const transit_context_t &cur_vertex)
    {
        vid_t max_degree = std::max(prev_vertex.degree, cur_vertex.degree);
        return (1.0 - alpha) * max_degree / cur_vertex.degree;
    };
    app_func.query_comm_neighbor_func = [&alpha](const transit_context_t &prev_vertex, const transit_context_t &cur_vertex)
    {
        vid_t max_degree = std::max(prev_vertex.degree, cur_vertex.degree);
        return (1.0 - alpha) * max_degree / cur_vertex.degree +  alpha * max_degree / prev_vertex.degree;
    };
    app_func.query_other_vertex_func = [&alpha](const transit_context_t &prev_vertex, const transit_context_t &cur_vertex)
    {
        vid_t max_degree = std::max(prev_vertex.degree, cur_vertex.degree);
        return (1.0 - alpha) * max_degree / cur_vertex.degree;
    };
    app_func.query_upper_bound_func = [&alpha](const transit_context_t &prev_vertex, const transit_context_t &cur_vertex)
    {
        vid_t max_degree = std::max(prev_vertex.degree, cur_vertex.degree);
        if(prev_vertex.degree > 0)
            return (1.0 - alpha) * max_degree / cur_vertex.degree + alpha * max_degree / prev_vertex.degree;
        else
            return (1.0 - alpha) * max_degree / cur_vertex.degree;
    };
    app_func.query_lower_bound_func = [&alpha](const transit_context_t &prev_vertex, const transit_context_t &cur_vertex)
    {
        vid_t max_degree = std::max(prev_vertex.degree, cur_vertex.degree);
        return (1.0 - alpha) * max_degree / cur_vertex.degree;
    };

    second_order_app_t userprogram(walks, steps, app_func);
    graph_engine engine(cache, walk_mangager, driver, conf, m);

    naive_sample_t naive_sampler;
    its_sample_t its_sampler;
    alias_sample_t alias_sampler;
    reject_sample_t reject_sampler;

    // scheduler *scheduler = nullptr;
    sample_policy_t *sampler = nullptr;
    std::string type = get_option_string("sample", "its");
    if (type == "its")
        sampler = &its_sampler;
    else if (type == "alias")
        sampler = &alias_sampler;
    else if (type == "reject")
        sampler = &reject_sampler;
    else
        sampler = &its_sampler;

    logstream(LOG_INFO) << "sample policy : " << sampler->sample_name() << std::endl;

    lp_solver_scheduler_t walk_scheduler(m);

    auto init_func = [walks, steps](graph_walk *walk_manager)
    {
        #pragma omp parallel for schedule(static)
        for(wid_t off = 0; off < walks; off++)
        {
            vid_t vertex = rand() % walk_manager->nvertices;
            walker_t walker = walker_makeup(vertex, off, vertex, vertex, steps);
            walk_manager->move_walk(walker);
        }
    };

    engine.prologue(userprogram, init_func);
    engine.run(userprogram, &walk_scheduler, sampler);
    engine.epilogue(userprogram);

    sampler->report();
    metrics_report(m);

    return 0;
}
