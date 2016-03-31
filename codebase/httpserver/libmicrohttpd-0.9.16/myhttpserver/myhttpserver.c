#include <sys/types.h>
#ifndef _WIN32
#include <sys/select.h>
#include <sys/socket.h>
#else
#include <winsock2.h>
#endif
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
#define FILENAME "params.txt"
#define MIMETYPE "text/plain"

#define SHMSZ 4096


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
    char buffer[5];

    //share set channel variable start
    int ShmID_chShare;
    char *ShmPTR_chShare;
    key_t MyKey_chShare;
    MyKey_chShare = 5555;
    ShmID_chShare   = shmget(MyKey_chShare, sizeof(char), IPC_CREAT | 0666);
    ShmPTR_chShare  = (char *) shmat(ShmID_chShare, NULL, 0);
    //share set channel variable end


    printf("answer to connection\n");
    printf("url = %s \n",url);

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
        //ShmID   = shmget(MyKey, sizeof(pid_t), 0666);
        if ((ShmID_getchar   = shmget(MyKey_getchar, sizeof(char), 0666)) < 0) {
            perror("shmget");
            exit(1);
        }

        ShmPTR_getchar = (char *) shmat(ShmID_getchar, NULL, 0);
        channel = *ShmPTR_getchar;
        printf("Received channel from hostapd = %c\n", channel);
        buffer[0] = channel;
        response = MHD_create_response_from_data(3, "OK\n", MHD_NO, MHD_NO);
        //response =
        //  MHD_create_response_from_fd_at_offset (sbuf.st_size, fd, 0);
        //MHD_add_response_header (response, "Content-Type", MIMETYPE);
        //ret = MHD_queue_response (connection, MHD_HTTP_OK, response);
        //MHD_destroy_response (response);
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
        response = MHD_create_response_from_data(3, "OK\n", MHD_NO, MHD_NO);
    }

    /*
    //access shared memory start<

    key = 5678;

    if ((shmid = shmget(key, SHMSZ, 0666)) < 0) {
        perror("shmget");
        exit(1);
    }

    if ((shm = shmat(shmid, NULL, 0)) == (char *) -1) {
        perror("shmat");
        exit(1);
    }

    printf("Received char string is %s\n",shm);

    size = strlen(shm);
    printf("Performing memory copy from shm to buffer");
    memcpy(buffer, shm, size);

    printf("Size of the string = %d\n", size);

    //*shm = '*';

    //printf("First character of the shared memory changed\n");

    //access shared memory end>
    */

    /*if ( (-1 == (fd = open (FILENAME, O_RDONLY))) ||
         (0 != fstat (fd, &sbuf)) )
    {
      
        if (fd != -1)
	  (void) close (fd);
        const char *errorstr =
          "<html><body>An internal server error has occured!\
                              </body></html>";
        response =
	  MHD_create_response_from_buffer (strlen (errorstr),
					 (void *) errorstr,
					 MHD_RESPMEM_PERSISTENT);
        if (NULL != response)
        {
            ret =
              MHD_queue_response (connection, MHD_HTTP_INTERNAL_SERVER_ERROR,
                                response);
            MHD_destroy_response (response);

            return ret;
        }
        else
            return MHD_NO;
    }

    response = MHD_create_response_from_buffer(size, buffer, MHD_RESPMEM_PERSISTENT);

    //response =
    //  MHD_create_response_from_fd_at_offset (sbuf.st_size, fd, 0);
    MHD_add_response_header (response, "Content-Type", MIMETYPE);*/
    ret = MHD_queue_response (connection, MHD_HTTP_OK, response);
    MHD_destroy_response (response);
    return ret;
}


int
main ()
{

    printf("In main\n");
    struct MHD_Daemon *daemon;


    daemon = MHD_start_daemon (MHD_USE_SELECT_INTERNALLY | MHD_USE_DEBUG | MHD_USE_POLL, PORT, NULL, NULL,
                             &answer_to_connection, (void *) PAGE, MHD_OPTION_END);
    if (NULL == daemon)
      return 1;
    
    //pid_t pid;
    key_t MyKey;
    int ShmID;
    pid_t *ShmPTR;

    MyKey   = 1234;
    //ShmID   = shmget(MyKey, sizeof(pid_t), 0666);
    if ((ShmID   = shmget(MyKey, sizeof(pid_t), 0666)) < 0) {
        perror("shmget");
        exit(1);
    }


    ShmPTR  = (pid_t *) shmat(ShmID, NULL, 0);
    pid     = *ShmPTR;                

    printf("Received pid is %d\n",pid);
    /*printf("Sending signal to hostapd_cli...\n");
    kill(pid, SIGINT);
    printf("Channel set command Signal sent to hostapd_cli\n");
    sleep(1);
    kill(pid, SIGQUIT);
    printf("Channel get command Signal sent to hostapd_cli\n");*/

    (void) getchar ();

    //*shm = '*';

    MHD_stop_daemon (daemon);

    return 0;
}
