#include "kernel/types.h"
#include "user/user.h"



int main(int argc,char *argv[]){

	if(argc!=2){
		fprintf(2,"Usage: 'command sleep 'only need one parameter (example:sleep 10)\n");
		exit(1);
	}
	
	int ticks = atoi(argv[1]);
	int ret = sleep(ticks);
	exit(ret);
}
