#include "dtsw.hpp"
#include "util.hpp"
#define LOG_DTSW_DATA 0
namespace dtsw{
  /*----------------------------------------*/
  Parameters_t Parameters;
  Data *F1,*F2,*F3,*F4;
  Data *H1,*H2,*H3,*H4;
  Data *H,*T,*D;
  AtmArray Atm;
  SWAlgorithm *sw_engine;
  SpInfo *spD;
  double ProfileTime;
  /*----------------------------------------*/
  char * get_path_from_args(int argc, char *argv[]){
    Parameters.IterNo = 2;
    Parameters.pure_mpi = false;
    for ( int i=0;i<argc;i++){
      LOG_INFO(LOG_DTSW,"argv[%d]=%s.\n",i,argv[i]);
      if (strcmp(argv[i],"--data-path")==0){
	LOG_INFO(LOG_DTSW,"Found argv[%d]=%s.\n",i+1,argv[i+1]);
	return (argv[i+1]);
      }
      if (strcmp(argv[i],"--iter-no")==0){
	Parameters.IterNo = atoi(argv[i+1]);
	LOG_INFO(LOG_DTSW,"TimeStep #:%d.\n",Parameters.IterNo);
      }
      if(strcmp(argv[i],"--pure-mpi") ==0){
	Parameters.pure_mpi =true;
	LOG_INFO(LOG_DTSW,"Parameter pure mpi read from cl as :%d.\n",Parameters.pure_mpi);
      }
    }
    LOG_INFO(LOG_DTSW,"Returned null ptr.\n");
    return nullptr;
  }
  /*----------------------------------------*/
  void parse_args(int argc,char *argv[]){
    Parameters.partition_level[0].M              = config.M;
    Parameters.partition_level[0].N              = config.N;
    Parameters.partition_level[0].blocks_per_row = 1;
    Parameters.partition_level[0].blocks_per_col = 1;
    
    Parameters.partition_level[1].M              = config.M / config.Mb;
    Parameters.partition_level[1].N              = config.N / config.Nb;
    Parameters.partition_level[1].blocks_per_row = config.Mb;
    Parameters.partition_level[1].blocks_per_col = config.Nb;

    Parameters.partition_level[2].M              = config.M / config.Mb / config.mb;
    Parameters.partition_level[2].N              = config.N / config.Nb / config.nb;
    Parameters.partition_level[2].blocks_per_row = config.mb;
    Parameters.partition_level[2].blocks_per_col = config.nb;

    Parameters.P = config.P;
    Parameters.p = config.p;
    Parameters.q = config.q;
    
    Parameters.dt  = 600.0;
    Parameters.gh0 = 0.1;
    
  }
  /*----------------------------------------*/
  void prepare_D(){
    int nby  = Parameters.partition_level[1].Mb;
    int nby2 = Parameters.partition_level[2].blocks_per_row;
    spD = new SpInfo;

    SpInfo &sp_info=*spD;
    string fn=Parameters.data_path+string("D");

    LOG_INFO(LOG_DTSW_DATA,"Reading sparse D operator.\n");

    uint64_t  d_n = read_var_D(fn.c_str(),sp_info.index,sp_info.data);
    Parameters.N = d_n;
    int chunk_size_L1 = (d_n / Parameters.partition_level[1].blocks_per_row)+1;

    LOG_INFO(LOG_DTSW_DATA,"Split sp by: %dx%d of %d size each.\n",Parameters.partition_level[1].blocks_per_row,Parameters.partition_level[1].blocks_per_col,chunk_size_L1);

    split(sp_info,Parameters.partition_level[1].blocks_per_row,Parameters.partition_level[1].blocks_per_col,chunk_size_L1);
    int chunk_size_L2 = (chunk_size_L1/Parameters.partition_level[2].blocks_per_row)+1;
    for(uint32_t i=0;i<sp_info.sp_blocks.size();i++){

      LOG_INFO(LOG_DTSW_DATA,"Second Level Split sp by: %dx%d of %d size each.\n",
	       Parameters.partition_level[2].blocks_per_row,
	       Parameters.partition_level[2].blocks_per_col,
	       chunk_size_L2);
      split(*sp_info.sp_blocks[i],
	    Parameters.partition_level[2].blocks_per_row,
	    Parameters.partition_level[2].blocks_per_col,
	    chunk_size_L2);
      
    }
    for(uint32_t i=0;i<sp_info.sp_blocks.size();i++){
      for(uint32_t j=0;j<sp_info.sp_blocks[i]->sp_blocks.size();j++){
	LOG_INFO(LOG_DTSW_DATA,"Assign D %dx%d.\n",sp_info.sp_blocks.size(),sp_info.sp_blocks[i]->sp_blocks.size());	
	SpInfo &sp = *sp_info.sp_blocks[i]->sp_blocks[j];
	LOG_INFO(LOG_DTSW,"D(%d)(%d), rb:%d, cb:%d, n:%d\n",i,j,sp.rb,sp.cb,sp.data.size());
	Data &A = (*D)(i%nby,i/nby);
	int   k = j%nby2;
	int   l = j/nby2;
	
	LOG_INFO(LOG_DTSW_DATA,"Assign D(%d,%d).SG(%d,%d).\n",i%nby,i/nby,k,l);
	
	SGData &a = (*A.sg_data)(k,l);
	assert(sp_info.sp_blocks[i]);
	assert(sp_info.sp_blocks[i]->sp_blocks[j]);
	a.set_sp_info(sp_info.sp_blocks[i]->sp_blocks[j]);
	A.add_nnz(sp.data.size());
      }
    }
    sp_info.index.clear();
    sp_info.data.clear();
  }
  /*----------------------------------------*/
  void prepare_H(){
    int nby  = Parameters.partition_level[1].Mb;
    int nby2 = Parameters.partition_level[2].blocks_per_row;
    int nbx2 = 1;
    string fn = Parameters.data_path+string("H");
    
    LOG_INFO(LOG_DTSW_DATA,"Reading variable H from file %s.\n",fn.c_str());
    
    for(int block_index = 0; block_index < nby; block_index ++){
      byte *mem ;
      
      LOG_INFO(LOG_DTSW_DATA,"Reading variable H(%d) from file %s.\n",block_index,fn.c_str());
      
      int block_size = read_var_H_block(fn.c_str(),mem,nby ,block_index);
      LOG_INFO(LOG_DTSW_DATA,"H(%d) mem :%p block size in bytes= %d, in elems= %d.\n",block_index , mem,block_size, block_size/sizeof(quad<double>));
      (*H)(block_index).set_memory(mem,block_size,block_size/sizeof(quad<double>));
      (*H)(block_index).setHost(block_index % Parameters.P);
      (*H)(block_index).setHostType(IData::SINGLE_HOST);
      LOG_INFO(LOG_DTSW_DATA,"Host for H(%d) is set to %d .\n",block_index,block_index % Parameters.P);
    }
  }
  /*----------------------------------------*/
  void partition_all(){
    int nby  = Parameters.partition_level[1].Mb;
    int nbx  = Parameters.partition_level[1].Nb;
    int nby2 = Parameters.partition_level[2].blocks_per_row;
    for(uint32_t i=0;i<nby ;i++){
      int nbx2 = 1;

      (new SGData )->partition_data(  (*H)(i), nby2,nbx2);
      (new SGData )->partition_data(  (*T)(i), nby2,nbx2);
      (new SGData )->partition_data( (*F1)(i), nby2,nbx2);
      (new SGData )->partition_data( (*F2)(i), nby2,nbx2);
      (new SGData )->partition_data( (*F3)(i), nby2,nbx2);
      (new SGData )->partition_data( (*F4)(i), nby2,nbx2);
      (new SGData )->partition_data( (*H1)(i), nby2,nbx2);
      (new SGData )->partition_data( (*H2)(i), nby2,nbx2);
      (new SGData )->partition_data( (*H3)(i), nby2,nbx2);
      (new SGData )->partition_data( (*H4)(i), nby2,nbx2);

      nbx2 = Parameters.partition_level[2].blocks_per_col;
      for(uint32_t j=0;j<nbx ;j++){
	
	LOG_INFO(LOG_DTSW_DATA,"Partitioning D(%d,%d) into %dx%d sg_data.\n",i,j,nby2,nbx2);

	Data &dt = (*D)(i,j);
	SGData *p = new SGData ;
	p->partition_data(dt, nby2, nbx2);
	dt.sg_data = p;
      }
    }
    LOG_INFO(LOG_DTSW_DATA,"All partitioned.\n");
  }
  /*----------------------------------------*/
  void prepare_atm(){
    int nby  = Parameters.partition_level[1].Mb;
    int nby2 = Parameters.partition_level[2].blocks_per_row;
    string fn = Parameters.data_path+string("atm");    
    uint64_t size=read_var_Atm(fn.c_str(),Atm);
    LOG_INFO(LOG_DTSW_DATA,"Reading Atm from file %s, atm length=%ld.\n",fn.c_str(),size);
    Parameters.atm_length = size;
    Parameters.atm_block_size_L1 = size/nby;
    Parameters.atm_block_size_L2 = size/nby/nby2;
  }
  /*----------------------------------------*/
  void init(int argc, char *argv[]){
    const bool isSparse = true;
    const bool notAllocate = true;
    Parameters.data_path = new char[200];
    strcpy(Parameters.data_path,get_path_from_args(argc,argv));
    //strcpy(Parameters.data_path,"./data/galew-6400-31-ep2.7-o4-gc-0.05/");
    string fn = Parameters.data_path + string("params");
    FILE *f = fopen(fn.c_str(),"rb");
    uint64_t dummy;
    fread(&dummy,sizeof(uint64_t),1,f);
    fread(&Parameters.gh0,sizeof(double),1,f);
    fclose(f);
    LOG_INFO(LOG_DTSW_DATA,"Parameter gh0 is read as:%lf.\n",Parameters.gh0);

    dtEngine.set_memory_policy(engine::ALL_USER_ALLOCATED);
    dtEngine.start(argc,argv);
    LOG_INFO(LOG_DTSW,"SWAlgorithm constructed with pure-mpi=%d,num-thrd:%d.\n",Parameters.pure_mpi,config.getNumThreads());
    sw_engine = new SWAlgorithm(config.getNumThreads(),Parameters.pure_mpi);
    
    LOG_INFO(LOG_DTSW,"After DuctTeip Init.\n");
    parse_args(argc,argv);

    int nby  = Parameters.partition_level[1].blocks_per_row;
    int nby2 = Parameters.partition_level[2].blocks_per_row;
    int nbx = Parameters.partition_level[1].Nb;
    int M = Parameters.partition_level[0].M;
    int N = Parameters.partition_level[0].N;
    LOG_INFO(LOG_DTSW,"Partitions of vectors:%d\n",nbx);
    LOG_INFO(LOG_DTSW,"Defining DT Data .\n");
    int quad_double_size = sizeof(quad<double>) ,
      quad_vec4_size = sizeof(quad<quad<double>>);
    M = ( (M/nby+1)/nby2+1)*nby*nby2;
    Parameters.partition_level[0].M=M;

    H  = new Data(M,1,nbx, 1  ,"H" , quad_double_size * M  , quad_double_size );
    D  = new Data(M,1,nby, nbx,"D" , quad_double_size * M  , quad_double_size ,isSparse   );
    T  = new Data(M,1,nbx, 1  ,"T" , quad_vec4_size   * M  , quad_vec4_size   );
    F1 = new Data(M,1,nbx, 1  ,"F1", quad_double_size * M  , quad_double_size );
    F2 = new Data(M,1,nbx, 1  ,"F2", quad_double_size * M  , quad_double_size );
    F3 = new Data(M,1,nbx, 1  ,"F3", quad_double_size * M  , quad_double_size );
    F4 = new Data(M,1,nbx, 1  ,"F4", quad_double_size * M  , quad_double_size );

    H1 = new Data(M,1,nbx, 1  ,"H1", quad_double_size * M  , quad_double_size );
    H2 = new Data(M,1,nbx, 1  ,"H2", quad_double_size * M  , quad_double_size );
    H3 = new Data(M,1,nbx, 1  ,"H3", quad_double_size * M  , quad_double_size );
    H4 = new Data(M,1,nbx, 1  ,"H4", quad_double_size * M  , quad_double_size );

    LOG_INFO(LOG_DTSW_DATA,"Partitioning variables for 2nd level.\n");
    partition_all();
    
    prepare_D();
    prepare_H();
    prepare_atm();

    if (LOG_DTSW_DATA){
      H->report_data();
      T->report_data();
      D->report_data();
      F1->report_data();
      F2->report_data();
      F3->report_data();
      F4->report_data();

      H1->report_data();
      H2->report_data();
      H3->report_data();
      H4->report_data();
    }

  }
  /*----------------------------------------*/
  void finalize(){
    sw_engine->finalize();
    ProfileTime = UserTime() - ProfileTime ;
    printf("P:%d, p:%d, q:%d, N:%d, B:%d, b:%d, S:%d, M:%d, T:",
	   Parameters.P,Parameters.p,Parameters.q,
	   Parameters.N,Parameters.partition_level[1].blocks_per_row,Parameters.partition_level[2].blocks_per_row,Parameters.IterNo,Parameters.pure_mpi);
    if ( !Parameters.pure_mpi ){
      stringstream fn;
      fn << "execution_" << Parameters.P << "_" << Parameters.partition_level[0].M <<"_B"
	 <<Parameters.partition_level[1].blocks_per_row << "_b"
	 <<Parameters.partition_level[2].blocks_per_row << "_"
	 << me << ".log";
      Trace<Options>::dump(fn.str().c_str());
    }
    delete D;
    delete T;
    delete H;
    delete F1;
    delete F2;
    delete F3;
    delete F4;

    delete H1;
    delete H2;
    delete H3;
    delete H4;
    delete sw_engine;
  }
  /*----------------------------------------*/
  void mult(Data *t,Data *d, Data *h, SWTask *p){
    Data &_D(*d),&_H(*h), &_T(*t);    
    for(int i=0; i< _D.get_rows(); i++){
      for(int j=0; j< _D.get_cols(); j++)
	{
	  if ( _D(i,j).get_nnz() ==0) 
	    continue;
	  DiffTask *Diff = new DiffTask(_D(i,j),_H(j),_T(i),p);
	  sw_engine->submit(Diff);
	  LOG_INFO(LOG_DTSW,"Diff task (%d,%d) for host:%d submitted. Children#:%d, Parent: %s children#:%d\n",
		   i,j,Diff->getHost(),(int)Diff->child_count,Diff->parent->getName().c_str(),(int)Diff->parent->child_count);
	  LOG_INFO(LOG_DTSW,"(****)%s uses  %s its req version is write:%s read:%s .\n",Diff->getName().c_str(),_H(j).getName().c_str(),
		   _H(j).getWriteVersion().dumpString().c_str(),
		   _H(j).getReadVersion().dumpString().c_str() );
	}
    }
  }
  /*----------------------------------------*/
  void rhs(Data *dH , Data *t,Data *h, SWTask *p){
    Data &DH(*dH),&_H(*h), &_T(*t);
    int nb = Parameters.partition_level[1].blocks_per_row;
    for(int i=0; i< nb; i++){
      RHSTask *RHS= new RHSTask(_T(i),_H(i),DH(i),p);
      sw_engine->submit(RHS);
      LOG_INFO(LOG_DTSW,"RHS task (%d) submitted, children#:%d.\n",i,(int)RHS->child_count);
      LOG_INFO(LOG_DTSW,"(****)%s uses %s its req version is write:%s read:%s .\n",RHS->getName().c_str(),_H(i).getName().c_str(),
	       _H(i).getWriteVersion().dumpString().c_str(),
	       _H(i).getReadVersion().dumpString().c_str() );

    }
  }
  /*----------------------------------------*/
  void f(Data *dH,Data *h, SWTask * p){
    mult( T, D, h, p);
    rhs (dH, T, h , p);
  }
  /*----------------------------------------*/
  void add (Data *a1,Data *a2, double dt, Data *a4, SWTask *p){
    Data &Y(*a1),&_H(*a2),&X(*a4);
    int nb = Parameters.partition_level[1].blocks_per_row;
    for(int i=0; i< nb; i++){
      AddTask *Add = new AddTask(_H(i),X(i),dt,Y(i),p);
      sw_engine->submit(Add);
      LOG_INFO(LOG_DTSW,"(****)%s uses %s its req version is write:%s read:%s .\n",Add->getName().c_str(),_H(i).getName().c_str(),
	       _H(i).getWriteVersion().dumpString().c_str(),
	       _H(i).getReadVersion().dumpString().c_str() );
    }
  }
  /*----------------------------------------------------*/
  void step(Data *h, Data *f1, Data *f2 , Data *f3 , Data *f4, SWTask *p){
    Data &_H(*h),&_F1(*f1),&_F2(*f2),&_F3(*f3),&_F4(*f4);
    int nb = Parameters.partition_level[1].blocks_per_row;
    for(int i=0; i< nb; i++){
      StepTask * Step=new StepTask(_F1(i),_F2(i),_F3(i),_F4(i),_H(i),p);
      sw_engine->submit(Step);
      LOG_INFO(LOG_DTSW,"(****)%s uses  %s its req version is write:%s read:%s .\n",Step->getName().c_str(),_H(i).getName().c_str(),
	       _H(i).getWriteVersion().dumpString().c_str(),
	       _H(i).getReadVersion().dumpString().c_str() );
    }
  }
  /*----------------------------------------------------*/
  void runStep(SWTask*);
  IterationData *TimeStepsTask::D = nullptr;
  int TimeStepsTask::last_step=0;
  /*----------------------------------------------------*/
  void run(int argc, char *argv[]){
    TimeStepsTask::D = new IterationData();
    int n = Parameters.IterNo;//( Parameters.IterNo<2)?Parameters.IterNo:2;
    for(int i=0; i < n; i++){
      TimeStepsTask *step = new TimeStepsTask;
      sw_engine->submit(step);
      //runStep(step);
    }
    sw_engine->flush();
  }
  /*----------------------------------------------------*/
  void TimeStepsTask::finished(){
    SWTask::finished();
    
    LOG_INFO(LOG_DTSW,"step :%d, Par.StepNo :%d\n",last_step, Parameters.IterNo);
    if ( last_step  < Parameters.IterNo ) {        
      sw_engine->submit(new TimeStepsTask );
      sw_engine->flush();
    }
    LOG_INFO(LOG_DTSW,"(****)TimeStepTask::fin H(0) version is  write:%s read:%s .\n",
	     (*H)(0).getWriteVersion().dumpString().c_str(),
	     (*H)(0).getReadVersion().dumpString().c_str() );
    LOG_INFO(LOG_DTSW,"(****)TimeStePDATA gt-ver: rd %s, wr %s --- rt-ver: rd %s wr %s\n",
	     TimeStepsTask::D->getReadVersion().dumpString().c_str(),
	     TimeStepsTask::D->getWriteVersion().dumpString().c_str() ,
	     TimeStepsTask::D->getRunTimeVersion(IData::READ).dumpString().c_str(),
	     TimeStepsTask::D->getRunTimeVersion(IData::WRITE).dumpString().c_str());
  }
  /*----------------------------------------------------*/
  void TimeStepsTask::runKernel(){
    is_submitting = true;
    runStep(this);
    is_submitting = false;
    sw_engine->flush();
    LOG_INFO(LOG_DTSW,"DT Tasks count:%d.\n",sw_engine->get_tasks_count());
    LOG_INFO(LOG_DTSW,"(****)TimeStepTask::fin H(0) version is  write:%s read:%s .\n",
	     (*H)(0).getWriteVersion().dumpString().c_str(),
	     (*H)(0).getReadVersion().dumpString().c_str() );
    LOG_INFO(LOG_DTSW,"(****)TimeStePDATA gt-ver: rd %s, wr %s --- rt-ver: rd %s wr %s\n",
	     TimeStepsTask::D->getReadVersion().dumpString().c_str(),
	     TimeStepsTask::D->getWriteVersion().dumpString().c_str() ,
	     TimeStepsTask::D->getRunTimeVersion(IData::READ).dumpString().c_str(),
	     TimeStepsTask::D->getRunTimeVersion(IData::WRITE).dumpString().c_str());
    
    if (sw_engine->get_tasks_count()<=2)// Only time step tasks are added.
      finished();
    
  }
  /*----------------------------------------------------*/
  void runStep(SWTask *p){
    double dt = Parameters.dt;
    LOG_INFO(LOG_DTSW,"f(F1,H) is called.\n");
    f(F1,H, p);                      // F1 = f(H)
    add(H1, H , 0.5*dt, F1 , p);     // H1 = H + 0.5*dt*F1
    f(F2,H1,p);                      // F2 = f(H1)
    add(H2, H , 0.5*dt, F2 , p);     // H2 = H + 0.5*dt*F2
    f(F3,H2,p);                      // F3 = f(H2)
    add(H3, H ,     dt, F3 , p);     // H3 = H +     dt*F3
    f(F4,H3,p);                      // F4 = f(H3)
    step(H,F1,F2,F3,F4,p);          // H = H + dt/6*(F1+2*F2+2*F3+F4)
    return;
  }
  /*----------------------------------------------------------------*/
  TimeStepsTask::~TimeStepsTask(){
    LOG_INFO(LOG_DTSW,"step :%d, Par.StepNo :%d\n",last_step, Parameters.IterNo);
    if ( last_step  < Parameters.IterNo )         
      sw_engine->submit(new TimeStepsTask );
  }
  /*----------------------------------------------------------------*/
  void TimeStepsTask::register_data(){
    parent_context = sw_engine;
    setName("TStep");
    TimeStepsTask::D->setName("TimeStepsData");
    if ( last_step ==1)
      TimeStepsTask::D->setRunTimeVersion("0.0",0);      
    IDuctteipTask::key = key;
    DataAccessList *dlist = new DataAccessList;    
    //    data_access(dlist,TimeStepsTask::D,(last_step==1)?IData::READ:IData::WRITE);
    setDataAccessList(dlist);
    LOG_INFO(LOG_DTSW,"(****)TimeStePDATA gt-ver: rd %s, wr %s --- rt-ver: rd %s wr %s\n",
	     TimeStepsTask::D->getReadVersion().dumpString().c_str(),
	     TimeStepsTask::D->getWriteVersion().dumpString().c_str() ,
	     TimeStepsTask::D->getRunTimeVersion(IData::READ).dumpString().c_str(),
	     TimeStepsTask::D->getRunTimeVersion(IData::WRITE).dumpString().c_str());
    LOG_INFO(LOG_DTSW,"(****) H(0) version is  write:%s read:%s .\n",
	     (*H)(0).getWriteVersion().dumpString().c_str(),
	     (*H)(0).getReadVersion().dumpString().c_str() );
    host = me;
  }
  /*----------------------------------------------------------------*/
  DTSWData::DTSWData(){      
    memory_type = USER_ALLOCATED;
    host_type=SINGLE_HOST;
    IData::parent_data = NULL;
    setDataHandle( sw_engine->createDataHandle(this));
    setDataHostPolicy( glbCtx.getDataHostPolicy() ) ;
    setLocalNumBlocks(1,1);
    IData::Mb = 0;
    IData::Nb = 0;
    setHostType(SINGLE_HOST);
    setParent(sw_engine);
    sw_engine->addInputData(this);
    LOG_INFO(LOG_DTSW_DATA,"Data handle for new dtswdata:%d\n",my_data_handle->data_handle);
    setRunTimeVersion("0.0",0);
  }
  /*----------------------------------------------------------------*/
  IterationData::IterationData(){      
    memory_type = USER_ALLOCATED;
    host_type=ALL_HOST;
    IData::parent_data = NULL;
    setDataHandle( sw_engine->createDataHandle(this));
    setDataHostPolicy( glbCtx.getDataHostPolicy() ) ;
    setLocalNumBlocks(1,1);
    IData::Mb = 0;
    IData::Nb = 0;
    setHostType(ALL_HOST);
    setParent(sw_engine);
    sw_engine->addInputData(this);
    setRunTimeVersion("0.0",0);
  }
  /*----------------------------------------------------------------*/
  void SGSWData::partition_data(DTSWData &d,int R,int C){
    rows = R; cols = C;
    int bz = d.get_mem_size_in_bytes();
    int block_size_in_bytes = (bz / R / C );
    dt_data = static_cast<Data *>(&d);
    d.sg_data = this;
    my_row=my_col=-1;
    memory_p = nullptr;
    parts.clear();
    name.assign("_"+d.getName());
    LOG_INFO(LOG_DTSW_DATA,"Blk-size in bytes for '%s' is computed as: %d using L2-mem-sz:%d /R/C .\n",name.c_str(),block_size_in_bytes, d.get_mem_size_in_bytes());
    LOG_INFO(LOG_DTSW_DATA,"New SGData will be made from parent memory at:%p , R:%d,C:%d, Blk_size_in_bytes:%d, Szof(quad):%d\n",d.get_memory(), R,C, block_size_in_bytes, sizeof(quad<double>) ); 

    for ( int i=0;i<rows;i++){
      for ( int j=0;j<cols;j++){
	SGData *sgd = new SGData (i,j);
	parts.push_back(sgd);
	sgd->memory_p = d.get_memory() + (rows * j + i) * block_size_in_bytes;
	sgd->mem_size_in_bytes    = block_size_in_bytes;
	sgd->mem_size_in_elements = block_size_in_bytes / d.get_item_size();
	sgd->     data = (quad<double>*) sgd->memory_p;
	sgd->pack_data = (DataPack*    ) sgd->memory_p;
	LOG_INFO(LOG_DTSW_DATA,"Resulting memory for sg(%d,%d) is :%p with size_in_elems of :%d\n",i,j,sgd->memory_p,sgd->mem_size_in_elements);
	sgd->name.assign(d.getName() + "_");
      }
    }
  }
  /*----------------------------------------------------------------------------*/
  DTSWData::DTSWData (int M, int N, int r,int c, std::string n,int total_size_in_bytes, int item_size_, bool isSparse)
    {
      nnz=0;
      setName(n);
      item_size = item_size_;
    for(int j=0;j<c;j++){
      for(int i=0;i<r;i++){
	DTSWData*t=new DTSWData;
	t->row_idx = i;
	t->col_idx = j;
	t->sp_row  = i;
	t->sp_col  = j;
	std::stringstream ss;
	if ( c>1)
	  ss << n << "(" << i << "," << j << ")";
	else
	  ss << n << "(" << i <<  ")";
	t->name.assign(ss.str());
	t->item_size = item_size_;
	t->memory_p = nullptr;
	if(!isSparse){
	  int partition_size = total_size_in_bytes / r/ c ;
	  t->mem_size_in_bytes = partition_size;
	  t->memory_p = new byte[partition_size];
	  t->mem_size_in_elements = partition_size / item_size_;
	  LOG_INFO(LOG_DTSW_DATA,"Host for %s is set to %d modu %d   = %d .\n",ss.str().c_str(),i,Parameters.P,i%Parameters.P);
	  LOG_INFO(LOG_DTSW_DATA,"Memory :%p L2 mem  size(in bytes):%d (in items count):%d.\n",t->memory_p,t->mem_size_in_bytes, t->mem_size_in_elements);
	  t->setHost(i%Parameters.P);	    
	  t->setHostType(SINGLE_HOST);
	  t->allocateMemory();
	}
	else{// for sparse data D 
	  t->setHostType(ALL_HOST);
	  t->setHost(-1);	    
	}
	t->sg_data =nullptr;
	Dlist.push_back(t);
      }
    }
    rows = r;
    cols = c;
    row_idx=col_idx = -1;
    sg_data = nullptr;
    memory_p = nullptr;
  }
  /*---------------------------------------------*/
  RHSTask::RHSTask(Data &a, Data &b, Data &c,SWTask *p){
    A = static_cast<Data *>(&a);
    B = static_cast<Data *>(&b);
    C = static_cast<Data *>(&c);
    child_count = 0;
    parent = p;
    if (p)
      step_no = p->step_no;
    host = C->getHost();
    atm_offset = a.get_block_row() * Parameters.atm_block_size_L1;
    key = RHS;
    setNameWithParent("_RHS");	  
    *this << *A << *B >> *C;
    if(getHost() == me ) 
      if(parent)
	Atomic::increase(&parent->child_count);
  }
  /*---------------------------------------------*/
  void DTSWData::report_data(){
    LOG_INFO(LOG_DTSW_DATA,"Data %s at (%d,%d) has memory at address:%p size(in-bytes):%d, size in elems:%d\n",
	     name.c_str(),row_idx,col_idx,memory_p, mem_size_in_bytes, mem_size_in_elements );
    for(auto d:Dlist){
      d->report_data();
    }
    if (sg_data)
      sg_data->report_data();
  }
  /*---------------------------------------------*/
  void SGSWData ::report_data(){
    if ( name.size() ){
      LOG_INFO(LOG_DTSW_DATA,"SGData %s at (%d,%d) has memory :%p sz in bytes:%d, sz in elems:%d, and SParse Info ptr:%p.\n",
	       name.c_str(),my_row,my_col,memory_p,mem_size_in_bytes, mem_size_in_elements,sp_info);
    }
    else{
      LOG_INFO(LOG_DTSW_DATA,"SGData ?? at (%d,%d) has memory :%p, and SParse Info ptr:%p.\n",my_row,my_col,memory_p,sp_info);
    }
    if (sp_info)
      sp_info->report_data();
    for(auto sd: parts){
      sd->report_data();
    }
  }
}

