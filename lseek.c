#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

int
main(int argc, char *argv[])
{
  int fd, ret, n;
  char buf[128];
  int offset, whence;

  if(argc != 4){
    printf(1, "Usage: %s <filename> <offset> <whence>\n", argv[0]);
    printf(1, "whence: 0 (SEEK_SET), 1 (SEEK_CUR), 2 (SEEK_END)\n");
    exit();
  }

  // Convert command-line arguments to integers.
  offset = atoi(argv[2]);
  whence = atoi(argv[3]);

  // Open the file in read-only mode.
  fd = open(argv[1], O_RDONLY);
  if(fd < 0){
    printf(1, "Error: cannot open %s\n", argv[1]);
    exit();
  }

  // Call lseek with the given offset and whence.
  ret = lseek(fd, offset, whence);
  printf(1,"Returned from seek system call.\n");
  if(ret < 0){
    printf(1, "lseek failed\n");
    // cprintf("Lsek system call failed.\n");
    close(fd);
    exit();
  }
  printf(1, "After lseek: new file offset = %d\n", ret);

  // Read data starting at the new offset.
  n = read(fd, buf, sizeof(buf)-1);
  if(n < 0){
    printf(1, "read failed\n");
    // cprintf("File reading failed.\n");
    close(fd);
    exit();
  }
  buf[n] = '\0';
  printf(1, "Data read from file:\n%s\n", buf);

  close(fd);
  exit();
}
