//
//  TaskQueue.hpp
//  CBaseTools
//
//  Created by 李冠军 on 2018/10/15.
//  Copyright © 2018年 liguanjun. All rights reserved.
//

#ifndef TaskQueue_hpp
#define TaskQueue_hpp

#include <stdio.h>
#include "CXXThreadPool.hpp"

namespace CXXThread {
    
    class TaskQueue;
    class GlobalQueue;
    
    class QueueTask: public Task {
    public:
        std::weak_ptr<TaskQueue> inQueue;
        void beforeRun();
        void afterRun();
    };
    
    class TaskQueue: public NonCopyableProtocol {
    public:
        static std::shared_ptr<GlobalQueue> globalQueue();
        
        TaskQueue(int maxConcurrentCount = 1);
        void addSubQueue(std::shared_ptr<TaskQueue>& subQueue);
        void setSuperQueue(std::shared_ptr<TaskQueue>& superQueue);
        void push(std::shared_ptr<QueueTask>& task);
        std::shared_ptr<QueueTask> pop();
        std::shared_ptr<QueueTask> popQueueTask();
        int runningTaskCount();
        void increaseRunningCount();
        void decreaseRunningCount();
        virtual void signal();
    protected:
        MutexLock m_lock;
    private:
        friend class ThreadPool;
        const int m_maxConcurrentCount;
        int m_runningTaskCount;
        int m_currentQueueIndex;
        
        std::queue<std::shared_ptr<QueueTask>> m_taskQueue;
        std::vector<std::shared_ptr<TaskQueue>> m_subQueues;
        std::weak_ptr<TaskQueue> m_superQueue;
        
    };
    
    class GlobalQueue: public TaskQueue {
    public:
        GlobalQueue(int maxConcurrentCount = 1);
        void signal() override;
        void waitSignal();
        
    private:
        MutexLock m_mutex;
        Condition m_condition;
    };
    
}

#endif /* TaskQueue_hpp */
