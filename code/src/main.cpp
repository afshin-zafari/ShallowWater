#include <iostream>
#include "dtsw.hpp"

using namespace std;

int main(int argc , char *argv[])
{
    dtsw::init(argc,argv);
    dtsw::run();
    dtsw::finalize();

    return 0;
}
