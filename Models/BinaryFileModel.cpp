#include "BinaryFileModel.h"
#define NOMINMAX
#include <Windows.h>
#include <QByteArray>
#include <QFile>
#include <QString>
#include <QDebug>

BinaryFileModel::BinaryFileModel(){

}

BinaryFileModel::~BinaryFileModel()
{
    UnmapViewOfFile(_lpBuffer);
    CloseHandle(hFile);
    CloseHandle(hFileMap);
}

void BinaryFileModel::open(QString Filename)
{
    m_Filename = Filename;

    QFile wFile(m_Filename);
    if(wFile.open(QIODevice::ReadOnly) == false)
         return;

    wFile.close();

    ////////////////////////////////////////////////////
    hFile = CreateFileW(m_Filename.toStdWString().c_str(),GENERIC_READ,FILE_SHARE_READ | FILE_SHARE_WRITE,NULL,OPEN_EXISTING,NULL,NULL);
    if(hFile == INVALID_HANDLE_VALUE) return;

    HANDLE hFileMap = CreateFileMapping(hFile,NULL,PAGE_READONLY,NULL,NULL,NULL);

    _lpBuffer = MapViewOfFile(hFileMap,FILE_MAP_READ,NULL,NULL,NULL);
    m_BaseAddress = 0x00401000;
}

unsigned char *BinaryFileModel::data()
{
    return (unsigned char*)(_lpBuffer);
}

const address_t BinaryFileModel::baseAddress() const
{
    return m_BaseAddress;
}

const quint64 BinaryFileModel::size()
{
    return m_Size;
}




