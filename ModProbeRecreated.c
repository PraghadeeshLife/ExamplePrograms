#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/utsname.h>

int main(int argc, char* argv[]){
    char* point;
    struct stat fileinfo;
    struct utsname buffer;
    char module_path[512];
    uname(&buffer);
    sprintf(module_path,"/lib/modules/%s/modules.dep",buffer.release);
    

    int fd = open(module_path, O_RDONLY);
    if(fd == -1){
        perror("File Opening Failed");
        return 1;
    }
    if(fstat(fd, &fileinfo) == -1){
        perror("Fstat Failed");
        return 2;
    }

    printf("File Size is %ld \n", fileinfo.st_size);
    char* filebuffer = (char*) malloc(fileinfo.st_size);

    read(fd, filebuffer, fileinfo.st_size);

    //mpls_router
    point = strstr(filebuffer,argv[1]);
    char* token = strtok(point, ": ");
    int count = 0;
    while(token != NULL && count < 1){
        printf("%s \n", token);
        token = strtok(NULL, ": ");
        count++;
    }
    char* cleanedstring = strtok(token, "\n");
    printf("The dependency is %s \n", cleanedstring);
    
    char* modulename = strtok(cleanedstring, "/");
    char module[256];
     while(modulename != NULL){
        strcpy(module, modulename);
        modulename = strtok(NULL, "/");
    }
    printf("The dependency module is %s \n", module);

    char commandbuff[1024];
    sprintf(commandbuff, "sudo insmod %s", module);
    printf("Execution %s \n", commandbuff);
    system(commandbuff);
    
    sprintf(commandbuff, "sudo insmod %s.ko", argv[1]);
    printf("Executing Dependent Module %s \n", commandbuff);
    system(commandbuff);

    free(filebuffer);
    return 0;
}
