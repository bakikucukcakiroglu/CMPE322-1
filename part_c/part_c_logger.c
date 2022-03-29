/**
* @author Hasan Baki Küçükçakıroğlu
* 
* @file part_c_logger.c
* 
* @brief
* 
* This is the logger program. This program connects to the server program via sockets. 
* Server program provides IP and port informations and connects logger. Server send inputs 
* and outputs of the blackbox program via sockets to the logger program. Logger program takes
* these data and writes them to the output file accordingly. I utilized the code from GeekForGeeks
* to create this file. 
* 
*/

/**
 * Necessary libraries and headers are included. 
 */
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>

int main(int argc, char const *argv[])
{
    /**
     * 
     * Main takes two arguments. First of them is the address of the output file and 
     * second of them is the port number of the logger program.
     * 
    */
    const char * outputAdress;
    outputAdress= argv[1];
    int port= atoi(argv[2]); //convert string argument to the integer

    /*
    * 
    * server_fd for
    * new_socket for
    * valread for
    */
    int server_fd, new_socket, valread; 

    struct sockaddr_in address;         // Address of the server.
    int opt = 1;                        // Flag used for arrangements of the socket. 
    int addrlen = sizeof(address);      // Length of the address
    char buffer[1024] = {0};            // Buffer to write data which is coming from sockets.
    
    /**
     * 
     * Creating socket file descriptor. This descriptor used to provide communication between 
     * sockets and transfer data.
     * 
     */ 
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    /**
     * 
     * Forcefully attaching socket to the port 8080. Makes necessary arrangements using
     * flags. If it fails to attach exits program.
     * 
     */ 
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
      &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;           // Sets IP adress type of the server.
    address.sin_addr.s_addr = INADDR_ANY;   // Sets IP adress type of the server. INADDR_ANY means localhost.
    address.sin_port = htons( port );       // Sets port number of the server. 


    /**
     * 
     * Forcefully attaching socket to the port 8080. It allows logger to use listen to listen sockets.
     * If binding failures, program exits.
     * 
     */ 
    if (bind(server_fd, (struct sockaddr *)&address, 
     sizeof(address))<0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    /**
     * 
     * If binding occurs normally, program can listen sockets to know if there is a data coming from any other program or not.
     * If program couldn't listen, it exits.
    * 
    */
    if (listen(server_fd, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    /*
    *
    * It creates a new socket instance to provide communicaiton between other programs. This socket can take data from other
    * programs and this data can be read from it with using read system call. 
    */
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, 
       (socklen_t*)&addrlen))<0)
    {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    /**
     * 
     * File pointer is created. This pointer will be used to point to the file that will be opened. 
     * The name and adress of the file that will be created comes from arguments. a+ option provide us 
     * append some content to old content instead of writing on it.
     * 
     */
    FILE * fp;
    fp = fopen (outputAdress, "a+");
    valread = read( new_socket , buffer, 1024); // It reads input and output data which are coming from server program from socket.
    buffer[valread]='\0';                        // We add null terminator to the end of buffer to prevent it behave anormally.
    fprintf(fp, "%s\n", buffer);                // Writes the data to the output file specified in arguments.

    return 0;
}


