#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

int
main(void)
{
  int fd, ret1,ret2,ret3, n;
  char buf1[128],buf2[128],buf3[128];
  int offset1,offset2,offset3, whence1,whence2,whence3;
  char *filename = "testfile.txt"; // Hard-coded file name

  // Hard-code the parameters.
  offset1 = 5; 
  offset2 = -3;
  offset3 = -5;      // For example, seek to 10 bytes from the start
  whence1 = SEEK_SET; // Using SEEK_SET (from the beginning)
  whence2 = SEEK_CUR;
  whence3 = SEEK_END;

  // Open the file in read-only mode.
  fd = open(filename, O_RDONLY);
  if(fd < 0){
    printf(1, "Error: cannot open %s\n", filename);
    exit();
  }

  // Call lseek with the given offset and whence for SEEK SET
  ret1 = lseek(fd, offset1, whence1);
  printf(1, "Returned from lseek system call.\n");
  if(ret1 < 0){
    printf(1, "lseek failed\n");
    close(fd);
    exit();
  }
  printf(1, "After lseek: new file offset = %d\n", ret1);

  // Read data starting at the new offset.
  n = read(fd, buf1, sizeof(buf1)-1);
  if(n < 0){
    printf(1, "read failed\n");
    close(fd);
    exit();
  }
  buf1[n] = '\0';
  printf(1, "Data read from file for SEEK SET:%s\n", buf1);

  // Call lseek with the given offset and whence for SEEK CUR
  ret2 = lseek(fd, offset2, whence2);
  printf(1, "Returned from lseek system call.\n");
  if(ret2 < 0){
    printf(1, "lseek failed\n");
    close(fd);
    exit();
  }
  printf(1, "After lseek: new file offset = %d\n", ret2);

  // Read data starting at the new offset.
  n = read(fd, buf2, sizeof(buf2)-1);
  if(n < 0){
    printf(1, "read failed\n");
    close(fd);
    exit();
  }
  buf2[n] = '\0';
  printf(1, "Data read from file for SEEK CUR:%s\n", buf2);

  // Call lseek with the given offset and whence for SEEK END
  ret3 = lseek(fd, offset3, whence3);
  printf(1, "Returned from lseek system call.\n");
  if(ret3 < 0){
    printf(1, "lseek failed\n");
    close(fd);
    exit();
  }
  printf(1, "After lseek: new file offset = %d\n", ret2);

  // Read data starting at the new offset.
  n = read(fd, buf3, sizeof(buf2)-1);
  if(n < 0){
    printf(1, "read failed\n");
    close(fd);
    exit();
  }
  buf3[n] = '\0';
  printf(1, "Data read from file for SEEK END:%s\n", buf3);


  close(fd);
  exit();
}
