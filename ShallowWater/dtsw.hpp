#ifndef DTSW_HPP
#define DTSW_HPP
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
    class DTSWData {
    public:
        DTSWData (int r,int c){}
    };
    typedef DTSWData Data;
}
#endif // DTSW_HPP
