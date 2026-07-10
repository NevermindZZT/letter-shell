/**
 * @file shell_port.c
 * @brief SEGGER RTT port layer for letter shell.
 *
 * This file is installed as a CMSIS-Pack config file. Keep application
 * specific buffer, timing, and RTT channel choices here; pack updates should
 * not replace the project copy.
 */

#include "shell_port.h"

#include "SEGGER_RTT.h"

#ifndef shell_delay
    #error "shell_delay is not defined"
#endif

#ifndef shell_get_ms
    #error "shell_get_ms is not defined"
#endif

#ifndef LETTER_SHELL_RTT_CHANNEL
    #define LETTER_SHELL_RTT_CHANNEL 0
#endif

#ifndef LETTER_SHELL_RTT_BUFFER_SIZE
    #define LETTER_SHELL_RTT_BUFFER_SIZE 512
#endif

Shell       g_shell;
static char shell_buffer[LETTER_SHELL_RTT_BUFFER_SIZE];

/**
 * @brief RTT shell write.
 *
 * @param data data buffer
 * @param len requested write length
 * @return signed short actual written length
 */
signed short shell_write(char *data, unsigned short len)
{
    static int   blocked = 0;
    unsigned int time = shell_get_ms();
    signed short avail = 0;
    signed short write = 0;
    signed short wrote = 0;

    do
    {
        avail = (signed short)SEGGER_RTT_GetAvailWriteSpace(LETTER_SHELL_RTT_CHANNEL);
        if (avail <= 0)
        {
            if (blocked > 10)
            {
                return 0;
            }
            shell_delay(1);
            blocked++;
        }
        else
        {
            blocked = 0;
            write = avail >= (signed short)len ? (signed short)len : avail;
            SEGGER_RTT_Write(LETTER_SHELL_RTT_CHANNEL, data, (unsigned int)write);
            data += write;
            len -= (unsigned short)write;
            wrote += write;
        }
    } while (len > 0 && shell_get_ms() - time < 10U);

    return wrote;
}

/**
 * @brief RTT shell read.
 *
 * @param data data buffer
 * @param len requested read length
 * @return signed short actual read length
 */
signed short shell_read(char *data, unsigned short len)
{
    return (signed short)SEGGER_RTT_Read(LETTER_SHELL_RTT_CHANNEL, data, len);
}

/**
 * @brief Initialize the RTT shell instance.
 */
void shell_init(void)
{
    g_shell.write = shell_write;
    g_shell.read = shell_read;
    shellInit(&g_shell, shell_buffer, sizeof(shell_buffer));
}
