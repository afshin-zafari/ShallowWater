#include "dtsw.hpp"
#include "util.hpp"
namespace dtsw{
  /*----------------------------------------*/
  Parameters_t Parameters;
  Data *H,*T,*D;
  Data *F1,*F2,*F3,*F4;
  Data *H1,*H2,*H3,*H4;
  AtmArray Atm;
  SWAlgorithm *sw_engine;
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
    
    Parameters.dt  = 0.5;
    Parameters.gh0 = 0.1;//todo values of dt and gh0

    Parameters.data_path = new char[200];
    strcpy(Parameters.data_path,"./data/galew-6400-31-ep2.7-o4-gc-0.05/");
    
  }
  /*----------------------------------------*/
  void init(int argc, char *argv[]){
    // import problem setup
    const bool isSparse = true;
    const bool notAllocate = true;
    dtEngine.start(argc,argv);
    sw_engine = new SWAlgorithm;
    printf("After DuctTeip Init.\n");
    parse_args(argc,argv);
    int nby = Parameters.partition_level[1].Mb;
    int nbx = Parameters.partition_level[1].Nb;
    int M = Parameters.partition_level[0].M;
    int N = Parameters.partition_level[0].N;
    printf("Defining DT Data .\n");
    H  = new Data(M,1,nby, 1  ,"H" , notAllocate );
    T  = new Data(M,1,nby, 1  ,"T" );
    D  = new Data(M,1,nby, nbx,"D" , isSparse    );
    F1 = new Data(M,1,nby, 1  ,"F1");
    F2 = new Data(M,1,nby, 1  ,"F2");
    F3 = new Data(M,1,nby, 1  ,"F3");
    F4 = new Data(M,1,nby, 1  ,"F4");

    H1 = new Data(M,1,nby, 1  ,"H1");
    H2 = new Data(M,1,nby, 1  ,"H2");
    H3 = new Data(M,1,nby, 1  ,"H3");
    H4 = new Data(M,1,nby, 1  ,"H4");

    SpInfo sp_info;
    string fn=Parameters.data_path+string("D");
    printf("Reading sparse D operator.\n");
    uint64_t  d_n = read_var_D(fn.c_str(),sp_info.index,sp_info.data);
    int chunk_size_L1 = d_n / Parameters.partition_level[1].blocks_per_row;
    printf("Split sp by: %dx%d of %d size each.\n",Parameters.partition_level[1].blocks_per_row,Parameters.partition_level[1].blocks_per_col,chunk_size_L1);
    split(sp_info,Parameters.partition_level[1].blocks_per_row,Parameters.partition_level[1].blocks_per_col,chunk_size_L1);
    int chunk_size_L2 = chunk_size_L1/Parameters.partition_level[2].blocks_per_row;
    for(uint32_t i=0;i<sp_info.sp_blocks.size();i++){
      printf("Second Level Split sp by: %dx%d of %d size each.\n",
	     Parameters.partition_level[2].blocks_per_row,
	     Parameters.partition_level[2].blocks_per_col,
	     chunk_size_L2);
      split(*sp_info.sp_blocks[i],
	    Parameters.partition_level[2].blocks_per_row,
	    Parameters.partition_level[2].blocks_per_col,
	    chunk_size_L2);
      
    }
    for(uint32_t i=0;i<nby ;i++){
      int nby2 = Parameters.partition_level[2].rows_per_block;
      int nbx2 = Parameters.partition_level[2].cols_per_block;

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
           
      for(uint32_t j=0;j<nbx ;j++){
	printf("Partitioning D(%d,%d) into %dx%d sg_data.\n",i,j, Parameters.partition_level[2].rows_per_block,Parameters.partition_level[2].cols_per_block);
	Data &dt = (*D)(i,j);
	SGData *p = new SGData ;
	p->partition_data ( dt, Parameters.partition_level[2].rows_per_block,
			        Parameters.partition_level[2].cols_per_block);
	dt.sg_data = p;
      }
    }
    for(uint32_t i=0;i<sp_info.sp_blocks.size();i++){
      for(uint32_t j=0;j<sp_info.sp_blocks[i]->sp_blocks.size();j++){
	printf("Assign D %dx%d.\n",sp_info.sp_blocks.size(),sp_info.sp_blocks[i]->sp_blocks.size());
	SpInfo &sp = *sp_info.sp_blocks[i]->sp_blocks[j];
	int R = D->get_rows();
	Data &A = (*D)(i%R,i/R);
	int K = A.sg_data->get_row_blocks();
	int k = j%K;
	int l = j/K;
	printf("Assign D(%d,%d).SG(%d,%d).\n",i%R,i/R,k,l);
	SGData &a = (*A.sg_data)(k,l);
	a.set_sp_info(sp);
      }
    }
    int nby2 = Parameters.partition_level[2].blocks_per_row;
    int nbx2 = 1; // All variables here are vectors    // Parameters.partition_level[2].blocks_per_col;
    fn = Parameters.data_path+string("H");
    printf("Reading variable H from file %s.\n",fn.c_str());
    for(int block_index = 0; block_index < nby; block_index ++){
      byte *mem ;
      printf("Reading variable H(%d) from file %s.\n",block_index,fn.c_str());
      read_var_H_block(fn.c_str(),mem,nby ,block_index);
      (*H)(block_index).set_memory(mem);
    }
    fn = Parameters.data_path+string("atm");
    printf("Reading Attm fromom file %s.\n",fn.c_str());
    uint64_t size=read_var_Atm(fn.c_str(),Atm);
    Parameters.atm_block_size_L1 = size/nby;
    Parameters.atm_block_size_L2 = size/nby/nby2;
    printf("Partitioning variables for 2nd level.\n");
    H->partition_2nd_level(nby2,nbx2);printf("H partitioned.\n");
     T->partition_2nd_level(nby2,nbx2);printf("T partitioned.\n");
    F1->partition_2nd_level(nby2,nbx2);printf("F1 partitioned.\n");
    F2->partition_2nd_level(nby2,nbx2);
    F3->partition_2nd_level(nby2,nbx2);
    F4->partition_2nd_level(nby2,nbx2);
    H1->partition_2nd_level(nby2,nbx2);printf("H1 partitioned.\n");
    H2->partition_2nd_level(nby2,nbx2);
    H3->partition_2nd_level(nby2,nbx2);
    H4->partition_2nd_level(nby2,nbx2);
    printf("All partitioned.\n");
  }
  /*----------------------------------------*/
  void finalize(){
    //sw_engine->dump_tasks();
    sw_engine->finalize();
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
      for(int j=0; j< _D.get_cols(); j++){
	DiffTask *Diff = new DiffTask(_D(i,j),_H(j),_T(j),p);
	sw_engine->submit(Diff);
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
    }
  }
  /*----------------------------------------*/
  void f(Data *dH,Data *h, SWTask * p){
    mult( T, D, h, p);
    rhs (dH, T, h, p);
  }
  /*----------------------------------------*/
  void add (Data *a1,Data *a2, double dt, Data *a4, SWTask *p){
    Data &Y(*a1),&A(*a2),&X(*a4);
    int nb = Parameters.partition_level[1].blocks_per_row;
    for(int i=0; i< nb; i++){
      AddTask *Add = new AddTask(A(i),X(i),dt,Y(i),p);
      sw_engine->submit(Add);
    }
  }
  /*----------------------------------------------------*/
  void step(Data *h, Data *f1, Data *f2 , Data *f3 , Data *f4, SWTask *p){
    Data &_H(*h),&_F1(*f1),&_F2(*f2),&_F3(*f3),&_F4(*f4);
    int nb = Parameters.partition_level[1].blocks_per_row;
    for(int i=0; i< nb; i++){
      StepTask * Step=new StepTask(_F1(i),_F2(i),_F3(i),_F4(i),_H(i),p);
      sw_engine->submit(Step);
    }
  }
  /*----------------------------------------------------*/
  IterationData *TimeStepsTask::D = nullptr;
  int TimeStepsTask::last_step=0;
  void run(){
    TimeStepsTask::D = new IterationData();    
    sw_engine->submit(new TimeStepsTask);    
    sw_engine->submit(new TimeStepsTask);        
  }
  /*----------------------------------------------------*/
  void runStep(SWTask*);
  void TimeStepsTask::finished(){
    SWTask::finished();
    if ( last_step  < 4 )         
      sw_engine->submit(new TimeStepsTask );
  }
  void TimeStepsTask::runKernel(){
    runStep(this);
  }
  /*----------------------------------------------------*/
  void runStep(SWTask *p){
    double dt = Parameters.dt;
    f(F1,H, p);                      // F1 = f(H)
    add(H1, H , 0.5*dt, F1 , p);     // H1 = H + 0.5*dt*F1
    return;
    f(F2,H1,p);
    add(H2, H , 0.5*dt, F2 , p);     // H2 = H + 0.5*dt*F2
    f(F3,H2,p);
    add(H3, H ,     dt, F3 , p);     // H2 = H +     dt*F3
    f(F4,H3,p);
    step(H,F1,F2,F3,F4,p);          // H = H + dt/6*(F1+2*F2+2*F3+F4)
  }
  /*----------------------------------------------------------------*/
  void TimeStepsTask::register_data(){
    parent_context = sw_engine;
    setName("TimeSetpTask");
    D->setName("TimeStepsData");
    if ( last_step ==1)
      D->setRunTimeVersion("0.0",0);      
    IDuctteipTask::key = key;
    DataAccessList *dlist = new DataAccessList;    
    data_access(dlist,D,last_step %2?IData::READ:IData::WRITE);
    cout << "TS arg count: " << dlist->size() << endl;
    setDataAccessList(dlist);      
    host = me;
  }
  /*----------------------------------------------------------------*/
  DTSWData::DTSWData(){      
    memory_type = USER_ALLOCATED;
    host_type=SINGLE_HOST;
    IData::parent_data = NULL;
    setDataHandle( sw_engine->createDataHandle());
    setDataHostPolicy( glbCtx.getDataHostPolicy() ) ;
    setLocalNumBlocks(1,1);
    IData::Mb = 0;
    IData::Nb = 0;
    setHostType(SINGLE_HOST);
    setParent(sw_engine);
    sw_engine->addInputData(this);
    setRunTimeVersion("0.0",0);
  }
  /*----------------------------------------------------------------*/
  IterationData::IterationData(){      
    memory_type = USER_ALLOCATED;
    host_type=ALL_HOST;
    IData::parent_data = NULL;
    setDataHandle( sw_engine->createDataHandle());
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
    int block_size = d.level2_mem_size / R / C;
    dt_data = static_cast<Data *>(&d);
    d.sg_data = this;
    for ( int i=0;i<rows;i++){
      for ( int j=0;j<cols;j++){
	SGData *sgd = new SGData (i,j);
	parts.push_back(sgd);
	sgd->memory = d.get_memory() + (rows * j + i) * block_size * sizeof(quad<double>);
      }
    }
  }
  /*----------------------------------------------------------------------------*/
  DTSWData::DTSWData (int M, int N, int r,int c, std::string n, bool isSparse)
    :name(n){
      level2_mem_size = M*N*sizeof(quad<double>)/r/c;
      for(int j=0;j<c;j++){
	for(int i=0;i<r;i++){
	  DTSWData*t=new DTSWData;
	  t->row_idx = i;
	  t->col_idx = j;
	  t->sp_row  = i;
	  t->sp_col  = j;
	  if(!isSparse){
	    t->level2_mem_size = level2_mem_size;
	    t->memory = new byte[level2_mem_size];
	    t->setHost(j%Parameters.P);
	    t->setHostType(SINGLE_HOST);
	  }
	  std::stringstream ss;
	  if ( c>1)
	    ss << n << "(" << i << "," << j << ")";
	  else
	    ss << n << "(" << i <<  ")";
	  t->setName(ss.str());
	  name.assign(getName());
	  Dlist.push_back(t);
	}
      }
      rows = r;
      cols = c;
    }
  /*---------------------------------------------*/
  RHSTask::RHSTask(Data &a, Data &b, Data &c,SWTask *p){
    A = static_cast<Data *>(&a);
    B = static_cast<Data *>(&b);
    C = static_cast<Data *>(&c);
    p = parent;
    atm_offset = a.get_block_row() * Parameters.atm_block_size_L1;
    if(parent)
      parent->child_count++;
    *this << *A << *B >> *C;
    key = RHS;
    host = C->getHost();
    setName("RHSTask");	  
  }
}

