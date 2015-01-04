#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h> 

#include <semaphore.h>
#include <fcntl.h>          /* O_CREAT, O_EXEC          */

#include "modbus_master.h"
#include "tcp.h"

sem_t *sem;

void terminar(int sig)
{
   sem_destroy (sem);
   close_server();
   exit(0);
}

int main( int argc, char *argv[] )
{
   signal(SIGCHLD,SIG_IGN); // termina procesos hijos en estado zombie
   signal(SIGINT, terminar);  // registrar señal para capturar interrupcion Ctrl+c

   sem = sem_open ("pSem", O_CREAT | O_EXCL, 0644, 1);   // abrir semaforo compartido entre procesos, si no existe, con valor de 1
   sem_unlink ("pSem");
   printf ("semaphores initialized.\n\n");
   
   open_modbus(100, 9600);
   inicializar_server_socket();
   escucha_socket(); // llamada al ciclo infinito de escucha
}
