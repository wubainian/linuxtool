#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdint.h>
#include <errno.h>
#include <pthread.h>
#include <sys/inotify.h>

#define PROC_MEM_FILE "/proc/self/mem"

void* _notify_thread(void* param){
    int handle;
    int watch;
    struct inotify_event event;
    int n_read;
    int mask;
    
    handle = inotify_init();
    if( handle < 0 ){
        printf("[-] failed to inotify_init, err=%d(%s)\n", errno, strerror(errno));
        return NULL;
    }
    watch = inotify_add_watch(handle, PROC_MEM_FILE, IN_ACCESS | IN_MODIFY | IN_CLOSE_WRITE | IN_CLOSE_NOWRITE | IN_OPEN );
    if( watch < 0 ){
        printf("[-] failed to inotify_add_watch, err=%d(%s)\n", errno, strerror(errno));
        return NULL;
    }
    
    while( (n_read = read(handle, &event, sizeof(event)) ) > 0 ){
        mask = event.mask;
        if( mask & IN_OPEN ){
            printf("[+] file is opened ****** \n");
        }else if( (mask & IN_CLOSE_WRITE) || (mask & IN_CLOSE_NOWRITE) ){
            printf("[+] file is closed ****** \n");
        }else if( mask & IN_MODIFY ){
            printf("[+] file is modified ****** \n");
        }else if( mask & IN_ACCESS ){
            printf("[+] file is accessed ****** \n");
        }
        
    }
    
    close(handle);
    return NULL;
}


void start_notify_thread(){
    pthread_t pid;
    
    if( pthread_create(&pid, NULL, _notify_thread, NULL) ){
        printf("[-] failed to start thread\n");
    }
}

void modify_var(void* addr, uint32_t new_value){
    int fd;
    uint64_t cur;
    int write_len;
    int read_len;
    uint32_t old_value;
    
    fd = open(PROC_MEM_FILE, O_RDWR);
    
    if( -1 == fd ){
        printf("[-] failed to open mem file\n");
        return;
    }
    
    //read
    if( -1 == lseek64(fd, (off64_t)addr, SEEK_SET) ){
        printf("[-] failed to lseek 1\n");
        goto end_label;
    }
    
    cur = lseek64(fd, 0, SEEK_CUR);
    printf("[+] addr=%p, cur=0x%llx\n", addr, cur);
    
    if( ( read_len = read(fd, &old_value, sizeof(old_value)) ) < 0 ){
        printf("[-] failed to read");
        goto end_label;
    }
    printf("[+] old_value=0x%x\n", old_value);
    
    //write
    if( -1 == lseek64(fd, (off64_t)addr, SEEK_SET) ){
        printf("[-] failed to lseek 2\n");
        goto end_label;
    }
    
    cur = lseek64(fd, 0, SEEK_CUR);
    printf("[+] addr=%p, cur=0x%llx\n", addr, cur);
    
    write_len = write(fd, &new_value, sizeof(new_value));
    if( sizeof(new_value) != write_len ){
        printf("[-] failed to write new_value, write_len=%d, err=%d(%s)\n", write_len, errno, strerror(errno));
        goto end_label;
    }

end_label:
    close(fd);
}

static uint32_t var = 0x12345678;

int main(int argc, char* argv[]){
    //start notify thread
    start_notify_thread();
    
    //wait notify thread
    sleep(2);
    
    //before modify
    printf("[+] var=0x%x\n", var);
    
    //modify
    modify_var(&var, 0x87654321);
    
    //after modify
    printf("[+] var=0x%x\n", var);
    
    return 0;
}