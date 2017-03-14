#include <iostream>
#include "dtsw.hpp"

using namespace std;
namespace Tests{
  void time_steps(){}
  
}

int main(int argc , char *argv[])
{
  DuctTeip_Start(argc,argv);
  Tests::time_steps();
  dtsw::init(argc,argv);
  dtsw::run();
  dtsw::finalize();
  DuctTeip_Finish();

  return 0;
}
