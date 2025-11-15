#ifndef UROS_UTILITY_H
#define UROS_UTILITY_H

#include "uxr/client/transport.h"

bool Uros_UartDmaTransport_Open(uxrCustomTransport *transport);
bool Uros_UartDmaTransport_Close(uxrCustomTransport *transport);
size_t Uros_UartDmaTransport_Write(
    struct uxrCustomTransport *transport,
    const uint8_t *buf,
    size_t len,
    uint8_t *err
);
size_t Uros_UartDmaTransport_Read(
    struct uxrCustomTransport *transport,
    uint8_t *buf,
    size_t len,
    int timeout,
    uint8_t *err
);

bool Uros_UsbCdcTransport_Open(struct uxrCustomTransport *transport);
bool Uros_UsbCdcTransport_Close(struct uxrCustomTransport *transport);
size_t Uros_UsbCdcTransport_Write(
    struct uxrCustomTransport *transport,
    const uint8_t *buf,
    size_t len,
    uint8_t *err
);
size_t Uros_UsbCdcTransport_Read(
    struct uxrCustomTransport *transport,
    uint8_t *buf,
    size_t len,
    int timeout,
    uint8_t *err
);

size_t Uros_GetFreeHeapSize(void);
size_t Uros_GetMinFreeHeapSize(void);
size_t Uros_GetBlockSize(void *pv);

void *Uros_Allocate(size_t size, void *state);
void Uros_Deallocate(void *pointer, void *state);
void *Uros_Reallocate(void *pointer, size_t size, void *state);
void *Uros_ZeroAllocate(size_t number_of_elements, size_t size_of_element, void *state);

#endif
