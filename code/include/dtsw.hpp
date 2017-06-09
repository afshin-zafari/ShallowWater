#ifndef DTSW_HPP
#define DTSW_HPP
#include "sw_dist.hpp"
#include "util.hpp"
namespace dtsw{
  /*----------------------------------------*/
  void init(int argc, char *argv[]);
  void finalize();
  void run();
  /*----------------------------------------*/
  typedef struct Partition{    
    union {
      int rows_per_block;
      int M;
      int chunk_size;
    };
    union {
      int cols_per_block;
      int N;
    };
    union{
      int blocks_per_row;
      int Mb;
    };
    union{
      int blocks_per_col;
      int Nb;
    };
  }Partition_t;
  /*----------------------------------------*/
  typedef struct parameters{
    int p,q,P;
    double dt;
    Partition_t partition_level[3];
    char *filename;
    double gh0;
  }Parameters_t;
  /*----------------------------------------*/
  typedef struct atmdata {
    double f;
    double x, y, z;
    double p_u[3], p_v[3], p_w[3];
    double ghm;
    double gradghm[3];
  }atmdata_t;
  /*----------------------------------------*/

  extern Parameters_t Parameters;
  extern atmdata_t *Atm;
  extern SWAlgorithm *sw_engine;
  /*----------------------------------------*/
}
#endif // DTSW_HPP
