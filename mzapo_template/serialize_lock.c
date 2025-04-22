#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#include "serialize_lock.h"

const char *serialize_lock_fname = "/run/lock/serialize_lock";
int serialize_lock_fd = -1;

int serialize_lock(int no_wait)
{
  int fd;

  fd = open(serialize_lock_fname,
    O_RDWR      |   /* open the file for both read and write access */
    O_CREAT     |   /* create file if it does not already exist */
    O_CLOEXEC   ,   /* close on execute */
    S_IRUSR     |   /* user permission: read */
    S_IWUSR     );  /* user permission: write */

  if (fd == -1)
    return -1;

  if (no_wait) {
    /* try to lock the "semaphore", if busy report that */
    if (lockf( fd, F_TLOCK, 0 ) == -1) {
      close(fd);
      return errno == EAGAIN? 0: -1;
    }
  } else  {
    /* lock the "semaphore", wait until available */
    if (lockf( fd, F_LOCK, 0 ) == -1)
      return -1;
  }

  serialize_lock_fd = fd;

  return 1;
}

void serialize_unlock(void)
{
  int fd = serialize_lock_fd;

  if (fd == -1)
    return;

  /* close() automatically releases the file lock */
  /* so technically the call with F_ULOCK is not necessary */
  lockf( fd, F_ULOCK, 0 );
  close( fd );
  serialize_lock_fd = -1;
}
