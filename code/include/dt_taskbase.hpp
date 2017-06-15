#ifndef DT_TASKBASE_HPP
#define DT_TASKBASE_HPP

#include <list>
#include <atomic>
#include "dt_database.hpp"
#include "ductteip.hpp"

namespace dtsw{
  class SWTask: public IDuctteipTask {
  private:
  public:
    SWTask *parent;
    std::atomic<size_t> child_count;
    virtual void dump()=0;
    virtual void runKernel()=0;
    /*------------------------------------------------------------*/
    SWTask &operator <<(Data &d1){ // Read data
      DataAccess *daxs = new DataAccess;
      IData *d = &d1;
      daxs->data = d;
      daxs->required_version = d->getWriteVersion();
      daxs->required_version.setContext( glbCtx.getLevelString() );
      d->getWriteVersion().setContext( glbCtx.getLevelString() );
      d->getReadVersion() .setContext( glbCtx.getLevelString() );
      daxs->type = IData::READ;
      data_list->push_back(daxs);
      d->incrementVersion(IData::READ);
      return *this;
	
    }
    /*------------------------------------------------------------*/
    SWTask &operator >>(Data &d1){// Write Data 
      DataAccess *daxs = new DataAccess;
      IData *d = &d1;
      daxs->data = d;
      daxs->required_version = d->getReadVersion();
      daxs->required_version.setContext( glbCtx.getLevelString() );
      d->getWriteVersion().setContext( glbCtx.getLevelString() );
      d->getReadVersion() .setContext( glbCtx.getLevelString() );
      daxs->type = IData::WRITE;
      data_list->push_back(daxs);
      d->incrementVersion(IData::WRITE);
      return *this;
    }
    //list<DataAccess*> *getDataAccess(){return data_list;}
    /*------------------------------------------------------------*/
    SWTask(){
      data_list = new list<DataAccess*>;
      child_count = 0;
      parent = nullptr;
    }
    /*------------------------------------------------------------*/
    virtual void finished(){
      setFinished(true);
      if (parent){
	LOG_INFO(LOG_DTSW, "parent's task :%s child_count :%d\n " ,parent->getName().c_str(),(int)parent->child_count );
	if ( --parent->child_count ==0)
	  parent->finished();
	
      }
	
    }
  };
}
#endif //DT_TASKBASE_HPP
