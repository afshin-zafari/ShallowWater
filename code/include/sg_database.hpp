#ifndef SG_DATABASE_HPP
#define SG_DATABASE_HPP
#include "util.hpp"
namespace dtsw{
  struct Dimension{
    int num_blocks,num_elems;
  };
  struct Partitions{
    Dimension x,y;
  };
  Partitions Level[3];
  //  Level[0].y.num_elems;
  /*----------------------------------------------------------------*/
  struct Options: public DefaultOptions<Options>{};
  typedef Handle <Options> SGHandle;
  /*----------------------------------------------------------------*/
  struct DataPack{
    quad<double> data[4];
  };
  /*----------------------------------------------------------------*/
  typedef DataPack *DataPackList;
  /*===================================================================*/
  class DTSWData;
  class SGSWData{
  private:
    typedef SGSWData         SGData;
    typedef vector<SGData *> SGDataList;
    quad<double>      *data;
    DTSWData         *dt_data;
    int          rows,cols,elem_rows_deprec,elem_cols_dep,my_row,my_col;
    SGDataList   parts;
    SGHandle     sg_handle;
    double       val;
    DataPack     *pack_data;
    SpInfo       *sp_info;
    byte         *memory_p;
    int           mem_size_in_bytes,mem_size_in_elements;
    string       name;
  public:
    /*----------------------------------------------------------------*/
    SGSWData(){sp_info = nullptr;memory_p = nullptr;}
    void partition_data(DTSWData &d,int R,int C);
    /*----------------------------------------------------------------*/
    SGSWData(int i, int j){my_row=i;my_col=j;sp_info = nullptr;}
    /*----------------------------------------------------------------*/
    SGHandle&get_sg_handle(){return sg_handle;}
    /*----------------------------------------------------------------*/
    int get_blocks(){return rows;}
    int get_row_blocks(){return rows;}
    int get_col_blocks(){return cols;}
    /*----------------------------------------------------------------*/
    SGSWData &  operator()(int i, int j){
      return *parts[j*rows+i];
    }
    /*----------------------------------------------------------------*/
    SGSWData &  operator()(int i){
      return operator()(i,0);
    }
    /*----------------------------------------------------------------*/
    double &v(int i, int j){
      return data[i].v[j];
    }
    /*----------------------------------------------------------------*/
    double  &x(int i, int j){      
      return pack_data[i].data[0].v[j];
    }
    /*----------------------------------------------------------------*/
    double  &y(int i, int j){
      return pack_data[i].data[1].v[j];
    }
    /*----------------------------------------------------------------*/
    double  &z(int i, int j){
      return pack_data[i].data[2].v[j];
    }
    /*----------------------------------------------------------------*/
    double  &l(int i, int j){
      return pack_data[i].data[3].v[j];
    }
    /*----------------------------------------------------------------*/
    quad<double>  &operator[](int i){
      return data[i];
    }
    /*----------------------------------------------------------------*/
    quad<double> *get_data(){return data;}
    /*----------------------------------------------------------------*/
    SpInfo &get_sp_info(){return *sp_info;}
    void    set_sp_info(SpInfo *sp){sp_info = sp;}
    int     get_row_index(){return my_row;}
    void    report_data();
    string  get_name(){return name;}
    void    set_rows_old(int n){}
    int get_mem_size_in_bytes(){return mem_size_in_bytes;}
    int get_mem_size_in_elems(){return mem_size_in_elements;}
    int get_rows(){return get_mem_size_in_elems();}
    /*----------------------------------------------------------------*/
  };
  /*----------------------------------------------------------------*/
  typedef SGSWData SGData;
  typedef vector<SGData *> SGDataList;
  class TData : public SGSWData {
  public:
  };
  /*----------------------------------------------------------------*/
}
#endif //SG_DATABASE_HPP

