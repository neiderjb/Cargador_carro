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
static void mbox_event_cb(lv_obj_t *obj, lv_event_t evt);
static void btn_event_cb(lv_obj_t *obj, lv_event_t event);
static void kb_event_cb(lv_obj_t *event_kb, lv_event_t event);
static void ta_event_cb(lv_obj_t *ta, lv_event_t event);

//LV_KEY_UP = 17,  /*0x11*/
//LV_KEY_DOWN = 18,  /*0x12*/
//LV_KEY_RIGHT = 19,  /*0x13*/
//LV_KEY_LEFT = 20,  /*0x14*/
//LV_KEY_ESC = 27,  /*0x1B*/
//LV_KEY_DEL = 127, /*0x7F*/
//LV_KEY_BACKSPACE = 8,   /*0x08*/
//LV_KEY_ENTER = 10,  /*0x0A, '\n'*/
//LV_KEY_NEXT = 9,   /*0x09, '\t'*/
//LV_KEY_PREV = 11,  /*0x0B, '*/
//LV_KEY_HOME = 2,   /*0x02, STX*/
//LV_KEY_END = 3,   /*0x03, ETX*/

lv_obj_t *scr1;

char str1[] = {'1', '2', '3', '4', '\n'};
char str2[] = {'0', '0', '0', '0', '\n'};

bool welcome = false;
bool EnableCharger = true;
bool StopCharger = false;

lv_obj_t *cont_screen_welcome;

lv_obj_t *cont_screen_code;
lv_obj_t *warning;
lv_obj_t *label_codeStatus;
lv_obj_t *label_code;
lv_obj_t *ta_code;

lv_obj_t *cont_screen_alert_info_outService;

lv_obj_t *cont_screen_init;
lv_obj_t *labelCon;
lv_obj_t *reloj;
lv_obj_t *btnCancel;
lv_obj_t *btnContinuar;

lv_obj_t *cont_screen_CharOne;
// lv_obj_t* labelPotencia;
// lv_obj_t* labelCarga;
// lv_obj_t* labelCoste;
// lv_obj_t* labelTiempo;
lv_obj_t *labelVehiculo;
lv_obj_t *conecte;
lv_obj_t *conectado;
lv_obj_t *cargando;
lv_obj_t *error;
lv_obj_t *btnCancel2;
lv_obj_t *btnCargar;

lv_obj_t *cont_screen_config;
lv_obj_t *btnContinuarConfig;
lv_obj_t *btnCancelConfig;

lv_obj_t *btn_info;
lv_obj_t *btn_config;
lv_obj_t *btn_airis;
lv_obj_t *btn_close_alert;

lv_coord_t hres;
lv_coord_t vres;

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_obj_t *mbox, *info;
static lv_obj_t *chart;
static lv_obj_t *ta; //Text Area
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

//LV_IMG_DECLARE(img_touch)

/**********************
 *      MACROS
 **********************/

/**********************
  *   GLOBAL FUNCTIONS
  **********************/

static lv_theme_t *th;
static lv_style_t styleLabel1;
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
	screen_welcome();
}

/**
 * Create 1 screen application
 */
void screen_welcome(void)
{

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
	if (!StopCharger)
	{
		lv_label_set_text(label, "   Validando Ticket"); /*Set the text*/
	}
	else
	{
		lv_label_set_text(label, "   Carga terminada cerrando Ticket"); /*Set the text*/
	}
	lv_obj_align(label, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, -120);
	ESP_ERROR_CHECK(esp_timer_start_once(Timer_Screen_Control, 10000000));

	/*Create a Preloader object*/
	lv_obj_t *preload = lv_preload_create(cont_screen_init, NULL);
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
	lv_obj_set_event_cb(btnCancel2, btn_event_cb);
	lv_obj_set_size(btnCancel2, 400, 50);
	lv_obj_align(btnCancel2, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, -20);
	lv_obj_t *labelbtn = lv_label_create(btnCancel2, NULL);
	lv_label_set_text(labelbtn, "CANCELAR");

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
}

void update_error_carga_one()
{

	lv_obj_del(cargando);
	error = lv_img_create(cont_screen_CharOne, NULL);
	lv_img_set_src(conecte, &img_error);
	lv_obj_align(conecte, NULL, LV_ALIGN_CENTER, -270, -20);

	lv_label_set_text(labelVehiculo, " Error\nCargando"); /*Set the text*/
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
    strcat(res ,dest);
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
	ready_information = false;
	screen_welcome();
	StopCharger = false;
	EnableCharger = true;
	welcome = false;
	lv_obj_del(cont_screen_CharOne);
}

void screen_alert_info_outService()
{
	ready_information = false;
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

	lv_obj_t *label2 = lv_label_create(cont_screen_alert_info_outService, NULL); /*First parameters (scr) is the parent*/
	lv_label_set_style(label2, LV_LABEL_LONG_EXPAND, &styleLabel1);
	lv_label_set_text(label2, "DISCULPE LAS MOLESTIAS"); /*Set the text*/
	lv_obj_align(label2, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, -20);
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

	btnCancelConfig = lv_btn_create(cont_screen_config, NULL);
	lv_obj_set_event_cb(btnCancelConfig, btn_event_cb);
	lv_obj_set_size(btnCancelConfig, 200, 50);
	lv_obj_align(btnCancelConfig, NULL, LV_ALIGN_IN_BOTTOM_MID, 150, -20);
	lv_obj_t *labelbtn = lv_label_create(btnCancelConfig, NULL);
	lv_label_set_text(labelbtn, "CANCELAR");

	btnContinuarConfig = lv_btn_create(cont_screen_config, NULL);
	lv_obj_set_event_cb(btnContinuarConfig, btn_event_cb);
	lv_obj_set_size(btnContinuarConfig, 200, 50);
	lv_obj_align(btnContinuarConfig, NULL, LV_ALIGN_IN_BOTTOM_MID, -150, -20);
	labelbtn = lv_label_create(btnContinuarConfig, NULL);
	lv_label_set_text(labelbtn, "CONTINUAR");
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

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

		else if (obj == cont_screen_code)
		{
			printf("Clicked Screen Code\n");
		}

		// else if (obj == btnContinuar && !StopCharger)
		// {

		// 	lv_obj_del(cont_screen_init);
		// 	printf("Clicked Continue chargerOne\n");
		// 	screen_carga_one();
		// 	xSemaphoreGive(Semaphore_Start_Charging);
		// }

		else if (obj == btnContinuar && StopCharger)
		{
			screen_welcome();
			StopCharger = false;
			EnableCharger = true;
			welcome = false;
			lv_obj_del(cont_screen_init);
		}

		// else if (obj == btnCancel)
		// {
		// 	lv_obj_del(cont_screen_init);
		// 	printf("Clicked Cancel Init\n");
		// 	EnableCharger = false;
		// 	screen_code();
		// }
		else if (obj == btnCancel2)
		{
			lv_obj_del(cont_screen_CharOne);
			printf("Clicked Cancel CharOne\n");
			EnableCharger = false;
			screen_code();
		}
	}
	else if (event == LV_EVENT_VALUE_CHANGED)
	{
		printf("Toggled\n");
	}
}

static void kb_event_cb(lv_obj_t *event_kb, lv_event_t event)
{
	/* Just call the regular event handler */
	lv_kb_def_event_cb(event_kb, event);
}

int i = 0;
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
		printf("Tecla %s \n", str);

		str2[i] = str[0];
		i++;

		if (str[0] == 127)
		{
			i = 0;
			memset(str2, 0, sizeof(str2));
		}

		if (str[0] == '\n')
		{
			printf("ticket %s\n", str2);

			if (strcmp("0000\n", str2) == 0)
			{
				screen_configuration();
				lv_obj_del(cont_screen_code);
			}
			else
			{
				if (!EnableCharger)
				{
					StopCharger = true;
				}
				screen_init_carga();
				lv_obj_del(cont_screen_code);
			}
			i = 0;
		}
	}
}

void ChargeControlOne()
{
	bool response = compare_ticket(str2);
	printf("Response : %d\n", (int)response);
	// int tryBtoC = 20;
	// while (tryBtoC != 0) //Realiza 20 Intentos para que el vehiculo cambie de estado B a C
	// {
	// 	tryBtoC--;
	// 	vTaskDelay(200 / portTICK_RATE_MS);
	// }
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

#endif /*LV_USE_DEMO*/
