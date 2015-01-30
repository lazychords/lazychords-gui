#include <QMouseEvent>
#include "myslider.h"

MySlider::MySlider(QWidget *p) :
    QSlider(p)
{
}

void MySlider::mousePressEvent ( QMouseEvent * e )
  {
    if (e->button() == Qt::LeftButton)
    {
        if (orientation() == Qt::Vertical)
            setValue(minimum() + ((maximum()-minimum()) * (height()-e->y())) / height() ) ;
        else
            setValue(minimum() + ((maximum()-minimum()) * e->x()) / width() ) ;

        e->accept();
    }
    QSlider::mousePressEvent(e);
  }
