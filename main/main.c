/* Includes */
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include "driver/i2c.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "Registers.h"
#include "main_header.h"

portMUX_TYPE mux;

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

void fetsTask (void* pvParameters){
	//writeRegBit(CONTROL_2, 1, 6);
	taskENTER_CRITICAL(&mux);
	writeReg(CONTROL_2, 0x40);
	taskEXIT_CRITICAL(&mux);
	while(1){
		taskENTER_CRITICAL(&mux);
		writeReg(CONTROL_1, 1);
		taskEXIT_CRITICAL(&mux);
        vTaskDelay(30000/portTICK_PERIOD_MS);
        taskENTER_CRITICAL(&mux);
		writeReg(CONTROL_1, 2);
		taskEXIT_CRITICAL(&mux);
        vTaskDelay(30000/portTICK_PERIOD_MS);
	}
}


void printInfoTask (void* pvParameters){
	TickType_t xLastWakeTime;
	const TickType_t xFrequency = 8000/portTICK_PERIOD_MS;

    // Initialise the xLastWakeTime variable with the current time.
    xLastWakeTime = xTaskGetTickCount();

    for( ;; )
    {
        // Wait for the next cycle.
        vTaskDelayUntil( &xLastWakeTime, xFrequency );

        printf("ERROR_STATUS_0 REGISTER: ");
        taskENTER_CRITICAL(&mux);
        readReg(ERROR_STATUS_0);
        taskEXIT_CRITICAL(&mux);
        printf("\n");
        printf("ERROR_STATUS_1 REGISTER: ");
        taskENTER_CRITICAL(&mux);
        readReg(ERROR_STATUS_1);
        taskEXIT_CRITICAL(&mux);
        printf("\n");
        printf("ERROR_STATUS_2 REGISTER: ");
        taskENTER_CRITICAL(&mux);
        readReg(ERROR_STATUS_2);
        taskEXIT_CRITICAL(&mux);
        printf("\n");
        printf("ERROR_STATUS_3 REGISTER: ");
        taskENTER_CRITICAL(&mux);
        readReg(ERROR_STATUS_3);
        taskEXIT_CRITICAL(&mux);
        printf("\n");
        printf("CONTROL_1 REGISTER: ");
        taskENTER_CRITICAL(&mux);
        readReg(CONTROL_1);
        taskEXIT_CRITICAL(&mux);
        printf("\n");
        printf("CONTROL_2 REGISTER: ");
        taskENTER_CRITICAL(&mux);
        readReg(CONTROL_2);
        taskEXIT_CRITICAL(&mux);
        printf("\n");
    }


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
static esp_err_t writeReg(uint8_t regAddress, uint8_t writeValue) {
	uint8_t regValue;
    uint8_t write_buf[2] = {CELL_SELECT_REG, 0}; /* 4 cells */
    ESP_ERROR_CHECK_WITHOUT_ABORT(i2c_master_write_read_device(I2C_MASTER_NUM, ISL94202_ADDR, &regAddress, sizeof(regAddress), &regValue, sizeof(regValue), I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS));
    printf("Initial %02x register value: %02x\n", regAddress, regValue);
    write_buf[1] = (writeValue);
    ESP_ERROR_CHECK_WITHOUT_ABORT(i2c_master_write_to_device(I2C_MASTER_NUM, ISL94202_ADDR, write_buf, sizeof(write_buf), I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS));
    ESP_ERROR_CHECK_WITHOUT_ABORT(i2c_master_write_read_device(I2C_MASTER_NUM, ISL94202_ADDR, &regAddress, sizeof(regAddress), &regValue, sizeof(regValue), I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS));
    printf("Register %02x value after write: %02x\n", regAddress, regValue);

	return (ESP_OK);
}

/**
 * @brief write to Register
 */
static esp_err_t writeRegBit(uint8_t regAddress, uint8_t writeValue, uint8_t offset) {
	uint8_t regValue =0;
    uint8_t write_buf[2] = {CELL_SELECT_REG, 0}; /* 4 cells */
    ESP_ERROR_CHECK_WITHOUT_ABORT(i2c_master_write_read_device(I2C_MASTER_NUM, ISL94202_ADDR, &regAddress, sizeof(regAddress), &regValue, sizeof(regValue), I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS));
    printf("Initial %02x register value: %02x\n", regAddress, regValue);
    write_buf[1] = (regValue | (writeValue << offset));
    ESP_ERROR_CHECK_WITHOUT_ABORT(i2c_master_write_to_device(I2C_MASTER_NUM, ISL94202_ADDR, write_buf, sizeof(write_buf), I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS));
    ESP_ERROR_CHECK_WITHOUT_ABORT(i2c_master_write_read_device(I2C_MASTER_NUM, ISL94202_ADDR, &regAddress, sizeof(regAddress), &regValue, sizeof(regValue), I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS));
    printf("Register %02x value after write: %02x\n", regAddress, regValue);

	return (ESP_OK);
}

/**
 * @brief read to Register
 */
static esp_err_t readReg(uint8_t regAddress) {
	uint8_t regValue;
    ESP_ERROR_CHECK_WITHOUT_ABORT(i2c_master_write_read_device(I2C_MASTER_NUM, ISL94202_ADDR, &regAddress, sizeof(regAddress), &regValue, sizeof(regValue), I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS));
    printf("%02x register value: %02x\n", regAddress, regValue);

	return (ESP_OK);
}

/**
 * @brief main function
 */
void app_main() {
    i2c_master_init();
    
    printf("Sending 4 cells info...\n");
    writeReg(CELL_SELECT_REG, 0b11000011);

    xTaskCreate(fetsTask, "FETs Task", 4096, NULL, tskIDLE_PRIORITY + 2, NULL );
    xTaskCreate(printInfoTask, "printInfoTask", 4096, NULL, tskIDLE_PRIORITY + 1, NULL );

    /* ESP_ERROR_CHECK(i2c_driver_delete(I2C_MASTER_NUM));
    ESP_LOGI(TAG, "I2C de-initialized successfully"); */
}
