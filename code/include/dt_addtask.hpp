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
      host = C->getHost();
      parent = p;
      child_count = 0 ;
      if ( host == me )
	if ( parent )
	  Atomic::increase(&parent->child_count);
      key = ADD;
      setNameWithParent("_Add");
      *this << *A << *B	>> *C;      
    }
    /*----------------------------------------------*/
    void dump(){
      std::cout << "Add  " << A->getName() <<" "
                << B->getName() <<" "
                << C->getName() <<" "
                << std::endl;
    }
    /*----------------------------------------------*/
    virtual void runKernel();
  };

}
#endif //DT_ADDTASK_HPP
