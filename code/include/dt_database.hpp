#ifndef DT_DATABASE_HPP
#define DT_DATABASE_HPP
#include "ductteip.hpp"
#include "sg_database.hpp"
namespace dtsw{
  enum {ADD,DIFF,RHS,STEP,TIMESTEPS};
  /*=============================================================================*/
  class DTSWData : public IData {
  private:
    std::vector <DTSWData *> Dlist;
    int rows,cols,row_idx,col_idx,host;
    byte *memory;
  public:
    SGData *sg_data;
    int sp_row,sp_col,level2_mem_size;
    std::string name;
    /*---------------------------------------------------------------------------*/
    DTSWData();
    /*---------------------------------------------------------------------------*/
    DTSWData (int M, int N, int r,int c, std::string n, bool isSparse = false, bool isQuadVec4=false);
    /*---------------------------------------------------------------------------*/
    DTSWData &operator()(int i, int j){
      return *Dlist[j*rows + i];
    }
    /*---------------------------------------------------------------------------*/
    DTSWData &operator()(int i){
      return this->operator()(i,0);
    }
    /*---------------------------------------------------------------------------*/
    int size(){return rows*cols;}
    /*---------------------------------------------------------------------------*/
    void getExistingMemoryInfo(byte **b, int *s, int *l){
      *b = (byte *)memory;
      *s = level2_mem_size;
      *l = 1;
    }
    /*---------------------------------------------------------------------------*/
    void setNewMemoryInfo(MemoryItem*mi){
      //M = level2_mem_size / sizeof (quad<double>);
      //N = 1;
      memory = (byte*)(mi->getAddress()+getHeaderSize());
    }
    int get_rows(){return rows;}
    int get_cols(){return cols;}
    int get_block_row(){return row_idx;}
    void partition_2nd_level(int nby,int nbx){
      for(auto d: Dlist){
	LOG_INFO(LOG_DTSW,"%s(%d,%d) partitioned.\n",d->name.c_str(),d->row_idx,d->col_idx);
	if (d->sg_data)
	  d->sg_data->partition_data(*d,nby,nbx);
	else
	  LOG_INFO(LOG_DTSW,"no sg data. \n");
      }
    }
    /*---------------------------------------------------------------------------*/
    byte *get_memory(){return memory;}
    void set_memory(byte *mem){memory=mem;}
    void report_data();
    /*---------------------------------------------------------------------------*/

  };
  typedef DTSWData Data;
  class IterationData: public DTSWData {
  public:
    IterationData();
  };
  
}
#endif //DT_DATABASE_HPP
