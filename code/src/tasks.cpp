#include "dtsw.hpp"
namespace dtsw{
  /*---------------------------------------------*/
  void AddTask::runKernel(){

    int nb=3;
    SGData a(*A,nb),b(*B,nb),c(*C,nb);
    
    for(int i=0;i<a.get_blocks(); i++){
      SGAddTask *t = new SGAddTask(a(i),b(i),dt,c(i));
      sw_engine->subtask(this,t);
    }
    
  }
  /*---------------------------------------------*/
  void SGAddTask::run(){
    SGData &A = *a;
    SGData &B = *b;
    SGData &C = *c;
    return;
    for(int i=0;i<A.get_rows(); i++){
      C[i] += A[i] * dt * B[i];
    }
  }
  /*---------------------------------------------*/
  void RHSTask::runKernel(){
    int nb=3;
    SGData a(*A,nb),b(*B,nb),c(*C,nb);
    
    for(int i=0;i<a.get_blocks(); i++){
      SGRHSTask *t = new SGRHSTask(a(i),b(i),c(i));
      sw_engine->subtask(this,t);
    }
  }
  /*---------------------------------------------*/
  void SGRHSTask::run(){
  }
  /*---------------------------------------------*/
  void DiffTask::runKernel(){
    int nb=3;
    SGData a(*A,nb),b(*B,nb),c(*C,nb);
    
    for(int i=0;i<a.get_blocks(); i++){
      SGDiffTask *t = new SGDiffTask(a(i),b(i),c(i));
      sw_engine->subtask(this,t);
    }
  }
  /*---------------------------------------------*/
  void SGDiffTask::run(){}
  /*---------------------------------------------*/
  void StepTask::runKernel(){
    int nb=3;
    SGData a(*A,nb),b(*B,nb),c(*C,nb),d(*D,nb),e(*E,nb);
    
    for(int i=0;i<a.get_blocks(); i++){
      SGStepTask *t = new SGStepTask(a(i),b(i),c(i),d(i),e(i));
      sw_engine->subtask(this,t);
    }
  }
  /*---------------------------------------------*/
  void SGStepTask::run(){}
  /*---------------------------------------------*/
  /*---------------------------------------------*/
  /*---------------------------------------------*/
}
