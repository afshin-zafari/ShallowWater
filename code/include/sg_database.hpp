#ifndef SG_DATABASE_HPP
#define SG_DATABASE_HPP
namespace dtsw{
    class SGSWData{
    private:
        double *data;
    public:
        SGSWData(Data &d){}
        SGSWData(){}
        SGSWData & operator()(int i, int j){
            return *this;
        }
        SGSWData &operator()(int i){
            return this->operator()(i,0);
        }
        double &operator[](int i){
            return data[i];
        }

        int rows,element_rows;;
    };
    typedef SGSWData SGData;
  
}
#endif //SG_DATABASE_HPP
