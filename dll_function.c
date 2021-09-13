#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>

int main(int argc, char **argv) {
    void *handle;
    double (*cosine)();
    char *error;
    // printf("HI");

    handle = dlopen ("/lib/x86_64-linux-gnu/libm.so.6", RTLD_LAZY);
    if (!handle) {
        fputs (dlerror(), stderr);
        exit(1);
    }

    cosine = dlsym(handle, "atan2");
    if ((error = dlerror()) != NULL)  {
        fputs(error, stderr);
        exit(1);
    }
    double* arr = malloc(sizeof(double) * 2);
    arr[0] = 1;
    arr[1] = 3;

    printf ("%f\n", (*cosine)(*arr, *(arr + 1)));
    dlclose(handle);
}