#include <stdio.h>
#include<stdlib.h>
#include <string.h>
#include <sys/types.h>
#include<fcntl.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdbool.h>

bool invalid(const char *path){
    const char*invalidC = "<>";
return strpbrk(path, invalidC) != NULL;
}

void list_rec(const char *path, bool permission, int size){
    DIR *dir = NULL;
    struct dirent *entry = NULL;
    dir = opendir(path);
    char fullPath[1024];
    struct stat statBuf;


    if(dir == NULL) {
      // printf("ERROR\ninvalid directory path\n");
       return;
           }

   while((entry = readdir(dir)) != NULL) {
    if(strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name , "..") !=0 && entry->d_name[0]!='.' && !invalid(entry->d_name)){
         snprintf(fullPath, 1024, "%s/%s", path, entry->d_name);

        if(lstat(fullPath, &statBuf) == 0) {
            bool check_size = (size<0) || (S_ISREG(statBuf.st_mode) && statBuf.st_size > size);
          bool check_permission =  (permission == 0) || (statBuf.st_mode & S_IXUSR);
         if(check_permission && check_size){
            if(S_ISDIR(statBuf.st_mode)){
                 printf("%s\n", fullPath);
                list_rec(fullPath, permission, size);
            }else{
                        printf("%s\n", fullPath);
                         list_rec(fullPath, permission, size);
                }
        }
        else if(S_ISDIR(statBuf.st_mode)){
            list_rec(fullPath, permission, size);
        }
    }
   }
}
closedir(dir);
}



int list(const char *path, bool permission, int size)
 {
    DIR *dir = NULL;
    struct dirent *entry = NULL;
    dir = opendir(path);
    char fullPath[1024];
    struct stat statBuf;


    if(dir == NULL) {
       printf("ERROR\ninvalid directory path\n");
       return -1;
           }

   while((entry = readdir(dir)) != NULL) {
    snprintf(fullPath, 1024, "%s/%s", path, entry->d_name);
    if(strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name , "..") != 0 && entry->d_name[0]!='.' && !invalid(entry->d_name)){
         //snprintf(fullPath, 512, "%s/%s", path, entry->d_name);

        if(lstat(fullPath, &statBuf) == 0) {
           bool check_size = (size<0) || (S_ISREG(statBuf.st_mode) && statBuf.st_size > size);
            bool check_permission =  (permission == 0) || (statBuf.st_mode & 0100);

        if(check_permission && check_size){
            if(S_ISDIR(statBuf.st_mode)){
                        printf("%s/%s\n", path, entry->d_name);

            }else{
                        printf("%s/%s\n", path, entry->d_name);
        }
     }
        }
   }
   }

    closedir(dir);
    return 0;
   
 }

 int parse(int fd){
    char magic[3] = {0};
    char header_size[3] = {0};
    char version[5] = {0};
    char nr_of_sections;
    char sect_name[19]={0};
    char sect_type[3]={0};
    char sect_offset[5]={0};
    int sect_size;


    lseek(fd, -4, SEEK_END);
    int read_header =read(fd, header_size, 2);
    if(read_header < 2){
        printf("SUCCESS\n");
        close(fd);
        return -1;
    }
    header_size[2] = '\0';
    int read_magic = read(fd, magic ,2);
    if(strcmp(magic, "BM") != 0){
        printf("ERROR\nwrong magic\n");
        close(fd);
        return -1;
    }
    else if(read_magic < 2){
        printf("SUCCESS\n");
        close(fd);
        return -1;
    }
    magic[2] = '\0';
    lseek(fd, -read_header, SEEK_CUR);
    int read_version = read(fd, &version, 4);
    if(read_version == 4){
        if(atoi(version)< 113 || atoi(version) > 201){
            printf("ERROR\nwrong version\n");
            close(fd);
            return -1;
         }
    }
   version[4] = '\0';

    int read_nr_sections = read(fd, &nr_of_sections, 1);
   if(read_nr_sections == 1){
    if (nr_of_sections < 7 || nr_of_sections > 16){
        printf("ERROR\nwrong sect_nr\n");
        close(fd);
        return -1;
    }
    if(read_nr_sections < 1){
        printf("SUCCESS\n");
        close(fd);
        return -1;
    }

    for(char i = 0; i < nr_of_sections; i++){
        read(fd, sect_name, 18);
        sect_name[19] = '\0';
        read(fd, sect_type, 2);
        if(atoi(sect_type) != 76 && atoi(sect_type) != 28 && atoi(sect_type) != 29 && atoi(sect_type) != 87 && atoi(sect_type) != 38){
            printf("ERROR\nwrong sect_types\n");
        }

        sect_type[3] = '\0';
        read(fd, sect_offset, 4);
        sect_offset[5] = '\0';

        read(fd, &sect_size, 4);
    }
   }


   close(fd);
   return 0;
    
 }
 


int main(int argc, char **argv){
    if(argc >= 2){
        if(strcmp(argv[1], "variant") == 0){
            printf("64529\n");
        }
        
       else if(strcmp(argv[1], "list") == 0){
            int i = 2, s = -1;
            bool recursive = 0, permission = 0;
            char*path = NULL;
            struct stat statBuf;

            while(i < argc){
                if(strcmp(argv[i], "recursive") == 0){
                    recursive = 1;
                }else if(strncmp(argv[i],"size_greater=", 13) == 0){
                    s = atoi(argv[i]+13);
                }
                else if(strcmp(argv[i], "has_perm_execute") == 0){
                    permission = 1;
                }else if(strncmp(argv[i], "path=", 5) == 0){
                    path = argv[i] + 5;
                }
                i++;
            }

            if(path== NULL || stat(path, &statBuf) < 0 || !S_ISDIR(statBuf.st_mode)){
                    printf("ERROR\ninvalid directory path\n");
            }
                else{
                    printf("SUCCESS\n");
                    if(recursive == 0)
                    list(path, permission, s);
                   else list_rec(path, permission, s);
                }

        }

        else if(strcmp(argv[1], "parse") == 0){
            char* filepath = NULL;
            if(strncmp(argv[2], "path=", 5) == 0)
           filepath = argv[2] + 5;
           else{
            printf("ERROR\nfile_path not specified");
           }
           
           int fd = -1;
           fd = open(filepath, O_RDONLY);
          if(fd == -1) {
             printf("ERROR\ninvalid file_path");
             return 1;
            }

            parse(fd);
        }

    }
    return 0;
}
