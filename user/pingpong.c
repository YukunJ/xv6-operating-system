#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
  int pid;
  int pipes1[2], pipes2[2];
  char buf[] = {'a'};
  pipe(pipes1);
  pipe(pipes2);

  int ret = fork();

  // parent send in pipes1[1], child receives in pipes1[0]
  // child send in pipes2[1], parent receives in pipes2[0]
  // should have checked close & read & write return value for error, but i am
  // lazy
  if (ret == 0) {
    // i am the child
    pid = getpid();
    close(pipes1[1]);
    close(pipes2[0]);
    read(pipes1[0], buf, 1);
    printf("%d: received ping\n", pid);
    write(pipes2[1], buf, 1);
    exit(0);
  } else {
    // i am the parent
    pid = getpid();
    close(pipes1[0]);
    close(pipes2[1]);
    write(pipes1[1], buf, 1);
    read(pipes2[0], buf, 1);
    printf("%d: received pong\n", pid);
    exit(0);
  }
}
