#ifndef SW_DIST_HPP
#define SW_DIST_HPP
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <iostream>
#include "dt_addtask.hpp"
#include "sg_taskbase.hpp"
#include "dt_rhstask.hpp"
#include "dt_steptask.hpp"
#include "dt_difftask.hpp"
#include "ductteip.hpp"

namespace dtsw{
  /*---------------------------------------------------------*/
  class SWAlgorithm: public Algorithm{
  private:
    std::vector<SWTask*> tasks;
  public:
    SWAlgorithm(){}
    void submit(SWTask *t){
      tasks.push_back(t);
    }
    void subtask( SWTask *, SGTask *){}
    void dump_tasks(){
      for(auto t: tasks){
	t->dump();
      }
    }
    void runKernels(DuctTeip_Task *task ){}
    string getTaskName(unsigned long key){}
    void  taskFinished(DuctTeip_Task *task, TimeUnit dur){}
  };
  extern SWAlgorithm *sw_engine;
  /*----------------------------------------------*/
}
#endif // SW_DIST_HPP
