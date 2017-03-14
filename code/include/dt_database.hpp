#ifndef DT_DATABASE_HPP
#define DT_DATABASE_HPP
namespace dtsw{
    class DTSWData {
    private:
        std::vector <DTSWData *> Dlist;
        int rows,cols,row_idx,col_idx;
    public:
        DTSWData(){}
        DTSWData (int r,int c, std::string n):name(n){
            for(int j=0;j<c;j++){
                for(int i=0;i<r;i++){
                    DTSWData*t=new DTSWData;
                    t->row_idx = i;
                    t->col_idx = j;
                    t->sp_row  = i;
                    t->sp_col  = j;
                    std::stringstream ss;
                    if ( c>1)
                        ss << n << "(" << i << "," << j << ")";
                    else
                        ss << n << "(" << i <<  ")";
                    t->name.assign(ss.str());
                    Dlist.push_back(t);
                }
            }
            rows = r;
            cols = c;
        }
        DTSWData &operator()(int i, int j){
            return *Dlist[j*rows + i];
        }
        DTSWData &operator()(int i){
            return this->operator()(i,0);
        }
        int size(){return rows*cols;}
        int sp_row,sp_col;
        std::string name;
    };
    typedef DTSWData Data;
  
}
#endif //DT_DATABASE_HPP
