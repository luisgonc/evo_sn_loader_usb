
#include <QThread>
#include <QDebug>
#include <iostream>
#include "mainwindow.h"
#include "ui_mainwindow.h"

// Number of different PIDs
//
#define NUMBER_OF_PIDS  5

// List of all the PID numbers from GFE products.
//
static const unsigned short pid_list[NUMBER_OF_PIDS] =
{
    0x0100, 0x0200, 0x0300, 0x0301, 0x0302
};

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ExtraInitialization();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::ExtraInitialization(void)
{
    ConnectionEstablished = false;
}

void MainWindow::on_ButtonConnect_clicked()
{
    unsigned int i;

    if(ConnectionEstablished)
    {
        if(OUsbHid.GetPortOpenStatus())
        {
            OUsbHid.ClosePort();
        }

        if(OUsbHid.GetPortOpenStatus())
        {
            PrintKonsole("Could not Disconnect device");
        }
        else
        {
            ConnectionEstablished = false;
            ui->ButtonConnect->setText("Connect");
            PrintKonsole("Device successfully disconnected");
        }

    }
    else
    {
        if(OUsbHid.GetPortOpenStatus())
        {
            OUsbHid.ClosePort();
        }

        // Run the entire list of PIDs
        //
        for(i = 0; i < NUMBER_OF_PIDS; i++)
        {
            OUsbHid.OpenUSBDevice(0x2C1E, pid_list[i]);

            if(OUsbHid.GetPortOpenStatus())
            {
                break; // Get out in the first successfull connection
            }
        }

        if(OUsbHid.GetPortOpenStatus())
        {
            ConnectionEstablished = true;
            ui->ButtonConnect->setText("Disconnect");

            ClearKonsole();
            PrintKonsole("Device Connected\n");

            QString aux_string;

            // Print Manufacturer String
            aux_string = "Manufacturer: ";
            PrintKonsole(aux_string.append(OUsbHid.GetDeviceManufacturerStr()));

            // Print Product String
            aux_string = "Product: ";
            PrintKonsole(aux_string.append(OUsbHid.GetDeviceProductStr()));

            //Serial Number String
            aux_string = "Serial: ";
            PrintKonsole(aux_string.append(OUsbHid.GetSerialStr()));

            PrintKonsole(" ");

        }
        else
        {
            PrintKonsole("Device NOT Connected");
        }
    }
}


void MainWindow::on_ButtonBootInfo_clicked()
{
    if(ConnectionEstablished)
    {
        char buffer[100];
        unsigned short buff_len;
        QString string;

        char command = READ_BOOT_INFO;

        OBootloader.EncodeTxFrame(&command, 1, &buffer[0], &buff_len);

        PrintKonsole("\nBoot info Request...");

        OUsbHid.WriteUSBDevice(buffer, buff_len);

        QThread::sleep(1);

        unsigned short resp_len;
        char resp_buff[100];
        resp_len = OUsbHid.ReadUSBDevice(resp_buff, 30);

        if(resp_len)
        {

            if(OBootloader.DecodeRxFrame(resp_buff, resp_len, buffer, &buff_len))
            {
                string.sprintf("Bootloader Firmware Version: %d.%d", buffer[1], buffer[2]);
                PrintKonsole(string);
            }
            else
            {
                PrintKonsole("Bad response from device");
            }
        }
        else
        {
            PrintKonsole("No response from device");
        }
    }
}


void MainWindow::on_ButtonSnRead_clicked()
{
    if(ConnectionEstablished)
    {
        char buffer[100];
        unsigned short buff_len;
        QString string;

        char command = READ_SERIAL_NUMBER;

        OBootloader.EncodeTxFrame(&command, 1, &buffer[0], &buff_len);

        PrintKonsole("\nSerial Number Request...");

        OUsbHid.WriteUSBDevice(buffer, buff_len);

        QThread::sleep(1);

        unsigned short resp_len;
        char resp_buff[100];
        resp_len = OUsbHid.ReadUSBDevice(resp_buff, 30);

        if(resp_len)
        {

            if(OBootloader.DecodeRxFrame(resp_buff, resp_len, buffer, &buff_len))
            {
                string.sprintf("Serial Number: %c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c", buffer[1], buffer[2], buffer[3], buffer[4], buffer[5], buffer[6], buffer[7], buffer[8], buffer[9], buffer[10], buffer[11], buffer[12], buffer[13], buffer[14], buffer[15], buffer[16]);
                PrintKonsole(string);
            }
            else
            {
                PrintKonsole("Bad response from device");
            }
        }
        else
        {
            PrintKonsole("No response from device");
        }
    }
}


void MainWindow::on_ButtonSnProgram_clicked()
{
    if(ConnectionEstablished)
    {
        char buffer[100];
        unsigned short buff_len;
        int string_len;
        QString string;


        string = ui->lineEditSerialNumber->text();

        string_len = string.count();

        if(string_len != 16)
        {
            QString error;
            error.sprintf("O Nº de série tem que ter 16 caracteres!!!");
            QMessageBox::warning(NULL, "Erro a gravar nº de série", error, QMessageBox::Ok);

        }
        else
        {
            QByteArray sn_byte_aray;

            sn_byte_aray = string.toLocal8Bit();

            char command[100];

            command[0] = PROGRAM_SERIAL_NUMBER;

            for(int i = 0; i < 16; i++)
            {
                command[1+i] = sn_byte_aray.at(i);
            }

            OBootloader.EncodeTxFrame(&command[0], 17, &buffer[0], &buff_len);

#ifdef QT_DEBUG
                std::cout << "len:" << buff_len << std::endl;
#endif

            PrintKonsole("\nProgramming Serial Number...");

            OUsbHid.WriteUSBDevice(buffer, buff_len);

            QThread::sleep(1);

            unsigned short resp_len;
            char resp_buff[100];
            resp_len = OUsbHid.ReadUSBDevice(resp_buff, 30);

            if(resp_len)
            {

                if(OBootloader.DecodeRxFrame(resp_buff, resp_len, buffer, &buff_len))
                {
                    if(buffer[1] == 0)
                    {
                        string.sprintf("SN Prog. ERROR! - Panel SN already Programmed! -");
                    }
                    else if(buffer[1] == 1)
                    {
                        string.sprintf("SN Prog. ERROR! - Invalid characters! -");
                    }
                    else if(buffer[1] == 2)
                    {
                        string.sprintf("SN Prog. ERROR! - Bad Lenght!-");
                    }
                    else if(buffer[1] == 15)
                    {
                        string.sprintf("Serial Number programmed successfully!");
                    }
                    else
                    {
                        string.sprintf("SN Prog. ERROR! - Unknown error! -");
                    }

                    PrintKonsole(string);

                }
                else
                {
                    PrintKonsole("Bad response from device");
                }
            }
            else
            {
                PrintKonsole("No response from device");
            }

        }
    }
}


/****************************************************************************
 * Print Konsole
 *
 *
 *****************************************************************************/
void MainWindow::PrintKonsole(QString string)
{
    //    UpdateData(false);
    // Set the console window vertical scroll to the end.
    ui->plainTextEdit->setPlainText(ui->plainTextEdit->toPlainText().append(string).append("\n"));
}

/****************************************************************************
 * Clears konsole
 *
 *
 *****************************************************************************/
void MainWindow::ClearKonsole(void)
{
    //    UpdateData(false);
    // Set the console window vertical scroll to the end.
    ui->plainTextEdit->setPlainText("");
}




