#ifndef _SECOND_ORDER_H_
#define _SECOND_ORDER_H_

#include <omp.h>
#include <unordered_set>
#include <functional>
#include "api/types.hpp"
#include "engine/walk.hpp"
#include "engine/sample.hpp"
#include "engine/context.hpp"
#include "util/timer.hpp"
#include "util/hash.hpp"

class second_order_app_t
{
private:
    wid_t _walkpersource;
    hid_t _hops;
    transit_func_t func_param;
    bool continue_update;
    walk_timer wtimer;

public:
    second_order_app_t(wid_t nwalks, hid_t steps, transit_func_t app_func, bool c_update = true)
    {
        _walkpersource = nwalks;
        _hops = steps;
        func_param = app_func;
        continue_update = c_update;
    }

    void prologue(graph_walk *walk_manager, std::function<void(graph_walk *walk_manager)> init_func = nullptr)
    {
        wtimer.register_entry("walker_update");
        wtimer.register_entry("vertex_sample");

        if(init_func) init_func(walk_manager);

        for (bid_t blk = 0; blk < walk_manager->totblocks; blk++)
        {
            walk_manager->set_max_hop(blk, this->_hops);
        }
    }

    wid_t update_walk(const walker_t &walker, graph_cache *cache, graph_walk *walk_manager, sample_policy_t *sampler, unsigned int *seed)
    {
        // tid_t tid = (tid_t)omp_get_thread_num();
        vid_t cur_vertex = WALKER_POS(walker), prev_vertex = WALKER_PREVIOUS(walker);
        hid_t hop = WALKER_HOP(walker);
        bid_t cur_blk = walk_manager->global_blocks->get_block(cur_vertex);
        bid_t prev_blk = walk_manager->global_blocks->get_block(prev_vertex);
        // unsigned seed = (unsigned)(cur_vertex + prev_vertex + hop + tid + time(NULL));
        bid_t cur_cache_index = (*(walk_manager->global_blocks))[cur_blk].cache_index, prev_cache_index = (*(walk_manager->global_blocks))[prev_blk].cache_index;
        bid_t nblocks = walk_manager->global_blocks->nblocks;
        if(cur_cache_index == nblocks || prev_cache_index == nblocks) {
            std::cout << "vertex : " << cur_vertex << " " << prev_vertex << std::endl;
            std::cout << "block : " << cur_blk << " " << prev_blk << std::endl;
            std::cout << "index : " << cur_cache_index << " " << prev_cache_index << std::endl;
        }
        assert(cur_cache_index != nblocks && prev_cache_index != nblocks);

        wtimer.start_time("walker_update");
        wid_t run_step = 0;
        while (cur_cache_index != nblocks && hop < this->_hops)
        {
            cache_block *cur_block = &(cache->cache_blocks[cur_cache_index]);
            cache_block *prev_block = &(cache->cache_blocks[prev_cache_index]);

            vid_t start_vertex = cur_block->block->start_vert, off = cur_vertex - start_vertex;
            vid_t prev_start_vertex = prev_block->block->start_vert, prev_off = prev_vertex - prev_start_vertex;
            eid_t adj_head = cur_block->beg_pos[off] - cur_block->block->start_edge, adj_tail = cur_block->beg_pos[off + 1] - cur_block->block->start_edge;
            eid_t prev_adj_head = prev_block->beg_pos[prev_off] - prev_block->block->start_edge, prev_adj_tail = prev_block->beg_pos[prev_off + 1] - prev_block->block->start_edge;

            vid_t next_vertex = 0;
            wtimer.start_time("vertex_sample");
            if (cur_block->weights == nullptr)
            {
                walk_context<UNWEIGHTED> ctx(cur_vertex, walk_manager->nvertices, cur_block->csr + adj_head, cur_block->csr + adj_tail,
                                            prev_vertex, prev_block->csr + prev_adj_head, prev_block->csr + prev_adj_tail, func_param, seed);
                next_vertex = vertex_sample(ctx, sampler, &wtimer);
            }
            else
            {
                walk_context<WEIGHTED> ctx(cur_vertex, walk_manager->nvertices, cur_block->csr + adj_head, cur_block->csr + adj_tail,
                                            prev_vertex, prev_block->csr + prev_adj_head, prev_block->csr + prev_adj_tail, func_param,
                                            cur_block->weights + adj_head, cur_block->weights + adj_tail, prev_block->weights + prev_adj_head, 
                                            prev_block->weights + prev_adj_tail, seed);
                next_vertex = vertex_sample(ctx, sampler, &wtimer);
            }
            wtimer.stop_time("vertex_sample");
            prev_vertex = cur_vertex;
            cur_vertex = next_vertex;
            hop++;
            run_step++;

            prev_cache_index = cur_cache_index;
            if (!(cur_vertex >= cur_block->block->start_vert && cur_vertex < cur_block->block->start_vert + cur_block->block->nverts))
            {
                cur_blk = walk_manager->global_blocks->get_block(cur_vertex);
                cur_cache_index = (*(walk_manager->global_blocks))[cur_blk].cache_index;
                if(!continue_update) break;
            }
        }
        wtimer.stop_time("walker_update");

        if (hop < this->_hops)
        {
            walker_t next_walker = walker_makeup(WALKER_ID(walker), WALKER_SOURCE(walker), prev_vertex, cur_vertex, hop);
            walk_manager->move_walk(next_walker);
            walk_manager->set_max_hop(next_walker);
        }
        return run_step;
    }

    void epilogue()
    {
        
    }

    wid_t get_numsources() { return _walkpersource; }
    hid_t get_hops() { return _hops; }
};

#endif
