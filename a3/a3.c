#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <sys/mman.h>
#include <string.h>
#define FIFO1 "RESP_PIPE_64529"
#define FIFO2 "REQ_PIPE_64529"


int main(int argc, char** argv){

    char connect[8] = {0x07, 0x43, 0x4f, 0x4e, 0x4e, 0x45, 0x43, 0x54};
    char request[4] = {0x50, 0x49, 0x4E, 0x47};
    char request2[4] = {0x50, 0x4f, 0x4e, 0x47};
    unsigned int variant = 64529;
    
    char succ[] = { 0x53, 0x55, 0x43, 0x43,  0x45, 0x53,  0x53};
    char err[] = { 0x45,  0x52,  0x52, 0x4f, 0x52};
    unsigned char err_size = 5;
    unsigned char succ_size = 7;


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

        

        //while(true){
        unsigned char received_size;

        if(read(fd2, &received_size, sizeof(unsigned char)) == 1){
         
            char received[received_size];
            
            read(fd2, received, received_size);
           // received[received_size] = '\0';
             
            //  if(strcmp(received, "EXIT") == 0){
            //     break;
            // }
           

            if(strncmp(received, "PING",4) == 0){
                write(fd1, &received_size, 1);
                write(fd1, request, sizeof(request));
                write(fd1, &variant, sizeof(unsigned int));
                write(fd1, &received_size, sizeof(unsigned char));
                write(fd1, request2, sizeof(request2));
            }

            if(strncmp(received, "CREATE_SHM", 10)){
                unsigned int size_shm;
                read(fd2, &size_shm, sizeof(unsigned int));
                int fd3 = shm_open("/51xp1coC", O_CREAT | O_RDWR, 0644);
                if(fd3 < 0) {
                    write(fd1, &received_size, sizeof(unsigned char));
                    write(fd1, received, sizeof(received));
                     write(fd1, &err_size, sizeof(unsigned char));
                     write (fd1, err, sizeof(err));
                    
                }

                ftruncate(fd3, size_shm);
                volatile unsigned char *sharedChar = (volatile unsigned char*)mmap(NULL, sizeof(size_shm), PROT_READ | PROT_WRITE, MAP_SHARED, fd3, 0);
                 if(sharedChar != (void*)-1){
                     write(fd1, &received_size, sizeof(unsigned char));
                    write(fd1, received, sizeof(received));
                     write(fd1, &succ_size, sizeof(unsigned char));
                     write (fd1, succ, sizeof(succ));
                    
                 }else{
                     write(fd1, &received_size, sizeof(unsigned char));
                    write(fd1, received, sizeof(received));
                     write(fd1, &err_size, sizeof(unsigned char));
                     write (fd1, err, sizeof(err));
                    
                 }

            }
        }

        // }



     //  }



        
        

close(fd1);
close(fd2);
unlink(FIFO1);
unlink(FIFO2);

return 0;

    }


    


