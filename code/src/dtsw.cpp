#include "dtsw.hpp"
namespace dtsw{
    /*----------------------------------------*/
    Parameters_t Parameters;
    Data *H,*T,*D;
    Data *F1,*F2,*F3,*F4;
    Data *H1,*H2,*H3,*H4;
    atmdata_t *Atm;
    SWAlgorithm *sw_engine;
    /*----------------------------------------*/
    void init(int argc, char *argv[]){
        // import problem setup
        int nb = Parameters.nb = 3;
        H  = new Data(nb, 1 ,"H" );
        T  = new Data(nb, 1 ,"T" );
        D  = new Data(nb, nb,"D" );
        F1 = new Data(nb, 1 ,"F1");
        F2 = new Data(nb, 1 ,"F2");
        F3 = new Data(nb, 1 ,"F3");
        F4 = new Data(nb, 1 ,"F4");

        H1 = new Data(nb, 1 ,"H1");
        H2 = new Data(nb, 1 ,"H2");
        H3 = new Data(nb, 1 ,"H3");
        H4 = new Data(nb, 1 ,"H4");

        sw_engine = new SWAlgorithm;

    }
    /*----------------------------------------*/
    void finalize(){
        sw_engine->dump_tasks();
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
    void mult(Data *t,Data *d, Data *h){
        Data &_D(*d),&_H(*h), &_T(*t);
        for(int i=0; i< _D.size(); i++){
            int row = _D(i).sp_row;
            int col = _D(i).sp_col;
            DiffTask *Diff = new DiffTask(_T(row),_D(row,col),_H(col));
            sw_engine->submit(Diff);
        }
    }
    /*----------------------------------------*/
    void rhs(Data *dH , Data *t,Data *h){
        Data &DH(*dH),&_H(*h), &_T(*t);
        int nb = Parameters.nb;
        for(int i=0; i< nb; i++){
            RHSTask *RHS= new RHSTask(DH(i),_T(i),_H(i));
            sw_engine->submit(RHS);
        }
    }
    /*----------------------------------------*/
    void f(Data *dH,Data *h){
        mult( T, D, h );
        rhs(dH,T,h);
    }
    /*----------------------------------------*/
    void add (Data *a1,Data *a2, double dt, Data *a4){
        Data &Y(*a1),&A(*a2),&X(*a4);
        int nb = Parameters.nb;
        for(int i=0; i< nb; i++){
            AddTask *Add = new AddTask(Y(i),A(i),dt,X(i));
            sw_engine->submit(Add);
        }

    }
    /*----------------------------------------------------*/
    void step(Data *h, Data *f1, Data *f2 , Data *f3 , Data *f4){
        Data &_H(*h),&_F1(*f1),&_F2(*f2),&_F3(*f3),&_F4(*f4);
        int nb = Parameters.nb;
        for(int i=0; i< nb; i++){
            StepTask * Step=new StepTask(_H(i),_F1(i),_F2(i),_F3(i),_F4(i));
            sw_engine->submit(Step);
        }
    }
    /*----------------------------------------------------*/
    void run(){
        double dt = Parameters.dt;
        f(F1,H);                      // F1 = f(H)
        add(H1, H , 0.5*dt, F1 );     // H1 = H + 0.5*dt*F1
        f(F2,H1);
        add(H2, H , 0.5*dt, F2 );     // H2 = H + 0.5*dt*F2
        f(F3,H2);
        add(H3, H ,     dt, F3 );     // H2 = H +     dt*F3
        f(F4,H3);
        step(H,F1,F2,F3,F4);          // H = H + dt/6*(F1+2*F2+2*F3+F4)
        // next_time_step
    }
    /*----------------------------------------*/
}
