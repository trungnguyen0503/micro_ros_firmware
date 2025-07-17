#include "project/uros/utility.h"

#include "rmw_microxrcedds_c/config.h"

#include "cmsis_os.h"
#include "usart.h"

#ifdef RMW_UXRCE_TRANSPORT_CUSTOM

// --- micro-ROS Transports ---
#define UART_DMA_BUFFER_SIZE 2048

static uint8_t Uros_dma_buffer[UART_DMA_BUFFER_SIZE] = { 0 };
static size_t Uros_dma_head = 0;
static size_t Uros_dma_tail = 0;

bool Uros_UartDmaTransport_Open(struct uxrCustomTransport *const transport) {
    UART_HandleTypeDef *uart = transport->args;
    HAL_UART_Receive_DMA(uart, Uros_dma_buffer, UART_DMA_BUFFER_SIZE);
    return true;
}

bool Uros_UartDmaTransport_Close(struct uxrCustomTransport *const transport) {
    UART_HandleTypeDef *uart = transport->args;
    HAL_UART_DMAStop(uart);
    return true;
}

size_t Uros_UartDmaTransport_Write(
    struct uxrCustomTransport *const transport,
    const uint8_t *const buf,
    const size_t len,
    uint8_t *const err
) {
    UNUSED(err);

    UART_HandleTypeDef *uart = transport->args;

    HAL_StatusTypeDef ret = HAL_ERROR;
    if (uart->gState == HAL_UART_STATE_READY) {
        ret = HAL_UART_Transmit_DMA(uart, buf, (uint16_t)len);
        while (ret == HAL_OK && uart->gState != HAL_UART_STATE_READY) {
            osDelay(1);
        }
        return ret == HAL_OK ? len : 0;
    }
    return 0;
}

size_t Uros_UartDmaTransport_Read(
    struct uxrCustomTransport *const transport,
    uint8_t *const buf,
    const size_t len,
    const int timeout,
    uint8_t *const err
) {
    UNUSED(err);

    UART_HandleTypeDef *uart = transport->args;

    int ms_used = 0;
    do {
        __disable_irq();
        Uros_dma_tail = UART_DMA_BUFFER_SIZE - __HAL_DMA_GET_COUNTER(uart->hdmarx);
        __enable_irq();
        ms_used++;
        osDelay(portTICK_RATE_MS);
    } while (Uros_dma_head == Uros_dma_tail && ms_used < timeout);

    size_t wrote = 0;
    while (Uros_dma_head != Uros_dma_tail && wrote < len) {
        buf[wrote] = Uros_dma_buffer[Uros_dma_head];
        Uros_dma_head = (Uros_dma_head + 1) % UART_DMA_BUFFER_SIZE;
        wrote++;
    }

    return wrote;
}

#endif // RMW_UXRCE_TRANSPORT_CUSTOM
