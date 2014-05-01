#include "clsstylesettings.h"
#include <QColor>
#include <QFont>
#include <QFontMetrics>

clsStyleSettings::clsStyleSettings()
{
    m_ColorRegister = QColor(128,0,255,255);
    m_ColorValue = QColor(0,136,0,255);
    m_ColorComment = QColor(138,138,138,255);
    m_ColorBackground = QColor(239,238,254);
    m_ColorHighlight = QColor(174,206,242);


    m_DefaultFont = QFont("Fixedsys", 10);
    //m_DefaultFont = QFont("Consolas", 10);

    const QFontMetrics metrics(m_DefaultFont);
    m_FontWidth  = metrics.width('W');
    m_FontHeight = metrics.height();

}
