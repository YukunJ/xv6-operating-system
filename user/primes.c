/*
    * primes.c
    *
    *  Created on: 2025年12月9日
    * 
    * 存在着 访问指针越界的问题
    * 
    * 整体思路：
    * 1. 主进程 初始化2~36的数组，创建管道和子进程
    * 2. 主进程把数组数据写入管道
    * 3. 子进程读取管道数据，输出第一个素数
    * 4. 子进程过滤数据，保留非素数倍数的数据
    * 5. 子进程递归调用步骤1~4，直到没有数据
    * 6. 所有进程退出
    * 
*/


#include "user.h"
#include "stddef.h"
#include "kernel/types.h"

#define TOTAL_NUM 35  // 2~36 共35个数

void primes_detect(int read_fd);

void primes_init(int *nums,int len){
    int fd[2];
    if(pipe(fd)<0){
        fprintf(2,"pipe create error\n");
        exit(1);
    }
    int pid=fork();
    if(pid==0){
        close(fd[1]);
        primes_detect(fd[0]);
    }else{
        close(fd[0]);
        for(int i =0;i<len;i++){
            int written = 0 ;
            while(written<sizeof(int)){
                int ret = write(fd[1],&nums[i]+written,sizeof(int)-written);
                if(ret<0){
                    fprintf(2,"write error\n");
                    exit(1);
                }
                written+=ret;
            }
        }
    }
}

void primes_detect(int read_fd) {
    //把数据 接受过来 保留到子进程当中
    int nums[TOTAL_NUM],count=0;
    int ret;
    while((ret=read(read_fd,&nums[count],sizeof(int)))==sizeof(int)){
        count++;
        if(count>=TOTAL_NUM){
            break;
        }
    }
    close(read_fd);
    if(ret<0){
        fprintf(2,"read error\n");
        exit(1);
    }

    //终止条件
    if(count==0){
        exit(0);
    }
    //输出数据
    int prime =nums[0];
    printf("prime:%d\n",prime);
    //存储 过滤后的数据
    int filtered[TOTAL_NUM],filter_count=0;
    for(int i=1;i<count;i++){
        if(nums[i]%prime!=0){
            filtered[filter_count++]=nums[i];
        }
    }
    if(filter_count==0){
        exit(0);
    }
    primes_init(filtered,filter_count);
}

int main() {
    // 堆内存存储初始数组（避免栈溢出）
    int *num_ary = malloc(TOTAL_NUM * sizeof(int));
    if (num_ary == NULL) {
        fprintf(2, "malloc init nums failed\n");
        exit(1);
    }

    // 初始化2~36
    for (int i = 0; i < TOTAL_NUM; i++) {
        num_ary[i] = i + 2;
    }
    primes_init(num_ary,TOTAL_NUM);
    exit(0);
}
