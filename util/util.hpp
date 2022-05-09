#ifndef _GRAPH_UTIL_H_
#define _GRAPH_UTIL_H_

#include <string>
#include <sstream>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <cstdio>
#include <cmath>
#include "logger/logger.hpp"

// for windows mkdir
#ifdef _WIN32
#include <dirent.h>
#endif

#define max_value(a, b) (((a) > (b)) ? (a) : (b))
#define min_value(a, b) (((a) < (b)) ? (a) : (b))

#ifndef NDEBUG
#define ASSERT(condition, message)                                             \
    do                                                                         \
    {                                                                          \
        if (!(condition))                                                      \
        {                                                                      \
            std::cerr << "Assertion `" #condition "` failed in " << __FILE__   \
                      << " line " << __LINE__ << ": " << message << std::endl; \
            std::terminate();                                                  \
        }                                                                      \
    } while (false)
#else
#define ASSERT(condition, message) \
    do                             \
    {                              \
    } while (false)
#endif

template<typename T>
std::string concatnate_name(std::string filename, T val) {
    std::stringstream ss;
    ss << filename;
    ss << "_" << val;
    return ss.str();
}

std::string base_name(std::string const & path) {
    return path.substr(path.find_last_of("/\\")+1);
}

inline std::string file_base_name(std::string const &path) {
    return base_name(path);
}

std::string remove_extension(std::string const & filename) {
    std::string::size_type const p(filename.find_last_of('.'));
    return p>0&&p!=std::string::npos ? filename.substr(0, p) : filename;
}

/** test a file existence */
bool test_exists(const std::string & filename) {
    struct stat buffer;
    return (stat(filename.c_str(), &buffer) == 0);
}

/** test a file existence and if the file exist then delete it */
bool test_delete(const std::string & filename) {
    if(test_exists(filename)) {
        std::remove(filename.c_str());
        return true;
    }
    return false;
}

/** given data vertex, return the block that the vertex belongs to */
bid_t get_block(std::vector<vid_t>& vblocks, vid_t v) {
    bid_t nblocks = vblocks.size() - 1;
    for(bid_t p = 0; p < nblocks; p++) {
        if(v < vblocks[p+1]) return p;
    }
    return nblocks;
}

std::string get_path_name(const std::string& s) {
    char sep = '/';
#ifdef _WIN32
    sep = '\\';
#endif
    size_t pos = s.rfind(sep, s.length());
    if(pos != std::string::npos) return s.substr(0, pos + 1);
    return "./";
}

std::string get_file_name(const std::string& s) {
    char sep = '/';
#ifdef _WIN32
    sep = '\\';
#endif
    size_t pos = s.rfind(sep, s.length());
    if (pos != std::string::npos) return s.substr(pos + 1, s.length() - pos - 1);
    return s;
}

int sowalker_mkdir(const char* path) {
#ifdef _WIN32
    return ::_mkdir(path);
#else
    return ::mkdir(path, 0777);
#endif
}

bool test_folder_exists(const std::string& folder_name) {
    struct stat st;
    int ret = stat(folder_name.c_str(), &st);
    return ret == 0 && (st.st_mode & S_IFDIR);
}

/**
 * @brief 
 * @param base_name the base_name is the dataset_path removes extension
 */
inline std::string get_beg_pos_name(std::string const &base_name) {
    return base_name + ".beg";
}

inline std::string get_csr_name(std::string const & base_name) {
    return base_name + ".csr";
}

inline std::string get_weights_name(std::string const & base_name) {
    return base_name + ".wht";
}

inline std::string get_meta_name(std::string const & base_name) {
    return base_name + ".meta";
}

std::string get_dataset_block_folder(std::string const& base_name, size_t blocksize) {
    std::string folder = get_path_name(base_name);
    folder += concatnate_name("sowalker", blocksize / (1024 * 1024));
    return folder;
}

std::string get_vert_blocks_name(std::string const &base_name, size_t blocksize)
{
    std::string folder = get_dataset_block_folder(base_name, blocksize), dataset_name = get_file_name(base_name);
    dataset_name = concatnate_name(dataset_name, blocksize / (1024 * 1024)) + "MB.vert.blocks";
    return folder + "/" + dataset_name;
}

std::string get_edge_blocks_name(std::string const & base_name, size_t blocksize) {
    std::string folder = get_dataset_block_folder(base_name, blocksize), dataset_name = get_file_name(base_name);
    dataset_name = concatnate_name(dataset_name, blocksize / (1024 * 1024)) + "MB.edge.blocks";
    return folder + "/" + dataset_name;
}

std::string get_walk_name(std::string const &base_name, size_t blocksize, bid_t blk)
{
    std::string folder = get_dataset_block_folder(base_name, blocksize);
    std::string walk_name = std::to_string(blk) + ".walk";
    return folder + "/" + walk_name;
}

std::string get_expected_walk_length_name(std::string const & base_name, size_t blocksize) {
    std::string folder = get_dataset_block_folder(base_name, blocksize), dataset_name = get_file_name(base_name);
    dataset_name += ".exp";
    return folder + "/" + dataset_name;
}

bool test_dataset_processed_exists(std::string const &base_name) {
    std::string beg_pos_name = get_beg_pos_name(base_name);
    std::string csr_name = get_csr_name(base_name);
    std::string meta_name = get_meta_name(base_name);

    return test_exists(beg_pos_name) && test_exists(csr_name) && test_exists(meta_name);
}

void delete_processed_dataset(std::string const &base_name) {
    std::string beg_pos_name = get_beg_pos_name(base_name);
    std::string csr_name = get_csr_name(base_name);
    std::string meta_name = get_meta_name(base_name);

    test_delete(beg_pos_name);
    test_delete(csr_name);
    test_delete(meta_name);
}

bool test_dataset_block_data_exists(std::string const & base_name, size_t blocksize) {
    std::string vert_block_name = get_vert_blocks_name(base_name, blocksize);
    std::string edge_block_name = get_edge_blocks_name(base_name, blocksize);
    std::string exp_block_name = get_expected_walk_length_name(base_name, blocksize);

    return test_exists(vert_block_name) && test_exists(edge_block_name) && test_exists(exp_block_name);
}

void delete_processed_block_data(std::string const & base_name, size_t blocksize) {
    std::string vert_block_name = get_vert_blocks_name(base_name, blocksize);
    std::string edge_block_name = get_edge_blocks_name(base_name, blocksize);
    std::string exp_block_name  = get_expected_walk_length_name(base_name, blocksize);

    test_delete(vert_block_name);
    test_delete(edge_block_name);
    test_delete(exp_block_name);
}

#endif
