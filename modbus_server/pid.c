#include <stdio.h>

#define KP 1.0
#define TI 500.0
#define TD 1.0

// Variables de memoria para el control PID
double integral, integral_1;
double derivativo, derivativo_1;

// Parametros pid para granancia proporcional, termino integral y termino derivativo
double Kp = 0.8;
double Ti = 1;
double Td = 1;


unsigned int TS;	// Tiempo de muestreo

void reset_pid()
{
	Kp = 0;
	Ti = 0;
	Td = 0;

  integral = 0;
  integral_1= 0;
  derivativo = 0;
  derivativo_1 = 0;
}

void set_pid_params(double kp, double ti, double td, int ts)
{
	Kp = kp;
	Ti = ti;
	Td = td;
    TS = ts;

  printf("SET PARAMS Kp: %f, Ti: %f, Td %f, TS: %d\n",Kp, Ti, Td, TS);
}

void stop_motor()
{

}

void manipulacion(int m)
{

}

 int pid_control(int error)
{
  int correction;
	printf("PID ERROR recieved: %d\n",error);

  if (integral < 0)	// acotamiento
    integral = 0;

  integral_1 = integral;

  if (integral_1 < 0)	// acotamiento
    integral_1 = 0;

  integral=(int)(KP*(TS/TI)*error + integral_1);

  if (derivativo < 0)	// acotamiento
    derivativo = 0;

  derivativo_1 = derivativo;

  if (derivativo_1 < 0)	// acotamiento
    derivativo_1 = 0;

  derivativo =  (int)(KP*(TD/TS)*error - derivativo_1);

    printf("Termino proporcional: %f, %d\n", KP, (int)(KP*error));
    printf("Termino integral: %f, %d\n", TI, (int)integral);
    printf("Termino derivativo: %f, %d\n", TD, (int)derivativo);

	printf("PID CORRECTION: %d\n", (int)(KP*error + integral + derivativo));
    correction =(int)(KP*error + integral + derivativo);

    if(correction > 600)
      correction = 600;

    if(correction < 0)
      correction = 0;

    return correction;
}
