#ifndef POOL_H_
#include <string>
#include <pthread.h>
#include <semaphore.h>
#include <deque>
#include <map>
#include <string>
#include <vector>

using namespace std;

class Task {
public:
    sem_t* task_has_completed_sem;

    Task();
    virtual ~Task();

    virtual void Run() = 0;  // implemented by subclass
};

class ThreadPool {
public:
    pthread_mutex_t* shared_data_lock;
    pthread_cond_t* task_to_consume_cv;
    sem_t* tasks_have_been_assigned_sem;
    map<string, Task*>* task_lookup;
    map<string, pthread_cond_t*>* task_completed_cv_lookup;
    deque<string>* task_queue;
    vector<pthread_t*>* threads;
    vector<struct thread_args*>* thread_args;

    ThreadPool(int num_threads);

    // Submit a task with a particular name.
    void SubmitTask(const std::string &name, Task *task);
 
    // Wait for a task by name, if it hasn't been waited for yet. Only returns after the task is completed.
    void WaitForTask(const std::string &name);

    // Stop all threads. All tasks must have been waited for before calling this.
    // You may assume that SubmitTask() is not caled after this is called.
    void Stop();
};
#endif
