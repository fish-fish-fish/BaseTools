//
//  CXXThreadPool.hpp
//  ios-learn
//
//  Created by liguanjun on 2018/5/14.
//  Copyright © 2018年 liguanjun. All rights reserved.
//

#ifndef CXXThreadPool_hpp
#define CXXThreadPool_hpp

#include <stdio.h>
#include <vector>
#include <queue>
#include <string>
#include <iostream>
#include "CXXThread.hpp"

namespace CXXThread {
    class Task;
    class TaskThread;
    class TaskQueue;
#pragma mark - 线程池
    class ThreadPool: NonCopyableProtocol {
    public:
        static ThreadPool& sharedInstance();
        void start(int numThreads);
        
    private:
        friend class TaskThread;
        static void init();
        ThreadPool();
        
        std::vector< std::unique_ptr<TaskThread> > m_threads;
        std::shared_ptr<TaskQueue> m_taskQueue;
        MutexLock m_mutex;
        Condition m_condition;
    };

#pragma mark - 线程池---工作线程
    class TaskThread: public Thread {
    public:
        virtual void body() final;
    };
    
#pragma mark - 纯虚基类任务
    class Task {
    public:
        virtual ~Task() {};
    protected:
        friend class TaskThread;
        virtual void operator() () = 0;
    };
    
#pragma mark - 测试任务
    class TestTask: public Task {
    public:
        TestTask(): m_ID(-1) {
            
        }
        
        TestTask(int id): m_ID(id) {
            std::cout<< m_ID << "create" << std::endl;
        }
        
        virtual ~TestTask() {
            std::cout<< m_ID << "destroy" << std::endl;
        }
        
    protected:
        void operator() () override {
            std::cout<< m_ID << "start" << std::endl;
            sleep(5);
            std::cout<< m_ID << "end" << std::endl;
        }
        
    private:
        int m_ID;
    };
}


#endif /* CXXThreadPool_hpp */
