//powerflex 40 registers

// REGISTROS PARA ESCRIBIR
#define LOGIC_CMD_ADDR 8192
#define	 SPEED_REFERENCE_ADDR 8193

// REGISTROS PARA LEER
#define LOGIC_DATA_ADDR 8448
#define ERROR_CODES_ADDR 8449
#define COMMANDED_FREQ_ADDR 8450
#define FEEDBACK_ADDR 8451
#define OUTPUT_CURRENT_ADDR 8452
#define OUTPUT_VOLTAGE_ADDR 8453

typedef struct params{
  uint16_t commanded_freq;
  uint16_t feedback;
  uint16_t output_current;
  uint16_t output_voltage;
} params;

int open_modbus(unsigned short slave_address, unsigned short baud_rate);
void close_modebus();
uint16_t read_register(uint16_t register_addr);
int write_register(uint16_t register_addr, uint16_t value);