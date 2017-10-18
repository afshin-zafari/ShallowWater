#ifndef DT_STEPTASK_HPP
#define DT_STEPTASK_HPP
#include "dt_taskbase.hpp"
namespace dtsw{
  class StepTask: public SWTask{
  private:
    Data *A,*B,*C,*D,*E;
  public:
    /*------------------------------------------------------------*/
    void dump(){
      std::cout << "Step " << A->getName() <<" "
                << B->getName() <<" "
                << C->getName() <<" "
                << D->getName() <<" "
                << E->getName() <<" "
		<< std::endl;
    }
    /*------------------------------------------------------------*/
    StepTask(Data &a, Data &b , Data &c, Data &d, Data &e,SWTask *p){
      A = static_cast<Data *>(&a);
      B = static_cast<Data *>(&b);
      C = static_cast<Data *>(&c);
      D = static_cast<Data *>(&d);
      E = static_cast<Data *>(&e);
      parent = p;
      if(p)
	step_no = p->step_no;
      child_count = 0;
      host = E->getHost();
      if ( host == me ) 
	if (parent)
	  Atomic::increase(&parent->child_count);
      key = STEP;
      setNameWithParent("_Step");
      *this <<*A <<*B << *C <<*D >> *E;
    }
    /*------------------------------------------------------------*/
    virtual void runKernel();
  };
  
}
#endif //DT_STEPTASK_HPP
