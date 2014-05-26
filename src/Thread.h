/*
 * =====================================================================================
 *
 *       Filename:  Thread.h
 *
 *    Description:  Abandon!!!
 *
 *        Version:  1.0
 *        Created:  05/22/2014 03:04:31 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  billowkiller (), billowkiller@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef ECAP_ADAPTER_STRMATCHER_H
#define ECAP_ADAPTER_STRMATCHER_H

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

#endif