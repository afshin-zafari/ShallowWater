#ifndef SG_TASKBASE_HPP
#define SG_TASKBASE_HPP
#include "sg_database.hpp"
namespace dtsw{
    class SGTask{
    public:
        virtual void run()=0;
    };
    class SGAddTask: public SGTask{
    private:
        SGData a,b,c;
        double dt;
    public:

        SGAddTask(SGData &, SGData &, double , SGData &){}
        void run();
    };
  
}
#endif //SG_TASKBASE_HPP
