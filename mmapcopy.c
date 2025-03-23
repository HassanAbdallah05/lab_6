#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
    char *buffer;
    int fd;
    struct stat info;
    size_t length;
    ssize_t s;
    pid_t pid; 
    int status;

    // Check if the file name is passed as an argument
    if (argc < 2) {
        printf("Error\n");
        exit(EXIT_FAILURE);
    }

    fd = open(argv[1], O_RDONLY);
    if (fd == -1) {
        perror("Error while opening file");
        exit(EXIT_FAILURE);
    }

    
    if (fstat(fd, &info) == -1) {          
        perror("Error while getting file information");
        close(fd);
        exit(EXIT_FAILURE);
    }

    if (info.st_size == 0) {
        fprintf(stderr, "Error: File %s is empty.\n", argv[1]);
        close(fd);
        exit(EXIT_FAILURE);
    }

   
    if ((pid = fork()) == 0) { 
        printf("Child process\n");

        
        buffer = mmap(NULL, info.st_size, PROT_READ, MAP_PRIVATE, fd, 0); 
        if (buffer == MAP_FAILED) {
            perror("Error while mapping file");
            close(fd);
            exit(EXIT_FAILURE);
        }


        if (write(STDOUT_FILENO, buffer, info.st_size) != info.st_size) {
            perror("Error writing to stdout");
            munmap(buffer, info.st_size);
            close(fd);
            exit(EXIT_FAILURE);
        }

        munmap(buffer, info.st_size);
        close(fd);
        printf("Child process: done\n");
        exit(EXIT_SUCCESS);
    }

    
    printf("Parent process\n");
    waitpid(pid, &status, 0);
    printf("Parent process: done\n");

    close(fd);
    exit(EXIT_SUCCESS);
}

