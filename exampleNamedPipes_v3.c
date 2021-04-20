/*
//pipe_handle to endpoint; jest też file_descriptorem
// 0b10; // 2
//  010; // 8
//   10; // 10
// 0x10; // 16
// 
// See instruction "8.3 Potoki nazwane (kolejki FIFO)"
//
// int mkfifo ( const char *name , mode_t mode );
// call example: mkfifo("named_pipe", 0666);
//
// int pipe_handle = open("named_pipe", O_WRONLY); // or O_RDONLY, returns -1 if error
// write(pipe_handle, &number, sizeof(number));
// read(pipe_handle, &number, sizeof(number));
// close(pipe_handle);
//
// mode_t:
// uprawnienia użytkowników w systemie ósemkowym
// Owner | Group | Others
// R W X | R W X | R W X // Read Write Execute
// 1 1 0 | 1 1 0 | 1 1 0
//   6       6       6
*/
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main()
{
    int pipe_handle; //pipe_handle is an endpoint, the handle to the pipe; is also a file descriptor
    pid_t child_pid; //pid_t works like size_t (some number)
    char message[] = "test message";
    char received[32]; //buffer to which we can enter the received message

    unlink("pipe"); //before creating a new queue, I delete other queues (if there are no other queues, nothing will happen)
	//unlink == if it exists, remove it

	//CREATION OF A NAMED PIPE
	int status = mkfifo("pipe", 0666); //mkfifo parameters are the name of the pipe and access permissions //in this case you can write and read owner, group and the rest
    if (status != 0) // status=0 in case of success, status = -1 in case of failure
    {
        printf("[Parent] Cannot make named pipe\n");
        return 1; //return 0 on success, return (not zero) on failure
    }

    child_pid = fork(); //two processes are created that will communicate with each other (send a message)
    if (child_pid == 0) // We are in child process, because child_pid=0     
    {         
		printf("[Child ] Opening...\n");
		//this process will just write - O_WRONLY
		pipe_handle = open("pipe", O_WRONLY); //or O_RDONLY, O is the opening mode
		//open returns a handle to the pipe
		if (pipe_handle == -1) //returns -1 if error 
		{
			printf("[Child ] Cannot open named pipe to write\n");
			return 1;
		}
		
		printf("[Child ] Writing...\n"); //writing to pipe
		//ATTEMPT TO WRITE TO THE PIPE and check if it was possible to save in one
		if (write(pipe_handle, &message, sizeof(message)) == -1) // I give the pointer, and the size I take is the message itself
		{
			printf("[Child ] Cannot write to named pipe\n");
			return 1;
		}
		
		printf("[Child ] Closing pipe...\n");
		if (close(pipe_handle) == -1) //after sending the message, the pipe is closed (the side that wrote)
		{
			printf("[Child ] Cannot close named pipe (write)\n");
			return 1;
		}     
        
		printf("[Child ] Exit\n");
        return 0;
    }

    /* We are in parent process */  
	printf("[Parent] Opening...\n");
	pipe_handle = open("pipe", O_RDONLY);
	if (pipe_handle == -1)
	{
		printf("[Parent] Cannot open named pipe to read\n");
		return 1;
	}
	printf("[Parent] Reading...\n");
	//TRYING TO READ FROM THE PIPE and checking if it was possible to read it
	if (read(pipe_handle, &received, sizeof(message)) == -1) //read () parameters: handle to the pipe, message address, number of bytes in the message (it will read the maximum number of bytes)
	{
		printf("[Parent] Cannot read from named pipe\n");
		return 1;
	}

	printf("[Parent] Message: \"%s\"\n", received); //displays a message on the screen

	printf("[Parent] Closing pipe...\n");
	if (close(pipe_handle) == -1)
	{
		printf("[Parent] Cannot close named pipe (read)\n");
		return 1;
	}
    
    printf("[Parent] Waiting for child to exit...\n");
    wait(NULL); //the wait function waits for the child process to change state
    printf("[Parent] Child exited\n");

    printf("[Parent] Unlinking pipe...\n");
    unlink("pipe"); //release the queue, delete the queue file

    return 0;
}