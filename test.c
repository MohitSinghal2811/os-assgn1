#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>
#include "DLL_invoker.c"
#include <string.h>



int dll_function_test1(){
    struct request req = {
        .args = "1",
        .file_name = "/lib/x86_64-linux-gnu/libm.so.6",
        .function_name = "exp"
    };
    char* res = "2.718282";
    char* ans = dll_function(req);
    printf("%s ", ans);
    return (strcmp(res, ans) == 0);
}

char * printtest(int result){
    if(result == 1){
        return ("\nTest passed\n");
    }
    else{
        return ("\nTest failed\n");
    }
}

int main(){
    int test1_result = dll_function_test1();
    printf("%s" ,printtest(test1_result));
}


