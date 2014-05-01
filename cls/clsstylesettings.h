#ifndef CLSSTYLESETTINGS_H
#define CLSSTYLESETTINGS_H

#include <QColor>
#include <QFont>

class clsStyleSettings
{
public:
    clsStyleSettings();

    QColor m_ColorRegister ;
    QColor m_ColorValue ;
    QColor m_ColorComment ;
    QColor m_ColorBackground ;
    QColor m_ColorHighlight ;
    int m_FontWidth, m_FontHeight;

    QFont m_DefaultFont;
};

#endif // CLSSTYLESETTINGS_H
