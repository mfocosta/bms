/* Includes */
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include "Registers.h"
#include "driver/i2c.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"



/* Defines */
#define I2C_MASTER_NUM              0                          /*!< I2C master i2c port number, the number of i2c peripheral interfaces available will depend on the chip */
#define I2C_MASTER_FREQ_HZ          400000                     /*!< I2C master clock frequency */
#define I2C_MASTER_TX_BUF_DISABLE   0                          /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE   0                          /*!< I2C master doesn't need buffer */
#define I2C_MASTER_TIMEOUT_MS       1000
#define I2C_MASTER_SCL_IO           22
#define I2C_MASTER_SDA_IO           21

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

    return (i2c_driver_install(i2c_master_port, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0));
}

/**
 * @brief write to EEprom
 */
static esp_err_t writeToEEprom() {


	return (ESP_OK);
}

/**
 * @brief write to Register
 */
static esp_err_t writeReg(uint8_t regAddress, uint8_t *write_buf) {
	uint8_t regValue;
    ESP_ERROR_CHECK_WITHOUT_ABORT(i2c_master_write_read_device(I2C_MASTER_NUM, ISL94202_ADDR, &regAddress, sizeof(regAddress), &regValue, sizeof(regValue), I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS));
    printf("Initial %02x register value: %02x\n", regAddress, regValue);
    ESP_ERROR_CHECK_WITHOUT_ABORT(i2c_master_write_to_device(I2C_MASTER_NUM, ISL94202_ADDR, write_buf, sizeof(write_buf), I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS));
    ESP_ERROR_CHECK_WITHOUT_ABORT(i2c_master_write_read_device(I2C_MASTER_NUM, ISL94202_ADDR, &regAddress, sizeof(regAddress), &regValue, sizeof(regValue), I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS));
    printf("Register %02x value after write: %02x\n", regAddress, regValue);

	return (ESP_OK);
}


void showMainMenu() {
    printf("Main Menu:\n");
    printf("1. Register 4 cells\n");
    printf("2. Option 2\n");
    printf("3. Exit\n");
}

void handleOption(int option) {
    switch (option) {
        case 1:
            printf("Sending 4 cells info...\n");
            uint8_t write_buf[2] = {CELL_SELECT_REG, 0b11000011}; /* 4 cells */
            writeReg(CELL_SELECT_REG, write_buf);
            break;
        case 2:
            printf("You selected Option 2.\n");
            break;
        case 3:
            printf("Exiting...\n");
            break;
        default:
            printf("Invalid option. Please try again.\n");
    }
}

void processCommand(char* command) {
    int option = atoi(command);
    handleOption(option);
}

/**
 * @brief main function
 */
void app_main() {
    char command[10];

    i2c_master_init();
    
    while (1) {
        showMainMenu();

        printf("Enter option: ");
        fflush(stdout);

        // Read user input from the command line
        fgets(command, sizeof(command), stdin);

        // Remove newline character from the end of the string
        strtok(command, "\n");

        // Process the command
        processCommand(command);

        // Check if user wants to exit
        if (atoi(command) == 3) {
            break;
        }
    }

    ESP_ERROR_CHECK(i2c_driver_delete(I2C_MASTER_NUM));
    ESP_LOGI(TAG, "I2C de-initialized successfully");
}
