#include "sig_semiphor.h"

#define _GNU_SOURCE
#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <stdint.h>
#include <assert.h>

#define GO_SIG SIGUSR1
#define DONE_SIG SIGUSR2
#define WAIT_SIG SIGUSR3

static sigset_t GO_MASK;
static sigset_t SEMI_MASK;

/**
 * because of multicore caches we cant really be sure about the order of anything...
 * however we can be sure because of the OS that signals are not send twice.
 */
void* sync_thread(void* arg){
	int status = (int) arg;
	int wait_count = 0;

	for(;;){
		int sig = 0;
		assert(!waitsig(&SEMI_MASK,&sig));
		switch(sig){
		case DONE_SIG:
			/*status must be 0 here since you can only be done if the shared resource is owned by you*/
			assert(status==0);
			if(!wait_count){
				status=1;
			}
			else{
				wait_count-=1;
				assert(!kill(getpid(),GO_SIG));
			}
			break;
		case WAIT_SIG:
			if(status==1){
				status = 0;
				assert(!kill(getpid(),GO_SIG));
			}else{
				wait_count+=1;
			}
			break;
		}

	}
}

void sem_down(){
	int sig;
	assert(!kill(getpid(),WAIT_SIG));
	waitsig(&GO_MASK,&sig);

}
void sem_up(){
	assert(!kill(getpid(),DONE_SIG));
}

void sem_init(int status) {

	/*set up all the masks first so we dont acidently catch our signals with the defualt handler*/
    sigemptyset(&GO_MASK);
    sigaddset(&GO_MASK, GO_SIG);
    sigprocmask(SIG_BLOCK, &GO_MASK, NULL);


   	sigemptyset( &SEMI_MASK );
   	sigaddset( &SEMI_MASK, DONE_SIG);
   	sigaddset( &SEMI_MASK, WAIT_SIG);
   	sigprocmask( SIG_BLOCK, &SEMI_MASK, NULL );

   	/*lunch sercer*/
    pthread_t thread;
    if (pthread_create(&thread, NULL, sync_thread, (void*)(intptr_t)status) != 0) {
        perror("pthread_create");
        exit(1);
    }

    /*we dont need that thread so...*/
    pthread_detach(thread);
}
