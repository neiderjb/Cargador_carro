#include <stdio.h>
#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include "esp_err.h"
#include "esp_log.h"
#include "esp_vfs_fat.h"
#include "driver/sdmmc_host.h"
#include "driver/sdspi_host.h"
#include "sdmmc_cmd.h"

static const char *TAG = "SDCard";

#include "sdcard.h"

// Pin mapping when using SPI mode.
// With this mapping, SD card can be used both in SPI and 1-line SD mode.
// Note that a pull-up on CS line is required in SD mode.
#define PIN_NUM_MISO 24 //24
#define PIN_NUM_MOSI 23 //23
#define PIN_NUM_CLK 13  //13
#define PIN_NUM_CS 16   //16

sdmmc_card_t *card;
sdmmc_host_t host = SDMMC_HOST_DEFAULT();
sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();
FILE *f;

void begin_sdcard()
{

    ESP_LOGI(TAG, "Using SDMMC peripheral");
    //host = SDMMC_HOST_DEFAULT();
    //slot_config = SDMMC_SLOT_CONFIG_DEFAULT();

    gpio_set_pull_mode(PIN_NUM_MISO, GPIO_PULLUP_ONLY); // CMD, needed in 4- and 1- line modes
    gpio_set_pull_mode(PIN_NUM_MOSI, GPIO_PULLUP_ONLY); // D0, needed in 4- and 1-line modes
    gpio_set_pull_mode(PIN_NUM_CLK, GPIO_PULLUP_ONLY);  // D1, needed in 4-line mode only
    gpio_set_pull_mode(PIN_NUM_CS, GPIO_PULLUP_ONLY);   // D2, needed in 4-line mode only

    mount_sd();
    // Card has been initialized, print its properties
    sdmmc_card_print_info(stdout, card);
}

//"/sdcard/hello.txt"
//"Hello %s!\n"
void write_sd(char *filename, char *datawrite)
{
    char rute[] = "/sdcard/";
    strcat(rute, filename);

    char format[] = " %s!\n";
    strcat(datawrite, format);

    // Use POSIX and C standard library functions to work with files.
    // First create a file.
    ESP_LOGI(TAG, "Opening new file : |%s|",rute);
    f = fopen(rute, "w");
    if (f == NULL)
    {
        ESP_LOGE(TAG, "Failed to open file for writing");
        return;
    }
    ESP_LOGI(TAG, "DataWrite file : |%s|", datawrite);
    fprintf(f, datawrite, card->cid.name);
    fclose(f);
    ESP_LOGI(TAG, "File written");
}

void delete_sd(char *filename)
{
    char rute[] = "/sdcard/";
    strcat(rute, filename);

    // Check if destination file exists before renaming
    struct stat st;
    if (stat(rute, &st) == 0)
    {
        // Delete it if it exists
        unlink(rute);
    }
}

void rename_sd(char *filename, char *newfilename )
{
    char rute[] = "/sdcard/";
    strcat(rute, filename);

    char rutenew[] = "/sdcard/";
    strcat(rutenew, newfilename);

    // Rename original file
    ESP_LOGI(TAG, "Renaming file");
    if (rename(rute, rutenew) != 0)
    {
        ESP_LOGE(TAG, "Rename failed");
        return;
    }
}

void openFile_sd(char *filename)
{
    char rute[] = "/sdcard/";
    strcat(rute, filename);

    // Open renamed file for reading
    ESP_LOGI(TAG, "Reading file");
    f = fopen(rute, "r");
    if (f == NULL)
    {
        ESP_LOGE(TAG, "Failed to open file for reading");
        return;
    }
    char line[64];
    fgets(line, sizeof(line), f);
    fclose(f);
    // strip newline
    char *pos = strchr(line, '\n');
    if (pos)
    {
        *pos = '\0';
    }
    ESP_LOGI(TAG, "Read from file: '%s'", line);
}

void mount_sd()
{
    // Options for mounting the filesystem.
    // If format_if_mount_failed is set to true, SD card will be partitioned and
    // formatted in case when mounting fails.
    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = false,
        .max_files = 5,
        .allocation_unit_size = 16 * 1024};

    // Use settings defined above to initialize SD card and mount FAT filesystem.
    // Note: esp_vfs_fat_sdmmc_mount is an all-in-one convenience function.
    // Please check its source code and implement error recovery when developing
    // production applications.
    //sdmmc_card_t *card;
    esp_err_t ret = esp_vfs_fat_sdmmc_mount("/sdcard", &host, &slot_config, &mount_config, &card);
    if (ret != ESP_OK)
    {
        if (ret == ESP_FAIL)
        {
            ESP_LOGE(TAG, "Failed to mount filesystem. "
                          "If you want the card to be formatted, set format_if_mount_failed = true.");
        }
        else
        {
            ESP_LOGE(TAG, "Failed to initialize the card (%s). "
                          "Make sure SD card lines have pull-up resistors in place.",
                     esp_err_to_name(ret));
        }
        return;
    }
}

void unmount_sd()
{
    // All done, unmount partition and disable SDMMC or SPI peripheral
    esp_vfs_fat_sdmmc_unmount();
    ESP_LOGI(TAG, "Card unmounted");
}
