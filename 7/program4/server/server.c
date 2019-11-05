/*****************************************************************
//
//  NAME:        Keanu Williams
//
//  HOMEWORK:    7
//
//  CLASS:       ICS 451
//
//  INSTRUCTOR:  Ravi Narayan
//
//  DATE:        November 3, 2019
//
//  FILE:        server.c
//
//  DESCRIPTION: This file contains the source code for the server.
//
****************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define DEBUG 0 // make 1 when debugging and 0 when not

void print_header_info(unsigned char * header);

/*****************************************************************
//
//  FUNCTION NAME: main
//
//  DESCRIPTION:   The main function
//
//  PARAMETERS:    argc - the argument count
//                 argv - the argument values
//
//  RETURN VALUES: 0 - exits normally
//                 anything else - abnormal exit
//
****************************************************************/

int main(int argc, char **argv)
{
    int ret_flag = 0;
    int done = 0; // determines when to close socket
    int i, source_port, dest_port, s_socket, c_socket;
    struct sockaddr_in server;
    unsigned char header[20];
    unsigned char temp[4];

    /* Error Check Port Number */
    if (argc != 2)
    {
        printf("Error: incorrect amount of arguments.\nThe correct form is\n\'./server [Port Number]\'\n");
        ret_flag = 1;
    }
    else
    {
        dest_port = atoi(argv[1]);

        /* Create Socket */
        printf("Creating socket...");
        s_socket = socket(PF_INET, SOCK_STREAM, 0);
        if (s_socket == -1)
        {
            printf("\nError: not able to create server socket.\n");
            ret_flag = 1;
        }
        else
        {
            printf("Socket created.\n");

            /* Bind Socket */
            server.sin_family = AF_INET;
            server.sin_addr.s_addr = INADDR_ANY;
            server.sin_port = htons(dest_port);
            printf("Binding socket...");
            if (bind(s_socket, (struct sockaddr *) &server, sizeof(server)) < 0)
            {
                printf("\nError: bind failed.\n");
                ret_flag = 1;
            }
            else
            {
                printf("Bind successful.\n");

                /* Listen for Connections */
                if (!listen(s_socket, 5))
                {
                    while(!done)
                    {
                        /* Accept Incoming Connection */
                        printf("Awaiting connection...");
                        c_socket = accept(s_socket, (struct sockaddr *) NULL, NULL);
                        printf("Connected.\nAwaiting SYN from client...");
                        recv(c_socket, header, 20, 0);
                        /************************************************************
                         * TCP Header (20 bytes)
                         * (1) Source TCP Port Number (2 bytes) // header[0-1]
                         * (2) Destination TCP Port Number (2 bytes) // header[2-3]
                         * (3) Sequence Number (4 bytes) // header[4-7]
                         * (4) Acknowledgment Number (4 bytes) // header[8-11]
                         * (5) TCP Data Offset (4 bits) // header[12]
                         * (6) Reserved Data (3 bits) // header[12]
                         * (7) Control Flags (9 bits) // header[12-13]
                         * ||NS||CWR|ECE|URG|ACK|PSH|RST|SYN|FIN||
                         * (8) Window Size (2 bytes) // header[14-15]
                         * (9) TCP Checksum (2 bytes) // header[16-17]
                         *(10) Urgent Pointers (2 bytes) // header[18-19]
                         ************************************************************/
                        if (DEBUG)
                        {
                            printf("\n");
                            for (i = 0; i < sizeof(header); i++)
                            {
                                printf("DEBUG: header[%d] = %x\n", i, header[i]);
                            }
                            /*dest_port = header[0];
                            dest_port = (dest_port << 8);
                            dest_port = (dest_port | header[1]);
                            source_port = header[2];
                            source_port = (source_port << 8);
                            source_port = (source_port | header[3]);
                            printf("DEBUG: source_port = %u\n", source_port);
                            printf("DEBUG: dest_port = %u\n", dest_port);*/
                        }
                        if (header[13] == 0x02) // ack flag not set and syn flag set
                        {
                            printf("SYN received.\n");
                            print_header_info(header);
                            printf("Preparing to send SYN-ACK to client...");
                            /* Switch source and dest ports in header */
                            temp[0] = header[0];
                            temp[1] = header[1];
                            temp[2] = header[2];
                            temp[3] = header[3];
                            header[0] = temp[2];
                            header[1] = temp[3];
                            header[2] = temp[0];
                            header[3] = temp[1];
                            if (DEBUG)
                            {
                                for (i = 0; i < sizeof(temp); i++)
                                {
                                    printf("DEBUG: temp[%d] = %x\n", i, temp[i]);
                                }
                            }
                            /* Adjust Acknowledgment Number */
                            /* https://packetlife.net/blog/2010/jun/7/understanding-tcp-sequence-acknowledgment-numbers/ */
                            header[8] = header[4];
                            header[9] = header[5];
                            header[10] = header[6];
                            header[11] = header[7] + 0x01;

                            header[13] = 0x12; // syn-ack set

                            printf("Finished.\n");
                            print_header_info(header);
                            if (DEBUG)
                            {
                                printf("\n");
                                for (i = 0; i < sizeof(header); i++)
                                {
                                    printf("DEBUG: header[%d] = %x\n", i, header[i]);
                                }
                            }
                            printf("Sending to client...");
                            send(c_socket, header, 20, 0);
                            printf("Sent.\nAwaiting ACK from client...");
                            recv(c_socket, header, 20, 0);
                            if (DEBUG)
                            {
                                printf("\n");
                                for (i = 0; i < sizeof(header); i++)
                                {
                                    printf("DEBUG: header[%d] = %x\n", i, header[i]);
                                }
                            }
                            if (header[13] == 0x10) // ack flag set and syn flag not set
                            {
                                printf("ACK received.\n");
                                print_header_info(header);
                                printf("Connection established.\n");
                            }
                            else
                            {
                                printf("ACK not received.\nClosing connection...");
                                close(c_socket);
                                close(s_socket);
                                done = 1;
                            }
                        }
                        else
                        {
                            printf("SYN not received.\nClosing connection...");
                            close(c_socket);
                            close(s_socket);
                            done = 1;
                        }
                    }
                    printf("Connection closed.\n");
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

/*****************************************************************
//
//  FUNCTION NAME: print_header_info
//
//  DESCRIPTION:   Prints the TCP header information
//
//  PARAMETERS:    header - the header array
//
//  RETURN VALUES: N/A
//
****************************************************************/

void print_header_info(unsigned char * header)
{
    int i;
    uint8_t data_offset, ns, cwr, ece, urg, ack, psh, rst, syn, fin;
    uint16_t source_port, dest_port, window_size, checksum, urgent_pointer;
    uint32_t seq_num, ack_num;

    printf("TCP Header: ");
    for (i = 0; i < sizeof(header); i++)
    {
        printf("%x", header[i]);
    }
    printf("\n");
    source_port = header[0];
    source_port = (source_port << 8);
    source_port = (source_port | header[1]);
    dest_port = header[2];
    dest_port = (dest_port << 8);
    dest_port = (dest_port | header[3]);
    seq_num = header[4];
    seq_num = (seq_num << 8);
    seq_num = (seq_num | header[5]);
    seq_num = (seq_num << 8);
    seq_num = (seq_num | header[6]);
    seq_num = (seq_num << 8);
    seq_num = (seq_num | header[7]);
    ack_num = header[8];
    ack_num = (ack_num << 8);
    ack_num = (ack_num | header[9]);
    ack_num = (ack_num << 8);
    ack_num = (ack_num | header[10]);
    ack_num = (ack_num << 8);
    ack_num = (ack_num | header[11]);
    data_offset = (header[12] >> 4);
    ns = (header[12] & 0x01);
    cwr = (header[13] >> 7);
    cwr = (cwr & 0x01);
    ece = (header[13] >> 6);
    ece = (ece & 0x01);
    urg = (header[13] >> 5);
    urg = (urg & 0x01);
    ack = (header[13] >> 4);
    ack = (ack & 0x01);
    psh = (header[13] >> 3);
    psh = (psh & 0x01);
    rst = (header[13] >> 2);
    rst = (rst & 0x01);
    syn = (header[13] >> 1);
    syn = (syn & 0x01);
    fin = (header[13] & 0x01);
    window_size = header[14];
    window_size = (window_size << 8);
    window_size = (window_size | header[15]);
    checksum = header[16];
    checksum = (checksum << 8);
    checksum = (checksum | header[17]);
    urgent_pointer = header[18];
    urgent_pointer = (urgent_pointer << 8);
    urgent_pointer = (urgent_pointer | header[19]);
    printf("Source TCP Port Number: %u\n", source_port);
    printf("Destination TCP Port Number: %u\n", dest_port);
    printf("Sequence Number: %u\n", seq_num);
    printf("Acknowledgment Number: %u\n", ack_num);
    printf("Data Offset: %u\n", data_offset);
    printf("Reserved: 0\n");
    printf(" NS Flag: %u\n", ns);
    printf("CWR Flag: %u\n", cwr);
    printf("ECE Flag: %u\n", ece);
    printf("URG Flag: %u\n", urg);
    printf("ACK Flag: %u\n", ack);
    printf("PSH Flag: %u\n", psh);
    printf("RST Flag: %u\n", rst);
    printf("SYN Flag: %u\n", syn);
    printf("FIN Flag: %u\n", fin);
    printf("Window Size: %u\n", window_size);
    printf("Checksum: %u\n", checksum);
    printf("Urgent Pointer: %u\n", urgent_pointer);
}
