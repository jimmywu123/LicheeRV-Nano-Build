/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * ST7703 4.7" 720x1280 MIPI DSI panel - 2 lanes (31pin 標準介面)
 *
 * 對應螢幕 : 景鳴科技 4.7" 31PIN IPS 720x1280 ST7703(含驅動板)
 * 對應主機 : LicheeRV Nano (SG2002) 31pin MIPI DSI 座, 2 lane
 *
 * 初始化序列以 GX040HD (ST7703 720x720 4Lane, 廠商 BSP) 為基底改寫:
 *   - SETMIPI(0xBA) 參數1: 0x33 -> 0x31   (4 lane -> 2 lane)
 *   - SETDISP(0xB2)  參數1: 0x3C -> 0xC8   (NL=200 -> 1280 條線)
 * 其餘 gamma(0xE0)/VCOM(0xB6)/BGP(0xB5)/GIP(0xE9,0xEA)/POWER(0xC1) 為參考值,
 * 拿到景鳴提供的 ST7703 init code 後請以廠商數值覆蓋。
 *
 * 時序(HSA/HBP/HFP/VSA/VBP/VFP)為常見 720x1280 設定, 請以廠商 datasheet 覆核;
 * 若畫面偏移/閃爍, 調整 porch 或 pixel_clk。
 */

#ifndef _MIPI_TX_PARAM_ST7703_4P7_720X1280_H_
#define _MIPI_TX_PARAM_ST7703_4P7_720X1280_H_

#ifndef __UBOOT__
#include <linux/vo_mipi_tx.h>
#include <linux/cvi_comm_mipi_tx.h>
#else
#include <cvi_mipi.h>
#endif

/* ==================== 面板時序 (以廠商 datasheet 為準) ==================== */
#define ST7703_4P7_HACT 720
#define ST7703_4P7_HSA  10
#define ST7703_4P7_HBP  40
#define ST7703_4P7_HFP  40

#define ST7703_4P7_VACT 1280
#define ST7703_4P7_VSA  4
#define ST7703_4P7_VBP  16
#define ST7703_4P7_VFP  12

/* 單位 kHz: 720x1280@60Hz 約 63.7MHz */
#define ST7703_4P7_PIXEL_CLK(x) ((x##_VACT + x##_VSA + x##_VBP + x##_VFP) \
	* (x##_HACT + x##_HSA + x##_HBP + x##_HFP) * 60 / 1000)

struct combo_dev_cfg_s dev_cfg_st7703_720x1280 = {
	.devno = 0,
	/* Nano 只有 2 lane: D0 + CLK + D1 */
	.lane_id = {MIPI_TX_LANE_0, MIPI_TX_LANE_CLK, MIPI_TX_LANE_1, -1, -1},
	/* 若完全無畫面, 試著全部改 true (驅動板 P/N 對調) */
	.lane_pn_swap = {false, false, false, false, false},
	.output_mode = OUTPUT_MODE_DSI_VIDEO,
	.video_mode = BURST_MODE,
	.output_format = OUT_FORMAT_RGB_24_BIT,
	.sync_info = {
		.vid_hsa_pixels = ST7703_4P7_HSA,
		.vid_hbp_pixels = ST7703_4P7_HBP,
		.vid_hfp_pixels = ST7703_4P7_HFP,
		.vid_hline_pixels = ST7703_4P7_HACT,
		.vid_vsa_lines = ST7703_4P7_VSA,
		.vid_vbp_lines = ST7703_4P7_VBP,
		.vid_vfp_lines = ST7703_4P7_VFP,
		.vid_active_lines = ST7703_4P7_VACT,
		.vid_vsa_pos_polarity = false,
		.vid_hsa_pos_polarity = false,
	},
	.pixel_clk = ST7703_4P7_PIXEL_CLK(ST7703_4P7),
};

const struct hs_settle_s hs_timing_cfg_st7703_720x1280 = { .prepare = 6, .zero = 32, .trail = 1 };

/* DSI data_type 慣例: len==1 -> 0x05, len==2 -> 0x15, len>=3 -> 0x29 */
static CVI_U8 data_st7703_0[] = { 0xff, 0xf1, 0x12, 0x83 };			/* SETEXTC 解鎖 */
static CVI_U8 data_st7703_1[] = {						/* SETMIPI, 0x31 = 2 lane */
	0xba, 0x31, 0x81, 0x05, 0xf9, 0x0e, 0x0e, 0x20,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x44,
	0x25, 0x00, 0x90, 0x0a, 0x00, 0x00, 0x01, 0x4f,
	0x01, 0x00, 0x00, 0x37
};
static CVI_U8 data_st7703_2[] = { 0xb8, 0x25, 0x22, 0xf0, 0x63 };		/* SETPOWER_EXT */
static CVI_U8 data_st7703_3[] = { 0xbf, 0x02, 0x11, 0x00 };
static CVI_U8 data_st7703_4[] = {						/* SETRGBIF */
	0xb3, 0x10, 0x10, 0x28, 0x28, 0x03, 0xff, 0x00,
	0x00, 0x00, 0x00
};
static CVI_U8 data_st7703_5[] = {						/* SETSCR */
	0xc0, 0x73, 0x73, 0x50, 0x50, 0x00, 0x00, 0x12,
	0x70, 0x00
};
static CVI_U8 data_st7703_6[] = { 0xbc, 0x46 };				/* SETVDC */
static CVI_U8 data_st7703_7[] = { 0xcc, 0x0b };				/* SETPANEL 掃描方向 */
static CVI_U8 data_st7703_8[] = { 0xb4, 0x80 };				/* SETCYC */
static CVI_U8 data_st7703_9[] = { 0xb2, 0xc8, 0x12, 0x30 };			/* SETDISP: NL=0xC8 -> 1280 線, RESO_SEL=720RGB */
static CVI_U8 data_st7703_10[] = {						/* SETEQ */
	0xe3, 0x07, 0x07, 0x0b, 0x0b, 0x03, 0x0b, 0x00,
	0x00, 0x00, 0x00, 0xff, 0x00, 0xc0, 0x10
};
static CVI_U8 data_st7703_11[] = {						/* SETPOWER */
	0xc1, 0x36, 0x00, 0x32, 0x32, 0x77, 0xf1, 0xcc,
	0xcc, 0x77, 0x77, 0x33, 0x33
};
static CVI_U8 data_st7703_12[] = { 0xb5, 0x0a, 0x0a };			/* SETBGP */
static CVI_U8 data_st7703_13[] = { 0xb6, 0xb2, 0xb2 };			/* SETVCOM */
static CVI_U8 data_st7703_14[] = {						/* SETGIP1 */
	0xe9, 0xc8, 0x10, 0x0a, 0x10, 0x0f, 0xa1, 0x80,
	0x12, 0x31, 0x23, 0x47, 0x86, 0xa1, 0x80, 0x47,
	0x08, 0x00, 0x00, 0x0d, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x0d, 0x00, 0x00, 0x00, 0x48, 0x02, 0x8b,
	0xaf, 0x57, 0x13, 0x88, 0x88, 0x88, 0x88, 0x88,
	0x48, 0x13, 0x8b, 0xaf, 0x57, 0x13, 0x88, 0x88,
	0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88,
	0x88, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
static CVI_U8 data_st7703_15[] = {						/* SETGIP2 */
	0xea, 0x96, 0x12, 0x01, 0x01, 0x01, 0x78, 0x02,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x4f, 0x31, 0x8b,
	0xa8, 0x31, 0x75, 0x88, 0x88, 0x88, 0x88, 0x88,
	0x4f, 0x20, 0x8b, 0xa8, 0x20, 0x64, 0x88, 0x88,
	0x88, 0x88, 0x88, 0x23, 0x00, 0x00, 0x01, 0x02,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40,
	0xa1, 0x80, 0x00, 0x00, 0x00, 0x00
};
static CVI_U8 data_st7703_16[] = {						/* SETGAMMA */
	0xe0, 0x00, 0x0a, 0x0f, 0x29, 0x3b, 0x3f, 0x42,
	0x39, 0x06, 0x0d, 0x10, 0x13, 0x15, 0x14, 0x15,
	0x10, 0x17, 0x00, 0x0a, 0x0f, 0x29, 0x3b, 0x3f,
	0x42, 0x39, 0x06, 0x0d, 0x10, 0x13, 0x15, 0x14,
	0x15, 0x10, 0x17
};
static CVI_U8 data_st7703_17[] = { 0x11 };					/* SLPOUT */
static CVI_U8 data_st7703_18[] = { 0x29 };					/* DISPON */

const struct dsc_instr dsi_init_cmds_st7703_720x1280[] = {
	{.delay = 0,   .data_type = 0x29, .size = 4,  .data = data_st7703_0 },
	{.delay = 0,   .data_type = 0x29, .size = 28, .data = data_st7703_1 },
	{.delay = 0,   .data_type = 0x29, .size = 5,  .data = data_st7703_2 },
	{.delay = 0,   .data_type = 0x29, .size = 4,  .data = data_st7703_3 },
	{.delay = 0,   .data_type = 0x29, .size = 11, .data = data_st7703_4 },
	{.delay = 0,   .data_type = 0x29, .size = 10, .data = data_st7703_5 },
	{.delay = 0,   .data_type = 0x15, .size = 2,  .data = data_st7703_6 },
	{.delay = 0,   .data_type = 0x15, .size = 2,  .data = data_st7703_7 },
	{.delay = 0,   .data_type = 0x15, .size = 2,  .data = data_st7703_8 },
	{.delay = 0,   .data_type = 0x29, .size = 4,  .data = data_st7703_9 },
	{.delay = 0,   .data_type = 0x29, .size = 15, .data = data_st7703_10 },
	{.delay = 0,   .data_type = 0x29, .size = 13, .data = data_st7703_11 },
	{.delay = 0,   .data_type = 0x29, .size = 3,  .data = data_st7703_12 },
	{.delay = 0,   .data_type = 0x29, .size = 3,  .data = data_st7703_13 },
	{.delay = 0,   .data_type = 0x29, .size = 64, .data = data_st7703_14 },
	{.delay = 0,   .data_type = 0x29, .size = 62, .data = data_st7703_15 },
	{.delay = 0,   .data_type = 0x29, .size = 35, .data = data_st7703_16 },
	{.delay = 120, .data_type = 0x05, .size = 1,  .data = data_st7703_17 },	/* SLPOUT 後等 120ms */
	{.delay = 20,  .data_type = 0x05, .size = 1,  .data = data_st7703_18 },	/* DISPON */
};

#endif /* _MIPI_TX_PARAM_ST7703_4P7_720X1280_H_ */