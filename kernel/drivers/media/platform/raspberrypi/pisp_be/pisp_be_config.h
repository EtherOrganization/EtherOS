/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * PiSP Back End configuration definitions.
 *
 * Copyright (C) 2021 - Raspberry Pi Ltd
 *
 */
#ifndef _PISP_BE_CONFIG_H_
#define _PISP_BE_CONFIG_H_

#include <linux/types.h>

#include <media/raspberrypi/pisp_common.h>

/* byte alignment for inputs */
#define PISP_BACK_END_INPUT_ALIGN 4u
/* alignment for compressed inputs */
#define PISP_BACK_END_COMPRESSED_ALIGN 8u
/* minimum required byte alignment for outputs */
#define PISP_BACK_END_OUTPUT_MIN_ALIGN 16u
/* preferred byte alignment for outputs */
#define PISP_BACK_END_OUTPUT_MAX_ALIGN 64u

/* minimum allowed tile width anywhere in the pipeline */
#define PISP_BACK_END_MIN_TILE_WIDTH 16u
/* minimum allowed tile width anywhere in the pipeline */
#define PISP_BACK_END_MIN_TILE_HEIGHT 16u

#define PISP_BACK_END_NUM_OUTPUTS 2
#define PISP_BACK_END_HOG_OUTPUT 1

#define PISP_BACK_END_NUM_TILES 64

enum pisp_be_bayer_enable {
	PISP_BE_BAYER_ENABLE_INPUT = 0x000001,
	PISP_BE_BAYER_ENABLE_DECOMPRESS = 0x000002,
	PISP_BE_BAYER_ENABLE_DPC = 0x000004,
	PISP_BE_BAYER_ENABLE_GEQ = 0x000008,
	PISP_BE_BAYER_ENABLE_TDN_INPUT = 0x000010,
	PISP_BE_BAYER_ENABLE_TDN_DECOMPRESS = 0x000020,
	PISP_BE_BAYER_ENABLE_TDN = 0x000040,
	PISP_BE_BAYER_ENABLE_TDN_COMPRESS = 0x000080,
	PISP_BE_BAYER_ENABLE_TDN_OUTPUT = 0x000100,
	PISP_BE_BAYER_ENABLE_SDN = 0x000200,
	PISP_BE_BAYER_ENABLE_BLC = 0x000400,
	PISP_BE_BAYER_ENABLE_STITCH_INPUT = 0x000800,
	PISP_BE_BAYER_ENABLE_STITCH_DECOMPRESS = 0x001000,
	PISP_BE_BAYER_ENABLE_STITCH = 0x002000,
	PISP_BE_BAYER_ENABLE_STITCH_COMPRESS = 0x004000,
	PISP_BE_BAYER_ENABLE_STITCH_OUTPUT = 0x008000,
	PISP_BE_BAYER_ENABLE_WBG = 0x010000,
	PISP_BE_BAYER_ENABLE_CDN = 0x020000,
	PISP_BE_BAYER_ENABLE_LSC = 0x040000,
	PISP_BE_BAYER_ENABLE_TONEMAP = 0x080000,
	PISP_BE_BAYER_ENABLE_CAC = 0x100000,
	PISP_BE_BAYER_ENABLE_DEBIN = 0x200000,
	PISP_BE_BAYER_ENABLE_DEMOSAIC = 0x400000,
};

enum pisp_be_rgb_enable {
	PISP_BE_RGB_ENABLE_INPUT = 0x000001,
	PISP_BE_RGB_ENABLE_CCM = 0x000002,
	PISP_BE_RGB_ENABLE_SAT_CONTROL = 0x000004,
	PISP_BE_RGB_ENABLE_YCBCR = 0x000008,
	PISP_BE_RGB_ENABLE_FALSE_COLOUR = 0x000010,
	PISP_BE_RGB_ENABLE_SHARPEN = 0x000020,
	/* Preferred colours would occupy 0x000040 */
	PISP_BE_RGB_ENABLE_YCBCR_INVERSE = 0x000080,
	PISP_BE_RGB_ENABLE_GAMMA = 0x000100,
	PISP_BE_RGB_ENABLE_CSC0 = 0x000200,
	PISP_BE_RGB_ENABLE_CSC1 = 0x000400,
	PISP_BE_RGB_ENABLE_DOWNSCALE0 = 0x001000,
	PISP_BE_RGB_ENABLE_DOWNSCALE1 = 0x002000,
	PISP_BE_RGB_ENABLE_RESAMPLE0 = 0x008000,
	PISP_BE_RGB_ENABLE_RESAMPLE1 = 0x010000,
	PISP_BE_RGB_ENABLE_OUTPUT0 = 0x040000,
	PISP_BE_RGB_ENABLE_OUTPUT1 = 0x080000,
	PISP_BE_RGB_ENABLE_HOG = 0x200000
};

#define PISP_BE_RGB_ENABLE_CSC(i) (PISP_BE_RGB_ENABLE_CSC0 << (i))
#define PISP_BE_RGB_ENABLE_DOWNSCALE(i) (PISP_BE_RGB_ENABLE_DOWNSCALE0 << (i))
#define PISP_BE_RGB_ENABLE_RESAMPLE(i) (PISP_BE_RGB_ENABLE_RESAMPLE0 << (i))
#define PISP_BE_RGB_ENABLE_OUTPUT(i) (PISP_BE_RGB_ENABLE_OUTPUT0 << (i))

/*
 * We use the enable flags to show when blocks are "dirty", but we need some
 * extra ones too.
 */
enum pisp_be_dirty {
	PISP_BE_DIRTY_GLOBAL = 0x0001,
	PISP_BE_DIRTY_SH_FC_COMBINE = 0x0002,
	PISP_BE_DIRTY_CROP = 0x0004
};

struct pisp_be_global_config {
	u32 bayer_enables;
	u32 rgb_enables;
	u8 bayer_order;
	u8 pad[3];
};

struct pisp_be_input_buffer_config {
	/* low 32 bits followed by high 32 bits (for each of up to 3 planes) */
	u32 addr[3][2];
};

struct pisp_be_dpc_config {
	u8 coeff_level;
	u8 coeff_range;
	u8 pad;
#define PISP_BE_DPC_FLAG_FOLDBACK 1
	u8 flags;
};

struct pisp_be_geq_config {
	u16 offset;
#define PISP_BE_GEQ_SHARPER BIT(15)
#define PISP_BE_GEQ_SLOPE ((1 << 10) - 1)
	/* top bit is the "sharper" flag, slope value is bottom 10 bits */
	u16 slope_sharper;
	u16 min;
	u16 max;
};

struct pisp_be_tdn_input_buffer_config {
	/* low 32 bits followed by high 32 bits */
	u32 addr[2];
};

struct pisp_be_tdn_config {
	u16 black_level;
	u16 ratio;
	u16 noise_constant;
	u16 noise_slope;
	u16 threshold;
	u8 reset;
	u8 pad;
};

struct pisp_be_tdn_output_buffer_config {
	/* low 32 bits followed by high 32 bits */
	u32 addr[2];
};

struct pisp_be_sdn_config {
	u16 black_level;
	u8 leakage;
	u8 pad;
	u16 noise_constant;
	u16 noise_slope;
	u16 noise_constant2;
	u16 noise_slope2;
};

struct pisp_be_stitch_input_buffer_config {
	/* low 32 bits followed by high 32 bits */
	u32 addr[2];
};

#define PISP_BE_STITCH_STREAMING_LONG 0x8000
#define PISP_BE_STITCH_EXPOSURE_RATIO_MASK 0x7fff

struct pisp_be_stitch_config {
	u16 threshold_lo;
	u8 threshold_diff_power;
	u8 pad;

	/* top bit indicates whether streaming input is the long exposure */
	u16 exposure_ratio;

	u8 motion_threshold_256;
	u8 motion_threshold_recip;
};

struct pisp_be_stitch_output_buffer_config {
	/* low 32 bits followed by high 32 bits */
	u32 addr[2];
};

struct pisp_be_cdn_config {
	u16 thresh;
	u8 iir_strength;
	u8 g_adjust;
};

#define PISP_BE_LSC_LOG_GRID_SIZE 5
#define PISP_BE_LSC_GRID_SIZE (1 << PISP_BE_LSC_LOG_GRID_SIZE)
#define PISP_BE_LSC_STEP_PRECISION 18

struct pisp_be_lsc_config {
	/* (1<<18) / grid_cell_width */
	u16 grid_step_x;
	/* (1<<18) / grid_cell_height */
	u16 grid_step_y;
	/* RGB gains jointly encoded in 32 bits */
	u32 lut_packed[PISP_BE_LSC_GRID_SIZE + 1]
			   [PISP_BE_LSC_GRID_SIZE + 1];
};

struct pisp_be_lsc_extra {
	u16 offset_x;
	u16 offset_y;
};

#define PISP_BE_CAC_LOG_GRID_SIZE 3
#define PISP_BE_CAC_GRID_SIZE (1 << PISP_BE_CAC_LOG_GRID_SIZE)
#define PISP_BE_CAC_STEP_PRECISION 20

struct pisp_be_cac_config {
	/* (1<<20) / grid_cell_width */
	u16 grid_step_x;
	/* (1<<20) / grid_cell_height */
	u16 grid_step_y;
	/* [gridy][gridx][rb][xy] */
	s8 lut[PISP_BE_CAC_GRID_SIZE + 1][PISP_BE_CAC_GRID_SIZE + 1][2][2];
};

struct pisp_be_cac_extra {
	u16 offset_x;
	u16 offset_y;
};

#define PISP_BE_DEBIN_NUM_COEFFS 4

struct pisp_be_debin_config {
	s8 coeffs[PISP_BE_DEBIN_NUM_COEFFS];
	s8 h_enable;
	s8 v_enable;
	s8 pad[2];
};

#define PISP_BE_TONEMAP_LUT_SIZE 64

struct pisp_be_tonemap_config {
	u16 detail_constant;
	u16 detail_slope;
	u16 iir_strength;
	u16 strength;
	u32 lut[PISP_BE_TONEMAP_LUT_SIZE];
};

struct pisp_be_demosaic_config {
	u8 sharper;
	u8 fc_mode;
	u8 pad[2];
};

struct pisp_be_ccm_config {
	s16 coeffs[9];
	u8 pad[2];
	s32 offsets[3];
};

struct pisp_be_sat_control_config {
	u8 shift_r;
	u8 shift_g;
	u8 shift_b;
	u8 pad;
};

struct pisp_be_false_colour_config {
	u8 distance;
	u8 pad[3];
};

#define PISP_BE_SHARPEN_SIZE 5
#define PISP_BE_SHARPEN_FUNC_NUM_POINTS 9

struct pisp_be_sharpen_config {
	s8 kernel0[PISP_BE_SHARPEN_SIZE * PISP_BE_SHARPEN_SIZE];
	s8 pad0[3];
	s8 kernel1[PISP_BE_SHARPEN_SIZE * PISP_BE_SHARPEN_SIZE];
	s8 pad1[3];
	s8 kernel2[PISP_BE_SHARPEN_SIZE * PISP_BE_SHARPEN_SIZE];
	s8 pad2[3];
	s8 kernel3[PISP_BE_SHARPEN_SIZE * PISP_BE_SHARPEN_SIZE];
	s8 pad3[3];
	s8 kernel4[PISP_BE_SHARPEN_SIZE * PISP_BE_SHARPEN_SIZE];
	s8 pad4[3];
	u16 threshold_offset0;
	u16 threshold_slope0;
	u16 scale0;
	u16 pad5;
	u16 threshold_offset1;
	u16 threshold_slope1;
	u16 scale1;
	u16 pad6;
	u16 threshold_offset2;
	u16 threshold_slope2;
	u16 scale2;
	u16 pad7;
	u16 threshold_offset3;
	u16 threshold_slope3;
	u16 scale3;
	u16 pad8;
	u16 threshold_offset4;
	u16 threshold_slope4;
	u16 scale4;
	u16 pad9;
	u16 positive_strength;
	u16 positive_pre_limit;
	u16 positive_func[PISP_BE_SHARPEN_FUNC_NUM_POINTS];
	u16 positive_limit;
	u16 negative_strength;
	u16 negative_pre_limit;
	u16 negative_func[PISP_BE_SHARPEN_FUNC_NUM_POINTS];
	u16 negative_limit;
	u8 enables;
	u8 white;
	u8 black;
	u8 grey;
};

struct pisp_be_sh_fc_combine_config {
	u8 y_factor;
	u8 c1_factor;
	u8 c2_factor;
	u8 pad;
};

#define PISP_BE_GAMMA_LUT_SIZE 64

struct pisp_be_gamma_config {
	u32 lut[PISP_BE_GAMMA_LUT_SIZE];
};

struct pisp_be_crop_config {
	u16 offset_x, offset_y;
	u16 width, height;
};

#define PISP_BE_RESAMPLE_FILTER_SIZE 96

struct pisp_be_resample_config {
	u16 scale_factor_h, scale_factor_v;
	s16 coef[PISP_BE_RESAMPLE_FILTER_SIZE];
};

struct pisp_be_resample_extra {
	u16 scaled_width;
	u16 scaled_height;
	s16 initial_phase_h[3];
	s16 initial_phase_v[3];
};

struct pisp_be_downscale_config {
	u16 scale_factor_h;
	u16 scale_factor_v;
	u16 scale_recip_h;
	u16 scale_recip_v;
};

struct pisp_be_downscale_extra {
	u16 scaled_width;
	u16 scaled_height;
};

struct pisp_be_hog_config {
	u8 compute_signed;
	u8 channel_mix[3];
	u32 stride;
};

struct pisp_be_axi_config {
	u8 r_qos; /* Read QoS */
	u8 r_cache_prot; /* Read { prot[2:0], cache[3:0] } */
	u8 w_qos; /* Write QoS */
	u8 w_cache_prot; /* Write { prot[2:0], cache[3:0] } */
};

enum pisp_be_transform {
	PISP_BE_TRANSFORM_NONE = 0x0,
	PISP_BE_TRANSFORM_HFLIP = 0x1,
	PISP_BE_TRANSFORM_VFLIP = 0x2,
	PISP_BE_TRANSFORM_ROT180 =
		(PISP_BE_TRANSFORM_HFLIP | PISP_BE_TRANSFORM_VFLIP)
};

struct pisp_be_output_format_config {
	struct pisp_image_format_config image;
	u8 transform;
	u8 pad[3];
	u16 lo;
	u16 hi;
	u16 lo2;
	u16 hi2;
};

struct pisp_be_output_buffer_config {
	/* low 32 bits followed by high 32 bits (for each of 3 planes) */
	u32 addr[3][2];
};

struct pisp_be_hog_buffer_config {
	/* low 32 bits followed by high 32 bits */
	u32 addr[2];
};

struct pisp_be_config {
	/* I/O configuration: */
	struct pisp_be_input_buffer_config input_buffer;
	struct pisp_be_tdn_input_buffer_config tdn_input_buffer;
	struct pisp_be_stitch_input_buffer_config stitch_input_buffer;
	struct pisp_be_tdn_output_buffer_config tdn_output_buffer;
	struct pisp_be_stitch_output_buffer_config stitch_output_buffer;
	struct pisp_be_output_buffer_config
				output_buffer[PISP_BACK_END_NUM_OUTPUTS];
	struct pisp_be_hog_buffer_config hog_buffer;
	/* Processing configuration: */
	struct pisp_be_global_config global;
	struct pisp_image_format_config input_format;
	struct pisp_decompress_config decompress;
	struct pisp_be_dpc_config dpc;
	struct pisp_be_geq_config geq;
	struct pisp_image_format_config tdn_input_format;
	struct pisp_decompress_config tdn_decompress;
	struct pisp_be_tdn_config tdn;
	struct pisp_compress_config tdn_compress;
	struct pisp_image_format_config tdn_output_format;
	struct pisp_be_sdn_config sdn;
	struct pisp_bla_config blc;
	struct pisp_compress_config stitch_compress;
	struct pisp_image_format_config stitch_output_format;
	struct pisp_image_format_config stitch_input_format;
	struct pisp_decompress_config stitch_decompress;
	struct pisp_be_stitch_config stitch;
	struct pisp_be_lsc_config lsc;
	struct pisp_wbg_config wbg;
	struct pisp_be_cdn_config cdn;
	struct pisp_be_cac_config cac;
	struct pisp_be_debin_config debin;
	struct pisp_be_tonemap_config tonemap;
	struct pisp_be_demosaic_config demosaic;
	struct pisp_be_ccm_config ccm;
	struct pisp_be_sat_control_config sat_control;
	struct pisp_be_ccm_config ycbcr;
	struct pisp_be_sharpen_config sharpen;
	struct pisp_be_false_colour_config false_colour;
	struct pisp_be_sh_fc_combine_config sh_fc_combine;
	struct pisp_be_ccm_config ycbcr_inverse;
	struct pisp_be_gamma_config gamma;
	struct pisp_be_ccm_config csc[PISP_BACK_END_NUM_OUTPUTS];
	struct pisp_be_downscale_config downscale[PISP_BACK_END_NUM_OUTPUTS];
	struct pisp_be_resample_config resample[PISP_BACK_END_NUM_OUTPUTS];
	struct pisp_be_output_format_config
				output_format[PISP_BACK_END_NUM_OUTPUTS];
	struct pisp_be_hog_config hog;
	struct pisp_be_axi_config axi;
	/* Non-register fields: */
	struct pisp_be_lsc_extra lsc_extra;
	struct pisp_be_cac_extra cac_extra;
	struct pisp_be_downscale_extra
				downscale_extra[PISP_BACK_END_NUM_OUTPUTS];
	struct pisp_be_resample_extra resample_extra[PISP_BACK_END_NUM_OUTPUTS];
	struct pisp_be_crop_config crop;
	struct pisp_image_format_config hog_format;
	u32 dirty_flags_bayer; /* these use pisp_be_bayer_enable */
	u32 dirty_flags_rgb; /* use pisp_be_rgb_enable */
	u32 dirty_flags_extra; /* these use pisp_be_dirty_t */
};

/*
 * We also need a tile structure to describe the size of the tiles going
 * through the pipeline.
 */

enum pisp_tile_edge {
	PISP_LEFT_EDGE = (1 << 0),
	PISP_RIGHT_EDGE = (1 << 1),
	PISP_TOP_EDGE = (1 << 2),
	PISP_BOTTOM_EDGE = (1 << 3)
};

struct pisp_tile {
	u8 edge; // enum pisp_tile_edge
	u8 pad0[3];
	// 4 bytes
	u32 input_addr_offset;
	u32 input_addr_offset2;
	u16 input_offset_x;
	u16 input_offset_y;
	u16 input_width;
	u16 input_height;
	// 20 bytes
	u32 tdn_input_addr_offset;
	u32 tdn_output_addr_offset;
	u32 stitch_input_addr_offset;
	u32 stitch_output_addr_offset;
	// 36 bytes
	u32 lsc_grid_offset_x;
	u32 lsc_grid_offset_y;
	// 44 bytes
	u32 cac_grid_offset_x;
	u32 cac_grid_offset_y;
	// 52 bytes
	u16 crop_x_start[PISP_BACK_END_NUM_OUTPUTS];
	u16 crop_x_end[PISP_BACK_END_NUM_OUTPUTS];
	u16 crop_y_start[PISP_BACK_END_NUM_OUTPUTS];
	u16 crop_y_end[PISP_BACK_END_NUM_OUTPUTS];
	// 68 bytes
	/* Ordering is planes then branches */
	u16 downscale_phase_x[3 * PISP_BACK_END_NUM_OUTPUTS];
	u16 downscale_phase_y[3 * PISP_BACK_END_NUM_OUTPUTS];
	// 92 bytes
	u16 resample_in_width[PISP_BACK_END_NUM_OUTPUTS];
	u16 resample_in_height[PISP_BACK_END_NUM_OUTPUTS];
	// 100 bytes
	/* Ordering is planes then branches */
	u16 resample_phase_x[3 * PISP_BACK_END_NUM_OUTPUTS];
	u16 resample_phase_y[3 * PISP_BACK_END_NUM_OUTPUTS];
	// 124 bytes
	u16 output_offset_x[PISP_BACK_END_NUM_OUTPUTS];
	u16 output_offset_y[PISP_BACK_END_NUM_OUTPUTS];
	u16 output_width[PISP_BACK_END_NUM_OUTPUTS];
	u16 output_height[PISP_BACK_END_NUM_OUTPUTS];
	// 140 bytes
	u32 output_addr_offset[PISP_BACK_END_NUM_OUTPUTS];
	u32 output_addr_offset2[PISP_BACK_END_NUM_OUTPUTS];
	// 156 bytes
	u32 output_hog_addr_offset;
	// 160 bytes
};

static_assert(sizeof(struct pisp_tile) == 160);

struct pisp_be_tiles_config {
	struct pisp_be_config config;
	struct pisp_tile tiles[PISP_BACK_END_NUM_TILES];
	int num_tiles;
};

#endif /* _PISP_BE_CONFIG_H_ */
