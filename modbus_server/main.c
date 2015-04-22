#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h> 

#include "modbus_master.h"

#define PORT_NUM 5555

/*
   Variables globales para estructura de servidor TCP
*/
int sockfd, newsockfd, clilen;
   char buffer[256];
   struct sockaddr_in serv_addr, cli_addr;
   int  n, pid;


/*
   Rutina de servicio para server TCP
*/
void procesar_request (int sock)
{
   int n;
   char buffer[256];
   
   bzero(buffer,256);
   
   n = read(sock,buffer,255);
   
   if (n < 0)
      {
      perror("ERROR reading from socket");
      exit(1);
      }
   
   printf("Here is the message: %s\n",buffer);
   n = write(sock,"I got your message",18);
   
   if (n < 0)
      {
      perror("ERROR writing to socket");
      exit(1);
      }
}

/*
   Rutina de incializacion del servidor TCP
*/

void inicializar_server_socket(){

    /* First call to socket() function */
   sockfd = socket(AF_INET, SOCK_STREAM, 0);
   
   if (sockfd < 0)
      {
      perror("ERROR opening socket");
      exit(1);
      }
   
   /* Initialize socket structure */
   bzero((char *) &serv_addr, sizeof(serv_addr));
   
   serv_addr.sin_family = AF_INET;
   serv_addr.sin_addr.s_addr = INADDR_ANY;
   serv_addr.sin_port = htons(PORT_NUM);
   
   /* Now bind the host address using bind() call.*/
   if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
      {
      perror("ERROR on binding");
      exit(1);
      }

}


/*
   Rutina de escucha para servidor TCP, una vez llamada el programa entra en ciclo infinito
*/
   void escucha_socket()
   {
/* Now start listening for the clients, here
   * process will go in sleep mode and will wait
   * for the incoming connection
   */
   
   listen(sockfd,5);
   clilen = sizeof(cli_addr);
   
   while (1)
   {
      newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
      if (newsockfd < 0)
         {
         perror("ERROR on accept");
         exit(1);
         }
      
      /* Create child process */
      pid = fork();
      if (pid < 0)
         {
         perror("ERROR on fork");
         exit(1);
         }
      
      if (pid == 0)
         {
         /* This is the client process */
         close(sockfd);
         procesar_request(newsockfd);
         exit(0);
         }
      else
         {
         close(newsockfd);
         }
   } /* end of while */

   }  // end escucha socket


int main( int argc, char *argv[] )
{

   signal(SIGCHLD,SIG_IGN); // termina procesos hijos en estado zombie
   
   open_modbus();

   inicializar_server_socket();
   escucha_socket(); // llamada al ciclo infinito de escucha
}
