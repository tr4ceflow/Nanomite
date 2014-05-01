#ifndef CLSARROW_H
#define CLSARROW_H

#include <QPainter>
#include <QColor>
class ArrowHelper
{
    static const int ArrowSizeX = 4;
    static const int ArrowSizeY = 4;
public:
    ArrowHelper();
    // draws "--->"
    static void drawStraightArrow(QPainter* painter, int x1, int y1, int x2, int y2);
    // draws ">"
    static void drawToRightArrowTip(QPainter *painter, int x2, int y2);
    // draws jump arrows that starts and ends within viewport
    static void drawJumpArrowInViewport(QPainter* painter,int StepWidth,  int x_start, int y_start,int y_end, int Offset);
    // draws jump arrows that starts in viewport and leaves viewport
    static void drawJumpArrowLeaveTopViewport(QPainter* painter,int StepWidth,  int x_start, int y_start,int Offset);

    static void drawJumpArrowLeaveBottomViewport(QPainter* painter,  int StepWidth,  int x_start, int y_start,int Offset,int viewportheight);
    static void drawToBottomArrowTip(QPainter *painter, int x2, int y2);
};

#endif // CLSARROW_H
