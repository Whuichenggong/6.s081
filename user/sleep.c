#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
    if (argc < 2 || argc > 2 ) 
    {
    write(2,"only 1 arguments place write again\n",36);
    exit(1);
    }
    sleep(atoi(argv[1]));
    exit(0);
}
