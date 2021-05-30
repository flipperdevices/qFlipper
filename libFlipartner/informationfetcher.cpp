#include "informationfetcher.h"

#include <QSerialPort>
#include <QDebug>

#include "serialhelper.h"

using namespace Flipper;

AbstractInformationFetcher::~AbstractInformationFetcher()
{}

VCPInformationFetcher::VCPInformationFetcher(const USBDeviceParams &parameters):
    m_parameters(parameters)
{
}

void VCPInformationFetcher::run()
{
}
