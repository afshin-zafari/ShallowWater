#include <iostream>
#include "dtsw.hpp"

using namespace std;

int main(int argc , char *argv[])
{
    cout << "Hello world!" << endl;

    dtsw::init(argc,argv);
    dtsw::run();

    return 0;
}
