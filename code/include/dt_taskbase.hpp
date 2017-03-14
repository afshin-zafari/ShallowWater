#ifndef DT_TASKBASE_HPP
#define DT_TASKBASE_HPP
#include "dt_database.hpp"
namespace dtsw{
    class SWTask{
    private:
    public:
        virtual void run()=0;
        virtual void dump()=0;
    };
}
#endif //DT_TASKBASE_HPP
