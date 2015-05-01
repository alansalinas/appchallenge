#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <semaphore.h>

#include "modbus_master.h"

#define PORT_NUM 5555   // puerto tcp de escucha para el servidor
#define SLAVE_ADDRESS 100  // direccion comm modbus del variador de frecuencia
#define MODBUS_BAUD_RATE 9600  // velocidad de transmision del protocolo modbus

// definiciones de comandos de request provenientes del app movil
#define GET_STATUS 0x31
#define GET_PARAMS 0x32
#define START_MOTOR 0x33
#define STOP_MOTOR 0x34
#define SET_REFERENCE 0x35

#define GET_FEEDBACK 0x55
#define GET_COMMANDED 0x56
#define GET_VOLTAGE 0x57
#define GET_CURRENT 0x58

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
   int speed;
   int rc;
   uint16_t res;
   char buffer[256];
   char res_buf[3];
   
   bzero(buffer,256);   // limpiar buffer
   
   n = read(sock,buffer,255); // leer buffer de socket
   
   if (n < 0)
      {
      perror("ERROR reading from socket");
      exit(1);
      }

      //checar si hay verde
      // operacion P
      // else
      // contesta NACK
   
   switch (buffer[0])
   {
      case GET_STATUS:
         printf("GET_STATUS REQUEST\n");
         //res = read_register(LOGIC_DATA_ADDR);
         res = 0x000A;
         n = write(sock,&res,16);
         break;

      case GET_PARAMS:
         printf("GET_PARAMS REQUEST\n");
         /*
         // send feedback
         res = read_register(FEEDBACK_ADDR);
         res_buf[0] = GET_FEEDBACK;
         res_buf[1] = (res >> 8) & 0x00FF;
         res_buf[2] = res & 0x00FF;
         n = write(sock,res_buf,24);

         // send commanded
         res = read_register(COMMANDED_FREQ_ADDR);
         res_buf[0] = GET_COMMANDED;
         res_buf[1] = (res >> 8) & 0x00FF;
         res_buf[2] = res & 0x00FF;
         n = write(sock,res_buf,24)

         // send voltage
         res = read_register(OUTPUT_VOLTAGE_ADDR);
         res_buf[0] = GET_VOLTAGE;
         res_buf[1] = (res >> 8) & 0x00FF;
         res_buf[2] = res & 0x00FF;
         n = write(sock,res_buf,24)

         // send current
         res = read_register(OUTPUT_CURRENT_ADDR);
         res_buf[0] = GET_CURRENT;
         res_buf[1] = (res >> 8) & 0x00FF;
         res_buf[2] = res & 0x00FF;
         n = write(sock,res_buf,24)
      */
         break;

      case START_MOTOR:
         printf("START MOTOR\n");
         //rc = write_register(LOGIC_CMD_ADDR, 0x02);
         //n = write(sock, "START_MOTOR response\n", 21);
         break;

      case STOP_MOTOR:
         printf("START MOTOR\n");
         //rc = write_register(LOGIC_CMD_ADDR, 0x01);
         break;

      case SET_REFERENCE:
         speed = 0;
         speed = (buffer[1] << 8) | (buffer[2]);
         printf("SET REFERENCE CMD: %d\n", speed);
         //rc = write_register(SPEED_REFERENCE_ADDR, speed);
         break;

      default:
         printf("UNRECOGNIZED COMMAND: %s\n", buffer);
         break;   
      }
   
   if (n < 0)
      {
      perror("ERROR writing to socket");
      exit(1);
      }

      // operacion V
}

/*
   Rutina de incializacion del servidor TCP
*/

void inicializar_server_socket(){

   sockfd = socket(AF_INET, SOCK_STREAM, 0); // crear socket tcp
   
   if (sockfd < 0)
      {
      perror("ERROR opening socket");
      exit(1);
      }
   
   /* Inicializar estructura de socket */
   bzero((char *) &serv_addr, sizeof(serv_addr));
   
   serv_addr.sin_family = AF_INET;  // protocolo IPv4
   serv_addr.sin_addr.s_addr = INADDR_ANY;   // cualquier direccion de interfaz local
   serv_addr.sin_port = htons(PORT_NUM);  // declarar el puerto de escucha tcp
   
   /* hacer bind de la direccion con el socket */
   if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
      {
      perror("ERROR on binding");
      exit(1);
      }

}


/*
   Rutina de escucha para servidor TCP, una vez llamada el programa entra en ciclo infinito.
   El proceso escucha a traves del socket inicializado, el proceso hiberna hasta que hay una conexion entrante
   entonces crea un proceso hijo nuevo para atender la peticion
*/
void escucha_socket()
   {

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
      
      /* Crear proceso hijo */
      pid = fork();
      if (pid < 0)
         {
         perror("ERROR on fork");
         exit(1);
         }
      
      if (pid == 0)
         {
         /* Este punto se encuentra en el nuevo proceso hijo */
         close(sockfd);
         procesar_request(newsockfd);
         exit(0);
         }
      else
         {
         close(newsockfd);
         }
   } // end while

   }  // end escucha socket

   void close_server()
   {
      close(sockfd);
   }


