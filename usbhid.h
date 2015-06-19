#ifndef CUSBHID_H
#define CUSBHID_H

#include <QMessageBox>
#include "hidapi.h"

class CUsbHid
{

private:

    hid_device *hidDevice;

public:

    bool OpenUSBDevice(unsigned int vid, unsigned int pid);
    bool WriteUSBDevice(char *buffer, unsigned int bufflen);
    unsigned short ReadUSBDevice(char* buffer, unsigned int bufflen);
    bool GetPortOpenStatus(void);
    QString GetDeviceManufacturerStr(void);
    QString GetDeviceProductStr(void);
    QString GetSerialStr(void);
    void ClosePort(void);

    // Constructor
    CUsbHid();

    // Destructor
    ~CUsbHid();
};

#endif // CUSBHID_H
