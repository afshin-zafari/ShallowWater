#ifndef SG_TASKBASE_HPP
#define SG_TASKBASE_HPP
#include <sstream>
#include "dt_taskbase.hpp" 
#include "sg/superglue.hpp"
#include "sg/platform/atomic.hpp"
#include "sg_database.hpp"
namespace dtsw{
  /*=========================================================*/
  class SGTask : public Task<Options>{
  private:
    SWTask *parent;
    int step_no;
  protected:
    string name;    
  public:
    SWTask* get_parent(){return parent;}
    string get_name(){return name;}
    void set_step_no(int sno){
      step_no = sno;
      std::ostringstream ss;
      ss << name << '_' << sno;
      name=ss.str();
      LOG_INFO(LOG_DTSW,"SGTask name for step: %d, %s.\n",step_no,name.c_str());
      //      name += string(std::itoa(sno));
    }
    virtual void run()=0;
    /*---------------------------------------------------------*/
    void set_parent(SWTask *p){
      parent = p;
      if ( p)
	sg::Atomic::increase(&parent->child_count);	
    }
    /*---------------------------------------------------------*/
    ~SGTask(){
      if (parent){
	/*
	auto t = getTime();
	while ( parent->is_still_submitting() ) {};
	auto d = getTime() - t;
	fprintf(stdout,"time to wait for parent finish submission: %d\n",d);
	*/
	
	if ( sg::Atomic::decrease_nv(&parent->child_count) ==0 )
	  parent->finished();
      }	
    }

  };
  /*=========================================================*/
  class SGAddTask: public SGTask{
  private:
    SGData *a,*b,*c;
    double dt;
    
  public:

      /*---------------------------------------------------------*/
    SGAddTask(SGData &a_, SGData &b_, double dt_ , SGData &c_){
      a = &a_;
      b = &b_;
      c = &c_;
      dt = dt_;
      register_access(ReadWriteAdd::read ,a->get_sg_handle());
      register_access(ReadWriteAdd::read ,b->get_sg_handle());
      register_access(ReadWriteAdd::write,c->get_sg_handle());
      name.assign("SGAdd");
    }
    void run();
  };
  /*---------------------------------------------------------*/
  /*=========================================================*/
  class SGRHSTask: public SGTask{
  private:
    SGData *t,*h,*dh;
    int atm_offset;
    
  public:

    /*---------------------------------------------------------*/
    SGRHSTask(int atm_,SGData &t_, SGData &h_,  SGData &dh_){
      atm_offset = atm_;
      t = &t_;
      h = &h_;
      dh = &dh_;
      register_access(ReadWriteAdd::read , t->get_sg_handle());
      register_access(ReadWriteAdd::read , h->get_sg_handle());
      register_access(ReadWriteAdd::write,dh->get_sg_handle());
      name.assign("SGRHS");
    }
    void run();
  };
  /*=========================================================*/
  class SGDiffTask: public SGTask{
  private:
    SGData *a,*b,*c;
    
  public:

    /*---------------------------------------------------------*/
    SGDiffTask(SGData &a_, SGData &b_,  SGData &c_){
      a = &a_;
      b = &b_;
      c = &c_;
      register_access(ReadWriteAdd::read ,a->get_sg_handle());
      register_access(ReadWriteAdd::read ,b->get_sg_handle());
      register_access(ReadWriteAdd::write,c->get_sg_handle());
      name.assign("Diff");
    }
    void run();
  };
  /*=========================================================*/
  class SGStepTask: public SGTask{
  private:
    SGData *a,*b,*c,*d,*e;
    
  public:

    /*---------------------------------------------------------*/
    SGStepTask(SGData &a_, SGData &b_,  SGData &c_, SGData &d_, SGData &e_){
      a = &a_;
      b = &b_;
      c = &c_;
      d = &d_;
      e = &e_;
      register_access(ReadWriteAdd::read ,a->get_sg_handle());
      register_access(ReadWriteAdd::read ,b->get_sg_handle());
      register_access(ReadWriteAdd::read ,c->get_sg_handle());
      register_access(ReadWriteAdd::read ,d->get_sg_handle());
      register_access(ReadWriteAdd::write,e->get_sg_handle());
      name.assign("Step");
    }
    void run();
  };
  
}
#endif //SG_TASKBASE_HPP
