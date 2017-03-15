#ifndef DT_ADDTASK_HPP
#define DT_ADDTASK_HPP
#include "dt_taskbase.hpp"
namespace dtsw{
  /*----------------------------------------------*/
  class AddTask: public SWTask{
    dtsw::Data *A,*B,*C;
    double dt;
  public:
    AddTask(dtsw::Data &a,dtsw::Data &b, double d, dtsw::Data &c):A(&a),B(&b),C(&c),dt(d){
      *this << *A
	    << *B
	    >> *C;
      key = ADD;
    }
    void run();
    void dump(){
      std::cout << "Add  " << A->name <<" "
                << B->name <<" "
                << C->name <<" "
                << std::endl;
    }
    virtual void runKernel()override {}
  };

}
#endif //DT_ADDTASK_HPP
