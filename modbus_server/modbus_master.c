#include <stdio.h>
#include <modbus.h>
#include "modbus_master.h"

modbus_t *ctx;
int errno;

int open_modbus(unsigned short slave_address, unsigned short baud_rate)
{
ctx = modbus_new_rtu("/dev/ttyUSB0", baud_rate, 'N', 8, 1);

if (ctx == NULL) {
    fprintf(stderr, "Unable to create the libmodbus context\n");
    return -1;
}

if (modbus_connect(ctx) == -1) {
    fprintf(stderr, "Connection failed: %s\n", modbus_strerror(errno));
    modbus_free(ctx);
    return -1;
}

printf("Modbus RTU context created at %u, N, 8bit, 1\n", baud_rate);

modbus_set_slave(ctx,slave_address);

printf("Set comm slave address: %u\n", slave_address);

return 0;

}  // end open_modbus

void close_modebus()
{
  modbus_close(ctx);
  modbus_free(ctx);

  printf("Modbus closed\n");
}

uint16_t read_register(uint16_t register_addr)
{
	int rc;
	uint16_t registers[4];
	
	rc = modbus_read_registers(ctx, register_addr, 1, registers); // leer registro de conexion ctx

	if (rc == -1) {
    		fprintf(stderr, "%s\n", modbus_strerror(errno));
    		return -1;
		}

	return registers[0];
} // end read register

int write_register(uint16_t register_addr, uint16_t value)
{
  int rc;

  rc = modbus_write_register(ctx, register_addr, value);

  if (rc == -1) {
        fprintf(stderr, "%s\n", modbus_strerror(errno));
        return -1;
    }
} // end write_register

params read_params()
{
  params read_params;

  read_params.feedback = read_register(FEEDBACK_ADDR);
  read_params.commanded_freq = read_register(COMMANDED_FREQ_ADDR);
  read_params.output_current = read_register(OUTPUT_CURRENT_ADDR);
  read_params.output_voltage = read_register(OUTPUT_VOLTAGE_ADDR);

  return read_params;
}





