#ifndef DFUMEMORYLAYOUT_H
#define DFUMEMORYLAYOUT_H

#include <QList>
#include <QString>
#include <QByteArray>

class DFUMemoryLayout
{
public:
    struct PageBank {
        size_t pageSize;
        size_t pageCount;
        char type;
    };

    static DFUMemoryLayout fromStringDescriptor(const QByteArray &desc);

    const QString &name() const;
    uint32_t address() const;
    const QList<PageBank> &pageBanks() const;

    const QList<uint32_t> pageAddresses(uint32_t start, uint32_t end) const;


private:
    QString m_name;
    uint32_t m_address;
    QList<PageBank> m_pageBanks;
};

#endif // DFUMEMORYLAYOUT_H
