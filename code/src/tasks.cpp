#include "dtsw.hpp"
namespace dtsw{
  /*---------------------------------------------*/
  void AddTask::runKernel(){
    SGData &a = *A->sg_data;
    SGData &b = *B->sg_data;
    SGData &c = *C->sg_data;
    LOG_INFO(LOG_DTSW,"Add task :%s kernel called\n",getName().c_str());
    
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
    LOG_INFO(LOG_DTSW,"SG Add task for parent:%s kernel called\n",get_parent()->getName().c_str());
    if ( !A.get_data())
      return;
    if ( !B.get_data())
      return;
    if ( !C.get_data())
      return;
    for(int i=0;i<A.get_rows(); i++){
      C[i] += A[i] * dt * B[i];
    }
  }
  /*---------------------------------------------*/
  void RHSTask::runKernel(){
    SGData &a = *A->sg_data;
    SGData &b = *B->sg_data;
    SGData &c = *C->sg_data;
    LOG_INFO(LOG_DTSW,"RHS kernel called.\n");
    
    
    for(int i=0;i<a.get_blocks(); i++){
      SGRHSTask *t = new SGRHSTask(atm_offset,a(i),b(i),c(i));
      LOG_INFO(LOG_DTSW,"SG RHS(%d) submitted.\n",i);
      sw_engine->subtask(this,t);
    }
  }
  /*---------------------------------------------*/
  void SGRHSTask::run(){

    SGData &H(*a);
    SGData &T(*b);
    SGData &F(*c);
    LOG_INFO(LOG_DTSW,"SG RHS Kernel called.\n");
    
    
    double d = H.v(0,0) +  H.x(0,0);
    const double gh0 = Parameters.gh0;
    for (uint32_t i = 0; i < H.get_rows() ; ++i) {
      const atmdata &a(Atm[H.get_row_index()*Parameters.atm_block_size_L2 + atm_offset+ i]);

      const double p = -(    H.v(i,0) * T.x(i,0)
			   + H.v(i,1) * T.y(i,0)
			   + H.v(i,2) * T.z(i,0)
			   + a.f * (a.y * H.v(i,2) - a.z * H.v(i,1)) + T.x(i,3));
      const double q = -(  H.v(i,0) * T.x(i,1)
			   + H.v(i,1) * T.y(i,1)
			   + H.v(i,2) * T.z(i,1)
			   + a.f * (a.z * H.v(i,0) - a.x * H.v(i,2)) + T.y(i,3));
      const double s = -(  H.v(i,0) * T.x(i,2)
			   + H.v(i,1) * T.y(i,2)
			   + H.v(i,2) * T.z(i,2)
			   + a.f * (a.x * H.v(i,1) - a.y * H.v(i,0)) + T.z(i,3));

      F.v(i,0) = a.p_u[0]*p + a.p_u[1]*q + a.p_u[2]*s + T.l(i,0);
      F.v(i,1) = a.p_v[0]*p + a.p_v[1]*q + a.p_v[2]*s + T.l(i,1);
      F.v(i,2) = a.p_w[0]*p + a.p_w[1]*q + a.p_w[2]*s + T.l(i,2);

      F.v(i,3) = -(  H.v(i,0) * (T.x(i,3) - a.gradghm[0])
		   + H.v(i,1) * (T.y(i,3) - a.gradghm[1])
		   + H.v(i,2) * (T.z(i,3) - a.gradghm[2])
		   + (H.v(i,3)+gh0-a.ghm) * (T.x(i,0) + T.y(i,1) + T.z(i,2)))
	+ T.l(i,3);
    }
  }
  /*---------------------------------------------*/
  void DiffTask::runKernel(){

    SGData &a = *A->sg_data;
    SGData &b = *B->sg_data;
    SGData &c = *C->sg_data;
    LOG_INFO(LOG_DTSW,"Diff task Kernel called.\n");

    
    for(int i=0;i<a.get_blocks(); i++){
      SGDiffTask *t = new SGDiffTask(a(i),b(i),c(i));
      sw_engine->subtask(this,t);
      LOG_INFO(LOG_DTSW,"SG Diff task(%d) is submitted, parent's children#:%d.\n",i,(int)t->get_parent()->child_count);
    }
  }
  /*---------------------------------------------*/
  void SGDiffTask::run(){
    SGData &A(*a),&B(*b),&C(*c);
    LOG_INFO(LOG_DTSW,"SG Diff for %s task Kernel called.\n",A.get_name().c_str());

    for ( uint32_t i=0;i<A.get_sp_info().data.size(); i++){
      int r =  A.get_sp_info().index[i].first ;
      int col =  A.get_sp_info().index[i].second ;
      //      if(1)
      {
	/*
	LOG_INFO(LOG_DTSW,"C size :%d B. size : %d. \n",C.get_mem_size_in_elems(),B.get_mem_size_in_elems());
	LOG_INFO(LOG_DTSW,"C:%p (%d) = D(%d) * B:%p (%d)\n",C.get_data(),r,i,B.get_data(), col);
	double temp = B[col].v[0];
	LOG_INFO(LOG_DTSW,"%lf\n",temp);
	temp = A.get_sp_info().data[i].v[0];
	LOG_INFO(LOG_DTSW,"%lf\n",temp);
	temp = C.x(r,0);
	LOG_INFO(LOG_DTSW,"%lf\n",temp);
	*/
	for(int j=0;j<4;j++){
	  C.x(r,j) +=  A.get_sp_info().data[i].v[j] * B[col].v[j];
	  C.y(r,j) +=  A.get_sp_info().data[i].v[j] * B[col].v[j];
	  C.z(r,j) +=  A.get_sp_info().data[i].v[j] * B[col].v[j];
	  C.l(r,j) +=  A.get_sp_info().data[i].v[j] * B[col].v[j];
	}
      }
    }    
  }
  /*---------------------------------------------*/
  void StepTask::runKernel(){

    SGData &a = *A->sg_data;
    SGData &b = *B->sg_data;
    SGData &c = *C->sg_data;
    SGData &d = *D->sg_data;
    SGData &e = *E->sg_data;
    
    for(int i=0;i<a.get_blocks(); i++){
      SGStepTask *t = new SGStepTask(a(i),b(i),c(i),d(i),e(i));
      sw_engine->subtask(this,t);
    }
  }
  /*---------------------------------------------*/
  void SGStepTask::run(){
    SGData &H(*e),&F1(*a),&F2(*b),&F3(*c),&F4(*d);
    double s = 1.0* (TimeStepsTask::last_step-1) * Parameters.dt /6.0;
    if (!H.get_data() ) return;
    for(int i=0;i<H.get_rows();i++){
      H[i] += s *( F1[i] + 2.0*(F2[i]+F3[i])+F4[i]); 
    }
  }
  /*---------------------------------------------*/
  /*---------------------------------------------*/
  /*---------------------------------------------*/
}
