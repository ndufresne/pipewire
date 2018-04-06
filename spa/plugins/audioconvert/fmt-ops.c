/* Spa
 * Copyright (C) 2018 Wim Taymans <wim.taymans@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include <string.h>
#include <stdio.h>

#include <spa/utils/defs.h>

#define U8_MIN		0
#define U8_MAX		((1U << 8) - 1)
#define U8_SCALE	((1U << 7) - 1)
#define U8_OFFS		(1U << 7)

#define S16_MIN		-((1U << 15) - 1)
#define S16_MAX		((1U << 15) - 1)
#define S16_SCALE	((1U << 15) - 1)

#define S24_MIN		-((1U << 23) - 1)
#define S24_MAX		((1U << 23) - 1)
#define S24_SCALE	((1U << 23) - 1)

#define S32_MIN		-((1U << 31) - 1)
#define S32_MAX		((1U << 31) - 1)
#define S32_SCALE	((1U << 31) - 1)

static void
conv_copy(void *data, int n_dst, void *dst[n_dst], int n_src, const void *src[n_src], int n_bytes)
{
	int i;
	for (i = 0; i < n_src; i++)
		memcpy(dst[i], src[i], n_bytes);
}

#define U8_TO_F32(v)	(((v) * (1.0f / U8_OFFS)) - 1.0)

static void
conv_u8_to_f32(void *data, int n_dst, void *dst[n_dst], int n_src, const void *src[n_src], int n_bytes)
{
	int i, j;

	for (i = 0; i < n_src; i++) {
		const int8_t *s = src[i];
		float *d = dst[i];

		for (j = 0; j < n_bytes; j++)
			d[j] = U8_TO_F32(s[j]);
	}
}

static void
conv_u8_to_f32d(void *data, int n_dst, void *dst[n_dst], int n_src, const void *src[n_src], int n_bytes)
{
	const int8_t *s = src[0];
	float **d = (float **) dst;
	int i, j;

	n_bytes /= n_dst;
	for (j = 0; j < n_bytes; j++) {
		for (i = 0; i < n_dst; i++)
			d[i][j] = U8_TO_F32(*s++);
	}
}

static void
conv_u8d_to_f32(void *data, int n_dst, void *dst[n_dst], int n_src, const void *src[n_src], int n_bytes)
{
	const int8_t **s = (const int8_t **) src;
	float *d = dst[0];
	int i, j;

	n_bytes /= n_src;
	for (j = 0; j < n_bytes; j++) {
		for (i = 0; i < n_src; i++)
			*d++ = U8_TO_F32(s[i][j]);
	}
}

#define S16_TO_F32(v)	((v) * (1.0f / S16_SCALE))

static void
conv_s16_to_f32(void *data, int n_dst, void *dst[n_dst], int n_src, const void *src[n_src], int n_bytes)
{
	int i, j;

	n_bytes /= sizeof(int16_t);
	for (i = 0; i < n_src; i++) {
		const int16_t *s = src[i];
		float *d = dst[i];

		for (j = 0; j < n_bytes; j++)
			d[j] = S16_TO_F32(s[j]);
	}
}

static void
conv_s16_to_f32d(void *data, int n_dst, void *dst[n_dst], int n_src, const void *src[n_src], int n_bytes)
{
	const int16_t *s = src[0];
	float **d = (float **) dst;
	int i, j;

	n_bytes /= (sizeof(int16_t) * n_dst);
	for (j = 0; j < n_bytes; j++) {
		for (i = 0; i < n_dst; i++)
			d[i][j] = S16_TO_F32(*s++);
	}
}

static void
conv_s16d_to_f32(void *data, int n_dst, void *dst[n_dst], int n_src, const void *src[n_src], int n_bytes)
{
	const int16_t **s = (const int16_t **) src;
	float *d = dst[0];
	int i, j;

	n_bytes /= (sizeof(int16_t) * n_src);
	for (j = 0; j < n_bytes; j++) {
		for (i = 0; i < n_src; i++)
			*d++ = S16_TO_F32(s[i][j]);
	}
}

#define S32_TO_F32(v)	((v) * (1.0f / S32_SCALE))

static void
conv_s32_to_f32(void *data, int n_dst, void *dst[n_dst], int n_src, const void *src[n_src], int n_bytes)
{
	int i, j;

	n_bytes /= sizeof(int32_t);
	for (i = 0; i < n_src; i++) {
		const int32_t *s = src[i];
		float *d = dst[i];

		for (j = 0; j < n_bytes; j++)
			d[j] = S32_TO_F32(s[j]);
	}
}

static void
conv_s32_to_f32d(void *data, int n_dst, void *dst[n_dst], int n_src, const void *src[n_src], int n_bytes)
{
	const int32_t *s = src[0];
	float **d = (float **) dst;
	int i, j;

	n_bytes /= (sizeof(int32_t) * n_dst);
	for (j = 0; j < n_bytes; j++) {
		for (i = 0; i < n_dst; i++)
			d[i][j] = S32_TO_F32(*s++);
	}
}

static void
conv_s32d_to_f32(void *data, int n_dst, void *dst[n_dst], int n_src, const void *src[n_src], int n_bytes)
{
	const int32_t **s = (const int32_t **) src;
	float *d = dst[0];
	int i, j;

	n_bytes /= (sizeof(int32_t) * n_src);
	for (j = 0; j < n_bytes; j++) {
		for (i = 0; i < n_src; i++)
			*d++ = S32_TO_F32(s[i][j]);
	}
}

#define READ24(s) (((uint32_t)s[0] << 16) | ((uint32_t)s[1] << 8) | ((uint32_t)s[2]))

#define S24_TO_F32(v)	((v) * (1.0f / S24_SCALE))

static void
conv_s24_to_f32(void *data, int n_dst, void *dst[n_dst], int n_src, const void *src[n_src], int n_bytes)
{
	int i, j;

	n_bytes /= 3;
	for (i = 0; i < n_src; i++) {
		const int8_t *s = src[i];
		float *d = dst[i];

		for (j = 0; j < n_bytes; j++) {
			d[j] = S24_TO_F32(READ24(s));
			s += 3;
		}
	}
}

static void
conv_s24_to_f32d(void *data, int n_dst, void *dst[n_dst], int n_src, const void *src[n_src], int n_bytes)
{
	const int8_t *s = src[0];
	float **d = (float **) dst;
	int i, j;

	n_bytes /= (3 * n_dst);
	for (j = 0; j < n_bytes; j++) {
		for (i = 0; i < n_dst; i++) {
			d[i][j] = S24_TO_F32(READ24(s));
			s += 3;
		}
	}
}

static void
conv_s24d_to_f32(void *data, int n_dst, void *dst[n_dst], int n_src, const void *src[n_src], int n_bytes)
{
	const int8_t **s = (const int8_t **) src;
	float *d = dst[0];
	int i, j;

	n_bytes /= (3 * n_src);
	for (j = 0; j < n_bytes; j++) {
		for (i = 0; i < n_src; i++) {
			*d++ = S24_TO_F32(READ24(s[i]));
			s += 3;
		}
	}
}

static void
conv_s24_32_to_f32(void *data, int n_dst, void *dst[n_dst], int n_src, const void *src[n_src], int n_bytes)
{
	int i, j;

	n_bytes /= sizeof(int32_t);
	for (i = 0; i < n_src; i++) {
		const int32_t *s = src[i];
		float *d = dst[i];

		for (j = 0; j < n_bytes; j++)
			d[j] = S24_TO_F32(s[j]);
	}
}

static void
conv_s24_32_to_f32d(void *data, int n_dst, void *dst[n_dst], int n_src, const void *src[n_src], int n_bytes)
{
	const int32_t *s = src[0];
	float **d = (float **) dst;
	int i, j;

	n_bytes /= (sizeof(int32_t) * n_dst);
	for (j = 0; j < n_bytes; j++) {
		for (i = 0; i < n_dst; i++)
			d[i][j] = S24_TO_F32(*s++);
	}
}

static void
conv_s24_32d_to_f32(void *data, int n_dst, void *dst[n_dst], int n_src, const void *src[n_src], int n_bytes)
{
	const int32_t **s = (const int32_t **) src;
	float *d = dst[0];
	int i, j;

	n_bytes /= (sizeof(int32_t) * n_src);
	for (j = 0; j < n_bytes; j++) {
		for (i = 0; i < n_src; i++)
			*d++ = S24_TO_F32(s[i][j]);
	}
}

#define F32_TO_U8(v)			\
({					\
	typeof(v) _v = (v);		\
	_v < -1.0f ? U8_MIN :		\
	_v >= 1.0f ? U8_MAX :		\
	(_v * U8_SCALE) + U8_OFFS;	\
})

static void
conv_f32_to_u8(void *data, int n_dst, void *dst[n_dst], int n_src, const void *src[n_src], int n_bytes)
{
	int i, j;

	n_bytes /= sizeof(float);
	for (i = 0; i < n_src; i++) {
		const float *s = src[i];
		int8_t *d = dst[i];

		for (j = 0; j < n_bytes; j++)
			d[j] = F32_TO_U8(s[j]);
	}
}

static void
conv_f32_to_u8d(void *data, int n_dst, void *dst[n_dst], int n_src, const void *src[n_src], int n_bytes)
{
	const float *s = src[0];
	int8_t **d = (int8_t **) dst;
	int i, j;

	n_bytes /= (sizeof(float) * n_dst);
	for (j = 0; j < n_bytes; j++) {
		for (i = 0; i < n_dst; i++)
			d[i][j] = F32_TO_U8(*s++);
	}
}

static void
conv_f32d_to_u8(void *data, int n_dst, void *dst[n_dst], int n_src, const void *src[n_src], int n_bytes)
{
	const int8_t **s = (const int8_t **) src;
	float *d = dst[0];
	int i, j;

	n_bytes /= (sizeof(float) * n_src);
	for (j = 0; j < n_bytes; j++) {
		for (i = 0; i < n_src; i++)
			*d++ = F32_TO_U8(s[i][j]);
	}
}

#define F32_TO_S16(v)		\
({				\
	typeof(v) _v = (v);	\
	_v < -1.0f ? S16_MIN :	\
	_v >= 1.0f ? S16_MAX :	\
	_v * S16_SCALE;		\
})

static void
conv_f32_to_s16(void *data, int n_dst, void *dst[n_dst], int n_src, const void *src[n_src], int n_bytes)
{
	int i, n, n_samples;

	n_samples = n_bytes / sizeof(float);
	for (i = 0; i < n_src; i++) {
		const float *s = src[i];
		int16_t *d = dst[i];

		for (n = 0; n < n_samples; n++)
			d[n] = F32_TO_S16(s[n]);
	}
}

static void
conv_f32_to_s16d(void *data, int n_dst, void *dst[n_dst], int n_src, const void *src[n_src], int n_bytes)
{
	const float *s = src[0];
	int16_t **d = (int16_t **) dst;
	int i, n, n_samples;

	n_samples = n_bytes / (sizeof(float) * n_dst);
	for (n = 0; n < n_samples; n++) {
		for (i = 0; i < n_dst; i++)
			d[i][n] = F32_TO_S16(*s++);
	}
}

static void
conv_f32d_to_s16(void *data, int n_dst, void *dst[n_dst], int n_src, const void *src[n_src], int n_bytes)
{
	const float **s = (const float **) src;
	int16_t *d = dst[0];
	int i, j;

	n_bytes /= (sizeof(float) * n_src);
	for (j = 0; j < n_bytes; j++) {
		for (i = 0; i < n_src; i++)
			*d++ = F32_TO_S16(s[i][j]);
	}
}

#define F32_TO_S32(v)		\
({				\
	typeof(v) _v = (v);	\
	_v < -1.0f ? S32_MIN :	\
	_v >= 1.0f ? S32_MAX :	\
	_v * S32_SCALE;		\
})

static void
conv_f32_to_s32(void *data, int n_dst, void *dst[n_dst], int n_src, const void *src[n_src], int n_bytes)
{
	int i, j;

	n_bytes /= sizeof(float);
	for (i = 0; i < n_src; i++) {
		const float *s = src[i];
		int32_t *d = dst[i];

		for (j = 0; j < n_bytes; j++)
			d[j] = F32_TO_S32(s[j]);
	}
}

static void
conv_f32_to_s32d(void *data, int n_dst, void *dst[n_dst], int n_src, const void *src[n_src], int n_bytes)
{
	const float *s = src[0];
	int32_t **d = (int32_t **) dst;
	int i, j;

	n_bytes /= (sizeof(float) * n_dst);
	for (j = 0; j < n_bytes; j++) {
		for (i = 0; i < n_dst; i++)
			d[i][j] = F32_TO_S32(*s++);
	}
}

static void
conv_f32d_to_s32(void *data, int n_dst, void *dst[n_dst], int n_src, const void *src[n_src], int n_bytes)
{
	const float **s = (const float **) src;
	int32_t *d = dst[0];
	int i, j;

	n_bytes /= (sizeof(float) * n_src);
	for (j = 0; j < n_bytes; j++) {
		for (i = 0; i < n_src; i++)
			*d++ = F32_TO_S32(s[i][j]);
	}
}

#define F32_TO_S24(v)			\
({					\
	typeof(v) _v = (v);		\
	_v < -1.0f ? S24_MIN :		\
	_v >= 1.0f ? S24_MAX :		\
	(uint32_t) (_v * S24_SCALE);	\
})

#define WRITE24(d,v)			\
({					\
	typeof(v) _v = (v);		\
	d[0] = (uint8_t) (_v >> 16);	\
	d[1] = (uint8_t) (_v >> 8);	\
	d[2] = (uint8_t) _v;		\
})

static void
conv_f32_to_s24(void *data, int n_dst, void *dst[n_dst], int n_src, const void *src[n_src], int n_bytes)
{
	int i, j;

	n_bytes /= sizeof(float);
	for (i = 0; i < n_src; i++) {
		const float *s = src[i];
		int8_t *d = dst[i];

		for (j = 0; j < n_bytes; j++)
			WRITE24(d, F32_TO_S24(s[j]));
			d += 3;
	}
}

static void
conv_f32_to_s24d(void *data, int n_dst, void *dst[n_dst], int n_src, const void *src[n_src], int n_bytes)
{
	const float *s = src[0];
	int8_t **d = (int8_t **) dst;
	int i, j;

	n_bytes /= (sizeof(float) * n_dst);
	for (j = 0; j < n_bytes; j++) {
		for (i = 0; i < n_dst; i++) {
			WRITE24(d[i], F32_TO_S24(*s++));
			d[i] += 3;
		}
	}
}

static void
conv_f32d_to_s24(void *data, int n_dst, void *dst[n_dst], int n_src, const void *src[n_src], int n_bytes)
{
	const float **s = (const float **) src;
	int8_t *d = dst[0];
	int i, j;

	n_bytes /= (sizeof(float) * n_src);
	for (j = 0; j < n_bytes; j++) {
		for (i = 0; i < n_src; i++) {
			WRITE24(d, F32_TO_S24(s[i][j]));
			d += 3;
		}
	}
}


static void
conv_f32_to_s24_32(void *data, int n_dst, void *dst[n_dst], int n_src, const void *src[n_src], int n_bytes)
{
	int i, j;

	n_bytes /= sizeof(float);
	for (i = 0; i < n_src; i++) {
		const float *s = src[i];
		int32_t *d = dst[i];

		for (j = 0; j < n_bytes; j++)
			d[j] = F32_TO_S24(s[j]);
	}
}

static void
conv_f32_to_s24_32d(void *data, int n_dst, void *dst[n_dst], int n_src, const void *src[n_src], int n_bytes)
{
	const float *s = src[0];
	int32_t **d = (int32_t **) dst;
	int i, j;

	n_bytes /= (sizeof(float) * n_dst);
	for (j = 0; j < n_bytes; j++) {
		for (i = 0; i < n_dst; i++)
			d[i][j] = F32_TO_S24(*s++);
	}
}

static void
conv_f32d_to_s24_32(void *data, int n_dst, void *dst[n_dst], int n_src, const void *src[n_src], int n_bytes)
{
	const float **s = (const float **) src;
	int32_t *d = dst[0];
	int i, j;

	n_bytes /= (sizeof(float) * n_src);
	for (j = 0; j < n_bytes; j++) {
		for (i = 0; i < n_src; i++)
			*d++ = F32_TO_S24(s[i][j]);
	}
}

static void
deinterleave_8(void *data, int n_dst, void *dst[n_dst], int n_src, const void *src[n_src], int n_bytes)
{
	const uint8_t *s = src[0];
	uint8_t **d = (uint8_t **) dst;
	int i, j;

	n_bytes /= n_dst;
	for (j = 0; j < n_bytes; j++) {
		for (i = 0; i < n_dst; i++)
			d[i][j] = *s++;
	}
}

static void
deinterleave_16(void *data, int n_dst, void *dst[n_dst], int n_src, const void *src[n_src], int n_bytes)
{
	const uint16_t *s = src[0];
	uint16_t **d = (uint16_t **) dst;
	int i, j;

	n_bytes /= (sizeof(uint16_t) * n_dst);
	for (j = 0; j < n_bytes; j++) {
		for (i = 0; i < n_dst; i++)
			d[i][j] = *s++;
	}
}

static void
deinterleave_24(void *data, int n_dst, void *dst[n_dst], int n_src, const void *src[n_src], int n_bytes)
{
	const uint8_t *s = src[0];
	uint8_t **d = (uint8_t **) dst;
	int i, j;

	n_bytes /= (3 * n_dst);
	for (j = 0; j < n_bytes; j++) {
		for (i = 0; i < n_dst; i++) {
			WRITE24(d[i], READ24(s));
			d += 3;
			s += 3;
		}
	}
}

static void
deinterleave_32(void *data, int n_dst, void *dst[n_dst], int n_src, const void *src[n_src], int n_bytes)
{
	const uint32_t *s = src[0];
	uint32_t **d = (uint32_t **) dst;
	int i, j;

	n_bytes /= (sizeof(uint32_t) * n_dst);
	for (j = 0; j < n_bytes; j++) {
		for (i = 0; i < n_dst; i++)
			d[i][j] = *s++;
	}
}

static void
interleave_8(void *data, int n_dst, void *dst[n_dst], int n_src, const void *src[n_src], int n_bytes)
{
	const int8_t **s = (const int8_t **) src;
	uint8_t *d = dst[0];
	int i, j;

	n_bytes /= (sizeof(uint8_t) * n_src);
	for (j = 0; j < n_bytes; j++) {
		for (i = 0; i < n_src; i++)
			*d++ = s[i][j];
	}
}

static void
interleave_16(void *data, int n_dst, void *dst[n_dst], int n_src, const void *src[n_src], int n_bytes)
{
	const int16_t **s = (const int16_t **) src;
	uint16_t *d = dst[0];
	int i, j;

	n_bytes /= (sizeof(uint16_t) * n_src);
	for (j = 0; j < n_bytes; j++) {
		for (i = 0; i < n_src; i++)
			*d++ = s[i][j];
	}
}

static void
interleave_24(void *data, int n_dst, void *dst[n_dst], int n_src, const void *src[n_src], int n_bytes)
{
	const int8_t **s = (const int8_t **) src;
	uint8_t *d = dst[0];
	int i, j;

	n_bytes /= (3 * n_src);
	for (j = 0; j < n_bytes; j++) {
		for (i = 0; i < n_src; i++) {
			WRITE24(d, READ24(s[i]));
			d += 3;
			s += 3;
		}
	}
}

static void
interleave_32(void *data, int n_dst, void *dst[n_dst], int n_src, const void *src[n_src], int n_bytes)
{
	const int32_t **s = (const int32_t **) src;
	uint32_t *d = dst[0];
	int i, j;

	n_bytes /= (sizeof(uint32_t) * n_src);
	for (j = 0; j < n_bytes; j++) {
		for (i = 0; i < n_src; i++)
			*d++ = s[i][j];
	}
}