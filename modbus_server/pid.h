
void reset_pid();
void set_pid_params(double kp, double ti, double td);
int pid_control(int error);
