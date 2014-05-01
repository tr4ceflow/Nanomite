#ifndef CLSSTRINGHELPER_H
#define CLSSTRINGHELPER_H

#include <QString>

class clsStringHelper
{
public:
    clsStringHelper();
    static QString intToHexString(qint32 value);
    static bool isPrintable(quint32 value);
    static bool isHex(QString Str);
};

#endif // CLSSTRINGHELPER_H
