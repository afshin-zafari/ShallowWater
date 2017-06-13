#ifndef DTSW_HPP
#define DTSW_HPP
#include "sw_dist.hpp"
#include "util.hpp"
namespace dtsw{
  typedef byte* Buffer;
  /*------------------------------------------*/
  uint64_t  read_var_D(const char *filename,
		  std::vector< std::pair<uint32_t, uint32_t> > &idx,
		  std::vector< quad<double> > &data);
  void split(SpInfo  &M, int ny, int nx,int chunk_size);
  void read_var_H(const char *filename,
		  std::vector<double> &data);
  //  void read_var_Atm(const char *filename,                    AtmArray &data);
  /*----------------------------------------*/
  void init(int argc, char *argv[]);
  void finalize();
  void run(int , char *[]);
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
    int         p,q,P;
    double      dt;
    Partition_t partition_level[3];
    char       *data_path;
    double      gh0;
    int         atm_block_size_L1,atm_block_size_L2;
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
  typedef atmdata_t *AtmArray;
  typedef AtmArray  *AtmArrayAdr;
  /*----------------------------------------*/
  uint64_t  read_var_Atm(const char *filename,
		    AtmArray &data);
  int  read_var_H_block(const char *,
			Buffer &,
			int ,
			int );
  /*----------------------------------------*/

  extern Parameters_t Parameters;
  extern AtmArray Atm;
  extern SWAlgorithm *sw_engine;
  /*----------------------------------------*/
}
#endif // DTSW_HPP
