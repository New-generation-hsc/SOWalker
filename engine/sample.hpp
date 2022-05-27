#ifndef _GRAPH_SAMPLE_H_
#define _GRAPH_SAMPLE_H_

#include <cstdlib>
#include <ctime>
#include <vector>
#include <algorithm>
#include <cassert>
#include <numeric>
#include <iostream>
#include <limits>
#include "api/types.hpp"
#include "metrics/metrics.hpp"
#include "context.hpp"
#include "cache.hpp"
#include "util/timer.hpp"

const vid_t INF = std::numeric_limits<vid_t>::max();

template <typename iterator_type>
size_t naive_sample_impl(const iterator_type &first, const iterator_type &last, unsigned int *seed)
{
    size_t n = static_cast<size_t>(last - first);
    size_t ret = rand_r(seed) % n;
    // logstream(LOG_DEBUG) << "naive_sample seed : " << *seed << ", n = " << n << ", res = " << ret << std::endl;
    return ret;
}

template <typename iterator_type>
size_t its_sample_impl(const iterator_type &first, const iterator_type &last, unsigned int *seed)
{
    size_t n = static_cast<size_t>(last - first);
    std::vector<real_t> prefix_sum(n + 1, 0.0);
    int idx = 1;
    for (iterator_type iter = first; iter != last; ++iter)
    {
        prefix_sum[idx] = prefix_sum[idx - 1] + *iter;
        idx++;
    }

    real_t diff = prefix_sum[n];
    real_t random = (real_t)rand_r(seed) / (real_t)RAND_MAX;
    real_t rand_val = diff * random;
    size_t pos = std::upper_bound(prefix_sum.begin(), prefix_sum.end(), rand_val) - prefix_sum.begin();
    assert(pos > 0);
#ifdef TEST_SAMPLE
    std::cout << "prefix:";
    for (const auto &s : prefix_sum)
        std::cout << " " << s;
    std::cout << std::endl;
#endif
    return pos - 1;
}

template <typename iterator_type>
size_t alias_sample_impl(const iterator_type &first, const iterator_type &last, unsigned int *seed)
{
    size_t n = static_cast<size_t>(last - first);
    std::vector<real_t> aux_weights(first, last);
    real_t sum = std::accumulate(first, last, 0.0);
    real_t avg = sum / static_cast<real_t>(n);
    std::vector<real_t> prob_table(n), alias_table(n);
    std::vector<size_t> small_table, large_table;
    for (iterator_type iter = first; iter != last; ++iter)
    {
        if (*iter < avg)
            small_table.push_back(iter - first);
        else
            large_table.push_back(iter - first);
    }
    while (!small_table.empty() && !large_table.empty())
    {
        size_t s = small_table.back(), l = large_table.back();
        prob_table[s] = aux_weights[s];
        alias_table[s] = l;
        small_table.pop_back();
        large_table.pop_back();
        real_t r = (aux_weights[s] + aux_weights[l]) - avg;
        aux_weights[l] = r;
        if (r < avg)
            small_table.push_back(l);
        else
            large_table.push_back(l);
    }

    while (!large_table.empty())
    {
        size_t l = large_table.back();
        large_table.pop_back();
        prob_table[l] = avg;
        alias_table[l] = n;
    }

    while (!small_table.empty())
    {
        size_t s = small_table.back();
        small_table.pop_back();
        prob_table[s] = avg;
        alias_table[s] = n;
    }

#ifdef TEST_SAMPLE
    std::cout << "avg: " << avg << std::endl;
    std::cout << "prob table:";
    for (const auto &s : prob_table)
        std::cout << " " << s;
    std::cout << "\nalias table:";
    for (const auto &s : alias_table)
        std::cout << " " << s;
    std::cout << std::endl;
#endif

    real_t rand_val = static_cast<real_t>(rand_r(seed)) / static_cast<real_t>(RAND_MAX) * avg;
    size_t rand_pos = rand_r(seed) % n;
    if (rand_val < prob_table[rand_pos])
        return rand_pos;
    else
        return alias_table[rand_pos];
}

template <typename iterator_type>
size_t reject_sample_impl(const iterator_type &first, const iterator_type &last, unsigned int *seed)
{
    size_t n = static_cast<size_t>(last - first);
    real_t pmax = *std::max_element(first, last);
    real_t rand_val = static_cast<real_t>(rand_r(seed)) / static_cast<real_t>(RAND_MAX) * pmax;
    size_t rand_pos = rand_r(seed) % n;
    while (rand_val > *(first + rand_pos))
    {
        rand_pos = rand_r(seed) % n;
        rand_val = static_cast<real_t>(rand_r(seed)) / static_cast<real_t>(RAND_MAX) * pmax;
    }
    return rand_pos;
}

class sample_policy_t
{
private:
    uint64_t sample_counter;
public:
    sample_policy_t() { sample_counter = 0; }

    virtual size_t sample(const std::vector<real_t>& weights) {
        return INF;
    }
    virtual size_t sample(const real_t* first, const real_t* last) {
        return INF;
    }
    virtual vid_t sample(walk_context<UNWEIGHTED>& ctx, walk_timer* wtimer) {
        return INF;
    }
    virtual vid_t sample(walk_context<WEIGHTED>& ctx, walk_timer* wtimer) {
        return INF;
    }
    virtual std::string sample_name() const {
        return "base_sample_policy";
    }
    void increase() {
        __sync_fetch_and_add(&sample_counter, 1ul);
    }
    void report() {
        std::cout << "sample counter : " << sample_counter << std::endl;
    }
};

/**
 * This method generates a uniform [0, 1] random variable r
*/
class naive_sample_t : public sample_policy_t {
public:
    naive_sample_t() {}

    virtual size_t sample(const std::vector<real_t> &weights)
    {
        unsigned int local_seed = time(NULL);
        return naive_sample_impl(weights.begin(), weights.end(), &local_seed);
    }

    virtual size_t sample(const real_t *first, const real_t *last)
    {
        unsigned int local_seed = time(NULL);
        return naive_sample_impl(first, last, &local_seed);
    }
    virtual vid_t sample(walk_context<UNWEIGHTED> &ctx, walk_timer* wtimer)
    {
        size_t off = naive_sample_impl(ctx.adj_start, ctx.adj_end, ctx.local_seed);
        return ctx.adj_start[off];
    }
    virtual vid_t sample(walk_context<WEIGHTED> &ctx, walk_timer *wtimer)
    {
        size_t off = naive_sample_impl(ctx.adj_start, ctx.adj_end, ctx.local_seed);
        return ctx.adj_start[off];
    }
    std::string sample_name() const {
        return "naive_sample";
    }
};

/** This method first computes the cumulative distribution of weights,
 * then generates a random number x between [0, P_sum),
 * finally uses a binary search to find the smallest index i such that x < P_i
*/
class its_sample_t : public sample_policy_t {
public:
    its_sample_t() {}

    virtual size_t sample(const std::vector<real_t> &weights)
    {
        unsigned int local_seed = time(NULL);
        return its_sample_impl(weights.begin(), weights.end(), &local_seed);
    }

    virtual size_t sample(const real_t *first, const real_t *last)
    {
        unsigned int local_seed = time(NULL);
        return its_sample_impl(first, last, &local_seed);
    }

    virtual vid_t sample(walk_context<UNWEIGHTED> &ctx, walk_timer* wtimer)
    {
        std::vector<real_t> adj_weights;
        ctx.query_neigbors_weight(adj_weights);
        size_t off = its_sample_impl(adj_weights.begin(), adj_weights.end(), ctx.local_seed);
        return ctx.adj_start[off];
    }

    virtual vid_t sample(walk_context<WEIGHTED> &ctx, walk_timer* wtimer)
    {
        std::vector<real_t> adj_weights;
        ctx.query_neigbors_weight(adj_weights);
        size_t off = its_sample_impl(adj_weights.begin(), adj_weights.end(), ctx.local_seed);
        return ctx.adj_start[off];
    }

    std::string sample_name() const {
        return "its_sample";
    }
};

/**
 * This method builds two tables: the probability table H, and the alias table A.
 * The generation phase generates a uniform integer x and retrieves H[x].
 * Next, it generates a uniform real number y in [0, 1), if y < H[x], then pick the first, otherwise select the second.
 *
 * for more details, pelease refer to : https://www.keithschwarz.com/darts-dice-coins/
 */
class alias_sample_t : public sample_policy_t {
public:
    alias_sample_t() {}

    virtual size_t sample(const std::vector<real_t> &weights)
    {
        unsigned int local_seed = time(NULL);
        return alias_sample_impl(weights.begin(), weights.end(), &local_seed);
    }

    virtual size_t sample(const real_t *first, const real_t *last)
    {
        unsigned int local_seed = time(NULL);
        return alias_sample_impl(first, last, &local_seed);
    }

    virtual vid_t sample(walk_context<UNWEIGHTED> &ctx, walk_timer* wtimer)
    {
        std::vector<real_t> adj_weights;
        ctx.query_neigbors_weight(adj_weights);
        size_t off = alias_sample_impl(adj_weights.begin(), adj_weights.end(), ctx.local_seed);
        return ctx.adj_start[off];
    }

    virtual vid_t sample(walk_context<WEIGHTED> &ctx, walk_timer* wtimer)
    {
        std::vector<real_t> adj_weights;
        ctx.query_neigbors_weight(adj_weights);
        size_t off = alias_sample_impl(adj_weights.begin(), adj_weights.end(), ctx.local_seed);
        return ctx.adj_start[off];
    }

    std::string sample_name() const {
        return "alias_sample";
    }
};

/**
 * This method can be viewed as throwing a dart on a rectangle dartboard
 * util hitting the target area. It follows the following steps:
 * - generate a uniform number x and a uniform real number y in [0, p_max)
 * - if y < p_x then select the target area
 * - else repeat step 1)
*/
class reject_sample_t : public sample_policy_t {
public:
    reject_sample_t() {}

    virtual size_t sample(const std::vector<real_t> &weights)
    {
        unsigned int local_seed = time(NULL);
        return reject_sample_impl(weights.begin(), weights.end(), &local_seed);
    }

    virtual size_t sample(const real_t *first, const real_t *last)
    {
        unsigned int local_seed = time(NULL);
        return reject_sample_impl(first, last, &local_seed);
    }

    virtual vid_t sample(walk_context<UNWEIGHTED> &ctx, walk_timer* wtimer)
    {
        wtimer->start_time("reject_sample");
        size_t n = static_cast<size_t>(ctx.adj_end - ctx.adj_start);
        real_t pmax = ctx.query_max_weight(), pmin = ctx.query_min_weight();
        bool accept = false;
        size_t rand_pos = 0;
        size_t counter = 0;
        while(!accept) {
            real_t rand_val = static_cast<real_t>(rand_r(ctx.local_seed)) / static_cast<real_t>(RAND_MAX) * pmax;
            rand_pos = rand_r(ctx.local_seed) % n;
            sample_policy_t::increase();
            if(rand_val <= pmin || rand_val < ctx.query_vertex_weight(rand_pos)) {
                accept = true;
            }
            counter++;
            if(counter > 500) {
                logstream(LOG_ERROR) << "reject sample " << counter << " times, vertex : " << ctx.cur_vertex << ", pmax : " << pmax << ", pmin : " << pmin  << ", prev_vertex degree : " << (ctx.prev_adj_end - ctx.prev_adj_start) << ", cur_vertex degree : " << (ctx.adj_end - ctx.adj_start) << std::endl;
                exit(0);
            }
        }
        wtimer->stop_time("reject_sample");
        return ctx.adj_start[rand_pos];
    }

    virtual vid_t sample(walk_context<WEIGHTED> &ctx, walk_timer *wtimer)
    {
        std::vector<real_t> adj_weights;
        ctx.query_neigbors_weight(adj_weights);
        size_t off = reject_sample_impl(adj_weights.begin(), adj_weights.end(), ctx.local_seed);
        return ctx.adj_start[off];
    }

    std::string sample_name() const {
        return "reject_sample";
    }
};


template<WeightType wht_type>
vid_t vertex_sample(walk_context<wht_type> &ctx, sample_policy_t *sampler, walk_timer *wtimer = nullptr) {
    eid_t deg = (eid_t)(ctx.adj_end - ctx.adj_start);
    if(deg == 0) return rand_r(ctx.local_seed) % ctx.nvertices;
    else if(deg == 1) return *ctx.adj_start;
    else {
        return sampler->sample(ctx, wtimer);
    }
}


#endif
