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
    DTSWData (int M, int N, int r,int c, std::string n, bool isSparse = false):name(n){
      level2_mem_size = M*N*sizeof(quad<double>)/r/c;
      for(int j=0;j<c;j++){
	for(int i=0;i<r;i++){
	  DTSWData*t=new DTSWData;
	  t->row_idx = i;
	  t->col_idx = j;
	  t->sp_row  = i;
	  t->sp_col  = j;
	  if(!isSparse){
	    t->level2_mem_size = level2_mem_size;
	    t->memory = new byte[level2_mem_size];
	  }
	  std::stringstream ss;
	  if ( c>1)
	    ss << n << "(" << i << "," << j << ")";
	  else
	    ss << n << "(" << i <<  ")";
	  t->setName(ss.str());
	  name.assign(getName());
	  Dlist.push_back(t);
	}
      }
      rows = r;
      cols = c;
    }
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
    void partition_2nd_level(int nby,int nbx){
      for(auto d: Dlist){
	d->sg_data->partition_data(*d,nby,nbx);
      }
    }
    /*---------------------------------------------------------------------------*/
    byte *get_memory(){return memory;}
    void set_memory(byte *mem){memory=mem;}
    /*---------------------------------------------------------------------------*/

  };
  typedef DTSWData Data;
  
}
#endif //DT_DATABASE_HPP
