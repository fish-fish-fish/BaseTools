//
//  DownloadTask.cpp
//  ios-learn
//
//  Created by liguanjun on 17/3/15.
//  Copyright © 2017年 liguanjun. All rights reserved.
//

#include "DownloadTask.hpp"
using namespace std;
#define DEADLINE 20

namespace CXXThread
{
    size_t DownloadTask::write_header_callback(char *buffer, size_t size, size_t nitems, void *outstream)
    {
        DownloadTask *task = (DownloadTask *)outstream;
        string content(buffer);
        task->m_response.append(content);
        if (size * nitems == 2) {
            task->receiveResponse(task->m_response);
        }
        return size * nitems;
    }
    
    
    size_t DownloadTask::write_data_callback(char *buffer, size_t size, size_t nitems, void *outstream)
    {
        DownloadTask *task = (DownloadTask *)outstream;
        task->receiveData(buffer, size * nitems);
        return size * nitems;
    }
    
    int DownloadTask::progress_callback(void *clientp, double dltotal, double dlnow, double ultotal, double ulnow)
    {
        DownloadTask *task = (DownloadTask *)clientp;
        // 检测是否取消下载任务
        if (task->m_canceled)
        {
            return -1;
        }
        
        time_t nowtime = time(NULL);
        
        // 心跳
//        cout << nowtime << "  bengbeng" << endl;
        
        // 更新进度
        task->m_dltotal = dltotal;
        if (dltotal && task->m_dlnow != dlnow) {
            task->m_dlnow = dlnow;
            task->m_lastUpdateTime = nowtime;
            task->updateProgress(task->m_rangeFrom + dltotal, task->m_rangeFrom + dlnow);
        }
        
        // 暂停／恢复 处理
        if (task->m_willPaused == true && task->m_isPaused == false) {
            curl_easy_pause(task->m_handle, CURLPAUSE_RECV);
            task->m_isPaused = true;
            task->didPaused();
        }else if (task->m_willPaused == false && task->m_isPaused == true) {
            curl_easy_pause(task->m_handle, CURLPAUSE_RECV_CONT);
            task->m_isPaused = false;
            task->didResumed();
            task->m_lastUpdateTime = nowtime;
        }
        
        // 长时间无新数据下载时，断开链接
        if (task->m_canceled || (task->m_isPaused == false && nowtime - task->m_lastUpdateTime > DEADLINE) ) {
            return -1;
        } else {
            return 0;
        }
    }
    
    DownloadTask::DownloadTask(string url)
    {
        this->m_url = url;
        this->m_willPaused = false;
        this->m_isPaused = false;
        this->m_rangeFrom = 0;
        this->m_dlnow = 0;
        this->m_dltotal = 0;
        this->m_retryTimes = 3;
        std::cout << "DownloadTask::DownloadTask(string url)" << endl;
    }
    
    DownloadTask::DownloadTask(string url, double rangeFrom):DownloadTask(url)
    {
        this->m_rangeFrom = rangeFrom;
    }
    
    DownloadTask::~DownloadTask()
    {
        std::cout << "DownloadTask::~DownloadTask()" << endl;
    }
    
    void DownloadTask::cancel()
    {
        this->m_canceled = true;
    }
    
    void DownloadTask::pause()
    {
        this->m_willPaused = true;
    }
    
    void DownloadTask::resum()
    {
        this->m_willPaused = false;
    }
    
    void DownloadTask::body()
    {
        m_handle = curl_easy_init();
        curl_easy_setopt(m_handle, CURLOPT_URL, m_url.c_str());
        curl_easy_setopt(m_handle, CURLOPT_WRITEFUNCTION, write_data_callback);
        curl_easy_setopt(m_handle, CURLOPT_WRITEDATA, this);
        curl_easy_setopt(m_handle, CURLOPT_PROGRESSFUNCTION, progress_callback);
        curl_easy_setopt(m_handle, CURLOPT_PROGRESSDATA, this);
        curl_easy_setopt(m_handle, CURLOPT_NOPROGRESS, false);
        curl_easy_setopt(m_handle, CURLOPT_HEADERFUNCTION, write_header_callback);
        curl_easy_setopt(m_handle, CURLOPT_HEADERDATA, this);
        curl_easy_setopt(m_handle, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(m_handle, CURLOPT_SSL_VERIFYHOST, 0L);
        m_code = CURL_LAST;
        while (m_code != CURLE_OK && m_retryTimes--) {
//            curl_easy_setopt(handle, CURLOPT_RESUME_FROM_LARGE, this->dlnow);
            struct curl_slist *headers = NULL;
            char buffer[64] = {0};
            sprintf(buffer, "Range: %lld-", (long long)this->m_rangeFrom);
            headers = curl_slist_append(headers, buffer);
            curl_easy_setopt(m_handle, CURLOPT_HTTPHEADER, headers);
            m_lastUpdateTime = time(NULL);
            cout << "code:" << m_code << endl;
            cout << "range from: " << (long long)this->m_rangeFrom << endl;
            cout << "still have " << m_retryTimes << " times" << endl;
            m_code = curl_easy_perform(m_handle);
            m_rangeFrom += m_dlnow;
            
            // 取消 不再重试连接
            if (m_canceled) {
                break;
            }
        }
        if (m_code == CURLE_OK) {
            //  下载完成
            this->downloadComleted();
        }else{
            // 下载失败
            this->downloadFailed(this->m_code);
        }
        curl_easy_cleanup(m_handle);
        m_handle = nullptr;
    }
    
    bool DownloadTask::isPaused()
    {
        return m_isPaused;
    }
    
    void DownloadTask::didResumed()
    {
        
    }
    
    void DownloadTask::didPaused()
    {
        
    }
    
    void DownloadTask::receiveResponse(const string &response)
    {
        cout<< response << endl;
    }
    void DownloadTask::receiveData(char *buffer, size_t size)
    {
        cout<< "receiveData size:" << size << endl;
    }
    
    void DownloadTask::updateProgress(const double dltotal, const double dlnow)
    {
        cout<<"dltotal:" << (long long)dltotal << "  dlnow:" << (long long)dlnow << endl;
    }
    
    void DownloadTask::downloadComleted()
    {
        cout << "download completed." << endl;
    }
    
    void DownloadTask::downloadFailed(int code)
    {
        cout << "downlaod failed with code: " << code << endl;
    }
    
}
