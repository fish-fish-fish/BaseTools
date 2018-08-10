//
//  CXXThread.cpp
//  ios-learn
//
//  Created by liguanjun on 17/2/9.
//  Copyright © 2017年 liguanjun. All rights reserved.
//

#include "CXXThread.hpp"
namespace CXXThread
{
#pragma mark - Thread
    void* Thread::workingFunc(void *obj)
    {
        ((Thread*)obj)->body();
        return obj;
    }

    void Thread::wait(Thread& thread)
    {
        pthread_join(thread.m_thread, nullptr);
    }
    void Thread::wait(Thread* thread)
    {
        pthread_join(thread->m_thread, nullptr);
    }
    
    void Thread::cancel(Thread& thread)
    {
        pthread_cancel(thread.m_thread);
    }
    void Thread::cancel(Thread* thread)
    {
        pthread_cancel(thread->m_thread);
    }
    
    Thread::Thread()
    {
        pthread_attr_init(&m_attr);
        pthread_attr_setdetachstate(&m_attr,PTHREAD_CREATE_DETACHED);
    }
    
    Thread::~Thread()
    {
        pthread_attr_destroy(&m_attr);
    }
    
    void Thread::start()
    {
        pthread_create(&m_thread, &m_attr, &Thread::workingFunc, this);
    }
    
    void Thread::cancel()
    {
        pthread_cancel(m_thread);
    }
    
    void Thread::body()
    {
        
    }
    
#pragma mark - MutexLock
    MutexLock::MutexLock()
    {
        pthread_mutex_init(&m_mutex, nullptr);
    }
    int MutexLock::lock()
    {
        return pthread_mutex_lock(&m_mutex);
    }
    int MutexLock::tryLock()
    {
        return pthread_mutex_trylock(&m_mutex);
    }
    int MutexLock::unlock()
    {
        return pthread_mutex_unlock(&m_mutex);
    }
    MutexLock::~MutexLock()
    {
        pthread_mutex_destroy(&m_mutex);
    }
}
