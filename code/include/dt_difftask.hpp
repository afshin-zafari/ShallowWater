#ifndef DT_DIFFTASK_HPP
#define DT_DIFFTASK_HPP
#include "dt_taskbase.hpp"
namespace dtsw{
    class DiffTask: public SWTask{
    private:
        Data *A,*B,*C;
    public:
        DiffTask(Data &a, Data &b , Data &c):A(&a),B(&b),C(&c){
	  *this << *A << *B >> *C;
	  key = DIFF;
	  host = C->getHost();
	}
        void run();
        void dump(){
            std::cout << "Diff " << A->name <<" "
                << B->name <<" "
                << C->name <<" "
                << std::endl;
        }
      virtual void runKernel(){}
    };
}
#endif //DT_DIFFTASK_HPP
