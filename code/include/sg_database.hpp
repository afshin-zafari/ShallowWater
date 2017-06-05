#ifndef SG_DATABASE_HPP
#define SG_DATABASE_HPP
namespace dtsw{
  struct Options: public DefaultOptions<Options>{};
  typedef Handle <Options> SGHandle;
  /*----------------------------------------------------------------*/
  class SGSWData{
  private:
    typedef SGSWData         SGData;
    typedef vector<SGData *> SGDataList;
    double     *data;
    Data       *dt_data;
    int         rows,cols,elem_rows,elem_cols;
    SGDataList  parts;
    SGHandle    sg_handle;
  public:
    /*----------------------------------------------------------------*/
    SGSWData(Data &d,int R){
      rows = R; cols = 1;      
      dt_data =static_cast<Data *>(&d);
      for ( int i=0;i<rows;i++){
	for ( int j=0;j<cols;j++){
	  SGData *sgd = new SGData ();
	  parts.push_back(sgd);
	}
      }
    }
    /*----------------------------------------------------------------*/
    SGSWData(){}
    /*----------------------------------------------------------------*/
    SGHandle&get_sg_handle(){return sg_handle;}
    /*----------------------------------------------------------------*/
    int get_rows(){return elem_rows;}
    /*----------------------------------------------------------------*/
    int get_blocks(){return rows;}
    /*----------------------------------------------------------------*/
    SGSWData & operator()(int i, int j){
      return *parts[j*rows+i];
    }
    /*----------------------------------------------------------------*/
    SGSWData &operator()(int i){
      return this->operator()(i,0);
    }
    /*----------------------------------------------------------------*/
    double &operator[](int i){
      return data[i];
    }
    /*----------------------------------------------------------------*/
  };
  /*----------------------------------------------------------------*/
  typedef SGSWData SGData;
  typedef vector<SGData *> SGDataList;  
  /*----------------------------------------------------------------*/
}
#endif //SG_DATABASE_HPP
