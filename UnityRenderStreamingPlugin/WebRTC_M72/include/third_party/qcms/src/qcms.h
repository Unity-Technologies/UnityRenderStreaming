/* vim: set ts=8 sw=8 noexpandtab: */
//  qcms
//  Copyright (C) 2009 Mozilla Foundation
//
// Permission is hereby granted, free of charge, to any person obtaining 
// a copy of this software and associated documentation files (the "Software"), 
// to deal in the Software without restriction, including without limitation 
// the rights to use, copy, modify, merge, publish, distribute, sublicense, 
// and/or sell copies of the Software, and to permit persons to whom the Software 
// is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in 
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, 
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO 
// THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND 
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE 
// LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION 
// OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION 
// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#ifndef QCMS_H
#define QCMS_H

#ifdef  __cplusplus
extern "C" {
#endif

#include <stdio.h>

struct _qcms_profile;
typedef struct _qcms_profile qcms_profile;

struct _qcms_transform;
typedef struct _qcms_transform qcms_transform;

typedef int qcms_bool;

/* ICC Section 6.1.5 Color Space Signatures (abridged) */
typedef enum {
	XYZData		/* ‘XYZ ’ */ = 0x58595A20,
	labData		/* ‘Lab ’ */ = 0x4C616220,
	luvData		/* ‘Luv ’ */ = 0x4C757620,
	YCbCrData	/* ‘YCbr' */ = 0x59436272,
	YxyData		/* ‘Yxy ’ */ = 0x59787920,
	rgbData		/* ‘RGB ’ */ = 0x52474220,
	grayData	/* ‘GRAY’ */ = 0x47524159,
	hsvData		/* ‘HSV ’ */ = 0x48535620,
	hlsData		/* ‘HLS ’ */ = 0x484C5320,
	cmykData	/* ‘CMYK’ */ = 0x434D594B,
	cmyData		/* ‘CMY ’ */ = 0x434D5920,
} qcms_color_space;

/* ICC Section 6.1.11 Rendering Intents */
typedef enum {
	QCMS_INTENT_DEFAULT = 0,
	QCMS_INTENT_PERCEPTUAL = 0,
	QCMS_INTENT_RELATIVE_COLORIMETRIC = 1,
	QCMS_INTENT_SATURATION = 2,
	QCMS_INTENT_ABSOLUTE_COLORIMETRIC = 3
} qcms_intent;

/* Input data formats */
typedef enum {
	QCMS_DATA_RGB_8,
	QCMS_DATA_RGBA_8,
	QCMS_DATA_GRAY_8,
	QCMS_DATA_GRAYA_8
} qcms_data_type;

/* Output data format for qcms_transform_data_type() */
typedef enum {
	QCMS_OUTPUT_RGBX,
	QCMS_OUTPUT_BGRX
} qcms_output_type;

/* Output data format for qcms_transform_get_input|output_trc_rgba() */
typedef enum {
	QCMS_TRC_PARAMETRIC, // Not implemented.
	QCMS_TRC_FLOAT,      // Not implemented.
	QCMS_TRC_HALF_FLOAT, // IEE754: binary16.
	QCMS_TRC_USHORT,     // 0.16 fixed point.
} qcms_trc_type;

/* Output data of specific channel curve for qcms_profile_get_parametric_curve() */
typedef enum {
	QCMS_TRC_RED,
	QCMS_TRC_GREEN,
	QCMS_TRC_BLUE,
} qcms_trc_channel;

typedef struct {
	double x;
	double y;
	double Y;
} qcms_CIE_xyY;

typedef struct {
	qcms_CIE_xyY red;
	qcms_CIE_xyY green;
	qcms_CIE_xyY blue;
} qcms_CIE_xyYTRIPLE;

typedef struct {
	float X;
	float Y;
	float Z;
} qcms_xyz_float;

qcms_profile* qcms_profile_create_rgb_with_gamma(
		qcms_CIE_xyY white_point,
		qcms_CIE_xyYTRIPLE primaries,
		float gamma);

qcms_profile* qcms_profile_from_memory(const void *mem, size_t size);

qcms_profile* qcms_profile_from_file(FILE *file);
qcms_profile* qcms_profile_from_path(const char *path);
#ifdef _WIN32
qcms_profile* qcms_profile_from_unicode_path(const wchar_t *path);
#endif
qcms_profile* qcms_profile_sRGB(void);
void qcms_profile_release(qcms_profile *profile);

qcms_bool qcms_profile_is_bogus(qcms_profile *profile);
qcms_bool qcms_profile_has_white_point(qcms_profile *profile);
qcms_xyz_float qcms_profile_get_white_point(qcms_profile *profile);
qcms_intent qcms_profile_get_rendering_intent(qcms_profile *profile);
qcms_color_space qcms_profile_get_color_space(qcms_profile *profile);
unsigned qcms_profile_get_version(qcms_profile *profile);
qcms_bool qcms_profile_white_transform(qcms_profile *profile, float XYZ[3]);

qcms_bool qcms_profile_match(qcms_profile *p1, qcms_profile *p2);
const char* qcms_profile_get_description(qcms_profile *profile);

void qcms_profile_precache_output_transform(qcms_profile *profile);

size_t qcms_profile_get_vcgt_channel_length(qcms_profile *profile);
qcms_bool qcms_profile_get_vcgt_rgb_channels(qcms_profile *profile, unsigned short *data);

float qcms_profile_ntsc_relative_gamut_size(qcms_profile *profile);

size_t qcms_profile_get_parametric_curve(qcms_profile *profile, qcms_trc_channel channel, float data[7]);

qcms_transform* qcms_transform_create(
		qcms_profile *in, qcms_data_type in_type,
		qcms_profile *out, qcms_data_type out_type,
		qcms_intent intent);

size_t qcms_transform_get_input_trc_rgba(
		qcms_transform *transform, qcms_profile *in, qcms_trc_type type, unsigned short *data);
size_t qcms_transform_get_output_trc_rgba(
		qcms_transform *transform, qcms_profile *out, qcms_trc_type type, unsigned short *data);

qcms_bool qcms_transform_is_matrix(qcms_transform *transform);
float qcms_transform_get_matrix(qcms_transform *transform, unsigned i, unsigned j);

qcms_bool qcms_transform_create_LUT_zyx_bgra(
		qcms_profile *in, qcms_profile *out, qcms_intent intent,
		int samples, unsigned char* lut);

void qcms_transform_data(qcms_transform *transform, void *src, void *dest, size_t length);
void qcms_transform_data_type(qcms_transform *transform, void *src, void *dest, size_t length, qcms_output_type type);

void qcms_transform_release(qcms_transform *);

void qcms_enable_iccv4();

#ifdef  __cplusplus
}
#endif

/*
 * In general, QCMS is not threadsafe. However, it should be safe to create
 * profile and transformation objects on different threads, so long as you
 * don't use the same objects on different threads at the same time.
 */

#endif
