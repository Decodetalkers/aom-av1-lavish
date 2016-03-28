/*
 * Copyright (c) 2016, Alliance for Open Media. All rights reserved
 *
 * This source code is subject to the terms of the BSD 2 Clause License and
 * the Alliance for Open Media Patent License 1.0. If the BSD 2 Clause License
 * was not distributed with this source code in the LICENSE file, you can
 * obtain it at www.aomedia.org/license/software. If the Alliance for Open
 * Media Patent License 1.0 was not distributed with this source code in the
 * PATENTS file, you can obtain it at www.aomedia.org/license/patent.
 */
#ifndef TOOLS_COMMON_H_
#define TOOLS_COMMON_H_

#include <stdio.h>

#include "./aom_config.h"
#include "aom/aom_codec.h"
#include "aom/aom_image.h"
#include "aom/aom_integer.h"
#include "aom_ports/msvc.h"

#if CONFIG_ENCODERS
#include "./y4minput.h"
#endif

#if defined(_MSC_VER)
/* MSVS uses _f{seek,tell}i64. */
#define fseeko _fseeki64
#define ftello _ftelli64
#elif defined(_WIN32)
/* MinGW uses f{seek,tell}o64 for large files. */
#define fseeko fseeko64
#define ftello ftello64
#endif /* _WIN32 */

#if CONFIG_OS_SUPPORT
#if defined(_MSC_VER)
#include <io.h> /* NOLINT */
#define isatty _isatty
#define fileno _fileno
#else
#include <unistd.h> /* NOLINT */
#endif              /* _MSC_VER */
#endif              /* CONFIG_OS_SUPPORT */

/* Use 32-bit file operations in WebM file format when building ARM
 * executables (.axf) with RVCT. */
#if !CONFIG_OS_SUPPORT
#define fseeko fseek
#define ftello ftell
#endif /* CONFIG_OS_SUPPORT */

#define LITERALU64(hi, lo) ((((uint64_t)hi) << 32) | lo)

#ifndef PATH_MAX
#define PATH_MAX 512
#endif

#define IVF_FRAME_HDR_SZ (4 + 8) /* 4 byte size + 8 byte timestamp */
#define IVF_FILE_HDR_SZ 32

#define RAW_FRAME_HDR_SZ sizeof(uint32_t)

#define AV1_FOURCC 0x31305641

enum VideoFileType {
  FILE_TYPE_RAW,
  FILE_TYPE_IVF,
  FILE_TYPE_Y4M,
  FILE_TYPE_WEBM
};

struct FileTypeDetectionBuffer {
  char buf[4];
  size_t buf_read;
  size_t position;
};

struct VpxRational {
  int numerator;
  int denominator;
};

struct VpxInputContext {
  const char *filename;
  FILE *file;
  int64_t length;
  struct FileTypeDetectionBuffer detect;
  enum VideoFileType file_type;
  uint32_t width;
  uint32_t height;
  struct VpxRational pixel_aspect_ratio;
  aom_img_fmt_t fmt;
  aom_bit_depth_t bit_depth;
  int only_i420;
  uint32_t fourcc;
  struct VpxRational framerate;
#if CONFIG_ENCODERS
  y4m_input y4m;
#endif
};

#ifdef __cplusplus
extern "C" {
#endif

#if defined(__GNUC__)
#define AOM_NO_RETURN __attribute__((noreturn))
#else
#define AOM_NO_RETURN
#endif

/* Sets a stdio stream into binary mode */
FILE *set_binary_mode(FILE *stream);

void die(const char *fmt, ...) AOM_NO_RETURN;
void fatal(const char *fmt, ...) AOM_NO_RETURN;
void warn(const char *fmt, ...);

void die_codec(aom_codec_ctx_t *ctx, const char *s) AOM_NO_RETURN;

/* The tool including this file must define usage_exit() */
void usage_exit(void) AOM_NO_RETURN;

#undef AOM_NO_RETURN

int read_yuv_frame(struct VpxInputContext *input_ctx, aom_image_t *yuv_frame);

typedef struct VpxInterface {
  const char *const name;
  const uint32_t fourcc;
  aom_codec_iface_t *(*const codec_interface)();
} VpxInterface;

int get_aom_encoder_count(void);
const VpxInterface *get_aom_encoder_by_index(int i);
const VpxInterface *get_aom_encoder_by_name(const char *name);

int get_aom_decoder_count(void);
const VpxInterface *get_aom_decoder_by_index(int i);
const VpxInterface *get_aom_decoder_by_name(const char *name);
const VpxInterface *get_aom_decoder_by_fourcc(uint32_t fourcc);

// TODO(dkovalev): move this function to aom_image.{c, h}, so it will be part
// of aom_image_t support
int aom_img_plane_width(const aom_image_t *img, int plane);
int aom_img_plane_height(const aom_image_t *img, int plane);
void aom_img_write(const aom_image_t *img, FILE *file);
int aom_img_read(aom_image_t *img, FILE *file);

double sse_to_psnr(double samples, double peak, double mse);

#if CONFIG_AOM_HIGHBITDEPTH
void aom_img_upshift(aom_image_t *dst, aom_image_t *src, int input_shift);
void aom_img_downshift(aom_image_t *dst, aom_image_t *src, int down_shift);
void aom_img_truncate_16_to_8(aom_image_t *dst, aom_image_t *src);
#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif  // TOOLS_COMMON_H_
