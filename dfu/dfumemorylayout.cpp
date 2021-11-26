#include "dfumemorylayout.h"

#include <QDebug>

#include "debug.h"

// TODO: Try optimising it
DFUMemoryLayout DFUMemoryLayout::fromStringDescriptor(const QByteArray &desc)
{
    DFUMemoryLayout ret;

    const auto fields = desc.split('/');

    if(fields.size() != 3) {
        error_msg("bad descriptor syntax");
        return ret;
    }

    ret.m_name = fields.at(0).trimmed();
    ret.m_address = fields.at(1).toULong(nullptr, 16);

    const auto banks = fields.at(2).split(',');

    for(const auto &bank : banks) {
        const auto bankFields = bank.split('*');

        if(bankFields.size() != 2) {
            error_msg("bad page bank syntax");
            return ret;
        }

        const auto pageCount = bankFields.first();
        const auto pageSize = bankFields.last();
        const auto sizeMultiplier = (pageSize.at(pageSize.length() - 2) == 'K' ? 1024 : 1);

        PageBank elem;
        elem.pageCount = pageCount.toULong();
        elem.type = pageSize.back();
        elem.pageSize = pageSize.chopped(2).toULong() * sizeMultiplier;

        ret.m_pageBanks.append(elem);
    }

    return ret;
}

const QString &DFUMemoryLayout::name() const
{
    return m_name;
}

uint32_t DFUMemoryLayout::address() const
{
    return m_address;
}

const QList<DFUMemoryLayout::PageBank> &DFUMemoryLayout::pageBanks() const
{
    return m_pageBanks;
}

// TODO: Try optimising it, the function will traverse all banks regardless of the start
const QList<uint32_t> DFUMemoryLayout::pageAddresses(uint32_t start, uint32_t end) const
{
    QList<uint32_t> ret;

    if((start < m_address) || (start > end)) {
        error_msg("address error");
        return ret;
    } else if(m_pageBanks.isEmpty()) {
        error_msg("page banks empty");
        return ret;
    } else {}

    ret.append(start);

    uint32_t currentAddress = m_address;

    for(const auto &bank : m_pageBanks) {
        for(size_t i = 0; i < bank.pageCount; ++i) {
            currentAddress += (uint32_t)bank.pageSize;
            if((currentAddress > start) && (currentAddress < end)) {
                ret.append(currentAddress);
            } else if(currentAddress >= end) {
                return ret;
            }
        }
    }

    return ret;
}
