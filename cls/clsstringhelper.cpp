#include "clsstringhelper.h"
#include <QString>
#include <stdio.h>
#include <cctype>
#include <QRegExp>

clsStringHelper::clsStringHelper()
{
}



QString clsStringHelper::intToHexString(qint32 value)
{
    char buffer[9];
    sprintf (buffer, "%08x", value);
    return QString(buffer).toUpper();
}


bool clsStringHelper::isPrintable(quint32 value)
{

        // if it's standard ascii use isprint/isspace, otherwise go with our observations
        if(value < 0x80) {
            return std::isprint(value) || std::isspace(value);
        } else {
            return (value & 0xff) >= 0xa0;
        }

}


bool clsStringHelper::isHex(QString Str){
    QRegExp hexMatcher("^[0-9a-fA-F]*h?$", Qt::CaseInsensitive);
    return hexMatcher.exactMatch(Str);


}
