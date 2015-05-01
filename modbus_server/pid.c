
// Variables de memoria para el control PID
double integral, integral_1;
double derivativo, derivativo_1;

// Parametros pid para granancia proporcional, termino integral y termino derivativo
double Kp;
double Ti;
double Td;

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
}

void stop_motor()
{

}

void manipulacion(int m)
{

}

 int pid_control(int error)
{

  if (error < 0)
    error -= 2*error;

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

  derivativo =  (int)(Kp*(Td/TS)*error - derivativo_1);

  return (int)(Kp*error + integral + derivativo);
}