#include "dtsw.hpp"
#include "util.hpp"
namespace dtsw{
  /*----------------------------------------*/
  Parameters_t Parameters;
  Data *H,*T,*D;
  Data *F1,*F2,*F3,*F4;
  Data *H1,*H2,*H3,*H4;
  atmdata_t *Atm;
  SWAlgorithm *sw_engine;
  void parse_args(int argc,char *argv[]){
    //argv: -M x x x -N x x x -
    /*
    Parameters.nb = 3;//todo : setting/getting Parameters
    Parameters.dist.P = config.P;
    Parameters.dist.p = config.p;
    Parameters.dist.q = config.q;
    Parameters.dist.rows_per_block = ;    
    Parameters.dist.cols_per_block = ;
    Parameters.dist.blocks_per_row = ;
    Parameters.dist.blocks_per_col = ;

    Parameters.level2.rows_per_block = ;    
    Parameters.level2.cols_per_block = ;

    Parameters.dt=0.5;
    strcpy(Parameters.filename,"");
    Parameters.chunk_size = 0;
    */
    
  }
  /*----------------------------------------*/
  void init(int argc, char *argv[]){
    // import problem setup
    dtEngine.start(argc,argv);
    sw_engine = new SWAlgorithm;
    parse_args(argc,argv);
    int nby = Parameters.partition_level[1].Mb;
    int nbx = Parameters.partition_level[1].Nb;
    H  = new Data(nby, 1  ,"H" );
    T  = new Data(nby, 1  ,"T" );
    D  = new Data(nby, nbx,"D" );
    F1 = new Data(nby, 1  ,"F1");
    F2 = new Data(nby, 1  ,"F2");
    F3 = new Data(nby, 1  ,"F3");
    F4 = new Data(nby, 1  ,"F4");

    H1 = new Data(nby, 1  ,"H1");
    H2 = new Data(nby, 1  ,"H2");
    H3 = new Data(nby, 1  ,"H3");
    H4 = new Data(nby, 1  ,"H4");

    SpInfo sp_info;
    read_var_D(Parameters.filename,sp_info.index,sp_info.data);
    split(sp_info,Parameters.partition_level[1].blocks_per_row,Parameters.partition_level[1].blocks_per_col,Parameters.partition_level[1].chunk_size);
    for(uint32_t i=0;i<sp_info.sp_blocks.size();i++){
      split(*sp_info.sp_blocks[i],Parameters.partition_level[2].blocks_per_row,
	                          Parameters.partition_level[2].blocks_per_col,
	                          Parameters.partition_level[2].chunk_size);
      
    }
    for(uint32_t i=0;i<nby ;i++){
      for(uint32_t j=0;j<nbx ;j++){
	Data &dt = (*D)(i,j);
	SGData *p = new SGData ;
	p->partition_data ( dt, Parameters.partition_level[2].rows_per_block,
			        Parameters.partition_level[2].cols_per_block);
	dt.sg_data = p;
      }
    }
    int nby2 = Parameters.partition_level[2].blocks_per_row;
    int nbx2 = Parameters.partition_level[2].blocks_per_col;
    H->partition_2nd_level(nby2,nbx2);
    T->partition_2nd_level(nby2,nbx2);
    F1->partition_2nd_level(nby2,nbx2);
    F2->partition_2nd_level(nby2,nbx2);
    F3->partition_2nd_level(nby2,nbx2);
    F4->partition_2nd_level(nby2,nbx2);
    H1->partition_2nd_level(nby2,nbx2);
    H2->partition_2nd_level(nby2,nbx2);
    H3->partition_2nd_level(nby2,nbx2);
    H4->partition_2nd_level(nby2,nbx2);
    for(uint32_t i=0;i<sp_info.sp_blocks.size();i++){
      for(uint32_t j=0;j<sp_info.sp_blocks[i]->sp_blocks.size();j++){
	SpInfo &sp = *sp_info.sp_blocks[i]->sp_blocks[j];
	Data &A = (*D)(i,j);
	int K = A.sg_data->get_row_blocks();
	int L = A.sg_data->get_col_blocks();
	for(int k=0;k<K;k++){
	  for(int l=0;l<L;l++){
	    SGData &a = (*A.sg_data)(k,l);
	    a.set_sp_info(sp);
	  }
	}
      }
    }
      
    sp_info.data.clear();
    sp_info.index.clear();

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
  Data *TimeStepsTask::D = nullptr;
  int TimeStepsTask::last_step=0;
  void run(){
    TimeStepsTask::D = new Data();    
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
    dt_data = static_cast<Data *>(&d);
    for ( int i=0;i<rows;i++){
      for ( int j=0;j<cols;j++){
	SGData *sgd = new SGData (i,j);
	parts.push_back(sgd);
      }
    }
  }
}

