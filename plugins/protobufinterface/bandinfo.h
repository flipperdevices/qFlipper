#pragma once

#include <QList>

struct BandInfo {
    uint32_t start;
    uint32_t end;
    int32_t powerLimit;
    uint32_t dutyCycle;
};

using BandInfoList = QList<BandInfo>;
