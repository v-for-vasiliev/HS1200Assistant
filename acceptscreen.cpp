#include "acceptscreen.h"

AcceptScreen::AcceptScreen(QWidget *parent, QLineEdit *x, QLineEdit *y) :
    QWidget(parent)
{
    xEdit = x;
    yEdit = y;

    this->setWindowFlags(Qt::FramelessWindowHint | Qt::Tool | Qt::WindowStaysOnTopHint);
    this->resize(QApplication::desktop()->size());
    this->setFocusPolicy(Qt::StrongFocus);
    this->setAttribute(Qt::WA_QuitOnClose, true);

    installEventFilter(this);

    QPalette pal = palette();
    if (QApplication::desktop()->size().height() > 1100) {
        pal.setBrush(backgroundRole(), QBrush(QPixmap(":/images/acceptscreen_1920_1200.jpg")));
    } else {
        pal.setBrush(backgroundRole(), QBrush(QPixmap(":/images/acceptscreen.jpg")));
    }
    setPalette(pal);
}

void AcceptScreen::mouseReleaseEvent(QMouseEvent *event)
{
    xEdit->setText(QString::number(event->pos().x()));
    yEdit->setText(QString::number(event->pos().y()));
    close();
}
