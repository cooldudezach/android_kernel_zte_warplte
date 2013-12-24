/* Copyright (c) 2011, Code Aurora Forum. All rights reserved.
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

#include "msm_fb.h"
#include "mipi_dsi.h"
#include "mipi_toshiba.h"
#include <mach/gpio.h>


/* Macros assume PMIC GPIOs and MPPs start at 1 */
#define PM8921_GPIO_BASE		NR_GPIO_IRQS
#define PM8921_GPIO_PM_TO_SYS(pm_gpio)	(pm_gpio - 1 + PM8921_GPIO_BASE)
#define PM8921_MPP_BASE			(PM8921_GPIO_BASE + PM8921_NR_GPIOS)
#define PM8921_MPP_PM_TO_SYS(pm_gpio)	(pm_gpio - 1 + PM8921_MPP_BASE)
#define PM8921_IRQ_BASE			(NR_MSM_IRQS + NR_GPIO_IRQS)


static struct msm_panel_info pinfo;

extern u32 LcdPanleID;
#define GPIO_LCD_RESET 58

//extern int lcdinit_backlight_delay;
static struct mipi_dsi_phy_ctrl dsi_video_mode_phy_db = {
	{0x03, 0x0a, 0x04, 0x00, 0x20},
	/* timing */
	{0xab, 0x28, 0x18, 0x00, 0x92, 0x97, 0x1b, 0x28,
	0x0c, 0x03, 0x04, 0xa0},
    /* phy ctrl */
	{0x5f, 0x00, 0x00, 0x10},
    /* strength */
	{0xff, 0x00, 0x06, 0x00},
	/* pll control */
	{0x0, 0x7f, 0x31, 0xda, 0x00, 0x50, 0x48, 0x63,
	0x30, 0x07, 0x03,//0x41, 0x0f, 0x01,
	0x00, 0x14, 0x03, 0x00, 0x02, 0x00, 0x20, 0x00, 0x01 },
	60,

};
static struct mipi_dsi_phy_ctrl dsi_video_mode_phy_db_30 =  {
	//480 800 2lane 30fps 209.25Mhz
	{0x09, 0x08, 0x05, 0x00, 0x20}, /* regulator */
        /* timing */
	{0x37, 0xc, 0x7, 0x00, 0x16, 0x1c, 0xc, 0x10,
	 0xe, 0x03, 0x04, 0xa0},
	{0x5f, 0x00, 0x00, 0x10}, /* phy ctrl */
	{0xff, 0x00, 0x06, 0x00}, /* strength */
        /* pll control */
	{0x0, 0x1e, 0x30, 0xc1, 0x00, 0x10, 0x0f, 0x61,
	 0x40, 0x07, 0x03,
	 0x00, 0x1a, 0x00, 0x00, 0x02, 0x00, 0x20, 0x00, 0x02},
	30,
};

static struct mipi_dsi_phy_ctrl dsi_video_mode_phy_db_45 =  {
        //480 800 2lane 45fps 315Mhz
	{0x09, 0x08, 0x05, 0x00, 0x20}, /* regulator */
        /* timing */
	{0x52, 0x12, 0xb, 0x00, 0x21, 0x28, 0x10, 0x16,
	 0x16, 0x03, 0x04, 0xa0},
	{0x5f, 0x00, 0x00, 0x10}, /* phy ctrl */
	{0xff, 0x00, 0x06, 0x00}, /* strength */
        /* pll control */
	{0x0, 0x22, 0x30, 0xc2, 0x00, 0x10, 0x0f, 0x61,
	 0x40, 0x07, 0x03,
	 0x00, 0x1a, 0x00, 0x00, 0x02, 0x00, 0x20, 0x00, 0x02},
	45,
};

static struct mipi_dsi_phy_ctrl *dsi_video_mode_phy_dbs[] = {
	&dsi_video_mode_phy_db,
	&dsi_video_mode_phy_db_45,
	&dsi_video_mode_phy_db_30
};


static int bl_lpm = 0x02;



static struct dsi_buf mipi_tx_buf;
static struct dsi_buf mipi_rx_buf;

static char exit_sleep[2] = {0x11, 0x00};
static char display_on[2] = {0x29, 0x00};
static char display_off[2] = {0x28, 0x00};
static char enter_sleep[2] = {0x10, 0x00};



#define COLOR_ENHANCE



static struct dsi_cmd_desc display_off_cmds[] = {
	{DTYPE_DCS_WRITE, 1, 0, 0, 50, sizeof(display_off), display_off},
	{DTYPE_DCS_WRITE, 1, 0, 0, 120, sizeof(enter_sleep), enter_sleep}
};

static char tianma_hx8369a_para01[]={0xB9,0xFF,0x83,0x69};
static char tianma_hx8369a_para02[]={0xB1,0x01,0x00,0x44,0x0A,0x00,0x0F,0x0F,0x2C,0x34,0x3F,
									0x3F,0x01,0x3A,0x01,0xE6,0xE6,0xE6,0xE6,0xE6};	
static char tianma_hx8369a_para03[]={0xB2,0x00,0x13};  
static char tianma_hx8369a_para04[]={0xB4,0x00,0x1d,0x80,0x00,0x00};
static char tianma_hx8369a_para05[]={0xB6,0x46,0x46};
static char tianma_hx8369a_para06[]={0x36,0x00};
static char tianma_hx8369a_para07[]={0xD4,0x12,0x04};  	 
static char tianma_hx8369a_para08[]={0xD5,0x00,0x01,0x03,0x7D,0x01,0x0A,0x28,0x5A,0x11,0x13,
									0x00,0x00,0x60,0x04,0x71,0x05,0x00,0x00,0x71,0x05,0x60,
									0x04,0x07,0x0F,0x04,0x04};
static char tianma_hx8369a_para09[]={0xE0,0x00,0x20,0x26,0x34,0x38,0x3F,0x33,0x4B,0x09,0x13,
									0x0E,0x15,0x16,0x14,0x15,0x11,0x17,0x00,0x20,0x26,0x34,
									0x38,0x3F,0x33,0x4B,0x09,0x13,0x0E,0x15,0x16,0x14,0x15,
									0x11,0x17};
static char tianma_hx8369a_para10[]={0xCC,0x02};		 
static char tianma_hx8369a_para11[]={0xBA,0x00,0xA0,0xC6,0x80,0x0A,0x00,0x10,0x30,0x6F,0x02,
									0x11,0x18,0x40};

static struct dsi_cmd_desc TM_display_on_cmds[] =                                 
{                                                                                  
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(tianma_hx8369a_para01), tianma_hx8369a_para01},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(tianma_hx8369a_para02), tianma_hx8369a_para02},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(tianma_hx8369a_para03), tianma_hx8369a_para03},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(tianma_hx8369a_para04), tianma_hx8369a_para04},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(tianma_hx8369a_para05), tianma_hx8369a_para05},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(tianma_hx8369a_para06), tianma_hx8369a_para06},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(tianma_hx8369a_para07), tianma_hx8369a_para07},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(tianma_hx8369a_para08), tianma_hx8369a_para08},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(tianma_hx8369a_para09), tianma_hx8369a_para09},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(tianma_hx8369a_para10), tianma_hx8369a_para10},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(tianma_hx8369a_para11), tianma_hx8369a_para11},
	                                                                                 
	{DTYPE_DCS_WRITE,  1, 0, 0, 120, sizeof(exit_sleep), exit_sleep},                
	{DTYPE_DCS_WRITE, 1, 0, 0, 10, sizeof(display_on), display_on},                  
}; 

static char boe_otm8009a_para00[]={0x00,0x00};
static char boe_otm8009a_para01[]={0xFF,0x80,0x09,0x01};
static char boe_otm8009a_para02[]={0x00,0x80};
static char boe_otm8009a_para03[]={0xFF,0x80,0x09};
static char boe_otm8009a_para04[]={0x00,0x03};
static char boe_otm8009a_para05[]={0xFF,0x01};
static char boe_otm8009a_para06[]={0x00,0x90};
static char boe_otm8009a_para07[]={0xB3,0x02};
static char boe_otm8009a_para08[]={0x00,0x92};
static char boe_otm8009a_para09[]={0xB3,0x45};
static char boe_otm8009a_para10[]={0x00,0xA6};
static char boe_otm8009a_para11[]={0xB3,0x20};
static char boe_otm8009a_para12[]={0x00,0xA7};
static char boe_otm8009a_para13[]={0xB3,0x01};
static char boe_otm8009a_para14[]={0x00,0x80};
static char boe_otm8009a_para15[]={0xCE,0x85,0x01,0x00,0x84,0x01,0x00};
static char boe_otm8009a_para16[]={0x00,0xA0};
static char boe_otm8009a_para17[]={0xCE,0x18,0x04,0x03,0x5B,0x00,0x00,0x00,0x18,0x03,0x03,0x5C,0x00,0x00,0x00};
static char boe_otm8009a_para18[]={0x00,0xB0};
static char boe_otm8009a_para19[]={0xCE,0x18,0x02,0x03,0x5D,0x00,0x00,0x00,0x18,0x01,0x03,0x5E,0x00,0x00,0x00};
static char boe_otm8009a_para20[]={0x00,0xC0};
static char boe_otm8009a_para21[]={0xCF,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x00,0x00};
static char boe_otm8009a_para22[]={0x00,0xD0};
static char boe_otm8009a_para23[]={0xCF,0x00};
static char boe_otm8009a_para24[]={0x00,0x80};
static char boe_otm8009a_para25[]={0xCB,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
static char boe_otm8009a_para26[]={0x00,0x90};
static char boe_otm8009a_para27[]={0xCB,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
static char boe_otm8009a_para28[]={0x00,0xA0};
static char boe_otm8009a_para29[]={0xCB,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
static char boe_otm8009a_para30[]={0x00,0xB0};
static char boe_otm8009a_para31[]={0xCB,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
static char boe_otm8009a_para32[]={0x00,0xC0};
static char boe_otm8009a_para33[]={0xCB,0x00,0x04,0x04,0x04,0x04,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
static char boe_otm8009a_para34[]={0x00,0xD0};
static char boe_otm8009a_para35[]={0xCB,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x04,0x04,0x04,0x04,0x00,0x00,0x00,0x00};
static char boe_otm8009a_para36[]={0x00,0xE0};
static char boe_otm8009a_para37[]={0xCB,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
static char boe_otm8009a_para38[]={0x00,0xF0};
static char boe_otm8009a_para39[]={0xCB,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
static char boe_otm8009a_para40[]={0x00,0x80};
static char boe_otm8009a_para41[]={0xCC,0x00,0x26,0x09,0x0B,0x01,0x25,0x00,0x00,0x00,0x00};
static char boe_otm8009a_para42[]={0x00,0x90};
static char boe_otm8009a_para43[]={0xCC,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x26,0x0A,0x0C,0x02};
static char boe_otm8009a_para44[]={0x00,0xA0};
static char boe_otm8009a_para45[]={0xCC,0x25,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
static char boe_otm8009a_para46[]={0x00,0xB0};
static char boe_otm8009a_para47[]={0xCC,0x00,0x25,0x0A,0x0C,0x02,0x26,0x00,0x00,0x00,0x00};
static char boe_otm8009a_para48[]={0x00,0xC0};
static char boe_otm8009a_para49[]={0xCC,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x25,0x09,0x0B,0x01};
static char boe_otm8009a_para50[]={0x00,0xD0};
static char boe_otm8009a_para51[]={0xCC,0x26,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
static char boe_otm8009a_para52[]={0x00,0x00};
static char boe_otm8009a_para53[]={0xE1,0x05,0x0D,0x13,0x0D,0x06,0x10,0x0A,0x09,0x04,0x07,0x09,0x03,0x0B,0x18,0x16,0x0E};
static char boe_otm8009a_para54[]={0x00,0x00};
static char boe_otm8009a_para55[]={0xE2,0x05,0x0D,0x13,0x0E,0x06,0x0F,0x0A,0x09,0x04,0x07,0x09,0x03,0x0B,0x17,0x15,0x0E};
static char boe_otm8009a_para56[]={0x00,0xA3};
static char boe_otm8009a_para57[]={0xC0,0x1B};
static char boe_otm8009a_para58[]={0x00,0xB4};
static char boe_otm8009a_para59[]={0xC0,0x50};
static char boe_otm8009a_para60[]={0x00,0x81};
static char boe_otm8009a_para61[]={0xC4,0x04};
static char boe_otm8009a_para62[]={0x00,0x80};
static char boe_otm8009a_para63[]={0xC5,0x03};
static char boe_otm8009a_para64[]={0x00,0x82};
static char boe_otm8009a_para65[]={0xC5,0x03};
static char boe_otm8009a_para66[]={0x00,0x90};
static char boe_otm8009a_para67[]={0xC5,0x96};
static char boe_otm8009a_para68[]={0x00,0x91};
static char boe_otm8009a_para69[]={0xC5,0x1D,0x01,0x7B,0x33};
static char boe_otm8009a_para70[]={0x00,0x00};
static char boe_otm8009a_para71[]={0xD8,0x70,0x70};
static char boe_otm8009a_para72[]={0x00,0x00};
static char boe_otm8009a_para73[]={0xD9,0x39};
static char boe_otm8009a_para74[]={0x00,0xB1};
static char boe_otm8009a_para75[]={0xC5,0x29};
static char boe_otm8009a_para76[]={0x00,0x81};
static char boe_otm8009a_para77[]={0xC1,0x66};
static char boe_otm8009a_para78[]={0x00,0x80};
static char boe_otm8009a_para79[]={0xC0,0x00,0x58,0x00,0x15,0x15,0x00,0x58,0x15,0x15};
static char boe_otm8009a_para80[]={0x00,0xA1};
static char boe_otm8009a_para81[]={0xC1,0x08};
static char boe_otm8009a_para82[]={0x00,0x00};
static char boe_otm8009a_para83[]={0x36,0x00};

static struct dsi_cmd_desc BOE_display_on_cmds[] = 
{                                                           
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(boe_otm8009a_para00), boe_otm8009a_para00},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(boe_otm8009a_para01), boe_otm8009a_para01},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(boe_otm8009a_para02), boe_otm8009a_para02},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(boe_otm8009a_para03), boe_otm8009a_para03},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(boe_otm8009a_para04), boe_otm8009a_para04},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(boe_otm8009a_para05), boe_otm8009a_para05},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(boe_otm8009a_para06), boe_otm8009a_para06},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(boe_otm8009a_para07), boe_otm8009a_para07},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(boe_otm8009a_para08), boe_otm8009a_para08},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(boe_otm8009a_para09), boe_otm8009a_para09},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(boe_otm8009a_para10), boe_otm8009a_para10},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(boe_otm8009a_para11), boe_otm8009a_para11},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(boe_otm8009a_para12), boe_otm8009a_para12},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(boe_otm8009a_para13), boe_otm8009a_para13},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(boe_otm8009a_para14), boe_otm8009a_para14},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(boe_otm8009a_para15), boe_otm8009a_para15},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(boe_otm8009a_para16), boe_otm8009a_para16},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(boe_otm8009a_para17), boe_otm8009a_para17},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(boe_otm8009a_para18), boe_otm8009a_para18},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(boe_otm8009a_para19), boe_otm8009a_para19},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(boe_otm8009a_para20), boe_otm8009a_para20},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(boe_otm8009a_para21), boe_otm8009a_para21},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(boe_otm8009a_para22), boe_otm8009a_para22},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(boe_otm8009a_para23), boe_otm8009a_para23},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(boe_otm8009a_para24), boe_otm8009a_para24},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(boe_otm8009a_para25), boe_otm8009a_para25},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(boe_otm8009a_para26), boe_otm8009a_para26},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(boe_otm8009a_para27), boe_otm8009a_para27},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(boe_otm8009a_para28), boe_otm8009a_para28},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(boe_otm8009a_para29), boe_otm8009a_para29},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(boe_otm8009a_para30), boe_otm8009a_para30},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(boe_otm8009a_para31), boe_otm8009a_para31},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(boe_otm8009a_para32), boe_otm8009a_para32},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(boe_otm8009a_para33), boe_otm8009a_para33},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(boe_otm8009a_para34), boe_otm8009a_para34},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(boe_otm8009a_para35), boe_otm8009a_para35},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(boe_otm8009a_para36), boe_otm8009a_para36},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(boe_otm8009a_para37), boe_otm8009a_para37},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(boe_otm8009a_para38), boe_otm8009a_para38},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(boe_otm8009a_para39), boe_otm8009a_para39},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(boe_otm8009a_para40), boe_otm8009a_para40},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(boe_otm8009a_para41), boe_otm8009a_para41},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(boe_otm8009a_para42), boe_otm8009a_para42},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(boe_otm8009a_para43), boe_otm8009a_para43},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(boe_otm8009a_para44), boe_otm8009a_para44},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(boe_otm8009a_para45), boe_otm8009a_para45},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(boe_otm8009a_para46), boe_otm8009a_para46},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(boe_otm8009a_para47), boe_otm8009a_para47},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(boe_otm8009a_para48), boe_otm8009a_para48},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(boe_otm8009a_para49), boe_otm8009a_para49},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(boe_otm8009a_para50), boe_otm8009a_para50},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(boe_otm8009a_para51), boe_otm8009a_para51},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(boe_otm8009a_para52), boe_otm8009a_para52},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(boe_otm8009a_para53), boe_otm8009a_para53},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(boe_otm8009a_para54), boe_otm8009a_para54},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(boe_otm8009a_para55), boe_otm8009a_para55},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(boe_otm8009a_para56), boe_otm8009a_para56},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(boe_otm8009a_para57), boe_otm8009a_para57},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(boe_otm8009a_para58), boe_otm8009a_para58},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(boe_otm8009a_para59), boe_otm8009a_para59},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(boe_otm8009a_para60), boe_otm8009a_para60},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(boe_otm8009a_para61), boe_otm8009a_para61},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(boe_otm8009a_para62), boe_otm8009a_para62},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(boe_otm8009a_para63), boe_otm8009a_para63},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(boe_otm8009a_para64), boe_otm8009a_para64},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(boe_otm8009a_para65), boe_otm8009a_para65},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(boe_otm8009a_para66), boe_otm8009a_para66},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(boe_otm8009a_para67), boe_otm8009a_para67},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(boe_otm8009a_para68), boe_otm8009a_para68},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(boe_otm8009a_para69), boe_otm8009a_para69},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(boe_otm8009a_para70), boe_otm8009a_para70},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(boe_otm8009a_para71), boe_otm8009a_para71},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(boe_otm8009a_para72), boe_otm8009a_para72},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(boe_otm8009a_para73), boe_otm8009a_para73},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(boe_otm8009a_para74), boe_otm8009a_para74},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(boe_otm8009a_para75), boe_otm8009a_para75},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(boe_otm8009a_para76), boe_otm8009a_para76},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(boe_otm8009a_para77), boe_otm8009a_para77},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(boe_otm8009a_para78), boe_otm8009a_para78},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(boe_otm8009a_para79), boe_otm8009a_para79},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(boe_otm8009a_para80), boe_otm8009a_para80},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(boe_otm8009a_para81), boe_otm8009a_para81},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(boe_otm8009a_para82), boe_otm8009a_para82},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(boe_otm8009a_para83), boe_otm8009a_para83},
	
	{DTYPE_DCS_WRITE,  1, 0, 0, 120, sizeof(exit_sleep), exit_sleep},
	{DTYPE_DCS_WRITE, 1, 0, 0, 10, sizeof(display_on), display_on}, 
};

static char tianma_hx8379_para01[ 4]={0xb9,0xff,0x83,0x79};
static char tianma_hx8379_para02[ 3]={0xba,0x51,0x93};
static char tianma_hx8379_para03[32]={0xb1,0x00,0x50,0x24,0xea,0x90,0x08,0x11,0x11,0x71,0x2f,0x37,
									0xa6,0x26,0x42,0x0b,0x6e,0xf1,0x00,0xe6,0xe6,0xe6,0xe6,0xe6,
									0x00,0x04,0x05,0x0a,0x0b,0x04,0x05,0x6f};
static char tianma_hx8379_para04[14]={0xb2,0x00,0x00,0xfe,0x19,0x09,0x19,0x22,0x00,0xff,0x19,0x09,
									0x19,0x20};
static char tianma_hx8379_para05[32]={0xb4,0x80,0x0c,0x00,0x32,0x10,0x0e,0x32,0x13,0x5f,0x32,0x10,
									0x28,0x23,0x01,0x27,0x10,0x27,0x04,0x26,0x08,0x2e,0x30,0x08,
									0x00,0x40,0x08,0x28,0x08,0x30,0x30,0x04};
static char tianma_hx8379_para06[ 2]={0xcc,0x02};
static char tianma_hx8379_para07[48]={0xd5,0x00,0x00,0x0a,0x00,0x01,0x05,0x00,0x03,0x00,0x99,0x88,
									0x88,0x88,0x67,0x45,0x23,0x01,0x01,0x23,0x88,0x88,0x88,0x88,
									0x88,0x88,0x88,0x99,0x88,0x88,0x76,0x10,0x32,0x54,0x10,0x32,
									0x88,0x88,0x88,0x88,0x88,0x88,0x00,0x00,0x00,0x00,0x00,0x00};
static char tianma_hx8379_para08[ 4]={0xde,0x05,0x70,0x04};
static char tianma_hx8379_para09[36]={0xe0,0x79,0x20,0x30,0x34,0x3c,0x3f,0x3f,0x3c,0x52,0x07,0x0c,
									0x0f,0x13,0x14,0x13,0x15,0x10,0x12,0x20,0x30,0x34,0x3c,0x3f,
									0x3f,0x3c,0x52,0x07,0x0c,0x0f,0x13,0x14,0x13,0x15,0x10,0x12};
static char tianma_hx8379_para10[ 5]={0xb6,0x00,0x9c,0x00,0x9c};
static char tianma_hx8379_para11[ 3]={0xc6,0x00,0x04};
//static char tianma_hx8379_para12[10]={0xca,0x30,0x29,0x26,0x35,0x23,0x23,0x22,0x22,0x22};
static char tianma_hx8379_para13[35]={0xce,0x1d,0x1d,0x1d,0x1d,0x1d,0x1d,0x1d,0x1d,0x1d,0x1d,0x1d,
									0x1d,0x1d,0x1d,0x1d,0x1d,0x1d,0x1d,0x1d,0x1d,0x1d,0x1d,0x1d,
									0x1d,0x1d,0x1d,0x1d,0x1d,0x1d,0x1d,0x1d,0x1d,0x1d,0x00};
static char tianma_hx8379_para14[10]={0xc9,0x0f,0x40,0x1e,0x1e,0x00,0x00,0x00,0x01,0x3e};

static struct dsi_cmd_desc tianma8379_display_on_cmds[] = 
{                                                           
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(tianma_hx8379_para01), tianma_hx8379_para01},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(tianma_hx8379_para02), tianma_hx8379_para02},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(tianma_hx8379_para03), tianma_hx8379_para03},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(tianma_hx8379_para04), tianma_hx8379_para04},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(tianma_hx8379_para05), tianma_hx8379_para05},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tianma_hx8379_para06), tianma_hx8379_para06},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(tianma_hx8379_para07), tianma_hx8379_para07},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(tianma_hx8379_para08), tianma_hx8379_para08},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(tianma_hx8379_para09), tianma_hx8379_para09},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(tianma_hx8379_para10), tianma_hx8379_para10},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(tianma_hx8379_para11), tianma_hx8379_para11},
	//{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(tianma_hx8379_para12), tianma_hx8379_para12},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(tianma_hx8379_para13), tianma_hx8379_para13},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(tianma_hx8379_para14), tianma_hx8379_para14},
	
	{DTYPE_DCS_WRITE,  1, 0, 0, 120, sizeof(exit_sleep), exit_sleep},
	{DTYPE_DCS_WRITE, 1, 0, 0, 10, sizeof(display_on), display_on}, 
};
static char cmi_nt35510_para01[6] = {0xf0,0x55,0xaa,0x52,0x08,0x01};
static char cmi_nt35510_para02[4] = {0xb0,0x0d,0x0d,0x0d};
static char cmi_nt35510_para03[4] = {0xb6,0x34,0x34,0x34};
static char cmi_nt35510_para04[4] = {0xb1,0x0d,0x0d,0x0d};
static char cmi_nt35510_para05[4] = {0xb7,0x35,0x35,0x35};
static char cmi_nt35510_para06[4] = {0xb2,0x00,0x00,0x00};
static char cmi_nt35510_para07[4] = {0xb8,0x24,0x24,0x24};
static char cmi_nt35510_para08[2] = {0xbf,0x01};
static char cmi_nt35510_para09[4] = {0xb3,0x08,0x08,0x08};
static char cmi_nt35510_para10[4] = {0xb9,0x34,0x34,0x34};
static char cmi_nt35510_para11[4] = {0xba,0x14,0x14,0x14};
static char cmi_nt35510_para12[4] = {0xbc,0x00,0xa0,0x00};
static char cmi_nt35510_para13[4] = {0xbd,0x00,0xa0,0x00};
static char cmi_nt35510_para14[3] = {0xbe,0x00,0x93};
static char cmi_nt35510_para15[53] = {0xd1,0x00,0x05,0x00,0x17,0x00,0x35,0x00,0x5a,
0x00,0x68,0x00,0x8c,0x00,0xb1,0x00,0xe2,0x01,0x0a,0x01,0x4a,
0x01,0x7b,0x01,0xcb,0x02,0x0d,0x02,0x0e,0x02,0x4c,0x02,0x90,
0x02,0xbb,0x02,0xf6,0x03,0x1d,0x03,0x50,0x03,0x79,0x03,0x9a,
0x03,0xae,0x03,0xea,0x03,0xf4,0x03,0xff};
static char cmi_nt35510_para16[53] = {0xd2,0x00,0x05,0x00,0x17,0x00,0x35,0x00,0x5a,
0x00,0x68,0x00,0x8c,0x00,0xb1,0x00,0xe2,0x01,0x0a,0x01,0x4a,
0x01,0x7b,0x01,0xcb,0x02,0x0d,0x02,0x0e,0x02,0x4c,0x02,0x90,
0x02,0xbb,0x02,0xf6,0x03,0x1d,0x03,0x50,0x03,0x79,0x03,0x9a,
0x03,0xae,0x03,0xea,0x03,0xf4,0x03,0xff};
static char cmi_nt35510_para17[53] = {0xd3,0x00,0x05,0x00,0x17,0x00,0x35,0x00,0x5a,
0x00,0x68,0x00,0x8c,0x00,0xb1,0x00,0xe2,0x01,0x0a,0x01,0x4a,
0x01,0x7b,0x01,0xcb,0x02,0x0d,0x02,0x0e,0x02,0x4c,0x02,0x90,
0x02,0xbb,0x02,0xf6,0x03,0x1d,0x03,0x50,0x03,0x79,0x03,0x9a,
0x03,0xae,0x03,0xea,0x03,0xf4,0x03,0xff};
static char cmi_nt35510_para18[53] = {0xd4,0x00,0x05,0x00,0x17,0x00,0x35,0x00,0x5a,
0x00,0x68,0x00,0x8c,0x00,0xb1,0x00,0xe2,0x01,0x0a,0x01,0x4a,
0x01,0x7b,0x01,0xcb,0x02,0x0d,0x02,0x0e,0x02,0x4c,0x02,0x90,
0x02,0xbb,0x02,0xf6,0x03,0x1d,0x03,0x50,0x03,0x79,0x03,0x9a,
0x03,0xae,0x03,0xea,0x03,0xf4,0x03,0xff};
static char cmi_nt35510_para19[53] = {0xd5,0x00,0x05,0x00,0x17,0x00,0x35,0x00,0x5a,
0x00,0x68,0x00,0x8c,0x00,0xb1,0x00,0xe2,0x01,0x0a,0x01,0x4a,
0x01,0x7b,0x01,0xcb,0x02,0x0d,0x02,0x0e,0x02,0x4c,0x02,0x90,
0x02,0xbb,0x02,0xf6,0x03,0x1d,0x03,0x50,0x03,0x79,0x03,0x9a,
0x03,0xae,0x03,0xea,0x03,0xf4,0x03,0xff};
static char cmi_nt35510_para20[53] = {0xd6,0x00,0x05,0x00,0x17,0x00,0x35,0x00,0x5a,
0x00,0x68,0x00,0x8c,0x00,0xb1,0x00,0xe2,0x01,0x0a,0x01,0x4a,
0x01,0x7b,0x01,0xcb,0x02,0x0d,0x02,0x0e,0x02,0x4c,0x02,0x90,
0x02,0xbb,0x02,0xf6,0x03,0x1d,0x03,0x50,0x03,0x79,0x03,0x9a,
0x03,0xae,0x03,0xea,0x03,0xf4,0x03,0xff};
static char cmi_nt35510_para21[6] = {0xf0,0x55,0xaa,0x52,0x08,0x00};
static char cmi_nt35510_para22[3] = {0xb1,0xfc,0x00};
static char cmi_nt35510_para23[2] = {0xb5,0x6b};
static char cmi_nt35510_para24[2] = {0xb6,0x05};
static char cmi_nt35510_para25[3] = {0xb7,0x70,0x70};
static char cmi_nt35510_para26[5] = {0xb8,0x01,0x05,0x05,0x05};
static char cmi_nt35510_para27[4] = {0xbc,0x00,0x00,0x00};
static char cmi_nt35510_para28[6] = {0xc9,0xd0,0x82,0x50,0x28,0x28};
static char cmi_nt35510_para29[6] = {0xbd,0x01,0x6c,0x1d,0x1d,0x00};
static char cmi_nt35510_para30[3] = {0xe0,0x00,0x01};
static char cmi_nt35510_para31[2] = {0x36,0x00};
//static char cmi_nt35510_para32[3] = {0xb1,0xfc,0x02};

static struct dsi_cmd_desc cmint35510_display_on_cmds[] = 
{                                                           
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(cmi_nt35510_para01), cmi_nt35510_para01},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(cmi_nt35510_para02), cmi_nt35510_para02},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(cmi_nt35510_para03), cmi_nt35510_para03},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(cmi_nt35510_para04), cmi_nt35510_para04},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(cmi_nt35510_para05), cmi_nt35510_para05},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(cmi_nt35510_para06), cmi_nt35510_para06},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(cmi_nt35510_para07), cmi_nt35510_para07},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(cmi_nt35510_para08), cmi_nt35510_para08},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(cmi_nt35510_para09), cmi_nt35510_para09},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(cmi_nt35510_para10), cmi_nt35510_para10},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(cmi_nt35510_para11), cmi_nt35510_para11},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(cmi_nt35510_para12), cmi_nt35510_para12},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(cmi_nt35510_para13), cmi_nt35510_para13},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(cmi_nt35510_para14), cmi_nt35510_para14},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(cmi_nt35510_para15), cmi_nt35510_para15},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(cmi_nt35510_para16), cmi_nt35510_para16},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(cmi_nt35510_para17), cmi_nt35510_para17},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(cmi_nt35510_para18), cmi_nt35510_para18},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(cmi_nt35510_para19), cmi_nt35510_para19},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(cmi_nt35510_para20), cmi_nt35510_para20},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(cmi_nt35510_para21), cmi_nt35510_para21},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(cmi_nt35510_para22), cmi_nt35510_para22},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(cmi_nt35510_para23), cmi_nt35510_para23},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(cmi_nt35510_para24), cmi_nt35510_para24},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(cmi_nt35510_para25), cmi_nt35510_para25},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(cmi_nt35510_para26), cmi_nt35510_para26},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(cmi_nt35510_para27), cmi_nt35510_para27},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(cmi_nt35510_para28), cmi_nt35510_para28},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(cmi_nt35510_para29), cmi_nt35510_para29},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(cmi_nt35510_para30), cmi_nt35510_para30},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(cmi_nt35510_para31), cmi_nt35510_para31},
	//{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(cmi_nt35510_para32), cmi_nt35510_para32},
	
	{DTYPE_DCS_WRITE,  1, 0, 0, 120, sizeof(exit_sleep), exit_sleep},
	{DTYPE_DCS_WRITE, 1, 0, 0, 10, sizeof(display_on), display_on}, 
};


static void lcd_panle_reset(void)
{
	gpio_direction_output(GPIO_LCD_RESET,1);
	msleep(10);
	gpio_direction_output(GPIO_LCD_RESET,0);
	msleep(20);
	gpio_direction_output(GPIO_LCD_RESET,1);
	msleep(50);
}

static int firsttimeboot = 1;
static int mipi_lcd_on(struct platform_device *pdev)
{
	struct msm_fb_data_type *mfd;
	mfd = platform_get_drvdata(pdev);
	if (!mfd)
		return -ENODEV;
	if (mfd->key != MFD_KEY)
		return -EINVAL;

	if(firsttimeboot){
		firsttimeboot = 0;
		printk("LCD, first time run!!!!\n");
		return 0;
	}
	
	printk("LCD, mipi_lcd_on , LcdPanleID = %d\n", LcdPanleID);
	lcd_panle_reset();
	/*
	printk("{0x%x,0x%x,0x%x,0x%x,0x%x},\n",MIPI_INP(MIPI_DSI_BASE + 0x0500),MIPI_INP(MIPI_DSI_BASE + 0x0500+4),MIPI_INP(MIPI_DSI_BASE + 0x0500+4*2),MIPI_INP(MIPI_DSI_BASE + 0x0500+4*3),MIPI_INP(MIPI_DSI_BASE + 0x0500+4*4));
	printk("{0x%x,0x%x,0x%x,0x%x,0x%x,0x%x,0x%x,0x%x,0x%x,0x%x,0x%x,0x%x},\n",MIPI_INP(MIPI_DSI_BASE + 0x0440),MIPI_INP(MIPI_DSI_BASE + 0x0440+4),MIPI_INP(MIPI_DSI_BASE + 0x0440+4*2),MIPI_INP(MIPI_DSI_BASE + 0x0440+4*3),MIPI_INP(MIPI_DSI_BASE + 0x0440+4*4),MIPI_INP(MIPI_DSI_BASE + 0x0440+4*5),MIPI_INP(MIPI_DSI_BASE + 0x0440+4*6),MIPI_INP(MIPI_DSI_BASE + 0x0440+4*7),MIPI_INP(MIPI_DSI_BASE + 0x0440+4*8),MIPI_INP(MIPI_DSI_BASE + 0x0440+4*9),MIPI_INP(MIPI_DSI_BASE + 0x0440+4*10),MIPI_INP(MIPI_DSI_BASE + 0x0440+4*11));
	printk("{0x%x,0x%x,0x%x,0x%x},\n",MIPI_INP(MIPI_DSI_BASE + 0x0470),MIPI_INP(MIPI_DSI_BASE + 0x0470+4),MIPI_INP(MIPI_DSI_BASE + 0x0470+4*2),MIPI_INP(MIPI_DSI_BASE + 0x0470+4*3));
	printk("{0x%x,0x%x,0x%x,0x%x},\n",MIPI_INP(MIPI_DSI_BASE + 0x0480),MIPI_INP(MIPI_DSI_BASE + 0x0480+4),MIPI_INP(MIPI_DSI_BASE + 0x0480+4*2),0x00);
	printk("{0x%x,0x%x,0x%x,0x%x,0x%x,0x%x,0x%x,0x%x,0x%x,0x%x,0x%x,0x%x,0x%x,0x%x,0x%x,0x%x,0x%x,0x%x,0x%x,0x%x},\n",MIPI_INP(MIPI_DSI_BASE + 0x200),MIPI_INP(MIPI_DSI_BASE + 0x200+4),MIPI_INP(MIPI_DSI_BASE + 0x200+4*2),MIPI_INP(MIPI_DSI_BASE + 0x200+4*3),MIPI_INP(MIPI_DSI_BASE + 0x200+4*4),MIPI_INP(MIPI_DSI_BASE + 0x200+4*5),MIPI_INP(MIPI_DSI_BASE + 0x200+4*6),MIPI_INP(MIPI_DSI_BASE + 0x200+4*7),MIPI_INP(MIPI_DSI_BASE + 0x200+4*8),MIPI_INP(MIPI_DSI_BASE + 0x200+4*9),MIPI_INP(MIPI_DSI_BASE + 0x200+4*10),MIPI_INP(MIPI_DSI_BASE + 0x200+4*11),MIPI_INP(MIPI_DSI_BASE + 0x200+4*12),MIPI_INP(MIPI_DSI_BASE + 0x200+4*13),MIPI_INP(MIPI_DSI_BASE + 0x200+4*14),MIPI_INP(MIPI_DSI_BASE + 0x200+4*15),MIPI_INP(MIPI_DSI_BASE + 0x200+4*16),MIPI_INP(MIPI_DSI_BASE + 0x200+4*17),MIPI_INP(MIPI_DSI_BASE + 0x200+4*18),MIPI_INP(MIPI_DSI_BASE + 0x200+4*19));
	*/
	mipi_set_tx_power_mode(1);
	switch(LcdPanleID)
	{				
		case LCD_PANEL_4P5_TM_HX8369A_FWVGA:
			printk("LCD: LCD_PANEL_4P5_TM_HX8369A_FWVGA\n");
			mipi_dsi_cmds_tx(&mipi_tx_buf, TM_display_on_cmds, ARRAY_SIZE(TM_display_on_cmds));
			break;
		case LCD_PANEL_4P5_BOE_OTM8009A_FWVGA:
			printk("LCD: LCD_PANEL_4P5_BOE_OTM8009A_FWVGA\n");
			mipi_dsi_cmds_tx(&mipi_tx_buf, BOE_display_on_cmds, ARRAY_SIZE(BOE_display_on_cmds));
			break;					
		case LCD_PANEL_4P5_TM_HX8379A_FWVGA:
			printk("LCD: LCD_PANEL_4P5_TM_HX8379A_FWVGA\n");
			mipi_dsi_cmds_tx(&mipi_tx_buf, tianma8379_display_on_cmds, ARRAY_SIZE(tianma8379_display_on_cmds));
			break;				
		case LCD_PANEL_4P5_CMI_NT35510B_FWVGA:
			printk("LCD: LCD_PANEL_4P5_CMI_NT35510B_FWVGA\n");
			mipi_dsi_cmds_tx(&mipi_tx_buf, cmint35510_display_on_cmds, ARRAY_SIZE(cmint35510_display_on_cmds));
			break;
		default:
			printk("LCD: not found the lcd id, error\n");
			break;
	}
	mipi_set_tx_power_mode(0);
	return 0;
}

static int mipi_lcd_off(struct platform_device *pdev)
{
	struct msm_fb_data_type *mfd;
	
	printk("\nLCD %s: \n", __func__);
	
	mfd = platform_get_drvdata(pdev);
	if (!mfd)
	{
		printk("\nLCD %s no such device: \n", __func__);	
		return -ENODEV;
	}
	if (mfd->key != MFD_KEY)
		return -EINVAL;

	mipi_dsi_cmds_tx(&mipi_tx_buf, display_off_cmds, ARRAY_SIZE(display_off_cmds));

	return 0;
}

static bool onewiremode = true;
static void select_1wire_mode(void)
{
	gpio_direction_output(bl_lpm, 1);
	udelay(120);
	gpio_direction_output(bl_lpm, 0);
	udelay(280);				////ZTE_LCD_LUYA_20100226_001
	gpio_direction_output(bl_lpm, 1);
	udelay(650);				////ZTE_LCD_LUYA_20100226_001
	
}

static void send_bkl_address(void)
{
	unsigned int i,j;
	i = 0x72;
	gpio_direction_output(bl_lpm, 1);
	udelay(10);
	printk("[LY] send_bkl_address \n");
	for(j = 0; j < 8; j++)
	{
		if(i & 0x80)
		{
			gpio_direction_output(bl_lpm, 0);
			udelay(10);
			gpio_direction_output(bl_lpm, 1);
			udelay(180);
		}
		else
		{
			gpio_direction_output(bl_lpm, 0);
			udelay(180);
			gpio_direction_output(bl_lpm, 1);
			udelay(10);
		}
		i <<= 1;
	}
	gpio_direction_output(bl_lpm, 0);
	udelay(10);
	gpio_direction_output(bl_lpm, 1);

}

static void send_bkl_data(int level)
{
	unsigned int i,j;
	i = level & 0x1F;
	gpio_direction_output(bl_lpm, 1);
	udelay(10);
	printk("[LY] send_bkl_data \n");
	for(j = 0; j < 8; j++)
	{
		if(i & 0x80)
		{
			gpio_direction_output(bl_lpm, 0);
			udelay(10);
			gpio_direction_output(bl_lpm, 1);
			udelay(180);
		}
		else
		{
			gpio_direction_output(bl_lpm, 0);
			udelay(180);
			gpio_direction_output(bl_lpm, 1);
			udelay(10);
		}
		i <<= 1;
	}
	gpio_direction_output(bl_lpm, 0);
	udelay(10);
	gpio_direction_output(bl_lpm, 1);

}

static void mipi_set_backlight(struct msm_fb_data_type *mfd)
{
       /*value range is 1--32*/
    int current_lel = mfd->bl_level;
    unsigned long flags;


    printk("lizhiye, [ZYF] lcdc_set_bl level=%d, %d\n", 
		   current_lel , mfd->panel_power_on);

    if(!mfd->panel_power_on)
	  {
    	gpio_direction_output(bl_lpm, 0);			///ZTE_LCD_LUYA_20100201_001
			onewiremode = FALSE;
	    return;
    }

    if(current_lel < 1)
    {
        current_lel = 0;
    }
    if(current_lel > 32)
    {
        current_lel = 32;
    }

    /*ZTE_BACKLIGHT_WLY_005,@2009-11-28, set backlight as 32 levels, end*/
    local_irq_save(flags);
    if(current_lel==0)
    {
    	gpio_direction_output(bl_lpm, 0);
		mdelay(3);
		onewiremode = FALSE;
			
    }
    else 
	{
		if(!onewiremode)	//select 1 wire mode
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

static int __devinit mipi_lcd_probe(struct platform_device *pdev)
{
	if (pdev->id == 0) {
		return 0;
	}
	msm_fb_add_device(pdev);
	return 0;
}

static struct platform_driver this_driver = {
	.probe  = mipi_lcd_probe,
	.driver = {
		.name   = "mipi_warplte",
	},
};

static struct msm_fb_panel_data toshiba_panel_data = {
	.on		= mipi_lcd_on,
	.off		= mipi_lcd_off,
	.set_backlight  = mipi_set_backlight,
};

static int ch_used[3];

int mipi_device_register(struct msm_panel_info *pinfo,
					u32 channel, u32 panel)
{
	struct platform_device *pdev = NULL;
	int ret;

	if ((channel >= 3) || ch_used[channel])
		return -ENODEV;

	ch_used[channel] = TRUE;

	pdev = platform_device_alloc("mipi_warplte", (panel << 8)|channel);
	if (!pdev)
		return -ENOMEM;

	toshiba_panel_data.panel_info = *pinfo;

	ret = platform_device_add_data(pdev, &toshiba_panel_data,
		sizeof(toshiba_panel_data));
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

static int __init mipi_lcd_init(void)
{
	mipi_dsi_buf_alloc(&mipi_tx_buf, DSI_BUF_SIZE);
	mipi_dsi_buf_alloc(&mipi_rx_buf, DSI_BUF_SIZE);

	return platform_driver_register(&this_driver);
}

module_init(mipi_lcd_init);

//pinfo.clk_rate = 384000000;
static int __init mipi_video_wsvga_pt_init(void)
{
		int ret;
		
		pinfo.xres = 480;
		pinfo.yres = 854;
		pinfo.type = MIPI_VIDEO_PANEL;
		pinfo.pdest = DISPLAY_1;
		pinfo.wait_cycle = 0;
		pinfo.bpp = 24;
//		pinfo.clk_rate = 38900000;	//51907008;

	pinfo.lcdc.h_back_porch = 100;
	pinfo.lcdc.h_front_porch = 100;
	pinfo.lcdc.h_pulse_width = 10;
	pinfo.lcdc.v_back_porch = 18;//26;//lead panel must use 26
	pinfo.lcdc.v_front_porch = 10;
	pinfo.lcdc.v_pulse_width = 10;
	pinfo.clk_rate = 420292800;

		pinfo.lcdc.border_clr = 0;	/* blk */
		pinfo.lcdc.underflow_clr = 0xff;	/* blue */
		pinfo.lcdc.hsync_skew = 0;
		pinfo.bl_max = 31;
		pinfo.bl_min = 1;
		pinfo.fb_num = 2;

	
		pinfo.mipi.mode = DSI_VIDEO_MODE;
		pinfo.mipi.pulse_mode_hsa_he = TRUE;
		pinfo.mipi.hfp_power_stop = TRUE;
		pinfo.mipi.hbp_power_stop = TRUE;
		pinfo.mipi.hsa_power_stop = TRUE;
		pinfo.mipi.eof_bllp_power_stop = TRUE;
		pinfo.mipi.bllp_power_stop = TRUE;
		pinfo.mipi.traffic_mode = DSI_BURST_MODE;
		pinfo.mipi.dst_format = DSI_VIDEO_DST_FORMAT_RGB888;
		pinfo.mipi.vc = 0;
		pinfo.mipi.rgb_swap = DSI_RGB_SWAP_BGR;
		pinfo.mipi.data_lane0 = TRUE;
	pinfo.mipi.data_lane1 = TRUE;
	pinfo.mipi.t_clk_post = 0x22;	//0x03;
	pinfo.mipi.t_clk_pre = 0x3d;	//0x24;
		pinfo.mipi.stream = 0; /* dma_p */
		pinfo.mipi.mdp_trigger = DSI_CMD_TRIGGER_SW;
		pinfo.mipi.dma_trigger = DSI_CMD_TRIGGER_SW;
		pinfo.mipi.frame_rate =59;
		pinfo.mipi.dsi_phy_db = dsi_video_mode_phy_dbs;
		pinfo.mipi.dsi_phy_db_count = ARRAY_SIZE(dsi_video_mode_phy_dbs);
		pinfo.mipi.esc_byte_ratio = 4;
		pinfo.mipi.dlane_swap = 0x01;

	ret = mipi_device_register(&pinfo, MIPI_DSI_PRIM,
						MIPI_DSI_PANEL_WVGA_PT);
	if (ret)
		printk(KERN_ERR "%s: failed to register device!\n", __func__);

	return ret;
}

module_init(mipi_video_wsvga_pt_init);

