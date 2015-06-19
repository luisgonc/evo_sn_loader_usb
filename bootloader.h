#ifndef BOOTLOADER_H
#define BOOTLOADER_H


// bootloader commands
typedef enum
{
    READ_BOOT_INFO = 1,
    ERASE_FLASH,
    PROGRAM_FLASH,
    READ_CRC,
    JMP_TO_APP,
    ERASE_SPI_FLASH,
    PROGRAM_SPI_FLASH,
    READ_SPI_CRC,
    SERIAL_CHANGE_BAUDE_RATE,
    SERIAL_TEST_BAUDE_RATE,
    READ_HARDWARE_CODE,
    PROGRAM_SERIAL_NUMBER,
    READ_SERIAL_NUMBER
}commands_t;


// Bootloader class
class CBootloader
{
public:
    CBootloader();
    ~CBootloader();

    bool DecodeRxFrame(char *buff, unsigned short bufflen, char *return_buff, unsigned short *return_len);
    void EncodeTxFrame(char *buff, unsigned short bufflen, char *return_buff, unsigned short *return_len);
};

#endif // BOOTLOADER_H
