//
//  CXXThread.hpp
//  ios-learn
//
//  Created by liguanjun on 17/2/9.
//  Copyright © 2017年 liguanjun. All rights reserved.
//

#ifndef CXXThread_hpp
#define CXXThread_hpp

#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread/pthread.h>

namespace CXXThread {
    
#pragma mark - no copy protocol
    class NonCopyableProtocol {
    protected:
        NonCopyableProtocol() {}
        ~NonCopyableProtocol() {}
    private:  // emphasize the following members are private
        NonCopyableProtocol(const NonCopyableProtocol&);
        const NonCopyableProtocol& operator=(const NonCopyableProtocol&);
    };
    
#pragma mark - Lockable Protocol
    class LockableProtocol {
    public:
        virtual int lock() = 0;
        virtual int unlock() = 0;
    };

#pragma mark - 互斥锁
    class MutexLock: public LockableProtocol, NonCopyableProtocol {
    public:
        MutexLock();
        int tryLock();
        int lock();
        int unlock();
        ~MutexLock();
    private:
        friend class Condition;
        pthread_mutex_t m_mutex;
    };

#pragma mark - MutexLock Guard
    class MutexLockGuard: public NonCopyableProtocol {
    public:
        MutexLockGuard(MutexLock& mutex): m_mutex(mutex) {
            m_mutex.lock();
        }
        
        ~MutexLockGuard() {
            m_mutex.unlock();
        }
    private:
        MutexLock& m_mutex;
    };

#pragma mark - 条件变量
    class Condition {
    public:
        Condition(MutexLock& mutexLock): m_mutex(mutexLock)
        {
            pthread_cond_init(&m_cond, nullptr);
        }
        void wait()
        {
            pthread_cond_wait(&m_cond, &m_mutex.m_mutex);
        }
        int notifyOnce()
        {
            return pthread_cond_signal(&m_cond);
        }
        int notifyAll()
        {
            return pthread_cond_broadcast(&m_cond);
        }
        ~Condition()
        {
            pthread_cond_destroy(&m_cond);
        }
    private:
        pthread_cond_t m_cond;
        MutexLock &m_mutex;
    };
    
#pragma mark - 线程对象
    class Thread: public NonCopyableProtocol {
    public:
        static void wait(Thread& thread);
        static void wait(Thread* thread);
        static void cancel(Thread& thread);
        static void cancel(Thread* thread);
        
        Thread();
        virtual ~Thread();
        
        void start();
        void cancel();
        
    protected:
        virtual void body();
    private:
        static void* workingFunc(void *obj);
        
        pthread_t m_thread;
        pthread_attr_t m_attr;
    };

}

#endif /* CXXThread_hpp */
