#ifndef DESCRIPTORS_H
#define DESCRIPTORS_H

#include <cstdint>

struct DFUFunctionalDescriptor {
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint8_t bmAttributes;
    uint16_t wDetachTimeout;
    uint16_t wTransferSize;
    uint16_t bcdDFUVersion;
};

#endif // DESCRIPTORS_H
