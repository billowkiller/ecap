#include <pthread.h>
#include <string>
#include <boost/function.hpp>
#include <boost/bind.hpp>

using namespace std;
class Thread
{
    typedef boost::function<void()> ThreadFun;
    public:
        Thread(const ThreadFun& threadFun,const string& threadName = string());
        pid_t getThreadId();
        string    getThreadName();
        int start();
    
    private:
        static void* startThread(void* thread);
  
    private:
        pthread_t   m_thread;
        pid_t       m_tid;
        string      m_strThreadName;
        ThreadFun   m_func;
};
