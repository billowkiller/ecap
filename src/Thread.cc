/*
 * =====================================================================================
 *
 *       Filename:  Thread.cc
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  05/13/2014 09:41:23 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  billowkiller (), billowkiller@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include "thread.h"

Thread::Thread(const Thread::ThreadFun& threadFun, const string& threadName):
m_func(threadFun), m_strThreadName(threadName)
{
}

int Thread::start()
{
    m_tid = pthread_create(&m_thread, NULL, &startThread, this);
    return 0;
}

void* Thread::startThread(void* obj)
{
  Thread* thread = static_cast<Thread*>(obj);
  thread->m_func();
  return NULL;
}

pid_t Thread::getThreadId()
{
    return m_tid;
};

string  Thread::getThreadName()
{
    return m_strThreadName;
}
