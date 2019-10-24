/*****************************************************************
//
//  NAME:        Keanu Williams
//
//  HOMEWORK:    3
//
//  CLASS:       ICS 451
//
//  INSTRUCTOR:  Ravi Narayan
//
//  DATE:        September 24, 2019
//
//  FILE:        server.c
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
    int port_number, server_socket, client_socket;
    char c[1024];
    char * msg = "Hello Keanu. ICS 451 is fun!\n"; 
    struct sockaddr_in server;

    /* Error Check Port Number Input */

    if (argc != 2) 
    {
        printf("Error: incorrect amount of arguments.\nThe correct form is\n\'./server [Port Number]\'\n");
    }
    else 
    {
        port_number = atoi(argv[1]);

        /* Create Socket */
        
        server_socket = socket(PF_INET, SOCK_STREAM, 0);
        if (server_socket == -1) 
        {
            printf("Error: not able to create socket.\n");
        }
        else 
        {
            printf("Socket created.\n");

            /* Bind Socket */

            server.sin_family = AF_INET;
            server.sin_addr.s_addr = INADDR_ANY;
            server.sin_port = htons(port_number); 

            printf("Binding socket...\n");

            if (bind (server_socket, (struct sockaddr *) &server, sizeof(server)) < 0)
            {
                printf("Error: bind failed.\n");
                ret_flag = 1;
            }
            else
            {
                printf("Bind successful.\n");

                /* Listen for Connections */     

                if (!listen(server_socket, 3))
                {
                    printf("Looking for connections...\n");
                    snprintf(c, sizeof(c), "%s\n", msg);

                    /* Accept Incoming Connection */
 
                    while (1)
                    {
                        printf("Awaiting connection...\n");
                        client_socket = accept(server_socket, (struct sockaddr *) NULL, NULL);
                        printf("Connected.\nSending message to client %d.\n", client_socket);
                        write(client_socket, c, strlen(c));
                        printf("Message sent.\nClosing connection to client %d.\n", client_socket);
                        close(client_socket);
                        printf("Connection to client %d closed.\n", client_socket);
                    }
                }
                else
                {
                    printf("Error: unable to accept connections.\n");
                    ret_flag = 1;
                }
            }
        }
    }    
    
    return ret_flag; 

}

