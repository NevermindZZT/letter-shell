/**
 * @file shell_port.c
 * @brief Project-local port layer for letter shell.
 *
 * This file is installed as a CMSIS-Pack config file. Keep application
 * specific read/write/task code here; pack updates should not replace the
 * project copy.
 */

#include "shell_port.h"

#ifndef LETTER_SHELL_BUFFER_SIZE
    #define LETTER_SHELL_BUFFER_SIZE 512
#endif

Shell       g_shell;
static char shell_buffer[LETTER_SHELL_BUFFER_SIZE];

/**
 * @brief Write bytes to the project terminal transport.
 *
 * Replace this body with UART, RTT, USB CDC, TCP, or another transport.
 *
 * @param data data buffer
 * @param len requested write length
 * @return signed short actual written length
 */
signed short shell_write(char *data, unsigned short len)
{
    (void)data;
    return (signed short)len;
}

/**
 * @brief Read bytes from the project terminal transport.
 *
 * Replace this body with a non-blocking or blocking read implementation that
 * matches how shellTask or shellHandler is driven in the project.
 *
 * @param data data buffer
 * @param len requested read length
 * @return signed short actual read length
 */
signed short shell_read(char *data, unsigned short len)
{
    (void)data;
    (void)len;
    return 0;
}

#if SHELL_USING_LOCK == 1
/**
 * @brief Lock shell output/input resources.
 *
 * Replace this body with a recursive mutex or equivalent project lock.
 */
int userShellLock(Shell *shell)
{
    (void)shell;
    return 0;
}

/**
 * @brief Unlock shell output/input resources.
 */
int userShellUnlock(Shell *shell)
{
    (void)shell;
    return 0;
}
#endif

/**
 * @brief Initialize the project shell instance.
 *
 * Call this once from project startup, then create a shellTask task or feed
 * received bytes to shellHandler according to the project integration model.
 */
void shell_init(void)
{
    g_shell.write = shell_write;
    g_shell.read = shell_read;
#if SHELL_USING_LOCK == 1
    g_shell.lock = userShellLock;
    g_shell.unlock = userShellUnlock;
#endif
    shellInit(&g_shell, shell_buffer, sizeof(shell_buffer));
}
