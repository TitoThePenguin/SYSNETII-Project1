/*
 * TCPclient.c
 * Systems and Networks II
 * Project 1
 *
 * This file describes the functions to be implemented by the TCPclient.
 * You may also implement any auxillary functions you deem necessary.
 */
 
#include <stdio.h> 			/* for printf() and fprintf() */
#include <sys/socket.h>			/* for socket(), connect(), send(), and recv() */
#include <arpa/inet.h>			/* for sockaddr_in and inet_addr() */
#include <stdlib.h>			/* for atoi() and exit() */
#include <string.h>			/* for memset() */
#include <unistd.h>			/* for close() */
#include "TCPclient.h"

#define RCVBUFSIZE 256   /* Size of receive buffer */

/*
 * Creates a streaming socket and connects to a server.
 *
 * serverName - the ip address or hostname of the server given as a string
 * port       - the port number of the server
 * dest       - the server's address information; the structure should be created with information
 *              on the server (like port, address, and family) in this function call
 *
 * return value - the socket identifier or a negative number indicating the error if a connection could not be established
 */
int createSocket(char * serverName, int port, struct sockaddr_in * dest)
{
		int socketCreation = 0;
		memset(dest, 0, sizeof(dest));
		dest.sin_family = AF_INET;
		inet_pton(AF_INET, serverName, dest.sin_addr); //Sever IP address
		dest.sin_port = hton(port);
		socketCreation = socket(PF_INET, SOCK_STREAM, 0);
		if(connect(s, (struct sockaddr *)dest, sizeof(dest))< 0)
		{
			socketCreation = -1;
		}
		return socketCreation;
}

/*
 * Sends a request for service to the server. This is an asynchronous call to the server, 
 * so do not wait for a reply in this function.
 * 
 * sock    - the socket identifier
 * request - the request to be sent encoded as a string
 * dest    - the server's address information
 *
 * return   - 0, if no error; otherwise, a negative number indicating the error
 */
int sendRequest(int sock, char * request, struct sockaddr_in * dest)
{
	int error = 0;
	error = write(sock, request, strlen(request));
	return error;
}	

/*
 * Receives the server's response formatted as an XML text string.
 *
 * sock     - the socket identifier
 * response - the server's response as an XML formatted string to be filled in by this function;
 *            memory is allocated for storing response
 *
 * return   - 0, if no error; otherwise, a negative number indicating the error
 */
int receiveResponse(int sock, char * response)
{
	int error = 0;
	error = read(sock, response, RCVBUFSIZE);
	return error;
}

/*
 * Prints the response to the screen in a formatted way.
 *
 * response - the server's response as an XML formatted string
 *
 */
void printResponse(char* response)
{
		printf("\nResponse is: %s\n", response);

		char * temp;
		temp = strtok(response, " .\n");
		while(temp != NULL)
		{
			
			if((strstr(temp, "<reply>") == NULL) && (strstr(temp, "</reply>") == NULL))
			{
				printf("%s ", temp);
			}
			temp = strtok('\0', " .\n");
		}
		printf("\n\n");
}

/*
 * Closes the specified socket
 *
 * sock - the ID of the socket to be closed
 * 
 * return - 0, if no error; otherwise, a negative number indicating the error
 */
int closeSocket(int sock)
{
	int error = 0;
	error = close(sock);
	return error;
}

int main(int argc, char ** argv)
{
	int sock;                        /* Socket descriptor */
    struct sockaddr_in * ServAddr; 	 /* Server address */
    unsigned short echoServPort;     /* Echo server port */
    char * servIP;                   /* Server IP address (dotted quad) */
    char * stringSent;               /* String to send to server */
    char * stringReceived			 /* String send back to client*/
    char echoBuffer[RCVBUFSIZE];     /* Buffer for echo string */
    unsigned int echoStringLen;      /* Length of string to echo */
    int bytesRcvd, totalBytesRcvd;   /* Bytes read in single recv() 
                                        and total bytes read */
    int portNum, temp;

    stringSent = (char *)malloc(1+150*(sizeof(char)));
    stringReceived = (char *)malloc(1+150*(sizeof(char)));
    strcpy(stringSent, "<reply> Here is a string! </reply>\n");
	printf("String sent: %s\n\n", stringSent);
	printResponse(temp);

	 if(argc != 2)
	{
		perror("There is not enough arguments in entered in\n");
		exit(1);
	}
	servIP = argv[0];
	portNum = atoi(argv[1]);
	sock = createSocket(servIP, portNum, ServAddr);
	if(sock < 0)
	{
		perror("Could not create socket\n");
		exit(1);
	}
	temp = sendRequest(sock, stringSent, ServAddr);
	if(temp != 0)
	{
		perror("Could not send request\n");
		exit(1);
	}
	temp = receiveResponse(sock, stringReceived);
	if(temp != 0)
	{
		perror("Could not receive string\n");
		exit(1);
	}
	printResponse(stringReceived);
	
	free(stringSent);
	free(stringReceived);
	
	return 0;
}

void emptyBuffer(char * arry, int sizeArry)
{
	int i;
	for(i = 0; i < sizeArry; i++)
	{
		arry[i] = '\0';	
	}	
}

void showASCII(char * str)
{
    int i, sentSize, j;
    sentSize = strlen(str);
    sentSize = sentSize + 3;
    for(i = 0; i < sentSize; i++)
    {
      	printf("%d ", str[i]);   
    }
    printf("\n");
}
