#include "dtsw.hpp"
namespace dtsw{
  /*---------------------------------------------*/
  void AddTask::run(){
    SGData a(*A),b(*B),c(*C);
    for(int i=0;i<a.rows; i++){
      SGAddTask *t = new SGAddTask(a(i),b(i),dt,c(i));
      sw_engine->subtask(this,t);
    }
  }
  /*---------------------------------------------*/
  void SGAddTask::run(){
    for(int i=0;i<a.element_rows; i++){
      c[i] += a[i] * dt * b[i];
    }
  }
  /*---------------------------------------------*/
  void RHSTask::run(){}
  /*---------------------------------------------*/
  void DiffTask::run(){}
  /*---------------------------------------------*/
  void StepTask::run(){}
  /*---------------------------------------------*/
  void TimeStepsTask::run(){}
  /*---------------------------------------------*/
  /*---------------------------------------------*/
}
