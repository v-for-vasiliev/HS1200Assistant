#include "usbworker.h"

USBWorker::USBWorker(QObject *parent) :
    QObject(parent)
{
    working = false;
    dev = NULL;
}

USBWorker::~USBWorker()
{
    clean_up();
}

void USBWorker::print_buff(char *buff)
{
    if (buff == NULL)
    {
        return;
    }
    const char *p = buff;
    QString out("BUFF:");
    while (*p)
    {
        out += QString(" 0x%1").arg((unsigned char) *p, 0, 16);
        p++;
    }
    qDebug() << out;
    return;
}

usb_dev_handle *USBWorker::open_dev(void)
{
    struct usb_bus *bus;
    struct usb_device *dev;

    for (bus = usb_get_busses(); bus; bus = bus->next)
    {
        for (dev = bus->devices; dev; dev = dev->next)
        {
            if (dev->descriptor.idVendor == HS1200_VID
                    && dev->descriptor.idProduct == HS1200_PID
                    && dev->descriptor.iProduct == 4)
            {
                return usb_open(dev);
            }
        }
    }
    return NULL;
}

void USBWorker::handle_key(char *buff)
{
    print_buff(buff);
    if (strlen(buff) == 2)
    {
        if ((unsigned char) buff[0] == 0x01 && (unsigned char) buff[1] == HEXKEY_VOLUME_DOWN)
        {
            emit buttonClicked(VOLUME_DOWN);
        }
        if ((unsigned char) buff[0] == 0x01 && (unsigned char) buff[1] == HEXKEY_VOLUME_UP)
        {
            emit buttonClicked(VOLUME_UP);
        }
        if ((unsigned char) buff[0] == 0x01 && (unsigned char) buff[1] == HEXKEY_ACCEPT_CALL)
        {
            emit buttonClicked(ACCEPT_CALL);
        }
    }
}

void USBWorker::clean_up()
{
    working = false;

    if (dev)
    {
        usb_release_interface(dev, 0);
        usb_close(dev);
        dev = NULL;
    }
}

void USBWorker::process()
{
    char buff[BUF_SIZE];
    int ret = 0;

    working = true;

    usb_init();
    usb_find_busses();
    usb_find_devices();

    if (!(dev = open_dev()))
    {
        clean_up();
        emit finished(USB_ERROR);
        return;
    }

    if ((ret = usb_claim_interface(dev, HS1200_INTF)) < 0)
    {
        clean_up();
        emit finished(USB_ERROR);
        return;
    }

    while (working)
    {
        memset(buff, 0, BUF_SIZE);
        if ((ret = usb_bulk_read(dev, EP_IN, buff, sizeof(buff), 1000)) > 0)
        {
            handle_key(buff);
        }
    }
    clean_up();
    emit finished(USB_OK);
    return;
}

void USBWorker::stop()
{
    working = false;
    return;
}
