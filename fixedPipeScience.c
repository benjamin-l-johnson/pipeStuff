/* Have a parent create a child (RightMost) which creates a 
child (M) which creates a child (LeftMost) , all piped , and 
ultimately implements the command 

	ls -l  | UpCase  | LowerHalf
where those are local programs that play with character streams

Each process will have  a left and right pipe, fdl and fdr,  
(if you know what I mean) but only M uses them both. 

*/
#include <wait.h>
#include "apue.h"  // for error routines

int main ( )
{ 

	char buffer[80];
	pid_t pid; 
	int Status;
	
	int fdr[2];// Pipe File Descriptors 
	// 0 for reading, 1 for writing ? 

	write(STDERR_FILENO, "Top Level\n",10);

	pid = fork();
	if (pid == 0 ) // first child named "RM" for Rightmost
	{
		write(STDERR_FILENO,"RM\n",3);
		pipe(fdr); 
		pid = fork(); 

		if(pid >0 ) //RM is new parent 
		// RM's child is "M" for Middle
		// And  M and RM share a pipe 
		// Here we are still in RM 
		{ 
			close(fdr[1]); 
			//  RM does not write to "left"

			// we dup what is comming down the pipe
			// to RMs stdin
			fdr[0] = dup2(fdr[0],STDIN_FILENO);
			// But reads from left as stdin  
			
			write (STDERR_FILENO, "Ready to Exec RM\n",13);
			if (((execl("Lower","Lower",(char *) 0))!=0)) 
				write(STDERR_FILENO,"Lower  fail\n",15);
			// and transmogrifies 
		} // exit RM 


		if (pid == 0) 
		// M's world -- first make fdl <== fdr N.B. ****
		{	
			write (STDERR_FILENO,"M\n",2);
			
			//fdl[1] is the end we write down

			//fdr[1]=fdl[1]; // making 
			// M's right <==> RM's left

			//Take the stdout from middle and
			// push it down the pipe
			fprintf(stderr, "Ms fd[1] is: %d\n",fdr[1] );
			fdr[1] =dup2(fdr[1],STDOUT_FILENO);
			
			// and M's stdout is the pipe out 
			

			//fdr[0]=fdl[0]; 
			close(fdr[0]);			     
			// for symmetry	
			
			pipe(fdr); // Science Q ? We can pipe and repipe 
			// fdl because they are just integers ? 

			pid = fork(); // M hatches LM (leftmost)

			if (pid >  0)  // Still M's world
			{
				fprintf(stderr, "after 2nd pipe Ms fdr[1] is: %d\n", fdr[1]);
				close(fdr[1]); 
				// No write on left
				// but read from pipe as from stdin


				//Anything comming down the pipe is stdin for M
				fdr[0]= dup2(fdr[0],STDIN_FILENO);//MIDDLE 
				
				// and transmogrify
				write (STDERR_FILENO, "Ready to Exec in M\n",19);
				if ((execl("Upcase","Upcase",(char *)0)!=0)) 
    				write(STDERR_FILENO,"Upcase fail\n",12);
    			fprintf(stderr, "after exec\n" );

			} // exit M
		
			/* UNTIL HERE FOR MIDDLE PROCESS */

			if (pid == 0)   // LM's world 
			// LM has fdl and fdr
			// and M has messed with fdl  
			// so for symmetry ....
			{  
				write(STDERR_FILENO,"LM\n",3);
				//fdr[0] = fdl[0];
				//fdr[1] = fdl[1]; 

				//No need to read
				close(fdr[0]);

				//push everything from stdout down the pipe
				fdr[1] = dup2(fdr[1],STDOUT_FILENO);
				
				// and LM has no tricks on stdin because
				// LM will not fork and no redirection '>'
				if ((execl("/bin/ls","ls","-l", (char *) 0)!=0)) 
					write(STDERR_FILENO,"ls fail\n",8);
					// transmogrify 
			} // exit LM

		}  // which was within M from up there at ****

	} // end of the show ?
	
	
	waitpid(pid,&Status,0);   
	write(STDERR_FILENO,"Top Level Again\n",16);
	return 0;
} // end of the show ? 

