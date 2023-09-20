#include "life.h"
#include <pthread.h>

//Code Source: https://stackoverflow.com/questions/1352749/multiple-arguments-to-function-called-by-pthread-create
struct arg_struct {
    LifeBoard* even;
    LifeBoard* odd;
    int steps;
    int LOWER_ROW_BOUND;
    int UPPER_ROW_BOUND;
    pthread_barrier_t* barrier;
};

void* execute_thread(void* arguments) {
    //Retrieve Arguments Code Source: https://stackoverflow.com/questions/1352749/multiple-arguments-to-function-called-by-pthread-create
    struct arg_struct* args = (struct arg_struct*) arguments;

    LifeBoard* even = args->even;
    LifeBoard* odd = args->odd;
    int steps = args->steps;
    const int LOWER_ROW_BOUND = (const int) args->LOWER_ROW_BOUND;
    const int UPPER_ROW_BOUND = (const int) args->UPPER_ROW_BOUND;
    pthread_barrier_t* barrier = args->barrier;
    
    //Variables to update simulation state between steps
    LifeBoard* write_to = NULL; 
    LifeBoard* read_from = NULL;
    bool write_to_odd = true;

    for (int step = 0; step < steps; step++){

        //Define which board is current state and which board is next state
        write_to = write_to_odd ? odd : even;
        read_from = write_to_odd ? even : odd;

        //Preform Thread work
        for(int y = LOWER_ROW_BOUND; y <= UPPER_ROW_BOUND; y++){
            if (y <= 0 || y >= read_from->height()-1) continue; //bounds check
            for(int x = 1; x < read_from->width()-1; x++){
                /*Borrowed from Single Thread */
                int live_in_window = 0;
                for (int y_offset = -1; y_offset <= 1; y_offset ++){
                    if (y+y_offset < 0 || y+y_offset >= read_from->height()) continue; //bounds check
                    for (int x_offset = -1; x_offset <= 1; x_offset ++){
                        if (read_from->at(x + x_offset, y + y_offset)) {
                            ++live_in_window;
                        }
                    }
                }
                write_to->at(x, y) = (
                    live_in_window == 3 /* dead cell with 3 neighbors or live cell with 2 */ ||
                    (live_in_window == 4 && read_from->at(x, y)) /* live cell with 3 neighbors */
                );
                /*End Borrow from Single Thread*/
            }
        }
        //Setup next step and syncronize
        write_to_odd = !write_to_odd;
        pthread_barrier_wait(barrier); //All Finish Current Step
    }
    return NULL;
}

void simulate_life_parallel(int threads, LifeBoard &state, int steps) {
    if (steps <= 0) return; //itrival case
    //const to imporve efficency
    const int NUM_ROWS = state.height();

    //Setup Varriables
    LifeBoard* even = new LifeBoard(state);
    LifeBoard* odd = new LifeBoard(state);

    //Figure out how many rows each thread will do work on and how many threads are needed
    int num_rows_per_thread = std::max((NUM_ROWS/threads), 1);

    //Compute cutoffs for each thread as to which rows they will do work over
    //create exclusive upper bounds for each threads
    std::vector<int> cutoffs;
    int temp = 0;
    int num_threads = 0;
    cutoffs.push_back(0);
    for(int i = 1; i < NUM_ROWS; i++){
        temp++;
        if(temp >= num_rows_per_thread-1) {
            num_threads++;
            temp = 0;

            cutoffs.push_back(i);
            i++;
            cutoffs.push_back(i);
        }
    }
    num_threads++;
    cutoffs.push_back(NUM_ROWS);

    //Create threads and start work
    threads = std::min(num_threads, threads);//Correct the number of threads
    pthread_barrier_t* barrier = new pthread_barrier_t;
    pthread_barrier_init(barrier, NULL, (unsigned int) (threads+1)); //+1 for monitoring in controler thread

    int counter = 0;
    std::vector<struct arg_struct*> thread_arg_list(threads);
    std::vector<pthread_t*> thread_array(threads);
    for(int i = 0; i < threads; i++){
        pthread_t* cur_thread = new pthread_t;
        thread_array[i]= cur_thread;
        //Setup arguments for thread worker
        const int LOWER_ROW_BOUND = cutoffs[counter];
        const int UPPER_ROW_BOUND = i == threads-1 ? NUM_ROWS-1 : cutoffs[counter+1];
        counter +=2;

        struct arg_struct* args = new struct arg_struct;
        thread_arg_list[i] = args;
        args->even = even;
        args->odd = odd;
        args->steps = steps;
        args->LOWER_ROW_BOUND = LOWER_ROW_BOUND;
        args->UPPER_ROW_BOUND = UPPER_ROW_BOUND;
        args->barrier = barrier;
    
        int ret = pthread_create(cur_thread, NULL, &execute_thread, (void*) args);
        if (ret < 0) abort();

        //Stop making threads if all rows have been assigned a thread
        if (UPPER_ROW_BOUND == NUM_ROWS) break;
    }
    //Wait for threads to finish execution
    for (int step = 0; step < steps; step++) {
        pthread_barrier_wait(barrier); //finsih step
    }
    //Set final State of computation
    state = (steps % 2 == 0) ? *even : *odd;

    //deaollocate memory
    //Join Threads
    for(int i = 0; i < (int) thread_array.size(); i++){
        pthread_join(*thread_array[i], NULL);
        delete(thread_array[i]);
        delete(thread_arg_list[i]);
    }
    delete(even); delete(odd);
    pthread_barrier_destroy(barrier);
    delete(barrier);
}