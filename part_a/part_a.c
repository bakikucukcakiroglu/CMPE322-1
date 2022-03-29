/**
 * @file part_a.c
 * @author Hasan Baki Küçükçakýroðlu
 *
 * @brief 
 * This program takes two arguments. First of them is location of blockbox executable and second is location of output file. 
 * We use pipes to establish communication between parent and child processes. After pipelines are described, parent process 
 * forks and creates its child process. Since child process has same pipelines, data flow between these two are provided.
 * Parent process takes arguments from command line and pass them to child process through pipelines. Child process uses exec
 * system call to load blackbox process. After execution of blackbox output of it comes to parent through again pipelines. File
 * operations are done there and output is written appropriate file which is given as command line arguments.
 * 
 */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include<sys/wait.h>
#include <sys/types.h>
#include <string.h>

int main(int argc, char*argv[]) {
    /**
     * p2c and c2p are the pipe handles for the parent-to-child and child-to-parent pipes.
     * pid is the process id of the process. 
     * p_str is the char array where inputs are kept sequantially. 
     */
    int     p2c[2], c2p[2], errc2p[2], nbytes;
    pid_t   pid;
    char    p_str[500] ,
            buffer[5000];

    /**
     * 
     * in1 and in2 are two integers which is taken from user and will be passed to child process to be proccessed by blackbox.
     * 
     */
    int in1, in2; 

    /**
     * 
     * The parent creates both pipes, which will be shared between the parent and the child. When parent forks, 
     * pipes are coppied as well.After forking, they creates a virtual pipe between parent and child to transfer data.
     * 
     */
    pipe(p2c);
    pipe(c2p);
    pipe(errc2p);

   /**
    * Parent forks to create child process. If it fails, it returns -1. If it is succesfull, it returns id of the child process.
    * 
    */
    if((pid=fork()) == -1) {
        fprintf(stderr, "fork() failed.\n");
        exit(-1);
    }

    //.......................................................................................................
    /**
     * 
     * This is the code for the parent process.
     * 
     */
    if(pid > 0) { 
      /**
       * 
       * Two integer which are used in child process are taken from user in parent process by scanf. 
       * 
       */
    	scanf("%d", &in1);
    	scanf("%d", &in2);


        /**
        * 
        * We concatenate two integer to obtain one input string. This string will be given to blackbox.
        * 
        */
    	sprintf(p_str, "%d %d", in1, in2); //"12 12"


        /**
         * 
         * The parent should close the ends of the pipes that it will not use.
         * 
         */
        close(p2c[0]);    // Parent will not read from p2c
        close(c2p[1]);    // Parent will not write to c2p
        close(errc2p[1]); //Parent will not write to error

        /**
         * 
         * Send a request message to the child process.
         * Don't forget to add "1" to the length of the string for the NULL character.
         * 
         */
        write(p2c[1], p_str, (sizeof(p_str)));

        
        /**
         * 
         * File pointer is created. This pointer will be used to point to the file that will be opened. 
         * The name and adress of the file that will be created comes from arguments. a+ option provide us 
         * append some content to old content instead of writing on it.
         * 
         */
        FILE * fp;

   		fp = fopen (argv[2], "a+");

        /**
         * 
        * If parent read output  pipe and number of characters that is read is more than zore, that means 
        * blackbox returned a output. We write success to the output file.
        * 
        */
        if(read(c2p[0], buffer, sizeof(buffer))){

        	fprintf(fp, "%s\n", "SUCCESS:");

            
        }
        /**
         * 
        * If parent read output  pipe and number of characters that is read is zore, that means 
        * blackbox returned a output. We write fail to the output file.
        * 
        */
        else if(read(errc2p[0], buffer, sizeof(buffer))){

        	fprintf(fp, "%s\n", "FAIL:");
        }
        /**
         * 
        * It prints the output returnd by blackbox. 
        * 
        */
		fprintf(fp, "%s",buffer);
		fclose(fp);

        /**
         * Now, wait for the response of the child.
         */
        

        /**
         * We will not bother for waiting for the child (no need to worry if the parent 
         * dies before the child responds) since the parent cannot terminate before 
         * the child process sends the response (due to the read() function call.
         */
    }
    //.......................................................................................................
    else {
        /**
         * 
         * This is the code for the child process.
         * Dup2 redirects stdin, stdout, and stderr file descriptors of the child process. Since child process calls 
         * exec system call with blackbox, stdinn stdout and stderr of the blackbox are redirected. Stdin of the
         * process are redirected to p2c file descriptor. Which means parent to child.
         * 
         */
        if (dup2(p2c[0], STDIN_FILENO) == -1) {
		perror("Failed to redirect standard input");
		return 1;
		}
        /**
        * 
        * Stdout file of the process are redirected to c2p file descriptor. Which means child to parent.
        * 
        */
		if (dup2(c2p[1], STDOUT_FILENO) == -1) {
		perror("Failed to redirect standard output");
		return 1;
		}
        /**
        *
        * Stderr file of the process are redirected to errc2p file descriptor. Which means error of child to parent.
        *
        */
		if (dup2(errc2p[1], STDERR_FILENO) == -1) {
		perror("Failed to redirect standard error");
		return 1;
		}


		/*if (close(fd) == -1) {
		perror("Failed to close the file");
		return 1;
		}*/

		 /**
         * The child should close the ends of the pipes that it will not use.
         */
        close(c2p[0]);    // Child will not read from c2p
        close(c2p[1]);
        close(p2c[0]);    // Child will not write to p2c
        close(p2c[1]);
        close(errc2p[0]);    // Child will not write to error
        close(errc2p[1]);

        /**
        * 
        * Process calls exec system call. This brings the content of blackout process to child process. File descriptors,
        * open files, process id remain same. 
        * 
        */
        execl(argv[1], argv[1], NULL);
        perror("Child failed to exec \n");
        return 1; 
        	  
    }

    return(0);
}

