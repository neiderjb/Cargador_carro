/**
 * @file demo.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "cargador.h"
#include "drv/Parameters.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if LV_USE_DEMO

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void mbox_event_cb(lv_obj_t *obj, lv_event_t evt);
static void btn_event_cb(lv_obj_t *obj, lv_event_t event);
static void kb_event_cb(lv_obj_t *event_kb, lv_event_t event);
static void ta_event_cb(lv_obj_t *ta, lv_event_t event);

lv_obj_t *scr1;
lv_obj_t *win;

lv_obj_t *cont_screen_welcome;
lv_obj_t *cont_screen_code;
lv_obj_t *cont_screen_CharOne;
lv_obj_t *cont_screen_CharTwo;
lv_obj_t *cont_screen_alert_info;
lv_obj_t *cont_screen_alert_info_wait;
lv_obj_t *cont_screen_alert_info_outService;

lv_obj_t *btn_info;
lv_obj_t *btn_config;
lv_obj_t *btn_airis;
lv_obj_t *btn_close_alert;

lv_obj_t *btn1, *btn2, *btn3, *btn4;
lv_obj_t *labelCharger;

lv_coord_t hres;
lv_coord_t vres;

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_obj_t *mbox, *info;
lv_obj_t *mbox1;
static lv_obj_t *chart;
static lv_obj_t *ta; //Text Area
static lv_obj_t *kb; //Teclado

LV_IMG_DECLARE(img_logo)
// LV_IMG_DECLARE(img_touch)
LV_IMG_DECLARE(img_alert2)
LV_IMG_DECLARE(img_airis_logo)

//LV_IMG_DECLARE(img_touch)

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Create a demo application
 */
void cargador_create(void)
{
	hres = lv_disp_get_hor_res(NULL);
	vres = lv_disp_get_ver_res(NULL);

	scr1 = lv_page_create(NULL, NULL);

	//scr1 = lv_obj_create(NULL, NULL);
	lv_disp_load_scr(scr1);

	screen_welcome();
	printf("CargadorScreen GUI OK\n");
}

/**
 * Create 1 screen application
 */
void screen_welcome(void)
{

	cont_screen_welcome = lv_cont_create(lv_scr_act(), NULL);
	lv_obj_set_auto_realign(cont_screen_welcome, true);
	lv_obj_align_origo(cont_screen_welcome, NULL, LV_ALIGN_CENTER, 0, 0); /*This parametrs will be sued when realigned*/
	lv_cont_set_fit(cont_screen_welcome, LV_FIT_FLOOD);
	lv_cont_set_layout(cont_screen_welcome, LV_LAYOUT_OFF);
	lv_obj_set_event_cb(cont_screen_welcome, btn_event_cb);

	lv_obj_t *labelwelcome = lv_label_create(cont_screen_welcome, NULL); /*First parameters (scr) is the parent*/
	lv_label_set_text(labelwelcome, "Bienvenido");						 /*Set the text*/
	lv_obj_align(labelwelcome, NULL, LV_ALIGN_IN_TOP_MID, 0, 20);

	lv_obj_t *logo = lv_img_create(cont_screen_welcome, NULL);
	lv_img_set_src(logo, &img_airis_logo);
	lv_obj_align(logo, NULL, LV_ALIGN_IN_TOP_MID, 0, 40);

	lv_obj_t *label;
	btn1 = lv_btn_create(cont_screen_welcome, NULL);
	lv_obj_set_size(btn1, 200, 50);
	lv_obj_set_event_cb(btn1, btn_event_cb);
	lv_obj_align(btn1, NULL, LV_ALIGN_IN_TOP_MID, -150, 290);
	label = lv_label_create(btn1, NULL);
	lv_label_set_text(label, "Phoenix Indicador ON");

	btn2 = lv_btn_create(cont_screen_welcome, NULL);
	lv_obj_set_size(btn2, 200, 50);
	lv_obj_set_event_cb(btn2, btn_event_cb);
	lv_obj_align(btn2, NULL, LV_ALIGN_IN_TOP_MID, 150, 290);
	labelCharger = lv_label_create(btn2, NULL);
	lv_label_set_text(labelCharger, "INICIAR CARGA");
	lv_label_set_recolor(labelCharger, true);

	btn3 = lv_btn_create(cont_screen_welcome, NULL);
	lv_obj_set_size(btn3, 200, 50);
	lv_obj_set_event_cb(btn3, btn_event_cb);
	lv_obj_align(btn3, NULL, LV_ALIGN_IN_TOP_MID, -150, 350);
	label = lv_label_create(btn3, NULL);
	lv_label_set_text(label, "Rele Carga");

	btn4 = lv_btn_create(cont_screen_welcome, NULL);
	lv_obj_set_size(btn4, 200, 50);
	lv_obj_set_event_cb(btn4, btn_event_cb);
	lv_obj_align(btn4, NULL, LV_ALIGN_IN_TOP_MID, 150, 350);
	label = lv_label_create(btn4, NULL);
	lv_label_set_text(label, "Indicador Led");

	labelTime = lv_label_create(cont_screen_welcome, NULL);
	lv_label_set_recolor(labelTime, true);
	lv_label_set_text(labelTime, "Tiempo:"); /*Set the text*/
	lv_obj_align(labelTime, NULL, LV_ALIGN_IN_TOP_MID, 500, 410);

	labelVol = lv_label_create(cont_screen_welcome, NULL);
	lv_label_set_recolor(labelVol, true);
	lv_label_set_text(labelVol, "Voltaje:"); /*Set the text*/
	lv_obj_align(labelVol, NULL, LV_ALIGN_IN_TOP_MID, -200, 410);

	labelCur = lv_label_create(cont_screen_welcome, NULL);
	lv_label_set_recolor(labelCur, true);
	lv_label_set_text(labelCur, "Corriente:"); /*Set the text*/
	lv_obj_align(labelCur, NULL, LV_ALIGN_IN_TOP_MID, -75, 410);

	labelPow = lv_label_create(cont_screen_welcome, NULL);
	lv_label_set_recolor(labelPow, true);
	lv_label_set_text(labelPow, "Potencia:"); /*Set the text*/
	lv_obj_align(labelPow, NULL, LV_ALIGN_IN_TOP_MID, 75, 410);

	labelCon = lv_label_create(cont_screen_welcome, NULL);
	lv_label_set_recolor(labelCon, true);
	lv_label_set_text(labelCon, "Consumo:"); /*Set the text*/
	lv_obj_align(labelCon, NULL, LV_ALIGN_IN_TOP_MID, 200, 410);
}

void lv_ex_mbox_1(void)
{
	static const char *btns[] = {"Si que va!", "Bueno Toco...", ""};

	mbox1 = lv_mbox_create(lv_scr_act(), NULL);
	lv_mbox_set_text(mbox1, "Espere Porfavor.");
	lv_mbox_add_btns(mbox1, btns);
	lv_obj_set_width(mbox1, 200);
	lv_obj_set_event_cb(mbox1, btn_event_cb);
	lv_obj_align(mbox1, NULL, LV_ALIGN_CENTER, 0, 0); /*Align to the corner*/
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
bool statechar = false;
bool stateindi = false;

static void btn_event_cb(lv_obj_t *obj, lv_event_t event)
{
	if (event == LV_EVENT_CLICKED)
	{

		if (obj == btn1)
		{
			if (!stateindi)
			{
				stateindi = true;
				xSemaphoreGive(Semaphore_Out_Phoenix);
				printf("Clicked Phoenix Indicador OFF\n");
				//lv_ex_mbox_1();
			}
			else
			{
				stateindi = false;
				printf("Clicked Phoenix Indicador ON\n");
			}
		}
		else if (obj == btn2)
		{

			if (!statechar)
			{
				statechar = true;
				xSemaphoreGive(Semaphore_Start_Charging);
				lv_label_set_text(labelCharger, "DETENER CARGA");
				printf("Clicked Phoenix Start \n");
			}
			else
			{
				statechar = false;
				xSemaphoreGive(Semaphore_Stop_Charging);
				lv_label_set_text(labelCharger, "INICIAR CARGA");
				printf("Clicked Phoenix Stop \n");
			}
		}
		else if (obj == btn3)
		{
		}
		else if (obj == btn4)
		{
		}
	}
	else if (event == LV_EVENT_VALUE_CHANGED)
	{
		printf("Toggled\n");
	}
}

#endif /*LV_USE_DEMO*/