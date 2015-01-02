#include <stdio.h>
#include <modbus.h>

//powerflex 40 registers
// PARA ESCRIBIR
#define LOGIC_CMD 8192
#define LOGIC_DATA 8448
#define SPEED_REFERENCE 8193

// PARA LEER
#define COMMANDED_FREQ 8450
#define FEEDBACK 8451
#define OUTPUT_CURRENT 8452
#define OUTPUT_VOLTAGE 8453
#define ERROR_CODES 8449

modbus_t *ctx;
int errno;


int open_modbus()
{
ctx = modbus_new_rtu("/dev/ttyUSB0", 9600, 'N', 8, 1);

if (ctx == NULL) {
    fprintf(stderr, "Unable to create the libmodbus context\n");
    return -1;
}

if (modbus_connect(ctx) == -1) {
    fprintf(stderr, "Connection failed: %s\n", modbus_strerror(errno));
    modbus_free(ctx);
    return -1;
}

printf("Modbus RTU context created at 9600, N, 8bit, 1\n");

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
	uint16_t registers[4];
	registers[0]=10;
	modbus_set_slave(ctx,100);
	//rc = modbus_read_registers(ctx, COMMANDED_FREQ, 1, registers);
	rc = modbus_write_register(ctx, LOGIC_CMD, 0x01);
	rc = modbus_write_register(ctx, SPEED_REFERENCE, 000);
	if (rc == -1) {
    		fprintf(stderr, "%s\n", modbus_strerror(errno));
    		return -1;
		}

	return registers[0];

}

