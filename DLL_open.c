#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dlfcn.h>
#include <assert.h>


char * DLL_func(char * message){
	// Separate message to get library, function and arguments
	char** tokens;
    tokens = split_string(message, '|');

	// Checking the library used
	if(strcmp(*tokens, "libm.so.6")==0){
		void *handle;
		handle = dlopen ("libm.so.6", RTLD_LAZY);
		if (!handle) {
			fputs (dlerror(), stderr);
			exit(1);
		}

		// Checking the funtion in the library
		// Using the function to calulate result
		// Returning the result 
		if(strcmp(*(tokens+1), "cos") == 0){
			double (*cosine)(double);
			char *error;
			cosine = dlsym(handle, "cos");
			if ((error = dlerror()) != NULL)  {
				fputs(error, stderr);
				exit(1);
			}
			static char res[100];
			gcvt((*cosine)(atof(*(tokens+2))), 6, res);
			dlclose(handle);
			return res;
		}else if(strcmp(*(tokens+1), "sin") == 0){
			double (*sine)(double);
			char *error;
			sine = dlsym(handle, "sin");
			if ((error = dlerror()) != NULL)  {
				fputs(error, stderr);
				exit(1);
			}
			static char res[100];
			gcvt((*sine)(atof(*(tokens+2))), 6, res);
			dlclose(handle);
			return res;
		}else if(strcmp(*(tokens+1), "tan") == 0){
			double (*sine)(double);
			char *error;
			sine = dlsym(handle, "tan");
			if ((error = dlerror()) != NULL)  {
				fputs(error, stderr);
				exit(1);
			}
			static char res[100];
			gcvt((*sine)(atof(*(tokens+2))), 6, res);
			dlclose(handle);
			return res;
		}else if(strcmp(*(tokens+1), "acos") == 0){
			double (*sine)(double);
			char *error;
			sine = dlsym(handle, "acos");
			if ((error = dlerror()) != NULL)  {
				fputs(error, stderr);
				exit(1);
			}
			static char res[100];
			gcvt((*sine)(atof(*(tokens+2))), 6, res);
			dlclose(handle);
			return res;
		}else if(strcmp(*(tokens+1), "asin") == 0){
			double (*sine)(double);
			char *error;
			sine = dlsym(handle, "asin");
			if ((error = dlerror()) != NULL)  {
				fputs(error, stderr);
				exit(1);
			}
			static char res[100];
			gcvt((*sine)(atof(*(tokens+2))), 6, res);
			dlclose(handle);
			return res;
		}else if(strcmp(*(tokens+1), "atan") == 0){
			double (*sine)(double);
			char *error;
			sine = dlsym(handle, "atan");
			if ((error = dlerror()) != NULL)  {
				fputs(error, stderr);
				exit(1);
			}
			static char res[100];
			gcvt((*sine)(atof(*(tokens+2))), 6, res);
			dlclose(handle);
			return res;
		}else if(strcmp(*(tokens+1), "cosh") == 0){
			double (*sine)(double);
			char *error;
			sine = dlsym(handle, "cosh");
			if ((error = dlerror()) != NULL)  {
				fputs(error, stderr);
				exit(1);
			}
			static char res[100];
			gcvt((*sine)(atof(*(tokens+2))), 6, res);
			dlclose(handle);
			return res;
		}else if(strcmp(*(tokens+1), "sinh") == 0){
			double (*sine)(double);
			char *error;
			sine = dlsym(handle, "sinh");
			if ((error = dlerror()) != NULL)  {
				fputs(error, stderr);
				exit(1);
			}
			static char res[100];
			gcvt((*sine)(atof(*(tokens+2))), 6, res);
			dlclose(handle);
			return res;
		}else if(strcmp(*(tokens+1), "tanh") == 0){
			double (*sine)(double);
			char *error;
			sine = dlsym(handle, "tanh");
			if ((error = dlerror()) != NULL)  {
				fputs(error, stderr);
				exit(1);
			}
			static char res[100];
			gcvt((*sine)(atof(*(tokens+2))), 6, res);
			dlclose(handle);
			return res;
		}else if(strcmp(*(tokens+1), "exp") == 0){
			double (*sine)(double);
			char *error;
			sine = dlsym(handle, "exp");
			if ((error = dlerror()) != NULL)  {
				fputs(error, stderr);
				exit(1);
			}
			static char res[100];
			gcvt((*sine)(atof(*(tokens+2))), 6, res);
			dlclose(handle);
			return res;
		}else if(strcmp(*(tokens+1), "log") == 0){
			double (*sine)(double);
			char *error;
			sine = dlsym(handle, "log");
			if ((error = dlerror()) != NULL)  {
				fputs(error, stderr);
				exit(1);
			}
			static char res[100];
			gcvt((*sine)(atof(*(tokens+2))), 6, res);
			dlclose(handle);
			return res;
		}else if(strcmp(*(tokens+1), "pow") == 0){
			double (*pow)(double, double);
			char *error;
			pow = dlsym(handle, "pow");
			if ((error = dlerror()) != NULL)  {
				fputs(error, stderr);
				exit(1);
			}
			static char res[100];
			gcvt((*pow)(atof(*(tokens+2)), atof(*(tokens+3))), 10, res);
			dlclose(handle);
			return res;
		}else if(strcmp(*(tokens+1), "sqrt") == 0){
			double (*sine)(double);
			char *error;
			sine = dlsym(handle, "sqrt");
			if ((error = dlerror()) != NULL)  {
				fputs(error, stderr);
				exit(1);
			}
			static char res[100];
			gcvt((*sine)(atof(*(tokens+2))), 6, res);
			dlclose(handle);
			return res;
		}else{
			static char res[100];
			strcpy(res, "Function not valid.");
			dlclose(handle);
			return res;
		}
	}

}