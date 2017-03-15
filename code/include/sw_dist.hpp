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
#include "dt_timestepstask.hpp"
#include "ductteip.hpp"

namespace dtsw{
  /*---------------------------------------------------------*/
  class SWAlgorithm: public IContext{
  private:
    std::vector<SWTask*> tasks;
  public:
    SWAlgorithm():IContext("ShallowWater"){}
    void submit(SWTask *t){
      tasks.push_back(t);
      //IDuctteipTask *task = new IDuctteipTask(this,t->name,t->key,t->host,t->getDataAccess());
      dtEngine.register_task(t);
      //      IDuctteipTask *tt=dtEngine.getTask(th);
      //      tt->run();
    }
    void subtask( SWTask *, SGTask *){}
    void dump_tasks(){
      for(auto t: tasks){
	t->dump();
      }
    }
    virtual void runKernels(IDuctteipTask *task )override {
      std::cout << "runKernels for " << task->get_name() << std::endl;
    }
    string getTaskName(unsigned long key){}
    void  taskFinished(DuctTeip_Task *task, TimeUnit dur){}
  };
  extern SWAlgorithm *sw_engine;
  /*----------------------------------------------*/
}
#endif // SW_DIST_HPP
