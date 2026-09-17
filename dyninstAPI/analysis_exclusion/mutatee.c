/* One mutatee, three behaviours:
 *   (none)  print and exit promptly          -- create, rewrite
 *   wait    print and stay alive             -- attach
 *   dlopen  stay alive, then dlopen plugins  -- dlopen
 */
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <dlfcn.h>

extern int toc_marker(void);
extern int toc_get(void);


int main(int argc, char **argv)
{
    const char *mode = (argc > 1) ? argv[1] : "run";
    printf("    mutatee: marker=%d get=%#x\n", toc_marker(), toc_get());
    fflush(stdout);

    if (!strcmp(mode, "dlopen"))  {
        sleep(2);                          /* let the mutator attach first */
        void *a = dlopen("./libplugA.so", RTLD_NOW);
        void *b = dlopen("./libplugB.so", RTLD_NOW);
        printf("    mutatee: dlopen A=%s B=%s\n", a?"ok":"fail", b?"ok":"fail");
        fflush(stdout);
    }
    if (!strcmp(mode, "wait") || !strcmp(mode, "dlopen"))  {
        for (int i = 0; i < 60; i++)
            sleep(1);
    }  else  {
        sleep(1);
    }
    return 0;
}
