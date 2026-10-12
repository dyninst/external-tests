#include <stdio.h>

// Defined in libdef.so: imported, so the executable gets a PLT stub for it.
int plt_target(int x);

// Defined here: the control.  A lookup of this name must find this definition
// whatever incUninstrumentable says, and must never find a stub.
int local_only(int x)
{
    return x * 2;
}

int main(void)
{
    printf("%d %d\n", plt_target(41), local_only(21));
    return 0;
}
