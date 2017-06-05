#ifndef DT_ADDTASK_HPP
#define DT_ADDTASK_HPP
#include "dt_taskbase.hpp"
namespace dtsw{
  /*----------------------------------------------*/
  class AddTask: public SWTask{
    dtsw::Data *A,*B,*C;
    double dt;
  public:
    /*----------------------------------------------*/
    AddTask(dtsw::Data &a,dtsw::Data &b, double d, dtsw::Data &c,SWTask *p){
      A = static_cast<Data *>(&a);
      B = static_cast<Data *>(&b);
      C = static_cast<Data *>(&c);
      dt=d;
      parent = p;
      if ( parent )
	parent->child_count ++;
      *this << *A << *B	>> *C;
      key = ADD;
      host = C->getHost();
      setName("AddTask");      
    }
    /*----------------------------------------------*/
    void dump(){
      std::cout << "Add  " << A->name <<" "
                << B->name <<" "
                << C->name <<" "
                << std::endl;
    }
    /*----------------------------------------------*/
    virtual void runKernel();
  };

}
#endif //DT_ADDTASK_HPP
