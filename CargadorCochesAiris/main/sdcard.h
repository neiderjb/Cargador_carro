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


void begin_sdcard();

void write_sd(char *filename, char *datawrite);
void delete_sd(char *filename);
void rename_sd(char *filename, char *newfilename );
void openFile_sd(char *filename);
void mount_sd();
void unmount_sd();