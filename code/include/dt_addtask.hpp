#ifndef DT_ADDTASK_HPP
#define DT_ADDTASK_HPP
#include "dt_taskbase.hpp"
namespace dtsw{
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

}
#endif //DT_ADDTASK_HPP
