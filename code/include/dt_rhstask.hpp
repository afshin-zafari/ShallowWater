#ifndef DT_RHSTASK_HPP
#define DT_RHSTASK_HPP
#include "dt_taskbase.hpp"
namespace dtsw{
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
  
}
#endif //DT_RHSTASK_HPP
