#include <driver/uart.h>
#include "hw.h"
#include "pin.h"
#include "com.h"

//define list
#define UART_PORT   UART_NUM_2
#define BAUD_RATE   115200

//Initialize communication
int32_t com_init(void) {
    // Set UART parameters (both controllers must use the same settings)
    const uart_config_t uart_config = {
        .baud_rate = BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };
    // Error Check
    if (uart_param_config(UART_PORT, &uart_config) != ESP_OK) {
        return -1;
    }

    // TX: HW_EX8 (IO17), RX: HW_EX7 (IO16), RTS/CTS unused
    if (uart_set_pin(UART_PORT, HW_EX8, HW_EX7,
                     UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE) != ESP_OK) {
        return -1;
    }

    // Install driver: RX buffer = 2x HW FIFO, no TX buffer, no queue
    if (uart_driver_install(UART_PORT, UART_HW_FIFO_LEN(UART_PORT) * 2,
                            0, 0, NULL, 0) != ESP_OK) {
        return -1;
    }

    // Idle UART line is high; pullup reduces glitches on cable connect/reboot
    pin_pullup(HW_EX7, true);

    return 0;
}

// Deinitialize the communication
int32_t com_deinit(void) {
    //check installed
    if (uart_is_driver_installed(UART_PORT)) {
        // Delete
        if (uart_driver_delete(UART_PORT) != ESP_OK) {
            return -1;
        }
    }
    return 0;
}

// write out function
int32_t com_write(const void *buf, uint32_t size) {
    // Non-blocking: copies as many bytes as fit into the TX FIFO and returns
    // the count written (or -1 on error).
    return uart_tx_chars(UART_PORT, (const char *)buf, size);
}

// read in function
int32_t com_read(void *buf, uint32_t size) {
    // Zero ticks to wait, so this returns immediately with 0 if no data.
    return uart_read_bytes(UART_PORT, buf, size, 0);
}