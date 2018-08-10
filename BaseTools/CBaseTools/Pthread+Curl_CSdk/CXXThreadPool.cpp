//
//  CXXThreadPool.cpp
//  ios-learn
//
//  Created by liguanjun on 2018/5/14.
//  Copyright © 2018年 liguanjun. All rights reserved.
//

#include "CXXThreadPool.hpp"

namespace CXXThread {
    
#pragma mark - ThreadPool
    static ThreadPool *pInstance;
    ThreadPool& ThreadPool::sharedInstance() {
        static pthread_once_t once;
        pthread_once(&once, &ThreadPool::init);
        return *pInstance;
    }
    
    void ThreadPool::init() {
        pInstance = new ThreadPool();
        pInstance->start(5);
    }
    
    ThreadPool::ThreadPool(): m_threads(), m_taskQueue(), m_mutex(), m_condition(m_mutex) {
        
    }
    
    void ThreadPool::start(int numThreads) {
        for (int i=0; i < numThreads; ++i) {
            std::unique_ptr<TaskThread> thread(new TaskThread());
            thread->start();
            m_threads.push_back( std::move(thread) );
        }
    }
    
    void ThreadPool::push(std::shared_ptr<Task>& task){
        MutexLockGuard guard(m_mutex);
        m_taskQueue.push(task);
        m_condition.notifyOnce();
    }
    
    std::shared_ptr<Task> ThreadPool::pop(){
        MutexLockGuard guard(m_mutex);
        if (m_taskQueue.size() == 0) {
            m_condition.wait();
        }
        std::shared_ptr<Task> task = std::move( m_taskQueue.front() );
        m_taskQueue.pop();
        return task;
    }
    
#pragma mark - TaskThread
    void TaskThread::body() {
        while (1) {
            ThreadPool& pool = ThreadPool::sharedInstance();
            std::shared_ptr<Task> task = pool.pop();
            assert(task);
            task->operator()();
        }
    }
    
}
