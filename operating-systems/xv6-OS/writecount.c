#include "types.h"
#include "stat.h"
#include "user.h"

int
main(int argc, char **argv)
{
  printf(1, "Current Word Count: %d \n", writecount());
  printf(2, "Write Count Reset \n");
  setwritecount(0);
  exit();
}