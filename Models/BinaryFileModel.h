#ifndef BINARYFILEMODEL_H
#define BINARYFILEMODEL_H

#define NOMINMAX
#include <Windows.h>
#include "../Global.h"
#include <QString>
#include <QByteArray>

class BinaryFileModel
{
private:
    QString m_Filename;
    address_t m_BaseAddress;
    quint64 m_Size;
    QByteArray m_Data;

    HANDLE hFile;
    HANDLE hFileMap;
    LPVOID _lpBuffer;
    LPVOID lpOrgBuffer;
protected:
public:
    BinaryFileModel();
    ~BinaryFileModel();
    void open(QString Filename);
    unsigned char *data();

    const address_t baseAddress() const;
    const quint64 size();
};

#endif // BINARYFILEMODEL_H
