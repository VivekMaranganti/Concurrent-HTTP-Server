#include <iostream>
#include <queue>
#include <functional>
#include <vector>
#include <thread>

using namespace std;

class thread_pool {
    private:
        queue<function<void()>> tasks;
        mutex mutex_q;

        vector<thread> threads;
        condition_variable isSleep;

        bool isStop;

    public:
        thread_pool(int n) {
            isStop = false;
            //Create n threads
            for (int i = 0; i < n; i++) {   
                threads.emplace_back([this] {
                    while (true) {
                        function<void()> task;
                        {
                            //Ensures that only one thread can modify the tasks queue at a time
                            unique_lock<mutex> lock(mutex_q);

                            //Sleep and release the lock
                            isSleep.wait(lock, [this] {
                                //Wake up when one of the conditions is true
                                return !tasks.empty() || isStop;
                            });
                            if (isStop && tasks.empty()) {
                                return;
                            }
                            //Peek at the next task and pop it
                            task = move(tasks.front());
                            tasks.pop();
                        }
                        task();
                    }
                });
            }
        }

        //Deconstructor
        ~thread_pool() {
            {
                //Set stop flag
                unique_lock<mutex> lock(mutex_q);
                isStop = true;
            }
            //Wakes all threads so that they see isStop == true and exit
            isSleep.notify_all();
            
            //Join all threads
            for (int i = 0; i < threads.size(); i++) {
                threads[i].join();
            }
        }

        void enqueue(function<void()> task) {
            {
                //Push a task to the tasks queue
                unique_lock<mutex> lock(mutex_q);
                tasks.push(task);
            }
            //Wake one thread per task
            isSleep.notify_one();

        }
};