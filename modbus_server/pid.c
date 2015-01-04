#include <stdio.h>
// Variables de memoria para el control PID
double integral, integral_1;
double derivativo, derivativo_1;

// Parametros pid para granancia proporcional, termino integral y termino derivativo
double Kp = 1;
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

void set_pid_params(double kp, double ti, double td)
{
	Kp = kp;
	Ti = ti;
	Td = td;

printf("Kp: %f, Ti: %f, Td %f\n",Kp, Ti, Td);
}

void stop_motor()
{

}

void manipulacion(int m)
{

}

 int pid_control(int error)
{
	printf("PID ERROR %d\n",error);

  if (integral < 0)	// acotamiento
    integral = 0;

  integral_1 = integral;

  if (integral_1 < 0)	// acotamiento
    integral_1 = 0;

  integral=(int)(Kp*(TS/Ti)*error + integral_1);

  if (derivativo < 0)	// acotamiento
    derivativo = 0;

  derivativo_1 = derivativo;

  if (derivativo_1 < 0)	// acotamiento
    derivativo_1 = 0;
	printf("K: %f, %d\n", Kp, (int)(Kp*error));
  derivativo =  (int)(Kp*(Td/TS)*error - derivativo_1);
	printf("PID CORR: %d\n", (int)(Kp*error + integral + derivativo));
  return (int)(Kp*error + integral + derivativo);
}
