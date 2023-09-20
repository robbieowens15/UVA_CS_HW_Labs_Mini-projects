#include "pool.h"
#include <iostream> //? Remove in final implimentation
#include <pthread.h>
#include <semaphore.h>
#include <deque>
#include <map>
#include <string>
#include <vector>

using namespace std;

/*
Struct passed on thread initalization
with arguments to setup thread.
as well as arguments can be updated by
controller thread to add new tasks
*/
struct thread_args {
    pthread_mutex_t* shared_data_lock;
    deque<string>* task_queue;
    map<string, Task*>* task_lookup;
    pthread_cond_t* task_to_consume_cv;
    sem_t* tasks_have_been_assigned_sem;
    bool terminate;
};

void* worker_thread(void* arguments){
    struct thread_args* args = (struct thread_args*) arguments;
    Task* task = NULL;
    string task_name = "";
    
    while(true){ /*Consume and execute task loop*/
        pthread_mutex_lock(args->shared_data_lock);
        while(args->task_queue->empty() && args->terminate == false){
            pthread_cond_wait(args->task_to_consume_cv, args->shared_data_lock);
        }
        //Other terminate case
        if (args->terminate == true && args->task_queue->empty()) {
            pthread_mutex_unlock(args->shared_data_lock);
            break;
        }
        task_name = args->task_queue->front();
        cout << "Consumer Task Lookup" << endl;
        task = args->task_lookup->at(task_name);
        args->task_queue->pop_front();
        sem_post(args->tasks_have_been_assigned_sem);
        pthread_mutex_unlock(args->shared_data_lock);

        task->Run();

        task = NULL;
        task_name = "";

        //Should thread terminate? Is there no more work to do?
        pthread_mutex_lock(args->shared_data_lock);
        if (args->terminate) {
            pthread_mutex_unlock(args->shared_data_lock);
            break;
        }
        pthread_mutex_unlock(args->shared_data_lock);
    }
    return NULL;
}

Task::Task() {
    task_has_completed_sem = new sem_t;
    sem_init(task_has_completed_sem, 0, 0);
}

Task::~Task() {
    sem_destroy(this->task_has_completed_sem);
    delete(task_has_completed_sem);
}

ThreadPool::ThreadPool(int num_threads) {
    /*Inintialize obj varrible values*/
    //Syncronization
    shared_data_lock = new pthread_mutex_t;
    pthread_mutex_init(shared_data_lock, NULL);
    task_to_consume_cv = new pthread_cond_t;
    pthread_cond_init(task_to_consume_cv, NULL);
    tasks_have_been_assigned_sem = new sem_t;
    sem_init(tasks_have_been_assigned_sem, 0, 1); //STOP is the 1 additional wait

    //Shared Data
    task_lookup = new map<string, Task*>;
    task_queue = new deque<string>;

    //Bookkeeping
    threads = new vector<pthread_t*>;
    thread_args = new vector<struct thread_args*>;

    /*Create Threads*/
    for(int i = 0; i < num_threads; i ++){
        struct thread_args* default_arguments = new struct thread_args;
        default_arguments->shared_data_lock = shared_data_lock;
        default_arguments->task_queue = task_queue;
        default_arguments->task_lookup = task_lookup;
        default_arguments->task_to_consume_cv = task_to_consume_cv;
        default_arguments->tasks_have_been_assigned_sem = tasks_have_been_assigned_sem;
        default_arguments->terminate = false;

        pthread_t* new_thread = new pthread_t;
        threads->push_back(new_thread);
        thread_args->push_back(default_arguments);
        pthread_create(new_thread, NULL, &worker_thread, (void*) default_arguments);
    }
}

void ThreadPool::SubmitTask(const std::string &name, Task* task) {
    pthread_mutex_lock(this->shared_data_lock);
    sem_post(task->task_has_completed_sem);
    this->task_queue->push_back(name);
    cout << "Producer Task Submision" << endl;
    this->task_lookup->insert(make_pair(name, task));
    pthread_cond_broadcast(this->task_to_consume_cv);
    pthread_mutex_unlock(this->shared_data_lock);
}

void ThreadPool::WaitForTask(const std::string &name) {
    //todo Waits until a task has been marked as completed - use task specific semaphore
    pthread_mutex_lock(this->shared_data_lock);
    cout << "Wait for Task Lookup" << endl;
    Task* task = this->task_lookup->at(name); 

    sem_wait(task->task_has_completed_sem);
    sem_wait(this->tasks_have_been_assigned_sem); //Task no longer assigned (important for STOP)

    delete(task);
    this->task_lookup->erase(name);
    pthread_mutex_unlock(this->shared_data_lock);
}

void ThreadPool::Stop() {
    /*Wait until all tasks have been assigned and completed*/
    sem_wait(this->tasks_have_been_assigned_sem); //Will hold until all tasks have completed
    /*Mark all threads to terminate and join then*/
    for(int i = 0; i < (int) this->threads->size(); i++){
        pthread_mutex_lock(this->shared_data_lock); //!Deadlock with Consume Task?
        this->thread_args->at(i)->terminate = true;
        pthread_cond_broadcast(this->task_to_consume_cv);
        pthread_mutex_unlock(this->shared_data_lock);
        pthread_join(*this->threads->at(i), NULL);
        delete(this->threads->at(i));
        delete(this->thread_args->at(i));
    }
    /*Cleanup: Deallocate all memeory on heap*/
    pthread_mutex_destroy(this->shared_data_lock);
    pthread_cond_destroy(this->task_to_consume_cv);
    sem_destroy(this->tasks_have_been_assigned_sem);
    delete(this->shared_data_lock);
    delete(this->task_to_consume_cv);
    delete(this->tasks_have_been_assigned_sem);
    delete(this->task_lookup);
    delete(this->task_queue);
    delete(this->threads);
    delete(this->thread_args);
}
