#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <pthread.h>

#define SV_SOCK_PATH_0 "/tmp/sandbox/socket_0"
#define SV_SOCK_PATH_1 "/tmp/sandbox/socket_1"
#define SV_SOCK_PATH_2 "/tmp/sandbox/socket_2"
#define BUF_SIZE 100

int open_socket(char path[]) {

    struct sockaddr_un addr;

    int sfd = socket(AF_UNIX, SOCK_STREAM, 0);
    printf("Server socket fd = %d\n", sfd);

    if (sfd == -1) {
        printf("Socket file descriptor is not 'legit'");
        exit(1);
    }

    if (remove(path) == -1 && errno != ENOENT) {
        printf("Remove file at %s\n", path);
        exit(100);
    }

    memset(&addr, 0, sizeof(struct sockaddr_un));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, path, sizeof(addr.sun_path) - 1);

    if (bind(sfd, (struct sockaddr *) &addr, sizeof(struct sockaddr_un)) == -1) {
        printf("Unable to bind socket\n");
        exit(101);
    }

    if (listen(sfd, 5) == -1) {
        printf("Unable to listen for socket\n");
        exit(102);
    }

    return sfd;
}

void accept_socket(int sfd) {
    ssize_t num_read;
    char buf[BUF_SIZE];
    for (;;) {

        printf("Waiting to accept a connection...\n");

        int cfd = accept(sfd, NULL, NULL);
        printf("Accepted socket fd = %d\n", cfd);

        while ((num_read = read(cfd, buf, BUF_SIZE)) > 0) {
            if (write(STDOUT_FILENO, buf, num_read) != num_read) {
                printf("Partial failed write");
            }
        }

        if (num_read == -1) {
            printf("Read error");
            return;
        }

        if (close(cfd) == -1) {
            printf("Socket closed");
            return;
        }
    }
}

void *socket_0() {
    int sfd_0 = open_socket(SV_SOCK_PATH_0);
    accept_socket(sfd_0);
    pthread_exit(NULL);
}

void *socket_1() {
    int sfd_1 = open_socket(SV_SOCK_PATH_1);
    accept_socket(sfd_1);
    pthread_exit(NULL);
}

void *socket_2() {
    int sfd_2 = open_socket(SV_SOCK_PATH_2);
    accept_socket(sfd_2);
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    pthread_t tid_0;
    pthread_t tid_1;
    pthread_t tid_2;

    pthread_create(&tid_0, NULL, socket_0, NULL);
    pthread_create(&tid_1, NULL, socket_1, NULL);
    pthread_create(&tid_2, NULL, socket_2, NULL);
    pthread_join(tid_0, NULL);
    pthread_join(tid_1, NULL);
    pthread_join(tid_2, NULL);
}
