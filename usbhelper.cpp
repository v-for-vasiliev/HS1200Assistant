#include <lusb0_usb.h>
#include <stdio.h>
#include <locale.h>
#include <conio.h>
#include <Windows.h>

#define HS1200_VID 0x041E
#define HS1200_PID 0x0401

#define EP_IN 0x83
#define EP_OUT 0x03

#define BUF_SIZE 8

#define HS1200_CONFIG 1
#define HS1200_INTF 3

usb_dev_handle *open_dev(void)
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
		
void left_click()
{  
	INPUT Input = { 0 };

	Input.type = INPUT_MOUSE;
	Input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
	SendInput(1, &Input, sizeof(INPUT));

	ZeroMemory(&Input, sizeof(INPUT));
	Input.type = INPUT_MOUSE;
	Input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
	SendInput(1, &Input, sizeof(INPUT));
}

void handle_key(char* buff)
{
	if (strlen(buff) == 2)
	{
		if ((unsigned char) buff[0] == 0x01 && (unsigned char) buff[1] == 0xEA)
		{
			printf("key volume down\n");
		}
		if ((unsigned char) buff[0] == 0x01 && (unsigned char) buff[1] == 0xE9)
		{
			printf("key volume up\n");
		}
		if ((unsigned char) buff[0] == 0x01 && (unsigned char) buff[1] == 0xB1)
		{
			printf("key accept call\n");
			left_click();
		}
	}
}

void print_buff(char* buff)
{
	const char *p = buff;
	while (*p)
	{
		printf(" 0x%02X", (unsigned char) *p);
		p++;
	}
	printf("\n");
    handle_key(buff);
}

int poll_usb() {
	char buff[BUF_SIZE];
	int ret = 0;

	usb_dev_handle *dev = NULL;

    usb_init();
    usb_find_busses();
    usb_find_devices();

	enum_devices();

	if (!(dev = open_dev()))
    {
        printf("error opening device: \n%s\n", usb_strerror());
        goto END;
    }
    else
    {
        printf("success: device %04X:%04X opened\n", HS1200_VID, HS1200_PID);
    }

	if ((ret = usb_claim_interface(dev, HS1200_INTF)) < 0)
    {
        printf("error claiming interface #%d: %d, %s\n", HS1200_INTF, ret, usb_strerror());
        goto END;
    }
    else
    {
        printf("success: claim_interface #%d\n", HS1200_INTF);
    }

	/*
	BYTE *data = new BYTE[256];
	memset(data, 1, 256);
	ret = usb_bulk_write(dev, EP_OUT, (char*) data, 256, 5000);
    if (ret < 0)
    {
        printf("error writing: %d, %s\n", ret, usb_strerror());
    }
    else
    {
        printf("success: bulk write %d bytes\n", ret);
    }
	*/

	/*
	printf("reading data...");
	ret = usb_bulk_read(dev, EP_IN, buff, sizeof(buff), 5000);
    if (ret < 0)
    {
        printf("error reading: %d, %s\n", ret, usb_strerror());
		goto END;
    }
    else
    {
        printf("success: bulk read %d bytes\n", ret);
    }
	*/

	printf("reading data...\n");
	while (!kbhit())
	{
		memset(buff, 0, 8);
		if ((ret = usb_bulk_read(dev, EP_IN, buff, sizeof(buff), 1000)) > 0)
		{
			printf("success: bulk read %d bytes:", ret); 
			print_buff(buff);
		}
	}

END:

    usb_release_interface(dev, 0);

	if (dev)
    {
        usb_close(dev);
    }
    printf("Done.\n");

	getchar();

    return 0;
}
