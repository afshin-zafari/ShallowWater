#ifndef DT_TIMESTEPSTASK_HPP
#define DT_TIMESTEPSTASK_HPP
#include "dt_taskbase.hpp"
namespace dtsw{
  class TimeStepsTask : public SWTask{
  private:
    int time_step;
    Data *A,*B;
  public:
    TimeStepsTask(Data &d,int i):A(&d),B(nullptr),time_step(i){
      *this >> *A;
      this->key = TIMESTEPS;
      this->host = A->getHost();
      name.assign("TimeSteps");
    }
    TimeStepsTask(Data &d1, Data &d2,int i):A(&d1),B(&d2),time_step(i){
      std::cout << "timestepDep " << std::endl;
      *this <<  *A	>> *B ;
      this->key = TIMESTEPS;
      this->host = B->getHost();
      name.assign("TimeStepsDep");
    }
    void run();
    virtual void runKernel(){
      std::cout <<"runKernel of " << name << std::endl;
      setFinished(true);
    }
    void dump(){
      std::cout << "TimeStep  "<< time_step << " " 
		<< A->name <<" ";
      if (B)
	std::cout << B->name <<" ";
      
    std:cout << std::endl;
    }
  };
}
#endif
