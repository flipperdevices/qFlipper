#include "qflipperbackend.h"

#include "flipperzero/flipperzero.h"
#include "flipperzero/remotecontroller.h"

using namespace Flipper;

QFlipperBackend::QFlipperBackend()
{
    qRegisterMetaType<Flipper::Updates::FileInfo>("Flipper::Updates::FileInfo");
    qRegisterMetaType<Flipper::Updates::VersionInfo>("Flipper::Updates::VersionInfo");
    qRegisterMetaType<Flipper::Zero::RemoteController*>("Flipper::Zero::RemoteController*");
    qRegisterMetaType<Flipper::UpdateChannelModel*>("Flipper::UpdateChannelModel*");
}

QFlipperBackend::~QFlipperBackend()
{}
