#ifndef UTIL_HPP
#define UTIL_HPP
#include <cstdio>
#include <iostream>
#include <vector>
#include <cstdint>
#include <iomanip>
#include <cmath>
namespace dtsw{
    template<typename T>
    struct quad{
        T v[4];
    };
    typedef quad<double> quad_t;
    /*------------------------------------------*/
    void read_var_D(const char *filename,
                    std::vector< std::pair<uint32_t, uint32_t> > &idx,
                    std::vector< quad<double> > &data);
    void read_var_D_block(const char * filename,
                    std::vector< std::pair<uint32_t, uint32_t> > &idx,
                    std::vector< quad<double> > &data,
                    size_t item_count, size_t block_idx);
    void print_index(std::vector< std::pair<uint32_t, uint32_t> > &idx,int from,int N);
    void print_data(std::vector< quad<double> > &data,int from,int N);
    void test_read_sparse();
    void read_var_H(const char *filename,
                    std::vector<double> &data);
    void read_var_H_block(const char *filename,
                    std::vector<double> &data,
                    int item_count,
                    int block_index);
    void test_read_H();
    void read_var_Atm(const char *filename,
                    std::vector<double> &data);
    void read_var_Atm_block(const char *filename,
                    std::vector<double> &data,
                    int item_count,
                    int block_index);
    void test_read_Atm();
    void test_sparse_d_dist();
    void test_vector_H_dist();
    void test_vector_Atm_dist();
}
#endif // UTIL_HPP
