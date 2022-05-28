#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SV_SOCK_PATH_0 "/tmp/sandbox/socket"
#define BUF_SIZE 100

int main(int argc, char *argv[]) {
  struct sockaddr_un addr;
  ssize_t num_read;
  char buf[BUF_SIZE];

  int sfd = socket(AF_UNIX, SOCK_STREAM, 0);
  printf("Client socket fd = %d\n", sfd);

  if (sfd == -1) {
    printf("Socket file descriptor is not 'legit'");
    return 1;
  }

  memset(&addr, 0, sizeof(struct sockaddr_un));
  addr.sun_family = AF_UNIX;
  strncpy(addr.sun_path, SV_SOCK_PATH_0, sizeof(addr.sun_path) - 1);

  if (connect(sfd, (struct sockaddr *) &addr, sizeof(struct sockaddr_un)) == -1) {
    printf("Unable to connect");
    return 10;
  }

  while ((num_read = read(STDIN_FILENO, buf, BUF_SIZE)) > 0) {
    if (write(sfd, buf, num_read) != num_read) {
      printf("Partial failed write");
      return 101;
    }

    if (num_read == -1) {
      printf("Read error");
      return 102;
    }

  }

  return 0;
}
