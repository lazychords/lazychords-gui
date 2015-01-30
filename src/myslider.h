#ifndef MYSLIDER_H
#define MYSLIDER_H

#include <QSlider>

class MySlider : public QSlider
{
    Q_OBJECT
public:
    explicit MySlider(QWidget *parent = 0);

signals:

public slots:

protected:
    void mousePressEvent ( QMouseEvent * event );
};

#endif // MYSLIDER_H
