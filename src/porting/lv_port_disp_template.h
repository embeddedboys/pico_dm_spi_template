/**
 * @file lv_port_disp_linux.h
 *
 */

/*Copy this file as "lv_port_disp.h" and set this value to "1" to enable content*/
#if 1

#ifndef LV_PORT_DISP_LINUX_H
#define LV_PORT_DISP_LINUX_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lvgl/lvgl.h"
#include "tft.h"

/*********************
 *      DEFINES
 *********************/
#define MY_DISP_HOR_RES (TFT_HOR_RES)
#define MY_DISP_VER_RES (TFT_VER_RES)
/* error handler */
#define handle_error(msg) do {perror(msg);exit(-1);} \
    while(0)

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/
void lv_port_disp_init( void );

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_PORT_DISP_TEMPL_H*/

#endif /*Disable/Enable content*/
