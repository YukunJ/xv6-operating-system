#include "kernel/param.h"
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  int i;
  char *nargv[MAXARG];

  if(argc < 3 || (argv[1][0] < '0' || argv[1][0] > '9')){
    fprintf(2, "Usage: %s mask command\n", argv[0]);
    exit(1);
  }

  //atoi 是把字符串转换成整数的函数
  if (trace(atoi(argv[1])) < 0) {
    fprintf(2, "%s: trace failed\n", argv[0]);
    exit(1);
  }

  //例如shell中命令是 trace 2147483647 grep hello README
  //把grep hello README 保存到 nargv 数组 当中


  for(i = 2; i < argc && i < MAXARG; i++){
    nargv[i-2] = argv[i];
  }
 
  //用grep hello README去替换当前指令
  exec(nargv[0], nargv);
  exit(0);
}
