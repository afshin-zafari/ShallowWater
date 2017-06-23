#ifndef UTIL_HPP
#define UTIL_HPP
#include <cstdio>
#include <iostream>
#include <vector>
#include <cstdint>
#include <iomanip>
#include <cmath>

//#define LOG_DTSW 0
#ifndef LOG_DTSW
#define LOG_DTSW 0x0FFFFFFFF
#endif

namespace dtsw{
  template<typename T>
  struct quad{
    T v[4];
    quad<T> operator *(quad<T> rhs){
      quad<T> t;
      t.v[0] = v[0] * rhs.v[0];
      t.v[1] = v[1] * rhs.v[1];
      t.v[2] = v[2] * rhs.v[2];
      t.v[3] = v[3] * rhs.v[3];
      return t;
    }
    quad<T> operator=(quad<T> rhs){
      v[0] = rhs.v[0];
      v[1] = rhs.v[1];
      v[2] = rhs.v[2];
      v[3] = rhs.v[3];
    }
    quad<T> operator +(quad<T> rhs){
      quad<T> t;
      t.v[0] = v[0] + rhs.v[0];
      t.v[1] = v[1] + rhs.v[1];
      t.v[2] = v[2] + rhs.v[2];
      t.v[3] = v[3] + rhs.v[3];
      return t;
    }
    template <typename R > friend  quad<R> operator *(R  lhs, quad<R> rhs);
    quad<T> operator *( double rhs){
      return rhs  * (*this);
    }
    quad<T> operator+=(quad<T> rhs){
      v[0] +=  rhs.v[0];
      v[1] +=  rhs.v[1];
      v[2] +=  rhs.v[2];
      v[3] +=  rhs.v[3];
    }
  };
  using quad_t =  quad<double>;

  quad<double> operator *(double lhs, quad<double> rhs){
    quad<double> t;
    t.v[0] = lhs * rhs.v[0];
    t.v[1] = lhs * rhs.v[1];
    t.v[2] = lhs * rhs.v[2];
    t.v[3] = lhs * rhs.v[3];
    return t;
  }
  /*----------------------------------------------------------------*/
  struct SpInfo{
    std::vector<dtsw::quad<double>> data;
    std::vector<std::pair<uint32_t,uint32_t> > index;
    int num_blocks_x,num_blocks_y,rb,cb;
    bool empty;
    typedef std::vector<SpInfo*> SpInfoList;
    SpInfoList   sp_blocks;
    SpInfo(){}
    SpInfo(int i, int j,bool ){}
    void report_data(){
      LOG_INFO(LOG_DTSW,"Sparse Info has %d elements \n",data.size());
      if (data.size()){
	LOG_INFO(LOG_DTSW," from row %d and column %d.\n",rb,cb);
      }
      for(auto s:sp_blocks){
	s->report_data();
      }
    }
  };
  typedef std::vector<SpInfo*> SpInfoList;
  /*------------------------------------------*/
  uint64_t read_var_D(const char *filename,
		  std::vector< std::pair<uint32_t, uint32_t> > &idx,
		  std::vector< quad<double> > &data);
  void split(SpInfo  &M, int ny, int nx,int chunk_size);
  void read_var_H(const char *filename,
		  std::vector<double> &data);
  /*=================================================*/
  void read_var_D_block(const char * filename,
			std::vector< std::pair<uint32_t, uint32_t> > &idx,
			std::vector< quad<double> > &data,
			size_t item_count, size_t block_idx);
  void print_index(std::vector< std::pair<uint32_t, uint32_t> > &idx,int from,int N);
  void print_data(std::vector< quad<double> > &data,int from,int N);
  void test_read_sparse();
  void test_read_H();
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
