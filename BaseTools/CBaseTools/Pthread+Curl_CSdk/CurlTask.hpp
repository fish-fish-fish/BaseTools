//
//  CurlTask.hpp
//  ios-learn
//
//  Created by liguanjun on 2018/5/15.
//  Copyright © 2018年 liguanjun. All rights reserved.
//

#ifndef CurlTask_hpp
#define CurlTask_hpp

#include <stdio.h>
#include "curl.h"
#include "CXXThreadPool.hpp"
#include "TaskQueue.hpp"
namespace CXXThread {
    
    class CurlTaskDelegate;
    class QueueTask;
    class CurlTask: public QueueTask {
    public:
        CurlTask(std::string url);
        CurlTask(std::string url, double rangeFrom);
        const std::string& url();
        void setDelegate(std::weak_ptr<CurlTaskDelegate>& delegate);
        void setDelegate(std::shared_ptr<CurlTaskDelegate>& delegate);
        bool isPaused();
        void resum();
        void pause();
        void cancel();
    
    protected:
        void operator() () ;
        
    private:
        CURL *m_handle;
        std::weak_ptr<CurlTaskDelegate> m_delegate;
        CURLcode m_code;
        std::string m_url;
        std::string m_response;
        time_t m_lastUpdateTime;
        int m_retryTimes;
        bool m_willPaused;
        bool m_isPaused;
        double m_dltotal;
        double m_dlnow;
        double m_rangeFrom;
        bool m_canceled;
        
        static size_t write_header_callback(char *buffer, size_t size, size_t nitems, void *outstream);
        static size_t write_data_callback(char *buffer, size_t size, size_t nitems, void *outstream);
        static int progress_callback(void *clientp, double dltotal, double dlnow, double ultotal, double ulnow);
    };
    
    class CurlTaskDelegate {
    public:
        virtual void didResumed() = 0;
        virtual void didPaused() = 0;
        virtual void receiveResponse(const std::string &response) = 0;
        virtual void receiveData(char *buffer, size_t size) = 0;
        virtual void updateProgress(const double dltotal, const double dlnow) = 0;
        virtual void downloadComleted() = 0;
        virtual void downloadFailed(int code) = 0;
    };
    
    class CurlTaskTestDelegate: public CurlTaskDelegate {
    public:
        virtual void didResumed() {
            std::cout<< __FUNCTION__ << std::endl;
        };
        virtual void didPaused() {
            std::cout<< __FUNCTION__ << std::endl;
        };
        virtual void receiveResponse(const std::string &response) {
            std::cout<< __FUNCTION__ << std::endl;
        };
        virtual void receiveData(char *buffer, size_t size) {
            std::cout<< __FUNCTION__ << std::endl;
        };
        virtual void updateProgress(const double dltotal, const double dlnow) {
            std::cout<< __FUNCTION__ << std::endl;
        };
        virtual void downloadComleted() {
            std::cout<< __FUNCTION__ << std::endl;
        };
        virtual void downloadFailed(int code) {
            std::cout<< __FUNCTION__ << std::endl;
        };
    };
}

#endif /* CurlTask_hpp */
