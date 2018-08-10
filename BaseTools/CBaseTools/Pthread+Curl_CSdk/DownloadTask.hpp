//
//  DownloadTask.hpp
//  ios-learn
//
//  Created by liguanjun on 17/3/15.
//  Copyright © 2017年 liguanjun. All rights reserved.
//

#ifndef DownloadTask_hpp
#define DownloadTask_hpp
#include "CXXThread.hpp"
#include <string>
#include <iostream>
#include <fstream>
#include "curl.h"
using namespace std;
namespace CXXThread
{
    class DownloadTask : public Thread {
    private:
        CURL *m_handle;
        CURLcode m_code;
        string m_url;
        string m_response;
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
    public:
        DownloadTask(string url);
        DownloadTask(string url, double rangeFrom);
        virtual ~DownloadTask();
        bool isPaused();
        void pause();
        void resum();
        void cancel();
        void body() override;
        virtual void didResumed();
        virtual void didPaused();
        virtual void receiveResponse(const string &response);
        virtual void receiveData(char *buffer, size_t size);
        virtual void updateProgress(const double dltotal, const double dlnow);
        virtual void downloadComleted();
        virtual void downloadFailed(int code);
    };
}
#endif /* DownloadTask_hpp */
