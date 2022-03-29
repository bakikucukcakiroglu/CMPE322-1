/*
 * @author Hasan Baki Küçükçakıroğlu
 * 
 * @file part_b_server.c
 * 
 * @brief
 *
 * This program takes two arguments. First of them is location of blockbox executable and second is location of output file. 
 * We use pipes to establish communication between parent and child processes. After pipelines are described, parent process 
 * forks and creates its child process. Since child process has same pipelines, data flow between these two are provided.
 * Parent process takes arguments from command line and pass them to child process through pipelines. Child process uses exec
 * system call to load blackbox process. After execution of blackbox output of it comes to parent through again pipelines. File
 * operations are done there and output is written appropriate file which is given as command line arguments.
 * 
 */


/**
 * Necessary libraries and headers are included. 
 */
#include "part_b.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include<sys/wait.h>
#include <sys/types.h>
#include <string.h>

/**
 * p2c and c2p are the pipe handles for the parent-to-child and child-to-parent pipes.
 * pid is the process id of the process. 
 * p_str is the char array where inputs are kept sequantially. 
 */
char **
part_b_1_svc(inputs *argp, struct svc_req *rqstp)
{
     /**
     * 
     * in1 and in2 are two integers which is taken from user and will be passed to child process to be proccessed by blackbox.
     * 
     */
	static char * result;
	int     p2c[2], c2p[2], errc2p[2], nbytes;
    pid_t   pid;
    char    p_str[500] ,
    buffer[5000];

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
       * Two integer which are used in child process are taken from client in the parent process.
       * Client sends this integers via remote process call. These two integers are concatanated with sprintf function and 
       * will be sent to child process. The difference between part a and part b is, part a takes integers from directly argument line and
       * executes that in place. In part b integers taken in client side and passed to server via rpc and processed here. The output of blackbox
       * is sent to parent via redirected files(stdin, stdout, stderror). The result is commented there and final output is sent to clinet back.
       * Client takes necessary actions to write the into apropriate files.
       * 
       */
    	sprintf(p_str, "%d %d", argp->a, argp->b); 

        close(p2c[0]);     // Parent will not read from p2c
        close(c2p[1]);    // Parent will not write to c2p
        close(errc2p[1]); //Parent will not write to error


        /**
         * 
         * Send a request message to the child process.
         * Don't forget to add "1" to the length of the string for the NULL character.
         * 
         */
        write(p2c[1], p_str, (sizeof(p_str))); 


        int size; // The number of bits which read system call reads from c2p file descriptor to buffer. It is limited with buffer size.

        
      /**
         * 
        * If parent read output  pipe and number of characters that is read is more than zore, that means 
        * blackbox returned a output. We write success to the output file before returning it to client program.
        * 
        */
        if((size=read(c2p[0], buffer, sizeof(buffer)))){
          
          buffer[size-1]='\0';                  // We add null terminator to the end of buffer to prevent it behave anormally.
          result=calloc(10+strlen(buffer), 1);  // We allocate memory to final output. Size of "success" and size of output from blackbox is summed up. 
          /**
           *
           * We put characters one by one to the result array. 
           *
           */
          result[0]='S';
          result[1]='U';
          result[2]='C';
          result[3]='C';
          result[4]='E';
          result[5]='S';
          result[6]='S';
          result[7]=':';
          result[8]='\n';
          strcat(result, buffer);  // Concatanate "success" with output that is come from blackbox.

      }
       /**
        * 
        * If parent read output  pipe and number of characters that is read is zore, that means 
        * blackbox returned a output. We write fail to the output file before returning it to the client program.
        * 
        */
      int size2;  // The number of bits which read system call reads from c2p file descriptor to buffer. It is limited with buffer size.
      if((size2=read(errc2p[0], buffer, sizeof(buffer)))){
          
          buffer[size2-1]='\0';                 // We add null terminator to the end of buffer to prevent it behave anormally.
          result= calloc(7+strlen(buffer), 1);  // We allocate memory to final output. Size of "fail" and size of output from blackbox is summed up. 
          /**
          * 
          *  We put characters one by one to the result array. 
          * 
          */
          result[0]='F';
          result[1]='A';
          result[2]='I';
          result[3]='L';
          result[4]=':';
          result[5]='\n';

          strcat(result, buffer); // Concatanate "fail" with output that is come from blackbox.

      }

  }

 /**
   * 
   * This is the code for the child process.
   * Dup2 redirects stdin, stdout, and stderr file descriptors of the child process. Since child process calls 
   * exec system call with blackbox, stdinn stdout and stderr of the blackbox are redirected. Stdin of the
   * process are redirected to p2c file descriptor. Which means parent to child.
   * 
   */
  else {

    if (dup2(p2c[0], STDIN_FILENO) == -1) { 
      perror("Failed to redirect standard input");

  }

   /**
    * 
    * Stdout file of the process are redirected to c2p file descriptor. Which means child to parent.
    * 
    */
  if (dup2(c2p[1], STDOUT_FILENO) == -1) { // ....
      perror("Failed to redirect standard output");

  }

  /**
    *
    * Stderr file of the process are redirected to errc2p file descriptor. Which means error of child to parent.
    *
    */
  if (dup2(errc2p[1], STDERR_FILENO) == -1) { // ....
      perror("Failed to redirect standard error");

  }

   /**
    * 
    * Process calls exec system call. This brings the content of blackout process to child process. File descriptors,
    * open files, process id remain same. 
    * 
    */
  execl(argp->path, argp->path, NULL);

}

return &result; //returns address of result to the client 
} 
