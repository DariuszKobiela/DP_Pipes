#include <unistd.h>
#include <stdio.h>
#define READ 0
#define WRITE 1

int main ()
{
	int handles[2]; //two ends of the pipe - handles
	int x;
	puts("Program pipes starts");
	
	puts("Creating pipe...");
	pipe(handles); //passing a handles array, creating an unnamed pipe
	
	puts("The fork is about to come");
	pid = fork();//fork returns the pid for the parent process (parent identifier)
	if(pid != 0) //splits into two processes (here pid!= 0)
	{
		puts("Parent process");
		close(handles[WRITE]); //closes this side of the pipe which it will not use
		puts("Waiting for the child proces...");
		read(handles[READ], &x, sizeof(x)); //here x = some system garbage
		printf("Parent process got %d\n", x); //here x=10
		close(handles[READ]);
	}
	else //pid==0 always for child process
	{
		puts("Child process");
		close(handles[READ]);
		x = 10; //exemplary number to send
		sleep(10) ;
		write(handles[WRITE], &x, sizeof(x)); //the write function writes to handles[WRITE] what is given in the second and third arguments
		puts("Child process has sent");
		close(handles[WRITE]);
	}
	
	puts("The pipes program ends");
}