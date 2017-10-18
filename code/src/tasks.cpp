#include "dtsw.hpp"
const int Z=1;
namespace dtsw{
  /*---------------------------------------------*/
  void AddTask::runKernel(){
    SGData &a = *A->sg_data;
    SGData &b = *B->sg_data;
    SGData &c = *C->sg_data;
    LOG_INFO(LOG_DTSW,"Add task :%s kernel called\n",getName().c_str());
    is_submitting = true;
    list<SGAddTask *> tlist;
    for(int i=0;i<a.get_blocks(); i++){
      SGAddTask *t = new SGAddTask(a(i),b(i),dt,c(i));
      tlist.push_back(t);
      t->set_parent(this);
      t->set_step_no(this->step_no);
    }
    is_submitting = false;
    for (auto t: tlist)
      sw_engine->submit(t);
    if ( Parameters.pure_mpi)
      finished();
  }
  /*---------------------------------------------*/
  void SGAddTask::run(){
    SGData &A = *a;
    SGData &B = *b;
    SGData &C = *c;
    if ( get_parent()){
      LOG_INFO(LOG_DTSW,"SG Add task for parent:%s kernel called\n",get_parent()->getName().c_str());
    }
    if ( !A.get_data())
      return;
    if ( !B.get_data())
      return;
    if ( !C.get_data())
      return;
    LOG_INFO(LOG_DTSW,"@Add Task work: %d \n",A.get_rows());
    for(int  j=0;j<Z; j++)
    for(int i=0;i<A.get_rows(); i++){
      assert(i<A.get_mem_size_in_elems());
      assert(i<C.get_rows());
      assert(i<B.get_rows());
      for(int j=0;j<4;j++){
	C[i].v[j] += A[i].v[j] * dt * B[i].v[j];
      }
    }
  }
  /*---------------------------------------------*/
  void RHSTask::runKernel(){
    SGData &a = *A->sg_data;
    SGData &b = *B->sg_data;
    SGData &c = *C->sg_data;
    LOG_INFO(LOG_DTSW,"RHS kernel called.\n");
    list<SGRHSTask*>tlist;
    is_submitting = true;
    for(int i=0;i<a.get_blocks(); i++){
      SGRHSTask *t = new SGRHSTask(atm_offset,a(i),b(i),c(i));
      LOG_INFO(LOG_DTSW,"SG RHS(%d) submitted.\n",i);
      t->set_parent(this);
      t->set_step_no(this->step_no);
      tlist.push_back(t);
    }
    is_submitting = false;
    for(auto t:tlist)
      sw_engine->submit(t);
    if ( Parameters.pure_mpi)
      finished();
  }
  /*---------------------------------------------*/
  void SGRHSTask::run(){

    SGData &H(*h);
    SGData &T(*t);
    SGData &F(*dh);
    LOG_INFO(LOG_DTSW,"SG RHS Kernel called.\n");
    
    
    double d = H.v(0,0) +  H.x(0,0);
    const double gh0 = Parameters.gh0;
    LOG_INFO(LOG_DTSW,"@RHS task work, H rows: %ld .\n",H.get_rows());
    
    for(int  j=0;j<Z; j++)
    for (uint32_t i = 0; i < H.get_rows() ; ++i) {
      if (H.get_row_index()*Parameters.atm_block_size_L2 + atm_offset+ i>=Parameters.atm_length){
	LOG_INFO(LOG_DTSW,"@RHS Task skipped, since %d,%d,%d,%d,%d,  len=%ld.\n",H.get_row_index(),Parameters.atm_block_size_L2 , atm_offset, i,
		 H.get_row_index()*Parameters.atm_block_size_L2 + atm_offset+ i,Parameters.atm_length);
	continue;
      }
      const atmdata &a(Atm[H.get_row_index()*Parameters.atm_block_size_L2 + atm_offset+ i]);
      //    LOG_INFO(LOG_DTSW,"@RHS task work, H rows: %ld .\n",H.get_rows());
      int Tz = T.get_mem_size_in_elems();
      int Hz = H.get_mem_size_in_elems();
      int Fz = F.get_mem_size_in_elems();
      assert(i<Tz);
      assert(i<Fz);
      assert(i<Hz);
      //      LOG_INFO(LOG_DTSW,"index :%d , sizes of %s :%d, %s:%d, %s:%d\n",i,T.get_name().c_str(),Tz,F.get_name().c_str(),Fz,H.get_name().c_str(),Hz);
      /*
      const double p = -(    H.v(i,0) * T.x(i,0)
			   + H.v(i,1) * T.y(i,0)
			   + H.v(i,2) * T.z(i,0)
			   + a.f * (a.y * H.v(i,2) - a.z * H.v(i,1)) + T.x(i,3));
      */
      const double p = -(    H.data[i].v[0] * T.pack_data[i].data[0].v[0]
			   + H.data[i].v[1] * T.pack_data[i].data[1].v[0]
			   + H.data[i].v[2] * T.pack_data[i].data[2].v[0]
			   + a.f * (a.y * H.data[i].v[2] - a.z * H.data[i].v[1]) + T.pack_data[i].data[0].v[3]);
      
      /*
      const double q = -(  H.v(i,0) * T.x(i,1)
			   + H.v(i,1) * T.y(i,1)
			   + H.v(i,2) * T.z(i,1)
			   + a.f * (a.z * H.v(i,0) - a.x * H.v(i,2)) + T.y(i,3));
      */
      const double q = -(    H.data[i].v[0] * T.pack_data[i].data[0].v[1]			     
			   + H.data[i].v[1] * T.pack_data[i].data[1].v[1]
			   + H.data[i].v[2] * T.pack_data[i].data[2].v[1]
			   + a.f * (a.z * H.data[i].v[0] - a.x * H.data[i].v[2]) + T.pack_data[i].data[1].v[3]);
      /*
      const double s = -(  H.v(i,0) * T.x(i,2)
			   + H.v(i,1) * T.y(i,2)
			   + H.v(i,2) * T.z(i,2)
			   + a.f * (a.x * H.v(i,1) - a.y * H.v(i,0)) + T.z(i,3));
      */
      const double s = -(    H.data[i].v[0] * T.pack_data[i].data[0].v[2]
			   + H.data[i].v[1] * T.pack_data[i].data[1].v[2]
			   + H.data[i].v[2] * T.pack_data[i].data[2].v[2]
			   + a.f * (a.x * H.data[i].v[1] - a.y * H.data[i].v[0]) + T.pack_data[i].data[2].v[3]);

      /*
      F.v(i,0) = a.p_u[0]*p + a.p_u[1]*q + a.p_u[2]*s + T.l(i,0);
      F.v(i,1) = a.p_v[0]*p + a.p_v[1]*q + a.p_v[2]*s + T.l(i,1);
      F.v(i,2) = a.p_w[0]*p + a.p_w[1]*q + a.p_w[2]*s + T.l(i,2);
      */

      F.data[i].v[0] = a.p_u[0]*p + a.p_u[1]*q + a.p_u[2]*s + T.pack_data[i].data[3].v[0];
      F.data[i].v[1] = a.p_v[0]*p + a.p_v[1]*q + a.p_v[2]*s + T.pack_data[i].data[3].v[1];
      F.data[i].v[2] = a.p_w[0]*p + a.p_w[1]*q + a.p_w[2]*s + T.pack_data[i].data[3].v[2];
      /*
      F.v(i,3) = -(  H.v(i,0) * (T.x(i,3) - a.gradghm[0])
		   + H.v(i,1) * (T.y(i,3) - a.gradghm[1])
		   + H.v(i,2) * (T.z(i,3) - a.gradghm[2])
		   + (H.v(i,3)+gh0-a.ghm) * (T.x(i,0) + T.y(i,1) + T.z(i,2)))
	+ T.l(i,3);
      */
      F.data[i].v[3] = -(    H.data[i].v[0]  * (T.pack_data[i].data[0].v[3] - a.gradghm[0])
			  +  H.data[i].v[1]  * (T.pack_data[i].data[1].v[3] - a.gradghm[1])
			  +  H.data[i].v[2]  * (T.pack_data[i].data[2].v[3] - a.gradghm[2])
			  + (H.data[i].v[3]+gh0-a.ghm) * (T.pack_data[i].data[0].v[0] + T.pack_data[i].data[1].v[1] + T.pack_data[i].data[2].v[2]) )
	+ T.pack_data[i].data[3].v[3];

    }
  }
  /*---------------------------------------------*/
  void DiffTask::runKernel(){

    SGData &a = *A->sg_data;
    SGData &b = *B->sg_data;
    SGData &c = *C->sg_data;
    LOG_INFO(LOG_DTSW,"Diff task Kernel called. B is %s,GT-ver: rd:%s, wrt:%s, RT-ver: rd:%s, wrt:%s \n",
	     B->getName().c_str(),
	     B->getReadVersion().dumpString().c_str(),
	     B->getWriteVersion().dumpString().c_str(),
	     B->getRunTimeVersion(IData::READ).dumpString().c_str(),
	     B->getRunTimeVersion(IData::WRITE).dumpString().c_str()	     
	     );
    is_submitting = true;
    list <SGDiffTask*> tlist;
    int task_count =0;
    for(int i=0;i<a.get_row_blocks(); i++){
      for(int j=0;j<a.get_col_blocks(); j++){
	if ( a(i,j).get_sp_info().data.size() ==0 ){
	  LOG_INFO(LOG_DTSW,"NO SG Diff submit for block(%d,%d) from %d blocks.\n",i,j,a.get_blocks());
	  continue;
	}
	task_count ++;
	LOG_INFO(LOG_DTSW,"Row:%d, Col:%d , b.rows:%d, c.rows:%d.\n",i,j,b.get_row_blocks(), c.get_row_blocks());
	assert(j < b.get_row_blocks());
	assert(i < c.get_row_blocks());
	SGDiffTask *t = new SGDiffTask(a(i,j),b(j),c(i));
	t->set_parent(this);
	t->set_step_no(this->step_no);
	tlist.push_back(t);
	/*
	SpInfo &sp=a.get_sp_info();
	  for(int k=0;k<sp.data.size();k+=640){
	    auto t=sp.data[k];
	    auto l=sp.index[k];
	  }
	*/
	LOG_INFO(LOG_DTSW,"SG Diff task(%d) is submitted, parent's children#:%d.\n",i,(int)t->get_parent()->child_count);
      }
    }
    is_submitting = false;
    for(auto t:tlist)
      sw_engine->submit(t);
    if ( task_count ==0)
      finished();
    if (Parameters.pure_mpi)
      finished();

  }
  /*---------------------------------------------*/
  void SGDiffTask::run(){
    SGData &A(*a),&B(*b),&C(*c);
    LOG_INFO(LOG_DTSW,"@SG Diff for %s task Kernel called with D.size:%d.\n",A.get_name().c_str(),A.get_sp_info().data.size());
    LOG_INFO(LOG_DTSW,"A=%s, B=%s, C=%s \n",A.get_name().c_str(),B.get_name().c_str(),C.get_name().c_str());

    for(int  j=0;j<Z; j++)
    for ( uint32_t i=0;i<A.get_sp_info().data.size(); i++){
      if(1)
      {
      int r =  A.get_sp_info().index[i].first ;
      int col =  A.get_sp_info().index[i].second ;
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
	assert(r<C.get_mem_size_in_elems());
	assert(col<B.get_mem_size_in_elems());
	for(int j=0;j<4;j++){
	  double d =  A.get_sp_info().data[i].v[j] ;
	  d *=   B.data[col].v[j];
	  C.pack_data[r].data[0].v[j] += d;
	  C.pack_data[r].data[1].v[j] += d;
	  C.pack_data[r].data[2].v[j] += d;
	  C.pack_data[r].data[3].v[j] += d;
		  
	  /*
	  C.x(r,j) += d;
	  C.y(r,j) += d;
	  C.z(r,j) += d;
	  C.l(r,j) += d;
	  */
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
    is_submitting = true;
    list<SGStepTask*> tlist;
    for(int i=0;i<a.get_blocks(); i++){
      SGStepTask *t = new SGStepTask(a(i),b(i),c(i),d(i),e(i));
      t->set_parent(this);
      t->set_step_no(this->step_no);
      tlist.push_back(t);
    }
    is_submitting = false;
    for(auto t:tlist)
      sw_engine->submit(t);
    if ( Parameters.pure_mpi)
      finished();
  }
  /*---------------------------------------------*/
  void SGStepTask::run(){
    SGData &H(*e),&F1(*a),&F2(*b),&F3(*c),&F4(*d);
    double s = 1.0* (TimeStepsTask::last_step-1) * Parameters.dt /6.0;
    if (!H.get_data() ) return;
    LOG_INFO(LOG_DTSW,"@Step task work, H rows: %ld .\n",H.get_rows());

    for(int  j=0;j<Z; j++)
    for(int i=0;i<H.get_rows();i++){
      assert(i < F1.get_mem_size_in_elems() );
      assert(i < F2.get_mem_size_in_elems() );
      assert(i < F3.get_mem_size_in_elems() );
      assert(i < F4.get_mem_size_in_elems() );
      for(int j=0;j<4;j++){
	H[i].v[j] += s *( F1[i].v[j] + 2.0*(F2[i].v[j]+F3[i].v[j])+F4[i].v[j]);
      }
    }
  }
  /*---------------------------------------------*/
}
