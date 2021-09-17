#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>
#include <string.h>


struct request{
    char * file_name;
    char * function_name;
    char * args;
};

char* dll_function(struct request req) {
    void *handle;
    char *error;
    handle = dlopen (req.file_name,  RTLD_LAZY);
    if (!handle) {
        fputs (dlerror(), stderr);
        exit(1);
    }
    char ans[500];
    char *var;
    printf("The result is ");


    if(strcmp(req.function_name, "cos") == 0 || strcmp(req.function_name, "sin") == 0 || strcmp(req.function_name, "tan") == 0 || strcmp(req.function_name, "acos") == 0 || strcmp(req.function_name, "asin") == 0 || strcmp(req.function_name, "atan") == 0 || strcmp(req.function_name, "cosh") == 0 || strcmp(req.function_name, "sinh") == 0 || strcmp(req.function_name, "tanh") == 0 || strcmp(req.function_name, "exp") == 0 || strcmp(req.function_name, "log") == 0){
        double(*function) ();
        function = dlsym(handle, req.function_name);
        if ((error = dlerror()) != NULL)  {
            fputs(error, stderr);
            exit(1);
        }
        sprintf(ans, "%f", (*function) (atof(req.args)));
        dlclose(handle);
        var = ans;
        return var;
    }
    else{
        return "Function not supported";
    }
}




// "/lib/x86_64-linux-gnu/libm.so.6"