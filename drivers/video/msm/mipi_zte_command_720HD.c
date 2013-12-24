/* Copyright (c) 2010-2012, Code Aurora Forum. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include <linux/leds.h>
#include "msm_fb.h"
#include "mipi_dsi.h"
#include "mdp4.h"
#include <mach/gpio.h>

#define NOVATEK_TWO_LANE
extern u32 LcdPanleID;
#define GPIO_LCD_RESET 58
#ifdef CONFIG_MACH_GAEA
#define USE_CABC_PANEL	//add cabc only for p893e10(gaea)
#endif

static struct mipi_dsi_panel_platform_data *mipi_novatek_pdata;

static struct dsi_buf novatek_tx_buf;
static struct dsi_buf novatek_rx_buf;
static int mipi_novatek_lcd_init(void);
static void lcd_panle_reset(void);

#define MIPI_DSI_NOVATEK_SPI_DEVICE_NAME	"dsi_novatek_3d_panel_spi"
#define HPCI_FPGA_READ_CMD	0x84
#define HPCI_FPGA_WRITE_CMD	0x04

static void novatek_fpga_write(uint8 addr, uint16 value)
{
	return;
}

static void novatek_fpga_read(uint8 addr)
{
	return;
}


/* novatek blue panel */

#ifdef NOVETAK_COMMANDS_UNUSED
static char display_config_cmd_mode1[] = {
	/* TYPE_DCS_LWRITE */
	0x2A, 0x00, 0x00, 0x01,
	0x3F, 0xFF, 0xFF, 0xFF
};

static char display_config_cmd_mode2[] = {
	/* DTYPE_DCS_LWRITE */
	0x2B, 0x00, 0x00, 0x01,
	0xDF, 0xFF, 0xFF, 0xFF
};

static char display_config_cmd_mode3_666[] = {
	/* DTYPE_DCS_WRITE1 */
	0x3A, 0x66, 0x15, 0x80 /* 666 Packed (18-bits) */
};

static char display_config_cmd_mode3_565[] = {
	/* DTYPE_DCS_WRITE1 */
	0x3A, 0x55, 0x15, 0x80 /* 565 mode */
};

static char display_config_321[] = {
	/* DTYPE_DCS_WRITE1 */
	0x66, 0x2e, 0x15, 0x00 /* Reg 0x66 : 2E */
};

static char display_config_323[] = {
	/* DTYPE_DCS_WRITE */
	0x13, 0x00, 0x05, 0x00 /* Reg 0x13 < Set for Normal Mode> */
};

static char display_config_2lan[] = {
	/* DTYPE_DCS_WRITE */
	0x61, 0x01, 0x02, 0xff /* Reg 0x61 : 01,02 < Set for 2 Data Lane > */
};

static char display_config_exit_sleep[] = {
	/* DTYPE_DCS_WRITE */
	0x11, 0x00, 0x05, 0x80 /* Reg 0x11 < exit sleep mode> */
};

static char display_config_TE_ON[] = {
	/* DTYPE_DCS_WRITE1 */
	0x35, 0x00, 0x15, 0x80
};

static char display_config_39H[] = {
	/* DTYPE_DCS_WRITE */
	0x39, 0x00, 0x05, 0x80
};

static char display_config_set_tear_scanline[] = {
	/* DTYPE_DCS_LWRITE */
	0x44, 0x00, 0x00, 0xff
};

static char display_config_set_twolane[] = {
	/* DTYPE_DCS_WRITE1 */
	0xae, 0x03, 0x15, 0x80
};

static char display_config_set_threelane[] = {
	/* DTYPE_DCS_WRITE1 */
	0xae, 0x05, 0x15, 0x80
};

#else

static char sw_reset[2] = {0x01, 0x00}; /* DTYPE_DCS_WRITE */
static char enter_sleep[2] = {0x10, 0x00}; /* DTYPE_DCS_WRITE */
static char exit_sleep[2] = {0x11, 0x00}; /* DTYPE_DCS_WRITE */
static char display_off[2] = {0x28, 0x00}; /* DTYPE_DCS_WRITE */
static char display_on[2] = {0x29, 0x00}; /* DTYPE_DCS_WRITE */

#ifdef USE_CABC_PANEL
	static char cabc_cmd_C1_51[] = {0x51,0xff};
	static char cabc_cmd_C1_53[] = {0x53,0x2c};
	static char cabc_cmd_C1_55[] = {0x55,0x01};

	static char Gen_write_1A_1P_SETTING[] = {0x00,0x80};
	static char Gen_write_1A_28P_SETTING[] = {0xCA,0x80,178,182,188,192,197,202,207,212,217,222,227,232,237,242,247,190,255,190,255,190,255,5,3,5,3,5,3};
	static char Gen_write_1A_1P_CABC1_1[] = {0x00,0x00};
	static char Gen_write_1A_1P_CABC1_2[] = {0xC7,0x10};
	static char Gen_write_1A_18P_CABC1[] = {0xC8,0x90,0x98,0x98,0x89,0x98,0x99,0x88,0x88,0x88,0x9A,0x88,0x99,0x98,0x88,0x78,0x77,0x66,0x55};
	static char Gen_write_1A_1P_CABC2_1[] = {0x00,0x00};
	static char Gen_write_1A_1P_CABC2_2[] = {0xC7,0x11};
	static char Gen_write_1A_18P_CABC2[] = {0xC8,0x90,0x98,0x98,0x89,0x98,0x99,0x88,0x88,0x88,0x99,0x88,0x99,0x88,0x88,0x88,0x77,0x66,0x56};
	static char Gen_write_1A_1P_CABC3_1[] = {0x00,0x00};
	static char Gen_write_1A_1P_CABC3_2[] = {0xC7,0x12};
	static char Gen_write_1A_18P_CABC3[] = {0xC8,0x90,0x98,0x98,0x89,0x98,0x99,0x88,0x88,0x88,0x98,0x98,0x98,0x89,0x88,0x88,0x77,0x66,0x56};
	static char Gen_write_1A_1P_CABC4_1[] = {0x00,0x00};
	static char Gen_write_1A_1P_CABC4_2[] = {0xC7,0x13};
	static char Gen_write_1A_18P_CABC4[] = {0xC8,0x90,0x98,0x98,0x89,0x98,0x89,0x89,0x88,0x88,0x88,0x98,0x99,0x88,0x88,0x88,0x77,0x77,0x56};
	static char Gen_write_1A_1P_CABC5_1[] = {0x00,0x00};
	static char Gen_write_1A_1P_CABC5_2[] = {0xC7,0x14};
	static char Gen_write_1A_18P_CABC5[] = {0xC8,0x90,0x98,0x98,0x89,0x88,0x89,0x88,0x88,0x88,0x98,0x98,0x99,0x88,0x88,0x88,0x77,0x77,0x56};
	static char Gen_write_1A_1P_CABC6_1[] = {0x00,0x00};
	static char Gen_write_1A_1P_CABC6_2[] = {0xC7,0x15};
	static char Gen_write_1A_18P_CABC6[] = {0xC8,0x90,0x98,0x98,0x89,0x88,0x89,0x88,0x88,0x88,0x88,0xA8,0x89,0x88,0x88,0x88,0x78,0x77,0x66};
	static char Gen_write_1A_1P_CABC7_1[] = {0x00,0x00};
	static char Gen_write_1A_1P_CABC7_2[] = {0xC7,0x16};
	static char Gen_write_1A_18P_CABC7[] = {0xC8,0x90,0x98,0x98,0x89,0x88,0x89,0x88,0x88,0x88,0x88,0xA8,0x98,0x88,0x88,0x88,0x78,0x77,0x66};
	static char Gen_write_1A_1P_CABC8_1[] = {0x00,0x00};
	static char Gen_write_1A_1P_CABC8_2[] = {0xC7,0x17};
	static char Gen_write_1A_18P_CABC8[] = {0xC8,0x90,0x98,0x98,0x88,0x88,0x89,0x88,0x88,0x88,0x88,0xA8,0x98,0x88,0x88,0x88,0x78,0x77,0x67};
	static char Gen_write_1A_1P_CABC9_1[] = {0x00,0x00};
	static char Gen_write_1A_1P_CABC9_2[] = {0xC7,0x18};
	static char Gen_write_1A_18P_CABC9[] = {0xC8,0x90,0x98,0x98,0x88,0x88,0x89,0x88,0x88,0x88,0x88,0xA8,0x88,0x88,0x88,0x88,0x88,0x77,0x67};
	static char Gen_write_1A_1P_CABC10_1[] = {0x00,0x00};
	static char Gen_write_1A_1P_CABC10_2[] = {0xC7,0x19};
	static char Gen_write_1A_18P_CABC10[] = {0xC8,0x90,0x98,0x98,0x88,0x88,0x89,0x88,0x88,0x88,0x88,0x98,0x88,0x88,0x88,0x88,0x88,0x77,0x67};
	static char Gen_write_1A_1P_CABC11_1[] = {0x00,0x00};
	static char Gen_write_1A_1P_CABC11_2[] = {0xC7,0x1A};
	static char Gen_write_1A_18P_CABC11[] = {0xC8,0x90,0x98,0x88,0x88,0x88,0x88,0x88,0x88,0x88,0x98,0x98,0x88,0x88,0x88,0x88,0x88,0x78,0x77};
	static char Gen_write_1A_1P_CABC12_1[] = {0x00,0x00};
	static char Gen_write_1A_1P_CABC12_2[] = {0xC7,0x1B};
	static char Gen_write_1A_18P_CABC12[] = {0xC8,0x90,0x99,0x88,0x88,0x88,0x88,0x88,0x88,0x88,0x88,0x88,0x88,0x88,0x98,0x88,0x88,0x78,0x77};
	static char Gen_write_1A_1P_CABC13_1[] = {0x00,0x00};
	static char Gen_write_1A_1P_CABC13_2[] = {0xC7,0x1C};
	static char Gen_write_1A_18P_CABC13[] = {0xC8,0x90,0x89,0x88,0x88,0x88,0x88,0x88,0x88,0x88,0x88,0x88,0x98,0x88,0x88,0x88,0x88,0x88,0x78};
	static char Gen_write_1A_1P_CABC14_1[] = {0x00,0x00};
	static char Gen_write_1A_1P_CABC14_2[] = {0xC7,0x1D};
	static char Gen_write_1A_18P_CABC14[] = {0xC8,0x90,0x89,0x88,0x88,0x88,0x88,0x88,0x88,0x88,0x88,0x88,0x88,0x88,0x88,0x88,0x88,0x88,0x78};
	static char Gen_write_1A_1P_CABC15_1[] = {0x00,0x00};
	static char Gen_write_1A_1P_CABC15_2[] = {0xC7,0x1E};
	static char Gen_write_1A_18P_CABC15[] = {0xC8,0x90,0x88,0x88,0x88,0x88,0x88,0x88,0x88,0x88,0x88,0x88,0x88,0x88,0x88,0x88,0x88,0x88,0x88};
	static char Gen_write_1A_1P_CABC16_1[] = {0x00,0x00};
	static char Gen_write_1A_1P_CABC16_2[] = {0xC7,0x1F};
	static char Gen_write_1A_18P_CABC16[] = {0xC8,0x80,0x88,0x88,0x88,0x88,0x88,0x88,0x88,0x88,0x88,0x88,0x88,0x88,0x88,0x88,0x88,0x88,0x88};



//static char cabc_cmd_C1_51[] = {0x51,0xff};
 
static struct dsi_cmd_desc set_backlight[] ={
 
	 {DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cabc_cmd_C1_51), cabc_cmd_C1_51}
	 
};
 
 
static void set_bk_lv(struct msm_fb_data_type *mfd,uint32 level)
{
 
	struct dcs_cmd_req cmdreq;
	mipi_set_tx_power_mode(0);
	cabc_cmd_C1_51[1] = level;
	cmdreq.cmds = set_backlight;
	cmdreq.cmds_cnt = ARRAY_SIZE(set_backlight);
	cmdreq.flags = CMD_REQ_COMMIT;
	cmdreq.rlen = 0;
	cmdreq.cb = NULL;
	mipi_dsi_cmdlist_put(&cmdreq);
	
}

#endif

static char rgb_888[2] = {0x3A, 0x77}; /* DTYPE_DCS_WRITE1 */

#if defined(NOVATEK_TWO_LANE)
static char set_num_of_lanes[2] = {0xae, 0x03}; /* DTYPE_DCS_WRITE1 */
#else  /* 1 lane */
static char set_num_of_lanes[2] = {0xae, 0x01}; /* DTYPE_DCS_WRITE1 */
#endif
/* commands by Novatke */
static char novatek_f4[2] = {0xf4, 0x55}; /* DTYPE_DCS_WRITE1 */
static char novatek_8c[16] = { /* DTYPE_DCS_LWRITE */
	0x8C, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x08, 0x08, 0x00, 0x30, 0xC0, 0xB7, 0x37};
static char novatek_ff[2] = {0xff, 0x55 }; /* DTYPE_DCS_WRITE1 */

static char set_width[5] = { /* DTYPE_DCS_LWRITE */
	0x2A, 0x00, 0x00, 0x02, 0x1B}; /* 540 - 1 */
static char set_height[5] = { /* DTYPE_DCS_LWRITE */
	0x2B, 0x00, 0x00, 0x03, 0xBF}; /* 960 - 1 */
#endif





static char led_pwm2[2] = {0x53, 0x24}; /* DTYPE_DCS_WRITE1 */
static char led_pwm3[2] = {0x55, 0x00}; /* DTYPE_DCS_WRITE1 */
static char otm_shift_00[] = {0x00, 0x00};

//truly OTM1281A
static char truly_cmd_FF1[] = {0xFF, 0x12,0x80,0x01};
static char truly_shift_FF2[] = {0x00, 0x80};
static char truly_cmd_FF2[] = {0xFF, 0x12,0x80};
static char truly_shift_b0[] = {0x00, 0xb0};
static char truly_cmd_b0[] = {0xb0, 0x20};
static char truly_shift_C51[] = {0x00, 0x90};
static char truly_cmd_C51[] = {0xC5, 0x20,0x6F,0x02,0x88,0x1d,0x15,0x00,0x08,0x44,0x44,0x44};
static char truly_shift_C52[] = {0x00, 0xa0};
static char truly_cmd_C52[] = {0xC5, 0x20,0x6F,0x02,0x88,0x1d,0x15,0x00,0x08};
static char truly_shift_C53[] = {0x00, 0x80};
static char truly_cmd_C53[] = {0xC5, 0x20,0x01,0x00,0xB0,0xB0,0x00,0x08,0x00};
static char truly_shift_D8[] = {0x00, 0x00};
static char truly_cmd_D8[] = {0xD8, 0x58,0x00,0x58,0x00};
static char truly_shift_F5[] = {0x00, 0xb8};
static char truly_cmd_F5[] = {0xF5, 0x0C,0x12};

/* add rate to 65hz
static char truly_shift_C1_80[] = {0x00, 0x80};
static char truly_cmd_C1_80[] = {0xc1, 0x85};
static char truly_shift_C5_D0[] = {0x00, 0xD0};
static char truly_cmd_C5_D0[] = {0xc5,0x38};
*/

static char truly_shift_00[] = {0x00, 0x00};

static char truly_shift_83[] = {0x00, 0x83};
static char truly_cmd_F4[] = {0xf4, 0x00};

static char truly_cmd_D4_00[] = {0xD4,
								0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,
								0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40
};
static char truly_cmd_D4_40[] = {0xD4,
								0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,
								0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40
};
static char truly_cmd_D4_80[] = {0xD4,
								0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,
								0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40
};
static char truly_cmd_D4_120[] = {0xD4,
								0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,
								0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40
};
static char truly_cmd_D4_160[] = {0xD4,
								0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,
								0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40
};
static char truly_cmd_D4_200[] = {0xD4,
								0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,
								0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40
};
static char truly_cmd_D4_240[] = {0xD4,
								0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,
								0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40
};
static char truly_cmd_D4_280[] = {0xD4,
								0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,
								0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40
};
static char truly_cmd_D4_320[] = {0xD4,
								0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,
								0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40
};
static char truly_cmd_D5_00[] = {0xD5,
							0x00,0x60,0x00,0x60,0x00,0x5f,0x00,0x5f,0x00,0x5e,0x00,0x5e,0x00,0x5d,0x00,0x5d,0x00,0x5d,0x00,0x5c,
							0x00,0x5c,0x00,0x5b,0x00,0x5b,0x00,0x5a,0x00,0x5a,0x00,0x5a,0x00,0x5b,0x00,0x5c,0x00,0x5d,0x00,0x5d
};
static char truly_cmd_D5_40[] = {0xD5,
							0x00,0x5e,0x00,0x5f,0x00,0x60,0x00,0x61,0x00,0x62,0x00,0x63,0x00,0x63,0x00,0x64,0x00,0x65,0x00,0x66,
							0x00,0x67,0x00,0x68,0x00,0x69,0x00,0x69,0x00,0x6a,0x00,0x6b,0x00,0x6c,0x00,0x6d,0x00,0x6e,0x00,0x6f
};
static char truly_cmd_D5_80[] = {0xD5,
							0x00,0x6f,0x00,0x70,0x00,0x71,0x00,0x72,0x00,0x73,0x00,0x74,0x00,0x74,0x00,0x75,0x00,0x76,0x00,0x77,
							0x00,0x78,0x00,0x78,0x00,0x79,0x00,0x7a,0x00,0x7b,0x00,0x7c,0x00,0x7d,0x00,0x7d,0x00,0x7e,0x00,0x7f
};
static char truly_cmd_D5_120[] = {0xD5,
							0x00,0x7f,0x00,0x7f,0x00,0x7f,0x00,0x7f,0x00,0x7f,0x00,0x7f,0x00,0x7f,0x00,0x7f,0x00,0x7f,0x00,0x7f,
							0x00,0x7f,0x00,0x7f,0x00,0x7f,0x00,0x7f,0x00,0x7f,0x00,0x7f,0x00,0x7f,0x00,0x7f,0x00,0x7f,0x00,0x7f
};
static char truly_cmd_D5_160[] = {0xD5,						
							0x00,0x7f,0x00,0x7f,0x00,0x7f,0x00,0x7f,0x00,0x7f,0x00,0x7f,0x00,0x7f,0x00,0x7f,0x00,0x7f,0x00,0x7f,
							0x00,0x7f,0x00,0x7f,0x00,0x7f,0x00,0x7f,0x00,0x7f,0x00,0x7f,0x00,0x7f,0x00,0x7f,0x00,0x7f,0x00,0x7f
};
static char truly_cmd_D5_200[] = {0xD5,						
							0x00,0x7f,0x00,0x7f,0x00,0x7f,0x00,0x7f,0x00,0x7f,0x00,0x7f,0x00,0x7f,0x00,0x7f,0x00,0x7f,0x00,0x7f,
							0x00,0x7f,0x00,0x7f,0x00,0x7f,0x00,0x7f,0x00,0x7f,0x00,0x7f,0x00,0x7f,0x00,0x7f,0x00,0x7f,0x00,0x7f
};
static char truly_cmd_D5_240[] = {0xD5,						
							0x00,0x7f,0x00,0x7f,0x00,0x7f,0x00,0x7f,0x00,0x7f,0x00,0x7f,0x00,0x7f,0x00,0x7f,0x00,0x7f,0x00,0x7f,
							0x00,0x7f,0x00,0x7f,0x00,0x7f,0x00,0x7f,0x00,0x7f,0x00,0x7f,0x00,0x7f,0x00,0x7e,0x00,0x7d,0x00,0x7c
};
static char truly_cmd_D5_280[] = {0xD5,						
							0x00,0x7b,0x00,0x7a,0x00,0x7a,0x00,0x79,0x00,0x78,0x00,0x77,0x00,0x76,0x00,0x76,0x00,0x75,0x00,0x74,
							0x00,0x73,0x00,0x72,0x00,0x71,0x00,0x71,0x00,0x70,0x00,0x6f,0x00,0x6e,0x00,0x6d,0x00,0x6c,0x00,0x6c
};
static char truly_cmd_D5_320[] = {0xD5,
							0x00,0x6b,0x00,0x6a,0x00,0x69,0x00,0x68,0x00,0x67,0x00,0x66,0x00,0x66,0x00,0x66,0x00,0x65,0x00,0x65,
							0x00,0x64,0x00,0x64,0x00,0x63,0x00,0x63,0x00,0x63,0x00,0x62,0x00,0x62,0x00,0x61,0x00,0x61,0x00,0x60
	};

//static char truly_cmd_36[] = {0x36, 0xd0};
static char truly_cmd_35[] = {0x35, 0x00};

static char truly_cmd_2c[] = {0x2c, 0x00};
static char truly_cmd_3c[] = {0x3c, 0x00};
static char truly_shift_B0_80[] = {0x00,0x80};
static char truly_cmd_B0_80[] = {0xb0,0x60};
static char truly_shift_B0_B1[] = {0x00,0xB1};
static char truly_cmd_B0_B1[] = {0xb0,004};


static struct dsi_cmd_desc OTM1281_AUO_CMD_INIT_PARA[] = {
		{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(truly_cmd_FF1),truly_cmd_FF1},
		{DTYPE_GEN_WRITE2, 1, 0, 0, 0, sizeof(truly_shift_FF2),truly_shift_FF2},
		{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(truly_cmd_FF2),truly_cmd_FF2},
		{DTYPE_GEN_WRITE2, 1, 0, 0, 0, sizeof(truly_shift_b0),truly_shift_b0},
		{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(truly_cmd_b0),truly_cmd_b0},
		{DTYPE_GEN_WRITE2, 1, 0, 0, 0, sizeof(truly_shift_C51),truly_shift_C51},
		{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(truly_cmd_C51),truly_cmd_C51},
		{DTYPE_GEN_WRITE2, 1, 0, 0, 0, sizeof(truly_shift_C52),truly_shift_C52},
		{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(truly_cmd_C52),truly_cmd_C52},
		{DTYPE_GEN_WRITE2, 1, 0, 0, 0, sizeof(truly_shift_C53),truly_shift_C53},
		{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(truly_cmd_C53),truly_cmd_C53},
		{DTYPE_GEN_WRITE2, 1, 0, 0, 0, sizeof(truly_shift_D8),truly_shift_D8},
		{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(truly_cmd_D8),truly_cmd_D8},
		{DTYPE_GEN_WRITE2, 1, 0, 0, 0, sizeof(truly_shift_F5),truly_shift_F5},
		{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(truly_cmd_F5),truly_cmd_F5},
/*	add rate to 65hz
		{DTYPE_GEN_WRITE2, 1, 0, 0, 0, sizeof(truly_shift_C1_80),truly_shift_C1_80},
		{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(truly_cmd_C1_80),truly_cmd_C1_80},
		{DTYPE_GEN_WRITE2, 1, 0, 0, 0, sizeof(truly_shift_C5_D0),truly_shift_C5_D0},
		{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(truly_cmd_C5_D0),truly_cmd_C5_D0},
*/
		
		{DTYPE_GEN_WRITE2, 1, 0, 0, 0, sizeof(truly_shift_00),truly_shift_00},
//		{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(truly_cmd_36),truly_cmd_36},

		{DTYPE_GEN_WRITE2, 1, 0, 0, 0, sizeof(truly_shift_83),truly_shift_83},
		{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(truly_cmd_F4),truly_cmd_F4},
		{DTYPE_GEN_WRITE2, 1, 0, 0, 0, sizeof(truly_shift_00),truly_shift_00},
		{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(truly_cmd_D4_00),truly_cmd_D4_00},
		{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(truly_cmd_D4_40),truly_cmd_D4_40},
		{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(truly_cmd_D4_80),truly_cmd_D4_80},
		{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(truly_cmd_D4_120),truly_cmd_D4_120},
		{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(truly_cmd_D4_160),truly_cmd_D4_160},
		{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(truly_cmd_D4_200),truly_cmd_D4_200},
		{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(truly_cmd_D4_240),truly_cmd_D4_240},
		{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(truly_cmd_D4_280),truly_cmd_D4_280},
		{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(truly_cmd_D4_320),truly_cmd_D4_320},
		{DTYPE_GEN_WRITE2, 1, 0, 0, 0, sizeof(truly_shift_00),truly_shift_00},
		{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(truly_cmd_D5_00),truly_cmd_D5_00},
		{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(truly_cmd_D5_40),truly_cmd_D5_40},
		{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(truly_cmd_D5_80),truly_cmd_D5_80},
		{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(truly_cmd_D5_120),truly_cmd_D5_120},
		{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(truly_cmd_D5_200),truly_cmd_D5_200},
		{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(truly_cmd_D5_160),truly_cmd_D5_160},
		{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(truly_cmd_D5_240),truly_cmd_D5_240},
		{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(truly_cmd_D5_280),truly_cmd_D5_280},
		{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(truly_cmd_D5_320),truly_cmd_D5_320},


		{DTYPE_GEN_WRITE2, 1, 0, 0, 0, sizeof(truly_shift_00),truly_shift_00},
		{DTYPE_DCS_WRITE, 1, 0, 0, 0, sizeof(truly_cmd_35),truly_cmd_35},
		{DTYPE_DCS_WRITE, 1, 0, 0, 120, sizeof(exit_sleep),exit_sleep},
	
		{DTYPE_DCS_WRITE, 1, 0, 0, 10, sizeof(display_on), display_on},

		{DTYPE_DCS_WRITE, 1, 0, 0, 0, sizeof(truly_cmd_2c),truly_cmd_2c},
	
		{DTYPE_DCS_WRITE, 1, 0, 0, 0, sizeof(truly_cmd_3c), truly_cmd_3c},
#ifdef USE_CABC_PANEL
		
		{DTYPE_GEN_WRITE1, 1, 0, 0, 0, sizeof(cabc_cmd_C1_51),cabc_cmd_C1_51},	
		{DTYPE_GEN_WRITE1, 1, 0, 0, 0, sizeof(cabc_cmd_C1_53),cabc_cmd_C1_53},
		{DTYPE_GEN_WRITE1, 1, 0, 0, 0, sizeof(cabc_cmd_C1_55),cabc_cmd_C1_55},
	
		{DTYPE_GEN_WRITE1, 1, 0, 0, 0, sizeof(Gen_write_1A_1P_SETTING),Gen_write_1A_1P_SETTING},  
		{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(Gen_write_1A_28P_SETTING),Gen_write_1A_28P_SETTING},
		{DTYPE_GEN_WRITE1, 1, 0, 0, 0, sizeof(Gen_write_1A_1P_CABC1_1),Gen_write_1A_1P_CABC1_1},  
		{DTYPE_GEN_WRITE1, 1, 0, 0, 0, sizeof(Gen_write_1A_1P_CABC1_2),Gen_write_1A_1P_CABC1_2},  
		{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(Gen_write_1A_18P_CABC1),Gen_write_1A_18P_CABC1},	  
		{DTYPE_GEN_WRITE1, 1, 0, 0, 0, sizeof(Gen_write_1A_1P_CABC2_1),Gen_write_1A_1P_CABC2_1},  
		{DTYPE_GEN_WRITE1, 1, 0, 0, 0, sizeof(Gen_write_1A_1P_CABC2_2),Gen_write_1A_1P_CABC2_2},  
		{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(Gen_write_1A_18P_CABC2),Gen_write_1A_18P_CABC2},	  
		{DTYPE_GEN_WRITE1, 1, 0, 0, 0, sizeof(Gen_write_1A_1P_CABC3_1),Gen_write_1A_1P_CABC3_1},  
		{DTYPE_GEN_WRITE1, 1, 0, 0, 0, sizeof(Gen_write_1A_1P_CABC3_2),Gen_write_1A_1P_CABC3_2},  
		{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(Gen_write_1A_18P_CABC3),Gen_write_1A_18P_CABC3},	  
		{DTYPE_GEN_WRITE1, 1, 0, 0, 0, sizeof(Gen_write_1A_1P_CABC4_1),Gen_write_1A_1P_CABC4_1},  
		{DTYPE_GEN_WRITE1, 1, 0, 0, 0, sizeof(Gen_write_1A_1P_CABC4_2),Gen_write_1A_1P_CABC4_2},  
		{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(Gen_write_1A_18P_CABC4),Gen_write_1A_18P_CABC4},	  
		{DTYPE_GEN_WRITE1, 1, 0, 0, 0, sizeof(Gen_write_1A_1P_CABC5_1),Gen_write_1A_1P_CABC5_1},  
		{DTYPE_GEN_WRITE1, 1, 0, 0, 0, sizeof(Gen_write_1A_1P_CABC5_2),Gen_write_1A_1P_CABC5_2},  
		{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(Gen_write_1A_18P_CABC5),Gen_write_1A_18P_CABC5},	  
		{DTYPE_GEN_WRITE1, 1, 0, 0, 0, sizeof(Gen_write_1A_1P_CABC6_1),Gen_write_1A_1P_CABC6_1},  
		{DTYPE_GEN_WRITE1, 1, 0, 0, 0, sizeof(Gen_write_1A_1P_CABC6_2),Gen_write_1A_1P_CABC6_2},  
		{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(Gen_write_1A_18P_CABC6),Gen_write_1A_18P_CABC6},	  
		{DTYPE_GEN_WRITE1, 1, 0, 0, 0, sizeof(Gen_write_1A_1P_CABC7_1),Gen_write_1A_1P_CABC7_1},  
		{DTYPE_GEN_WRITE1, 1, 0, 0, 0, sizeof(Gen_write_1A_1P_CABC7_2),Gen_write_1A_1P_CABC7_2},  
		{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(Gen_write_1A_18P_CABC7),Gen_write_1A_18P_CABC7},	  
		{DTYPE_GEN_WRITE1, 1, 0, 0, 0, sizeof(Gen_write_1A_1P_CABC8_1),Gen_write_1A_1P_CABC8_1},  
		{DTYPE_GEN_WRITE1, 1, 0, 0, 0, sizeof(Gen_write_1A_1P_CABC8_2),Gen_write_1A_1P_CABC8_2},  
		{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(Gen_write_1A_18P_CABC8),Gen_write_1A_18P_CABC8},	  
		{DTYPE_GEN_WRITE1, 1, 0, 0, 0, sizeof(Gen_write_1A_1P_CABC9_1),Gen_write_1A_1P_CABC9_1},  
		{DTYPE_GEN_WRITE1, 1, 0, 0, 0, sizeof(Gen_write_1A_1P_CABC9_2),Gen_write_1A_1P_CABC9_2},  
		{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(Gen_write_1A_18P_CABC9),Gen_write_1A_18P_CABC9},	  
		{DTYPE_GEN_WRITE1, 1, 0, 0, 0, sizeof(Gen_write_1A_1P_CABC10_1),Gen_write_1A_1P_CABC10_1},
		{DTYPE_GEN_WRITE1, 1, 0, 0, 0, sizeof(Gen_write_1A_1P_CABC10_2),Gen_write_1A_1P_CABC10_2},
		{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(Gen_write_1A_18P_CABC10),Gen_write_1A_18P_CABC10},  
		{DTYPE_GEN_WRITE1, 1, 0, 0, 0, sizeof(Gen_write_1A_1P_CABC11_1),Gen_write_1A_1P_CABC11_1},
		{DTYPE_GEN_WRITE1, 1, 0, 0, 0, sizeof(Gen_write_1A_1P_CABC11_2),Gen_write_1A_1P_CABC11_2},
		{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(Gen_write_1A_18P_CABC11),Gen_write_1A_18P_CABC11},  
		{DTYPE_GEN_WRITE1, 1, 0, 0, 0, sizeof(Gen_write_1A_1P_CABC12_1),Gen_write_1A_1P_CABC12_1},
		{DTYPE_GEN_WRITE1, 1, 0, 0, 0, sizeof(Gen_write_1A_1P_CABC12_2),Gen_write_1A_1P_CABC12_2},
		{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(Gen_write_1A_18P_CABC12),Gen_write_1A_18P_CABC12},  
		{DTYPE_GEN_WRITE1, 1, 0, 0, 0, sizeof(Gen_write_1A_1P_CABC13_1),Gen_write_1A_1P_CABC13_1},
		{DTYPE_GEN_WRITE1, 1, 0, 0, 0, sizeof(Gen_write_1A_1P_CABC13_2),Gen_write_1A_1P_CABC13_2},
		{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(Gen_write_1A_18P_CABC13),Gen_write_1A_18P_CABC13},  
		{DTYPE_GEN_WRITE1, 1, 0, 0, 0, sizeof(Gen_write_1A_1P_CABC14_1),Gen_write_1A_1P_CABC14_1},
		{DTYPE_GEN_WRITE1, 1, 0, 0, 0, sizeof(Gen_write_1A_1P_CABC14_2),Gen_write_1A_1P_CABC14_2},
		{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(Gen_write_1A_18P_CABC14),Gen_write_1A_18P_CABC14},  
		{DTYPE_GEN_WRITE1, 1, 0, 0, 0, sizeof(Gen_write_1A_1P_CABC15_1),Gen_write_1A_1P_CABC15_1},
		{DTYPE_GEN_WRITE1, 1, 0, 0, 0, sizeof(Gen_write_1A_1P_CABC15_2),Gen_write_1A_1P_CABC15_2},
		{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(Gen_write_1A_18P_CABC15),Gen_write_1A_18P_CABC15},  
		{DTYPE_GEN_WRITE1, 1, 0, 0, 0, sizeof(Gen_write_1A_1P_CABC16_1),Gen_write_1A_1P_CABC16_1},
		{DTYPE_GEN_WRITE1, 1, 0, 0, 0, sizeof(Gen_write_1A_1P_CABC16_2),Gen_write_1A_1P_CABC16_2},
		{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(Gen_write_1A_18P_CABC16),Gen_write_1A_18P_CABC16},  
		
#endif
		
		{DTYPE_DCS_WRITE, 1, 0, 0, 0, sizeof(truly_shift_B0_80),truly_shift_B0_80},
	
		{DTYPE_DCS_WRITE, 1, 0, 0, 0, sizeof(truly_cmd_B0_80), truly_cmd_B0_80},
		{DTYPE_DCS_WRITE, 1, 0, 0, 0, sizeof(truly_shift_B0_B1),truly_shift_B0_B1},
	
		{DTYPE_DCS_WRITE, 1, 0, 0, 0, sizeof(truly_cmd_B0_B1), truly_cmd_B0_B1},
};

static char cmi_cmd_FF[] = {0xFF, 0x12,0x80,0x01};
static char cmi_shift_FF_80[] = {0x00, 0x80};
static char cmi_cmd_FF_80[] = {0xFF, 0x12,0x80};
static char cmi_shift_B3_A0[] = {0x00,0xa0};
static char cmi_cmd_B3_A0[] = {0xb3,0x38,0x38};

static char cmi_cmd_truly_shift_b0[] = {0x00, 0xb0}; 
static char cmi_cmd_truly_cmd_b0[] = {0xb0, 0x20};

static char cmi_shift_CB_80[] = {0x00,0x80};
static char cmi_cmd_CB_80[] = {0xcb,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
static char cmi_shift_CB_90[] = {0x00,0x90};
static char cmi_cmd_CB_90[] = {0xcb,0x00,0xc0,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
static char cmi_shift_CB_A0[] = {0x00,0xa0};
static char cmi_cmd_CB_A0[] = {0xcb,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
static char cmi_shift_CB_B0[] = {0x00,0xb0};
static char cmi_cmd_CB_B0[] = {0xcb,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
static char cmi_shift_CB_C0[] = {0x00,0xc0};
static char cmi_cmd_CB_C0[] = {0xcb,0x04,0x00,0x0f,0x00,0x00,0x00,0x04,0x04,0x04,0x04,0x04,0x04,0xf4};
static char cmi_shift_CB_D0[] = {0x00,0xd0};
static char cmi_cmd_CB_D0[] = {0xcb,0xf4,0xf4,0x00,0xf4,0x08,0x04,0x04,0x04,0x00,0x00,0x00,0x00,0x00};
static char cmi_shift_CB_E0[] = {0x00,0xe0};
static char cmi_cmd_CB_E0[] = {0xcb,0x55,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x08,0x00,0x00,0x00};
static char cmi_shift_CB_F0[] = {0x00,0xf0};
static char cmi_cmd_CB_F0[] = {0xcb,0x00,0x70,0x01,0x00,0x00};
static char cmi_shift_CC_80[] = {0x00,0x80};
static char cmi_cmd_CC_80[] = {0xcc,0x41,0x42,0x47,0x48,0x49,0x4a,0x4b,0x4c,0x52,0x55,0x43,0x53,0x65,0x51,0x4D,0x4E,0x4F,0x91,0x8D,0x8E,0x8F,0x40,0x40,0x40,0x40};
static char cmi_shift_CC_A0[] = {0x00,0xA0};
static char cmi_cmd_CC_A0[] = {0xcc,0x41,0x42,0x47,0x48,0x4C,0x4B,0x4A,0x49,0x52,0x55,0x43,0x53,0x65,0x51,0x4D,0x4E,0x4F,0x91,0x8D,0x8E,0x8F,0x40,0x40,0x40,0x40,0xFF,0xFF,0xFF,0x01};
static char cmi_shift_CC_C0[] = {0x00,0xC0};
static char cmi_cmd_CC_C0[] = {0xcc,0x41,0x42,0x47,0x48,0x49,0x4A,0x4B,0x4C,0x52,0x55,0x43,0x53,0x54,0x51,0x4D,0x4E,0x4F,0x91,0x8D,0x8E,0x8F,0x40,0x40,0x40,0x40};
static char cmi_shift_CC_E0[] = {0x00,0xE0};
static char cmi_cmd_CC_E0[] = {0xcc,0x41,0x42,0x47,0x48,0x4C,0x4B,0x4A,0x49,0x52,0x55,0x43,0x53,0x54,0x51,0x4D,0x4E,0x4F,0x91,0x8D,0x8E,0x8F,0x40,0x40,0x40,0x40,0xFF,0xFF,0xFF,0x01};
static char cmi_shift_C1_90[] = {0x00,0x90};
static char cmi_cmd_C1_90[] = {0xc1,0x22,0x00,0x00,0x00,0x00};
static char cmi_shift_C0_80[] = {0x00,0x80};
static char cmi_cmd_C0_80[] = {0xc0,0x00,0x87,0x00,0x06,0x0a,0x00,0x87,0x06,0x0a,0x00,0x00,0x00};
static char cmi_shift_C0_90[] = {0x00,0x90};
static char cmi_cmd_C0_90[] = {0xc0,0x00,0x0a,0x00,0x14,0x00,0x2a};
static char cmi_shift_C0_A0[] = {0x00,0xA0};
static char cmi_cmd_C0_A0[] = {0xc0,0x00,0x03,0x01,0x01,0x01,0x01,0x1a,0x03,0x00,0x02};
static char cmi_shift_C2_80[] = {0x00,0x80};
static char cmi_cmd_C2_80[] = {0xc2,0x03,0x02,0x00,0x00,0x00,0x02,0x00,0x22};
static char cmi_shift_C2_90[] = {0x00,0x90};
static char cmi_cmd_C2_90[] = {0xc2,0x03,0x00,0xff,0xff,0x00,0x00,0x00,0x00,0x22};
static char cmi_shift_C2_A0[] = {0x00,0xA0};
static char cmi_cmd_C2_A0[] = {0xc2,0xff,0x00,0xff,0x00,0x00,0x0a,0x00,0x0a};
static char cmi_shift_C2_B0[] = {0x00,0xB0};
static char cmi_cmd_C2_B0[] = {0xc2,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
static char cmi_shift_C2_C0[] = {0x00,0xC0};
static char cmi_cmd_C2_C0[] = {0xc2,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
static char cmi_shift_C2_E0[] = {0x00,0xE0};
static char cmi_cmd_C2_E0[] = {0xc2,0x84,0x00,0x10,0x0d};
static char cmi_shift_C2_B3[] = {0x00,0xB3};
static char cmi_cmd_C2_B3[] = {0xc0,0x0f};
static char cmi_shift_C1_A2[] = {0x00,0xA2};
static char cmi_cmd_C1_A2[] = {0xc1,0xff};
static char cmi_shift_C0_B4[] = {0x00,0xB4};
static char cmi_cmd_C0_B4[] = {0xc0,0x54,0x00};
static char cmi_shift_C5_80[] = {0x00,0x80};
static char cmi_cmd_C5_80[] = {0xc5,0x20,0x07,0x00,0xb0,0xb0,0x00,0x08,0x00};
static char cmi_shift_C5_90[] = {0x00,0x90};
static char cmi_cmd_C5_90[] = {0xc5,0x20,0x85,0x02,0x88,0x96,0x15,0x00,0x08,0x44,0x44,0x44};
#ifdef COLOR_ENHANCE
static char cmi_cmd_D4_00[] = {0xD4,
						 0x00,0x40,0x00,0x40,0xff,0x40,0xff,0x40,0xfe,0x40,0xfe,0x40,0xfe,0x40,0xfd,0x40,0xfd,0x40,0xfc,0x40, 
						 0xfc,0x40,0xfb,0x40,0xfa,0x40,0xfa,0x40,0xf9,0x40,0xf9,0x40,0xf9,0x40,0xf9,0x40,0xfa,0x40,0xfa,0x40
};
static char cmi_cmd_D4_40[] = {0xD4,						 
						 0xfa,0x40,0xfa,0x40,0xfa,0x40,0xfc,0x40,0xfc,0x40,0xfc,0x40,0xfc,0x40,0xfc,0x40,0xfd,0x40,0xfd,0x40,
						 0xfd,0x40,0xfd,0x40,0xfc,0x40,0xfb,0x40,0xfa,0x40,0xfa,0x40,0xf9,0x40,0xf9,0x40,0xf8,0x40,0xf8,0x40
};
static char cmi_cmd_D4_80[] = {0xD4,							 
						 0xf7,0x3f,0xf7,0x3f,0xf6,0x3f,0xf6,0x3f,0xf5,0x3f,0xf5,0x3f,0xf6,0x3f,0xf7,0x3f,0xf7,0x3f,0xf8,0x40,
						 0xf9,0x40,0xf9,0x40,0xfa,0x40,0xfb,0x40,0xfc,0x40,0xfd,0x40,0xfe,0x40,0xfe,0x40,0xff,0x40,0x00,0x40
};
static char cmi_cmd_D4_120[] = {0xD4,							 
						 0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,
						 0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x01,0x40,0x01,0x40,0x01,0x40
};
static char cmi_cmd_D4_160[] = {0xD4,							 
						 0x02,0x40,0x02,0x40,0x02,0x40,0x03,0x40,0x03,0x40,0x03,0x40,0x04,0x40,0x04,0x40,0x04,0x40,0x06,0x40,
						 0x06,0x40,0x06,0x40,0x06,0x40,0x07,0x40,0x07,0x40,0x07,0x40,0x07,0x40,0x07,0x40,0x08,0x40,0x08,0x40
};
static char cmi_cmd_D4_200[] = {0xD4,							 
						 0x08,0x40,0x08,0x40,0x08,0x40,0x09,0x3f,0x09,0x3f,0x09,0x3f,0x08,0x40,0x08,0x40,0x07,0x40,0x07,0x40,
						 0x06,0x40,0x06,0x40,0x04,0x40,0x04,0x40,0x03,0x40,0x03,0x40,0x02,0x40,0x02,0x40,0x01,0x40,0x01,0x40
};
static char cmi_cmd_D4_240[] = {0xD4,							 
						 0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,
						 0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40
};
static char cmi_cmd_D4_280[] = {0xD4,							 
						 0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,
						 0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40
};
static char cmi_cmd_D4_320[] = {0xD4,							 
						 0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,
						 0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40
};
static char cmi_cmd_D5_00[] = {0xD5,
						 0x00,0x7f,0xff,0x7f,0xfe,0x7f,0xfe,0x7f,0xfc,0x7f,0xfc,0x7e,0xfb,0x7e,0xf9,0x7e,0xf9,0x7e,0xf7,0x7e,
						 0xf7,0x7d,0xf6,0x7d,0xf5,0x7d,0xf5,0x7d,0xf3,0x7c,0xf3,0x7c,0xf3,0x7c,0xf3,0x7d,0xf5,0x7d,0xf5,0x7d
};
static char cmi_cmd_D5_40[] = {0xD5,
						 0xf5,0x7d,0xf5,0x7d,0xf5,0x7e,0xf7,0x7e,0xf7,0x7e,0xf7,0x7e,0xf7,0x7e,0xf7,0x7f,0xf9,0x7f,0xf9,0x7f,
						 0xf9,0x7f,0xf8,0x7f,0xf7,0x7f,0xf6,0x7f,0xf5,0x7f,0xf4,0x7f,0xf3,0x7f,0xf2,0x7f,0xf0,0x7e,0xf0,0x7e
};
static char cmi_cmd_D5_80[] = {0xD5,						 
						 0xee,0x7e,0xee,0x7e,0xec,0x7e,0xec,0x7e,0xea,0x7d,0xea,0x7d,0xec,0x7e,0xed,0x7e,0xee,0x7e,0xf0,0x7e,
						 0xf2,0x7f,0xf3,0x7f,0xf5,0x7f,0xf6,0x7f,0xf7,0x7f,0xf9,0x7f,0xfb,0x7f,0xfc,0x7f,0xfe,0x7f,0xff,0x7f
};
static char cmi_cmd_D5_120[] = {0xD5,						 
						 0x00,0x7f,0x00,0x7f,0x00,0x7f,0x00,0x7f,0x00,0x7f,0x00,0x7f,0x00,0x7f,0x00,0x7f,0x00,0x7f,0x00,0x7f,
						 0x00,0x7f,0x00,0x7f,0x00,0x7f,0x00,0x7f,0x00,0x7f,0x00,0x7f,0x00,0x7f,0x02,0x7f,0x02,0x7f,0x02,0x7f
};
static char cmi_cmd_D5_160[] = {0xD5,						 
						 0x04,0x7f,0x04,0x7f,0x04,0x7f,0x07,0x7f,0x07,0x7f,0x07,0x7f,0x09,0x7f,0x09,0x7f,0x09,0x7f,0x0b,0x7f,
						 0x0b,0x7f,0x0b,0x7f,0x0b,0x7f,0x0d,0x7f,0x0d,0x7f,0x0d,0x7f,0x0d,0x7f,0x0d,0x7f,0x10,0x7e,0x10,0x7e
};
static char cmi_cmd_D5_200[] = {0xD5,						 
						 0x10,0x7e,0x10,0x7e,0x10,0x7e,0x12,0x7e,0x12,0x7e,0x12,0x7e,0x10,0x7e,0x10,0x7e,0x0d,0x7f,0x0d,0x7f,
						 0x0b,0x7f,0x0b,0x7f,0x09,0x7f,0x08,0x7f,0x07,0x7f,0x06,0x7f,0x04,0x7f,0x03,0x7f,0x02,0x7f,0x01,0x7f
};
static char cmi_cmd_D5_240[] = {0xD5,						 
						 0x00,0x7f,0x00,0x7f,0x00,0x7f,0x00,0x7f,0x00,0x7f,0x00,0x7f,0x00,0x7f,0x00,0x7f,0x00,0x7f,0x00,0x7f,
						 0x00,0x7f,0x00,0x7f,0x00,0x7f,0x00,0x7f,0x00,0x7f,0x00,0x7f,0x00,0x7f,0x00,0x7f,0x00,0x7e,0x00,0x7e
};
static char cmi_cmd_D5_280[] = {0xD5,						 
						 0x00,0x7d,0x00,0x7d,0x00,0x7d,0x00,0x7c,0x00,0x7c,0x00,0x7c,0x00,0x7b,0x00,0x7b,0x00,0x7a,0x00,0x7a,
						 0x00,0x7a,0x00,0x79,0x00,0x78,0x00,0x77,0x00,0x76,0x00,0x75,0x00,0x74,0x00,0x74,0x00,0x73,0x00,0x72
};
static char cmi_cmd_D5_320[] = {0xD5,						 
						 0x00,0x71,0x00,0x70,0x00,0x6f,0x00,0x6f,0x00,0x6e,0x00,0x6d,0x00,0x6f,0x00,0x70,0x00,0x71,0x00,0x72,
						 0x00,0x74,0x00,0x75,0x00,0x76,0x00,0x77,0x00,0x79,0x00,0x7a,0x00,0x7b,0x00,0x7c,0x00,0x7e,0x00,0x7f
};
#endif
static char cmi_shift_D8_00[] = {0x00,0x00};
static char cmi_cmd_D8_00[] = {0xd8,0x52,0x00,0x52,0x00};
static char cmi_shift_D9_00[] = {0x00,0x00};
static char cmi_cmd_D9_00[] = {0xd9,0x8f,0x73,0x80};
static char cmi_shift_C0_C0[] = {0x00,0xC0};
static char cmi_cmd_C0_C0[] = {0xc0,0x95};
static char cmi_shift_C0_D0[] = {0x00,0xD0};
static char cmi_cmd_C0_D0[] = {0xc0,0x05};
static char cmi_shift_F5_B6[] = {0x00,0xB6};
static char cmi_cmd_F5_B6[] = {0xf5,0x00,0x00};
static char cmi_shift_B3_B0[] = {0x00,0xB0};
static char cmi_cmd_B3_B0[] = {0xb3,0x11};
static char cmi_shift_F5_B0[] = {0x00,0xB0};
static char cmi_cmd_F5_B0[] = {0xf5,0x00,0x20};
static char cmi_shift_F5_B8[] = {0x00,0xB8};
static char cmi_cmd_F5_B8[] = {0xf5,0x0c,0x12};
static char cmi_shift_F5_94[] = {0x00,0x94};
static char cmi_cmd_F5_94[] = {0xf5,0x0a,0x14,0x06,0x17};
static char cmi_shift_F5_A2[] = {0x00,0xA2};
static char cmi_cmd_F5_A2[] = {0xf5,0x0a,0x14,0x07,0x14};
static char cmi_shift_F5_90[] = {0x00,0x90};
static char cmi_cmd_F5_90[] = {0xf5,0x07,0x16,0x07,0x14};
static char cmi_shift_F5_A0[] = {0x00,0xA0};
static char cmi_cmd_F5_A0[] = {0xf5,0x02,0x12,0x0a,0x12,0x07,0x12,0x06,0x12,0x0b,0x12,0x08,0x12};
//static char cmi_cmd_36[] = {0x36,0xd1};
static char cmi_cmd_35[] = {0x35,0x00};
static char cmi_shift_D6_80[] = {0x00,0x80};
static char cmi_cmd_D6_80[] = {0xd6,0x00};
static char cmi_shift_E1_00[] = {0x00,0x00};
static char cmi_cmd_E1_00[] = {0xE1,0x2C,0x2F,0x36,0x3E,0x0B,0x05,0x14,0x09,0x07,0x08,0x09,0x1C,0x05,0x0B,0x11,0x0E,0x0B,0x0B};
static char cmi_shift_E2_00[] = {0x00,0x00};
static char cmi_cmd_E2_00[] = {0xE2,0x2C,0x2F,0x36,0x3E,0x0B,0x05,0x14,0x09,0x07,0x08,0x09,0x1C,0x05,0x0B,0x11,0x0E,0x0B,0x0B};
static char cmi_shift_E3_00[] = {0x00,0x00};
static char cmi_cmd_E3_00[] = {0xE3,0x2C,0x2E,0x35,0x3C,0x0D,0x06,0x16,0x09,0x07,0x08,0x0A,0x1A,0x05,0x0B,0x12,0x0E,0x0B,0x0B};
static char cmi_shift_E4_00[] = {0x00,0x00};
static char cmi_cmd_E4_00[] = {0xE4,0x2C,0x2E,0x35,0x3C,0x0D,0x06,0x16,0x09,0x07,0x08,0x0A,0x1A,0x05,0x0B,0x12,0x0E,0x0B,0x0B};
static char cmi_shift_E5_00[] = {0x00,0x00};
static char cmi_cmd_E5_00[] = {0xE5,0x0E,0x16,0x23,0x2E,0x0E,0x07,0x1C,0x0A,0x08,0x07,0x09,0x19,0x05,0x0C,0x11,0x0D,0x0B,0x0B};
static char cmi_shift_E6_00[] = {0x00,0x00};
static char cmi_cmd_E6_00[] = {0xE6,0x0E,0x16,0x23,0x2E,0x0E,0x07,0x1C,0x0A,0x08,0x07,0x09,0x19,0x05,0x0C,0x11,0x0D,0x0B,0x0B};

static char cmi_shift_2A_4P[]={0x00, 0x00};
static char cmi_cmd_2A_4P[]={0x2A, 0x00, 0x00, 0x02, 0xcf};
static char cmi_shift_2B_4P[]={0x00, 0x00};
static char cmi_cmd_2B_4P[]={0x2b, 0x00, 0x00, 0x04, 0xff};
//static char cmi_shift_C1_80[] = {0x00, 0x80};
//static char cmi_cmd_C1_80[] = {0xc1, 0x85};
//static char cmi_shift_C5_D0[] = {0x00, 0xD0};
//static char cmi_cmd_C5_D0[] = {0xc5,0x38};

static struct dsi_cmd_desc OTM1280_CMI_CMD_INIT_PARA[] = {
		{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(cmi_cmd_FF),cmi_cmd_FF},
		{DTYPE_GEN_WRITE2, 1, 0, 0, 0, sizeof(cmi_shift_FF_80),cmi_shift_FF_80},
		{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(cmi_cmd_FF_80),cmi_cmd_FF_80},
		{DTYPE_GEN_WRITE2, 1, 0, 0, 0, sizeof(cmi_shift_B3_A0),cmi_shift_B3_A0},
		{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(cmi_cmd_B3_A0),cmi_cmd_B3_A0},

		{DTYPE_GEN_WRITE2, 1, 0, 0, 0, sizeof(cmi_cmd_truly_shift_b0),cmi_cmd_truly_shift_b0},
		{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(cmi_cmd_truly_cmd_b0),cmi_cmd_truly_cmd_b0},
		
		{DTYPE_GEN_WRITE2, 1, 0, 0, 0, sizeof(cmi_shift_CB_80),cmi_shift_CB_80},
		{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(cmi_cmd_CB_80),cmi_cmd_CB_80},
		{DTYPE_GEN_WRITE2, 1, 0, 0, 0, sizeof(cmi_shift_CB_90),cmi_shift_CB_90},
		{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(cmi_cmd_CB_90),cmi_cmd_CB_90},
		{DTYPE_GEN_WRITE2, 1, 0, 0, 0, sizeof(cmi_shift_CB_A0),cmi_shift_CB_A0},
		{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(cmi_cmd_CB_A0),cmi_cmd_CB_A0},
		{DTYPE_GEN_WRITE2, 1, 0, 0, 0, sizeof(cmi_shift_CB_B0),cmi_shift_CB_B0},
		{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(cmi_cmd_CB_B0),cmi_cmd_CB_B0},
		{DTYPE_GEN_WRITE2, 1, 0, 0, 0, sizeof(cmi_shift_CB_C0),cmi_shift_CB_C0},
		{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(cmi_cmd_CB_C0),cmi_cmd_CB_C0},
		{DTYPE_GEN_WRITE2, 1, 0, 0, 0, sizeof(cmi_shift_CB_D0),cmi_shift_CB_D0},
		{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(cmi_cmd_CB_D0),cmi_cmd_CB_D0},
		{DTYPE_GEN_WRITE2, 1, 0, 0, 0, sizeof(cmi_shift_CB_E0),cmi_shift_CB_E0},
		{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(cmi_cmd_CB_E0),cmi_cmd_CB_E0},
		{DTYPE_GEN_WRITE2, 1, 0, 0, 0, sizeof(cmi_shift_CB_F0),cmi_shift_CB_F0},
		{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(cmi_cmd_CB_F0),cmi_cmd_CB_F0},
		{DTYPE_GEN_WRITE2, 1, 0, 0, 0, sizeof(cmi_shift_CC_80),cmi_shift_CC_80},
		{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(cmi_cmd_CC_80),cmi_cmd_CC_80},
		{DTYPE_GEN_WRITE2, 1, 0, 0, 0, sizeof(cmi_shift_CC_A0),cmi_shift_CC_A0},
		{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(cmi_cmd_CC_A0),cmi_cmd_CC_A0},
		{DTYPE_GEN_WRITE2, 1, 0, 0, 0, sizeof(cmi_shift_CC_C0),cmi_shift_CC_C0},
		{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(cmi_cmd_CC_C0),cmi_cmd_CC_C0},
		{DTYPE_GEN_WRITE2, 1, 0, 0, 0, sizeof(cmi_shift_CC_E0),cmi_shift_CC_E0},
		{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(cmi_cmd_CC_E0),cmi_cmd_CC_E0},
		{DTYPE_GEN_WRITE2, 1, 0, 0, 0, sizeof(cmi_shift_C1_90),cmi_shift_C1_90},
		{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(cmi_cmd_C1_90),cmi_cmd_C1_90},
		{DTYPE_GEN_WRITE2, 1, 0, 0, 0, sizeof(cmi_shift_C0_80),cmi_shift_C0_80},
		{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(cmi_cmd_C0_80),cmi_cmd_C0_80},
		{DTYPE_GEN_WRITE2, 1, 0, 0, 0, sizeof(cmi_shift_C0_90),cmi_shift_C0_90},
		{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(cmi_cmd_C0_90),cmi_cmd_C0_90},
		{DTYPE_GEN_WRITE2, 1, 0, 0, 0, sizeof(cmi_shift_C0_A0),cmi_shift_C0_A0},
		{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(cmi_cmd_C0_A0),cmi_cmd_C0_A0},
		{DTYPE_GEN_WRITE2, 1, 0, 0, 0, sizeof(cmi_shift_C2_80),cmi_shift_C2_80},
		{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(cmi_cmd_C2_80),cmi_cmd_C2_80},
		{DTYPE_GEN_WRITE2, 1, 0, 0, 0, sizeof(cmi_shift_C2_90),cmi_shift_C2_90},
		{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(cmi_cmd_C2_90),cmi_cmd_C2_90},
		{DTYPE_GEN_WRITE2, 1, 0, 0, 0, sizeof(cmi_shift_C2_A0),cmi_shift_C2_A0},
		{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(cmi_cmd_C2_A0),cmi_cmd_C2_A0},
		{DTYPE_GEN_WRITE2, 1, 0, 0, 0, sizeof(cmi_shift_C2_B0),cmi_shift_C2_B0},
		{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(cmi_cmd_C2_B0),cmi_cmd_C2_B0},
		{DTYPE_GEN_WRITE2, 1, 0, 0, 0, sizeof(cmi_shift_C2_C0),cmi_shift_C2_C0},
		{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(cmi_cmd_C2_C0),cmi_cmd_C2_C0},
		{DTYPE_GEN_WRITE2, 1, 0, 0, 0, sizeof(cmi_shift_C2_E0),cmi_shift_C2_E0},
		{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(cmi_cmd_C2_E0),cmi_cmd_C2_E0},
		{DTYPE_GEN_WRITE2, 1, 0, 0, 0, sizeof(cmi_shift_C2_B3),cmi_shift_C2_B3},
		{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(cmi_cmd_C2_B3),cmi_cmd_C2_B3},
		{DTYPE_GEN_WRITE2, 1, 0, 0, 0, sizeof(cmi_shift_C1_A2),cmi_shift_C1_A2},
		{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(cmi_cmd_C1_A2),cmi_cmd_C1_A2},
		{DTYPE_GEN_WRITE2, 1, 0, 0, 0, sizeof(cmi_shift_C0_B4),cmi_shift_C0_B4},
		{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(cmi_cmd_C0_B4),cmi_cmd_C0_B4},
		{DTYPE_GEN_WRITE2, 1, 0, 0, 0, sizeof(cmi_shift_C5_80),cmi_shift_C5_80},
		{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(cmi_cmd_C5_80),cmi_cmd_C5_80},
		{DTYPE_GEN_WRITE2, 1, 0, 0, 0, sizeof(cmi_shift_C5_90),cmi_shift_C5_90},
		{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(cmi_cmd_C5_90),cmi_cmd_C5_90},
		{DTYPE_GEN_WRITE2, 1, 0, 0, 0, sizeof(cmi_shift_D8_00),cmi_shift_D8_00},
		{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(cmi_cmd_D8_00),cmi_cmd_D8_00},
		{DTYPE_GEN_WRITE2, 1, 0, 0, 0, sizeof(cmi_shift_D9_00),cmi_shift_D9_00},
		{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(cmi_cmd_D9_00),cmi_cmd_D9_00},
		{DTYPE_GEN_WRITE2, 1, 0, 0, 0, sizeof(cmi_shift_C0_C0),cmi_shift_C0_C0},
		{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(cmi_cmd_C0_C0),cmi_cmd_C0_C0},
		{DTYPE_GEN_WRITE2, 1, 0, 0, 0, sizeof(cmi_shift_C0_D0),cmi_shift_C0_D0},
		{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(cmi_cmd_C0_D0),cmi_cmd_C0_D0},
		{DTYPE_GEN_WRITE2, 1, 0, 0, 0, sizeof(cmi_shift_F5_B6),cmi_shift_F5_B6},
		{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(cmi_cmd_F5_B6),cmi_cmd_F5_B6},
		{DTYPE_GEN_WRITE2, 1, 0, 0, 0, sizeof(cmi_shift_B3_B0),cmi_shift_B3_B0},
		{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(cmi_cmd_B3_B0),cmi_cmd_B3_B0},
		{DTYPE_GEN_WRITE2, 1, 0, 0, 0, sizeof(cmi_shift_F5_B0),cmi_shift_F5_B0},
		{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(cmi_cmd_F5_B0),cmi_cmd_F5_B0},
		{DTYPE_GEN_WRITE2, 1, 0, 0, 0, sizeof(cmi_shift_F5_B8),cmi_shift_F5_B8},
		{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(cmi_cmd_F5_B8),cmi_cmd_F5_B8},
		{DTYPE_GEN_WRITE2, 1, 0, 0, 0, sizeof(cmi_shift_F5_94),cmi_shift_F5_94},
		{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(cmi_cmd_F5_94),cmi_cmd_F5_94},
		{DTYPE_GEN_WRITE2, 1, 0, 0, 0, sizeof(cmi_shift_F5_A2),cmi_shift_F5_A2},
		{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(cmi_cmd_F5_A2),cmi_cmd_F5_A2},
		{DTYPE_GEN_WRITE2, 1, 0, 0, 0, sizeof(cmi_shift_F5_90),cmi_shift_F5_90},
		{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(cmi_cmd_F5_90),cmi_cmd_F5_90},
		{DTYPE_GEN_WRITE2, 1, 0, 0, 0, sizeof(cmi_shift_F5_A0),cmi_shift_F5_A0},
		{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(cmi_cmd_F5_A0),cmi_cmd_F5_A0},
		{DTYPE_GEN_WRITE2, 1, 0, 0, 0, sizeof(otm_shift_00),otm_shift_00},
//		{DTYPE_GEN_WRITE2, 1, 0, 0, 0, sizeof(cmi_cmd_36),cmi_cmd_36},
		{DTYPE_GEN_WRITE2, 1, 0, 0, 0, sizeof(cmi_shift_D6_80),cmi_shift_D6_80},
		{DTYPE_GEN_WRITE2, 1, 0, 0, 0, sizeof(cmi_cmd_D6_80),cmi_cmd_D6_80},
		{DTYPE_GEN_WRITE2, 1, 0, 0, 0, sizeof(cmi_shift_E1_00),cmi_shift_E1_00},
		{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(cmi_cmd_E1_00),cmi_cmd_E1_00},
		{DTYPE_GEN_WRITE2, 1, 0, 0, 0, sizeof(cmi_shift_E2_00),cmi_shift_E2_00},
		{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(cmi_cmd_E2_00),cmi_cmd_E2_00},
		{DTYPE_GEN_WRITE2, 1, 0, 0, 0, sizeof(cmi_shift_E3_00),cmi_shift_E3_00},
		{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(cmi_cmd_E3_00),cmi_cmd_E3_00},
		{DTYPE_GEN_WRITE2, 1, 0, 0, 0, sizeof(cmi_shift_E4_00),cmi_shift_E4_00},
		{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(cmi_cmd_E4_00),cmi_cmd_E4_00},
		{DTYPE_GEN_WRITE2, 1, 0, 0, 0, sizeof(cmi_shift_E5_00),cmi_shift_E5_00},
		{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(cmi_cmd_E5_00),cmi_cmd_E5_00},
		{DTYPE_GEN_WRITE2, 1, 0, 0, 0, sizeof(cmi_shift_E6_00),cmi_shift_E6_00},
		{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(cmi_cmd_E6_00),cmi_cmd_E6_00},
		
		{DTYPE_GEN_WRITE2, 1, 0, 0, 0, sizeof(cmi_shift_2A_4P),cmi_shift_2A_4P},
		{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(cmi_cmd_2A_4P), cmi_cmd_2A_4P},
		{DTYPE_GEN_WRITE2, 1, 0, 0, 0, sizeof(cmi_shift_2B_4P),cmi_shift_2B_4P},
		{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(cmi_cmd_2B_4P), cmi_cmd_2B_4P},
/*
		{DTYPE_GEN_WRITE2, 1, 0, 0, 0, sizeof(cmi_shift_C1_80),cmi_shift_C1_80},
		{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(cmi_cmd_C1_80),cmi_cmd_C1_80},
		{DTYPE_GEN_WRITE2, 1, 0, 0, 0, sizeof(cmi_shift_C5_D0),cmi_shift_C5_D0},
		{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(cmi_cmd_C5_D0),cmi_cmd_C5_D0},
*/
#ifdef COLOR_ENHANCE
		{DTYPE_GEN_WRITE2, 1, 0, 0, 0, sizeof(truly_shift_83),truly_shift_83},
		{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(truly_cmd_F4),truly_cmd_F4},
		{DTYPE_GEN_WRITE2, 1, 0, 0, 0, sizeof(otm_shift_00),otm_shift_00},
		{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(cmi_cmd_D4_00),cmi_cmd_D4_00},
		{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(cmi_cmd_D4_40),cmi_cmd_D4_40},
		{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(cmi_cmd_D4_80),cmi_cmd_D4_80},
		{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(cmi_cmd_D4_120),cmi_cmd_D4_120},
		{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(cmi_cmd_D4_160),cmi_cmd_D4_160},
		{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(cmi_cmd_D4_200),cmi_cmd_D4_200},
		{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(cmi_cmd_D4_240),cmi_cmd_D4_240},
		{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(cmi_cmd_D4_280),cmi_cmd_D4_280},
		{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(cmi_cmd_D4_320),cmi_cmd_D4_320},
		{DTYPE_GEN_WRITE2, 1, 0, 0, 0, sizeof(otm_shift_00),otm_shift_00},
		{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(cmi_cmd_D5_00),cmi_cmd_D5_00},
		{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(cmi_cmd_D5_40),cmi_cmd_D5_40},
		{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(cmi_cmd_D5_80),cmi_cmd_D5_80},
		{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(cmi_cmd_D5_120),cmi_cmd_D5_120},
		{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(cmi_cmd_D5_200),cmi_cmd_D5_200},
		{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(cmi_cmd_D5_160),cmi_cmd_D5_160},
		{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(cmi_cmd_D5_240),cmi_cmd_D5_240},
		{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(cmi_cmd_D5_280),cmi_cmd_D5_280},
		{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(cmi_cmd_D5_320),cmi_cmd_D5_320},

#endif	
		{DTYPE_GEN_WRITE2, 1, 0, 0, 0, sizeof(otm_shift_00),otm_shift_00},
		{DTYPE_GEN_WRITE2, 1, 0, 0, 0, sizeof(cmi_cmd_35),cmi_cmd_35},
		
		{DTYPE_DCS_WRITE, 1, 0, 0, 120, sizeof(exit_sleep),exit_sleep},
		{DTYPE_DCS_WRITE, 1, 0, 0, 10, sizeof(display_on), display_on},
};
/**********************AUO(new) NT35590,2013.03.22,zzl***********************************/

	static char AUO_NT35590_para001[] = {0xFF,0xEE};
	static char AUO_NT35590_para002[] = {0x26,0x08};
	static char AUO_NT35590_para003[] = {0x26,0x00};
	static char AUO_NT35590_para004[] = {0xFF,0x00};
	static char AUO_NT35590_para005[] = {0xBA,0x03};
	static char AUO_NT35590_para006[] = {0xC2,0x08};
	static char AUO_NT35590_para007[] = {0xFF,0x01};
	static char AUO_NT35590_para008[] = {0xFB,0x01};
	static char AUO_NT35590_para009[] = {0x00,0x4A};
	static char AUO_NT35590_para010[] = {0x01,0x33};
	static char AUO_NT35590_para011[] = {0x02,0x53};
	static char AUO_NT35590_para012[] = {0x03,0x55};
	static char AUO_NT35590_para013[] = {0x04,0x55};
	static char AUO_NT35590_para014[] = {0x05,0x00};
	static char AUO_NT35590_para015[] = {0x06,0x22};
	static char AUO_NT35590_para016[] = {0x08,0x56};
	static char AUO_NT35590_para017[] = {0x09,0x8F};
	static char AUO_NT35590_para018[] = {0x36,0x73};
	static char AUO_NT35590_para019[] = {0x0B,0xCF};
	static char AUO_NT35590_para020[] = {0x0C,0xCF};
	static char AUO_NT35590_para021[] = {0x0D,0x2A};
	static char AUO_NT35590_para022[] = {0x0E,0x33};
	static char AUO_NT35590_para023[] = {0x11,0x83};
	static char AUO_NT35590_para024[] = {0x12,0x03};
	static char AUO_NT35590_para025[] = {0x0F,0x04};
	static char AUO_NT35590_para026[] = {0xFF,0xEE};
	static char AUO_NT35590_para027[] = {0xFB,0x01};
	static char AUO_NT35590_para028[] = {0x12,0x50};
	static char AUO_NT35590_para029[] = {0x13,0x02};
	static char AUO_NT35590_para030[] = {0x6A,0x60};
	static char AUO_NT35590_para031[] = {0x04,0xAD};
	static char AUO_NT35590_para032[] = {0xFF,0x05};
	static char AUO_NT35590_para033[] = {0xFB,0x01};
	static char AUO_NT35590_para034[] = {0x01,0x00};
	static char AUO_NT35590_para035[] = {0x02,0x88};
	static char AUO_NT35590_para036[] = {0x03,0x88};
	static char AUO_NT35590_para037[] = {0x04,0x88};
	static char AUO_NT35590_para038[] = {0x05,0x30};
	static char AUO_NT35590_para039[] = {0x06,0x33};
	static char AUO_NT35590_para040[] = {0x07,0x01};
	static char AUO_NT35590_para041[] = {0x08,0x00};
	static char AUO_NT35590_para042[] = {0x09,0x46};
	static char AUO_NT35590_para043[] = {0x0A,0x46};
	static char AUO_NT35590_para044[] = {0x0D,0x0B};
	static char AUO_NT35590_para045[] = {0x0E,0x1D};
	static char AUO_NT35590_para046[] = {0x0F,0x08};
	static char AUO_NT35590_para047[] = {0x10,0x53};
	static char AUO_NT35590_para048[] = {0x11,0x00};
	static char AUO_NT35590_para049[] = {0x12,0x00};
	static char AUO_NT35590_para050[] = {0x14,0x01};
	static char AUO_NT35590_para051[] = {0x15,0x00};
	static char AUO_NT35590_para052[] = {0x16,0x05};
	static char AUO_NT35590_para053[] = {0x17,0x00};
	static char AUO_NT35590_para054[] = {0x19,0x6F};
	static char AUO_NT35590_para055[] = {0x1A,0xFF};
	static char AUO_NT35590_para056[] = {0x1B,0x0F};
	static char AUO_NT35590_para057[] = {0x1C,0x00};
	static char AUO_NT35590_para058[] = {0x1D,0x00};
	static char AUO_NT35590_para059[] = {0x1E,0x00};
	static char AUO_NT35590_para060[] = {0x1F,0x07};
	static char AUO_NT35590_para061[] = {0x20,0x00};
	static char AUO_NT35590_para062[] = {0x21,0x00};
	static char AUO_NT35590_para063[] = {0x22,0x55};
	static char AUO_NT35590_para064[] = {0x23,0x4D};
	static char AUO_NT35590_para065[] = {0x6C,0x00};
	static char AUO_NT35590_para066[] = {0x6D,0x00};
	static char AUO_NT35590_para067[] = {0x2D,0x02};
	static char AUO_NT35590_para068[] = {0x83,0x01};
	static char AUO_NT35590_para069[] = {0x9E,0x58};
	static char AUO_NT35590_para070[] = {0x9F,0x68};
	static char AUO_NT35590_para071[] = {0xA0,0x01};
	static char AUO_NT35590_para072[] = {0xA2,0x10};
	static char AUO_NT35590_para073[] = {0xBB,0x0A};
	static char AUO_NT35590_para074[] = {0xBC,0x0A};
	static char AUO_NT35590_para075[] = {0x28,0x01};
	static char AUO_NT35590_para076[] = {0x2F,0x02};
	static char AUO_NT35590_para077[] = {0x32,0x08};
	static char AUO_NT35590_para078[] = {0x33,0xB8};
	static char AUO_NT35590_para079[] = {0x36,0x01};
	static char AUO_NT35590_para080[] = {0x37,0x00};
	static char AUO_NT35590_para081[] = {0x43,0x00};
	static char AUO_NT35590_para082[] = {0x4B,0x21};
	static char AUO_NT35590_para083[] = {0x4C,0x03};
	static char AUO_NT35590_para084[] = {0x50,0x21};
	static char AUO_NT35590_para085[] = {0x51,0x03};
	static char AUO_NT35590_para086[] = {0x58,0x21};
	static char AUO_NT35590_para087[] = {0x59,0x03};
	static char AUO_NT35590_para088[] = {0x5D,0x21};
	static char AUO_NT35590_para089[] = {0x5E,0x03};
	static char AUO_NT35590_para090[] = {0xFF,0x01};
	static char AUO_NT35590_para091[] = {0xFB,0x01};
	static char AUO_NT35590_para092[] = {0x75,0x00};
	static char AUO_NT35590_para093[] = {0x76,0x01};
	static char AUO_NT35590_para094[] = {0x77,0x00};
	static char AUO_NT35590_para095[] = {0x78,0x1F};
	static char AUO_NT35590_para096[] = {0x79,0x00};
	static char AUO_NT35590_para097[] = {0x7A,0x51};
	static char AUO_NT35590_para098[] = {0x7B,0x00};
	static char AUO_NT35590_para099[] = {0x7C,0x6F};
	static char AUO_NT35590_para100[] = {0x7D,0x00};
	static char AUO_NT35590_para101[] = {0x7E,0x8A};
	static char AUO_NT35590_para102[] = {0x7F,0x00};
	static char AUO_NT35590_para103[] = {0x80,0xA2};
	static char AUO_NT35590_para104[] = {0x81,0x00};
	static char AUO_NT35590_para105[] = {0x82,0xB7};
	static char AUO_NT35590_para106[] = {0x83,0x00};
	static char AUO_NT35590_para107[] = {0x84,0xC6};
	static char AUO_NT35590_para108[] = {0x85,0x00};
	static char AUO_NT35590_para109[] = {0x86,0xD6};
	static char AUO_NT35590_para110[] = {0x87,0x01};
	static char AUO_NT35590_para111[] = {0x88,0x0B};
	static char AUO_NT35590_para112[] = {0x89,0x01};
	static char AUO_NT35590_para113[] = {0x8A,0x33};
	static char AUO_NT35590_para114[] = {0x8B,0x01};
	static char AUO_NT35590_para115[] = {0x8C,0x73};
	static char AUO_NT35590_para116[] = {0x8D,0x01};
	static char AUO_NT35590_para117[] = {0x8E,0xA4};
	static char AUO_NT35590_para118[] = {0x8F,0x01};
	static char AUO_NT35590_para119[] = {0x90,0xED};
	static char AUO_NT35590_para120[] = {0x91,0x02};
	static char AUO_NT35590_para121[] = {0x92,0x2A};
	static char AUO_NT35590_para122[] = {0x93,0x02};
	static char AUO_NT35590_para123[] = {0x94,0x2B};
	static char AUO_NT35590_para124[] = {0x95,0x02};
	static char AUO_NT35590_para125[] = {0x96,0x5C};
	static char AUO_NT35590_para126[] = {0x97,0x02};
	static char AUO_NT35590_para127[] = {0x98,0x95};
	static char AUO_NT35590_para128[] = {0x99,0x02};
	static char AUO_NT35590_para129[] = {0x9A,0xB5};
	static char AUO_NT35590_para130[] = {0x9B,0x02};
	static char AUO_NT35590_para131[] = {0x9C,0xE5};
	static char AUO_NT35590_para132[] = {0x9D,0x03};
	static char AUO_NT35590_para133[] = {0x9E,0x06};
	static char AUO_NT35590_para134[] = {0x9F,0x03};
	static char AUO_NT35590_para135[] = {0xA0,0x30};
	static char AUO_NT35590_para136[] = {0xA2,0x03};
	static char AUO_NT35590_para137[] = {0xA3,0x3B};
	static char AUO_NT35590_para138[] = {0xA4,0x03};
	static char AUO_NT35590_para139[] = {0xA5,0x49};
	static char AUO_NT35590_para140[] = {0xA6,0x03};
	static char AUO_NT35590_para141[] = {0xA7,0x51};
	static char AUO_NT35590_para142[] = {0xA9,0x03};
	static char AUO_NT35590_para143[] = {0xAA,0x5C};
	static char AUO_NT35590_para144[] = {0xAB,0x03};
	static char AUO_NT35590_para145[] = {0xAC,0x5D};
	static char AUO_NT35590_para146[] = {0xAD,0x03};
	static char AUO_NT35590_para147[] = {0xAE,0x80};
	static char AUO_NT35590_para148[] = {0xAF,0x03};
	static char AUO_NT35590_para149[] = {0xB0,0xE2};
	static char AUO_NT35590_para150[] = {0xB1,0x03};
	static char AUO_NT35590_para151[] = {0xB2,0xEC};
	static char AUO_NT35590_para152[] = {0xB3,0x00};
	static char AUO_NT35590_para153[] = {0xB4,0x01};
	static char AUO_NT35590_para154[] = {0xB5,0x00};
	static char AUO_NT35590_para155[] = {0xB6,0x1F};
	static char AUO_NT35590_para156[] = {0xB7,0x00};
	static char AUO_NT35590_para157[] = {0xB8,0x51};
	static char AUO_NT35590_para158[] = {0xB9,0x00};
	static char AUO_NT35590_para159[] = {0xBA,0x6F};
	static char AUO_NT35590_para160[] = {0xBB,0x00};
	static char AUO_NT35590_para161[] = {0xBC,0x8A};
	static char AUO_NT35590_para162[] = {0xBD,0x00};
	static char AUO_NT35590_para163[] = {0xBE,0xA2};
	static char AUO_NT35590_para164[] = {0xBF,0x00};
	static char AUO_NT35590_para165[] = {0xC0,0xB7};
	static char AUO_NT35590_para166[] = {0xC1,0x00};
	static char AUO_NT35590_para167[] = {0xC2,0xC6};
	static char AUO_NT35590_para168[] = {0xC3,0x00};
	static char AUO_NT35590_para169[] = {0xC4,0xD6};
	static char AUO_NT35590_para170[] = {0xC5,0x01};
	static char AUO_NT35590_para171[] = {0xC6,0x0B};
	static char AUO_NT35590_para172[] = {0xC7,0x01};
	static char AUO_NT35590_para173[] = {0xC8,0x33};
	static char AUO_NT35590_para174[] = {0xC9,0x01};
	static char AUO_NT35590_para175[] = {0xCA,0x73};
	static char AUO_NT35590_para176[] = {0xCB,0x01};
	static char AUO_NT35590_para177[] = {0xCC,0xA4};
	static char AUO_NT35590_para178[] = {0xCD,0x01};
	static char AUO_NT35590_para179[] = {0xCE,0xED};
	static char AUO_NT35590_para180[] = {0xCF,0x02};
	static char AUO_NT35590_para181[] = {0xD0,0x2A};
	static char AUO_NT35590_para182[] = {0xD1,0x02};
	static char AUO_NT35590_para183[] = {0xD2,0x2B};
	static char AUO_NT35590_para184[] = {0xD3,0x02};
	static char AUO_NT35590_para185[] = {0xD4,0x5C};
	static char AUO_NT35590_para186[] = {0xD5,0x02};
	static char AUO_NT35590_para187[] = {0xD6,0x95};
	static char AUO_NT35590_para188[] = {0xD7,0x02};
	static char AUO_NT35590_para189[] = {0xD8,0xB5};
	static char AUO_NT35590_para190[] = {0xD9,0x02};
	static char AUO_NT35590_para191[] = {0xDA,0xE5};
	static char AUO_NT35590_para192[] = {0xDB,0x03};
	static char AUO_NT35590_para193[] = {0xDC,0x06};
	static char AUO_NT35590_para194[] = {0xDD,0x03};
	static char AUO_NT35590_para195[] = {0xDE,0x30};
	static char AUO_NT35590_para196[] = {0xDF,0x03};
	static char AUO_NT35590_para197[] = {0xE0,0x3B};
	static char AUO_NT35590_para198[] = {0xE1,0x03};
	static char AUO_NT35590_para199[] = {0xE2,0x49};
	static char AUO_NT35590_para200[] = {0xE3,0x03};
	static char AUO_NT35590_para201[] = {0xE4,0x51};
	static char AUO_NT35590_para202[] = {0xE5,0x03};
	static char AUO_NT35590_para203[] = {0xE6,0x5C};
	static char AUO_NT35590_para204[] = {0xE7,0x03};
	static char AUO_NT35590_para205[] = {0xE8,0x5D};
	static char AUO_NT35590_para206[] = {0xE9,0x03};
	static char AUO_NT35590_para207[] = {0xEA,0x80};
	static char AUO_NT35590_para208[] = {0xEB,0x03};
	static char AUO_NT35590_para209[] = {0xEC,0xE2};
	static char AUO_NT35590_para210[] = {0xED,0x03};
	static char AUO_NT35590_para211[] = {0xEE,0xEC};
	static char AUO_NT35590_para212[] = {0xEF,0x00};
	static char AUO_NT35590_para213[] = {0xF0,0x02};
	static char AUO_NT35590_para214[] = {0xF1,0x00};
	static char AUO_NT35590_para215[] = {0xF2,0x19};
	static char AUO_NT35590_para216[] = {0xF3,0x00};
	static char AUO_NT35590_para217[] = {0xF4,0x45};
	static char AUO_NT35590_para218[] = {0xF5,0x00};
	static char AUO_NT35590_para219[] = {0xF6,0x67};
	static char AUO_NT35590_para220[] = {0xF7,0x00};
	static char AUO_NT35590_para221[] = {0xF8,0x81};
	static char AUO_NT35590_para222[] = {0xF9,0x00};
	static char AUO_NT35590_para223[] = {0xFA,0x97};
	static char AUO_NT35590_para224[] = {0xFF,0x02};
	static char AUO_NT35590_para225[] = {0x00,0x00};
	static char AUO_NT35590_para226[] = {0x01,0xAA};
	static char AUO_NT35590_para227[] = {0x02,0x00};
	static char AUO_NT35590_para228[] = {0x03,0xBC};
	static char AUO_NT35590_para229[] = {0x04,0x00};
	static char AUO_NT35590_para230[] = {0x05,0xCC};
	static char AUO_NT35590_para231[] = {0x06,0x01};
	static char AUO_NT35590_para232[] = {0x07,0x01};
	static char AUO_NT35590_para233[] = {0x08,0x01};
	static char AUO_NT35590_para234[] = {0x09,0x2B};
	static char AUO_NT35590_para235[] = {0x0A,0x01};
	static char AUO_NT35590_para236[] = {0x0B,0x6B};
	static char AUO_NT35590_para237[] = {0x0C,0x01};
	static char AUO_NT35590_para238[] = {0x0D,0x9D};
	static char AUO_NT35590_para239[] = {0x0E,0x01};
	static char AUO_NT35590_para240[] = {0x0F,0xEA};
	static char AUO_NT35590_para241[] = {0x10,0x02};
	static char AUO_NT35590_para242[] = {0x11,0x24};
	static char AUO_NT35590_para243[] = {0x12,0x02};
	static char AUO_NT35590_para244[] = {0x13,0x26};
	static char AUO_NT35590_para245[] = {0x14,0x02};
	static char AUO_NT35590_para246[] = {0x15,0x5A};
	static char AUO_NT35590_para247[] = {0x16,0x02};
	static char AUO_NT35590_para248[] = {0x17,0x91};
	static char AUO_NT35590_para249[] = {0x18,0x02};
	static char AUO_NT35590_para250[] = {0x19,0xB4};
	static char AUO_NT35590_para251[] = {0x1A,0x02};
	static char AUO_NT35590_para252[] = {0x1B,0xE3};
	static char AUO_NT35590_para253[] = {0x1C,0x03};
	static char AUO_NT35590_para254[] = {0x1D,0x03};
	static char AUO_NT35590_para255[] = {0x1E,0x03};
	static char AUO_NT35590_para256[] = {0x1F,0x2F};
	static char AUO_NT35590_para257[] = {0x20,0x03};
	static char AUO_NT35590_para258[] = {0x21,0x3D};
	static char AUO_NT35590_para259[] = {0x22,0x03};
	static char AUO_NT35590_para260[] = {0x23,0x4C};
	static char AUO_NT35590_para261[] = {0x24,0x03};
	static char AUO_NT35590_para262[] = {0x25,0x60};
	static char AUO_NT35590_para263[] = {0x26,0x03};
	static char AUO_NT35590_para264[] = {0x27,0x61};
	static char AUO_NT35590_para265[] = {0x28,0x03};
	static char AUO_NT35590_para266[] = {0x29,0x62};
	static char AUO_NT35590_para267[] = {0x2A,0x03};
	static char AUO_NT35590_para268[] = {0x2B,0xCA};
	static char AUO_NT35590_para269[] = {0x2D,0x03};
	static char AUO_NT35590_para270[] = {0x2F,0xF3};
	static char AUO_NT35590_para271[] = {0x30,0x03};
	static char AUO_NT35590_para272[] = {0x31,0xF7};
	static char AUO_NT35590_para273[] = {0x32,0x00};
	static char AUO_NT35590_para274[] = {0x33,0x02};
	static char AUO_NT35590_para275[] = {0x34,0x00};
	static char AUO_NT35590_para276[] = {0x35,0x19};
	static char AUO_NT35590_para277[] = {0x36,0x00};
	static char AUO_NT35590_para278[] = {0x37,0x45};
	static char AUO_NT35590_para279[] = {0x38,0x00};
	static char AUO_NT35590_para280[] = {0x39,0x67};
	static char AUO_NT35590_para281[] = {0x3A,0x00};
	static char AUO_NT35590_para282[] = {0x3B,0x81};
	static char AUO_NT35590_para283[] = {0x3D,0x00};
	static char AUO_NT35590_para284[] = {0x3F,0x97};
	static char AUO_NT35590_para285[] = {0x40,0x00};
	static char AUO_NT35590_para286[] = {0x41,0xAA};
	static char AUO_NT35590_para287[] = {0x42,0x00};
	static char AUO_NT35590_para288[] = {0x43,0xBC};
	static char AUO_NT35590_para289[] = {0x44,0x00};
	static char AUO_NT35590_para290[] = {0x45,0xCC};
	static char AUO_NT35590_para291[] = {0x46,0x01};
	static char AUO_NT35590_para292[] = {0x47,0x01};
	static char AUO_NT35590_para293[] = {0x48,0x01};
	static char AUO_NT35590_para294[] = {0x49,0x2B};
	static char AUO_NT35590_para295[] = {0x4A,0x01};
	static char AUO_NT35590_para296[] = {0x4B,0x6B};
	static char AUO_NT35590_para297[] = {0x4C,0x01};
	static char AUO_NT35590_para298[] = {0x4D,0x9D};
	static char AUO_NT35590_para299[] = {0x4E,0x01};
	static char AUO_NT35590_para300[] = {0x4F,0xEA};
	static char AUO_NT35590_para301[] = {0x50,0x02};
	static char AUO_NT35590_para302[] = {0x51,0x24};
	static char AUO_NT35590_para303[] = {0x52,0x02};
	static char AUO_NT35590_para304[] = {0x53,0x26};
	static char AUO_NT35590_para305[] = {0x54,0x02};
	static char AUO_NT35590_para306[] = {0x55,0x5A};
	static char AUO_NT35590_para307[] = {0x56,0x02};
	static char AUO_NT35590_para308[] = {0x58,0x91};
	static char AUO_NT35590_para309[] = {0x59,0x02};
	static char AUO_NT35590_para310[] = {0x5A,0xB4};
	static char AUO_NT35590_para311[] = {0x5B,0x02};
	static char AUO_NT35590_para312[] = {0x5C,0xE3};
	static char AUO_NT35590_para313[] = {0x5D,0x03};
	static char AUO_NT35590_para314[] = {0x5E,0x03};
	static char AUO_NT35590_para315[] = {0x5F,0x03};
	static char AUO_NT35590_para316[] = {0x60,0x2F};
	static char AUO_NT35590_para317[] = {0x61,0x03};
	static char AUO_NT35590_para318[] = {0x62,0x3D};
	static char AUO_NT35590_para319[] = {0x63,0x03};
	static char AUO_NT35590_para320[] = {0x64,0x4C};
	static char AUO_NT35590_para321[] = {0x65,0x03};
	static char AUO_NT35590_para322[] = {0x66,0x60};
	static char AUO_NT35590_para323[] = {0x67,0x03};
	static char AUO_NT35590_para324[] = {0x68,0x61};
	static char AUO_NT35590_para325[] = {0x69,0x03};
	static char AUO_NT35590_para326[] = {0x6A,0x62};
	static char AUO_NT35590_para327[] = {0x6B,0x03};
	static char AUO_NT35590_para328[] = {0x6C,0xCA};
	static char AUO_NT35590_para329[] = {0x6D,0x03};
	static char AUO_NT35590_para330[] = {0x6E,0xF3};
	static char AUO_NT35590_para331[] = {0x6F,0x03};
	static char AUO_NT35590_para332[] = {0x70,0xF7};
	static char AUO_NT35590_para333[] = {0x71,0x00};
	static char AUO_NT35590_para334[] = {0x72,0x26};
	static char AUO_NT35590_para335[] = {0x73,0x00};
	static char AUO_NT35590_para336[] = {0x74,0x35};
	static char AUO_NT35590_para337[] = {0x75,0x00};
	static char AUO_NT35590_para338[] = {0x76,0x58};
	static char AUO_NT35590_para339[] = {0x77,0x00};
	static char AUO_NT35590_para340[] = {0x78,0x72};
	static char AUO_NT35590_para341[] = {0x79,0x00};
	static char AUO_NT35590_para342[] = {0x7A,0x89};
	static char AUO_NT35590_para343[] = {0x7B,0x00};
	static char AUO_NT35590_para344[] = {0x7C,0x9D};
	static char AUO_NT35590_para345[] = {0x7D,0x00};
	static char AUO_NT35590_para346[] = {0x7E,0xAF};
	static char AUO_NT35590_para347[] = {0x7F,0x00};
	static char AUO_NT35590_para348[] = {0x80,0xC0};
	static char AUO_NT35590_para349[] = {0x81,0x00};
	static char AUO_NT35590_para350[] = {0x82,0xCF};
	static char AUO_NT35590_para351[] = {0x83,0x01};
	static char AUO_NT35590_para352[] = {0x84,0x03};
	static char AUO_NT35590_para353[] = {0x85,0x01};
	static char AUO_NT35590_para354[] = {0x86,0x2B};
	static char AUO_NT35590_para355[] = {0x87,0x01};
	static char AUO_NT35590_para356[] = {0x88,0x6B};
	static char AUO_NT35590_para357[] = {0x89,0x01};
	static char AUO_NT35590_para358[] = {0x8A,0x9D};
	static char AUO_NT35590_para359[] = {0x8B,0x01};
	static char AUO_NT35590_para360[] = {0x8C,0xEA};
	static char AUO_NT35590_para361[] = {0x8D,0x02};
	static char AUO_NT35590_para362[] = {0x8E,0x25};
	static char AUO_NT35590_para363[] = {0x8F,0x02};
	static char AUO_NT35590_para364[] = {0x90,0x27};
	static char AUO_NT35590_para365[] = {0x91,0x02};
	static char AUO_NT35590_para366[] = {0x92,0x5B};
	static char AUO_NT35590_para367[] = {0x93,0x02};
	static char AUO_NT35590_para368[] = {0x94,0x92};
	static char AUO_NT35590_para369[] = {0x95,0x02};
	static char AUO_NT35590_para370[] = {0x96,0xB6};
	static char AUO_NT35590_para371[] = {0x97,0x02};
	static char AUO_NT35590_para372[] = {0x98,0xE8};
	static char AUO_NT35590_para373[] = {0x99,0x03};
	static char AUO_NT35590_para374[] = {0x9A,0x0E};
	static char AUO_NT35590_para375[] = {0x9B,0x03};
	static char AUO_NT35590_para376[] = {0x9C,0x47};
	static char AUO_NT35590_para377[] = {0x9D,0x03};
	static char AUO_NT35590_para378[] = {0x9E,0x5F};
	static char AUO_NT35590_para379[] = {0x9F,0x03};
	static char AUO_NT35590_para380[] = {0xA0,0x87};
	static char AUO_NT35590_para381[] = {0xA2,0x03};
	static char AUO_NT35590_para382[] = {0xA3,0xF2};
	static char AUO_NT35590_para383[] = {0xA4,0x03};
	static char AUO_NT35590_para384[] = {0xA5,0xFB};
	static char AUO_NT35590_para385[] = {0xA6,0x03};
	static char AUO_NT35590_para386[] = {0xA7,0xFC};
	static char AUO_NT35590_para387[] = {0xA9,0x03};
	static char AUO_NT35590_para388[] = {0xAA,0xFD};
	static char AUO_NT35590_para389[] = {0xAB,0x03};
	static char AUO_NT35590_para390[] = {0xAC,0xFE};
	static char AUO_NT35590_para391[] = {0xAD,0x03};
	static char AUO_NT35590_para392[] = {0xAE,0xFE};
	static char AUO_NT35590_para393[] = {0xAF,0x00};
	static char AUO_NT35590_para394[] = {0xB0,0x26};
	static char AUO_NT35590_para395[] = {0xB1,0x00};
	static char AUO_NT35590_para396[] = {0xB2,0x35};
	static char AUO_NT35590_para397[] = {0xB3,0x00};
	static char AUO_NT35590_para398[] = {0xB4,0x58};
	static char AUO_NT35590_para399[] = {0xB5,0x00};
	static char AUO_NT35590_para400[] = {0xB6,0x72};
	static char AUO_NT35590_para401[] = {0xB7,0x00};
	static char AUO_NT35590_para402[] = {0xB8,0x89};
	static char AUO_NT35590_para403[] = {0xB9,0x00};
	static char AUO_NT35590_para404[] = {0xBA,0x9D};
	static char AUO_NT35590_para405[] = {0xBB,0x00};
	static char AUO_NT35590_para406[] = {0xBC,0xAF};
	static char AUO_NT35590_para407[] = {0xBD,0x00};
	static char AUO_NT35590_para408[] = {0xBE,0xC0};
	static char AUO_NT35590_para409[] = {0xBF,0x00};
	static char AUO_NT35590_para410[] = {0xC0,0xCF};
	static char AUO_NT35590_para411[] = {0xC1,0x01};
	static char AUO_NT35590_para412[] = {0xC2,0x03};
	static char AUO_NT35590_para413[] = {0xC3,0x01};
	static char AUO_NT35590_para414[] = {0xC4,0x2B};
	static char AUO_NT35590_para415[] = {0xC5,0x01};
	static char AUO_NT35590_para416[] = {0xC6,0x6B};
	static char AUO_NT35590_para417[] = {0xC7,0x01};
	static char AUO_NT35590_para418[] = {0xC8,0x9D};
	static char AUO_NT35590_para419[] = {0xC9,0x01};
	static char AUO_NT35590_para420[] = {0xCA,0xEA};
	static char AUO_NT35590_para421[] = {0xCB,0x02};
	static char AUO_NT35590_para422[] = {0xCC,0x25};
	static char AUO_NT35590_para423[] = {0xCD,0x02};
	static char AUO_NT35590_para424[] = {0xCE,0x27};
	static char AUO_NT35590_para425[] = {0xCF,0x02};
	static char AUO_NT35590_para426[] = {0xD0,0x5B};
	static char AUO_NT35590_para427[] = {0xD1,0x02};
	static char AUO_NT35590_para428[] = {0xD2,0x92};
	static char AUO_NT35590_para429[] = {0xD3,0x02};
	static char AUO_NT35590_para430[] = {0xD4,0xB6};
	static char AUO_NT35590_para431[] = {0xD5,0x02};
	static char AUO_NT35590_para432[] = {0xD6,0xE8};
	static char AUO_NT35590_para433[] = {0xD7,0x03};
	static char AUO_NT35590_para434[] = {0xD8,0x0E};
	static char AUO_NT35590_para435[] = {0xD9,0x03};
	static char AUO_NT35590_para436[] = {0xDA,0x47};
	static char AUO_NT35590_para437[] = {0xDB,0x03};
	static char AUO_NT35590_para438[] = {0xDC,0x5F};
	static char AUO_NT35590_para439[] = {0xDD,0x03};
	static char AUO_NT35590_para440[] = {0xDE,0x87};
	static char AUO_NT35590_para441[] = {0xDF,0x03};
	static char AUO_NT35590_para442[] = {0xE0,0xF2};
	static char AUO_NT35590_para443[] = {0xE1,0x03};
	static char AUO_NT35590_para444[] = {0xE2,0xFB};
	static char AUO_NT35590_para445[] = {0xE3,0x03};
	static char AUO_NT35590_para446[] = {0xE4,0xFC};
	static char AUO_NT35590_para447[] = {0xE5,0x03};
	static char AUO_NT35590_para448[] = {0xE6,0xFD};
	static char AUO_NT35590_para449[] = {0xE7,0x03};
	static char AUO_NT35590_para450[] = {0xE8,0xFE};
	static char AUO_NT35590_para451[] = {0xE9,0x03};
	static char AUO_NT35590_para452[] = {0xEA,0xFE};
	static char AUO_NT35590_para453[] = {0xFF,0x00};
	static char AUO_NT35590_para454[] = {0xFB,0x01};
	static char AUO_NT35590_para455[] = {0xFF,0x05};//
	static char AUO_NT35590_para456[] = {0x19,0x7F};
	static char AUO_NT35590_para457[] = {0xFF,0x00};
	
	static char AUO_NT35590_para_TE[] = {0x35,0x00};
	static char AUO_NT35590_para_TE1[] = {0x44,0x01,0xe0};
	
#ifdef USE_CABC_PANEL
	static char AUO_NT35590_para_cabc2[] = {0x55,0x02};
	static char AUO_NT35590_para_cabc3[] = {0x53,0x2c};
#endif


static struct dsi_cmd_desc AUO_NT35590_CMD_INIT_PARA[] = {
	
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para001), AUO_NT35590_para001},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para002), AUO_NT35590_para002},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para003), AUO_NT35590_para003},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para004), AUO_NT35590_para004},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para005), AUO_NT35590_para005},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para006), AUO_NT35590_para006},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para007), AUO_NT35590_para007},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para008), AUO_NT35590_para008},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para009), AUO_NT35590_para009},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para010), AUO_NT35590_para010},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para011), AUO_NT35590_para011},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para012), AUO_NT35590_para012},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para013), AUO_NT35590_para013},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para014), AUO_NT35590_para014},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para015), AUO_NT35590_para015},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para016), AUO_NT35590_para016},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para017), AUO_NT35590_para017},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para018), AUO_NT35590_para018},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para019), AUO_NT35590_para019},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para020), AUO_NT35590_para020},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para021), AUO_NT35590_para021},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para022), AUO_NT35590_para022},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para023), AUO_NT35590_para023},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para024), AUO_NT35590_para024},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para025), AUO_NT35590_para025},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para026), AUO_NT35590_para026},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para027), AUO_NT35590_para027},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para028), AUO_NT35590_para028},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para029), AUO_NT35590_para029},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para030), AUO_NT35590_para030},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para031), AUO_NT35590_para031},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para032), AUO_NT35590_para032},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para033), AUO_NT35590_para033},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para034), AUO_NT35590_para034},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para035), AUO_NT35590_para035},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para036), AUO_NT35590_para036},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para037), AUO_NT35590_para037},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para038), AUO_NT35590_para038},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para039), AUO_NT35590_para039},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para040), AUO_NT35590_para040},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para041), AUO_NT35590_para041},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para042), AUO_NT35590_para042},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para043), AUO_NT35590_para043},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para044), AUO_NT35590_para044},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para045), AUO_NT35590_para045},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para046), AUO_NT35590_para046},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para047), AUO_NT35590_para047},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para048), AUO_NT35590_para048},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para049), AUO_NT35590_para049},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para050), AUO_NT35590_para050},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para051), AUO_NT35590_para051},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para052), AUO_NT35590_para052},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para053), AUO_NT35590_para053},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para054), AUO_NT35590_para054},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para055), AUO_NT35590_para055},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para056), AUO_NT35590_para056},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para057), AUO_NT35590_para057},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para058), AUO_NT35590_para058},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para059), AUO_NT35590_para059},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para060), AUO_NT35590_para060},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para061), AUO_NT35590_para061},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para062), AUO_NT35590_para062},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para063), AUO_NT35590_para063},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para064), AUO_NT35590_para064},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para065), AUO_NT35590_para065},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para066), AUO_NT35590_para066},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para067), AUO_NT35590_para067},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para068), AUO_NT35590_para068},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para069), AUO_NT35590_para069},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para070), AUO_NT35590_para070},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para071), AUO_NT35590_para071},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para072), AUO_NT35590_para072},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para073), AUO_NT35590_para073},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para074), AUO_NT35590_para074},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para075), AUO_NT35590_para075},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para076), AUO_NT35590_para076},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para077), AUO_NT35590_para077},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para078), AUO_NT35590_para078},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para079), AUO_NT35590_para079},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para080), AUO_NT35590_para080},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para081), AUO_NT35590_para081},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para082), AUO_NT35590_para082},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para083), AUO_NT35590_para083},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para084), AUO_NT35590_para084},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para085), AUO_NT35590_para085},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para086), AUO_NT35590_para086},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para087), AUO_NT35590_para087},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para088), AUO_NT35590_para088},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para089), AUO_NT35590_para089},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para090), AUO_NT35590_para090},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para091), AUO_NT35590_para091},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para092), AUO_NT35590_para092},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para093), AUO_NT35590_para093},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para094), AUO_NT35590_para094},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para095), AUO_NT35590_para095},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para096), AUO_NT35590_para096},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para097), AUO_NT35590_para097},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para098), AUO_NT35590_para098},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para099), AUO_NT35590_para099},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para100), AUO_NT35590_para100},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para101), AUO_NT35590_para101},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para102), AUO_NT35590_para102},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para103), AUO_NT35590_para103},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para104), AUO_NT35590_para104},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para105), AUO_NT35590_para105},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para106), AUO_NT35590_para106},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para107), AUO_NT35590_para107},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para108), AUO_NT35590_para108},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para109), AUO_NT35590_para109},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para110), AUO_NT35590_para110},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para111), AUO_NT35590_para111},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para112), AUO_NT35590_para112},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para113), AUO_NT35590_para113},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para114), AUO_NT35590_para114},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para115), AUO_NT35590_para115},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para116), AUO_NT35590_para116},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para117), AUO_NT35590_para117},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para118), AUO_NT35590_para118},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para119), AUO_NT35590_para119},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para120), AUO_NT35590_para120},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para121), AUO_NT35590_para121},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para122), AUO_NT35590_para122},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para123), AUO_NT35590_para123},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para124), AUO_NT35590_para124},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para125), AUO_NT35590_para125},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para126), AUO_NT35590_para126},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para127), AUO_NT35590_para127},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para128), AUO_NT35590_para128},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para129), AUO_NT35590_para129},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para130), AUO_NT35590_para130},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para131), AUO_NT35590_para131},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para132), AUO_NT35590_para132},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para133), AUO_NT35590_para133},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para134), AUO_NT35590_para134},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para135), AUO_NT35590_para135},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para136), AUO_NT35590_para136},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para137), AUO_NT35590_para137},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para138), AUO_NT35590_para138},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para139), AUO_NT35590_para139},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para140), AUO_NT35590_para140},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para141), AUO_NT35590_para141},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para142), AUO_NT35590_para142},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para143), AUO_NT35590_para143},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para144), AUO_NT35590_para144},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para145), AUO_NT35590_para145},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para146), AUO_NT35590_para146},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para147), AUO_NT35590_para147},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para148), AUO_NT35590_para148},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para149), AUO_NT35590_para149},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para150), AUO_NT35590_para150},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para151), AUO_NT35590_para151},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para152), AUO_NT35590_para152},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para153), AUO_NT35590_para153},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para154), AUO_NT35590_para154},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para155), AUO_NT35590_para155},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para156), AUO_NT35590_para156},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para157), AUO_NT35590_para157},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para158), AUO_NT35590_para158},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para159), AUO_NT35590_para159},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para160), AUO_NT35590_para160},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para161), AUO_NT35590_para161},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para162), AUO_NT35590_para162},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para163), AUO_NT35590_para163},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para164), AUO_NT35590_para164},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para165), AUO_NT35590_para165},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para166), AUO_NT35590_para166},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para167), AUO_NT35590_para167},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para168), AUO_NT35590_para168},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para169), AUO_NT35590_para169},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para170), AUO_NT35590_para170},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para171), AUO_NT35590_para171},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para172), AUO_NT35590_para172},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para173), AUO_NT35590_para173},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para174), AUO_NT35590_para174},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para175), AUO_NT35590_para175},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para176), AUO_NT35590_para176},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para177), AUO_NT35590_para177},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para178), AUO_NT35590_para178},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para179), AUO_NT35590_para179},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para180), AUO_NT35590_para180},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para181), AUO_NT35590_para181},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para182), AUO_NT35590_para182},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para183), AUO_NT35590_para183},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para184), AUO_NT35590_para184},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para185), AUO_NT35590_para185},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para186), AUO_NT35590_para186},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para187), AUO_NT35590_para187},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para188), AUO_NT35590_para188},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para189), AUO_NT35590_para189},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para190), AUO_NT35590_para190},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para191), AUO_NT35590_para191},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para192), AUO_NT35590_para192},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para193), AUO_NT35590_para193},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para194), AUO_NT35590_para194},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para195), AUO_NT35590_para195},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para196), AUO_NT35590_para196},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para197), AUO_NT35590_para197},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para198), AUO_NT35590_para198},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para199), AUO_NT35590_para199},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para200), AUO_NT35590_para200},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para201), AUO_NT35590_para201},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para202), AUO_NT35590_para202},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para203), AUO_NT35590_para203},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para204), AUO_NT35590_para204},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para205), AUO_NT35590_para205},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para206), AUO_NT35590_para206},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para207), AUO_NT35590_para207},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para208), AUO_NT35590_para208},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para209), AUO_NT35590_para209},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para210), AUO_NT35590_para210},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para211), AUO_NT35590_para211},	
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para212), AUO_NT35590_para212},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para213), AUO_NT35590_para213},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para214), AUO_NT35590_para214},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para215), AUO_NT35590_para215},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para216), AUO_NT35590_para216},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para217), AUO_NT35590_para217},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para218), AUO_NT35590_para218},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para219), AUO_NT35590_para219},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para220), AUO_NT35590_para220},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para221), AUO_NT35590_para221},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para222), AUO_NT35590_para222},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para223), AUO_NT35590_para223},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para224), AUO_NT35590_para224},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para225), AUO_NT35590_para225},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para226), AUO_NT35590_para226},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para227), AUO_NT35590_para227},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para228), AUO_NT35590_para228},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para229), AUO_NT35590_para229},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para230), AUO_NT35590_para230},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para231), AUO_NT35590_para231},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para232), AUO_NT35590_para232},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para233), AUO_NT35590_para233},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para234), AUO_NT35590_para234},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para235), AUO_NT35590_para235},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para236), AUO_NT35590_para236},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para237), AUO_NT35590_para237},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para238), AUO_NT35590_para238},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para239), AUO_NT35590_para239},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para240), AUO_NT35590_para240},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para241), AUO_NT35590_para241},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para242), AUO_NT35590_para242},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para243), AUO_NT35590_para243},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para244), AUO_NT35590_para244},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para245), AUO_NT35590_para245},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para246), AUO_NT35590_para246},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para247), AUO_NT35590_para247},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para248), AUO_NT35590_para248},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para249), AUO_NT35590_para249},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para250), AUO_NT35590_para250},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para251), AUO_NT35590_para251},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para252), AUO_NT35590_para252},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para253), AUO_NT35590_para253},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para254), AUO_NT35590_para254},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para255), AUO_NT35590_para255},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para256), AUO_NT35590_para256},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para257), AUO_NT35590_para257},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para258), AUO_NT35590_para258},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para259), AUO_NT35590_para259},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para260), AUO_NT35590_para260},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para261), AUO_NT35590_para261},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para262), AUO_NT35590_para262},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para263), AUO_NT35590_para263},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para264), AUO_NT35590_para264},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para265), AUO_NT35590_para265},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para266), AUO_NT35590_para266},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para267), AUO_NT35590_para267},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para268), AUO_NT35590_para268},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para269), AUO_NT35590_para269},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para270), AUO_NT35590_para270},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para271), AUO_NT35590_para271},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para272), AUO_NT35590_para272},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para273), AUO_NT35590_para273},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para274), AUO_NT35590_para274},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para275), AUO_NT35590_para275},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para276), AUO_NT35590_para276},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para277), AUO_NT35590_para277},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para278), AUO_NT35590_para278},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para279), AUO_NT35590_para279},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para280), AUO_NT35590_para280},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para281), AUO_NT35590_para281},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para282), AUO_NT35590_para282},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para283), AUO_NT35590_para283},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para284), AUO_NT35590_para284},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para285), AUO_NT35590_para285},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para286), AUO_NT35590_para286},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para287), AUO_NT35590_para287},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para288), AUO_NT35590_para288},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para289), AUO_NT35590_para289},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para290), AUO_NT35590_para290},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para291), AUO_NT35590_para291},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para292), AUO_NT35590_para292},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para293), AUO_NT35590_para293},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para294), AUO_NT35590_para294},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para295), AUO_NT35590_para295},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para296), AUO_NT35590_para296},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para297), AUO_NT35590_para297},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para298), AUO_NT35590_para298},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para299), AUO_NT35590_para299},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para300), AUO_NT35590_para300},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para301), AUO_NT35590_para301},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para302), AUO_NT35590_para302},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para303), AUO_NT35590_para303},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para304), AUO_NT35590_para304},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para305), AUO_NT35590_para305},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para306), AUO_NT35590_para306},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para307), AUO_NT35590_para307},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para308), AUO_NT35590_para308},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para309), AUO_NT35590_para309},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para310), AUO_NT35590_para310},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para311), AUO_NT35590_para311},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para312), AUO_NT35590_para312},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para313), AUO_NT35590_para313},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para314), AUO_NT35590_para314},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para315), AUO_NT35590_para315},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para316), AUO_NT35590_para316},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para317), AUO_NT35590_para317},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para318), AUO_NT35590_para318},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para319), AUO_NT35590_para319},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para320), AUO_NT35590_para320},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para321), AUO_NT35590_para321},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para322), AUO_NT35590_para322},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para323), AUO_NT35590_para323},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para324), AUO_NT35590_para324},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para325), AUO_NT35590_para325},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para326), AUO_NT35590_para326},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para327), AUO_NT35590_para327},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para328), AUO_NT35590_para328},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para329), AUO_NT35590_para329},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para330), AUO_NT35590_para330},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para331), AUO_NT35590_para331},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para332), AUO_NT35590_para332},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para333), AUO_NT35590_para333},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para334), AUO_NT35590_para334},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para335), AUO_NT35590_para335},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para336), AUO_NT35590_para336},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para337), AUO_NT35590_para337},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para338), AUO_NT35590_para338},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para339), AUO_NT35590_para339},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para340), AUO_NT35590_para340},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para341), AUO_NT35590_para341},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para342), AUO_NT35590_para342},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para343), AUO_NT35590_para343},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para344), AUO_NT35590_para344},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para345), AUO_NT35590_para345},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para346), AUO_NT35590_para346},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para347), AUO_NT35590_para347},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para348), AUO_NT35590_para348},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para349), AUO_NT35590_para349},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para350), AUO_NT35590_para350},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para351), AUO_NT35590_para351},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para352), AUO_NT35590_para352},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para353), AUO_NT35590_para353},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para354), AUO_NT35590_para354},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para355), AUO_NT35590_para355},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para356), AUO_NT35590_para356},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para357), AUO_NT35590_para357},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para358), AUO_NT35590_para358},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para359), AUO_NT35590_para359},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para360), AUO_NT35590_para360},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para361), AUO_NT35590_para361},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para362), AUO_NT35590_para362},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para363), AUO_NT35590_para363},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para364), AUO_NT35590_para364},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para365), AUO_NT35590_para365},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para366), AUO_NT35590_para366},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para367), AUO_NT35590_para367},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para368), AUO_NT35590_para368},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para369), AUO_NT35590_para369},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para370), AUO_NT35590_para370},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para371), AUO_NT35590_para371},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para372), AUO_NT35590_para372},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para373), AUO_NT35590_para373},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para374), AUO_NT35590_para374},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para375), AUO_NT35590_para375},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para376), AUO_NT35590_para376},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para377), AUO_NT35590_para377},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para378), AUO_NT35590_para378},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para379), AUO_NT35590_para379},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para380), AUO_NT35590_para380},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para381), AUO_NT35590_para381},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para382), AUO_NT35590_para382},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para383), AUO_NT35590_para383},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para384), AUO_NT35590_para384},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para385), AUO_NT35590_para385},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para386), AUO_NT35590_para386},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para387), AUO_NT35590_para387},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para388), AUO_NT35590_para388},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para389), AUO_NT35590_para389},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para390), AUO_NT35590_para390},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para391), AUO_NT35590_para391},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para392), AUO_NT35590_para392},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para393), AUO_NT35590_para393},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para394), AUO_NT35590_para394},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para395), AUO_NT35590_para395},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para396), AUO_NT35590_para396},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para397), AUO_NT35590_para397},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para398), AUO_NT35590_para398},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para399), AUO_NT35590_para399},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para400), AUO_NT35590_para400},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para401), AUO_NT35590_para401},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para402), AUO_NT35590_para402},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para403), AUO_NT35590_para403},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para404), AUO_NT35590_para404},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para405), AUO_NT35590_para405},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para406), AUO_NT35590_para406},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para407), AUO_NT35590_para407},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para408), AUO_NT35590_para408},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para409), AUO_NT35590_para409},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para410), AUO_NT35590_para410},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para411), AUO_NT35590_para411},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para412), AUO_NT35590_para412},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para413), AUO_NT35590_para413},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para414), AUO_NT35590_para414},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para415), AUO_NT35590_para415},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para416), AUO_NT35590_para416},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para417), AUO_NT35590_para417},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para418), AUO_NT35590_para418},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para419), AUO_NT35590_para419},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para420), AUO_NT35590_para420},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para421), AUO_NT35590_para421},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para422), AUO_NT35590_para422},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para423), AUO_NT35590_para423},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para424), AUO_NT35590_para424},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para425), AUO_NT35590_para425},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para426), AUO_NT35590_para426},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para427), AUO_NT35590_para427},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para428), AUO_NT35590_para428},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para429), AUO_NT35590_para429},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para430), AUO_NT35590_para430},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para431), AUO_NT35590_para431},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para432), AUO_NT35590_para432},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para433), AUO_NT35590_para433},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para434), AUO_NT35590_para434},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para435), AUO_NT35590_para435},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para436), AUO_NT35590_para436},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para437), AUO_NT35590_para437},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para438), AUO_NT35590_para438},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para439), AUO_NT35590_para439},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para440), AUO_NT35590_para440},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para441), AUO_NT35590_para441},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para442), AUO_NT35590_para442},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para443), AUO_NT35590_para443},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para444), AUO_NT35590_para444},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para445), AUO_NT35590_para445},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para446), AUO_NT35590_para446},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para447), AUO_NT35590_para447},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para448), AUO_NT35590_para448},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para449), AUO_NT35590_para449},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para450), AUO_NT35590_para450},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para451), AUO_NT35590_para451},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para452), AUO_NT35590_para452},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para453), AUO_NT35590_para453},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para454), AUO_NT35590_para454},
	 
	{DTYPE_DCS_WRITE, 1, 0, 0, 120, sizeof(exit_sleep), exit_sleep},
	 
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para455), AUO_NT35590_para455},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para456), AUO_NT35590_para456},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para457), AUO_NT35590_para457},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para_TE), AUO_NT35590_para_TE},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para_TE1), AUO_NT35590_para_TE1},
	{DTYPE_DCS_WRITE, 1, 0, 0, 10, sizeof(display_on), display_on},
#ifdef USE_CABC_PANEL	
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para_cabc2), AUO_NT35590_para_cabc2},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(AUO_NT35590_para_cabc3), AUO_NT35590_para_cabc3},
#endif
};

/******************AUO(new) NT35590 end ************************/


struct dsi_cmd_desc novatek_cmd_on_cmds[] = {
	{DTYPE_DCS_WRITE, 1, 0, 0, 50,
		sizeof(sw_reset), sw_reset},
	{DTYPE_DCS_WRITE, 1, 0, 0, 10,
		sizeof(exit_sleep), exit_sleep},
	{DTYPE_DCS_WRITE, 1, 0, 0, 10,
		sizeof(display_on), display_on},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 50,
		sizeof(novatek_f4), novatek_f4},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 50,
		sizeof(novatek_8c), novatek_8c},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 50,
		sizeof(novatek_ff), novatek_ff},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 10,
		sizeof(set_num_of_lanes), set_num_of_lanes},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 50,
		sizeof(set_width), set_width},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 50,
		sizeof(set_height), set_height},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 10,
		sizeof(rgb_888), rgb_888},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1,
		sizeof(led_pwm2), led_pwm2},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1,
		sizeof(led_pwm3), led_pwm3},
};

static struct dsi_cmd_desc novatek_display_off_cmds[] = {
	{DTYPE_DCS_WRITE, 1, 0, 0, 10,
		sizeof(display_off), display_off},
	{DTYPE_DCS_WRITE, 1, 0, 0, 120,
		sizeof(enter_sleep), enter_sleep}
};

#define PM8921_GPIO_BASE		NR_GPIO_IRQS
#define PM8921_GPIO_PM_TO_SYS(pm_gpio)	(pm_gpio - 1 + PM8921_GPIO_BASE)
#define PM8921_MPP_BASE			(PM8921_GPIO_BASE + PM8921_NR_GPIOS)
#define PM8921_MPP_PM_TO_SYS(pm_gpio)	(pm_gpio - 1 + PM8921_MPP_BASE)
#define PM8921_IRQ_BASE			(NR_MSM_IRQS + NR_GPIO_IRQS)
static char manufacture_id[2] = {0x04, 0x00}; /* DTYPE_DCS_READ */

static struct dsi_cmd_desc novatek_manufacture_id_cmd = {
	DTYPE_DCS_READ, 1, 0, 1, 5, sizeof(manufacture_id), manufacture_id};

static u32 manu_id;

static void mipi_novatek_manufacture_cb(u32 data)
{
	manu_id = data;
	pr_info("%s: manufacture_id=%x\n", __func__, manu_id);
}

 uint32 mipi_novatek_manufacture_id(struct msm_fb_data_type *mfd)
{
	struct dcs_cmd_req cmdreq;

	cmdreq.cmds = &novatek_manufacture_id_cmd;
	cmdreq.cmds_cnt = 1;
	cmdreq.flags = CMD_REQ_RX | CMD_REQ_COMMIT;
	cmdreq.rlen = 3;
	cmdreq.cb = mipi_novatek_manufacture_cb; /* call back */
	mipi_dsi_cmdlist_put(&cmdreq);
	/*
	 * blocked here, untill call back called
	 */

	return manu_id;
}

static int fpga_addr;
static int fpga_access_mode;
static bool support_3d;

static void mipi_novatek_3d_init(int addr, int mode)
{
	fpga_addr = addr;
	fpga_access_mode = mode;
}

static void mipi_dsi_enable_3d_barrier(int mode)
{
	void __iomem *fpga_ptr;
	uint32_t ptr_value = 0;

	if (!fpga_addr && support_3d) {
		pr_err("%s: fpga_addr not set. Failed to enable 3D barrier\n",
					__func__);
		return;
	}

	if (fpga_access_mode == FPGA_SPI_INTF) {
		if (mode == LANDSCAPE)
			novatek_fpga_write(fpga_addr, 1);
		else if (mode == PORTRAIT)
			novatek_fpga_write(fpga_addr, 3);
		else
			novatek_fpga_write(fpga_addr, 0);

		mb();
		novatek_fpga_read(fpga_addr);
	} else if (fpga_access_mode == FPGA_EBI2_INTF) {
		fpga_ptr = ioremap_nocache(fpga_addr, sizeof(uint32_t));
		if (!fpga_ptr) {
			pr_err("%s: FPGA ioremap failed."
				"Failed to enable 3D barrier\n",
						__func__);
			return;
		}

		ptr_value = readl_relaxed(fpga_ptr);
		if (mode == LANDSCAPE)
			writel_relaxed(((0xFFFF0000 & ptr_value) | 1),
								fpga_ptr);
		else if (mode == PORTRAIT)
			writel_relaxed(((0xFFFF0000 & ptr_value) | 3),
								fpga_ptr);
		else
			writel_relaxed((0xFFFF0000 & ptr_value),
								fpga_ptr);

		mb();
		iounmap(fpga_ptr);
	} else
		pr_err("%s: 3D barrier not configured correctly\n",
					__func__);
}


static void lcd_panle_reset(void)
{	
	gpio_direction_output(GPIO_LCD_RESET,1);
	msleep(10);
	gpio_direction_output(GPIO_LCD_RESET,0);
	msleep(20);
	gpio_direction_output(GPIO_LCD_RESET,1);
	msleep(50);
}

static int first_time_panel_on = 1;

static int mipi_novatek_lcd_on(struct platform_device *pdev)
{
	struct msm_fb_data_type *mfd;
	struct mipi_panel_info *mipi;
	struct msm_panel_info *pinfo;
	struct dcs_cmd_req cmdreq;

	mfd = platform_get_drvdata(pdev);
	if (!mfd)
		return -ENODEV;
	if (mfd->key != MFD_KEY)
		return -EINVAL;

	pinfo = &mfd->panel_info;
	if (pinfo->is_3d_panel)
		support_3d = TRUE;

	mipi  = &mfd->panel_info.mipi;
	
	printk("LCD, mipi_lcd_on , LcdPanleID = %d\n", LcdPanleID);
	
	if(first_time_panel_on)
	{
		first_time_panel_on = 0;
		printk("\n LCD first time lcd panel on ----return ");
		return 0;
	}
	
	lcd_panle_reset();
	mipi_set_tx_power_mode(1);
	switch(LcdPanleID){
			case LCD_PANEL_4P5_OTM1281_OT_TRULY:
				printk("\n LCD TRULY OTM1281A AUO panel initialized!\n");
				cmdreq.cmds = OTM1281_AUO_CMD_INIT_PARA;
				cmdreq.cmds_cnt = ARRAY_SIZE(OTM1281_AUO_CMD_INIT_PARA);
				cmdreq.flags = CMD_REQ_COMMIT;
				cmdreq.rlen = 0;
				cmdreq.cb = NULL;
				mipi_dsi_cmdlist_put(&cmdreq);
				break;
			case LCD_PANEL_4P5_OTM1280_OT_CMI:
				printk("\n LCD CMI OTM1280A panel initialized!\n");
				cmdreq.cmds = OTM1280_CMI_CMD_INIT_PARA;
				cmdreq.cmds_cnt = ARRAY_SIZE(OTM1280_CMI_CMD_INIT_PARA);
				cmdreq.flags = CMD_REQ_COMMIT;
				cmdreq.rlen = 0;
				cmdreq.cb = NULL;
				mipi_dsi_cmdlist_put(&cmdreq);
				break;
			case LCD_PANEL_4P5_NT35590_AUO_LEAD:
				printk("\n LCD AUO NT35590 panel initialized!\n");
				cmdreq.cmds = AUO_NT35590_CMD_INIT_PARA;
				cmdreq.cmds_cnt = ARRAY_SIZE(AUO_NT35590_CMD_INIT_PARA);
				cmdreq.flags = CMD_REQ_COMMIT;
				cmdreq.rlen = 0;
				cmdreq.cb = NULL;
				mipi_dsi_cmdlist_put(&cmdreq);
				break;
			default:
				printk("\n LCD OU!OU! NO PANEL...use TRULY OTM1281A AUO panel initialized!\n");				
				break;
				
	}

		/* clean up ack_err_status */
		//mipi_dsi_cmd_bta_sw_trigger();
	mipi_set_tx_power_mode(0);
	return 0;
}

static int mipi_novatek_lcd_off(struct platform_device *pdev)
{
	struct msm_fb_data_type *mfd;
	struct dcs_cmd_req cmdreq;

	mfd = platform_get_drvdata(pdev);

	if (!mfd)
		return -ENODEV;
	if (mfd->key != MFD_KEY)
		return -EINVAL;
	
	cmdreq.cmds = novatek_display_off_cmds;
	cmdreq.cmds_cnt = ARRAY_SIZE(novatek_display_off_cmds);
	cmdreq.flags = CMD_REQ_COMMIT;
	cmdreq.rlen = 0;
	cmdreq.cb = NULL;

	mipi_dsi_cmdlist_put(&cmdreq);

	printk("\n LCD panel off");
	return 0;
}


static bool onewiremode = false;
static int lcd_bkl_ctl=2;
void myudelay(unsigned int usec)
{
	udelay(usec);
}
static void select_1wire_mode(void)
{
	gpio_direction_output(lcd_bkl_ctl, 1);
	myudelay(120);
	gpio_direction_output(lcd_bkl_ctl, 0);
	myudelay(280);				////ZTE_LCD_LUYA_20100226_001
	gpio_direction_output(lcd_bkl_ctl, 1);
	myudelay(650);				////ZTE_LCD_LUYA_20100226_001
	
}

static void send_bkl_address(void)
{
	unsigned int i,j;
	i = 0x72;
	gpio_direction_output(lcd_bkl_ctl, 1);
	myudelay(10);
	printk("[LY] send_bkl_address \n");
	for(j = 0; j < 8; j++)
	{
		if(i & 0x80)
		{
			gpio_direction_output(lcd_bkl_ctl, 0);
			myudelay(10);
			gpio_direction_output(lcd_bkl_ctl, 1);
			myudelay(180);
		}
		else
		{
			gpio_direction_output(lcd_bkl_ctl, 0);
			myudelay(180);
			gpio_direction_output(lcd_bkl_ctl, 1);
			myudelay(10);
		}
		i <<= 1;
	}
	gpio_direction_output(lcd_bkl_ctl, 0);
	myudelay(10);
	gpio_direction_output(lcd_bkl_ctl, 1);

}

static void send_bkl_data(int level)
{
	unsigned int i,j;
	i = level & 0x1F;
	gpio_direction_output(lcd_bkl_ctl, 1);
	myudelay(10);
	printk("[LY] send_bkl_data \n");
	for(j = 0; j < 8; j++)
	{
		if(i & 0x80)
		{
			gpio_direction_output(lcd_bkl_ctl, 0);
			myudelay(10);
			gpio_direction_output(lcd_bkl_ctl, 1);
			myudelay(180);
		}
		else
		{
			gpio_direction_output(lcd_bkl_ctl, 0);
			myudelay(180);
			gpio_direction_output(lcd_bkl_ctl, 1);
			myudelay(10);
		}
		i <<= 1;
	}
	gpio_direction_output(lcd_bkl_ctl, 0);
	myudelay(10);
	gpio_direction_output(lcd_bkl_ctl, 1);

}
static void mipi_zte_set_backlight(struct msm_fb_data_type *mfd)
{
	/*value range is 1--32*/
	int current_lel = mfd->bl_level;
	unsigned long flags;

	printk("[ZYF] lcdc_set_bl level=%d, %d\n", 
	current_lel , mfd->panel_power_on);

	if(!mfd->panel_power_on)
	{
		gpio_direction_output(lcd_bkl_ctl, 0);
		onewiremode = FALSE;
		return;
	}

	if(current_lel < 1)
	{
		current_lel = 0;
	}
#ifdef USE_CABC_PANEL
	 if(current_lel > 255)
	 {
		 current_lel = 255;
	 }
 
	 printk("\n LCD cabc, mipi_set_backlight\n");
 
	 set_bk_lv(mfd,current_lel);

	 return;
#else
	if(current_lel > 32)
	{
		current_lel = 32;
	}
#endif
	local_irq_save(flags);

	if(current_lel==0)
	{
		gpio_direction_output(lcd_bkl_ctl, 0);
		mdelay(3);
		onewiremode = FALSE;
	}
	else 
	{
		if(!onewiremode)	
		{
			printk("[LY] before select_1wire_mode\n");
			select_1wire_mode();
			onewiremode = TRUE;
		}
		send_bkl_address();
		send_bkl_data(current_lel-1);
	}

	local_irq_restore(flags);
}

static int mipi_dsi_3d_barrier_sysfs_register(struct device *dev);
static int barrier_mode;

static int __devinit mipi_novatek_lcd_probe(struct platform_device *pdev)
{
	struct msm_fb_data_type *mfd;
	struct mipi_panel_info *mipi;
	struct platform_device *current_pdev;
	static struct mipi_dsi_phy_ctrl *phy_settings;
	static char dlane_swap;

	if (pdev->id == 0) {
		mipi_novatek_pdata = pdev->dev.platform_data;

		if (mipi_novatek_pdata
			&& mipi_novatek_pdata->phy_ctrl_settings) {
			phy_settings = (mipi_novatek_pdata->phy_ctrl_settings);
		}

		if (mipi_novatek_pdata
			&& mipi_novatek_pdata->dlane_swap) {
			dlane_swap = (mipi_novatek_pdata->dlane_swap);
		}

		if (mipi_novatek_pdata
			 && mipi_novatek_pdata->fpga_3d_config_addr)
			mipi_novatek_3d_init(mipi_novatek_pdata
	->fpga_3d_config_addr, mipi_novatek_pdata->fpga_ctrl_mode);

		/* create sysfs to control 3D barrier for the Sharp panel */
		if (mipi_dsi_3d_barrier_sysfs_register(&pdev->dev)) {
			pr_err("%s: Failed to register 3d Barrier sysfs\n",
						__func__);
			return -ENODEV;
		}
		barrier_mode = 0;

		return 0;
	}

	current_pdev = msm_fb_add_device(pdev);
	printk("[ycm] mipi_novatek_lcd_probe \n");

	if (current_pdev) {
		mfd = platform_get_drvdata(current_pdev);
		if (!mfd)
			return -ENODEV;
		if (mfd->key != MFD_KEY)
			return -EINVAL;

		mipi  = &mfd->panel_info.mipi;

	       if (phy_settings != NULL) {
		       mipi->dsi_phy_db = &phy_settings;
		       mipi->dsi_phy_db_count = 1;
	       }

		if (dlane_swap)
			mipi->dlane_swap = dlane_swap;
	}

	return 0;
}

static struct platform_driver this_driver = {
	.probe  = mipi_novatek_lcd_probe,
	.driver = {
		.name   = "mipi_warplte",
	},
};

static struct msm_fb_panel_data novatek_panel_data = {
	.on		= mipi_novatek_lcd_on,
	.off		= mipi_novatek_lcd_off,
	.set_backlight = mipi_zte_set_backlight,//mipi_novatek_set_backlight,
};

static ssize_t mipi_dsi_3d_barrier_read(struct device *dev,
				struct device_attribute *attr,
				char *buf)
{
	return snprintf((char *)buf, sizeof(buf), "%u\n", barrier_mode);
}

static ssize_t mipi_dsi_3d_barrier_write(struct device *dev,
				struct device_attribute *attr,
				const char *buf,
				size_t count)
{
	int ret = -1;
	u32 data = 0;

	if (sscanf((char *)buf, "%u", &data) != 1) {
		dev_err(dev, "%s\n", __func__);
		ret = -EINVAL;
	} else {
		barrier_mode = data;
		if (data == 1)
			mipi_dsi_enable_3d_barrier(LANDSCAPE);
		else if (data == 2)
			mipi_dsi_enable_3d_barrier(PORTRAIT);
		else
			mipi_dsi_enable_3d_barrier(0);
	}

	return count;
}

static struct device_attribute mipi_dsi_3d_barrier_attributes[] = {
	__ATTR(enable_3d_barrier, 0664, mipi_dsi_3d_barrier_read,
					 mipi_dsi_3d_barrier_write),
};

static int mipi_dsi_3d_barrier_sysfs_register(struct device *dev)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(mipi_dsi_3d_barrier_attributes); i++)
		if (device_create_file(dev, mipi_dsi_3d_barrier_attributes + i))
			goto error;

	return 0;

error:
	for (; i >= 0 ; i--)
		device_remove_file(dev, mipi_dsi_3d_barrier_attributes + i);
	pr_err("%s: Unable to create interface\n", __func__);

	return -ENODEV;
}

static int ch_used[3];

int mipi_novatek_device_register(struct msm_panel_info *pinfo,
					u32 channel, u32 panel)
{
	struct platform_device *pdev = NULL;
	int ret;

	if ((channel >= 3) || ch_used[channel])
		return -ENODEV;

	ch_used[channel] = TRUE;

	ret = mipi_novatek_lcd_init();
	if (ret) {
		pr_err("mipi_novatek_lcd_init() failed with ret %u\n", ret);
		return ret;
	}

	pdev = platform_device_alloc("mipi_warplte", (panel << 8)|channel);
	if (!pdev)
		return -ENOMEM;

	novatek_panel_data.panel_info = *pinfo;

	ret = platform_device_add_data(pdev, &novatek_panel_data,
		sizeof(novatek_panel_data));
	if (ret) {
		printk(KERN_ERR
		  "%s: platform_device_add_data failed!\n", __func__);
		goto err_device_put;
	}

	ret = platform_device_add(pdev);
	if (ret) {
		printk(KERN_ERR
		  "%s: platform_device_register failed!\n", __func__);
		goto err_device_put;
	}

	return 0;

err_device_put:
	platform_device_put(pdev);
	return ret;
}

static int mipi_novatek_lcd_init(void)
{
	mipi_dsi_buf_alloc(&novatek_tx_buf, DSI_BUF_SIZE);
	mipi_dsi_buf_alloc(&novatek_rx_buf, DSI_BUF_SIZE);

	return platform_driver_register(&this_driver);
}

static struct mipi_dsi_phy_ctrl dsi_cmd_mode_phy_db = {
/* 720*1280, RGB888, 4 Lane 60  fps video mode */
#if 1
	/* regulator */
	{0x03, 0x0a, 0x04, 0x00, 0x20},
	/* timing */
	{0x6f, 0x18, 0x10, 0x00, 0x3b, 0x3f, 0x1f, 0x1f,
	0x1b, 0x03, 0x04, 0xa0},
	/* phy ctrl */
	{0x5f, 0x00, 0x00, 0x10},
	/* strength */
	{0xff, 0x00, 0x06, 0x00},
	/* pll control */
	{0x01, 0x84, 0x31, 0xda, 0x00, 0x5f, 0x48, 0x63,
	0x31, 0x0f, 0x03,//0x41, 0x0f, 0x01,
	0x00, 0x14, 0x03, 0x00, 0x02, 0x00, 0x20, 0x00, 0x01 },
	60,
#endif
#if 0
/* 720*1280, RGB888, 4 Lane 60 fps video mode */
/* regulator */
	 {0x09, 0x08, 0x05, 0x00, 0x20},		 /* regulator */
	 /* timing	 */
	 {0x70, 0x19, 0x10, 0x00, 0x2d, 0x33, 0x15, 0x1d,
	 0x1e, 0x03, 0x04, 0xa0},
	 {0x5f, 0x00, 0x00, 0x10},		 /* phy ctrl */
	 {0xff, 0x00, 0x06, 0x00},		 /* strength */
	 /* pll control */
	 {0x0, 0xf, 0x30, 0xc0, 0x00, 0x10, 0x0f, 0x61,
	 0x40, 0x07, 0x03,
	 0x00, 0x1a, 0x00, 0x00, 0x02, 0x00, 0x20, 0x00, 0x02},

#endif
};
static struct mipi_dsi_phy_ctrl *dsi_cmd_mode_phy_dbs[] = {
	&dsi_cmd_mode_phy_db,
};

static int __init mipi_otm_cmd_hd_pt_init(void)
{
	int ret;
	static struct msm_panel_info pinfo;
	
	pinfo.xres = 720;
	pinfo.yres = 1280;
	pinfo.type = MIPI_CMD_PANEL;
	pinfo.pdest = DISPLAY_1;
	pinfo.wait_cycle = 0;
	pinfo.bpp = 24;
	pinfo.lcdc.h_back_porch = 52;
	pinfo.lcdc.h_front_porch = 52;
	pinfo.lcdc.h_pulse_width = 20;
	pinfo.lcdc.v_back_porch = 11;
	pinfo.lcdc.v_front_porch = 10;
	pinfo.lcdc.v_pulse_width = 5;
	pinfo.lcdc.border_clr = 0;	/* blk */
	pinfo.lcdc.underflow_clr = 0xff;	/* blue */
	pinfo.lcdc.hsync_skew = 0;
#ifdef USE_CABC_PANEL
	pinfo.bl_max = 254;
#else
	pinfo.bl_max = 31;
#endif
	pinfo.bl_min = 1;
	pinfo.fb_num = 2;
	pinfo.clk_rate = 389005096;
//	pinfo.is_3d_panel = FB_TYPE_3D_PANEL;
 	pinfo.lcd.vsync_enable = TRUE;
	pinfo.lcd.hw_vsync_mode = TRUE;
	pinfo.lcd.refx100 = 5900; /* adjust refx100 to prevent tearing */
	pinfo.lcd.v_back_porch = 11;
	pinfo.lcd.v_front_porch = 10;
	pinfo.lcd.v_pulse_width = 5;

	pinfo.mipi.mode = DSI_CMD_MODE;
	pinfo.mipi.dst_format = DSI_CMD_DST_FORMAT_RGB888;
	pinfo.mipi.vc = 0;
	pinfo.mipi.data_lane0 = TRUE;
	pinfo.mipi.data_lane1 = TRUE;
	pinfo.mipi.data_lane2 = TRUE;
	pinfo.mipi.data_lane3 = TRUE;
	pinfo.mipi.t_clk_post = 0x09;
	pinfo.mipi.t_clk_pre = 0x24;
	pinfo.mipi.esc_byte_ratio = 4;
	pinfo.mipi.stream = 0;	/* dma_p */
	pinfo.mipi.mdp_trigger = DSI_CMD_TRIGGER_SW_TE;
	pinfo.mipi.dma_trigger = DSI_CMD_TRIGGER_SW;
	pinfo.mipi.te_sel = 1; /* TE from vsycn gpio */
	pinfo.mipi.interleave_max = 1;
	pinfo.mipi.insert_dcs_cmd = TRUE;
	pinfo.mipi.wr_mem_continue = 0x3c;
	pinfo.mipi.wr_mem_start = 0x2c;

	pinfo.mipi.dsi_phy_db = dsi_cmd_mode_phy_dbs;
	pinfo.mipi.dsi_phy_db_count = ARRAY_SIZE(dsi_cmd_mode_phy_dbs);
		

	ret = mipi_novatek_device_register(&pinfo, MIPI_DSI_PRIM,
						MIPI_DSI_PANEL_720P_PT);
	if (ret)
		printk("%s: failed to register device!\n", __func__);

	return ret;
}

module_init(mipi_otm_cmd_hd_pt_init);
