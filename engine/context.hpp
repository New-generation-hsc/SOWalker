#ifndef _GRAPH_CONTEXT_H_
#define _GRAPH_CONTEXT_H_

#include <cstdlib>
#include <ctime>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include "api/types.hpp"
#include "logger/logger.hpp"
#include "util/hash.hpp"

class context
{
public:
    vid_t cur_vertex, prev_vertex;
    vid_t nvertices;
    vid_t *adj_start, *adj_end;
    vid_t *prev_adj_start, *prev_adj_end;
    transit_func_t app_func;
    unsigned *local_seed;
    
    context(vid_t vertex, vid_t num_vertices, vid_t *start, vid_t *end, vid_t prev, vid_t *p_start, vid_t *p_end, transit_func_t func, unsigned *seed)
    {
        this->cur_vertex = vertex;
        this->nvertices = num_vertices;
        this->adj_start = start,
        this->adj_end = end;
        this->prev_vertex = prev;
        this->prev_adj_start = p_start;
        this->prev_adj_end = p_end;
        this->app_func = func;
        this->local_seed = seed;
    }
};

template <WeightType wht_type>
class walk_context : public context
{
public:
    walk_context(vid_t vertex, vid_t num_vertices, vid_t *start, vid_t *end, vid_t prev,
                 vid_t *p_adj_s, vid_t *p_adj_e, transit_func_t func, unsigned *seed) : context(vertex, num_vertices, start, end, prev, p_adj_s, p_adj_e, func, seed)
    {

    }

    void query_neigbors_weight(std::vector<real_t> &adj_weights)
    {
        transit_context_t c_context = {cur_vertex, static_cast<vid_t>(adj_end - adj_start), nullptr};
        transit_context_t p_context = {prev_vertex, static_cast<vid_t>(prev_adj_end - prev_adj_start), nullptr};

        size_t deg = static_cast<size_t>(adj_end - adj_start);
        adj_weights.resize(deg);
        std::unordered_set<vid_t> prev_neighbors = std::unordered_set<vid_t>(prev_adj_start, prev_adj_end);
        for(size_t index = 0; index < deg; ++index) {
            if(*(adj_start + index) == prev_vertex) {
                adj_weights[index] = app_func.query_equal_func(c_context, p_context);
            }else if(prev_neighbors.find(*(adj_start + index)) != prev_neighbors.end()) {
                adj_weights[index] = app_func.query_comm_neighbor_func(c_context, p_context);
            }else {
                adj_weights[index] = app_func.query_other_vertex_func(c_context, p_context);
            }
        }
    }

    real_t query_max_weight() {
        transit_context_t c_context = {cur_vertex, static_cast<vid_t>(adj_end - adj_start), nullptr};
        transit_context_t p_context = {prev_vertex, static_cast<vid_t>(prev_adj_end - prev_adj_start), nullptr};

        if(cur_vertex == prev_vertex && app_func.query_upper_bound_func) return app_func.query_equal_func(c_context, p_context);
        else if(app_func.query_upper_bound_func) return app_func.query_upper_bound_func(c_context, p_context);
        return 0.0;
    }

    real_t query_min_weight() {
        transit_context_t c_context = {cur_vertex, static_cast<vid_t>(adj_end - adj_start), nullptr};
        transit_context_t p_context = {prev_vertex, static_cast<vid_t>(prev_adj_end - prev_adj_start), nullptr};

        if(cur_vertex == prev_vertex && app_func.query_upper_bound_func) return app_func.query_equal_func(c_context, p_context);
        else if(app_func.query_upper_bound_func) return app_func.query_lower_bound_func(c_context, p_context);
        return 0.0;
    }

    real_t query_vertex_weight(size_t index) {
        transit_context_t c_context = {cur_vertex, static_cast<vid_t>(adj_end - adj_start), nullptr};
        transit_context_t p_context = {prev_vertex, static_cast<vid_t>(prev_adj_end - prev_adj_start), nullptr};

        vid_t next_vertex = *(adj_start + index);
        // if(next_vertex == prev_vertex) return app_param.gamma;
        if(next_vertex == prev_vertex) return app_func.query_equal_func(c_context, p_context);

        bool exist = std::binary_search(prev_adj_start, prev_adj_end, next_vertex);
        if(exist) return app_func.query_comm_neighbor_func(c_context, p_context);
        else return app_func.query_other_vertex_func(c_context, p_context);
    }
};


template <>
class walk_context<WEIGHTED> : public context
{
public:
    real_t *weight_start, *weight_end;
    real_t *prev_weight_start, *prev_weight_end;
    walk_context(vid_t vertex, vid_t num_vertices, vid_t *start, vid_t *end,
                 vid_t prev, vid_t *p_adj_s, vid_t *p_adj_e, transit_func_t func, real_t *wht_start, real_t *wht_end,
                 real_t *p_wht_s, real_t *p_wht_e, unsigned *seed) : context(vertex, num_vertices, start, end, prev, p_adj_s, p_adj_e, func, seed)
    {
        this->weight_start = wht_start;
        this->weight_end = wht_end;
        this->prev_weight_start =  p_wht_s;
        this->prev_weight_end = p_wht_e;
    }

    void query_neigbors_weight(std::vector<real_t> &adj_weights)
    {
        size_t deg = static_cast<size_t>(adj_end - adj_start);
        adj_weights.resize(deg);
        std::unordered_set<vid_t> prev_neighbors = std::unordered_set<vid_t>(prev_adj_start, prev_adj_end);
        for(size_t index = 0; index < deg; ++index) {
            transit_context_t c_context = {cur_vertex, static_cast<vid_t>(adj_end - adj_start), weight_start + index };
            transit_context_t p_context = {prev_vertex, static_cast<vid_t>(prev_adj_end - prev_adj_start), prev_weight_start + index };
            if(*(adj_start + index) == prev_vertex) {
                adj_weights[index] = app_func.query_equal_func(c_context, p_context);
            }else if(prev_neighbors.find(*(adj_start + index)) != prev_neighbors.end()) {
                adj_weights[index] = app_func.query_comm_neighbor_func(c_context, p_context);
            }else {
                adj_weights[index] = app_func.query_other_vertex_func(c_context, p_context);
            }
        }
    }

    real_t query_max_weight() {
        return 0.0;
    }

    real_t query_min_weight() {
        return 0.0;
    }

    real_t query_vertex_weight(size_t index) {
        transit_context_t c_context = {cur_vertex, static_cast<vid_t>(adj_end - adj_start), weight_start + index};
        transit_context_t p_context = {prev_vertex, static_cast<vid_t>(prev_adj_end - prev_adj_start), prev_weight_start + index};

        vid_t next_vertex = *(adj_start + index);
        // if(next_vertex == prev_vertex) return app_param.gamma;
        if(next_vertex == prev_vertex) return app_func.query_equal_func(c_context, p_context);

        bool exist = std::binary_search(prev_adj_start, prev_adj_end, next_vertex);
        if(exist) return app_func.query_comm_neighbor_func(c_context, p_context);
        else return app_func.query_other_vertex_func(c_context, p_context);
    }
};

#endif
