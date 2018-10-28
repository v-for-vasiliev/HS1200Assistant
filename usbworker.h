#ifndef USBWORKER_H
#define USBWORKER_H

#include <QObject>
#include <QDebug>

#include <lusb0_usb.h>
#include <stdio.h>

#define     HS1200_VID              0x041E
#define     HS1200_PID              0x0401

#define     EP_IN                   0x83
#define     EP_OUT                  0x03

#define     BUF_SIZE                8

#define     HS1200_CONFIG           1
#define     HS1200_INTF             3

#define     HEXKEY_VOLUME_UP        0xE9
#define     HEXKEY_VOLUME_DOWN      0xEA
#define     HEXKEY_ACCEPT_CALL      0xB1

#define     USB_OK                  0
#define     USB_ERROR               1

enum
{
    VOLUME_UP   = 0,
    VOLUME_DOWN = 1,
    ACCEPT_CALL = 2
};

class USBWorker : public QObject
{
    Q_OBJECT
public:
    explicit USBWorker(QObject *parent = 0);
    ~USBWorker();

private:
    bool working;
    usb_dev_handle *dev;

    usb_dev_handle *open_dev(void);
    void clean_up();
    void handle_key(char* buff);
    void print_buff(char* buff);
    
signals:
    void buttonClicked(int button);
    void finished(int code);
    
public slots:
    void process(); // start reading usb
    void stop(); // stop reading usb
    
};

#endif // USBWORKER_H
