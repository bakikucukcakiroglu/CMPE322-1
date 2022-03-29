/**
* 
* @author Hasan Baki Küçükçakıroğlu
* 
* @file part_b_client.c
* 
* @brief
* 
* This file creates the client side of the communication that is established between server and client.
* This communication goes through remote procedure calls. Client takes arguments from command line, 
* pass them to server via rpc. Server side creates output and returns it back to the client side. Client 
* creates and open files according to arguments and fill them with output properly. 
* 
*/

#include "part_b.h"

/**
*  
* Black_prog are called by main fucntion with parameters obtained in main via I/O and argument passing. This program
* creates client, initialize inputs that will be sent via RPC and starts to wait. When server response its call, it 
* starts to proccess output file and write it to the file that is described in arguments. 
*/
void
black_prog_1(char *host, int x, int y, char argt[], char * pathOut )
{
	CLIENT *clnt; // creates client pointer
	char * *result_1; // the result that will be returned by server program and will be used in output file
	
	/**
	 *  They are the inputs that will be sent to the server program. It consists of three parts, 
	 * int a, int b and path. int a and int b indicates that the integers that will be proccessed.
	 * Path indicates that the path of output file that will be created by client.
	 */
	inputs  part_b_1_arg; 
						  

	/*
	* Creates client and point it with pointer that was created above. If the pointer is NULL, it means the client couldn't 
	* be created so program exits.
	*/
#ifndef	DEBUG
	clnt = clnt_create (host, BLACK_PROG, BLACK_VERS, "udp");
	if (clnt == NULL) {
		clnt_pcreateerror (host);
		exit (1);
	}
#endif	/* DEBUG */

	/*
	* 
	* X and Y are taken by main, they are integers that will be passed to server. In order to pass them server via RPC, 
	* we assign them to the argument of RPC struct.
	*/
	part_b_1_arg.a=x;
	part_b_1_arg.b=y;
	
	/**
	* This while takes path of the output file byte by byte. It assigns it to proper part of the RPC struct that will be 
	* passed to server.
	*/
	int i=0;
	while(1){

		if(argt[i]=='\0') { 			// That is the end of string. We have to stop reading. 

			part_b_1_arg.path[i]='\0'; 	// We add a null terminator to the end of the string to provent anormal behaviours. 
			break;					   	// We stop when we reached the end of the string.
		}

		part_b_1_arg.path[i]=argt[i];
		i++;

	}
	
	result_1 = part_b_1(&part_b_1_arg, clnt); // The result that returned by server process. It will later be processed.
	/**
	* 
	* If the result pointer is NULL, it means there is no result. Program should prompt an error.
	*/
	if (result_1 == (char **) NULL) {
		clnt_perror (clnt, "call failed");
	}
	
	/**
     * 
     * File pointer is created. This pointer will be used to point to the file that will be opened. 
     * The name and adress of the file that will be created comes from arguments. a+ option provide us 
     * append some content to old content instead of writing on it.
     * 
     */
	else{
		FILE * fp;
		fp = fopen (pathOut, "a+");
		fprintf(fp, "%s\n", *result_1);
	}

#ifndef	DEBUG
	clnt_destroy (clnt);
#endif	 /* DEBUG */
}

/**
* 
* Main function of the client side. This function takes 3 arguments. Place these arguments properly as blackbox address,
* output address, and ip address of server. Makes two io operation to obtain integers that will be pushed to server side.
* Calls black_prog_1, wghich will be create client, make rpc and handle the output. 
* 
*/
int main (int argc, char *argv[])
{
	char *host; 		// ip address of the server that client will sent the data and will wait for output 
	int in1, in2; 		// integers that will be sent to server to be processed. They are obtained by I/O operations. 
	/*
	* There are 4 arguments. Name of the program, address of the blackbox, address of the output file and ip address of the 
	* server. If any of them are missing, client prompts a error. 
	*/
	if (argc < 4) {
		printf ("usage: %s server_host\n", argv[0]);
		exit (1);
	}
	host = argv[3]; // 
	/*
	* 
	Two integers are taken from user via I/O operations.
	*/
	scanf("%d", &in1);
	scanf("%d", &in2);

	/*
	* 
	* Main calls the black_prog_1 which will handle ooperations other than I/O and argument taking. Taken arguments  are used 
	* in this function call.
	*/
	black_prog_1 (host, in1, in2, argv[1], argv[2] );
	exit (0);
}