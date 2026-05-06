#include "board.h"
#include "hpm_sdmmc_sd.h"
#include "ff.h"
#include "diskio.h"
#define APP_LOG_TAG "SD_FATFS"
#include "log.h"

FRESULT fatfs_result;
const TCHAR driver_num_buf[4] = { DEV_SD + '0', ':', '/', '\0' };
FATFS s_sd_disk;
FIL s_file;
BYTE work[FF_MAX_SS];

static FRESULT sd_mount_fs(void);
static FRESULT sd_mkfs(void);
static FRESULT sd_mkfile(const TCHAR *filename);
const char *show_error_string(FRESULT fresult);

void sd_fatfs_init(void)
{
    bool need_init_filesystem = true;
    DSTATUS dstatus = disk_status(DEV_SD);
    if (dstatus == STA_NODISK) {
        LOG_INFO("No disk in the SD slot, please insert an SD card...\n");
        do {
            dstatus = disk_status(DEV_SD);
        } while (dstatus == STA_NODISK);
        board_delay_ms(100);
        LOG_INFO("Detected SD card, re-initialize the filesystem...\n");
        need_init_filesystem = true;
    }
    dstatus = disk_initialize(DEV_SD);
    if (dstatus != RES_OK) {
        LOG_ERROR("Failed to initialize SD disk\n");
    }
    if (need_init_filesystem) {
        fatfs_result = sd_mount_fs();
        if (fatfs_result == FR_NO_FILESYSTEM) {
            LOG_INFO("There is no File system available, making file system...\n");
            fatfs_result = sd_mkfs();
            if (fatfs_result != FR_OK) {
                LOG_ERROR("Failed to make filesystem, cause:%s\n", show_error_string(fatfs_result));
                return;
            } else {
                need_init_filesystem = false;
            }
        }
    }

    sd_mkfile("data.txt");
}

static FRESULT sd_mount_fs(void)
{
    FRESULT fresult = f_mount(&s_sd_disk, driver_num_buf, 1);
    if (fresult == FR_OK) {
        LOG_INFO("SD card has been mounted successfully\n");
    } else {
        LOG_ERROR("Failed to mount SD card, cause: %s\n", show_error_string(fresult));
    }

    fresult = f_chdrive(driver_num_buf);
    return fresult;
}

static FRESULT sd_mkfs(void)
{
    LOG_INFO("Formatting the SD card, depending on the SD card capacity, the formatting process may take a long time\n");
    FRESULT fresult = f_mkfs(driver_num_buf, NULL, work, sizeof(work));
    if (fresult != FR_OK) {
        LOG_ERROR("Making File system failed, cause: %s\n", show_error_string(fresult));
    } else {
        LOG_INFO("Making file system is successful\n");
    }

    return fresult;
}

static FRESULT sd_mkfile(const TCHAR *filename)
{
    FRESULT fresult = f_open(&s_file, filename, FA_CREATE_NEW | FA_WRITE);
    if (fresult != FR_OK) {
        LOG_ERROR("Failed to create file, cause: %s\n", show_error_string(fresult));
    } else {
        LOG_INFO("File has been created successfully\n");
    }
    f_close(&s_file);
    return fresult;
}

const char *show_error_string(FRESULT fresult)
{
    const char *result_str;

    switch (fresult) {
    case FR_OK:
        result_str = "succeeded";
        break;
    case FR_DISK_ERR:
        result_str = "A hard error occurred in the low level disk I/O level";
        break;
    case FR_INT_ERR:
        result_str = "Assertion failed";
        break;
    case FR_NOT_READY:
        result_str = "The physical drive cannot work";
        break;
    case FR_NO_FILE:
        result_str = "Could not find the file";
        break;
    case FR_NO_PATH:
        result_str = "Could not find the path";
        break;
    case FR_INVALID_NAME:
        result_str = "Tha path name format is invalid";
        break;
    case FR_DENIED:
        result_str = "Access denied due to prohibited access or directory full";
        break;
    case FR_EXIST:
        result_str = "Access denied due to prohibited access";
        break;
    case FR_INVALID_OBJECT:
        result_str = "The file/directory object is invalid";
        break;
    case FR_WRITE_PROTECTED:
        result_str = "The physical drive is write protected";
        break;
    case FR_INVALID_DRIVE:
        result_str = "The logical driver number is invalid";
        break;
    case FR_NOT_ENABLED:
        result_str = "The volume has no work area";
        break;
    case FR_NO_FILESYSTEM:
        result_str = "There is no valid FAT volume";
        break;
    case FR_MKFS_ABORTED:
        result_str = "THe f_mkfs() aborted due to any problem";
        break;
    case FR_TIMEOUT:
        result_str = "Could not get a grant to access the volume within defined period";
        break;
    case FR_LOCKED:
        result_str = "The operation is rejected according to the file sharing policy";
        break;
    case FR_NOT_ENOUGH_CORE:
        result_str = "LFN working buffer could not be allocated";
        break;
    case FR_TOO_MANY_OPEN_FILES:
        result_str = "Number of open files > FF_FS_LOCK";
        break;
    case FR_INVALID_PARAMETER:
        result_str = "Given parameter is invalid";
        break;
    default:
        result_str = "Unknown error";
        break;
    }
    return result_str;
}