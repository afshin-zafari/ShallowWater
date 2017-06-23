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
    int rows,cols,row_idx,col_idx;
    byte *memory_p;
    int mem_size_in_bytes, mem_size_in_elements,item_size;
  public:
    SGData *sg_data;
    int sp_row,sp_col;
    std::string name_dep;
    /*---------------------------------------------------------------------------*/
    DTSWData();
    /*---------------------------------------------------------------------------*/
    DTSWData (int M, int N, int r,int c, std::string n, int , int , bool isSparse = false);
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
      LOG_INFO(LOG_DTSW,"Reported memory:%p with size %d for data %s.\n",
	       memory_p,mem_size_in_bytes,getName().c_str());
      *b = (byte *)memory_p;
      *s = mem_size_in_bytes;
      *l = 1;
    }
    /*---------------------------------------------------------------------------*/
    void setNewMemoryInfo(MemoryItem*mi){
      memory_p = (byte*)(mi->getAddress()+getHeaderSize());
    }
    /*---------------------------------------------------------------------------*/
    int get_rows(){return rows;}
    int get_cols(){return cols;}
    int get_block_row(){return row_idx;}
    /*---------------------------------------------------------------------------*/
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
    byte *get_memory(){return memory_p;}
    void set_memory(byte *mem,int size,int n){memory_p=mem;mem_size_in_bytes = size;mem_size_in_elements = n;}
    void report_data();
    int get_mem_size_in_bytes(){return mem_size_in_bytes;}
    int get_mem_size_in_elems(){return mem_size_in_elements;}
    int get_item_size(){return item_size;}
    
    /*---------------------------------------------------------------------------*/

  };
  typedef DTSWData Data;
  class IterationData: public DTSWData {
  public:
    IterationData();
  };
  
}
#endif //DT_DATABASE_HPP
