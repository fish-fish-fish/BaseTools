//
//  CurlTask.cpp
//  ios-learn
//
//  Created by liguanjun on 2018/5/15.
//  Copyright © 2018年 liguanjun. All rights reserved.
//

#include "CurlTask.hpp"

#define DEADLINE 20
namespace CXXThread {
    #pragma mark - CurlTask
    #pragma mark -- 构造函数
    CurlTask::CurlTask(std::string url)
    {
        this->m_url = url;
        this->m_willPaused = false;
        this->m_isPaused = false;
        this->m_canceled = false;
        this->m_rangeFrom = 0;
        this->m_dlnow = 0;
        this->m_dltotal = 0;
        this->m_retryTimes = 3;
        std::cout << "DownloadTask::DownloadTask(string url)" << std::endl;
    }
    
    CurlTask::CurlTask(std::string url, double rangeFrom):CurlTask(url)
    {
        this->m_rangeFrom = rangeFrom;
    }
    
    #pragma mark -- Public interfaces
    const std::string& CurlTask::url() {
        return m_url;
    }
    
    void CurlTask::setDelegate(std::weak_ptr<CurlTaskDelegate> &delegate) {
        m_delegate = delegate;
    }
    void CurlTask::setDelegate(std::shared_ptr<CurlTaskDelegate> &delegate) {
        m_delegate = delegate;
    }
    
    bool CurlTask::isPaused() {
        return m_isPaused;
    }
    
    void CurlTask::resum() {
        this->m_willPaused = false;
    }
    
    void CurlTask::pause() {
        this->m_willPaused = true;
    }
    
    void CurlTask::cancel() {
        this->m_canceled = true;
    }
    
    #pragma mark -- 函数对象重写
    void CurlTask::operator() () {
        m_handle = curl_easy_init();
        curl_easy_setopt(m_handle, CURLOPT_URL, m_url.c_str());
        curl_easy_setopt(m_handle, CURLOPT_WRITEFUNCTION, write_data_callback);
        curl_easy_setopt(m_handle, CURLOPT_WRITEDATA, this);
        curl_easy_setopt(m_handle, CURLOPT_PROGRESSFUNCTION, progress_callback);
        curl_easy_setopt(m_handle, CURLOPT_PROGRESSDATA, this);
        curl_easy_setopt(m_handle, CURLOPT_NOPROGRESS, false);
        curl_easy_setopt(m_handle, CURLOPT_HEADERFUNCTION, write_header_callback);
        curl_easy_setopt(m_handle, CURLOPT_HEADERDATA, this);
//        curl_easy_setopt(m_handle, CURLOPT_SSL_VERIFYPEER, 0L);
//        curl_easy_setopt(m_handle, CURLOPT_SSL_VERIFYHOST, 0L);
        curl_easy_setopt(m_handle, CURLOPT_SSL_VERIFYPEER, 1L);
        curl_easy_setopt(m_handle, CURLOPT_SSL_VERIFYHOST, 2L);
        m_code = CURL_LAST;
        while (m_code != CURLE_OK && m_retryTimes--) {
            //            curl_easy_setopt(handle, CURLOPT_RESUME_FROM_LARGE, this->dlnow);
            struct curl_slist *headers = NULL;
            char buffer[64] = {0};
            sprintf(buffer, "Range: %lld-", (long long)this->m_rangeFrom);
            headers = curl_slist_append(headers, buffer);
            curl_easy_setopt(m_handle, CURLOPT_HTTPHEADER, headers);
            m_lastUpdateTime = time(NULL);
            std::cout << "code:" << m_code << std::endl;
            std::cout << "range from: " << (long long)this->m_rangeFrom << std::endl;
            std::cout << "still have " << m_retryTimes << " times" << std::endl;
            m_code = curl_easy_perform(m_handle);
            m_rangeFrom += m_dlnow;
            
            // 取消 不再重试连接
            if (m_canceled) {
                break;
            }
        }
        auto delegate = this->m_delegate.lock();
        if (m_code == CURLE_OK) {
            //  下载完成
            if (delegate) {
                delegate->downloadComleted();
            }
        }else{
            // 下载失败
            if (delegate) {
                delegate->downloadFailed(this->m_code);
            }
        }
        curl_easy_cleanup(m_handle);
        m_handle = nullptr;
    }
    
#pragma mark -- Curl callback functions
    size_t CurlTask::write_header_callback(char *buffer, size_t size, size_t nitems, void *outstream)
    {
        CurlTask *task = (CurlTask *)outstream;
        std::string content(buffer);
        task->m_response.append(content);
        std::shared_ptr<CurlTaskDelegate> delegate = task->m_delegate.lock();
        if (size * nitems == 2 && delegate) {
            delegate->receiveResponse(task->m_response);
        }
        return size * nitems;
    }
    
    
    size_t CurlTask::write_data_callback(char *buffer, size_t size, size_t nitems, void *outstream)
    {
        CurlTask *task = (CurlTask *)outstream;
        std::shared_ptr<CurlTaskDelegate> delegate = task->m_delegate.lock();
        if (delegate) {
            delegate->receiveData(buffer, size * nitems);
        }
        return size * nitems;
    }
    
    int CurlTask::progress_callback(void *clientp, double dltotal, double dlnow, double ultotal, double ulnow)
    {
        CurlTask *task = (CurlTask *)clientp;
        // 检测是否取消下载任务
        if (task->m_canceled)
        {
            return -1;
        }
        
        time_t nowtime = time(NULL);
        
        // 心跳
        //        cout << nowtime << "  bengbeng" << endl;
        std::shared_ptr<CurlTaskDelegate> delegate = task->m_delegate.lock();
        // 更新进度
        task->m_dltotal = dltotal;
        if (dltotal && task->m_dlnow != dlnow) {
            task->m_dlnow = dlnow;
            task->m_lastUpdateTime = nowtime;
            if (delegate) {
                delegate->updateProgress(task->m_rangeFrom + dltotal, task->m_rangeFrom + dlnow);
            }
        }
        
        // 暂停／恢复 处理
        if (task->m_willPaused == true && task->m_isPaused == false) {
            curl_easy_pause(task->m_handle, CURLPAUSE_RECV);
            task->m_isPaused = true;
            if (delegate) {
                delegate->didPaused();
            }
        }else if (task->m_willPaused == false && task->m_isPaused == true) {
            curl_easy_pause(task->m_handle, CURLPAUSE_RECV_CONT);
            task->m_isPaused = false;
            if (delegate) {
                delegate->didResumed();
            }
            task->m_lastUpdateTime = nowtime;
        }
        
        // 长时间无新数据下载时，断开链接
        if (task->m_canceled || (task->m_isPaused == false && nowtime - task->m_lastUpdateTime > DEADLINE) ) {
            return -1;
        } else {
            return 0;
        }
    }
}
