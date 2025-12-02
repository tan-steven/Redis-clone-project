#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/ip.h>

static void msg(const char *msg) {
    fprintf(stderr, "%s\n", msg);
}

static void die(const char *msg) {
    int err = errno;
    fprintf(stderr, "[%d] %s\n", err, msg);
    abort();
}

//Read and write
static void do_something(int connfd) {
    char rbuf[64] = {};
    ssize_t n = read(connfd, rbuf, sizeof(rbuf) - 1);
    if (n < 0) {
      msg("read() error");
      return;
    }
    printf("client says: %s\n", rbuf);

    char wbuf[] = "world";
    write(connfd, wbuf, strlen(wbuf));
}


int main (){
  //This sets the socket handle
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0){
      die("socket()");
    };
  //settings the socket option. This is needed for most server applications
  //2nd & 3rd args sets optons, 4th sets option value. SO_REUSEADDR reuses the same
  //port as the last instance
    int val = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));

  //we're binding a wildcard address 0.0.0.0:1234 as a parameter for listen

    struct sockaddr_in addr ={};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(1234);
    addr.sin_addr.s_addr = htonl(0);
    int rv = bind(fd, (const struct sockaddr *)&addr, sizeof(addr));
    if (rv) {die("bind()");}

  // listen
  // SOMAXCONN is 4096 on Linux. The second argument for listen is the sizeof queue
    rv = listen(fd, SOMAXCONN);
    if (rv) {
      die("listen()");
    }
    while (true) {
    //accept
      struct sockaddr_in client_addr = {};
      socklen_t addrlen = sizeof(client_addr);
      int connfd = accept(fd, (struct sockaddr *)&client_addr, &addrlen);
      if (connfd < 0) {
        continue; //error
      }
      do_something(connfd);
      close(connfd);
    }
    return 0;
}
