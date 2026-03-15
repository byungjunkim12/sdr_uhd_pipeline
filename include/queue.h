#pragma once
#include <mutex>
#include <condition_variable>

template <typename T>
class IQQueue {
private:
    std::unique_ptr<T[]>    buffer_;
    size_t                  capacity_;
    size_t                  size_;
    size_t                  head_;
    size_t                  tail_;
    bool                    done_ = false;
    std::mutex              mu_;
    std::condition_variable cv_;

public:
    IQQueue(size_t capacity) :
        capacity_(capacity),
        size_(0),
        head_(0),
        tail_(0) { buffer_ = std::make_unique<T[]>(capacity); }
    
    IQQueue(IQQueue&& other) :
        capacity_(other.capacity_),
        size_(other.size_),
        head_(other.head_),
        tail_(other.tail_){
        buffer_ = std::move(other.buffer_); // needed?
        other.capacity_ = 0;
        other.size_ = 0;
        other.head_ = 0;
        other.tail_ = 0;
    }

    void push(const T& item){
        std::unique_lock<std::mutex> lk(mu_);
        cv_.wait(lk, [this] { return size_ < capacity_; });
        buffer_[tail_] = item;
        tail_ = (tail_ + 1) % capacity_;
        size_++;
        cv_.notify_one();
    }    

    void push(T&& item){
        std::unique_lock<std::mutex> lk(mu_);
        cv_.wait(lk, [this] { return size_ < capacity_; });
        buffer_[tail_] = std::move(item);
        tail_ = (tail_ + 1) % capacity_;
        size_++;
        cv_.notify_one();
    }
    
    bool pop(T& item){
        std::unique_lock<std::mutex> lk(mu_);
        cv_.wait(lk, [this] { return !empty() || done_; });
        if (empty()) return false;
        item = buffer_[head_];
        head_ = (head_ + 1) % capacity_;
        size_--;
        cv_.notify_one();
        return true;
    }

    bool empty() { return size_ == 0; }
    
    void set_done() {
        std::lock_guard<std::mutex> lk(mu_);
        done_ = true;
        cv_.notify_all();
    }
    
    bool is_done() { return done_; }
};
