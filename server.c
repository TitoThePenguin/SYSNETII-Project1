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
                                                                                                                                                                                   102,1-8       33%
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
        int addrLen = sizeof(struct sockaddr);
        getsockname(listensockfd, (struct sockaddr *) &printSock, &addrLen);
        fprintf(stderr, "Host name: %s\n", hostname);
        fprintf(stderr, "IP address: %s\n", inet_ntoa(((struct sockaddr_in*)&myFreq[1].ifr_addr)->sin_addr));
        fprintf(stderr, "Socket port = %d\n", ntohs(printSock.sin_port));

        //Accepts incoming connections.
        struct sockaddr_in clientAddress;
        int mysockfd;
        int clientLength = sizeof(clientAddress);
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
        int errCheck, count;
        char buffer[256];
        char message[256];
        bzero(buffer,256);
        bzero(message,256);

        //Reads from the socket.
        errCheck = read(aSock, buffer, 255);
        if(errCheck < 0)
        {
fprintf(stderr, "Error reading from socket\n");
                close(aSock);
                return;
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
                        errCheck = write(aSock, "<reply>%s</reply>", message);
                        close(aSock);
                        return;
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
                        errCheck = write(aSock, "<loadavg>%d;%d;%d</loadavg>", loadavg[0], loadavg[1], loadavg[2]);
                        close(aSock);
                        return;
                }
        }

        //Writes an error message to the socket if the message was not correctly formatted.
        errCheck = write(aSock, "<error>unknown format</error>");
        if(errCheck < 0)
        {
                fprintf(stderr, "Error writing to socket\n");
                close(aSock);
                return;
        }
        close(aSock);
        return;
}
