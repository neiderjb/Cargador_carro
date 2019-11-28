/**
 * @file demo.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "cargador.h"

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
static void mbox_event_cb(lv_obj_t* obj, lv_event_t evt);
static void btn_event_cb(lv_obj_t* obj, lv_event_t event);
static void kb_event_cb(lv_obj_t* event_kb, lv_event_t event);
static void ta_event_cb(lv_obj_t* ta, lv_event_t event);


lv_obj_t* scr1;
lv_obj_t* win;

lv_obj_t* cont_screen_welcome;
lv_obj_t* cont_screen_code;
lv_obj_t* cont_screen_CharOne;
lv_obj_t* cont_screen_CharTwo;
lv_obj_t* cont_screen_alert_info;
lv_obj_t* cont_screen_alert_info_wait;
lv_obj_t* cont_screen_alert_info_outService;

lv_obj_t* btn_info;
lv_obj_t* btn_config;
lv_obj_t* btn_airis;
lv_obj_t* btn_close_alert;


lv_coord_t hres;
lv_coord_t vres;

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_obj_t * mbox, * info;
static lv_obj_t * chart;
static lv_obj_t * ta;	//Text Area
static lv_obj_t * kb;	//Teclado


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

	/*Create a window*/
	win = lv_win_create(lv_scr_act(), NULL);
	lv_win_set_title(win, "AIRIS EV MOBILITY");                        /*Set the title*/
	lv_obj_set_opa_scale_enable(win, true);

	/*Add control button to the header*/
	btn_info = lv_win_add_btn(win, LV_SYMBOL_LIST);           /*Add close button and use built-in close action*/
	lv_obj_set_event_cb(btn_info, btn_event_cb);                 /*Assign a callback to the button*/
	btn_config = lv_win_add_btn(win, LV_SYMBOL_SETTINGS);        /*Add a setup button*/
	lv_obj_set_event_cb(btn_config, btn_event_cb);                 /*Assign a callback to the button*/

	//lv_obj_t* logo = lv_img_create(win, NULL);
	//lv_img_set_src(logo, &img_airis_logo);
	//lv_obj_align(logo, NULL, LV_ALIGN_CENTER, 0, 0);
	//btn_airis = lv_win_add_btn(win, logo);

	screen_welcome();

}


/**
 * Create 1 screen application
 */
void screen_welcome(void) {

	cont_screen_welcome = lv_cont_create(win, NULL);
	lv_obj_set_auto_realign(cont_screen_welcome, true);
	lv_obj_align_origo(cont_screen_welcome, NULL, LV_ALIGN_CENTER, 0, 0);  /*This parametrs will be sued when realigned*/
	lv_cont_set_fit(cont_screen_welcome, LV_FIT_FLOOD);
	lv_cont_set_layout(cont_screen_welcome, LV_LAYOUT_OFF);
	lv_obj_set_event_cb(cont_screen_welcome, btn_event_cb);

	lv_obj_t* label = lv_label_create(cont_screen_welcome, NULL); /*First parameters (scr) is the parent*/
	lv_label_set_text(label, "Bienvenido");  /*Set the text*/
	lv_obj_align(label, NULL, LV_ALIGN_IN_TOP_MID, 0, 20);

	lv_obj_t* logo = lv_img_create(cont_screen_welcome, NULL);
	lv_img_set_src(logo, &img_airis_logo);
	lv_obj_align(logo, NULL, LV_ALIGN_CENTER, 0, -20);

	// lv_obj_t* touch = lv_img_create(cont_screen_welcome, NULL);
	// lv_img_set_src(touch, &img_touch);
	// lv_obj_align(touch, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, -50);

	lv_obj_t* label2 = lv_label_create(cont_screen_welcome, NULL); /*First parameters (scr) is the parent*/
	lv_label_set_text(label2, "Toque la pantalla para empezar");  /*Set the text*/
	lv_obj_align(label2, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, -20);

}


void screen_code() {

	cont_screen_code = lv_cont_create(win, NULL);
	lv_obj_set_auto_realign(cont_screen_code, true);
	lv_obj_align_origo(cont_screen_code, NULL, LV_ALIGN_CENTER, 0, 0);  /*This parametrs will be sued when realigned*/
	lv_cont_set_fit(cont_screen_code, LV_FIT_FLOOD);
	lv_cont_set_layout(cont_screen_code, LV_LAYOUT_OFF);
	lv_obj_set_event_cb(cont_screen_code, btn_event_cb);

	lv_obj_t* label = lv_label_create(cont_screen_code, NULL); /*First parameters (scr) is the parent*/
	lv_label_set_text(label, "AIRIS- INGRESE CODIGO");  /*Set the text*/
	lv_obj_align(label, NULL, LV_ALIGN_IN_TOP_MID, 0, 20);


	lv_obj_t* label_code = lv_label_create(cont_screen_code, NULL);
	lv_label_set_text(label_code, "Ingrese su codigo aqui:");
	lv_obj_set_pos(label_code, 300, 60);

	lv_obj_t* ta_code = lv_ta_create(cont_screen_code, NULL);
	lv_ta_set_text(ta_code, "");
	lv_ta_set_pwd_mode(ta_code, true);
	lv_ta_set_one_line(ta_code, true);
	lv_obj_set_size(ta_code, hres - 400, 50);
	lv_obj_set_pos(ta_code, 200, 80);
	lv_obj_set_event_cb(ta_code, ta_event_cb);

	/* Create a keyboard and make it fill the width of the above text areas */
	kb = lv_kb_create(cont_screen_code, NULL);
	lv_obj_set_pos(kb, 5, 165);
	lv_obj_set_event_cb(kb, kb_event_cb); /* Setting a custom event handler stops the keyboard from closing automatically */
	lv_obj_set_size(kb, hres - 40, 220);

	lv_kb_set_ta(kb, ta_code); /* Focus it on one of the text areas to start */
	lv_kb_set_cursor_manage(kb, true); /* Automatically show/hide cursors on text areas */
	
}

void screen_carga_one() {

	cont_screen_CharOne = lv_cont_create(win, NULL);
	lv_obj_set_auto_realign(cont_screen_CharOne, true);
	lv_obj_align_origo(cont_screen_CharOne, NULL, LV_ALIGN_CENTER, 0, 0);  /*This parametrs will be sued when realigned*/
	lv_cont_set_fit(cont_screen_CharOne, LV_FIT_FLOOD);
	lv_cont_set_layout(cont_screen_CharOne, LV_LAYOUT_OFF);
	lv_obj_set_event_cb(cont_screen_CharOne, btn_event_cb);


	lv_obj_t* tabview = lv_tabview_create(cont_screen_CharOne, NULL);
	lv_obj_set_pos(tabview, 5, 5);
	lv_obj_t* tab1 = lv_tabview_add_tab(tabview, "Informacion Carga1");
	lv_obj_t* tab2 = lv_tabview_add_tab(tabview, "Informacion Carga2");

	
	/*Crealte a header cell style*/
	static lv_style_t style_cell2;
	lv_style_copy(&style_cell2, &lv_style_plain);
	style_cell2.body.border.width = 0;
	style_cell2.body.border.color = LV_COLOR_WHITE;
	style_cell2.body.main_color = LV_COLOR_WHITE;
	style_cell2.body.grad_color = LV_COLOR_WHITE;


	lv_obj_t* table = lv_table_create(tab1, NULL);
	//lv_table_set_style(table, LV_TABLE_STYLE_CELL1, &style_cell1);
	lv_obj_set_size(table, hres - 20, 220);
	lv_table_set_style(table, LV_TABLE_STYLE_CELL2, &style_cell2);
	lv_table_set_style(table, LV_TABLE_STYLE_BG, &lv_style_transp_tight);
	lv_table_set_col_cnt(table, 2);
	lv_table_set_row_cnt(table, 5);
	lv_table_set_col_width(table,0, 200);
	lv_table_set_col_width(table, 1, 200);

	lv_obj_align(table, NULL, LV_ALIGN_IN_TOP_LEFT, 5, 5);

	/*Make the cells of the first row center aligned */
	lv_table_set_cell_align(table, 0, 0, LV_LABEL_ALIGN_CENTER);
	lv_table_set_cell_align(table, 0, 1, LV_LABEL_ALIGN_CENTER);

	/*Make the cells of the first row TYPE = 2 (use `style_cell2`) */
	lv_table_set_cell_type(table, 0, 0, 2);
	lv_table_set_cell_type(table, 0, 1, 2);

	/*Fill the first column*/
	lv_table_set_cell_value(table, 0, 0, "");
	lv_table_set_cell_value(table, 1, 0, "Coste KW/h");
	lv_table_set_cell_value(table, 2, 0, "Amperios Max. Carga");
	lv_table_set_cell_value(table, 3, 0, "Contrase�a");
	lv_table_set_cell_value(table, 4, 0, "Error Placa");

	/*Fill the second column*/
	lv_table_set_cell_value(table, 0, 1, "");
	lv_table_set_cell_value(table, 1, 1, "Price");
	lv_table_set_cell_value(table, 2, 1, "$7");
	lv_table_set_cell_value(table, 3, 1, "$4");
	lv_table_set_cell_value(table, 4, 1, "$6");

}

void screen_carga_two() {
	cont_screen_CharTwo = lv_cont_create(lv_scr_act(), NULL);
	lv_obj_set_auto_realign(cont_screen_CharTwo, true);
	lv_obj_align_origo(cont_screen_CharTwo, NULL, LV_ALIGN_CENTER, 0, 0);  /*This parametrs will be sued when realigned*/
	lv_cont_set_fit(cont_screen_CharTwo, LV_FIT_FLOOD);
	lv_cont_set_layout(cont_screen_CharTwo, LV_LAYOUT_OFF);
	lv_obj_set_event_cb(cont_screen_CharTwo, btn_event_cb);

	lv_obj_t* label = lv_label_create(cont_screen_CharOne, NULL); /*First parameters (scr) is the parent*/
	lv_label_set_text(label, "AIRIS- CARGA2");  /*Set the text*/
	lv_obj_set_pos(label, 300, 5);
	lv_obj_set_x(label, 300);

	

}


void screen_alert_info() {
	lv_obj_set_opa_scale(win, LV_OPA_30);
	cont_screen_alert_info = lv_cont_create(lv_scr_act(), NULL);
	lv_obj_set_size(cont_screen_alert_info, 400, 220);

	lv_obj_set_auto_realign(cont_screen_alert_info, true);
	lv_obj_align_origo(cont_screen_alert_info, NULL, LV_ALIGN_CENTER, 0, 0);  /*This parametrs will be sued when realigned*/
	lv_cont_set_fit(cont_screen_alert_info, LV_FIT_NONE);
	lv_cont_set_layout(cont_screen_alert_info, LV_LAYOUT_CENTER);
	//lv_obj_set_event_cb(cont_screen_alert_info, btn_event_cb);

	lv_obj_t* label = lv_label_create(cont_screen_alert_info, NULL); /*First parameters (scr) is the parent*/
	lv_label_set_text(label, "Instrucciones de Uso");  /*Set the text*/
	lv_obj_set_pos(label, 5, 5);
	lv_obj_set_x(label, 300);
	/*Add some dummy content*/
	lv_obj_t* txt = lv_label_create(cont_screen_alert_info, NULL);
	lv_label_set_text(txt, "1: Ingrese el codigo\n"
		"2:\n"
		"3:\n"
		"4:\n"
		"5:\n"
		"El proceso de carga INICIARA!");

	btn_close_alert = lv_btn_create(cont_screen_alert_info, NULL);     /*Add a button the current screen*/
	lv_obj_set_pos(btn_close_alert, 100, 250);                            /*Set its position*/
	lv_obj_set_size(btn_close_alert, 100, 50);                          /*Set its size*/
	lv_obj_set_event_cb(btn_close_alert, btn_event_cb);                 /*Assign a callback to the button*/
	lv_obj_t* labelClose = lv_label_create(btn_close_alert, NULL);          /*Add a label to the button*/
	lv_label_set_text(labelClose, "Cerrar");

}

void screen_alert_info_error() {
	lv_obj_set_opa_scale(win, LV_OPA_30);
	cont_screen_alert_info = lv_cont_create(lv_scr_act(), NULL);
	lv_obj_set_size(cont_screen_alert_info, 300, 220);

	lv_obj_set_auto_realign(cont_screen_alert_info, true);
	lv_obj_align_origo(cont_screen_alert_info, NULL, LV_ALIGN_CENTER, 0, 0);  /*This parametrs will be sued when realigned*/
	lv_cont_set_fit(cont_screen_alert_info, LV_FIT_NONE);
	lv_cont_set_layout(cont_screen_alert_info, LV_LAYOUT_CENTER);
	//lv_obj_set_event_cb(cont_screen_alert_info, btn_event_cb);

	lv_obj_t* imgError = lv_img_create(cont_screen_alert_info, NULL);
	lv_img_set_auto_size(imgError, true);
	lv_img_set_src(imgError, &img_alert2);
	lv_obj_align(imgError, NULL, LV_ALIGN_CENTER, 0, -20);

	lv_obj_t* label = lv_label_create(cont_screen_alert_info, NULL); /*First parameters (scr) is the parent*/
	lv_label_set_align(label, LV_LABEL_ALIGN_CENTER);
	lv_label_set_text(label, "Codigo Erroneo \n Por favor, introduzca el codigo correcto");  /*Set the text*/
	

	btn_close_alert = lv_btn_create(cont_screen_alert_info, NULL);     /*Add a button the current screen*/
	lv_obj_set_pos(btn_close_alert, 100, 250);                            /*Set its position*/
	lv_obj_set_size(btn_close_alert, 100, 50);                          /*Set its size*/
	lv_obj_set_event_cb(btn_close_alert, btn_event_cb);                 /*Assign a callback to the button*/
	lv_obj_t* labelClose = lv_label_create(btn_close_alert, NULL);          /*Add a label to the button*/
	lv_label_set_text(labelClose, "Aceptar");

}



void screen_alert_info_outService() {
	cont_screen_alert_info_outService = lv_cont_create(win, NULL);
	lv_obj_set_auto_realign(cont_screen_alert_info_outService, true);
	lv_obj_align_origo(cont_screen_alert_info_outService, NULL, LV_ALIGN_CENTER, 0, 0);  /*This parametrs will be sued when realigned*/
	lv_cont_set_fit(cont_screen_alert_info_outService, LV_FIT_FLOOD);
	lv_cont_set_layout(cont_screen_alert_info_outService, LV_LAYOUT_OFF);
	lv_obj_set_event_cb(cont_screen_alert_info_outService, btn_event_cb);

	lv_obj_t* label = lv_label_create(cont_screen_alert_info_outService, NULL); /*First parameters (scr) is the parent*/
	lv_label_set_text(label, "FUERA DE SERVICIO");  /*Set the text*/
	lv_obj_align(label, NULL, LV_ALIGN_IN_TOP_MID, 0, 20);

	lv_obj_t* logo = lv_img_create(cont_screen_alert_info_outService, NULL);
	lv_img_set_src(logo, &img_airis_logo);
	lv_obj_align(logo, NULL, LV_ALIGN_CENTER, 0, -20);

	lv_obj_t* label2 = lv_label_create(cont_screen_alert_info_outService, NULL); /*First parameters (scr) is the parent*/
	lv_label_set_text(label2, "DISCULPE LAS MOLESTIAS");  /*Set the text*/
	lv_obj_align(label2, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, -20);


}

void screen_alert_info_wait() {
	cont_screen_alert_info_wait = lv_cont_create(win, NULL);
	lv_obj_set_auto_realign(cont_screen_alert_info_wait, true);
	lv_obj_align_origo(cont_screen_alert_info_wait, NULL, LV_ALIGN_CENTER, 0, 0);  /*This parametrs will be sued when realigned*/
	lv_cont_set_fit(cont_screen_alert_info_wait, LV_FIT_FLOOD);
	lv_cont_set_layout(cont_screen_alert_info_wait, LV_LAYOUT_OFF);
	lv_obj_set_event_cb(cont_screen_alert_info_wait, btn_event_cb);


}
 

/**********************
 *   STATIC FUNCTIONS
 **********************/


static void btn_event_cb(lv_obj_t* obj, lv_event_t event)
{
	if (event == LV_EVENT_CLICKED) {
		printf("Clicked\n");

		if (obj == cont_screen_welcome) {
			printf("Clicked btn Welcome\n");
			lv_obj_del(cont_screen_welcome);
			screen_code();
			
		}
		else if (obj == btn_info) {
			
			screen_alert_info();
		}
		else if (obj == btn_close_alert) {
			lv_obj_del(cont_screen_alert_info);
			lv_obj_set_opa_scale(win, LV_OPA_100);
		}
		else if (obj == btn_config) {
			//screen_alert_info_error();
			lv_obj_del(cont_screen_welcome);
			screen_alert_info_outService();
		}

	}
	else if (event == LV_EVENT_VALUE_CHANGED) {
		printf("Toggled\n");
	}
}

static void kb_event_cb(lv_obj_t* event_kb, lv_event_t event)
{
	/* Just call the regular event handler */
	lv_kb_def_event_cb(event_kb, event);

}

static void ta_event_cb(lv_obj_t* ta, lv_event_t event)
{
	if (event == LV_EVENT_CLICKED) {
		/* Focus on the clicked text area */
		if (kb != NULL)
			lv_kb_set_ta(kb, ta);
	}

	else if (event == LV_EVENT_INSERT) {
		const char* str = lv_event_get_data();
		if (str[0] == '\n') {
			
			screen_carga_one();
			lv_obj_del(cont_screen_code);
		}
	}


}
#endif  /*LV_USE_DEMO*/