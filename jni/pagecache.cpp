#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#ifndef __linux__
typedef char mincore_element_type;
#else
typedef unsigned char mincore_element_type;
#endif

int check_mem(void* addr, int size){
    int page_size;
    int num_pages;
    mincore_element_type* page_vector;
    int total_in_mem;
    
    page_size = getpagesize();
    num_pages = size / page_size + (size % page_size == 0 ? 0 : 1);
    page_vector = (mincore_element_type*) malloc(num_pages);
    if (page_vector == NULL) { 
        printf("[-] malloc() for page_vector failed\n"); 
        return -1; 
    }
    if ( mincore(addr, size, page_vector) == -1 ) { 
        printf("[-] mincore() failed"); 
        return -1; 
    }

    total_in_mem = 0;
    for (int p = 0; p < num_pages; p++) { 
        if ( page_vector[p] & 1 ) {
            total_in_mem++; 
        }
    }

    printf("[+] %p (size=%d, page size=%d", addr, size, page_size);
    printf("[+] --> %d/%d = %d%% pages cached)", total_in_mem, num_pages, (100*total_in_mem) / num_pages);
 
    printf("[+] : ");
    for (int p = 0; p < num_pages; p++) {
        if( page_vector[p] & 1 ){
            printf("1 ");
        }else{
            printf("0 ");
        }
    }   
    printf("\n");
    
    free(page_vector);
    
    return 0;
}

int main(int argc, char** argv)
{
    const char* file_name = "/system/lib/libc.so";
    int fd;
    struct stat stat_buffer;
    void* mapped_mem;
    int mmap_size = 0x1000;
    
    
    //file
    fd = open(file_name, O_RDONLY);
    if ( fd == -1 ) {
        printf("[-] open() failed\n");
        return -1;
    }
    
    if (fstat(fd, &stat_buffer) == -1) { 
        printf("[-] fstat() failed\n"); 
        return -1; 
    }
    if (stat_buffer.st_size == 0) { 
        printf("[-] %s: File is empty\n", file_name); 
        return -1; 
    }

    mapped_mem = mmap(NULL, stat_buffer.st_size, PROT_NONE, MAP_FILE | MAP_PRIVATE, fd, 0);
    if (mapped_mem == MAP_FAILED) { 
        printf("mmap() failed 1\n"); 
        return -1; 
    }
    
    check_mem(mapped_mem, stat_buffer.st_size);
    
    munmap(mapped_mem, stat_buffer.st_size);
    close(fd);
    
    //mem
    mapped_mem = mmap(NULL, mmap_size, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (mapped_mem == MAP_FAILED) { 
        printf("mmap() failed 2\n"); 
        return -1; 
    }
    
    check_mem(mapped_mem, mmap_size);
    *((int*)mapped_mem) = 0x123456;
    
    check_mem(mapped_mem, mmap_size);
    
    munmap(mapped_mem, mmap_size);

	return 0;
}
