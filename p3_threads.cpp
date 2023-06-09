#include "p3_threads.h"
#include "utils.h"

extern pthread_mutex_t mutex;
extern pthread_mutex_t taskDoneMutex;

extern pthread_cond_t resume[4];
extern pthread_cond_t init[4];
extern pthread_cond_t a_task_is_done;
extern pthread_cond_t preempt_task;
extern std::vector<int> ready_queue;
extern int running_thread;
extern int preempt;
extern int num_of_alive_tasks;

extern int global_work; // DO NOT INITIALIZE!!

void *threadfunc(void *param)
{
	ThreadCtrlBlk* tcb = (ThreadCtrlBlk*) param;
	int iter = 1;
	int id = tcb->id;
	int fail = 0;
	bool first = true;

	printf(" - [Thread %d] Started\n", id);

	// work loop
	while (true) {
		pthread_mutex_lock(&mutex);

		// if we are globally finished, end the thread
		if (global_work == 0) {
			pthread_mutex_unlock(&mutex);
			break;
		}

		// add the thread to the ready queue
		ready_queue.push_back(id);

		printf("[%6lu ms][Thread %d] Ready to be scheduled\n", get_time_stamp(), id);

		// tell main process that the thread is ready
		if (first) {
			first = false;
			pthread_cond_signal(&init[id]);
		}

		// goes into "sleep" and waits for a signal from scheduler
		while (true) {
			pthread_cond_wait(&resume[id], &mutex);

			// start working if no other thread is working
			if (running_thread == -1) break;
		}

		// if we are globally finished, end the thread
		if (global_work == 0) {
			pthread_mutex_unlock(&mutex);

			// exit work loop
			break;
		}

		// at this point, the thread was scheduled and is now running
		running_thread = id;
		pthread_mutex_unlock(&mutex);

		// if we are globally finished, end the thread
		if (global_work == 0) {
finished:
			pthread_mutex_lock(&mutex);
finished_nolock:
			running_thread = -1;
			pthread_mutex_unlock(&mutex);

			pthread_mutex_lock(&taskDoneMutex);
			pthread_cond_signal(&a_task_is_done);
			pthread_mutex_unlock(&taskDoneMutex);

			if (preempt) {
				preempt = 0;
				pthread_cond_signal(&preempt_task);
			}

			// exit work loop
			break;
		}

		// start the task
		printf("[%6lu ms][Thread %d] Starting task   (iteration %d) taking %4lu ms\n", get_time_stamp(), id, iter, tcb->task_time);

		// run in 100 ms chunks to support "preemption"
		for (int i = 0; i < tcb->task_time / 100; i++) {
			if (preempt) {
				printf("[%6lu ms][Thread %d] Thread preempted\n", get_time_stamp(), id);

				pthread_mutex_lock(&mutex);
				preempt = 0;
				running_thread = -1;
				ready_queue.push_back(id);
				printf("[%6lu ms][Thread %d] Ready to be scheduled\n", get_time_stamp(), id);
				pthread_cond_signal(&preempt_task);
				while (true) {
					pthread_cond_wait(&resume[id], &mutex);

					if (global_work == 0) goto finished_nolock;
					// start working if no other thread is working
					if (running_thread == -1) break;
				}
				running_thread = id;
				pthread_mutex_unlock(&mutex);
				printf("[%6lu ms][Thread %d] Restarting task (iteration %d) with   %4lu ms remaining\n", get_time_stamp(), id, iter, (tcb->task_time - (i * 100)));
			}
			usleep(MSEC(100));
		}

		long end_time = get_time_stamp();

		// task is completed
		printf("[%6lu ms][Thread %d] Completed task  (iteration %d) taking %4lu ms\n", end_time, id, iter, tcb->task_time);

		// check if missed deadline (50ms slack)
		if (end_time > (tcb->deadline + 50)) {
			printf("[%6lu ms][Thread %d] Task (iteration %d) missed its deadline!! (Deadline: %lu ms)\n", end_time, id, iter, tcb->deadline);
			fail = 1;

			goto finished;
		}

		// find amount of time to wait to sync with the next iteration's period
		int sleep = iter * tcb->period - get_time_stamp();
		iter++;

		// set deadline for next iteration and change running_thread to availble
		pthread_mutex_lock(&mutex);
		tcb->deadline = iter * tcb->period;
		running_thread = -1;
		pthread_mutex_unlock(&mutex);

		// signal that the task is done for this iteration
		pthread_mutex_lock(&taskDoneMutex);
		pthread_cond_signal(&a_task_is_done);
		pthread_mutex_unlock(&taskDoneMutex);

		if (preempt) {
			preempt = 0;
			pthread_cond_signal(&preempt_task);
		}

		// sleep until synced with next period
		if(sleep > 0) {
			usleep(MSEC(sleep));
		}
	}

	printf(" - [Thread %d] Complete\n", id);
	if (fail) {
		num_of_alive_tasks--;
		printf(" - [Thread %d] Missed deadline\n", id);
	}

	return NULL;
}
