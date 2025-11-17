#pragma once
#include <atomic>
#include <iostream>

class Thread {
protected:
    std::atomic<bool> thread_term_;
    Thread() : thread_term_(false) {}
    virtual ~Thread() {}

    virtual bool setup() = 0;
    virtual void cleanup() = 0;
    virtual void thread_loop() = 0;

    // 공통 static 스레드 진입점
    static void* thread_func(void* arg) {
        Thread* self = static_cast<Thread*>(arg);
        std::cout << "thread(ID : " << self->get_thread_id() << ") start...\n";
        try {
            self->thread_loop();
        } catch (const std::exception& e) {
            std::cerr << "[EXCEPT] thread exception: " << e.what() << '\n';
            self->thread_term_.store(true);
        }
        std::cout << "thread(ID : " << self->get_thread_id() << ") stop!!!\n";
        return nullptr;
    }

    // 파생 클래스에서 스레드 ID를 제공하도록 정의
    virtual std::string get_thread_id() const = 0;

public:
    virtual bool start_thread() = 0;
    virtual void stop_thread() = 0;

    bool get_thread_term() const { return thread_term_.load(); }
};