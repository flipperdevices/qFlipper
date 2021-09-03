#pragma once

class QIODevice;

class TarFile
{
public:
    TarFile(QIODevice *file);

private:
    void buildIndex();

    QIODevice *m_file;
};

