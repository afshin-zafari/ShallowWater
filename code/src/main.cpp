#include <iostream>
#include "dtsw.hpp"

using namespace std;
namespace Tests{
  dtsw::Data *TS;
  void time_steps(){
    int ts=10;
    TS= new dtsw::Data(ts,1,"TS");
    dtsw::Data _TS(*TS);
    for(int i=0;i<5;i++){
      cout << "TS name : "<<_TS(i).name << endl;
      _TS(i).setHost(me);
      dtsw::TimeStepsTask *task = new dtsw::TimeStepsTask(_TS(i),i);
      dtsw::sw_engine->submit(task);
    }
    for(int i=5;i<ts;i++){
      _TS(i).setHost(me);
      cout << "TS name : "<<_TS(i-5).name << endl;
      cout << "TS name : "<<_TS(i).name << endl;
      dtsw::TimeStepsTask *task = new dtsw::TimeStepsTask(_TS(i-5),_TS(i),i);
      cout << "111111111111111111111111111\n";
      dtsw::sw_engine->submit(task);
      cout << "2+++++++++++++++++++++++++++++++++++++++++++\n";
    }
  }
  void cleanup(){
    delete TS;
  }
  
}
class Base{
  double a;
};
class Inh: public Base{
  int r;
  char d[500];
};

int main(int argc , char *argv[])
{
  DuctTeip_Start(argc,argv);
  dtsw::init(argc,argv);
  cout << "me = " << me << endl;
  Tests::time_steps();

  //dtsw::run();

  dtsw::finalize();
  DuctTeip_Finish();
  Tests::cleanup();

  return 0;
}
