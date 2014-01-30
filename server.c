#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <errno.h>
#include <netdb.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <signal.h>
#include <stdint.h>
#include <unistd.h>

#define MAX_NUM_LISTENER_ALLOWED 5
#define FALSE 0
#define TRUE 1

/*
 * Used to exit the program when CTRL - C is pressed.
 */
void handler(int param)
{
        fprintf(stderr, "Exiting!\n");
        exit(EXIT_SUCCESS);
}

/*
 * Processes a request from the client.
 */
void * processRequest(void *);


int main()
{
        int listensockfd = 0;
        int errCheck;
        struct sockaddr_in servaddr;
        signal(SIGINT, handler);

        //Creates a TCP socket.
        listensockfd = socket(AF_INET, SOCK_STREAM, 0);
        if(listensockfd < 0)
        {
                fprintf(stderr, "Could not create socket!\n");
                close(listensockfd);
                return EXIT_FAILURE;
        }
        fprintf(stderr, "Socket succesfully created!\n");

        //Allocates memory for hostname and hostptr.
        char * hostname = malloc(sizeof(char) * 32);
        if(hostname == NULL)
        {
                fprintf(stderr, "Could not allocate memory for 'hostname'!\n");
                close(listensockfd);
                return EXIT_FAILURE;
        }
        struct hostent * hostptr = malloc(sizeof(struct hostent));
        if(hostptr == NULL)
        {
                fprintf(stderr, "Could not allocate memory for 'hostptr'!\n");
                close(listensockfd);
                return EXIT_FAILURE;
        }

        //Gets info about host running server.
        errCheck = gethostname(hostname, 32);
        if(errCheck != 0)
        {
                fprintf(stderr, "gethostname failed!\n");
                close(listensockfd);
                return EXIT_FAILURE;
        }
        hostptr = gethostbyname(hostname);

        //Fills in destination address structure.
        memset((void *) &servaddr, 0, (size_t)sizeof(servaddr));
        servaddr.sin_family = (short)(AF_INET);
        memcpy((void *)& servaddr.sin_addr, (void *) hostptr->h_addr, hostptr->h_length);
        //Dynamically binds port by assigning 0;
        servaddr.sin_port = 0;

        //Binds socket locally.
        errCheck = bind(listensockfd, (const struct sockaddr *) &servaddr, (socklen_t)sizeof(servaddr));
        if(errCheck != 0)
        {
                fprintf(stderr, "Could not bind socket!\n");
                close(listensockfd);
                return EXIT_FAILURE;
        }
        fprintf(stderr, "Socket succesfully bound!\n");

        //Listens on socket.
        errCheck = listen(listensockfd, MAX_NUM_LISTENER_ALLOWED);
        if(errCheck != 0)
        {
                fprintf(stderr, "Could not listen!\n");
                close(listensockfd);
        }

        //Allows for the IP address of the host server to be retrieved.
        struct ifreq myFreq[20];
        struct ifconf ic;
        ic.ifc_len = sizeof(myFreq);
        ic.ifc_req = myFreq;
        errCheck = ioctl(listensockfd, SIOCGIFCONF, &ic);
        if(errCheck != 0)
        {
                close(listensockfd);
                return EXIT_FAILURE;
        }

        //Prtints the host name, IP address, and port #.
        struct sockaddr_in printSock;
        socklen_t addrLen = sizeof(struct sockaddr);
        getsockname(listensockfd, (struct sockaddr *) &printSock, &addrLen);
        fprintf(stderr, "Host name: %s\n", hostname);
        fprintf(stderr, "IP address: %s\n", inet_ntoa(((struct sockaddr_in*)&myFreq[1].ifr_addr)->sin_addr));
        fprintf(stderr, "Socket port = %d\n", ntohs(printSock.sin_port));

        //Accepts incoming connections.
        struct sockaddr_in clientAddress;
        int mysockfd;
        socklen_t clientLength = sizeof(clientAddress);
        pthread_t tid[2];
        int threadCount = 0;
        while(1)
        {
                mysockfd = accept(listensockfd, (struct sockaddr *) &clientAddress, &clientLength);
                threadCount++;
                pthread_create(&tid[threadCount-1], NULL, processRequest, (void *)(intptr_t)mysockfd);
                threadCount--;
        }

        return EXIT_SUCCESS;
}

void * processRequest(void * mySock)
{
        int aSock = (intptr_t)mySock;
        int errCheck, count, count2;
        char buffer[256];
        char message[256];
        char reply[256];
        bzero(buffer,256);
        bzero(message,256);
        bzero(reply,256);

        //Reads from the socket.
        errCheck = read(aSock, buffer, 255);
        if(errCheck < 0)
        {
                fprintf(stderr, "Error reading from socket\n");
                close(aSock);
                return NULL;
        }
        fprintf(stderr, "Here is the message: %s\n", buffer);

        //Checks to see if the message is an <echo>.
        if(buffer[0] == '<' && buffer[1] == 'e' && buffer[2] == 'c' && buffer[3] == 'h' && buffer[4] == 'o' && buffer[5] == '>' && buffer[6] != ' ')
        {
                //Reads the message body into a buffer.
                count = 6;
                while(buffer[count] != '<' && buffer[count] != '\0')
                {
                        message[count - 6] = buffer[count];
                        count++;
                }
                message[count - 6] = '\0';

                //Checks that the message ends with </echo> and does not continue.
                if(message[count - 7] != ' ' && buffer[count] == '<' && buffer[count + 1] == '/' && buffer[count + 2] == 'e' && buffer[count + 3] == 'c' && buffer[count + 4] == 'h' && buffer[count + 5] == 'o' && buffer[count + 6] == '>' && buffer[count + 7] == '\0')
                {
                        //Writes the echo response to the socket.
                        reply[0] = '<';
                        reply[1] = 'r';
                        reply[2] = 'e';
                        reply[3] = 'p';
                        reply[4] = 'l';
                        reply[5] = 'y';
                        reply[6] = '>';
                        count2 = count - 6;
                        for(count = 0; count < count2; count++)
                                reply[count + 7] = message[count];
                        reply[count + 7] = '<';
                        reply[count + 8] = '/';
                        reply[count + 9] = 'r';
                        reply[count + 10] = 'e';
                        reply[count + 11] = 'p';
                        reply[count + 12] = 'l';
                        reply[count + 13] = 'y';
                        reply[count + 14] = '>';
                        reply[count + 15] = '\0';
                        errCheck = write(aSock, reply, sizeof(reply));
                        if(errCheck < 0)
                        {
                                fprintf(stderr, "Error writing to socket\n");
                                close(aSock);
                                return NULL;
                        }
                        close(aSock);
                        return NULL;
                }
        }
        //Checks if the message is just <loadavg/> with no extra characters (as loadavg should be).
        else if(strcmp(buffer, "<loadavg/>") == 0)
        {
                //Writes the loadavg response to the socket.
                double loadavg[3];
                errCheck = getloadavg(loadavg, 3);
                if(errCheck > 0)
                {
                        reply[0] = '<';
                        reply[1] = 'l';
                        reply[2] = 'o';
                        reply[3] = 'a';
                        reply[4] = 'd';
                        reply[5] = 'a';
                        reply[6] = 'v';
                        reply[7] = 'g';
                        reply[8] = '>';
                        memcpy(&reply[9], &loadavg[0], sizeof(loadavg[0]));
                        reply[13] = ';';
                        memcpy(&reply[14], &loadavg[1], sizeof(loadavg[1]));
                        reply[18] = ';';
                        memcpy(&reply[19], &loadavg[2], sizeof(loadavg[2]));
                        reply[23] = '<';
                        reply[24] = '/';
                        reply[25] = 'l';
                        reply[26] = 'o';
                        reply[27] = 'a';
                        reply[28] = 'd';
                        reply[29] = 'a';
                        reply[30] = 'v';
                        reply[31] = 'g';
                        reply[32] = '>';
                        reply[33] = '\0';
                        errCheck = write(aSock, reply, sizeof(reply));
                        if(errCheck < 0)
                        {
                                fprintf(stderr, "Error writing to socket\n");
                                close(aSock);
                                return NULL;
                        }
                        close(aSock);
                        return NULL;
                }
        }

        //Writes an error message to the socket if the message was not correctly formatted.
        reply[0] = '<';
        reply[1] = 'e';
        reply[2] = 'r';
        reply[3] = 'r';
        reply[4] = 'o';
        reply[5] = 'r';
        reply[6] = '>';
        reply[7] = 'u';
        reply[8] = 'n';
        reply[9] = 'k';
        reply[10] = 'n';
        reply[11] = 'o';
        reply[12] = 'w';
        reply[13] = 'n';
        reply[14] = ' ';
        reply[15] = 'f';
        reply[16] = 'o';
        reply[17] = 'r';
        reply[18] = 'm';
        reply[19] = 'a';
        reply[20] = 't';
        reply[21] = '<';
        reply[22] = '/';
        reply[23] = 'e';
        reply[24] = 'r';
        reply[25] = 'r';
        reply[26] = 'o';
        reply[27] = 'r';
        reply[28] = '>';
        reply[29] = '\0';
        errCheck = write(aSock, reply, sizeof(reply));
        if(errCheck < 0)
        {
                fprintf(stderr, "Error writing to socket\n");
                close(aSock);
                return NULL;
        }
        close(aSock);
        return NULL;
}
