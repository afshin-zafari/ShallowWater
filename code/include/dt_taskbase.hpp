#ifndef DT_TASKBASE_HPP
#define DT_TASKBASE_HPP

#include <list>
#include "dt_database.hpp"
#include "ductteip.hpp"

namespace dtsw{
  class SWTask: public IDuctteipTask {
    private:
    public:
        virtual void run()=0;
        virtual void dump()=0;
        virtual void runKernel()=0;
      SWTask &operator <<(Data &d1){ // Read data
	DataAccess *daxs = new DataAccess;
	IData *d = d1.getDTData();
	std::cout <<  "<< 1\n";
	daxs->data = d;
	std::cout <<  "<< 1\n";
	daxs->required_version = d->getWriteVersion();
	daxs->required_version.setContext( glbCtx.getLevelString() );
	d->getWriteVersion().setContext( glbCtx.getLevelString() );
	d->getReadVersion() .setContext( glbCtx.getLevelString() );
	daxs->type = IData::READ;
	std::cout <<  "<< 1\n";
	data_list->push_back(daxs);
	std::cout <<  "<< 1\n";
	d->incrementVersion(IData::READ);
	return *this;
	
      }
      SWTask &operator >>(Data &d1){// Write Data 
        DataAccess *daxs = new DataAccess;
	IData *d = d1.getDTData();
        daxs->data = d;
	std::cout <<  ">> 1\n";
        daxs->required_version = d->getReadVersion();
	std::cout <<  ">> 1\n";
        daxs->required_version.setContext( glbCtx.getLevelString() );
	std::cout <<  ">> 1\n";
        d->getWriteVersion().setContext( glbCtx.getLevelString() );
	std::cout <<  ">> 1\n";
        d->getReadVersion() .setContext( glbCtx.getLevelString() );
	std::cout <<  ">> 1\n";
        daxs->type = IData::WRITE;
	std::cout <<  ">> 1\n";
        data_list->push_back(daxs);
	std::cout <<  ">> 1\n";
	d->incrementVersion(IData::WRITE);
	return *this;
      }
      list<DataAccess*> *getDataAccess(){return data_list;}
      SWTask(){
	data_list = new list<DataAccess*>;
      }
    };
}
#endif //DT_TASKBASE_HPP
