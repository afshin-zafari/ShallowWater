#ifndef DT_TASKBASE_HPP
#define DT_TASKBASE_HPP

#include <list>
#include <atomic>
#include "dt_database.hpp"
#include "ductteip.hpp"
#include "sg/platform/atomic.hpp"

namespace dtsw{
  class SWTask: public IDuctteipTask {
  private:
  public:
    SWTask *parent;
    //std::atomic<size_t> child_count;
    int child_count;
    bool is_submitting;
    virtual void dump()=0;
    virtual void runKernel()=0;
    virtual ~SWTask(){}
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
      LOG_INFO(LOG_DTSW,"(****)Daxs Read %s for  %s is %p\n",
	       getName().c_str(), d1.getName().c_str(),daxs);
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
      LOG_INFO(LOG_DTSW,"(****)Daxs Write %s for  %s is %p\n",
	       getName().c_str(),d1.getName().c_str(),daxs);
      return *this;
    }
    //list<DataAccess*> *getDataAccess(){return data_list;}
    /*------------------------------------------------------------*/
    SWTask(){
      data_list = new list<DataAccess*>;
      LOG_INFO(LOG_DTSW,"(****)Daxs dlist new %p\n",data_list);
      child_count = 0;
      parent = nullptr;
      is_submitting = false;
    }
    /*------------------------------------------------------------*/
    virtual void finished(){
      if ( state >= Finished  ) return;
      setFinished(true);
      if (parent){
	while ( parent->is_still_submitting() ){};
	LOG_INFO(LOG_DTSW, "%s finished from parent's task :%s child_count :%d\n " ,getName().c_str(),parent->getName().c_str(),(int)parent->child_count );
	if ( Atomic::decrease_nv(&parent->child_count) ==0)
	  parent->finished();	
      }	
    }
    /*------------------------------------------------------------*/
    bool is_still_submitting(){return is_submitting;}
    /*------------------------------------------------------------*/
    void setNameWithParent(const char*n){
      if ( !parent ){
	setName(n);
	return;
      }
      std::stringstream ss;
      ss << parent->getName() << "_" << parent->child_count << n ;
      setName(ss.str());
    }

  };
}
#endif //DT_TASKBASE_HPP
