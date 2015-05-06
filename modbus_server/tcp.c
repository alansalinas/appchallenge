#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <semaphore.h>
#include <inttypes.h>
#include <math.h>
#include <time.h>

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
#define SET_PID 0x36
#define TEST 0x37

#define TS 500

/*
   Variables globales para estructura de servidor TCP
*/
int sockfd, newsockfd, clilen;
   char buffer[256];
   struct sockaddr_in serv_addr, cli_addr;
   int  n, pid;
int x = 0;


long get_millis ()
{
    long            ms; // Milliseconds
    time_t          s;  // Seconds
    struct timespec spec;

    clock_gettime(CLOCK_REALTIME, &spec);

    s  = spec.tv_sec;
    ms = round(spec.tv_nsec / 1.0e6); // Convert nanoseconds to milliseconds

}

/*
   Rutina de servicio para server TCP
*/
void procesar_request (int sock)
{
   int n;
   uint16_t speed;
   int rc;
	int error;
   uint16_t res, feed;
   char buffer[256];
   char res_buf[30];
   unsigned long t, tao, init_time;
   
   bzero(buffer,256);   // limpiar buffer

   n = write(sock,buffer,1);

   if (n < 0)
      exit(0);
   else
      printf("N: %d\n",n);

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
         res = read_register(LOGIC_DATA_ADDR);
         //res = 0x001A; // rotating forward active, accelerating
         res_buf[0] = GET_STATUS;
         res_buf[1] = (res >> 8) & 0x00FF;
         res_buf[2] = res & 0x00FF;

         // send feedback
         res = read_register(FEEDBACK_ADDR);
         res_buf[3] = (res >> 8) & 0x00FF;
         res_buf[4] = res & 0x00FF;
         printf("0: %x, %x, %x\n",res_buf[0],res_buf[3],res_buf[4]);
         
         // send commanded
         res = read_register(COMMANDED_FREQ_ADDR);
         res_buf[5] = (res >> 8) & 0x00FF;
         res_buf[6] = res & 0x00FF;
         printf("0: %x, %x, %x\n",res_buf[0],res_buf[5],res_buf[6]);

         // send voltage
         res = read_register(OUTPUT_VOLTAGE_ADDR);
         res_buf[7] = (res >> 8) & 0x00FF;
         res_buf[8] = res & 0x00FF;
         printf("0: %x, %x, %x\n",res_buf[0],res_buf[7],res_buf[8]);
         // send current
         res = read_register(OUTPUT_CURRENT_ADDR);
         res_buf[9] = (res >> 8) & 0x00FF;
         res_buf[10] = res & 0x00FF;
         printf("0: %x, %x, %x\n",res_buf[0],res_buf[9],res_buf[10]);
        res_buf[11] = '\n'; 
	     n = write(sock,&res_buf,12);
      
         break;


      case START_MOTOR:
         printf("START MOTOR\n");
         rc = write_register(LOGIC_CMD_ADDR, 0x02);
         break;

      case STOP_MOTOR:
         printf("STOP MOTOR\n");
	reset_pid();
         rc = write_register(LOGIC_CMD_ADDR, 0x01);
         break;

      case SET_REFERENCE:
       printf("SET REFERENCE CMD\n");
         speed = 0;
         speed = (buffer[1] << 8) | (buffer[2]);
         reset_pid();
         rc = write_register(SPEED_REFERENCE_ADDR, speed);
         break;

      case SET_PID:
         printf("SET PID REFERENCE CMD\n");
         speed = (buffer[1] << 8) | (buffer[2]);

         res=read_register(FEEDBACK_ADDR);
        //set_pid_params(1.0, 1.0, 0.001, 10);
        printf("RES; %d, setpoint: %d\n",res,speed);
        error = speed - res;
        //printf("ERROR: %d, PID CORRECTION: %d \n",error, pid_control(error));
        rc = write_register(SPEED_REFERENCE_ADDR, pid_control(error));

        res = read_register(LOGIC_DATA_ADDR);
         //res = 0x001A; // rotating forward active, accelerating
         res_buf[0] = GET_STATUS;
         res_buf[1] = (res >> 8) & 0x00FF;
         res_buf[2] = res & 0x00FF;

         // send feedback
         res = read_register(FEEDBACK_ADDR);
         res_buf[3] = (res >> 8) & 0x00FF;
         res_buf[4] = res & 0x00FF;
         printf("0: %x, %x, %x\n",res_buf[0],res_buf[3],res_buf[4]);
         
         // send commanded
         res = read_register(COMMANDED_FREQ_ADDR);
         res_buf[5] = (res >> 8) & 0x00FF;
         res_buf[6] = res & 0x00FF;
         printf("0: %x, %x, %x\n",res_buf[0],res_buf[5],res_buf[6]);

         // send voltage
         res = read_register(OUTPUT_VOLTAGE_ADDR);
         res_buf[7] = (res >> 8) & 0x00FF;
         res_buf[8] = res & 0x00FF;
         printf("0: %x, %x, %x\n",res_buf[0],res_buf[7],res_buf[8]);
         // send current
         res = read_register(OUTPUT_CURRENT_ADDR);
         res_buf[9] = (res >> 8) & 0x00FF;
         res_buf[10] = res & 0x00FF;
         printf("0: %x, %x, %x\n",res_buf[0],res_buf[9],res_buf[10]);
        res_buf[11] = '\n'; 
        n = write(sock,&res_buf,12);

         break;

      case TEST:
         rc = write_register(SPEED_REFERENCE_ADDR, 100);

         // esperar referencia
         do
         {
            res = read_register(LOGIC_DATA_ADDR);

         } while((res & 0x0100) == 0);

         tao = 0;
         t = get_millis();
         init_time = get_millis();
         rc = write_register(SPEED_REFERENCE_ADDR, 400);

         do
         {
            if(get_millis() - t > TS)
            {
              feed = read_register(LOGIC_DATA_ADDR); 
              if (feed >= 253)   // 400*0.632 = 252.8
               tao = get_millis();

               t = get_millis();
            }

            res = read_register(LOGIC_DATA_ADDR);

         } while((res & 0x0100) == 0);

         printf("TAO = %lu milliseconds\n", tao - init_time);

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
         while(1){
         procesar_request(newsockfd);
         }
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


