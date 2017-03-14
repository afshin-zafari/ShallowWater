#ifndef DTSW_HPP
#define DTSW_HPP
#include "sw_dist.hpp"
namespace dtsw{
    /*----------------------------------------*/
    void init(int argc, char *argv[]);
    void finalize();
    void run();
    /*----------------------------------------*/
    typedef struct parameters{
        int nb;
        double dt;
    }Parameters_t;
    extern Parameters_t Parameters;
    /*----------------------------------------*/
}
#endif // DTSW_HPP
