#include <QDebug>
#include <iostream>
#include "bootloader.h"


// Frame control characters
//
#define SOH 01
#define EOT 04
#define DLE 16


// Static table used for the table_driven implementation.
//
static const unsigned short crc_table[16] =
{
    0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7,
    0x8108, 0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xe1ce, 0xf1ef
};

/****************************************************************************
 * Update the crc value with new data.
 *
 * \param crc      The current crc value.
 * \param data     Pointer to a buffer of \a data_len bytes.
 * \param len		Number of bytes in the \a data buffer.
 * \return         The updated crc value.
 *****************************************************************************/
unsigned short CalculateCrc(char *data, unsigned int len)
{
    unsigned int i;
    unsigned short crc = 0;

    while(len--)
    {
        i = (crc >> 12) ^ (*data >> 4);
        crc = crc_table[i & 0x0F] ^ (crc << 4);
        i = (crc >> 12) ^ (*data >> 0);
        crc = crc_table[i & 0x0F] ^ (crc << 4);
        data++;
    }

    return (crc & 0xFFFF);
}



CBootloader::CBootloader()
{

}

CBootloader::~CBootloader()
{

}

/****************************************************************************
 * Decodes the frame data from the given encodded buffer
 *
 *
 *****************************************************************************/
bool CBootloader::DecodeRxFrame(char *buff, unsigned short bufflen, char *return_buff, unsigned short *return_len)
{
    bool escape = false;
    bool rx_frame_valid = false;
    unsigned short crc;
    unsigned short rx_data_len = 0;
    char rx_data[1000];


    while( (bufflen > 0) && (rx_frame_valid == false) )
    {
        bufflen--;

        switch(*buff)
        {
            // Start of Header
            //
            case SOH:
            {
                if(escape)
                {
                    // Received byte is not SOH, but data.
                    rx_data[rx_data_len++] = *buff;

                    // Reset the escape data
                    escape = false;
                }
                else
                {
                    // Received byte is indeed a SOH which indicates start of new frame.
                    rx_data_len = 0;
                }

                break;
            }

            // End of Transmission
            //
            case EOT:
            {
                if(escape)
                {
                    // Received byte is not EOT, but data.
                    rx_data[rx_data_len++] = *buff;

                    // Reset the escape data
                    escape = false;
                }
                else
                {
                    // Received byte is indeed a EOT which indicates end of frame.
                    // Calculate CRC to check the validity of the frame.
                    if(rx_data_len > 1)
                    {
                        crc = (rx_data[rx_data_len-2]) & 0x00ff;
                        crc = crc | ((rx_data[rx_data_len-1] << 8) & 0xFF00);
                        if((CalculateCrc(rx_data, (rx_data_len-2)) == crc) && (rx_data_len > 2))
                        {
                            // CRC matches and frame received is valid.
                            rx_frame_valid = true;
                        }
                        else
                        {
                            return false;
                        }
                    }
                }

                break;
            }

            // Data Link Escape
            //
            case DLE:
            {
                if(escape)
                {
                    // Receive byte is not escape but data
                    rx_data[rx_data_len++] = *buff;

                    // Reset the escape data
                    escape = false;
                }
                else
                {
                    // Received byte is an escape character. Set Escape flag to escape next byte.
                    escape = true;
                }

                break;
            }

            // Data
            //
            default:
            {
                rx_data[rx_data_len++] = *buff;
                // Reset the escape data
                escape = false;

                break;
            }

        }

        // Increment the pointer.
        buff++;
    }

    if(rx_frame_valid)
    {
        // Fill the return buffer with the received data
        //
        for(int i = 0; i < rx_data_len ; i++)
        {
            *return_buff = rx_data[i];
            return_buff++;
        }

        *return_len = rx_data_len;

        return true;
    }
    else
    {
        return false;
    }
}

/****************************************************************************
 * Returns and encoded buffer from the frame data
 *
 *
 *****************************************************************************/
void CBootloader::EncodeTxFrame(char *buff, unsigned short bufflen, char *return_buff, unsigned short *return_len)
{
    unsigned short crc;
    char crc_1st, crc_2nd;
    unsigned short tx_packet_len;
    int i;

    crc = CalculateCrc(buff, bufflen);
    crc_1st = (char)crc;
    crc_2nd = (char)(crc >> 8);

    tx_packet_len = 0;


    // SOH: Start of header
    return_buff[tx_packet_len++] = SOH;

    for(i = 0; i < bufflen; i++)
    {
        if((buff[i] == EOT) || (buff[i] == SOH)
                || (buff[i] == DLE))
        {
            return_buff[tx_packet_len++] = DLE;
        }
        return_buff[tx_packet_len++] = buff[i];
    }

    // CRC
    if((crc_1st == EOT) || (crc_1st == SOH)
            || (crc_1st == DLE))
    {
        return_buff[tx_packet_len++] = DLE;
    }

    return_buff[tx_packet_len++] = crc_1st;


    if((crc_2nd == EOT) || (crc_2nd == SOH)
            || (crc_2nd == DLE))
    {
        return_buff[tx_packet_len++] = DLE;
    }

    return_buff[tx_packet_len++] = crc_2nd;

    // EOT: End of transmission
    return_buff[tx_packet_len++] = EOT;

    *return_len = tx_packet_len;
}
