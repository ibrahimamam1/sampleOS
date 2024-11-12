#include <proc.h>
#include <stdio.h>
#include <syscall.h>

int main(int argc, char **argv)
{
    unsigned int val = 200;
    unsigned int *addr = (unsigned int *) 0xe0000000;

    printf("pong started.\n");
    printf("pong: the value at address %x: %d\n", addr, *addr);
    printf("pong: writing the value %d to the address %x\n", val, addr);
    *addr = val;
    yield();
    printf("pong: the new value at address %x: %d\n", addr, *addr);

    return 0;
}
