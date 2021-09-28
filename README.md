### Submitter name:
## Mohit Singhal
### Roll No.:
## 2018eeb1165
### Course:
## CS303

#

## What does this program do?


## Logic behind the code


## How to compile and run the program?

To compile the code for the server which mimics a basic dispatcher run the following in the shell:- 

```
gcc server.c -o server -pthread -ldl
```
`pthread` and `ldl` are linked dynamically.

Now run the following in the shell to run the server and the provide the necessary command line arguments:- 

```
./server [Thread Limit] [Memory Limit (in MB)] [File Limit]
```
**Note:-** Memory Limit argument here is the total limit of the virtual memory that the process and its child process can use. The limit should be atleast 70-80 MB otherwise the process may fail to create all the threads required for thread pool. 

