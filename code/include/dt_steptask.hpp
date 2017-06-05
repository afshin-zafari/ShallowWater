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
      std::cout << "Step " << A->name <<" "
                << B->name <<" "
                << C->name <<" "
                << D->name <<" "
                << E->name <<" "
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
      if (parent)
	parent->child_count ++;
      key = STEP;
      *this <<*A <<*B << *C <<*D >> *E;
      host = E->getHost();
      setName("StepTask");
    }
    /*------------------------------------------------------------*/
    virtual void runKernel();
  };
  
}
#endif //DT_STEPTASK_HPP
