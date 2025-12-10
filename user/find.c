#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

char* fmtname(char *path)
{
  static char buf[DIRSIZ+1];
  char *p;

  // Find first character after last slash.
  for(p=path+strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;

    // Return name (null-terminated).  For safety don't return
    // blank-padded string for comparisons.
    if(strlen(p) >= DIRSIZ)
        return p;
    int n = strlen(p);
    memmove(buf, p, n);
    buf[n] = '\0';
    return buf;
}

void find(char *path,char *filename){
    int fd;
    struct stat st;
    struct dirent de;

    if((fd = open(path, 0)) < 0){
        fprintf(2, "find: cannot open %s\n", path);
        return;
    }
    if(fstat(fd, &st) < 0){
        fprintf(2, "find: cannot stat %s\n", path);
        close(fd);
        return;
    }
    

    switch(st.type){
    case T_FILE:
        if(strcmp(fmtname(path),filename)==0){
            printf("%s\n", path);
        }
        close(fd);
        break;
    case T_DIR:
        char buf[512], *p;
        if(strlen(path) + 1 + DIRSIZ + 1 > sizeof(buf)){
            printf("find: path too long %s\n", path);
            close(fd);
            return;
        }
        strcpy(buf, path);
        p = buf+strlen(buf);
        *p++ = '/';

        while(read(fd, &de, sizeof(de)) == sizeof(de)){
            if(de.inum == 0)
                continue;
            char name[DIRSIZ+1];
            memmove(name, de.name, DIRSIZ);
            name[DIRSIZ] = 0;
            // trim trailing spaces
            int nl = strlen(name);
            while(nl > 0 && name[nl-1] == ' ') name[--nl] = 0;
            if(nl == 0)
                continue;
            if(strcmp(name, ".") == 0 || strcmp(name, "..") == 0)
                continue;
            memmove(p, name, nl);
            p[nl] = 0;
            if(stat(buf, &st) < 0){
                printf("find: cannot stat %s\n", buf);
                continue;
            }
            find(buf,filename);
        }
        close(fd);
        break;
    }
}

int main(int argc,char *argv[]){
    if(argc!=3){
        fprintf(2,"usage: find . b (function name + path name +file name)\n");
        exit(1);
    }
    find(argv[1],argv[2]);
    exit(0);
}