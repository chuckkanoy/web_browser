#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char buffer[256];

    //if the number of arguments is incorrect, then show usage and exit
    if (argc < 4) {
       fprintf(stderr,"usage %s host_name file_path port_no\n", argv[0]);
       exit(0);
    }

    //establish a TCP connection
    portno = atoi(argv[3]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
        error("ERROR connecting");
    
    //compose string consisting of HTTP request
    bzero(buffer, 256);
    strcpy(buffer, "GET ");
    strcat(buffer, argv[2]);
    strcat(buffer, " HTTP/1.1\r\n");
    strcat(buffer, "Host: ");
    strcat(buffer, argv[1]);
    strcat(buffer, "\r\n\r\n");
    printf("%s", buffer);
    //send the request through the socket
    n = write(sockfd, buffer, strlen(buffer));
    //printf("%d, %d", n, strlen(buffer));
    if(n<0)
        error("ERROR writing to socket");
    bzero(buffer,256);

    n = 1;
    //read and display the returned webpage as raw html file
    while(n > 0){
        n = read(sockfd, buffer, sizeof(buffer));
        if(n > 0) printf("%s\n", buffer);
        bzero(buffer, 256);
        //fflush(buffer);
    }

    close(sockfd);

    return 0;
}