#include "miniz.h" /* IWYU pragma: keep */
#define APP_LOG_TAG "COMPRESS"
#include "log.h"

#ifndef RELEASE

#define RAW_SIZE 256
#define ZIP_SIZE 512
static uint8_t raw_buf[RAW_SIZE];
static uint8_t zip_buf[ZIP_SIZE];
static uint8_t unzip_buf[RAW_SIZE];

static tdefl_compressor comp __attribute__((section(".fast_ram")));
static tinfl_decompressor decomp __attribute__((section(".fast_ram")));

void miniz_demo(void)
{
    memset(raw_buf, 0, RAW_SIZE);
    memset(zip_buf, 0, ZIP_SIZE);
    memset(unzip_buf, 0, RAW_SIZE);

    /*prepare raw data*/
    for (int i = 0; i < RAW_SIZE; i++) {
        raw_buf[i] = i;
    }

    int flags = TDEFL_WRITE_ZLIB_HEADER | TDEFL_DEFAULT_MAX_PROBES;
    tdefl_status def_status = tdefl_init(&comp, NULL, NULL, flags);
    if (def_status != TDEFL_STATUS_OKAY) {
        LOG_ERROR("tdefl_init failed: %d\n", def_status);
        return;
    }

    size_t deflate_in_size = RAW_SIZE;
    size_t deflate_out_size = ZIP_SIZE;
    def_status = tdefl_compress(&comp, raw_buf, &deflate_in_size, zip_buf, &deflate_out_size, TDEFL_FINISH);
    if (def_status != TDEFL_STATUS_DONE) {
        LOG_ERROR("tdefl_compress failed: %d, in=%u, out=%u\n",
                  def_status, (unsigned int) deflate_in_size, (unsigned int) deflate_out_size);
        return;
    }

    tinfl_init(&decomp);

    size_t inflate_in_size = deflate_out_size;
    size_t inflate_out_size = RAW_SIZE;
    tinfl_status inf_status = tinfl_decompress(&decomp,
                                               zip_buf,
                                               &inflate_in_size,
                                               unzip_buf,
                                               unzip_buf,
                                               &inflate_out_size,
                                               TINFL_FLAG_PARSE_ZLIB_HEADER |
                                               TINFL_FLAG_USING_NON_WRAPPING_OUTPUT_BUF);
    if (inf_status != TINFL_STATUS_DONE) {
        LOG_ERROR("tinfl_decompress failed: %d, in=%u, out=%u\n",
                  inf_status, (unsigned int) inflate_in_size, (unsigned int) inflate_out_size);
        return;
    }

    if (inflate_out_size != RAW_SIZE) {
        LOG_ERROR("length mismatch: %u\n", (unsigned int) inflate_out_size);
        return;
    }

    for (int i = 0; i < RAW_SIZE; i++) {
        if (unzip_buf[i] != raw_buf[i]) {
            LOG_ERROR("data mismatch\n");
            return;
        }
    }
    LOG_INFO("compress and decompress success, zip_len=%u\n", (unsigned int) deflate_out_size);
    for (int i = 0; i < inflate_out_size; i++) {
        printf("%d ", unzip_buf[i]);
    }
    printf("\n");
}

#endif


