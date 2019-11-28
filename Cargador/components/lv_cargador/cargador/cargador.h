/**
 * @file cargador.h
 *
 */

#ifndef CARGADOR_H
#define CARGADOR_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#ifdef LV_CONF_INCLUDE_SIMPLE
#include "lvgl.h"
#include "lv_ex_conf.h"
#else
#include "../../../lvgl/lvgl.h"
#include "../../../lv_ex_conf.h"
#endif

#if LV_USE_DEMO

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a demo application
 */
void cargador_create(void);

void screen_welcome(void);
void screen_code(void);
void screen_carga_one(void);
void screen_carga_two(void);


void screen_alert_info();
void screen_alert_info_error(void);
void screen_alert_info_outService(void);
void screen_alert_info_wait(void);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_DEMO*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*DEMO_H*/
