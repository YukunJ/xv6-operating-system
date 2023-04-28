#include "kernel/types.h"
#include "user/user.h"

/*
 * Run as a prime-number processor
 * the listenfd is from your left neighbor
 */
void runprocess(int listenfd) {
  int my_num = 0;
  int forked = 0;
  int passed_num = 0;
  int pipes[2];
  while (1) {
    int read_bytes = read(listenfd, &passed_num, 4);

    // left neighbor has no more number to provide
    if (read_bytes == 0) {
      close(listenfd);
      if (forked) {
        // wait my child termination
        int child_pid;
        wait(&child_pid);
      }
      exit(0);
    }

    // if my initial read
    if (my_num == 0) {
      my_num = passed_num;
      printf("prime %d\n", my_num);
    }

    // not my prime multiple, pass along
    if (passed_num % my_num != 0) {
      if (!forked) {
        pipe(pipes);
        forked = 1;
        int ret = fork();
        if (ret == 0) {
          // i am the parent
          close(pipes[0]);
        } else {
          // i am the child
          close(pipes[1]);
          close(listenfd);
          runprocess(pipes[0]);
        }
      }

      // pass the number to right neighbor
      write(pipes[1], &passed_num, 4);
    }
  }
}

int main(int argc, char *argv[]) {
  int pipes[2];
  pipe(pipes);
  for (int i = 2; i <= 35; i++) {
    write(pipes[1], &i, 4);
  }
  close(pipes[1]);
  runprocess(pipes[0]);
  exit(0);
}
