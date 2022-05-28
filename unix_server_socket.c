#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SV_SOCK_PATH "/tmp/sandbox/socket"
#define BUF_SIZE 100

int main(int argc, char *argv[]) {
  struct sockaddr_un addr;

  int sfd = socket(AF_UNIX, SOCK_STREAM, 0);
  printf("Server socket fd = %d\n", sfd);
  
  if (sfd == -1) {
    printf("Socket file descriptor is not 'legit'");
    return 1;
  }

  if (strlen(SV_SOCK_PATH) > sizeof(addr.sun_path) - 1) {
    printf("Server socket path too long: %s\n", SV_SOCK_PATH);
    return 10;
  }

  if (remove(SV_SOCK_PATH) == -1 && errno != ENOENT) {
    printf("Remove file at %s\n", SV_SOCK_PATH);
    return 100;
  }

  memset(&addr, 0, sizeof(struct sockaddr_un));
  addr.sun_family = AF_UNIX;
  strncpy(addr.sun_path, SV_SOCK_PATH, sizeof(addr.sun_path) -1);

  if (bind(sfd, (struct sockaddr *) &addr, sizeof(struct sockaddr_un)) == -1) {
    printf("Unable to bind socket\n");
    return 101;
  }

  if (listen(sfd, 5) == -1) {
    printf("Unable to listen for socket\n");
    return 102;
  }

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
      return 103;
    }

    if (close(cfd) == -1) {
      printf("Socket closed");
      return 104;
    }
  }

  return 0;
}
