#include "drv/TimerControl.h"
#include "drv/parameters.h"
#include "drv/phoenixcontact.h"
#include "drv/M90E32AS.h"
#include "lv_examples/lv_cargador/cargador/cargador.h"
#include "esp_timer.h"
#include "esp_system.h"
#include "Sim800L.h"
#include <stdio.h>
#include <string.h>

static const char *TAG = "TimerControl";

void timer_control_callback(void *arg)
{
    ChargeControlOne();
}

void Timer_Memory_Control_callback(void *arg)
{
    ESP_LOGW(TAG, "[APP] Free memory: %d bytes", esp_get_free_heap_size());
    //PublishMqtt2G("airis/1155/report", strlen("airis/1155/report"), "keepalive ", strlen("keepalive"));
}

void Timer_charge_Control_callback(void *arg)
{
}

void timer_begin()
{
    ESP_LOGI(TAG, "Timer Control Initialize");

    const esp_timer_create_args_t timer_control_arg = {
        .callback = &timer_control_callback,
        .name = "Handle_Timer_Screen_Control"};
    ESP_ERROR_CHECK(esp_timer_create(&timer_control_arg, &Timer_Screen_Control));

    const esp_timer_create_args_t timer_control_memory_arg = {
        .callback = &Timer_Memory_Control_callback,
        .name = "Handle_Timer_Memory_Control"};
    ESP_ERROR_CHECK(esp_timer_create(&timer_control_memory_arg, &Timer_Memory_Control));

    const esp_timer_create_args_t timer_control_charge_arg = {
        .callback = &Timer_charge_Control_callback,
        .name = "Handle_Timer_charge_Control"};
    ESP_ERROR_CHECK(esp_timer_create(&timer_control_charge_arg, &Timer_Charge_Control));

    ESP_LOGI(TAG, "Timer Control OK");
}