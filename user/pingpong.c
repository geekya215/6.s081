#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  int pid;
  int ctp[2];
  int ptc[2];
  char b = 'b';
  int status = 0;

  pipe(ctp);
  pipe(ptc);
  pid = fork();

  if (pid < 0) {
    close(ctp[0]);
    close(ctp[1]);
    close(ptc[0]);
    close(ptc[1]);
    fprintf(2, "fork error\n");
    exit(1);
  } else if (pid == 0) {
    close(ptc[1]);
    close(ctp[0]);

    if (read(ptc[0], &b, sizeof(char)) != sizeof(char)) {
      fprintf(2, "child read error\n");
      status = 1;
    } else {
      fprintf(1, "%d: received ping\n", getpid());
    }

    if (write(ctp[1], &b, sizeof(char)) != sizeof(char)) {
      fprintf(2, "child write error\n");
      status = 1;
    }

    close(ptc[0]);
    close(ctp[1]);

    exit(status);
  } else {   
    close(ctp[1]);
    close(ptc[0]);

    if (write(ptc[1], &b, sizeof(char)) != sizeof(char)) {
      fprintf(2, "parent write error\n");
      status = 1;
    }

    if (read(ctp[0], &b, sizeof(char)) != sizeof(char)) {
      fprintf(2, "parent read error\n");
      status = 1;
    } else {
      fprintf(1, "%d: received pong\n", getpid());
    }

    close(ctp[0]);
    close(ptc[1]);

    exit(status);
  }
}
