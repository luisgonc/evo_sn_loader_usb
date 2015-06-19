
#include <QDebug>
#include <iostream>
#include "usbhid.h"

// Data + reportID
#define USB_BUFFER_SIZE (64 + 1)


// Constructor
CUsbHid::CUsbHid()
{
    hidDevice = NULL;
}

// Destructor
CUsbHid::~CUsbHid()
{
    ClosePort();
}

/****************************************************************************
 *  Open USB device
 *
 * \param  vid: Vendor Id
 * \param  pid: Product Id
 * \param
 * \return
 *****************************************************************************/
bool CUsbHid::OpenUSBDevice(unsigned int vid, unsigned int pid)
{
    hidDevice = hid_open(vid, pid, NULL);

    if(!hidDevice)
    {
        QString error;
        error.sprintf("USB Device with VID= %04X and PID= %04X is not found", vid, pid);
        QMessageBox::warning(NULL, "Error opening USB Device", error, QMessageBox::Ok);
    }
    else
    {
      hid_set_nonblocking(hidDevice, true);
    }

    return (hidDevice != NULL);
}

/****************************************************************************
 *  Write USB device
 *
 * \param  *buffer: Pointer to the buffer
 * \param  bufflen: Buffer length
 * \param
 * \return
 *****************************************************************************/
bool CUsbHid::WriteUSBDevice(char *buffer, unsigned int bufflen)
{
    int BytesWritten;
    unsigned char _buffer[USB_BUFFER_SIZE];

    if(hidDevice == NULL || buffer == NULL || bufflen == 0)
    {
        return false;
    }

    memset(_buffer, 0xFF,  sizeof(_buffer));
    _buffer[0] = 0;  //Report ID = 0
    memcpy(&_buffer[1], buffer, bufflen);

    BytesWritten = hid_write(hidDevice, _buffer, USB_BUFFER_SIZE);


#ifdef QT_DEBUG
    if (BytesWritten == -1)
    {
      std::cout << "WriteUSBDevice BytesWritten == -1" << std::endl;
    }
    else if (BytesWritten > 0)
    {
      std::cout << "WriteUSBDevice BytesWritten == " << BytesWritten << "  bufflen: " << bufflen << std::endl;
    }
#endif

    return (BytesWritten != -1);
}

/****************************************************************************
 *  Read USB device
 *
 * \param  *buffer: Pointer to the buffer
 * \param  bufflen: Maximum size of the read buffer. (Not used, since USB buffer is always 64bytes)
 * \param
 * \return
 *****************************************************************************/
unsigned short CUsbHid::ReadUSBDevice(char* buffer, unsigned int bufflen)
{
    if(hidDevice == NULL)
    {
        return 0;
    }

    int bytesRead = hid_read(hidDevice, (unsigned char *)buffer, bufflen);

    if (bytesRead == -1)
    {
        bytesRead = 0;
#ifdef QT_DEBUG
        std::cout << "ReadUSBDevice bytesRead == -1" << std::endl;
#endif
    }
#ifdef QT_DEBUG
    else if (bytesRead > 0)
    {
        std::cout << "ReadUSBDevice bytesRead == " << bytesRead << std::endl;
    }
#endif
    return bytesRead;

}

/****************************************************************************
 *  Check if USB port is already opened.
 *
 * \param
 * \param
 * \param
 * \return
 *****************************************************************************/
bool CUsbHid::GetPortOpenStatus(void)
{
    return (hidDevice != NULL);
}

/****************************************************************************
 *  GetDeviceManufacturerString
 *
 * \param
 * \param
 * \param
 * \return
 *****************************************************************************/
QString CUsbHid::GetDeviceManufacturerStr(void)
{
    QString str = "None";
    wchar_t manufaturer_str[50];

    if(hidDevice != NULL)
    {
        str = "";

        hid_get_manufacturer_string(hidDevice, manufaturer_str, 50);

        str = QString::fromWCharArray(manufaturer_str);
    }

    return str;

}

/****************************************************************************
 *  GetDeviceProductString
 *
 * \param
 * \param
 * \param
 * \return
 *****************************************************************************/
QString CUsbHid::GetDeviceProductStr(void)
{
    QString str = "None";
    wchar_t product_str[50];

    if(hidDevice != NULL)
    {
        str = "";

        hid_get_product_string(hidDevice, product_str, 50);

        str = QString::fromWCharArray(product_str);
    }

    return str;
}

/****************************************************************************
 *  GetSerialStr
 *
 * \param
 * \param
 * \param
 * \return
 *****************************************************************************/
QString CUsbHid::GetSerialStr(void)
{
    QString str = "None";
    wchar_t serial_str[50];

    if(hidDevice != NULL)
    {
        str = "";

        hid_get_serial_number_string(hidDevice, serial_str, 50);

        str = QString::fromWCharArray(serial_str);
    }

    return str;
}

/****************************************************************************
 *  Close port
 *
 * \param
 * \param
 * \param
 * \return
 *****************************************************************************/
void CUsbHid::ClosePort(void)
{
    if(hidDevice != NULL)
    {
        hid_close(hidDevice);
        // Initialize device found to false.
        hidDevice = NULL;
    }
}
