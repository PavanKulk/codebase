#include <sys/types.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <microhttpd.h>
#include <time.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>

#include <signal.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define PAGE "<html><head><title>Error</title></head><body>Bad data</body></html>"

#define PORT 8888
#define MIMETYPE "text/plain"

int shmid;
key_t key;
char *shm, *s;
size_t size;
pid_t pid;

static int
answer_to_connection (void *cls, struct MHD_Connection *connection,
                      const char *url, const char *method,
                      const char *version, const char *upload_data,
                      size_t *upload_data_size, void **con_cls)
{
    struct MHD_Response *response;
    int fd;
    int ret;
    struct stat sbuf;
    char *buffer;

    //share set channel variable start
    int ShmID_chShare;
    char *ShmPTR_chShare;
    key_t MyKey_chShare;
    MyKey_chShare = 5555;
    ShmID_chShare   = shmget(MyKey_chShare, sizeof(char), IPC_CREAT | 0666);
    ShmPTR_chShare  = (char *) shmat(ShmID_chShare, NULL, 0);
    //share set channel variable end


    //printf("answer to connection\n");
    //printf("url = %s \n",url);

    if (0 != strcmp (method, "GET"))
      return MHD_NO;

    if(!strcmp(url, "/get")) {
        
        char channel;
        printf("GET channel called\n");
        if(kill(pid, SIGQUIT) != 0) {
            printf("Failed to send SIGQUIT signal\n");
        }
        printf("Channel get command Signal sent to hostapd_cli\n");
        sleep(1);

        key_t MyKey_getchar;
        int ShmID_getchar;
        char *ShmPTR_getchar;
        MyKey_getchar   = 7878;
        if ((ShmID_getchar   = shmget(MyKey_getchar, sizeof(char), 0666)) < 0) {
            perror("shmget");
            exit(1);
        }
        ShmPTR_getchar = (char *) shmat(ShmID_getchar, NULL, 0);
        channel = *ShmPTR_getchar;
        printf("Received channel from hostapd = %c\n", channel);

        response = MHD_create_response_from_data(3, ShmPTR_getchar, MHD_NO, MHD_NO);
    }

    if(strstr(url, "set") != NULL) {

        printf("SET channel called\n");

        char channel = url[10];
        *ShmPTR_chShare = channel;
        printf("Sending signal to hostapd_cli...\n");
        if(kill(pid, SIGINT) != 0) {
            printf("Failed to send SIGINT signal\n");
        }
        printf("Channel set command Signal sent to hostapd_cli\n");

        response = MHD_create_response_from_data(3, "OK", MHD_NO, MHD_NO);
    }

    ret = MHD_queue_response (connection, MHD_HTTP_OK, response);
    MHD_destroy_response (response);
    return ret;
}


int
main ()
{

    //printf("In main\n");
    struct MHD_Daemon *daemon;


    daemon = MHD_start_daemon (MHD_USE_SELECT_INTERNALLY | MHD_USE_DEBUG | MHD_USE_POLL, PORT, NULL, NULL,
                             &answer_to_connection, (void *) PAGE, MHD_OPTION_END);
    if (NULL == daemon)
      return 1;
    
    key_t MyKey;
    int ShmID;
    pid_t *ShmPTR;
    MyKey   = 1234;
    if ((ShmID   = shmget(MyKey, sizeof(pid_t), 0666)) < 0) {
        perror("shmget");
        exit(1);
    }
    ShmPTR  = (pid_t *) shmat(ShmID, NULL, 0);
    pid     = *ShmPTR;                
    printf("Received pid is %d\n",pid);

    while(1);

    MHD_stop_daemon (daemon);

    return 0;
}
