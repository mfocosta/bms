/* Defines */
#define I2C_MASTER_NUM              0                          /*!< I2C master i2c port number, the number of i2c peripheral interfaces available will depend on the chip */
#define I2C_MASTER_FREQ_HZ          400000                     /*!< I2C master clock frequency */
#define I2C_MASTER_TX_BUF_DISABLE   0                          /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE   0                          /*!< I2C master doesn't need buffer */
#define I2C_MASTER_TIMEOUT_MS       1000
#define I2C_MASTER_SCL_IO           22
#define I2C_MASTER_SDA_IO           21

const char *TAG = "bms-test";

/* Prototypes */
static esp_err_t i2c_master_init(void);
void fetsTask (void* pvParameters);
void printInfoTask (void* pvParameters);
static esp_err_t writeToEEprom();
static esp_err_t writeReg(uint8_t regAddress, uint8_t writeValue);
static esp_err_t writeRegBit(uint8_t regAddress, uint8_t writeValue, uint8_t offset);
static esp_err_t readReg(uint8_t regAddress);
