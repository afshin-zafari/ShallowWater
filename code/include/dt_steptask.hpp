#ifndef DT_STEPTASK_HPP
#define DT_STEPTASK_HPP
#include "dt_taskbase.hpp"
namespace dtsw{
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
  
}
#endif //DT_STEPTASK_HPP
