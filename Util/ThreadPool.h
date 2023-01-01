#include <functional>
#include <condition_variable>
#include <iostream>
#include <queue>
#include <thread>
#include <memory>
#include <future>
#include <functional>
#include <deque>
class ThreadPool {

private:
  std::vector<std::thread> threads;
  std::vector<std::function<void()>> tasks;
  std::deque<std::function<void()>>work;
  std::mutex mutex;
  std::condition_variable cond_var;
  std::atomic<bool> stop;


public:


  ThreadPool(size_t num_threads){
    for(int i=0;i<num_threads;++i){
      threads.emplace_back(std::thread(&ThreadPool::idleLoop,this));
    } 
  }
  ~ThreadPool(){
    shut_down();
  }

  void idleLoop(){
    //Nothing yet;
    while(true){
      std::unique_lock lock(mutex);
      cond_var.wait(lock,[this](){
          return !work.empty() || stop;
          });
      if(stop)
        break;
      auto task = work.front();
      work.pop_front();
      lock.unlock();
      task();
    }
  }

  template <typename Func, typename... Args>
  auto submit(Func && func, Args &&...args) {
    // do something here
    //some funny buisness with packaged task
    auto task  =std::make_shared<std::packaged_task<decltype(func(args...))(Args...)>>(func);
    std::function<void()> wrapper = [&args...,task](){
      task->operator()(std::forward<Args>(args)...);
    };
    work.emplace_back(wrapper);
    cond_var.notify_one();
    return task->get_future();
  }


  void shut_down(){
    stop =  true;
    cond_var.notify_all();
    for(auto&th : threads){
      th.join();
    }
  }

};
