#include "dtsw.hpp"
namespace dtsw{
    /*----------------------------------------*/
    Parameters_t Parameters;
    Data *H,*T,*D;
    Data *F1,*F2,*F3,*F4;
    Data *H1,*H2,*H3,*H4;
    /*----------------------------------------*/
    void init(int argc, char *argv[]){
        int nb = Parameters.nb;
        H = new Data(nb, 1 );
        T = new Data(nb, 1 );
        D = new Data(nb, nb);
        F1 = new Data(nb, 1 );
        F2 = new Data(nb, 1 );
        F3 = new Data(nb, 1 );
        F4 = new Data(nb, 1 );

        H1 = new Data(nb, 1 );
        H2 = new Data(nb, 1 );
        H3 = new Data(nb, 1 );
        H4 = new Data(nb, 1 );

        // import problem setup
        //
    }
    /*----------------------------------------*/
    void finalize(){
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
    }
    /*----------------------------------------*/
    void f(Data *a1,Data *a2){}
    /*----------------------------------------*/
    void add (Data *,Data *, double , Data *){}
    /*----------------------------------------------------*/
    void step(Data *, Data *, Data * , Data * , Data *){}
    /*----------------------------------------------------*/
    void run(){
        double dt = Parameters.dt;
        f(F1,H);                      // F1 = f(H)
        add(H1, H , 0.5*dt, F1 );     // H1 = H + 0.5*dt*F1
        f(F2,H1);
        add(H2, H , 0.5*dt, F2 );     // H2 = H + 0.5*dt*F2
        f(F3,H2);
        add(H3, H ,     dt, F3 );     // H2 = H + 0.5*dt*F3
        f(F4,H3);
        step(H,F1,F2,F3,F4);          // H = H + dt/6*(F1+2*F2+2*F3+F4)
        // next_time_step
    }
    /*----------------------------------------*/
}
