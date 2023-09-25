/* Includes */
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include "driver/i2c.h"
#include "Registers.h"
#include "esp_log.h"



/* Defines */
#define I2C_MASTER_NUM              0                          /*!< I2C master i2c port number, the number of i2c peripheral interfaces available will depend on the chip */
#define I2C_MASTER_FREQ_HZ          400000                     /*!< I2C master clock frequency */
#define I2C_MASTER_TX_BUF_DISABLE   0                          /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE   0                          /*!< I2C master doesn't need buffer */
#define I2C_MASTER_TIMEOUT_MS       1000
#define I2C_MASTER_SCL_IO 22
#define I2C_MASTER_SDA_IO 21

static const char *TAG = "bms-test";

/**
 * @brief i2c master initialization
 */
static esp_err_t i2c_master_init(void)
{
    int i2c_master_port = I2C_MASTER_NUM;

    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ,
    };

    i2c_param_config(i2c_master_port, &conf);

    return i2c_driver_install(i2c_master_port, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);
}

static esp_err_t writeToEEprom() {


	return (false);
}


void app_main() {
    uint8_t reg_value;
    uint8_t cellSelectReg = 0x49;
    uint8_t write_buf[2] = {0x49, 0b11000011}; /* 4 cells */
    i2c_master_init();
    i2c_master_write_read_device(I2C_MASTER_NUM, ISL94202_ADDR, &cellSelectReg, 1, &reg_value, 1, I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
    printf("Valor do registo inicial: %02x\n", reg_value);
    i2c_master_write_to_device(I2C_MASTER_NUM, ISL94202_ADDR, write_buf, sizeof(write_buf), I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
    i2c_master_write_read_device(I2C_MASTER_NUM, ISL94202_ADDR, &cellSelectReg, 1, &reg_value, 1, I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
    printf("Valor do registo apos escrita: %02x\n", reg_value);


    ESP_ERROR_CHECK(i2c_driver_delete(I2C_MASTER_NUM));
    ESP_LOGI(TAG, "I2C de-initialized successfully");
}
