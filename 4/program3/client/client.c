/***:**************************************************************
//
//  NAME:        Keanu Williams
//
//  HOMEWORK:    4
//
//  CLASS:       ICS 451
//
//  INSTRUCTOR:  Ravi Narayan
//
//  DATE:        October 12, 2019
//
//  FILE:        client.c
//
//  DESCRIPTION: This file contains the source code for the server.
//
****************************************************************/

#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>

/*****************************************************************
//
//  FUNCTION NAME: main
//
//  DESCRIPTION:   The main function
//
//  PARAMETERS:    argc - the argument count
//                 argv - the argument values
//
//  RETURN VALIES: 0 - exits normally
//                 anything else - abnormal exit
//
****************************************************************/

int main (int argc, char **argv) 
{
    int ret_flag = 0;
    int sock, port_number; 
    struct sockaddr_in server;
    FILE * file = fopen("file_client.html", "w"); 
    char c[50000];

    if (argc != 2)
    {
        printf("Error: incorrect amount of arguments.\nThe correct form is\n\'./server [Port Number]\'\n");
    }
    else
    {
        port_number = atoi(argv[1]);

        /* Create the Socket */
  
        printf("Creating socket...\n");

        sock = socket(PF_INET, SOCK_STREAM, 0);

        if (sock == -1)
        {
            printf("Error: not able to create socket.\n");
            ret_flag = 1;
        }
        else
        {
            printf("Socket created.\n");

            server.sin_addr.s_addr = inet_addr("127.0.0.1");
            server.sin_family = AF_INET;
            server.sin_port = htons(port_number);

            /* Connect to Server */

            printf("Connecting to server...\n");

            if (connect(sock, (struct sockaddr *) &server, sizeof(server)) < 0)
            {
                printf("Error: connection failed.\n");
                ret_flag = 1;
            }
            else
            {
                /* Receive Message from Server */

                printf("Connected.\nReceiving message from server...\n");

                while (read(sock, c, sizeof(c)))
                {
                    fputs(c, file);
                    bzero(c, sizeof(c));
                }
 
                printf("Message received.\n");
            }
        }
    }
    printf("Closing connection to server...\n");
    close(sock);
    printf("Connection closed.\n");
    return ret_flag; 
}

