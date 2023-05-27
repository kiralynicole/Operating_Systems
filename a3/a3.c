#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <string.h>
#define FIFO1 "RESP_PIPE_64529"
#define FIFO2 "REQ_PIPE_64529"


int main(int argc, char** argv){

    char connect[8] = {0x07, 0x43, 0x4f, 0x4e, 0x4e, 0x45, 0x43, 0x54};

    if(mkfifo(FIFO1, 0600) != 0){
        printf("EROR\ncannot create the response pipe");
        return 1;
    }

        int fd1 = -1, fd2 = -1;

        fd2 = open(FIFO2, O_RDONLY);
        if(fd2 == -1){
            printf("ERROR\ncannot open the request pipe");
            return -1;
        }

        fd1 = open(FIFO1, O_WRONLY);
        if(fd1 == -1){
            printf("ERROR\ncould not open the response pipe");
            return 1;
        }

        if(write(fd1, connect, sizeof(connect)) > 0 ){
            printf("SUCCESS\n");
        }


close(fd1);
close(fd2);
unlink(FIFO1);

return 0;

    }


