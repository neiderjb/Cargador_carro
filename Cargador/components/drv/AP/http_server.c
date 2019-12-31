#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "nvs_flash.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "mdns.h"
#include "lwip/api.h"
#include "lwip/err.h"
#include "lwip/netdb.h"
#include "lwip/opt.h"
#include "lwip/memp.h"
#include "lwip/ip.h"
#include "lwip/raw.h"
#include "lwip/udp.h"
#include "lwip/priv/api_msg.h"
#include "lwip/priv/tcp_priv.h"
#include "lwip/priv/tcpip_priv.h"
#include "esp_system.h"
#include "esp_spi_flash.h"

#include "http_server.h"
#include "wifi_manager.h"
#include "spiffs_lib.h"

EventGroupHandle_t http_server_event_group = NULL;
EventBits_t uxBits;

static const char TAG[] = "http_server";

/* embedded binary data */
extern const uint8_t index_html_start[] asm("_binary_index_html_start");
extern const uint8_t index_html_end[] asm("_binary_index_html_end");

const static char string_default [] = "1,DinoxInd,DinoAgrum,0,192,168,0,1,255,255,255,0,192,168,0,2,8,8,8,8";
const static char data_string_default [] = "Data Saved Correctly";

/* const http headers stored in ROM */
const static char http_html_hdr[] = "HTTP/1.1 200 OK\nContent-type: text/html\n\n";
/*const static char http_css_hdr[] = "HTTP/1.1 200 OK\nContent-type: text/css\nCache-Control: public, max-age=31536000\n\n";
const static char http_js_hdr[] = "HTTP/1.1 200 OK\nContent-type: text/javascript\n\n";
const static char http_config_json_hdr[] = "HTTP/1.1 200 OK\nContent-type: application/json\n\n";
const static char http_jquery_gz_hdr[] = "HTTP/1.1 200 OK\nContent-type: text/javascript\nAccept-Ranges: bytes\nContent-Length: 29995\nContent-Encoding: gzip\n\n";
*/const static char http_200_hdr[] = "HTTP/1.1 200 OK\nContent-type: text/plain\n\n";
const static char http_400_hdr[] = "HTTP/1.1 400 Bad Request\nContent-Length: 0\n\n";
const static char http_404_hdr[] = "HTTP/1.1 404 Not Found\nContent-Length: 0\n\n";
/*const static char http_503_hdr[] = "HTTP/1.1 503 Service Unavailable\nContent-Length: 0\n\n";
const static char http_ok_json_no_cache_hdr[] = "HTTP/1.1 200 OK\nContent-type: application/json\nCache-Control: no-store, no-cache, must-revalidate, max-age=0\nPragma: no-cache\n\n";
*/const static char http_redirect_hdr_start[] = "HTTP/1.1 302 Found\nLocation: http://";
const static char http_redirect_hdr_end[] = "/\n\n";

void http_server_set_event_start()
{
	if (!http_server_event_group)
		http_server_event_group = xEventGroupCreate();
	xEventGroupSetBits(http_server_event_group, HTTP_SERVER_START_BIT_0);
}

void http_server(void *pvParameters)
{

	if (!http_server_event_group)
		http_server_event_group = xEventGroupCreate();

	/* do not start the task until wifi_manager says it's safe to do so! */
	ESP_LOGD(TAG, "waiting for start bit");
	uxBits = xEventGroupWaitBits(http_server_event_group, HTTP_SERVER_START_BIT_0, pdFALSE, pdTRUE, portMAX_DELAY);
	ESP_LOGD(TAG, "received start bit, starting server");

	struct netconn *conn, *newconn;
	err_t err;
	conn = netconn_new(NETCONN_TCP);
	netconn_bind(conn, IP_ADDR_ANY, 80);
	netconn_listen(conn);
	ESP_LOGI(TAG, "HTTP Server listening on 80/tcp");
	do
	{
		err = netconn_accept(conn, &newconn);
		if (err == ERR_OK)
		{
			http_server_netconn_serve(newconn);
			netconn_delete(newconn);
		}
		vTaskDelay((TickType_t)10); /* allows the freeRTOS scheduler to take over if needed */
	} while (err == ERR_OK);
	netconn_close(conn);
	netconn_delete(conn);
}

char *http_server_get_header(char *request, char *header_name, int *len)
{
	*len = 0;
	char *ret = NULL;
	char *ptr = NULL;

	ptr = strstr(request, header_name);
	if (ptr)
	{
		ret = ptr + strlen(header_name);
		ptr = ret;
		while (*ptr != '\0' && *ptr != '\n' && *ptr != '\r')
		{
			(*len)++;
			ptr++;
		}
		return ret;
	}
	return NULL;
}

void http_server_netconn_serve(struct netconn *conn)
{

	struct netbuf *inbuf;
	char *buf = NULL;
	u16_t buflen;
	err_t err;
	const char new_line[2] = "\n";

	err = netconn_recv(conn, &inbuf);
	if (err == ERR_OK)
	{

		netbuf_data(inbuf, (void **)&buf, &buflen);

		/* extract the first line of the request */
		char *save_ptr = buf;
		char *line = strtok_r(save_ptr, new_line, &save_ptr);

		if (line)
		{

			/* captive portal functionality: redirect to the access point IP addresss */
			int lenH = 0;
			char *host = http_server_get_header(save_ptr, "Host: ", &lenH);
			if ((sizeof(host) > 0) && !strstr(host, DEFAULT_AP_IP))
			{
				netconn_write(conn, http_redirect_hdr_start, sizeof(http_redirect_hdr_start) - 1, NETCONN_NOCOPY);
				netconn_write(conn, DEFAULT_AP_IP, sizeof(DEFAULT_AP_IP) - 1, NETCONN_NOCOPY);
				netconn_write(conn, http_redirect_hdr_end, sizeof(http_redirect_hdr_end) - 1, NETCONN_NOCOPY);
			}
			/* default page */
			else if (strstr(line, "GET / "))
			{
				netconn_write(conn, http_html_hdr, sizeof(http_html_hdr) - 1, NETCONN_NOCOPY);
				netconn_write(conn, index_html_start, index_html_end - index_html_start, NETCONN_NOCOPY);
			}
			else if (strstr(line, "POST /write "))
			{
				printf("%s",save_ptr);
				char *last = ""; //last part of the request
				for (int i = 0; i < 13; i++)
				{
					last = strtok_r(save_ptr, new_line, &save_ptr);
				}

				ESP_LOGI(TAG, "received data: %s", last);
				save_data("/spiffs/config.json",last);
				netconn_write(conn, http_200_hdr, sizeof(http_200_hdr) - 1, NETCONN_NOCOPY); //Http response 200
				netconn_write(conn, data_string_default, sizeof(data_string_default) - 1, NETCONN_NOCOPY);
				fflush(stdout);
				esp_restart();
			}
			else if (strstr(line, "POST /read "))
			{
				netconn_write(conn, http_200_hdr, sizeof(http_200_hdr) - 1, NETCONN_NOCOPY); //Http response 200
				netconn_write(conn, string_default, sizeof(string_default) - 1, NETCONN_NOCOPY);
			}
			else if (strstr(line, "POST /upload "))
			{
				netconn_write(conn, http_200_hdr, sizeof(http_200_hdr) - 1, NETCONN_NOCOPY); //Http response 200
				//OTA
				
			}
			else
			{
				netconn_write(conn, http_400_hdr, sizeof(http_400_hdr) - 1, NETCONN_NOCOPY);
			}
		}
		else
		{
			netconn_write(conn, http_404_hdr, sizeof(http_404_hdr) - 1, NETCONN_NOCOPY);
		}
	}

	/* free the buffer */
	netbuf_delete(inbuf);
}
