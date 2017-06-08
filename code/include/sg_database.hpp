#ifndef SG_DATABASE_HPP
#define SG_DATABASE_HPP
#include "util.hpp"
namespace dtsw{
  /*----------------------------------------------------------------*/
  struct Options: public DefaultOptions<Options>{};
  typedef Handle <Options> SGHandle;
  /*----------------------------------------------------------------*/
  struct DataPack{
    double x[3],y[3],z[3],l[3],v[3];
  };
  /*----------------------------------------------------------------*/
  typedef vector<DataPack> DataPackList;
  /*===================================================================*/
  class DTSWData;
  class SGSWData{
  private:
    typedef SGSWData         SGData;
    typedef vector<SGData *> SGDataList;
    quad<double>      *data;
    DTSWData         *dt_data;
    int          rows,cols,elem_rows,elem_cols,my_row,my_col;
    SGDataList   parts;
    SGHandle     sg_handle;
    double       val;
    DataPackList pack_data;
    SpInfo       sp_info;
  public:
    /*----------------------------------------------------------------*/
    void partition_data(DTSWData &d,int R);
    /*----------------------------------------------------------------*/
    SGSWData(int i, int j){my_row=i;my_col=j;}
    /*----------------------------------------------------------------*/
    SGHandle&get_sg_handle(){return sg_handle;}
    /*----------------------------------------------------------------*/
    int get_rows(){return elem_rows;}
    /*----------------------------------------------------------------*/
    int get_blocks(){return rows;}
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
      return pack_data[i].v[j];
    }
    /*----------------------------------------------------------------*/
    double  &x(int i, int j){      
      return pack_data[i].x[j];
    }
    /*----------------------------------------------------------------*/
    double  &y(int i, int j){
      return pack_data[i].y[j];
    }
    /*----------------------------------------------------------------*/
    double  &z(int i, int j){
      return pack_data[i].z[j];
    }
    /*----------------------------------------------------------------*/
    double  &l(int i, int j){
      return pack_data[i].l[j];
    }
    /*----------------------------------------------------------------*/
    quad<double>  &operator[](int i){
      return data[i];
    }
    /*----------------------------------------------------------------*/
    quad<double> *get_data(){return data;}
    /*----------------------------------------------------------------*/
    SpInfo &get_sp_info(){return sp_info;}
    /*----------------------------------------------------------------*/
  };
  /*----------------------------------------------------------------*/
  typedef SGSWData SGData;
  typedef vector<SGData *> SGDataList;  
  /*----------------------------------------------------------------*/
}
#endif //SG_DATABASE_HPP

