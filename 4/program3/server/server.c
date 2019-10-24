/*****************************************************************
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
    int port_number, server_socket, client_socket, b;
    FILE * file = fopen("file_server.html", "r");
    char c[50000];
    struct sockaddr_in server;

    /* Check If File Exists */

    if (file == NULL)
    {
        printf("Error: file not found.\n");
        ret_flag = 1;
    }
    else
    {
        /* Error Check Port Number Input */

        if (argc != 2) 
        {
             printf("Error: incorrect amount of arguments.\nThe correct form is\n\'./server [Port Number]\'\n");
        }
        else 
        {
            port_number = atoi(argv[1]);

            /* Create Socket */
       
            printf("Creating socket...\n");
 
            server_socket = socket(PF_INET, SOCK_STREAM, 0);
            if (server_socket == -1) 
            {
                printf("Error: not able to create socket.\n");
                ret_flag = 1;
            }
            else 
            {
                printf("Socket created.\n");

                /* Bind Socket */

                server.sin_family = AF_INET;
                server.sin_addr.s_addr = INADDR_ANY;
                server.sin_port = htons(port_number); 

                printf("Binding socket...\n");

                if (bind(server_socket, (struct sockaddr *) &server, sizeof(server)) < 0)
                {
                    printf("Error: bind failed.\n");
                    ret_flag = 1;
                }
                else
                {
                    printf("Bind successful.\n");

                    /* Listen for Connections */     

                    if (!listen(server_socket, 5))
                    {
                        /* Accept Incoming Connection */
 
                        printf("Awaiting connection...\n");
                        client_socket = accept(server_socket, (struct sockaddr *) NULL, NULL);
                        printf("Connected.\nSending file to client...\n");
                        while (fgets(c, sizeof(c), file) != NULL)
                        {
                            write(client_socket, c, strlen(c));
                        }
                        printf("File sent.\nClosing connection to client...\n");
                        close(client_socket);
                        close(server_socket);
                        printf("Connection to client closed.\n");
                    }
                    else
                    {
                        printf("Error: unable to accept connections.\n");
                        ret_flag = 1;
                    }
                }
            }
        }    
    }
    
    return ret_flag; 

}

