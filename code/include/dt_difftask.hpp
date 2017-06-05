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
      parent = p;
      if ( p)
	p->child_count ++;
      *this << *A << *B >> *C;
      key = DIFF;
      host = C->getHost();
      setName("DiffTask");
    }
    /*------------------------------------------------------------*/
    void runKernel();
    /*------------------------------------------------------------*/
    void dump(){
      std::cout << "Diff " << A->name <<" "
                << B->name <<" "
                << C->name <<" "
                << std::endl;
    }
  };
}
#endif //DT_DIFFTASK_HPP
