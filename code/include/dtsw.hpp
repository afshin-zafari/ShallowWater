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
  typedef struct Distribution{
    int p,q,P;
    int rows_per_block,cols_per_block;
    int blocks_per_row,blocks_per_col;

  }Distribution_t;
  /*------------------------------------------*/
  extern Distribution_t dist;
  /*----------------------------------------*/
  typedef struct parameters{
    int nb;
    double dt;
    Distribution_t dist;
    char *filename;
    int chunk_size;
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
