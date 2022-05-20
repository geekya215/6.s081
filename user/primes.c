#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

/*
 *   2   -----------       -----------       -----------       -----------       -----------
 * ----->  print 2  |     |           |     |           |     |           |     |           |
 *   3  |           |  3  |           |     |           |     |           |     |           |
 * ----->           ------>  print 3  |     |           |     |           |     |           |
 *   4  |           |     |           |     |           |     |           |     |           |
 * ----->    drop   |     |           |     |           |     |           |     |           |
 *   5  |           |  5  |           |  5  |           |     |           |     |           |
 * ----->           ------>           ------>  print 5  |     |           |     |           |
 *   6  |           |     |           |     |           |     |           |     |           |
 * ----->    drop   |     |           |     |           |     |           |     |           |
 *   7  |           |  7  |           |  7  |           |  7  |           |     |           |
 * ----->           ------>           ------>           ------>  print 7  |     |           |
 *   8  |           |     |           |     |           |     |           |     |           |
 * ----->    drop   |     |           |     |           |     |           |     |           |
 *   9  |           |  9  |           |     |           |     |           |     |           |
 * ----->           ------>    drop   |     |           |     |           |     |           |
 *  10  |           |     |           |     |           |     |           |     |           |
 * ----->    drop   |     |           |     |           |     |           |     |           |
 *  11  |           | 11  |           | 11  |           | 11  |           | 11  |           |
 * ----->           ------>           ------>           ------>           ------>  print 11 |
 *       -----------       -----------       -----------       -----------       -----------
 */

#define R 0
#define W 1
#define MAX_NUMBER 35

int lpipe_first(int lpipe[2], int *data)
{
  if (read(lpipe[R], data, sizeof(int)) == sizeof(int)) {
    printf("prime %d\n", *data);
    return 0;
  }
  return -1;
}

void filtrate_data(int lpipe[2], int rpipe[2], int lfirst)
{
  int temp;
  while (read(lpipe[R], &temp, sizeof(int)) == sizeof(int)) {
    if (temp % lfirst) {
      write(rpipe[W], &temp, sizeof(int));
    }
  }
  close(lpipe[R]);
  close(rpipe[W]);
}

void sieve(int lpipe[2])
{
  close(lpipe[W]);
  int first;
  if (lpipe_first(lpipe, &first) == 0) {
    int rpipe[2];
    pipe(rpipe);
    filtrate_data(lpipe, rpipe, first);

    if (fork() == 0) {
      sieve(rpipe);
    } else {
      close(rpipe[R]);
      wait(0);
    }
  }
  exit(0);
}

int
main(int argc, char *argv[])
{
  int init_pipe[2];
  pipe(init_pipe);

  for (int i = 2; i <= MAX_NUMBER; i++) {
    write(init_pipe[W], &i, sizeof(int));
  }

  if (fork() == 0) {
    sieve(init_pipe);
  } else {
    close(init_pipe[R]);
    close(init_pipe[W]);
    wait(0);
  }

  int data;
  lpipe_first(init_pipe, &data);

  exit(0);
}
