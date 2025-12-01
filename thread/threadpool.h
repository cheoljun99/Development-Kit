#pragma once

#include "thread.h"
#include <iostream>
#include <deque>
#include <memory>

class ThreadPool{
protected:
    std::deque<std::unique_ptr<Thread>> threads_;
    bool start_flag_;
public:
    ThreadPool() : start_flag_(false){}
    bool start_pool(){
        if(start_flag_==true){
            std::cerr << "[ERROR] already start thread pool "
			<< "(WorkerPool::start_pool) " << '\n';
			return false;
        }
        for(size_t i=0; i < threads_.size();i++){ 
            if(threads_[i]->start_thread()==false){
                for(size_t j=0; j < i;++j){//rollback
                    threads_[j]->stop_thread();
                }
                return false;
            }
        }
        start_flag_=true;
        return true;
    }
    void stop_pool(){
        if(start_flag_==true){
            for(size_t i=0; i < threads_.size();++i){ 
                threads_[i]->stop_thread();
            }
            start_flag_=false;
        }
    }
    virtual bool monitor_pool() = 0;
    virtual ~ThreadPool(){}
};