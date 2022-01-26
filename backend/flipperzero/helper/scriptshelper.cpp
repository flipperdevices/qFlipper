#include "scriptshelper.h"

#include <QFile>

#include "tararchive.h"
#include "tarziparchive.h"

using namespace Flipper;
using namespace Zero;

ScriptsHelper::ScriptsHelper(QFile *scriptsArchive, QObject *parent):
    AbstractOperationHelper(parent),
    m_compressedFile(scriptsArchive)
{}

const QByteArray ScriptsHelper::optionBytesData() const
{
    return m_archive->archiveIndex()->fileData(QStringLiteral("scripts/ob.data"));
}

void ScriptsHelper::nextStateLogic()
{
    if(state() == AbstractOperationHelper::Ready) {
        setState(ScriptsHelper::UncompressingArchive);
        uncompressArchive();

    } else if(state() == ScriptsHelper::UncompressingArchive) {
        finish();
    }
}

void ScriptsHelper::uncompressArchive()
{
    m_archive = new TarZipArchive(m_compressedFile, this);

    if(m_archive->isError()) {
        finishWithError(m_archive->error(), QStringLiteral("Failed to uncompress archive file: %1").arg(m_archive->errorString()));
        return;
    }

    connect(m_archive, &TarZipArchive::ready, this, [=]() {
        if(m_archive->isError()) {
            finishWithError(m_archive->error(), QStringLiteral("Failed to uncompress archive file: %1").arg(m_archive->errorString()));
        } else {
            advanceState();
        }
    });
}
