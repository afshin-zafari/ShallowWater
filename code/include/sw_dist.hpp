#ifndef SW_DIST_HPP
#define SW_DIST_HPP
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <iostream>
namespace dtsw{
    /*----------------------------------------------*/
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
    /*----------------------------------------------*/
    class SGTask{
    public:
        virtual void run()=0;
    };
    class SGAddTask: public SGTask{
    private:
        SGData a,b,c;
        double dt;
    public:

        SGAddTask(SGData &, SGData &, double , SGData &){}
        void run();
    };
    class SWTask{
    private:

    public:
        virtual void run()=0;
        virtual void dump()=0;
    };
    /*----------------------------------------------*/
    class AddTask: public SWTask{
        Data A,B,C;
        double dt;
    public:
        AddTask(Data &a,Data&b, double d, Data &c):A(a),B(b),C(c),dt(d){}
        void run();
        void dump(){
            std::cout << "Add  " << A.name <<" "
                << B.name <<" "
                << C.name <<" "
                << std::endl;
        }
    };
    /*----------------------------------------------*/
    class RHSTask: public SWTask{
    private:
        Data A,B,C;
    public:
        void run();
        void dump(){
            std::cout << "RHS  " << A.name <<" "
                << B.name <<" "
                << C.name <<" "
                << std::endl;
        }
        RHSTask(Data &a, Data &b, Data &c):A(a),B(b),C(c){}
    };
    /*----------------------------------------------*/
    class MultTask: public SWTask{
    private :
        Data A,B,C;
    public:
        MultTask(Data &c,Data &a, Data &b):A(a),B(b),C(c){}
        void run(){}
        void dump(){
            std::cout << "Mult" << A.name <<" "
                << B.name <<" "
                << C.name <<" "
                << std::endl;
        }
    };
    /*----------------------------------------------*/
    class StepTask: public SWTask{
    private:
        Data A,B,C,D,E;
    public:
        void run();
        void dump(){
            std::cout << "Step " << A.name <<" "
                << B.name <<" "
                << C.name <<" "
                << D.name <<" "
                << E.name <<" " << std::endl;
        }
        StepTask(Data &a, Data &b , Data &c, Data &d, Data &e):A(a),B(b),C(c),D(d),E(e){}
    };
    /*---------------------------------------------------------*/
    class DiffTask: public SWTask{
    private:
        Data A,B,C;
    public:
        DiffTask(Data &a, Data &b , Data &c):A(a),B(b),C(c){}
        void run();
        void dump(){
            std::cout << "Diff " << A.name <<" "
                << B.name <<" "
                << C.name <<" "
                << std::endl;
        }
    };
    /*---------------------------------------------------------*/
    class SWAlgorithm{
    private:
        std::vector<SWTask*> tasks;
    public:
        SWAlgorithm(){}
        void submit(SWTask *t){
            tasks.push_back(t);
        }
        void subtask( SWTask *, SGTask *){}
        void dump_tasks(){
            for(auto t: tasks){
                t->dump();
            }
        }
    };
    extern SWAlgorithm *sw_engine;
    /*----------------------------------------------*/
    /*----------------------------------------------*/


}
#endif // SW_DIST_HPP
