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
    char magic[3];
    int header_size = 0;
    int version;
    char nr_of_sections;
    char sect_name[19];
    int sect_type;
    int sect_offset;
    int sect_size;



    lseek(fd, -4, SEEK_END);
   read(fd, &header_size, 2);
    read(fd, magic ,2);
    
    if(strcmp(magic, "BM") != 0){
        printf("ERROR\nwrong magic\n");
        close(fd);
        return -1;
    }
    

    magic[2] = '\0';
    lseek(fd, -header_size, SEEK_END);

    int read_version = read(fd, &version, 4);
    //printf("  versss %d", version);

    if(read_version == 4){
        if(version< 113 || version > 201){
            printf("ERROR\nwrong version\n");
            close(fd);
            return -1;
         }
   }

    int read_nr_sections = read(fd, &nr_of_sections, 1);
   // printf("nr of sectionss %d\n", (int)(nr_of_sections));
  if(read_nr_sections == 1){
    if (nr_of_sections < 7 || nr_of_sections > 16){
        printf("ERROR\nwrong sect_nr\n");
        close(fd);
        return -1;
    }
  }
    if(read_nr_sections < 1){
        printf("SUCCESS\n");
        close(fd);
        return -1;
    }

    for(int  i = 0; i < nr_of_sections; i++){
        read(fd, sect_name, 18);
        sect_name[18] = '\0';
        read(fd, &sect_type, 2);
        if(sect_type != 76 && sect_type != 28 && sect_type != 29 && sect_type != 87 && sect_type != 38){
            printf("ERROR\nwrong sect_types\n");
            close(fd);
            return -1;
        }

        read(fd, &sect_offset, 4);
        read(fd, &sect_size, 4);
    }
   
   read(fd, &header_size,2);
   read(fd,magic, 2);
   lseek(fd, -header_size, SEEK_CUR);
   lseek(fd, 5,SEEK_CUR);

    printf("SUCCESS\n");
    printf("version=%d\n", version);
    printf("nr_sections=%d\n", (int)(nr_of_sections));

    for (int i = 0; i < nr_of_sections; i++){
        read(fd, sect_name, 18);
        sect_name[18] = '\0';
        read(fd, &sect_type, 2);
        read(fd, &sect_offset, 4);
        read(fd, &sect_size, 4);
        printf("section%d: %s %d %d\n", i+1, sect_name, sect_type, sect_size);
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
