#include "user.h"
#include "kernel/types.h"

int main(){
	
	int fd[2];
	char buf[1024];
	if(pipe(fd)<0){
		fprintf(2,"create pipe error,cannot create pipe normally \n");
		exit(2);
	}
	if(fork()==0){
		close(fd[1]);
		read(fd[0],buf,sizeof(buf));
		close(fd[0]);
		printf("%d: received ping\n",getpid());
		exit(0);
	}
	else{
		
		close(fd[0]);
		write(fd[1],"hello xv6 pipe\n",15);
		close(fd[1]);
		wait(0);
		printf("%d: received pong\n",getpid());
		exit(0);
	}
}



