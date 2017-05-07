#ifndef DTSW_HPP
#define DTSW_HPP
#include "sw_dist.hpp"
#include "util.hpp"
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
    typedef struct atmdata {
        double f;
        double x, y, z;
        double p_u[3], p_v[3], p_w[3];
        double ghm;
        double gradghm[3];
    }atmdata_t;

    extern Parameters_t Parameters;
    extern atmdata_t *Atm;
    /*----------------------------------------*/
}
#endif // DTSW_HPP
