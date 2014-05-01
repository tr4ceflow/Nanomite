#include "ArrowHelper.h"

#include <QPainter>
#include <QColor>
#include <QPainterPath>
#include <QPointF>

ArrowHelper::ArrowHelper()
{
}

void ArrowHelper::drawStraightArrow(QPainter *painter, int x1, int y1, int x2, int y2)
{


    painter->drawLine(x1,y1,x2,y2);



    /*
    // this does not work
    QPainterPath path;

    path.moveTo( QPointF(x2, x2));
    path.lineTo (QPointF(x2-ArrowSizeX,y2-ArrowSizeY));
    path.lineTo (QPointF(x2-ArrowSizeX,y2+ArrowSizeY));
    path.closeSubpath();

    painter->setPen (Qt :: NoPen);
    painter->fillPath (path, QBrush (color));*/



    painter->drawLine(x2,y2,x2-ArrowSizeX,y2-ArrowSizeY);
    painter->drawLine(x2,y2,x2-ArrowSizeX,y2+ArrowSizeY);

}

void ArrowHelper::drawToRightArrowTip(QPainter* painter, int x2, int y2){
    painter->save();
    QPen tmp = painter->pen();
    tmp.setStyle(Qt::SolidLine);
    tmp.setWidth(2);
    painter->setPen(tmp);
    painter->drawLine(x2,y2,x2-ArrowSizeX,y2-ArrowSizeY);
    painter->drawLine(x2,y2,x2-ArrowSizeX,y2+ArrowSizeY);
    painter->restore();
}

void ArrowHelper::drawToBottomArrowTip(QPainter* painter, int x2, int y2){
    painter->save();
    QPen tmp = painter->pen();
    tmp.setStyle(Qt::SolidLine);
    tmp.setWidth(2);
    painter->setPen(tmp);
    painter->drawLine(x2,y2,x2-ArrowSizeX,y2-ArrowSizeY);
    painter->drawLine(x2,y2,x2+ArrowSizeX,y2-ArrowSizeY);
    painter->restore();
}

void ArrowHelper::drawJumpArrowInViewport(QPainter* painter, int StepWidth,  int x_start, int y_start,int y_end, int Offset)
{
    /*
     * draws
     *
     * +----     start
     * |
     * |         middle
     * |
     * +--->     end
     */




    // start
    painter->drawLine(x_start,y_start,x_start-StepWidth*Offset,y_start);
    // middle
    painter->drawLine(x_start-StepWidth*Offset,y_start,x_start-StepWidth*Offset,y_end);
    // end
    painter->drawLine(x_start,y_end,x_start-StepWidth*Offset,y_end);
    drawToRightArrowTip(painter,x_start,y_end);



}

void ArrowHelper::drawJumpArrowLeaveTopViewport(QPainter *painter,  int StepWidth, int x_start, int y_start, int Offset)
{

    // start
    painter->drawLine(x_start,y_start,x_start-StepWidth*Offset,y_start);
    // middle
    painter->drawLine(x_start-StepWidth*Offset,y_start,x_start-StepWidth*Offset,0);
}

void ArrowHelper::drawJumpArrowLeaveBottomViewport(QPainter *painter, int StepWidth, int x_start, int y_start, int Offset, int viewportheight)
{

    // start
    painter->drawLine(x_start,y_start,x_start-StepWidth*Offset,y_start);
    // middle
    painter->drawLine(x_start-StepWidth*Offset,y_start,x_start-StepWidth*Offset,viewportheight);

    drawToBottomArrowTip(painter,x_start-StepWidth*Offset,viewportheight);
}

