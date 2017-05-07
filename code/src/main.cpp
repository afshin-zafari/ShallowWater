#include <iostream>
#include "dtsw.hpp"

using namespace std;


int main(int argc , char *argv[])
{
//    dtsw::test_read_sparse();
//    dtsw::test_read_H();

    dtsw::test_read_Atm();
    /*
    dtsw::init(argc,argv);
    dtsw::run();
    dtsw::finalize();
    */

    return 0;
}
