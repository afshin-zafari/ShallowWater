#ifndef DT_RHSTASK_HPP
#define DT_RHSTASK_HPP
#include "dt_taskbase.hpp"
namespace dtsw{
  class RHSTask: public SWTask{
  private:
    Data *A,*B,*C;
  public:
    /*---------------------------------------------*/
    void dump(){
      std::cout << "RHS  " << A->name <<" "
                << B->name <<" "
                << C->name <<" "
                << std::endl;
    }
    /*---------------------------------------------*/
    RHSTask(Data &a, Data &b, Data &c,SWTask *p){
      A = static_cast<Data *>(&a);
      B = static_cast<Data *>(&b);
      C = static_cast<Data *>(&c);
      p = parent;
      if(parent)
	parent->child_count++;
      *this << *A << *B >> *C;
      key = RHS;
      host = C->getHost();
      setName("RHSTask");	  
    }
    /*---------------------------------------------*/
    virtual void runKernel();
  };
  
}
#endif //DT_RHSTASK_HPP
