#include <stdio.h>
#include <modbus.h>

modbus_t *ctx;
int errno;


int open_modbus()
{
ctx = modbus_new_rtu("/dev/ttyUSB0", 115200, 'N', 8, 1);

if (ctx == NULL) {
    fprintf(stderr, "Unable to create the libmodbus context\n");
    return -1;
}

if (modbus_connect(ctx) == -1) {
    fprintf(stderr, "Connection failed: %s\n", modbus_strerror(errno));
    modbus_free(ctx);
    return -1;
}

printf("Modbus RTU context created at 115200, N, 8bit, 1\n");

return 0;

}  // end open_modbus

void close_modebus()
{
  modbus_close(ctx);
  modbus_free(ctx);

  printf("Modbus closed\n");
}

uint16_t read_register()
{
	int rc;
	uint16_t registers[64];

	rc = modbus_read_registers(ctx, 0, 10, registers);

	if (rc == -1) {
    		fprintf(stderr, "%s\n", modbus_strerror(errno));
    		return -1;
		}

	return registers[0];

}

