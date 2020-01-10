/**
 * @file shell_port.c
 * @author EtcFly (EtcFlywy@126.com)
 * @brief Portable interface for letter shell
 * @version 1.0.0
 * @date 2020-01-10
 * 
 * @Copyright (c) 2019 Letter
 * 
 */
#ifndef _SHELL_PORT_H
#define _SHELL_PORT_H




#ifdef __cplusplus
extern "C" {
#endif

void shell_initAll(void);
SHELL_TypeDef *get_default_shell(void);
void set_defaultShell(SHELL_TypeDef *shell);

#ifdef __cplusplus
}
#endif

#endif



