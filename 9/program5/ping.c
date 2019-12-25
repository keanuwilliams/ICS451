/*****************************************************************
//
//  NAME:        Keanu Williams
//  HOMEWORK:    9
//  CLASS:       ICS 451
//  INSTRUCTOR:  Ravi Narayan
//  DATE:        December 2, 2019
//  FILE:        ping.c
//  DESCRIPTION: This file contains the source code for the
//               ping program. It is a software tool that sends
//               an X amount of packets and listens for an X
//               amount of packets in reply.
//
// RECEIVED HELP FROM: Jeffrey Suen, Sophia Kim, Kain Yogi, and Aaron Geronimo
****************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <sys/socket.h>
#include <sys/time.h>
#include "checksum.h"

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1
#define AMOUNT_OF_PACKETS 5

char *dns_lookup(char *addr_host, struct sockaddr_in *addr_con);
//char *reverse_dns_lookup(char *ip);

/* Packet to Send */
struct p_packet
{
   struct icmp icmp_h;
};

struct e_packet
{
   struct ip ip_h;
   struct icmp icmp_h;
};

/* https://www.geeksforgeeks.org/ping-in-c/ */
char *dns_lookup(char *addr_host, struct sockaddr_in *addr_con)
{
   struct hostent *host_entity;
   char *ip = (char *) malloc(NI_MAXHOST*sizeof(char));

   if ((host_entity = gethostbyname(addr_host)) == NULL)
   {
      ip = NULL;
   }
   else
   {
      strcpy(ip, inet_ntoa(* (struct in_addr *) host_entity->h_addr));
      (*addr_con).sin_family = host_entity->h_addrtype;
      (*addr_con).sin_port = htons(0);
      (*addr_con).sin_addr.s_addr = *(long*)host_entity->h_addr;
   }

   return ip;
}
/*
char *reverse_dns_lookup(char *ip)
{
   struct sockaddr_in temp;
   socklen_t len;
   char buf[NI_MAXHOST], *ret_buf;

   temp.sin_family = AF_INET;
   temp.sin_addr.s_addr = inet_addr(ip);
   len = sizeof(struct sockaddr_in);

   if (getnameinfo((struct sockaddr *) &temp, len, buf, sizeof(buf), NULL, 0, NI_NAMEREQD))
   {
      printf("ERROR: unable to resolve reverse lookup of hostname.\n");
      ret_buf = NULL;
   }
   else
   {
      ret_buf = (char *)malloc((strlen(buf)+1)*sizeof(char));
      strcpy(ret_buf, buf);
   }
   return ret_buf;
}
*/

/*****************************************************************
//
//  FUNCTION NAME: main
//  DESCRIPTION:   The main function.
//  PARAMETERS:    argc - the argument count
//                 argv - the argument values
//  RETURN VALUES: 0 - exits normally
//                 non-zero - abnormal exit
//
****************************************************************/

int main(int argc, char * argv[])
{
   char *hostname, *ip;
   unsigned short checksum_f, checksum_i;
   unsigned long time_f, time_i;
   int finished = 0, exit_code = EXIT_SUCCESS, r_socket = 0, i = 0, total_packets = 0;
   double total_time = 0, rtt = 0;
   struct addrinfo hints, *results;
   struct timeval time_interval;
   struct sockaddr_in addr_con;
   struct p_packet sent_packets[AMOUNT_OF_PACKETS];
   struct e_packet recv_packets[AMOUNT_OF_PACKETS];

   if (argc != 2) // check format
   {
      printf("ERROR: incorrect command-line format.\nFORMAT: sudo ./ping <URL>\n");
      exit_code = EXIT_FAILURE;
   }
   else
   {
      hostname = argv[1]; // get hostname from command-line
      memset(&hints, 0, sizeof(hints));
      hints.ai_family = AF_INET;
      hints.ai_flags = AI_CANONNAME;

      /* https://docs.microsoft.com/en-us/windows/win32/api/ws2tcpip/nf-ws2tcpip-getaddrinfo */
      /* https://www.binarytides.com/hostname-to-ip-address-c-sockets-linux */
      if (getaddrinfo(hostname, NULL, &hints, &results) != 0)
      {
         printf("ERROR: there was a problem connecting to the host %s.\n", hostname);
         exit_code = EXIT_FAILURE;
      }
      else
      {
         while (results != NULL && !finished)
         {
            if (strcmp(results->ai_canonname, hostname) == 0)
            {
               finished = 1;
            }
            results = results->ai_next;
         }
         printf("PING: %s\n", results->ai_canonname);
         /* https://docs.microsoft.com/en-us/windows/win32/api/ws2tcpip/nf-ws2tcpip-getnameinfohttps://docs.microsoft.com/en-us/windows/win32/api/ws2tcpip/nf-ws2tcpip-getnameinfo */
         ip = dns_lookup(hostname, &addr_con);
         //reverse_hostname = reverse_dns_lookup(ip);
         r_socket = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
         if (r_socket < 0)
         {
            printf("ERROR: there was a problem establishing the socket.\n");
            exit_code = EXIT_FAILURE;
         }
         else if(ip == NULL)
         {
            printf("ERROR: there was a problem with the DNS lookup.\n");
            exit_code = EXIT_FAILURE;
         }
         else
         {
            /* Send and Receive Packets */
            for (i = 0; i != AMOUNT_OF_PACKETS; ++i)
            {
               /* Prepare Packet to be Sent */
               //printf("Preparing packet %d to be sent...\n", i);
               gettimeofday(&time_interval, NULL);
               time_i = time_interval.tv_usec;
               sent_packets[i].icmp_h.icmp_type = 8;
               sent_packets[i].icmp_h.icmp_code = 0;
               sent_packets[i].icmp_h.icmp_cksum = 0;
               sent_packets[i].icmp_h.icmp_hun.ih_idseq.icd_id = htons(rand());
               sent_packets[i].icmp_h.icmp_hun.ih_idseq.icd_seq = i + 1;
               sent_packets[i].icmp_h.icmp_cksum = checksum((unsigned short*) &sent_packets[i], sizeof(struct p_packet));

               /* Send Packet */
               if (sendto(r_socket, &sent_packets[i], sizeof(struct p_packet), 0, (struct sockaddr*) results->ai_addr, sizeof(struct sockaddr)) < 0)
               {
                  printf("ERROR: there was a problem sending packet to socket.\n");
                  exit_code = EXIT_FAILURE;
               }
               else
               {
                  /* Receive Packet */
                  recvfrom(r_socket, &(recv_packets[i]), sizeof(struct e_packet), 0, (struct sockaddr*) results->ai_addr, &(results->ai_addrlen));
                  checksum_i = (&sent_packets[i].icmp_h)->icmp_cksum;
                  sent_packets[i].icmp_h.icmp_cksum = 0;
                  checksum_f = checksum((unsigned short*) (&sent_packets[i].icmp_h), sizeof(struct p_packet));

                  if (recv_packets[i].icmp_h.icmp_type == 0 && recv_packets[i].icmp_h.icmp_code == 0)
                  {
                     if (checksum_f == checksum_i)
                     {
                        gettimeofday(&time_interval, NULL);
                        time_f = time_interval.tv_usec;
                        rtt = (time_f - time_i) / 1000;
                        printf("%d bytes from %s: icmp_seq=%d ttl=%d time=%.2f ms\n", (int)sizeof(struct p_packet), ip, recv_packets[i].icmp_h.icmp_hun.ih_idseq.icd_seq, recv_packets[i].ip_h.ip_ttl, rtt);
                        total_packets++;
                        total_time += rtt;
                     }
                     else
                     {
                        printf("ERROR: there was a problem with the checksum.\n");
                        exit_code = EXIT_FAILURE;
                     }
                  }
                  else
                  {
                     printf("ERROR: there was a problem receiving packet from socket.\n");
                     exit_code = EXIT_FAILURE;
                  }
               }
               sleep(1);
            }
            printf("--- %s ping statistics ---\n", hostname);
            printf("%d packets transmitted, %d received, %d%% packet loss\n", AMOUNT_OF_PACKETS, total_packets, (AMOUNT_OF_PACKETS - total_packets) * 20);
            printf("rtt avg = %.2f ms\n", total_time/total_packets);
            shutdown(r_socket, 0);
         }
      }
   }
   return exit_code;
}
