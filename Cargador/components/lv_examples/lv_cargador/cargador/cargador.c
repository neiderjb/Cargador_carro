/**
 * @file demo.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "cargador.h"
#include "drv/Parameters.h"
#include "drv/Functions.h"
#include "drv/FunctionsCC.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

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
//static void mbox_event_cb(lv_obj_t *obj, lv_event_t evt);
static void btn_event_cb(lv_obj_t *obj, lv_event_t event);
static void dlist_event_handler(lv_obj_t *obj, lv_event_t event);
static void kb_event_cb(lv_obj_t *event_kb, lv_event_t event);
static void ta_event_cb(lv_obj_t *ta, lv_event_t event);

lv_obj_t *scr1;

char str1[] = {'1', '2', '3', '4', '\n'};
//char str2[20];

bool welcome = false;
bool EnableCharger = true;
bool StopCharging = false;

lv_obj_t *cont_screen_welcome;

lv_obj_t *cont_screen_code;
lv_obj_t *warning;
lv_obj_t *label_codeStatus;
lv_obj_t *label_code;
lv_obj_t *ta_code;

lv_obj_t *cont_screen_alert_info_outService;
lv_obj_t *labelError;

lv_obj_t *cont_screen_init;
lv_obj_t *labelCon;
lv_obj_t *reloj;
lv_obj_t *btnCancel;

lv_obj_t *cont_screen_end;

lv_obj_t *cont_screen_CharOne;

lv_obj_t *labelVehiculo;
lv_obj_t *conecte;
lv_obj_t *conectado;
lv_obj_t *cargando;
lv_obj_t *error;
lv_obj_t *btnCancel2;
lv_obj_t *btnCargar;

lv_obj_t *cont_screen_config;
lv_obj_t *labPhoenixVersion;
lv_obj_t *labPhoenixCurrent;
lv_obj_t *labPhoenixStatus;
lv_obj_t *labPhoenixError;
lv_obj_t *btnUpdatePhoe;
lv_obj_t *btnResetPhoe;
lv_obj_t *btnContinuarConfig;
lv_obj_t *btnCancelConfig;
lv_obj_t *ta_PASS;
lv_obj_t *ddlistTiempo;
lv_obj_t *ddlistTrifasica;
lv_obj_t *cb_fase1;
lv_obj_t *cb_fase2;
lv_obj_t *cb_fase3;

lv_obj_t *btn_info;
lv_obj_t *btn_config;
lv_obj_t *btn_airis;
lv_obj_t *btn_close_alert;

lv_coord_t hres;
lv_coord_t vres;

/**********************
 *  STATIC VARIABLES
 **********************/
//static lv_obj_t *mbox, *info;
//static lv_obj_t *chart;
//static lv_obj_t *ta; //Text Area
static lv_obj_t *kb; //Teclado

LV_IMG_DECLARE(img_logo_small)
LV_IMG_DECLARE(img_tactil)
LV_IMG_DECLARE(img_warning)
LV_IMG_DECLARE(img_reloj)
LV_IMG_DECLARE(img_conect)
LV_IMG_DECLARE(img_conectado)
LV_IMG_DECLARE(img_cargando)
LV_IMG_DECLARE(img_error)
LV_IMG_DECLARE(img_airis_logo)

/**********************
 *      MACROS
 **********************/

/**********************
  *   GLOBAL FUNCTIONS
  **********************/

static lv_theme_t *th;
static lv_style_t styleLabel1;
static lv_style_t styleLabel2;
static lv_style_t styleContent1;
static lv_style_t stylepreload;

/**
 * Create a demo application
 */
void cargador_create(void)
{
	th = lv_theme_mono_init(210, NULL);
	lv_theme_set_current(th);

	lv_style_copy(&styleLabel1, &lv_style_plain);
	styleLabel1.text.font = &lv_font_roboto_28;

	lv_style_copy(&styleLabel2, &lv_style_plain);
	styleLabel2.text.font = &lv_font_roboto_22;

	styleContent1.image.color = LV_COLOR_WHITE;

	lv_style_copy(&stylepreload, &lv_style_plain);
	stylepreload.line.width = 10;						   /*10 px thick arc*/
	stylepreload.line.color = lv_color_hex3(0x258);		   /*Blueish arc color*/
	stylepreload.body.border.color = lv_color_hex3(0xBBB); /*Gray background color*/
	stylepreload.body.border.width = 10;
	stylepreload.body.padding.left = 0;

	hres = lv_disp_get_hor_res(NULL);
	vres = lv_disp_get_ver_res(NULL);

	scr1 = lv_page_create(NULL, NULL);
	lv_disp_load_scr(scr1);
	if (!detectTouch && !detectModbus && !detectAnalizer)
	{
		screen_alert_info_outService();
		if (!detectTouch)
		{
			update_label_alert_info_outService("No se detecto el touch");
		}
		if (!detectModbus)
		{
			update_label_alert_info_outService("No se detecto Phoenix Contact");
		}
		if (!detectAnalizer)
		{
			update_label_alert_info_outService("No se detecto Analizador de Red");
		}
	}
	else
	{
		screen_welcome();
	}
}

/**
 * Create 1 screen application
 */
void screen_welcome(void)
{
	welcome = false;
	cont_screen_welcome = lv_cont_create(lv_scr_act(), NULL);
	//lv_cont_set_style(cont_screen_welcome, LV_CONT_STYLE_MAIN,&styleContent1);
	lv_obj_set_event_cb(cont_screen_welcome, btn_event_cb);
	lv_obj_set_auto_realign(cont_screen_welcome, true);
	lv_obj_align_origo(cont_screen_welcome, NULL, LV_ALIGN_CENTER, 0, 0); /*This parametrs will be sued when realigned*/
	lv_cont_set_fit(cont_screen_welcome, LV_FIT_FLOOD);
	lv_cont_set_layout(cont_screen_welcome, LV_LAYOUT_OFF);

	lv_obj_t *label = lv_label_create(cont_screen_welcome, NULL); /*First parameters (scr) is the parent*/
	lv_label_set_style(label, LV_LABEL_LONG_EXPAND, &styleLabel1);
	lv_label_set_text(label, "Bienvenido"); /*Set the text*/
	lv_obj_align(label, NULL, LV_ALIGN_IN_TOP_MID, 0, 20);

	lv_obj_t *logo = lv_img_create(cont_screen_welcome, NULL);
	lv_img_set_src(logo, &img_airis_logo);
	lv_obj_align(logo, NULL, LV_ALIGN_CENTER, 0, -20);

	lv_obj_t *touch = lv_img_create(cont_screen_welcome, NULL);
	lv_img_set_src(touch, &img_tactil);
	lv_obj_set_size(touch, 80, 80);
	lv_obj_align(touch, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, -70);

	lv_obj_t *label2 = lv_label_create(cont_screen_welcome, NULL); /*First parameters (scr) is the parent*/
	lv_label_set_style(label2, LV_LABEL_LONG_EXPAND, &styleLabel1);
	lv_label_set_text(label2, "Toque la pantalla para empezar"); /*Set the text*/
	lv_obj_align(label2, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, -20);
}

void screen_code()
{
	cont_screen_code = lv_cont_create(lv_scr_act(), NULL);
	lv_obj_set_auto_realign(cont_screen_code, true);
	lv_obj_align_origo(cont_screen_code, NULL, LV_ALIGN_CENTER, 0, 0); /*This parametrs will be sued when realigned*/
	lv_cont_set_fit(cont_screen_code, LV_FIT_FLOOD);
	lv_cont_set_layout(cont_screen_code, LV_LAYOUT_OFF);

	lv_obj_t *logosmall = lv_img_create(cont_screen_code, NULL);
	lv_obj_set_event_cb(logosmall, btn_event_cb);
	lv_img_set_src(logosmall, &img_logo_small);
	lv_obj_align(logosmall, NULL, LV_ALIGN_IN_TOP_RIGHT, -20, 10);

	label_codeStatus = lv_label_create(cont_screen_code, NULL); /*First parameters (scr) is the parent*/
	lv_label_set_recolor(label_codeStatus, true);
	lv_label_set_text(label_codeStatus, ""); /*Set the text*/
	lv_obj_set_size(label_codeStatus, 300, 50);
	lv_obj_align(label_codeStatus, NULL, LV_ALIGN_IN_TOP_MID, 0, 70);

	label_code = lv_label_create(cont_screen_code, NULL);
	lv_label_set_recolor(label_code, true);
	if (EnableCharger)
	{
		lv_label_set_text(label_code, "Ingrese su codigo aqui:");
	}
	else
	{
		lv_label_set_text(label_code, "Para cancelar la recarga ingrese su codigo aqui:");
	}

	lv_obj_set_size(label_code, 300, 50);
	lv_obj_align(label_code, NULL, LV_ALIGN_IN_TOP_MID, 0, 130);

	ta_code = lv_ta_create(cont_screen_code, NULL);
	lv_ta_set_text(ta_code, "");
	lv_ta_set_pwd_mode(ta_code, true);
	lv_ta_set_one_line(ta_code, true);
	lv_obj_set_size(ta_code, 400, 50);
	lv_obj_align(ta_code, NULL, LV_ALIGN_IN_TOP_MID, 0, 150);
	//lv_obj_set_pos(ta_code, 200, 150);
	lv_obj_set_event_cb(ta_code, ta_event_cb);

	/* Create a keyboard and make it fill the width of the above text areas */
	kb = lv_kb_create(cont_screen_code, NULL);
	lv_obj_set_pos(kb, 7, 220);
	lv_obj_set_event_cb(kb, kb_event_cb); /* Setting a custom event handler stops the keyboard from closing automatically */
	lv_obj_set_size(kb, hres - 40, 220);
	lv_kb_set_ta(kb, ta_code);		   /* Focus it on one of the text areas to start */
	lv_kb_set_cursor_manage(kb, true); /* Automatically show/hide cursors on text areas */
}

void screen_init_carga()
{
	cont_screen_init = lv_cont_create(lv_scr_act(), NULL);
	lv_obj_set_auto_realign(cont_screen_init, true);
	lv_obj_align_origo(cont_screen_init, NULL, LV_ALIGN_CENTER, 0, 0); /*This parametrs will be sued when realigned*/
	lv_cont_set_fit(cont_screen_init, LV_FIT_FLOOD);
	lv_cont_set_layout(cont_screen_init, LV_LAYOUT_OFF);

	lv_obj_t *logosmall = lv_img_create(cont_screen_init, NULL);
	lv_img_set_src(logosmall, &img_logo_small);
	lv_obj_align(logosmall, NULL, LV_ALIGN_IN_TOP_RIGHT, -20, 10);

	labelCon = lv_label_create(cont_screen_init, NULL); /*First parameters (scr) is the parent*/
	lv_obj_set_event_cb(labelCon, btn_event_cb);
	lv_label_set_style(labelCon, LV_LABEL_LONG_EXPAND, &styleLabel1);
	lv_label_set_text(labelCon, "   Espere por favor\nrealizando conexion"); /*Set the text*/
	lv_obj_align(labelCon, NULL, LV_ALIGN_IN_TOP_MID, 0, 20);

	reloj = lv_img_create(cont_screen_init, NULL);
	lv_obj_set_event_cb(reloj, btn_event_cb);
	lv_img_set_src(reloj, &img_reloj);
	lv_obj_align(reloj, NULL, LV_ALIGN_CENTER, 0, -40);

	lv_obj_t *label = lv_label_create(cont_screen_init, NULL); /*First parameters (scr) is the parent*/
	lv_obj_set_event_cb(label, btn_event_cb);
	lv_label_set_style(label, LV_LABEL_LONG_EXPAND, &styleLabel1);

	lv_label_set_text(label, "   Validando Ticket"); /*Set the text*/

	lv_obj_align(label, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, -120);
	ESP_ERROR_CHECK(esp_timer_start_once(Timer_Screen_Control, 3000000));

	/*Create a Preloader object*/
	lv_obj_t *preload = lv_preload_create(cont_screen_init, NULL);
	lv_obj_set_size(preload, 100, 100);
	lv_obj_align(preload, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, -10);
	lv_preload_set_style(preload, LV_PRELOAD_STYLE_MAIN, &stylepreload);
}

void screen_end_carga()
{

	cont_screen_end = lv_cont_create(lv_scr_act(), NULL);
	lv_obj_set_auto_realign(cont_screen_end, true);
	lv_obj_align_origo(cont_screen_end, NULL, LV_ALIGN_CENTER, 0, 0); /*This parametrs will be sued when realigned*/
	lv_cont_set_fit(cont_screen_end, LV_FIT_FLOOD);
	lv_cont_set_layout(cont_screen_end, LV_LAYOUT_OFF);

	lv_obj_t *logosmall = lv_img_create(cont_screen_end, NULL);
	lv_img_set_src(logosmall, &img_logo_small);
	lv_obj_align(logosmall, NULL, LV_ALIGN_IN_TOP_RIGHT, -20, 10);

	labelCon = lv_label_create(cont_screen_end, NULL); /*First parameters (scr) is the parent*/
	lv_obj_set_event_cb(labelCon, btn_event_cb);
	lv_label_set_style(labelCon, LV_LABEL_LONG_EXPAND, &styleLabel1);
	lv_label_set_text(labelCon, "   Espere por favor\nrealizando conexion"); /*Set the text*/
	lv_obj_align(labelCon, NULL, LV_ALIGN_IN_TOP_MID, 0, 20);

	reloj = lv_img_create(cont_screen_end, NULL);
	lv_obj_set_event_cb(reloj, btn_event_cb);
	lv_img_set_src(reloj, &img_reloj);
	lv_obj_align(reloj, NULL, LV_ALIGN_CENTER, 0, -40);

	lv_obj_t *label = lv_label_create(cont_screen_end, NULL); /*First parameters (scr) is the parent*/
	lv_obj_set_event_cb(label, btn_event_cb);
	lv_label_set_style(label, LV_LABEL_LONG_EXPAND, &styleLabel1);

	lv_label_set_text(label, "   Carga terminada cerrando Ticket"); /*Set the text*/

	lv_obj_align(label, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, -120);
	ESP_ERROR_CHECK(esp_timer_start_once(Timer_Screen_Control, 5000000));

	/*Create a Preloader object*/
	lv_obj_t *preload = lv_preload_create(cont_screen_end, NULL);
	lv_obj_set_size(preload, 100, 100);
	lv_obj_align(preload, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, -10);
	lv_preload_set_style(preload, LV_PRELOAD_STYLE_MAIN, &stylepreload);
}

void screen_carga_one()
{
	cont_screen_CharOne = lv_cont_create(lv_scr_act(), NULL);
	lv_obj_set_auto_realign(cont_screen_CharOne, true);
	lv_obj_align_origo(cont_screen_CharOne, NULL, LV_ALIGN_CENTER, 0, 0); /*This parametrs will be sued when realigned*/
	lv_cont_set_fit(cont_screen_CharOne, LV_FIT_FLOOD);
	lv_cont_set_layout(cont_screen_CharOne, LV_LAYOUT_OFF);

	lv_obj_t *logosmall = lv_img_create(cont_screen_CharOne, NULL);
	lv_img_set_src(logosmall, &img_logo_small);
	lv_obj_align(logosmall, NULL, LV_ALIGN_IN_TOP_RIGHT, -20, 10);

	conecte = lv_img_create(cont_screen_CharOne, NULL);
	lv_img_set_src(conecte, &img_conect);
	lv_obj_align(conecte, NULL, LV_ALIGN_CENTER, -270, -20);

	//-------------------------------------------------------------------------------//
	lv_obj_t *cont = lv_cont_create(cont_screen_CharOne, NULL);
	lv_obj_set_auto_realign(cont, true);
	lv_obj_set_size(cont, 480, 300);
	lv_obj_align_origo(cont, NULL, LV_ALIGN_CENTER, 100, -10); /*This parametrs will be sued when realigned*/
	lv_cont_set_fit2(cont, LV_FIT_NONE, LV_FIT_NONE);
	lv_cont_set_layout(cont, LV_LAYOUT_OFF);

	lv_obj_t *label = lv_label_create(cont, NULL); /*First parameters (scr) is the parent*/
	lv_label_set_style(label, LV_LABEL_LONG_EXPAND, &styleLabel1);
	lv_label_set_text(label, "Potencia: "); /*Set the text*/
	lv_obj_align(label, NULL, LV_ALIGN_IN_TOP_LEFT, 50, 40);
	labelPotencia = lv_label_create(cont, NULL); /*First parameters (scr) is the parent*/
	lv_label_set_style(labelPotencia, LV_LABEL_LONG_EXPAND, &styleLabel1);
	lv_label_set_text(labelPotencia, "-- KW"); /*Set the text*/
	lv_obj_align(labelPotencia, NULL, LV_ALIGN_IN_TOP_RIGHT, -110, 40);

	label = lv_label_create(cont, NULL); /*First parameters (scr) is the parent*/
	lv_label_set_style(label, LV_LABEL_LONG_EXPAND, &styleLabel1);
	lv_label_set_text(label, "Carga: "); /*Set the text*/
	lv_obj_align(label, NULL, LV_ALIGN_IN_TOP_LEFT, 50, 110);
	labelCarga = lv_label_create(cont, NULL); /*First parameters (scr) is the parent*/
	lv_label_set_style(labelCarga, LV_LABEL_LONG_EXPAND, &styleLabel1);
	lv_label_set_text(labelCarga, "-- KW/h"); /*Set the text*/
	lv_obj_align(labelCarga, NULL, LV_ALIGN_IN_TOP_RIGHT, -110, 110);

	label = lv_label_create(cont, NULL); /*First parameters (scr) is the parent*/
	lv_label_set_style(label, LV_LABEL_LONG_EXPAND, &styleLabel1);
	lv_label_set_text(label, "Coste: "); /*Set the text*/
	lv_obj_align(label, NULL, LV_ALIGN_IN_TOP_LEFT, 50, 180);
	labelCoste = lv_label_create(cont, NULL); /*First parameters (scr) is the parent*/
	lv_label_set_style(labelCoste, LV_LABEL_LONG_EXPAND, &styleLabel1);
	lv_label_set_text(labelCoste, "-- Euros"); /*Set the text*/
	lv_obj_align(labelCoste, NULL, LV_ALIGN_IN_TOP_RIGHT, -110, 180);

	label = lv_label_create(cont, NULL); /*First parameters (scr) is the parent*/
	lv_label_set_style(label, LV_LABEL_LONG_EXPAND, &styleLabel1);
	lv_label_set_text(label, "Tiempo: "); /*Set the text*/
	lv_obj_align(label, NULL, LV_ALIGN_IN_TOP_LEFT, 50, 250);
	labelTiempo = lv_label_create(cont, NULL); /*First parameters (scr) is the parent*/
	lv_label_set_style(labelTiempo, LV_LABEL_LONG_EXPAND, &styleLabel1);
	lv_label_set_text(labelTiempo, "-- Minutos"); /*Set the text*/
	lv_obj_align(labelTiempo, NULL, LV_ALIGN_IN_TOP_RIGHT, -110, 250);

	//-------------------------------------------------------------------------------//

	labelVehiculo = lv_label_create(cont_screen_CharOne, NULL); /*First parameters (scr) is the parent*/
	lv_label_set_style(labelVehiculo, LV_LABEL_LONG_EXPAND, &styleLabel1);
	lv_label_set_text(labelVehiculo, ""); /*Set the text*/
	lv_obj_align(labelVehiculo, NULL, LV_ALIGN_IN_LEFT_MID, 10, 70);

	btnCancel2 = lv_btn_create(cont_screen_CharOne, NULL);
	lv_btn_set_state(btnCancel2, LV_BTN_STATE_INA);
	//lv_btn_set_style(btnCancel2, LV_BTN_STYLE_INA);
	//lv_btn_set_toggle(btnCancel2, false);
	lv_obj_set_event_cb(btnCancel2, btn_event_cb);
	lv_obj_set_size(btnCancel2, 400, 50);
	lv_obj_align(btnCancel2, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, -20);
	lv_obj_t *labelbtn = lv_label_create(btnCancel2, NULL);
	lv_label_set_text(labelbtn, "DETENER CARGA");

	// ready_information = true;
}

//Al detectar la pistola actualiza el logo del carro, lo pasa de rojo a gris
void update_conectado_carga_one()
{
	lv_obj_del(conecte);
	conectado = lv_img_create(cont_screen_CharOne, NULL);
	lv_img_set_src(conectado, &img_conectado);
	lv_obj_align(conectado, NULL, LV_ALIGN_CENTER, -270, -20);

	lv_label_set_text(labelVehiculo, " Vehiculo\nConectado"); /*Set the text*/
	lv_obj_align(labelVehiculo, NULL, LV_ALIGN_CENTER, -270, -120);
}
//Al iniciar el proceso de carga, actualiza el logo del carro, lo pasa de gris a verde
void update_cargando_carga_one()
{

	lv_obj_del(conectado);
	cargando = lv_img_create(cont_screen_CharOne, NULL);
	lv_img_set_src(cargando, &img_cargando);
	lv_obj_align(cargando, NULL, LV_ALIGN_CENTER, -270, -20);

	lv_label_set_text(labelVehiculo, " Vehiculo\nCargando"); /*Set the text*/
	lv_obj_align(labelVehiculo, NULL, LV_ALIGN_CENTER, -270, -120);
	lv_btn_set_state(btnCancel2, LV_BTN_STATE_REL);
	//lv_btn_set_toggle(btnCancel2, true);
	//lv_btn_set_style(btnCancel2, LV_BTN_STYLE_REL);
}

void update_error_carga_one(uint16_t Error)
{
	lv_obj_del(cargando);
	error = lv_img_create(cont_screen_CharOne, NULL);
	lv_img_set_src(conecte, &img_error);
	lv_obj_align(conecte, NULL, LV_ALIGN_CENTER, -270, -20);

	lv_label_set_text(labelVehiculo, phoenixError(Error)); /*Set the text*/
	lv_obj_align(labelVehiculo, NULL, LV_ALIGN_CENTER, -270, -120);
}

void update_label_carga_one(float potencia, float carga, float coste, float tiempo)
{
	char res[20];
	char dest[20];

	ftoa(potencia, res, 2);
	strcpy(dest, " W");
	//strcat(dest, res);
	strcat(res, dest);
	lv_label_set_text(labelPotencia, res); /*Set the text*/
	memset(dest, 0, sizeof(dest));

	ftoa(carga, res, 2);
	strcpy(dest, " W/h");
	strcat(res, dest);
	lv_label_set_text(labelCarga, res); /*Set the text*/
	memset(dest, 0, sizeof(dest));

	ftoa(coste, res, 2);
	strcpy(dest, " Euros");
	strcat(res, dest);
	lv_label_set_text(labelCoste, res); /*Set the text*/
	memset(dest, 0, sizeof(dest));

	ftoa(tiempo, res, 2);
	strcpy(dest, " Minutos");
	strcat(res, dest);
	lv_label_set_text(labelTiempo, res); /*Set the text*/
	memset(dest, 0, sizeof(dest));
}

void close_carga_one()
{
	//ready_information = false;
	EnableCharger = false;
	StopCharging = true;

	screen_end_carga();
	lv_obj_del(cont_screen_CharOne);
}

void screen_alert_info_outService()
{
	//ready_information = false;
	cont_screen_alert_info_outService = lv_cont_create(lv_scr_act(), NULL);
	lv_obj_set_auto_realign(cont_screen_alert_info_outService, true);
	lv_obj_align_origo(cont_screen_alert_info_outService, NULL, LV_ALIGN_CENTER, 0, 0); /*This parametrs will be sued when realigned*/
	lv_cont_set_fit(cont_screen_alert_info_outService, LV_FIT_FLOOD);
	lv_cont_set_layout(cont_screen_alert_info_outService, LV_LAYOUT_OFF);

	lv_obj_t *label = lv_label_create(cont_screen_alert_info_outService, NULL); /*First parameters (scr) is the parent*/
	lv_label_set_style(label, LV_LABEL_LONG_EXPAND, &styleLabel1);
	lv_label_set_text(label, "FUERA DE SERVICIO"); /*Set the text*/
	lv_obj_align(label, NULL, LV_ALIGN_IN_TOP_MID, 0, 20);

	lv_obj_t *logo = lv_img_create(cont_screen_alert_info_outService, NULL);
	lv_img_set_src(logo, &img_airis_logo);
	lv_obj_align(logo, NULL, LV_ALIGN_CENTER, 0, -20);

	labelError = lv_label_create(cont_screen_alert_info_outService, NULL); /*First parameters (scr) is the parent*/
	lv_label_set_style(labelError, LV_LABEL_LONG_EXPAND, &styleLabel1);
	lv_label_set_text(labelError, ""); /*Set the text*/
	lv_obj_align(labelError, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, -50);

	lv_obj_t *label2 = lv_label_create(cont_screen_alert_info_outService, NULL); /*First parameters (scr) is the parent*/
	lv_label_set_style(label2, LV_LABEL_LONG_EXPAND, &styleLabel1);
	lv_label_set_text(label2, "DISCULPE LAS MOLESTIAS"); /*Set the text*/
	lv_obj_align(label2, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, -20);
}

void update_label_alert_info_outService(char *error)
{
	lv_label_set_text(labelError, ""); /*Set the text*/
}

void screen_configuration()
{
	cont_screen_config = lv_cont_create(lv_scr_act(), NULL);
	lv_obj_set_auto_realign(cont_screen_config, true);
	lv_obj_align_origo(cont_screen_config, NULL, LV_ALIGN_CENTER, 0, 0); /*This parametrs will be sued when realigned*/
	lv_cont_set_fit(cont_screen_config, LV_FIT_FLOOD);
	lv_cont_set_layout(cont_screen_config, LV_LAYOUT_OFF);

	lv_obj_t *logosmall = lv_img_create(cont_screen_config, NULL);
	lv_img_set_src(logosmall, &img_logo_small);
	lv_obj_align(logosmall, NULL, LV_ALIGN_IN_TOP_RIGHT, -20, 10);

	lv_obj_t *label = lv_label_create(cont_screen_config, NULL); /*First parameters (scr) is the parent*/
	lv_obj_set_event_cb(label, btn_event_cb);
	lv_label_set_style(label, LV_LABEL_LONG_EXPAND, &styleLabel1);
	lv_label_set_text(label, "CONFIGURACION"); /*Set the text*/
	lv_obj_align(label, NULL, LV_ALIGN_IN_TOP_MID, 0, 20);

	//-------------------------------------------------------------------------------------//
	lv_obj_t *cont = lv_cont_create(cont_screen_config, NULL);
	lv_obj_set_auto_realign(cont, true);
	lv_obj_set_size(cont, 360, 300);
	lv_obj_align_origo(cont, NULL, LV_ALIGN_OUT_LEFT_TOP, 200, 230); /*This parametrs will be sued when realigned*/
	lv_cont_set_fit2(cont, LV_FIT_NONE, LV_FIT_NONE);
	lv_cont_set_layout(cont, LV_LAYOUT_OFF);

	lv_obj_t *labelPhoenix = lv_label_create(cont, NULL); /*First parameters (scr) is the parent*/
	lv_obj_set_event_cb(labelPhoenix, btn_event_cb);
	lv_label_set_style(labelPhoenix, LV_LABEL_LONG_EXPAND, &styleLabel2);
	lv_label_set_text(labelPhoenix, "Phoenix Contact"); /*Set the text*/
	lv_obj_align(labelPhoenix, NULL, LV_ALIGN_IN_TOP_MID, 0, 10);

	labPhoenixVersion = lv_label_create(cont, NULL); /*First parameters (scr) is the parent*/
	lv_obj_set_event_cb(labPhoenixVersion, btn_event_cb);
	lv_label_set_style(labPhoenixVersion, LV_LABEL_LONG_EXPAND, &styleLabel2);
	lv_label_set_text(labPhoenixVersion, "-Serial Phoenix:"); /*Set the text*/
	lv_obj_align(labPhoenixVersion, NULL, LV_ALIGN_IN_TOP_LEFT, 10, 50);

	labPhoenixCurrent = lv_label_create(cont, NULL); /*First parameters (scr) is the parent*/
	lv_obj_set_event_cb(labPhoenixCurrent, btn_event_cb);
	lv_label_set_style(labPhoenixCurrent, LV_LABEL_LONG_EXPAND, &styleLabel2);
	lv_label_set_text(labPhoenixCurrent, "-Current Max:"); /*Set the text*/
	lv_obj_align(labPhoenixCurrent, NULL, LV_ALIGN_IN_TOP_LEFT, 10, 90);

	labPhoenixStatus = lv_label_create(cont, NULL); /*First parameters (scr) is the parent*/
	lv_obj_set_event_cb(labPhoenixStatus, btn_event_cb);
	lv_label_set_style(labPhoenixStatus, LV_LABEL_LONG_EXPAND, &styleLabel2);
	lv_label_set_text(labPhoenixStatus, "-Status IEC 6185-1:"); /*Set the text*/
	lv_obj_align(labPhoenixStatus, NULL, LV_ALIGN_IN_TOP_LEFT, 10, 130);

	labPhoenixError = lv_label_create(cont, NULL); /*First parameters (scr) is the parent*/
	lv_obj_set_event_cb(labPhoenixError, btn_event_cb);
	lv_label_set_style(labPhoenixError, LV_LABEL_LONG_EXPAND, &styleLabel2);
	lv_label_set_text(labPhoenixError, "-Error Status:"); /*Set the text*/
	lv_obj_align(labPhoenixError, NULL, LV_ALIGN_IN_TOP_LEFT, 10, 190);

	btnResetPhoe = lv_btn_create(cont, NULL);
	lv_obj_set_event_cb(btnResetPhoe, btn_event_cb);
	lv_obj_set_size(btnResetPhoe, 150, 30);
	lv_obj_align(btnResetPhoe, NULL, LV_ALIGN_IN_TOP_MID, 0, 250);
	lv_obj_t *labelbtnreset = lv_label_create(btnResetPhoe, NULL);
	lv_label_set_text(labelbtnreset, "Reiniciar Phoenix");

	//-------------------------------------------------------------------------------------//
	lv_obj_t *contConf2 = lv_cont_create(cont_screen_config, NULL);
	lv_obj_set_auto_realign(contConf2, true);
	lv_obj_set_size(contConf2, 360, 300);
	lv_obj_align_origo(contConf2, NULL, LV_ALIGN_OUT_RIGHT_TOP, -200, 230); /*This parametrs will be sued when realigned*/
	lv_cont_set_fit2(contConf2, LV_FIT_NONE, LV_FIT_NONE);
	lv_cont_set_layout(contConf2, LV_LAYOUT_OFF);

	lv_obj_t *labelConf2 = lv_label_create(contConf2, NULL); /*First parameters (scr) is the parent*/
	lv_obj_set_event_cb(labelConf2, btn_event_cb);
	lv_label_set_style(labelConf2, LV_LABEL_LONG_EXPAND, &styleLabel2);
	lv_label_set_text(labelConf2, "Otras configuraciones"); /*Set the text*/
	lv_obj_align(labelConf2, NULL, LV_ALIGN_IN_TOP_MID, 0, 10);

	// lv_obj_t *labelPass = lv_label_create(contConf2, NULL); /*First parameters (scr) is the parent*/
	// lv_label_set_style(labelPass, LV_LABEL_LONG_EXPAND, &styleLabel2);
	// lv_label_set_text(labelPass, "Contraseña:"); /*Set the text*/
	// lv_obj_align(labelPass, NULL, LV_ALIGN_IN_TOP_LEFT, 10, 50);

	ta_PASS = lv_ta_create(contConf2, NULL);
	lv_ta_set_text(ta_PASS, "Contrasena de Mantenimiento");
	lv_ta_set_pwd_mode(ta_PASS, false);
	lv_ta_set_one_line(ta_PASS, false);
	lv_obj_set_size(ta_PASS, 300, 30);
	lv_obj_align(ta_PASS, NULL, LV_ALIGN_IN_TOP_LEFT, 10, 50);
	//lv_obj_set_event_cb(ta_PASS, ta_event_cb);

	lv_obj_t *labeltiempoEsp = lv_label_create(contConf2, NULL); /*First parameters (scr) is the parent*/
	lv_label_set_style(labeltiempoEsp, LV_LABEL_LONG_EXPAND, &styleLabel2);
	lv_label_set_text(labeltiempoEsp, "Tiempo Espera Carga:"); /*Set the text*/
	lv_obj_align(labeltiempoEsp, NULL, LV_ALIGN_IN_TOP_LEFT, 10, 100);

	/*Create a drop down list*/
	ddlistTiempo = lv_ddlist_create(contConf2, NULL);
	lv_ddlist_set_options(ddlistTiempo, "1 minuto\n"
										"2 minutos\n"
										"5 minutos\n"
										"10 minutos");

	lv_ddlist_set_fix_width(ddlistTiempo, 150);
	lv_ddlist_set_draw_arrow(ddlistTiempo, true);
	lv_obj_align(ddlistTiempo, NULL, LV_ALIGN_IN_TOP_LEFT, 10, 130);
	lv_obj_set_event_cb(ddlistTiempo, dlist_event_handler);

	lv_obj_t *labelTrifasica = lv_label_create(contConf2, NULL); /*First parameters (scr) is the parent*/
	lv_label_set_style(labelTrifasica, LV_LABEL_LONG_EXPAND, &styleLabel2);
	lv_label_set_text(labelTrifasica, "Tipo de Alimentacion:"); /*Set the text*/
	lv_obj_align(labelTrifasica, NULL, LV_ALIGN_IN_TOP_LEFT, 10, 180);

	/*Create a drop down list*/
	ddlistTrifasica = lv_ddlist_create(contConf2, NULL);
	lv_ddlist_set_options(ddlistTrifasica, "Monofasico\n"
										   "Trifasico");

	lv_ddlist_set_fix_width(ddlistTrifasica, 150);
	lv_ddlist_set_draw_arrow(ddlistTrifasica, true);
	lv_obj_align(ddlistTrifasica, NULL, LV_ALIGN_IN_TOP_LEFT, 10, 210);
	lv_obj_set_event_cb(ddlistTrifasica, dlist_event_handler);

	cb_fase1 = lv_cb_create(contConf2, NULL);
	lv_cb_set_text(cb_fase1, "Fase 1");
	lv_obj_align(cb_fase1, NULL, LV_ALIGN_IN_TOP_LEFT, 10, 250);
	cb_fase2 = lv_cb_create(contConf2, NULL);
	lv_cb_set_text(cb_fase2, "Fase 2");
	lv_obj_align(cb_fase2, NULL, LV_ALIGN_IN_TOP_LEFT, 100, 250);
	cb_fase3 = lv_cb_create(contConf2, NULL);
	lv_cb_set_text(cb_fase3, "Fase 3");
	lv_obj_align(cb_fase3, NULL, LV_ALIGN_IN_TOP_LEFT, 200, 250);
	lv_cb_set_inactive(cb_fase1);
	lv_cb_set_inactive(cb_fase2);
	lv_cb_set_inactive(cb_fase3);

	//-------------------------------------------------------------------------------------//

	btnCancelConfig = lv_btn_create(cont_screen_config, NULL);
	lv_obj_set_event_cb(btnCancelConfig, btn_event_cb);
	lv_obj_set_size(btnCancelConfig, 200, 50);
	lv_obj_align(btnCancelConfig, NULL, LV_ALIGN_IN_BOTTOM_MID, 150, -10);
	lv_obj_t *labelbtn = lv_label_create(btnCancelConfig, NULL);
	lv_label_set_text(labelbtn, "CANCELAR");

	btnContinuarConfig = lv_btn_create(cont_screen_config, NULL);
	lv_obj_set_event_cb(btnContinuarConfig, btn_event_cb);
	lv_obj_set_size(btnContinuarConfig, 200, 50);
	lv_obj_align(btnContinuarConfig, NULL, LV_ALIGN_IN_BOTTOM_MID, -150, -10);
	labelbtn = lv_label_create(btnContinuarConfig, NULL);
	lv_label_set_text(labelbtn, "CONTINUAR");

	ScreenConfig = true;
}

void update_label_configuration(char *serial, float corriente, uint16_t error, uint16_t status, bool fase1, bool fase2, bool fase3)
{
	char res[70];
	char dest[70];

	char delimitador[] = " ";
	char *token = strtok(serial, delimitador);
	if (token != NULL)
	{
		//printf("Encontramos un token: %s", token);
	}
	strcpy(res, token);
	strcpy(dest, "-Serial Phoenix: ");
	strcat(dest, res);
	lv_label_set_text(labPhoenixVersion, dest); /*Set the text*/
	memset(res, 0, sizeof(res));

	ftoa(corriente, res, 2);
	strcpy(dest, "-Current Max: ");
	strcat(dest, res);
	lv_label_set_text(labPhoenixCurrent, dest); /*Set the text*/
	memset(res, 0, sizeof(res));

	if (status == 0x4131)
	{
		strcpy(res, "\nA- No hay pistola");
	}
	else if (status == 0x4232 || status == 0x4231)
	{
		strcpy(res, "\nB- Pistola conectada");
	}
	strcpy(dest, "-Status IEC 6185-1: ");
	strcat(dest, res);
	lv_label_set_text(labPhoenixStatus, dest); /*Set the text*/
	memset(dest, 0, sizeof(dest));

	strcpy(res, phoenixError(error));
	strcpy(dest, "-Error Status: \n");
	strcat(dest, res);
	lv_label_set_text(labPhoenixError, dest); /*Set the text*/
	memset(dest, 0, sizeof(dest));

	lv_cb_set_checked(cb_fase1, fase1);
	lv_cb_set_checked(cb_fase2, fase2);
	lv_cb_set_checked(cb_fase3, fase3);
}

void external_ticket()
{
	printf("External ticket MQTT %s\n", str2);
	//str2 = ticketMqtt;
	screen_init_carga();
	lv_obj_del(cont_screen_code);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
int i = 0;

static void btn_event_cb(lv_obj_t *obj, lv_event_t event)
{
	if (event == LV_EVENT_CLICKED)
	{
		//printf("Clicked\n");

		if (obj == cont_screen_welcome && !welcome)
		{
			lv_obj_del(cont_screen_welcome);
			printf("Clicked Screen Welcome\n");
			welcome = true;
			screen_code();
		}

		else if (obj == btnCancel2)
		{
			xSemaphoreGive(Semaphore_Stop_Charging);
			EnableCharger = false;
		}
		else if (obj == btnCancelConfig)
		{
			ScreenConfig = false;
			screen_welcome();
			lv_obj_del(cont_screen_config);
		}

		else if (obj == btnResetPhoe)
		{
			xSemaphoreGive(Semaphore_Reset_Phoenix);
		}
	}
	else if (event == LV_EVENT_VALUE_CHANGED)
	{
		printf("Toggled\n");
	}
}

static void dlist_event_handler(lv_obj_t *obj, lv_event_t event)
{
	if (event == LV_EVENT_VALUE_CHANGED)
	{
		char buf[32];
		lv_ddlist_get_selected_str(obj, buf, sizeof(buf));

		if (obj == ddlistTiempo)
		{
			printf("Option Tiempo: %s\n", buf);

			if (strcmp("1 minuto", buf) == 0)
			{
				timeWaitCharger = 1000000;
				printf("Configure 1 minute\n");
			}
			else if (strcmp("2 minutos", buf) == 0)
			{
				timeWaitCharger = 2000000;
				printf("Configure 2 minute\n");
			}
			else if (strcmp("5 minutos", buf) == 0)
			{
				timeWaitCharger = 5000000;
				printf("Configure 5 minute\n");
			}
			else if (strcmp("10 minutos", buf) == 0)
			{
				timeWaitCharger = 10000000;
				printf("Configure 10 minute\n");
			}
		}
		else if (obj == ddlistTrifasica)
		{
			printf("Option Trifasica: %s\n", buf);
			if (strcmp("Monofasico", buf) == 0)
			{
				printf("Configure MONOFASICO\n");
			}
			else if (strcmp("Trifasico", buf) == 0)
			{
				printf("Configure TRIFASICO\n");
			}
		}
	}
}

static void kb_event_cb(lv_obj_t *event_kb, lv_event_t event)
{
	/* Just call the regular event handler */
	lv_kb_def_event_cb(event_kb, event);

	if (event == LV_EVENT_CANCEL)
	{
		printf("CANCEL TECLADO");
		lv_obj_del(cont_screen_code);
		screen_welcome();
	}
	if (event == LV_EVENT_APPLY)
	{
		printf("OK TECLADO");
		printf("ticket %s\n", str2);

		if (strcmp("0000", str2) == 0)
		{

			screen_configuration();
			lv_obj_del(cont_screen_code);
		}
		else
		{
			screen_init_carga();
			lv_obj_del(cont_screen_code);
		}
		i = 0;
	}
}

static void ta_event_cb(lv_obj_t *ta, lv_event_t event)
{
	if (event == LV_EVENT_CLICKED)
	{
		/* Focus on the clicked text area */
		if (kb != NULL)
			lv_kb_set_ta(kb, ta);
	}

	else if (event == LV_EVENT_INSERT)
	{
		const char *str = lv_event_get_data();
		printf("Tecla %s - %d\n", str, i);

		str2[i] = str[0];
		//i++;

		if (str[0] == 127)
		{
			if (i > 0)
			{
				str2[i] = 0x00;
				i--;
				str2[i] = 0x00;
				printf("Tecla %s - %d\n", str, i);
				printf("ticket %s\n", str2);
			}
			else
				i = 0;
			//str2[i] = 0x0A;
		}
		else
		{
			i++;
		}
	}
}

void ChargeControlOne()
{
	if (!EnableCharger && StopCharging)
	{
		EnableCharger = true;
		screen_welcome();
		lv_obj_del(cont_screen_end);
	}
	else
	{
		bool response = compare_ticket(str2);
		printf("Response : %d\n", (int)response);

		if (response)
		{
			lv_obj_del(cont_screen_init);
			printf("Clicked Continue chargerOne\n");
			screen_carga_one();
			xSemaphoreGive(Semaphore_Start_Charging);
		}
		else
		{
			EnableCharger = false;
			lv_obj_del(cont_screen_init);
			screen_code();
			warning = lv_img_create(cont_screen_code, NULL);
			lv_img_set_src(warning, &img_warning);
			lv_obj_set_size(warning, 50, 45);
			lv_obj_align(warning, NULL, LV_ALIGN_IN_TOP_MID, 0, 30);

			lv_obj_align(label_codeStatus, NULL, LV_ALIGN_IN_TOP_MID, -65, 75);
			lv_label_set_text(label_codeStatus, "Codigo erroneo");

			lv_obj_align(label_code, NULL, LV_ALIGN_IN_TOP_MID, -65, 130);
			lv_label_set_text(label_code, "Por favor, introduzca el codigo correcto");
		}
		memset(str2, 0, sizeof(str2));
	}
}

#endif /*LV_USE_DEMO*/
