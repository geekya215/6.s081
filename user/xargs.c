#include "kernel/types.h"
#include "user/user.h"
#include "kernel/param.h"

#define BUF_SIZE 32

char buf[BUF_SIZE];
char *xargv[MAXARG];
int xargc;

void xargs(int argc, char *argv[])
{

  char *p = buf;
  xargc = 0;

  for (int i = 1; i < argc; i++) {
    xargv[xargc++] = argv[i];
  }

  int idx = 0;
  int n = read(0, buf, BUF_SIZE);

  while (n > 0) {
    idx += n;
    for (int i = 0; i < idx; i++) {
      if (buf[i] == '\n') {
        buf[i] = 0;
        xargv[xargc++] = p;
        xargv[xargc] = 0;
        xargc = argc - 1;
        if (fork() == 0) {
          exec(xargv[0], xargv);
        }
        p = &buf[i + 1];
        wait(0);
      }
      if (buf[i] == ' ') {
        buf[i] = 0;
        xargv[xargc++] = p;
        p = &buf[i + 1];
      }
    }
    n = read(0, &buf[idx], BUF_SIZE);
  }
}

int main(int argc, char *argv[])
{
  if (argc - 1 > MAXARG) {
    fprintf(2, "xargs: too much args\n");
    exit(1);
  }

  xargs(argc, argv);

  exit(0);
}
