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
static sigset_t GO_MASK;


void sem_down(){
	int sig;
	sigwait(&GO_MASK,&sig);

}
void sem_up(){
	assert(!kill(getpid(),GO_SIG));
}

void sem_init(int status) {

	/*set up all the masks first so we dont acidently catch our signals with the defualt handler*/
    sigemptyset(&GO_MASK);
    sigaddset(&GO_MASK, GO_SIG);
    sigprocmask(SIG_BLOCK, &GO_MASK, NULL);


   	/*save the locks state*/
   	if(status){
		assert(!kill(getpid(),GO_SIG));
   	}
}
