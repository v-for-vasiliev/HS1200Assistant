#ifndef ACCEPTSCREEN_H
#define ACCEPTSCREEN_H

#include <QWidget>
#include <QApplication>
#include <QDesktopWidget>
#include <QLineEdit>
#include <QMouseEvent>

class AcceptScreen : public QWidget
{
    Q_OBJECT

protected:
    void mouseReleaseEvent(QMouseEvent *event);

public:
    explicit AcceptScreen(QWidget *parent = 0, QLineEdit *x = NULL, QLineEdit *y = NULL);

private:
    QLineEdit *xEdit;
    QLineEdit *yEdit;
    
signals:
    
public slots:
    
};

#endif // ACCEPTSCREEN_H
