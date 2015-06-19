#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "usbhid.h"
#include "bootloader.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

protected:
    CUsbHid OUsbHid;
    CBootloader OBootloader;

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:

    void on_ButtonConnect_clicked();

    void on_ButtonBootInfo_clicked();

    void on_ButtonSnRead_clicked();

    void on_ButtonSnProgram_clicked();

private:
    Ui::MainWindow *ui;

    bool ConnectionEstablished;

    void ExtraInitialization(void);
    void PrintKonsole(QString string);
    void ClearKonsole(void);
};

#endif // MAINWINDOW_H
