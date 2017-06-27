#ifndef DT_DIFFTASK_HPP
#define DT_DIFFTASK_HPP
#include "dt_taskbase.hpp"
namespace dtsw{
  class DiffTask: public SWTask{
  private:
    Data *A,*B,*C;
  public:
    /*------------------------------------------------------------*/
    DiffTask(Data &a, Data &b , Data &c, SWTask *p){
      A = static_cast<Data *>(&a);
      B = static_cast<Data *>(&b);
      C = static_cast<Data *>(&c);
      child_count = 0;
      parent = p;
      host = C->getHost();
      LOG_INFO(LOG_DTSW,"host of Diff task %d\n",getHost());
      if ( getHost() == me ) 
	if (p)
	  Atomic::increase(&parent->child_count);
      *this << *B >> *C;
      key = DIFF;
      setNameWithParent("_Diff");
    }
    /*------------------------------------------------------------*/
    void runKernel();
    /*------------------------------------------------------------*/
    void dump(){
      std::cout << "Diff " << A->getName() <<" "
                << B->getName() <<" "
                << C->getName() <<" "
                << std::endl;
    }
  };
}
#endif //DT_DIFFTASK_HPP
