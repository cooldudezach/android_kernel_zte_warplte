/* Copyright (c) 2008-2011, Code Aurora Forum. All rights reserved.
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
#include "mipi_lead.h"
#include <mach/gpio.h>
#include <linux/gpio.h>
#include <mach/irqs.h>
#include "mdp4.h"
#include <linux/leds.h>
#include <linux/spi/spi.h>

static int wled_trigger_initialized;
DEFINE_LED_TRIGGER(bkl_led_trigger);

#define PM8921_GPIO_BASE		NR_GPIO_IRQS
#define PM8921_GPIO_PM_TO_SYS(pm_gpio)	(pm_gpio - 1 + PM8921_GPIO_BASE)
#define PM8921_MPP_BASE			(PM8921_GPIO_BASE + PM8921_NR_GPIOS)
#define PM8921_MPP_PM_TO_SYS(pm_gpio)	(pm_gpio - 1 + PM8921_MPP_BASE)
#define PM8921_IRQ_BASE			(NR_MSM_IRQS + NR_GPIO_IRQS)

static struct dsi_buf lead_tx_buf;
static struct dsi_buf lead_rx_buf;

extern u32 LcdPanleID;
#define GPIO_LCD_RESET 58
#define GPIO_LCD_ID  3

#define OTM9608A	1
#define NT35516		2


/*about icchip sleep and display on */
static char display_off[2] = {0x28, 0x00};
static char enter_sleep[2] = {0x10, 0x00};
static char exit_sleep[2] = {0x11, 0x00};
static char display_on[2] = {0x29, 0x00};
static struct dsi_cmd_desc display_off_cmds[] = {
	{DTYPE_DCS_WRITE, 1, 0, 0, 50, sizeof(display_off), display_off},
	{DTYPE_DCS_WRITE, 1, 0, 0, 120, sizeof(enter_sleep), enter_sleep}
};


static char qhd_boe_icid_rd_para[] = {0xa1,0x00}; //9608
static struct dsi_cmd_desc qhd_boe_icid_rd_cmd = 
{
	DTYPE_DCS_READ, 1, 0, 0, 0, sizeof(qhd_boe_icid_rd_para), qhd_boe_icid_rd_para
};

static char boe_gen_write_001[]={0x00,0x00};
static char boe_gen_write_002[]={0xFF,0x96,0x08,0x01};
static char boe_gen_write_003[]={0x00,0x80};
static char boe_gen_write_004[]={0xFF,0x96,0x08};
static char boe_gen_write_005[]={0x00,0x00};
static char boe_gen_write_006[]={0xA0,0x00};
static char boe_gen_write_007[]={0x00,0x80};			
static char boe_gen_write_008[]={0xB3,0x00,0x00,0x00,0x21,0x00};			
static char boe_gen_write_009[]={0x00,0x92};			
static char boe_gen_write_010[]={0xB3,0x01};			
static char boe_gen_write_011[]={0x00,0xC0};			
static char boe_gen_write_012[]={0xB3,0x11};			
static char boe_gen_write_013[]={0x00,0x80};			
static char boe_gen_write_014[]={0xC0,0x00,0x48,0x00,0x10,0x08,0x00,0x48,0x10,0x10};			
static char boe_gen_write_015[]={0x00,0x92};	
static char boe_gen_write_016[]={0xC0,0x00,0x35,0x00,0x38,0xC0,0x00,0x17,0x00,0x1A};	
static char boe_gen_write_017[]={0x00,0xA2};	
static char boe_gen_write_018[]={0xC0,0x01,0x10,0x00};	
static char boe_gen_write_019[]={0x00,0xB3};	
static char boe_gen_write_020[]={0xC0,0x00,0x50};	
static char boe_gen_write_021[]={0x00,0x81};	
static char boe_gen_write_022[]={0xC1,0x66};//77

static char boe_gen_write_shift_a0[]={0x00,0xa0};	
static char boe_gen_write_cmd_c1[]={0xC1,0x04};//screen scroll

static char boe_gen_write_023[]={0x00,0x80};
static char boe_gen_write_024[]={0xC4,0x00,0x84,0xFA,0x00,0x84,0xFA};
static char boe_gen_write_025[]={0x00,0xA0};
static char boe_gen_write_026[]={0xC4,0x33,0x09,0x90,0x2B,0x33,0x09,0x90,0x54};
static char boe_gen_write_027[]={0x00,0x80};
static char boe_gen_write_028[]={0xC5,0x08,0x00,0x90,0x11};
static char boe_gen_write_029[]={0x00,0x90};
static char boe_gen_write_030[]={0xC5,0x96,0x08,0x00,0x77,0x34,0x34,0x34};
static char boe_gen_write_031[]={0x00,0xA0};
static char boe_gen_write_032[]={0xC5,0x96,0x08,0x00,0x77,0x34,0x34,0x34};
static char boe_gen_write_033[]={0x00,0xB0};
static char boe_gen_write_034[]={0xC5,0x04,0xF8};
static char boe_gen_write_035[]={0x00,0x80};
static char boe_gen_write_036[]={0xC6,0x64};
static char boe_gen_write_037[]={0x00,0xB0};
static char boe_gen_write_038[]={0xC6,0x03,0x10,0x00,0x1F,0x12};
static char boe_gen_write_039[]={0x00,0xE1};
static char boe_gen_write_040[]={0xC0,0x9F};
static char boe_gen_write_041[]={0x00,0x00};
static char boe_gen_write_042[]={0xD0,0x01};
static char boe_gen_write_043[]={0x00,0x00};
static char boe_gen_write_044[]={0xD1,0x01,0x01};
static char boe_gen_write_045[]={0x00,0xB7};
static char boe_gen_write_046[]={0xB0,0x10};
static char boe_gen_write_047[]={0x00,0xC0};
static char boe_gen_write_048[]={0xB0,0x55};
static char boe_gen_write_049[]={0x00,0xB1};
static char boe_gen_write_050[]={0xB0,0x03,0x06};
static char boe_gen_write_051[]={0x00,0x80};
static char boe_gen_write_052[]={0xCB,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
static char boe_gen_write_053[]={0x00,0x90};
static char boe_gen_write_054[]={0xCB,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
static char boe_gen_write_055[]={0x00,0xA0};
static char boe_gen_write_056[]={0xCB,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
static char boe_gen_write_057[]={0x00,0xB0};
static char boe_gen_write_058[]={0xCB,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
static char boe_gen_write_059[]={0x00,0xC0};
static char boe_gen_write_060[]={0xCB,0x00,0x00,0x04,0x04,0x04,0x04,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
static char boe_gen_write_061[]={0x00,0xD0};
static char boe_gen_write_062[]={0xCB,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x04,0x04,0x04,0x04,0x00,0x00,0x00};
static char boe_gen_write_063[]={0x00,0xE0};
static char boe_gen_write_064[]={0xCB,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
static char boe_gen_write_065[]={0x00,0xF0};
static char boe_gen_write_066[]={0xCB,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
static char boe_gen_write_067[]={0x00,0x80};
static char boe_gen_write_068[]={0xCC,0x00,0x00,0x0B,0x09,0x01,0x25,0x26,0x00,0x00,0x00};
static char boe_gen_write_069[]={0x00,0x90};
static char boe_gen_write_070[]={0xCC,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0C,0x0A,0x02};
static char boe_gen_write_071[]={0x00,0xA0};
static char boe_gen_write_072[]={0xCC,0x25,0x26,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
static char boe_gen_write_073[]={0x00,0xB0};
static char boe_gen_write_074[]={0xCC,0x00,0x00,0x0A,0x0C,0x02,0x26,0x25,0x00,0x00,0x00};
static char boe_gen_write_075[]={0x00,0xC0};
static char boe_gen_write_076[]={0xCC,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x09,0x0B,0x01};
static char boe_gen_write_077[]={0x00,0xD0};
static char boe_gen_write_078[]={0xCC,0x26,0x25,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
static char boe_gen_write_079[]={0x00,0x80};
static char boe_gen_write_080[]={0xCE,0x85,0x01,0x00,0x84,0x01,0x00,0x0F,0x00,0x00,0x0F,0x00,0x00};
static char boe_gen_write_081[]={0x00,0x90};
static char boe_gen_write_082[]={0xCE,0xF0,0x00,0x00,0xf0,0x00,0x00,0xF0,0x00,0x00,0xF0,0x00,0x00,0x00,0x00};
static char boe_gen_write_083[]={0x00,0xA0};
static char boe_gen_write_084[]={0xCE,0x18,0x02,0x03,0xC2,0x86,0x00,0x22,0x18,0x01,0x03,0xC4,0x86,0x00,0x22};
static char boe_gen_write_085[]={0x00,0xB0};
static char boe_gen_write_086[]={0xCE,0x18,0x04,0x03,0xC3,0x86,0x00,0x22,0x18,0x03,0x03,0xC5,0x86,0x00,0x22};
static char boe_gen_write_087[]={0x00,0xC0};
static char boe_gen_write_088[]={0xCE,0xF0,0x00,0x00,0x10,0x00,0x00,0x00,0xF0,0x00,0x00,0x10,0x00,0x00,0x00};
static char boe_gen_write_089[]={0x00,0xD0};
static char boe_gen_write_090[]={0xCE,0xF0,0x00,0x00,0x10,0x00,0x00,0x00,0xF0,0x00,0x00,0x10,0x00,0x00,0x00};
static char boe_gen_write_091[]={0x00,0x80};
static char boe_gen_write_092[]={0xCF,0xF0,0x00,0x00,0x10,0x00,0x00,0x00,0xF0,0x00,0x00,0x10,0x00,0x00,0x00};
static char boe_gen_write_093[]={0x00,0x90};
static char boe_gen_write_094[]={0xCF,0xF0,0x00,0x00,0x10,0x00,0x00,0x00,0xF0,0x00,0x00,0x10,0x00,0x00,0x00};
static char boe_gen_write_095[]={0x00,0xA0};
static char boe_gen_write_096[]={0xCF,0xF0,0x00,0x00,0x10,0x00,0x00,0x00,0xF0,0x00,0x00,0x10,0x00,0x00,0x00};
static char boe_gen_write_097[]={0x00,0xB0};
static char boe_gen_write_098[]={0xCF,0xF0,0x00,0x00,0x10,0x00,0x00,0x00,0xF0,0x00,0x00,0x10,0x00,0x00,0x00};
static char boe_gen_write_099[]={0x00,0xC0};
static char boe_gen_write_100[]={0xCF,0x02,0x02,0x20,0x20,0x00,0x00,0x01,0x02,0x00,0x00};
static char boe_gen_write_101[]={0x00,0x80};
static char boe_gen_write_102[]={0xD6,0x00};
static char boe_gen_write_103[]={0x00,0x00};										
static char boe_gen_write_104[]={0xD7,0x00};													
static char boe_gen_write_105[]={0x00,0x00};												
static char boe_gen_write_106[]={0xD8,0x75,0x75};														
static char boe_gen_write_107[]={0x00,0x00};
static char boe_gen_write_108[]={0xD9,0x5D};	//7f
static char boe_gen_write_109[]={0x00,0x00};														
static char boe_gen_write_110[]={0xE1,0x00, 0x08, 0x0E, 0x0D, 0x06, 0x10, 0x0B, 0x0A};//G2.2 POS				
static char boe_gen_write_111[]={0xE1,0x03, 0x06 ,0x0A ,0x05 ,0x0F ,0x11 ,0x0C ,0x00};//G2.2 POS				
static char boe_gen_write_112[]={0x00,0x00};
static char boe_gen_write_113[]={0xE2,0x00, 0x09, 0x0D, 0x0D, 0x06, 0x10, 0x0B, 0x0A};//G2.2 POS
static char boe_gen_write_114[]={0xE2,0x03, 0x06, 0x09, 0x05 ,0x0F ,0x12 ,0x0C ,0x00};//G2.2 POS
static char boe_gen_write_115[]={0x00,0x00};				   
static char boe_gen_write_116[]={0xFF,0xFF,0xFF,0xFF};	
static char boe_gen_write_TE[]={0x35,0x00};//open TE
static char boe_gen_write_44[]={0x44,0x00,0xd0};//TE relatived

static struct dsi_cmd_desc boe_OTM9608A_video_on_cmds[] = {
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(boe_gen_write_001), boe_gen_write_001},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(boe_gen_write_002), boe_gen_write_002},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(boe_gen_write_003), boe_gen_write_003},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(boe_gen_write_004), boe_gen_write_004},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(boe_gen_write_005), boe_gen_write_005},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(boe_gen_write_006), boe_gen_write_006},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(boe_gen_write_007), boe_gen_write_007},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(boe_gen_write_008), boe_gen_write_008},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(boe_gen_write_009), boe_gen_write_009},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(boe_gen_write_010), boe_gen_write_010},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(boe_gen_write_011), boe_gen_write_011},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(boe_gen_write_012), boe_gen_write_012},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(boe_gen_write_013), boe_gen_write_013},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(boe_gen_write_014), boe_gen_write_014},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(boe_gen_write_015), boe_gen_write_015},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(boe_gen_write_016), boe_gen_write_016},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(boe_gen_write_017), boe_gen_write_017},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(boe_gen_write_018), boe_gen_write_018},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(boe_gen_write_019), boe_gen_write_019},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(boe_gen_write_020), boe_gen_write_020},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(boe_gen_write_021), boe_gen_write_021},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(boe_gen_write_022), boe_gen_write_022},

	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(boe_gen_write_shift_a0), boe_gen_write_shift_a0},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(boe_gen_write_cmd_c1), boe_gen_write_cmd_c1},//add for only recognize v sync
	
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(boe_gen_write_023), boe_gen_write_023},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(boe_gen_write_024), boe_gen_write_024},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(boe_gen_write_025), boe_gen_write_025},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(boe_gen_write_026), boe_gen_write_026},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(boe_gen_write_027), boe_gen_write_027},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(boe_gen_write_028), boe_gen_write_028},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(boe_gen_write_029), boe_gen_write_029},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(boe_gen_write_030), boe_gen_write_030},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(boe_gen_write_031), boe_gen_write_031},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(boe_gen_write_032), boe_gen_write_032},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(boe_gen_write_033), boe_gen_write_033},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(boe_gen_write_034), boe_gen_write_034},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(boe_gen_write_035), boe_gen_write_035},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(boe_gen_write_036), boe_gen_write_036},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(boe_gen_write_037), boe_gen_write_037},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(boe_gen_write_038), boe_gen_write_038},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(boe_gen_write_039), boe_gen_write_039},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(boe_gen_write_040), boe_gen_write_040},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(boe_gen_write_041), boe_gen_write_041},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(boe_gen_write_042), boe_gen_write_042},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(boe_gen_write_043), boe_gen_write_043},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(boe_gen_write_044), boe_gen_write_044},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(boe_gen_write_045), boe_gen_write_045},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(boe_gen_write_046), boe_gen_write_046},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(boe_gen_write_047), boe_gen_write_047},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(boe_gen_write_048), boe_gen_write_048},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(boe_gen_write_049), boe_gen_write_049},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(boe_gen_write_050), boe_gen_write_050},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(boe_gen_write_051), boe_gen_write_051},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(boe_gen_write_052), boe_gen_write_052},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(boe_gen_write_053), boe_gen_write_053},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(boe_gen_write_054), boe_gen_write_054},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(boe_gen_write_055), boe_gen_write_055},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(boe_gen_write_056), boe_gen_write_056},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(boe_gen_write_057), boe_gen_write_057},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(boe_gen_write_058), boe_gen_write_058},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(boe_gen_write_059), boe_gen_write_059},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(boe_gen_write_060), boe_gen_write_060},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(boe_gen_write_061), boe_gen_write_061},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(boe_gen_write_062), boe_gen_write_062},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(boe_gen_write_063), boe_gen_write_063},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(boe_gen_write_064), boe_gen_write_064},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(boe_gen_write_065), boe_gen_write_065},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(boe_gen_write_066), boe_gen_write_066},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(boe_gen_write_067), boe_gen_write_067},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(boe_gen_write_068), boe_gen_write_068},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(boe_gen_write_069), boe_gen_write_069},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(boe_gen_write_070), boe_gen_write_070},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(boe_gen_write_071), boe_gen_write_071},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(boe_gen_write_072), boe_gen_write_072},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(boe_gen_write_073), boe_gen_write_073},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(boe_gen_write_074), boe_gen_write_074},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(boe_gen_write_075), boe_gen_write_075},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(boe_gen_write_076), boe_gen_write_076},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(boe_gen_write_077), boe_gen_write_077},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(boe_gen_write_078), boe_gen_write_078},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(boe_gen_write_079), boe_gen_write_079},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(boe_gen_write_080), boe_gen_write_080},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(boe_gen_write_081), boe_gen_write_081},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(boe_gen_write_082), boe_gen_write_082},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(boe_gen_write_083), boe_gen_write_083},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(boe_gen_write_084), boe_gen_write_084},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(boe_gen_write_085), boe_gen_write_085},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(boe_gen_write_086), boe_gen_write_086},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(boe_gen_write_087), boe_gen_write_087},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(boe_gen_write_088), boe_gen_write_088},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(boe_gen_write_089), boe_gen_write_089},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(boe_gen_write_090), boe_gen_write_090},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(boe_gen_write_091), boe_gen_write_091},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(boe_gen_write_092), boe_gen_write_092},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(boe_gen_write_093), boe_gen_write_093},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(boe_gen_write_094), boe_gen_write_094},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(boe_gen_write_095), boe_gen_write_095},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(boe_gen_write_096), boe_gen_write_096},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(boe_gen_write_097), boe_gen_write_097},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(boe_gen_write_098), boe_gen_write_098},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(boe_gen_write_099), boe_gen_write_099},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(boe_gen_write_100), boe_gen_write_100},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(boe_gen_write_101), boe_gen_write_101},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(boe_gen_write_102), boe_gen_write_102},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(boe_gen_write_103), boe_gen_write_103},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(boe_gen_write_104), boe_gen_write_104},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(boe_gen_write_105), boe_gen_write_105},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(boe_gen_write_106), boe_gen_write_106},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(boe_gen_write_107), boe_gen_write_107},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(boe_gen_write_108), boe_gen_write_108},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(boe_gen_write_109), boe_gen_write_109},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(boe_gen_write_110), boe_gen_write_110},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(boe_gen_write_111), boe_gen_write_111},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(boe_gen_write_112), boe_gen_write_112},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(boe_gen_write_113), boe_gen_write_113},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(boe_gen_write_114), boe_gen_write_114},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(boe_gen_write_115), boe_gen_write_115},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(boe_gen_write_116), boe_gen_write_116},
	{DTYPE_DCS_WRITE,  1, 0, 0, 120, sizeof(exit_sleep), exit_sleep},
	{DTYPE_DCS_WRITE, 1, 0, 0, 15, sizeof(display_on), display_on},

	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(boe_gen_write_TE), boe_gen_write_TE}, 
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(boe_gen_write_44), boe_gen_write_44},
};

/*2.yushun, 4.5 OTM9608A*/
static char yushun_qhd_otm9608a_offset1[]={0x00,0x00};
static char yushun_qhd_otm9608a_3P_1[]={0xFF,0x96,0x08,0x01};

static char yushun_qhd_otm9608a_offset2[]={0x00,0x80};
static char yushun_qhd_otm9608a_2P_2[]={0xFF,0x96,0x08};

static char yushun_qhd_otm9608a_offset3[]={0x00,0x00};
static char yushun_qhd_otm9608a_1P_3[]={0xA0,0x00};

static char yushun_qhd_otm9608a_offset4[]={0x00,0x80};
static char yushun_qhd_otm9608a_5P_4[]={0xB3,0x00,0x00,0x20,0x00,0x00};	

static char yushun_qhd_otm9608a_offset5[]={0x00,0xc0};
static char yushun_qhd_otm9608a_1P_5[]={0xB3,0x09};

static char yushun_qhd_otm9608a_offset7[]={0x00,0x80};
static char yushun_qhd_otm9608a_9P_7[]={0xC0,0x00,0x48,0x00,0x10,0x10,0x00,0x47,0x10,0x10};

static char yushun_qhd_otm9608a_offset8[]={0x00,0x92};
static char yushun_qhd_otm9608a_4P_8[]={0xC0,0x00,0x10,0x00,0x13};

static char yushun_qhd_otm9608a_offset9[]={0x00,0xA2};
static char yushun_qhd_otm9608a_3P_9[]={0xC0,0x0c,0x05,0x02};

static char yushun_qhd_otm9608a_offset10[]={0x00,0xB3};
static char yushun_qhd_otm9608a_2P_10[]={0xC0,0x00,0x50};

static char yushun_qhd_otm9608a_offset11[]={0x00,0x81};
static char yushun_qhd_otm9608a_1P_11[]={0xC1,0x66};//55//77

static char yushun_qhd_otm9608a_shift_a0[]={0x00,0xa0};	
static char yushun_qhd_otm9608a_cmd_c1[]={0xC1,0x04};//screen scroll

static char yushun_qhd_otm9608a_offset12[]={0x00,0x80};
static char yushun_qhd_otm9608a_3P_12[]={0xC4,0x30,0x84,0xFC};

static char yushun_qhd_otm9608a_offset12_1[]={0x00,0x88};//++
static char yushun_qhd_otm9608a_3P_12_1[]={0xC4,0x40};//++

static char yushun_qhd_otm9608a_offset13[]={0x00,0xA0};//
static char yushun_qhd_otm9608a_8P_13[]={0xB3,0x10,0x00};

static char yushun_qhd_otm9608a_offset13_1[]={0x00,0xA0};//
static char yushun_qhd_otm9608a_8P_13_1[]={0xc0,0x00};

static char yushun_qhd_otm9608a_offset13_2[]={0x00,0xA0};
static char yushun_qhd_otm9608a_8P_13_2[]={0xC4,0x33,0x09,0x90,0x2B,0x33,0x09,0x90,0x54};

static char yushun_qhd_otm9608a_offset14[]={0x00,0x80};
static char yushun_qhd_otm9608a_4P_14[]={0xC5,0x08,0x00,0xa0,0x11};

static char yushun_qhd_otm9608a_offset15[]={0x00,0x90};
static char yushun_qhd_otm9608a_7P_15[]={0xC5,0xd6,0x57,0x00,0x57,0x33,0x33,0x34};	//96

static char yushun_qhd_otm9608a_offset16[]={0x00,0xA0};
static char yushun_qhd_otm9608a_7P_16[]={0xC5,0x96,0x57,0x00,0x57,0x33,0x33,0x34};	

static char yushun_qhd_otm9608a_offset17[]={0x00,0xB0};
static char yushun_qhd_otm9608a_2P_17[]={0xC5,0x04,0xac,0x01,0x00,0x71,0xb1,0x83};

static char yushun_qhd_otm9608a_offset18[]={0x00,0x00};//
static char yushun_qhd_otm9608a_1P_18[]={0xd9,0x61};

static char yushun_qhd_otm9608a_offset19[]={0x00,0x80};
static char yushun_qhd_otm9608a_1P_19[]={0xC6,0x64};

static char yushun_qhd_otm9608a_offset20[]={0x00,0xB0};
static char yushun_qhd_otm9608a_5P_20[]={0xC6,0x03,0x10,0x00,0x1F,0x12};

static char yushun_qhd_otm9608a_offset23[]={0x00,0xB7};	
static char yushun_qhd_otm9608a_1P_23[]={0xB0,0x10};	

static char yushun_qhd_otm9608a_offset24[]={0x00,0xC0};	
static char yushun_qhd_otm9608a_1P_24[]={0xB0,0x55};	

static char yushun_qhd_otm9608a_offset25[]={0x00,0xB1};	
static char yushun_qhd_otm9608a_2P_25[]={0xB0,0x03};

static char yushun_qhd_otm9608a_offset25_1[]={0x00,0x81};	//
static char yushun_qhd_otm9608a_2P_25_1[]={0xB6,0x00};//

static char yushun_qhd_otm9608a_offset26[]={0x00,0x00};	//
static char yushun_qhd_otm9608a_10P_26[]={0xe1,0x01,0x0d,0x13,0x0f,0x07,0x11,0x0b,0x0a,0x03,0x06,0x0B,0x08,0x0D,0x0E,0x09,0x01};	//

static char yushun_qhd_otm9608a_offset26_1[]={0x00,0x00};	//
static char yushun_qhd_otm9608a_10P_26_1[]={0xe2,0x02,0x0F,0x15,0x0E,0x08,0x10,0x0B,0x0C,0x02,0x04,0x0B,0x04,0x0E,0x0D,0x08,0x00};	//

static char yushun_qhd_otm9608a_offset26_2[]={0x00,0x80};	
static char yushun_qhd_otm9608a_10P_26_2[]={0xCB,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};	

static char yushun_qhd_otm9608a_offset27[]={0x00,0x90};
static char yushun_qhd_otm9608a_15P_27[]={0xCB,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

static char yushun_qhd_otm9608a_offset28[]={0x00,0xA0};
static char yushun_qhd_otm9608a_15P_28[]={0xCB,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

static char yushun_qhd_otm9608a_offset29[]={0x00,0xB0};	
static char yushun_qhd_otm9608a_10P_29[]={0xCB,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

static char yushun_qhd_otm9608a_offset30[]={0x00,0xC0};
static char yushun_qhd_otm9608a_15P_30[]={0xCB,0x04,0x04,0x04,0x04,0x08,0x04,0x08,0x04,0x08,0x04,0x08,0x04,0x04,0x04,0x08};

static char yushun_qhd_otm9608a_offset31[]={0x00,0xD0};
static char yushun_qhd_otm9608a_15P_31[]={0xCB,0x08,0x00,0x00,0x00,0x00,0x04,0x04,0x04,0x04,0x08,0x04,0x08,0x04,0x08,0x04};

static char yushun_qhd_otm9608a_offset32[]={0x00,0xE0};	
static char yushun_qhd_otm9608a_10P_32[]={0xCB,0x08,0x04,0x04,0x04,0x08,0x08,0x00,0x00,0x00,0x00};

static char yushun_qhd_otm9608a_offset33[]={0x00,0xF0};
static char yushun_qhd_otm9608a_10P_33[]={0xCB,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};	

static char yushun_qhd_otm9608a_offset34[]={0x00,0x80};	
static char yushun_qhd_otm9608a_10P_34[]={0xCC,0x26,0x25,0x23,0x24,0x00,0x0f,0x00,0x0d,0x00,0x0b};

static char yushun_qhd_otm9608a_offset35[]={0x00,0x90};
static char yushun_qhd_otm9608a_15P_35[]={0xCC,0x00,0x09,0x01,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x26,0x25,0x21,0x22,0x00};

static char yushun_qhd_otm9608a_offset36[]={0x00,0xA0};
static char yushun_qhd_otm9608a_15P_36[]={0xCC,0x10,0x00,0x0e,0x00,0x0c,0x00,0x0a,0x02,0x04,0x00,0x00,0x00,0x00,0x00,0x00};

static char yushun_qhd_otm9608a_offset37[]={0x00,0xB0};
static char yushun_qhd_otm9608a_10P_37[]={0xCC,0x25,0x26,0x21,0x22,0x00,0x0a,0x00,0x0c,0x00,0x0e};	

static char yushun_qhd_otm9608a_offset38[]={0x00,0xC0};
static char yushun_qhd_otm9608a_15P_38[]={0xCC,0x00,0x10,0x04,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x25,0x26,0x23,0x24,0x00};

static char yushun_qhd_otm9608a_offset39[]={0x00,0xD0};	
static char yushun_qhd_otm9608a_15P_39[]={0xCC,0x09,0x00,0x0b,0x00,0x0d,0x00,0x0f,0x03,0x01,0x00,0x00,0x00,0x00,0x00,0x00};

static char yushun_qhd_otm9608a_offset40[]={0x00,0x80};	
static char yushun_qhd_otm9608a_12P_40[]={0xCE,0x8a,0x03,0x06,0x89,0x03,0x06,0x88,0x03,0x06,0x87,0x03,0x06};	

static char yushun_qhd_otm9608a_offset41[]={0x00,0x90};
static char yushun_qhd_otm9608a_14P_41[]={0xCE,0xf0,0x00,0x00,0xf0,0x00,0x00,0xf0,0x00,0x00,0xf0,0x00,0x00,0x00,0x00};	

static char yushun_qhd_otm9608a_offset42[]={0x00,0xA0};	
static char yushun_qhd_otm9608a_14P_42[]={0xCE,0x38,0x02,0x03,0xc1,0x00,0x06,0x00,0x38,0x01,0x03,0xc2,0x00,0x06,0x00};

static char yushun_qhd_otm9608a_offset43[]={0x00,0xB0};	
static char yushun_qhd_otm9608a_14P_43[]={0xCE,0x38,0x00,0x03,0xc3,0x00,0x06,0x00,0x30,0x00,0x03,0xc4,0x00,0x06,0x00};

static char yushun_qhd_otm9608a_offset44[]={0x00,0xC0};
static char yushun_qhd_otm9608a_14P_44[]={0xCE,0x38,0x06,0x03,0xbd,0x00,0x06,0x00,0x38,0x05,0x03,0xbe,0x00,0x06,0x00};	

static char yushun_qhd_otm9608a_offset45[]={0x00,0xD0};	
static char yushun_qhd_otm9608a_14P_45[]={0xCE,0x38,0x04,0x03,0xbf,0x00,0x06,0x00,0x38,0x03,0x03,0xc0,0x00,0x06,0x00};	

static char yushun_qhd_otm9608a_offset46[]={0x00,0x80};	
static char yushun_qhd_otm9608a_14P_46[]={0xCF,0xf0,0x00,0x00,0x10,0x00,0x00,0x00,0xf0,0x00,0x00,0x10,0x00,0x00,0x00};	

static char yushun_qhd_otm9608a_offset47[]={0x00,0x90};	
static char yushun_qhd_otm9608a_14P_47[]={0xCF,0xf0,0x00,0x00,0x10,0x00,0x00,0x00,0xf0,0x00,0x00,0x10,0x00,0x00,0x00};	

static char yushun_qhd_otm9608a_offset48[]={0x00,0xA0};	
static char yushun_qhd_otm9608a_14P_48[]={0xCF,0xf0,0x00,0x00,0x10,0x00,0x00,0x00,0xf0,0x00,0x00,0x10,0x00,0x00,0x00};	

static char yushun_qhd_otm9608a_offset49[]={0x00,0xB0};	
static char yushun_qhd_otm9608a_14P_49[]={0xCF,0xf0,0x00,0x00,0x10,0x00,0x00,0x00,0xf0,0x00,0x00,0x10,0x00,0x00,0x00};	

static char yushun_qhd_otm9608a_offset50[]={0x00,0xC0};	
static char yushun_qhd_otm9608a_10P_50[]={0xCF,0x02,0x02,0x20,0x20,0x00,0x00,0x01,0x00,0x00,0x02};	

static char yushun_qhd_otm9608a_offset52[]={0x00,0x00};	//
static char yushun_qhd_otm9608a_1P_52[]={0xD8,0xA7,0xA7};	

static char yushun_qhd_otm9608a_offset57[]={0x00,0x00};	
static char yushun_qhd_otm9608a_3P_57[]={0xFF,0xFF,0xFF,0xFF};

static char yushun_qhd_otm9608a_offset58[]={0x00,0x00};	
static char yushun_qhd_otm9608a_1P_58[]={0x3A,0x77};

static char yushun_qhd_otm9608a_offset59[]={0x00,0x00};	//
static char yushun_qhd_otm9608a_1P_59[]={0x2c,0x00};

static char yushun_qhd_otm9608a_TE[]={0x35,0x00};//open TE
static char yushun_qhd_otm9608a_44[]={0x44,0x00,0xd0};//TE relatived
	
static struct dsi_cmd_desc yushun_OTM9608A_video_on_cmds[] = {
	{DTYPE_GEN_WRITE2, 1, 0, 0, 0, sizeof(yushun_qhd_otm9608a_offset1), yushun_qhd_otm9608a_offset1}, 
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(yushun_qhd_otm9608a_3P_1), yushun_qhd_otm9608a_3P_1},
	{DTYPE_GEN_WRITE2, 1, 0, 0, 0, sizeof(yushun_qhd_otm9608a_offset2), yushun_qhd_otm9608a_offset2},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(yushun_qhd_otm9608a_2P_2), yushun_qhd_otm9608a_2P_2},
	{DTYPE_GEN_WRITE2, 1, 0, 0, 0, sizeof(yushun_qhd_otm9608a_offset3), yushun_qhd_otm9608a_offset3},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(yushun_qhd_otm9608a_1P_3), yushun_qhd_otm9608a_1P_3},
	{DTYPE_GEN_WRITE2, 1, 0, 0, 0, sizeof(yushun_qhd_otm9608a_offset4), yushun_qhd_otm9608a_offset4},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(yushun_qhd_otm9608a_5P_4), yushun_qhd_otm9608a_5P_4},
	{DTYPE_GEN_WRITE2, 1, 0, 0, 0, sizeof(yushun_qhd_otm9608a_offset5), yushun_qhd_otm9608a_offset5},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(yushun_qhd_otm9608a_1P_5), yushun_qhd_otm9608a_1P_5},
	{DTYPE_GEN_WRITE2, 1, 0, 0, 0, sizeof(yushun_qhd_otm9608a_offset7), yushun_qhd_otm9608a_offset7},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(yushun_qhd_otm9608a_9P_7), yushun_qhd_otm9608a_9P_7},
	{DTYPE_GEN_WRITE2, 1, 0, 0, 0, sizeof(yushun_qhd_otm9608a_offset8), yushun_qhd_otm9608a_offset8},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(yushun_qhd_otm9608a_4P_8), yushun_qhd_otm9608a_4P_8},
	{DTYPE_GEN_WRITE2, 1, 0, 0, 0, sizeof(yushun_qhd_otm9608a_offset9), yushun_qhd_otm9608a_offset9},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(yushun_qhd_otm9608a_3P_9), yushun_qhd_otm9608a_3P_9},
	{DTYPE_GEN_WRITE2, 1, 0, 0, 0, sizeof(yushun_qhd_otm9608a_offset10), yushun_qhd_otm9608a_offset10},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(yushun_qhd_otm9608a_2P_10), yushun_qhd_otm9608a_2P_10},
	{DTYPE_GEN_WRITE2, 1, 0, 0, 0, sizeof(yushun_qhd_otm9608a_offset11), yushun_qhd_otm9608a_offset11},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(yushun_qhd_otm9608a_1P_11), yushun_qhd_otm9608a_1P_11},

	{DTYPE_GEN_WRITE2, 1, 0, 0, 0, sizeof(yushun_qhd_otm9608a_shift_a0), yushun_qhd_otm9608a_shift_a0},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(yushun_qhd_otm9608a_cmd_c1), yushun_qhd_otm9608a_cmd_c1},

	
	{DTYPE_GEN_WRITE2, 1, 0, 0, 0, sizeof(yushun_qhd_otm9608a_offset12), yushun_qhd_otm9608a_offset12},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(yushun_qhd_otm9608a_3P_12), yushun_qhd_otm9608a_3P_12},
	
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(yushun_qhd_otm9608a_offset12_1), yushun_qhd_otm9608a_offset12_1},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(yushun_qhd_otm9608a_3P_12_1), yushun_qhd_otm9608a_3P_12_1},

	{DTYPE_GEN_WRITE2, 1, 0, 0, 0, sizeof(yushun_qhd_otm9608a_offset13), yushun_qhd_otm9608a_offset13},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(yushun_qhd_otm9608a_8P_13), yushun_qhd_otm9608a_8P_13},
	{DTYPE_GEN_WRITE2, 1, 0, 0, 0, sizeof(yushun_qhd_otm9608a_offset13_1), yushun_qhd_otm9608a_offset13_1},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(yushun_qhd_otm9608a_8P_13_1), yushun_qhd_otm9608a_8P_13_1},
	{DTYPE_GEN_WRITE2, 1, 0, 0, 0, sizeof(yushun_qhd_otm9608a_offset13_2), yushun_qhd_otm9608a_offset13_2},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(yushun_qhd_otm9608a_8P_13_2), yushun_qhd_otm9608a_8P_13_2},
	{DTYPE_GEN_WRITE2, 1, 0, 0, 0, sizeof(yushun_qhd_otm9608a_offset14), yushun_qhd_otm9608a_offset14},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(yushun_qhd_otm9608a_4P_14), yushun_qhd_otm9608a_4P_14},
	{DTYPE_GEN_WRITE2, 1, 0, 0, 0, sizeof(yushun_qhd_otm9608a_offset15), yushun_qhd_otm9608a_offset15},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(yushun_qhd_otm9608a_7P_15), yushun_qhd_otm9608a_7P_15},
	{DTYPE_GEN_WRITE2, 1, 0, 0, 0, sizeof(yushun_qhd_otm9608a_offset16), yushun_qhd_otm9608a_offset16},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(yushun_qhd_otm9608a_7P_16), yushun_qhd_otm9608a_7P_16},
	{DTYPE_GEN_WRITE2, 1, 0, 0, 0, sizeof(yushun_qhd_otm9608a_offset17), yushun_qhd_otm9608a_offset17},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(yushun_qhd_otm9608a_2P_17), yushun_qhd_otm9608a_2P_17},
	{DTYPE_GEN_WRITE2, 1, 0, 0, 0, sizeof(yushun_qhd_otm9608a_offset18), yushun_qhd_otm9608a_offset18},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(yushun_qhd_otm9608a_1P_18), yushun_qhd_otm9608a_1P_18},
	{DTYPE_GEN_WRITE2, 1, 0, 0, 0, sizeof(yushun_qhd_otm9608a_offset19), yushun_qhd_otm9608a_offset19},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(yushun_qhd_otm9608a_1P_19), yushun_qhd_otm9608a_1P_19},
	{DTYPE_GEN_WRITE2, 1, 0, 0, 0, sizeof(yushun_qhd_otm9608a_offset20), yushun_qhd_otm9608a_offset20},	
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(yushun_qhd_otm9608a_5P_20), yushun_qhd_otm9608a_5P_20},
	{DTYPE_GEN_WRITE2, 1, 0, 0, 0, sizeof(yushun_qhd_otm9608a_offset23), yushun_qhd_otm9608a_offset23},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(yushun_qhd_otm9608a_1P_23), yushun_qhd_otm9608a_1P_23},
	{DTYPE_GEN_WRITE2, 1, 0, 0, 0, sizeof(yushun_qhd_otm9608a_offset24), yushun_qhd_otm9608a_offset24},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(yushun_qhd_otm9608a_1P_24), yushun_qhd_otm9608a_1P_24},
	{DTYPE_GEN_WRITE2, 1, 0, 0, 0, sizeof(yushun_qhd_otm9608a_offset25), yushun_qhd_otm9608a_offset25},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(yushun_qhd_otm9608a_2P_25), yushun_qhd_otm9608a_2P_25},
	{DTYPE_GEN_WRITE2, 1, 0, 0, 0, sizeof(yushun_qhd_otm9608a_offset25_1), yushun_qhd_otm9608a_offset25_1},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(yushun_qhd_otm9608a_2P_25_1), yushun_qhd_otm9608a_2P_25_1},
	{DTYPE_GEN_WRITE2, 1, 0, 0, 0, sizeof(yushun_qhd_otm9608a_offset26), yushun_qhd_otm9608a_offset26},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(yushun_qhd_otm9608a_10P_26), yushun_qhd_otm9608a_10P_26},
	{DTYPE_GEN_WRITE2, 1, 0, 0, 0, sizeof(yushun_qhd_otm9608a_offset26_1), yushun_qhd_otm9608a_offset26_1},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(yushun_qhd_otm9608a_10P_26_1), yushun_qhd_otm9608a_10P_26_1},
	{DTYPE_GEN_WRITE2, 1, 0, 0, 0, sizeof(yushun_qhd_otm9608a_offset26_2), yushun_qhd_otm9608a_offset26_2},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(yushun_qhd_otm9608a_10P_26_2), yushun_qhd_otm9608a_10P_26_2},
	{DTYPE_GEN_WRITE2, 1, 0, 0, 0, sizeof(yushun_qhd_otm9608a_offset27), yushun_qhd_otm9608a_offset27},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(yushun_qhd_otm9608a_15P_27), yushun_qhd_otm9608a_15P_27},
	{DTYPE_GEN_WRITE2, 1, 0, 0, 0, sizeof(yushun_qhd_otm9608a_offset28), yushun_qhd_otm9608a_offset28},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(yushun_qhd_otm9608a_15P_28), yushun_qhd_otm9608a_15P_28},
	{DTYPE_GEN_WRITE2, 1, 0, 0, 0, sizeof(yushun_qhd_otm9608a_offset29), yushun_qhd_otm9608a_offset29},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(yushun_qhd_otm9608a_10P_29), yushun_qhd_otm9608a_10P_29},
	{DTYPE_GEN_WRITE2, 1, 0, 0, 0, sizeof(yushun_qhd_otm9608a_offset30), yushun_qhd_otm9608a_offset30},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(yushun_qhd_otm9608a_15P_30), yushun_qhd_otm9608a_15P_30},
	{DTYPE_GEN_WRITE2, 1, 0, 0, 0, sizeof(yushun_qhd_otm9608a_offset31), yushun_qhd_otm9608a_offset31},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(yushun_qhd_otm9608a_15P_31), yushun_qhd_otm9608a_15P_31},
	{DTYPE_GEN_WRITE2, 1, 0, 0, 0, sizeof(yushun_qhd_otm9608a_offset32), yushun_qhd_otm9608a_offset32},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(yushun_qhd_otm9608a_10P_32), yushun_qhd_otm9608a_10P_32},
	{DTYPE_GEN_WRITE2, 1, 0, 0, 0, sizeof(yushun_qhd_otm9608a_offset33), yushun_qhd_otm9608a_offset33},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(yushun_qhd_otm9608a_10P_33), yushun_qhd_otm9608a_10P_33},
	{DTYPE_GEN_WRITE2, 1, 0, 0, 0, sizeof(yushun_qhd_otm9608a_offset34), yushun_qhd_otm9608a_offset34},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(yushun_qhd_otm9608a_10P_34), yushun_qhd_otm9608a_10P_34},
	{DTYPE_GEN_WRITE2, 1, 0, 0, 0, sizeof(yushun_qhd_otm9608a_offset35), yushun_qhd_otm9608a_offset35},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(yushun_qhd_otm9608a_15P_35), yushun_qhd_otm9608a_15P_35},
	{DTYPE_GEN_WRITE2, 1, 0, 0, 0, sizeof(yushun_qhd_otm9608a_offset36), yushun_qhd_otm9608a_offset36},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(yushun_qhd_otm9608a_15P_36), yushun_qhd_otm9608a_15P_36},
	{DTYPE_GEN_WRITE2, 1, 0, 0, 0, sizeof(yushun_qhd_otm9608a_offset37), yushun_qhd_otm9608a_offset37},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(yushun_qhd_otm9608a_10P_37), yushun_qhd_otm9608a_10P_37},
	{DTYPE_GEN_WRITE2, 1, 0, 0, 0, sizeof(yushun_qhd_otm9608a_offset38), yushun_qhd_otm9608a_offset38},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(yushun_qhd_otm9608a_15P_38), yushun_qhd_otm9608a_15P_38},
	{DTYPE_GEN_WRITE2, 1, 0, 0, 0, sizeof(yushun_qhd_otm9608a_offset39), yushun_qhd_otm9608a_offset39},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(yushun_qhd_otm9608a_15P_39), yushun_qhd_otm9608a_15P_39},
	{DTYPE_GEN_WRITE2, 1, 0, 0, 0, sizeof(yushun_qhd_otm9608a_offset40), yushun_qhd_otm9608a_offset40},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(yushun_qhd_otm9608a_12P_40), yushun_qhd_otm9608a_12P_40},
	{DTYPE_GEN_WRITE2, 1, 0, 0, 0, sizeof(yushun_qhd_otm9608a_offset41), yushun_qhd_otm9608a_offset41},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(yushun_qhd_otm9608a_14P_41), yushun_qhd_otm9608a_14P_41},
	{DTYPE_GEN_WRITE2, 1, 0, 0, 0, sizeof(yushun_qhd_otm9608a_offset42), yushun_qhd_otm9608a_offset42},	
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(yushun_qhd_otm9608a_14P_42), yushun_qhd_otm9608a_14P_42},
	{DTYPE_GEN_WRITE2, 1, 0, 0, 0, sizeof(yushun_qhd_otm9608a_offset43), yushun_qhd_otm9608a_offset43},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(yushun_qhd_otm9608a_14P_43), yushun_qhd_otm9608a_14P_43},
	{DTYPE_GEN_WRITE2, 1, 0, 0, 0, sizeof(yushun_qhd_otm9608a_offset44), yushun_qhd_otm9608a_offset44},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(yushun_qhd_otm9608a_14P_44), yushun_qhd_otm9608a_14P_44},
	{DTYPE_GEN_WRITE2, 1, 0, 0, 0, sizeof(yushun_qhd_otm9608a_offset45), yushun_qhd_otm9608a_offset45},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(yushun_qhd_otm9608a_14P_45), yushun_qhd_otm9608a_14P_45},
	{DTYPE_GEN_WRITE2, 1, 0, 0, 0, sizeof(yushun_qhd_otm9608a_offset46), yushun_qhd_otm9608a_offset46},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(yushun_qhd_otm9608a_14P_46), yushun_qhd_otm9608a_14P_46},
	{DTYPE_GEN_WRITE2, 1, 0, 0, 0, sizeof(yushun_qhd_otm9608a_offset47), yushun_qhd_otm9608a_offset47},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(yushun_qhd_otm9608a_14P_47), yushun_qhd_otm9608a_14P_47},
	{DTYPE_GEN_WRITE2, 1, 0, 0, 0, sizeof(yushun_qhd_otm9608a_offset48), yushun_qhd_otm9608a_offset48},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(yushun_qhd_otm9608a_14P_48), yushun_qhd_otm9608a_14P_48},
	{DTYPE_GEN_WRITE2, 1, 0, 0, 0, sizeof(yushun_qhd_otm9608a_offset49), yushun_qhd_otm9608a_offset49},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(yushun_qhd_otm9608a_14P_49), yushun_qhd_otm9608a_14P_49},
	{DTYPE_GEN_WRITE2, 1, 0, 0, 0, sizeof(yushun_qhd_otm9608a_offset50), yushun_qhd_otm9608a_offset50},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(yushun_qhd_otm9608a_10P_50), yushun_qhd_otm9608a_10P_50},
	{DTYPE_GEN_WRITE2, 1, 0, 0, 0, sizeof(yushun_qhd_otm9608a_offset52), yushun_qhd_otm9608a_offset52},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(yushun_qhd_otm9608a_1P_52), yushun_qhd_otm9608a_1P_52},
	{DTYPE_GEN_WRITE2, 1, 0, 0, 0, sizeof(yushun_qhd_otm9608a_offset57), yushun_qhd_otm9608a_offset57},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(yushun_qhd_otm9608a_3P_57), yushun_qhd_otm9608a_3P_57},
	{DTYPE_GEN_WRITE2, 1, 0, 0, 0, sizeof(yushun_qhd_otm9608a_offset58), yushun_qhd_otm9608a_offset58},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(yushun_qhd_otm9608a_1P_58), yushun_qhd_otm9608a_1P_58},	
	{DTYPE_DCS_WRITE,  1, 0, 0, 150, sizeof(exit_sleep), exit_sleep},
	{DTYPE_DCS_WRITE, 1, 0, 0, 20, sizeof(display_on), display_on}, 
	{DTYPE_GEN_WRITE2, 1, 0, 0, 0, sizeof(yushun_qhd_otm9608a_offset59), yushun_qhd_otm9608a_offset59},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(yushun_qhd_otm9608a_1P_59), yushun_qhd_otm9608a_1P_59},
	
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(yushun_qhd_otm9608a_TE), yushun_qhd_otm9608a_TE},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(yushun_qhd_otm9608a_44), yushun_qhd_otm9608a_44},
};


//3.tianma nt35516
static char tianma_nt35516_para03[]={0xB6,0x0B};
static char tianma_nt35516_para06[]={0xBA,0x05};
static char tianma_nt35516_para09[]={0xC7,0x70};
static char tianma_nt35516_para12[]={0x3A,0x77};	 
static char tianma_nt35516_para13[]={0x36,0x00};
static char tianma_nt35516_para15[]={0xB0,0x0A};
static char tianma_nt35516_para16[]={0xB6,0x54};
static char tianma_nt35516_para17[]={0xB1,0x0A};
static char tianma_nt35516_para18[]={0xB7,0x34};
static char tianma_nt35516_para19[]={0xB2,0x00};
static char tianma_nt35516_para20[]={0xB8,0x30};
static char tianma_nt35516_para21[]={0xB3,0x0D};
static char tianma_nt35516_para22[]={0xB9,0x34};
static char tianma_nt35516_para23[]={0xB4,0x08};
static char tianma_nt35516_para24[]={0xBA,0x24};
static char tianma_nt35516_para27[]={0xBE,0x3E};
static char tianma_nt35516_para04[]={0xB7,0x72,0x72};
static char tianma_nt35516_para02[]={0xB1,0xFC,0x00,0x00};
static char tianma_nt35516_para07[]={0xBC,0x00,0x00,0x00};
static char tianma_nt35516_para08[]={0xBB,0x53,0x03,0x53};
static char tianma_nt35516_para25[]={0xBC,0x00,0x78,0x00};
static char tianma_nt35516_para26[]={0xBD,0x00,0x78,0x00};
static char tianma_nt35516_para05[]={0xB8,0x01,0x04,0x04,0x04};   
static char tianma_nt35516_para28[]={0xD0,0x0A,0x10,0x0D,0x0F}; 
static char tianma_nt35516_para32[]={0xD4,0x03,0xF7,0x03,0xF8};	
static char tianma_nt35516_para36[]={0xD8,0x03,0xF7,0x03,0xF8};
static char tianma_nt35516_para40[]={0xDF,0x03,0xF7,0x03,0xF8};
static char tianma_nt35516_para44[]={0xE3,0x03,0xF7,0x03,0xF8};
static char tianma_nt35516_para48[]={0xE7,0x03,0xF7,0x03,0xF8};  
static char tianma_nt35516_para52[]={0xEB,0x03,0xF7,0x03,0xF8};  
static char tianma_nt35516_para01[]={0xF0,0x55,0xAA,0x52,0x08,0x00};
static char tianma_nt35516_para14[]={0xF0,0x55,0xAA,0x52,0x08,0x01};
static char tianma_nt35516_para10[]={0xC9,0x41,0x06,0x0d,0x3A,0x17,0x00};
static char tianma_nt35516_para11[]={0xCA,0x00,0xE4,0xE4,0xE4,0xE4,0xE4,0xE4,0x08,0x08,0x00,0x01};
static char tianma_nt35516_para29[]={0xD1,0x00,0x00,0x00,0x74,0x00,0xC9,0x01,0x05,0x01,0x16,0x01,0x3E,0x01,0x66,0x01,0x95};
static char tianma_nt35516_para30[]={0xD2,0x01,0xB8,0x01,0xE5,0x02,0x1B,0x02,0x5D,0x02,0x93,0x02,0x95,0x02,0xC4,0x02,0xF8};
static char tianma_nt35516_para31[]={0xD3,0x03,0x18,0x03,0x44,0x03,0x60,0x03,0x82,0x03,0x98,0x03,0xB5,0x03,0xDB,0x03,0xF0};
static char tianma_nt35516_para33[]={0xD5,0x00,0x00,0x00,0x74,0x00,0xC9,0x01,0x05,0x01,0x16,0x01,0x3E,0x01,0x66,0x01,0x95};
static char tianma_nt35516_para34[]={0xD6,0x01,0xB8,0x01,0xE5,0x02,0x1B,0x02,0x5D,0x02,0x93,0x02,0x95,0x02,0xC4,0x02,0xF8};
static char tianma_nt35516_para35[]={0xD7,0x03,0x18,0x03,0x44,0x03,0x60,0x03,0x82,0x03,0x98,0x03,0xB5,0x03,0xDB,0x03,0xF0};
static char tianma_nt35516_para37[]={0xD9,0x00,0x00,0x00,0x74,0x00,0xC9,0x01,0x05,0x01,0x16,0x01,0x3E,0x01,0x66,0x01,0x95};
static char tianma_nt35516_para38[]={0xDD,0x01,0xB8,0x01,0xE5,0x02,0x1B,0x02,0x5D,0x02,0x93,0x02,0x95,0x02,0xC4,0x02,0xF8};
static char tianma_nt35516_para39[]={0xDE,0x03,0x18,0x03,0x44,0x03,0x60,0x03,0x82,0x03,0x98,0x03,0xB5,0x03,0xDB,0x03,0xF0};
static char tianma_nt35516_para41[]={0xE0,0x00,0x00,0x00,0x74,0x00,0xC9,0x01,0x05,0x01,0x16,0x01,0x3E,0x01,0x66,0x01,0x95};
static char tianma_nt35516_para42[]={0xE1,0x01,0xB8,0x01,0xE5,0x02,0x1B,0x02,0x5D,0x02,0x93,0x02,0x95,0x02,0xC4,0x02,0xF8};
static char tianma_nt35516_para43[]={0xE2,0x03,0x18,0x03,0x44,0x03,0x60,0x03,0x82,0x03,0x98,0x03,0xB5,0x03,0xDB,0x03,0xF0};
static char tianma_nt35516_para45[]={0xE4,0x00,0x00,0x00,0x74,0x00,0xC9,0x01,0x05,0x01,0x16,0x01,0x3E,0x01,0x66,0x01,0x95};
static char tianma_nt35516_para46[]={0xE5,0x01,0xB8,0x01,0xE5,0x02,0x1B,0x02,0x5D,0x02,0x93,0x02,0x95,0x02,0xC4,0x02,0xF8};
static char tianma_nt35516_para47[]={0xE6,0x03,0x18,0x03,0x44,0x03,0x60,0x03,0x82,0x03,0x98,0x03,0xB5,0x03,0xDB,0x03,0xF0};
static char tianma_nt35516_para49[]={0xE8,0x00,0x00,0x00,0x74,0x00,0xC9,0x01,0x05,0x01,0x16,0x01,0x3E,0x01,0x66,0x01,0x95};
static char tianma_nt35516_para50[]={0xE9,0x01,0xB8,0x01,0xE5,0x02,0x1B,0x02,0x5D,0x02,0x93,0x02,0x95,0x02,0xC4,0x02,0xF8};
static char tianma_nt35516_para51[]={0xEA,0x03,0x18,0x03,0x44,0x03,0x60,0x03,0x82,0x03,0x98,0x03,0xB5,0x03,0xDB,0x03,0xF0};

static struct dsi_cmd_desc tianma_nt35516_video_on_cmds[] = 
{
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(tianma_nt35516_para01), tianma_nt35516_para01}, 
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(tianma_nt35516_para02), tianma_nt35516_para02},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tianma_nt35516_para03), tianma_nt35516_para03},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(tianma_nt35516_para04), tianma_nt35516_para04},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(tianma_nt35516_para05), tianma_nt35516_para05},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tianma_nt35516_para06), tianma_nt35516_para06},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(tianma_nt35516_para07), tianma_nt35516_para07},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(tianma_nt35516_para08), tianma_nt35516_para08},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tianma_nt35516_para09), tianma_nt35516_para09},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(tianma_nt35516_para10), tianma_nt35516_para10},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(tianma_nt35516_para11), tianma_nt35516_para11},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tianma_nt35516_para12), tianma_nt35516_para12},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tianma_nt35516_para13), tianma_nt35516_para13},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(tianma_nt35516_para14), tianma_nt35516_para14},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tianma_nt35516_para15), tianma_nt35516_para15},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tianma_nt35516_para16), tianma_nt35516_para16},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tianma_nt35516_para17), tianma_nt35516_para17},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tianma_nt35516_para18), tianma_nt35516_para18},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tianma_nt35516_para19), tianma_nt35516_para19},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tianma_nt35516_para20), tianma_nt35516_para20},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tianma_nt35516_para21), tianma_nt35516_para21},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tianma_nt35516_para22), tianma_nt35516_para22},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tianma_nt35516_para23), tianma_nt35516_para23},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tianma_nt35516_para24), tianma_nt35516_para24},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(tianma_nt35516_para25), tianma_nt35516_para25},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(tianma_nt35516_para26), tianma_nt35516_para26},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tianma_nt35516_para27), tianma_nt35516_para27},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(tianma_nt35516_para28), tianma_nt35516_para28},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(tianma_nt35516_para29), tianma_nt35516_para29},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(tianma_nt35516_para30), tianma_nt35516_para30},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(tianma_nt35516_para31), tianma_nt35516_para31},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(tianma_nt35516_para32), tianma_nt35516_para32},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(tianma_nt35516_para33), tianma_nt35516_para33},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(tianma_nt35516_para34), tianma_nt35516_para34},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(tianma_nt35516_para35), tianma_nt35516_para35},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(tianma_nt35516_para36), tianma_nt35516_para36},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(tianma_nt35516_para37), tianma_nt35516_para37},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(tianma_nt35516_para38), tianma_nt35516_para38},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(tianma_nt35516_para39), tianma_nt35516_para39},	
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(tianma_nt35516_para40), tianma_nt35516_para40},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(tianma_nt35516_para41), tianma_nt35516_para41},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(tianma_nt35516_para42), tianma_nt35516_para42},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(tianma_nt35516_para43), tianma_nt35516_para43},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(tianma_nt35516_para44), tianma_nt35516_para44},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(tianma_nt35516_para45), tianma_nt35516_para45},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(tianma_nt35516_para46), tianma_nt35516_para46},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(tianma_nt35516_para47), tianma_nt35516_para47},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(tianma_nt35516_para48), tianma_nt35516_para48},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(tianma_nt35516_para49), tianma_nt35516_para49},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(tianma_nt35516_para50), tianma_nt35516_para50},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(tianma_nt35516_para51), tianma_nt35516_para51},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(tianma_nt35516_para52), tianma_nt35516_para52},
	
	{DTYPE_DCS_WRITE,  1, 0, 0, 150, sizeof(exit_sleep), exit_sleep},
	{DTYPE_DCS_WRITE, 1, 0, 0, 10, sizeof(display_on), display_on}, 
};	
#if 0
static char OTM9608_CMI_001[]={0x00,0x00};
static char OTM9608_CMI_002[]={0xff,0x96,0x08,0x01}; 
static char OTM9608_CMI_003[]={0x00,0x80};
static char OTM9608_CMI_004[]={0xff,0x96,0x08}; 
static char OTM9608_CMI_005[]={0x00,0x00};
static char OTM9608_CMI_006[]={0xa0,0x00}; 
static char OTM9608_CMI_007[]={0x00,0x80};
static char OTM9608_CMI_008[]={0xb3,0x00,0x00,0x20,0x00,0x00}; 
static char OTM9608_CMI_009[]={0x00,0xc0};
static char OTM9608_CMI_010[]={0xb3,0x09}; 
static char OTM9608_CMI_011[]={0x00,0x80};
static char OTM9608_CMI_012[]={0xc0,0x00,0x48,0x00,0x10,0x10,0x00,0x47,0x10,0x10}; 
static char OTM9608_CMI_013[]={0x00,0x92};
static char OTM9608_CMI_014[]={0xc0,0x00,0x10,0x00,0x13}; 
static char OTM9608_CMI_015[]={0x00,0xa2};
static char OTM9608_CMI_016[]={0xc0,0x0c,0x05,0x02}; 
static char OTM9608_CMI_017[]={0x00,0xb3};
static char OTM9608_CMI_018[]={0xc0,0x00,0x10}; 
static char OTM9608_CMI_019[]={0x00,0x81};
static char OTM9608_CMI_020[]={0xc1,0x55}; 
static char OTM9608_CMI_021[]={0x00,0x80};
static char OTM9608_CMI_022[]={0xc4,0x00,0x84,0xfc}; 
static char OTM9608_CMI_023[]={0x00,0xa0};
static char OTM9608_CMI_024[]={0xb3,0x10,0x00}; 
static char OTM9608_CMI_025[]={0x00,0xa0};
static char OTM9608_CMI_026[]={0xc0,0x00}; 
static char OTM9608_CMI_027[]={0x00,0xa0};
static char OTM9608_CMI_028[]={0xc4,0x33,0x09,0x90,0x2b,0x33,0x09,0x90,0x54}; 
static char OTM9608_CMI_029[]={0x00,0x80};
static char OTM9608_CMI_030[]={0xc5,0x08,0x00,0xa0,0x11}; 
static char OTM9608_CMI_031[]={0x00,0x90};
static char OTM9608_CMI_032[]={0xc5,0x96,0x57,0x00,0x57,0x33,0x33,0x34}; 
static char OTM9608_CMI_033[]={0x00,0xa0};
static char OTM9608_CMI_034[]={0xc5,0x96,0x57,0x00,0x57,0x33,0x33,0x34}; 
static char OTM9608_CMI_035[]={0x00,0xb0};
static char OTM9608_CMI_036[]={0xc5,0x04,0xac,0x01,0x00,0x71,0xb1,0x83}; 
static char OTM9608_CMI_037[]={0x00,0x00};
static char OTM9608_CMI_038[]={0xd9,0x6f}; 
static char OTM9608_CMI_039[]={0x00,0x80};
static char OTM9608_CMI_040[]={0xc6,0x64}; 
static char OTM9608_CMI_041[]={0x00,0xb0}; 
static char OTM9608_CMI_042[]={0xc6,0x03,0x10,0x00,0x1f,0x12};
static char OTM9608_CMI_043[]={0x00,0xe1};
static char OTM9608_CMI_044[]={0xc0,0x9f}; 
static char OTM9608_CMI_045[]={0x00,0xb7};
static char OTM9608_CMI_046[]={0xb0,0x10}; 
static char OTM9608_CMI_047[]={0x00,0xc0};
static char OTM9608_CMI_048[]={0xb0,0x55}; 
static char OTM9608_CMI_049[]={0x00,0xb1};
static char OTM9608_CMI_050[]={0xb0,0x03}; 
static char OTM9608_CMI_051[]={0x00,0x81};
static char OTM9608_CMI_052[]={0xd6,0x00};
static char OTM9608_CMI_053[]={0x00,0x00};
static char OTM9608_CMI_054[]={0xe1,0x01,0x0F,0x15,0x0F,0x07,0x2F,0x0B,0x0A,0x03,0x06,0x0F,0x08,0x0D,0x0E,0x09,0x01};
static char OTM9608_CMI_055[]={0x00,0x00};
static char OTM9608_CMI_056[]={0xe2,0x01,0x0F,0x15,0x0F,0x07,0x2F,0x0B,0x0A,0x03,0x06,0x0F,0x08,0x0D,0x0E,0x09,0x01};
static char OTM9608_CMI_057[]={0x00,0x80};
static char OTM9608_CMI_058[]={0xcb,0x05,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}; 
static char OTM9608_CMI_059[]={0x00,0x90};
static char OTM9608_CMI_060[]={0xcb,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}; 
static char OTM9608_CMI_061[]={0x00,0xa0};
static char OTM9608_CMI_062[]={0xcb,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
static char OTM9608_CMI_063[]={0x00,0xb0};
static char OTM9608_CMI_064[]={0xcb,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
static char OTM9608_CMI_065[]={0x00,0xc0};
static char OTM9608_CMI_066[]={0xcb,0x00,0x00,0x00,0x04,0x00,0x00,0x04,0x04,0x00,0x00,0x04,0x04,0x04,0x00,0x00};
static char OTM9608_CMI_067[]={0x00,0xd0};
static char OTM9608_CMI_068[]={0xcb,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x04,0x04,0x00,0x00,0x04,0x04};
static char OTM9608_CMI_069[]={0x00,0xe0};
static char OTM9608_CMI_070[]={0xcb,0x00,0x00,0x00,0x00,0x04,0x00,0x00,0x00,0x00,0x00}; 
static char OTM9608_CMI_071[]={0x00,0xf0};
static char OTM9608_CMI_072[]={0xcb,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff}; 
static char OTM9608_CMI_073[]={0x00,0x80};
static char OTM9608_CMI_074[]={0xcc,0x00,0x00,0x00,0x02,0x00,0x00,0x0a,0x0e,0x00,0x00}; 
static char OTM9608_CMI_075[]={0x00,0x90}; 
static char OTM9608_CMI_076[]={0xcc,0x0c,0x10,0x06,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x09}; 
static char OTM9608_CMI_077[]={0x00,0xa0};
static char OTM9608_CMI_078[]={0xcc,0x0d,0x00,0x00,0x0b,0x0f,0x00,0x00,0x00,0x00,0x05,0x00,0x00,0x00,0x00,0x00}; 
static char OTM9608_CMI_079[]={0x00,0xb0};
static char OTM9608_CMI_080[]={0xcc,0x00,0x00,0x00,0x02,0x00,0x00,0x0a,0x0e,0x00,0x00}; 
static char OTM9608_CMI_081[]={0x00,0xc0};
static char OTM9608_CMI_082[]={0xcc,0x0c,0x10,0x06,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}; 
static char OTM9608_CMI_083[]={0x00,0xd0};
static char OTM9608_CMI_084[]={0xcc,0x05,0x00,0x00,0x00,0x00,0x0f,0x0b,0x00,0x00,0x0d,0x09,0x01,0x00,0x00,0x00}; 
static char OTM9608_CMI_085[]={0x00,0x80};
static char OTM9608_CMI_086[]={0xce,0x84,0x03,0x18,0x83,0x03,0x18,0x00,0x00,0x00,0x00,0x00,0x00}; 
static char OTM9608_CMI_087[]={0x00,0x90};
static char OTM9608_CMI_088[]={0xce,0x33,0xbf,0x18,0x33,0xc0,0x18,0x10,0x0f,0x18,0x10,0x10,0x18,0x00,0x00}; 
static char OTM9608_CMI_089[]={0x00,0xa0};
static char OTM9608_CMI_090[]={0xce,0x38,0x02,0x03,0xc1,0x00,0x18,0x00,0x38,0x01,0x03,0xc2,0x00,0x18,0x00}; 
static char OTM9608_CMI_091[]={0x00,0xb0};
static char OTM9608_CMI_092[]={0xce,0x38,0x00,0x03,0xc3,0x00,0x18,0x00,0x30,0x00,0x03,0xc4,0x00,0x18,0x00}; 
static char OTM9608_CMI_093[]={0x00,0xc0};
static char OTM9608_CMI_094[]={0xce,0x30,0x01,0x03,0xc5,0x00,0x18,0x00,0x30,0x02,0x03,0xc6,0x00,0x18,0x00}; 
static char OTM9608_CMI_095[]={0x00,0xd0};
static char OTM9608_CMI_096[]={0xce,0x30,0x03,0x03,0xc7,0x00,0x18,0x00,0x30,0x04,0x03,0xc8,0x00,0x18,0x00}; 
static char OTM9608_CMI_097[]={0x00,0x80};
static char OTM9608_CMI_098[]={0xcf,0xf0,0x00,0x00,0x00,0x00,0x00,0x00,0xf0,0x00,0x00,0x00,0x00,0x00,0x00}; 
static char OTM9608_CMI_099[]={0x00,0x90};
static char OTM9608_CMI_100[]={0xcf,0xf0,0x00,0x00,0x00,0x00,0x00,0x00,0xf0,0x00,0x00,0x00,0x00,0x00,0x00}; 
static char OTM9608_CMI_101[]={0x00,0xa0};
static char OTM9608_CMI_102[]={0xcf,0xf0,0x00,0x00,0x00,0x00,0x00,0x00,0xf0,0x00,0x00,0x00,0x00,0x00,0x00}; 
static char OTM9608_CMI_103[]={0x00,0xb0};
static char OTM9608_CMI_104[]={0xcf,0xf0,0x00,0x00,0x00,0x00,0x00,0x00,0xf0,0x00,0x00,0x00,0x00,0x00,0x00}; 
static char OTM9608_CMI_105[]={0x00,0xc0};
static char OTM9608_CMI_106[]={0xcf,0x01,0x01,0x20,0x20,0x00,0x00,0x02,0x04,0x00,0x00}; 
static char OTM9608_CMI_107[]={0x00,0x00};
static char OTM9608_CMI_108[]={0xd8,0xa7,0xa7};
static char OTM9608_CMI_109[]={0x00,0x00};
static char OTM9608_CMI_110[]={0xff,0xff,0xff,0xff}; 
static char OTM9608_CMI_111[]={0x00,0x81};
static char OTM9608_CMI_112[]={0xc1,0x66};
static char OTM9608_CMI_113[]={0x35,0x00};
#else
static char OTM9608_CMI_001[]={0x00,0x00};
static char OTM9608_CMI_002[]={0xFF,0x96,0x08,0x01};
static char OTM9608_CMI_003[]={0x00,0x80};
static char OTM9608_CMI_004[]={0xFF,0x96,0x08};
static char OTM9608_CMI_005[]={0x00,0x00};
static char OTM9608_CMI_006[]={0xA0,0x00};
static char OTM9608_CMI_007[]={0x00,0x80};
static char OTM9608_CMI_008[]={0xB3,0x00,0x00,0x00,0x21,0x00};
static char OTM9608_CMI_009[]={0x00,0x92};
static char OTM9608_CMI_010[]={0xB3,0x01};
static char OTM9608_CMI_011[]={0x00,0xC0};
static char OTM9608_CMI_012[]={0xB3,0x19};
static char OTM9608_CMI_013[]={0x00,0x80};
static char OTM9608_CMI_014[]={0xC0,0x00,0x48,0x00,0x10,0x10,0x00,0x47,0x1F,0x1F};
static char OTM9608_CMI_015[]={0x00,0x92};
static char OTM9608_CMI_016[]={0xC0,0x00,0x0E,0x00,0x11};
static char OTM9608_CMI_017[]={0x00,0xA2};
static char OTM9608_CMI_018[]={0xC0,0x01,0x10,0x00};
static char OTM9608_CMI_019[]={0x00,0xB3};
static char OTM9608_CMI_020[]={0xC0,0x00,0x50};
static char OTM9608_CMI_021[]={0x00,0x81};
static char OTM9608_CMI_022[]={0xC1,0x66};//0x55
static char OTM9608_CMI_023[]={0x00,0x80};
static char OTM9608_CMI_024[]={0xC4,0x30,0x84,0xFA};
static char OTM9608_CMI_025[]={0x00,0xA0};
static char OTM9608_CMI_026[]={0xC4,0x33,0x09,0x90,0x2B,0x33,0x09,0x90,0x54};
static char OTM9608_CMI_027[]={0x00,0x80};
static char OTM9608_CMI_028[]={0xC5,0x08,0x00,0x90,0x11};
static char OTM9608_CMI_029[]={0x00,0x90};
static char OTM9608_CMI_030[]={0xC5,0x84,0x76,0x00,0x76,0x33,0x33,0x34};
static char OTM9608_CMI_031[]={0x00,0xA0};
static char OTM9608_CMI_032[]={0xC5,0x96,0x76,0x06,0x76,0x33,0x33,0x34};
static char OTM9608_CMI_033[]={0x00,0xB0};
static char OTM9608_CMI_034[]={0xC5,0x04,0xF8};
static char OTM9608_CMI_035[]={0x00,0x80};
static char OTM9608_CMI_036[]={0xC6,0x64};
static char OTM9608_CMI_037[]={0x00,0xB0};
static char OTM9608_CMI_038[]={0xC6,0x03,0x10,0x00,0x1F,0x12};
static char OTM9608_CMI_039[]={0x00,0xE1};
static char OTM9608_CMI_040[]={0xC0,0x9F};
static char OTM9608_CMI_041[]={0x00,0x00};
static char OTM9608_CMI_042[]={0xD0,0x01};
static char OTM9608_CMI_043[]={0x00,0x00};
static char OTM9608_CMI_044[]={0xD1,0x01,0x01};
static char OTM9608_CMI_045[]={0x00,0xB7};
static char OTM9608_CMI_046[]={0xB0,0x10};
static char OTM9608_CMI_047[]={0x00,0xC0};
static char OTM9608_CMI_048[]={0xB0,0x55};
static char OTM9608_CMI_049[]={0x00,0xB1};
static char OTM9608_CMI_050[]={0xB0,0x03,0x06};
static char OTM9608_CMI_051[]={0x00,0x80};
static char OTM9608_CMI_052[]={0xCB,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
static char OTM9608_CMI_053[]={0x00,0x90};
static char OTM9608_CMI_054[]={0xCB,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
static char OTM9608_CMI_055[]={0x00,0xA0};
static char OTM9608_CMI_056[]={0xCB,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
static char OTM9608_CMI_057[]={0x00,0xB0};
static char OTM9608_CMI_058[]={0xCB,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
static char OTM9608_CMI_059[]={0x00,0xC0};
static char OTM9608_CMI_060[]={0xCB,0x00,0x00,0x00,0x04,0x00,0x00,0x04,0x04,0x00,0x00,0x04,0x04,0x04,0x00,0x00};
static char OTM9608_CMI_061[]={0x00,0xD0};
static char OTM9608_CMI_062[]={0xCB,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x04,0x04,0x00,0x00,0x04,0x04};
static char OTM9608_CMI_063[]={0x00,0xE0};
static char OTM9608_CMI_064[]={0xCB,0x00,0x00,0x00,0x00,0x04,0x00,0x00,0x00,0x00,0x00};
static char OTM9608_CMI_065[]={0x00,0xF0};
static char OTM9608_CMI_066[]={0xCB,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,};
static char OTM9608_CMI_067[]={0x00,0x80};
static char OTM9608_CMI_068[]={0xCC,0x00,0x00,0x00,0x02,0x00,0x00,0x0A,0x0E,0x00,0x00};
static char OTM9608_CMI_069[]={0x00,0x90};
static char OTM9608_CMI_070[]={0xCC,0x0C,0x10,0x06,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x09};
static char OTM9608_CMI_071[]={0x00,0xA0};
static char OTM9608_CMI_072[]={0xCC,0x0D,0x00,0x00,0x0B,0x0F,0x00,0x00,0x00,0x00,0x05,0x00,0x00,0x00,0x00,0x00};
static char OTM9608_CMI_073[]={0x00,0xB0};
static char OTM9608_CMI_074[]={0xCC,0x00,0x00,0x00,0x02,0x00,0x00,0x0A,0x0E,0x00,0x00};
static char OTM9608_CMI_075[]={0x00,0xC0};
static char OTM9608_CMI_076[]={0xCC,0x0C,0x10,0x06,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x09};
static char OTM9608_CMI_077[]={0x00,0xD0};
static char OTM9608_CMI_078[]={0xCC,0x0D,0x00,0x00,0x0B,0x0F,0x00,0x00,0x00,0x00,0x05,0x00,0x00,0x00,0x00,0x00};
static char OTM9608_CMI_079[]={0x00,0x80};
static char OTM9608_CMI_080[]={0xCE,0x84,0x03,0x18,0x83,0x03,0x18,0x00,0x0F,0x00,0x00,0x0F,0x00};
static char OTM9608_CMI_081[]={0x00,0x90};
static char OTM9608_CMI_082[]={0xCE,0x33,0xBF,0x18,0x33,0xC0,0x18,0xF0,0x00,0x00,0xF0,0x00,0x00,0x00,0x00};
static char OTM9608_CMI_083[]={0x00,0xA0};
static char OTM9608_CMI_084[]={0xCE,0x38,0x02,0x03,0xC1,0x00,0x18,0x00,0x38,0x01,0x03,0xC2,0x00,0x18,0x00};
static char OTM9608_CMI_085[]={0x00,0xB0};
static char OTM9608_CMI_086[]={0xCE,0x38,0x00,0x03,0xC3,0x00,0x18,0x00,0x30,0x00,0x03,0xC4,0x00,0x18,0x00};
static char OTM9608_CMI_087[]={0x00,0xC0};
static char OTM9608_CMI_088[]={0xCE,0x30,0x01,0x03,0xC5,0x00,0x18,0x00,0x30,0x02,0x03,0xC6,0x00,0x18,0x00};
static char OTM9608_CMI_089[]={0x00,0xD0};
static char OTM9608_CMI_090[]={0xCE,0x30,0x03,0x03,0xC7,0x00,0x18,0x00,0x30,0x04,0x03,0xC8,0x00,0x18,0x00};
static char OTM9608_CMI_091[]={0x00,0x80};
static char OTM9608_CMI_092[]={0xCF,0xF0,0x00,0x00,0x10,0x00,0x00,0x00,0xF0,0x00,0x00,0x10,0x00,0x00,0x00};
static char OTM9608_CMI_093[]={0x00,0x90};
static char OTM9608_CMI_094[]={0xCF,0xF0,0x00,0x00,0x10,0x00,0x00,0x00,0xF0,0x00,0x00,0x10,0x00,0x00,0x00};
static char OTM9608_CMI_095[]={0x00,0xA0};
static char OTM9608_CMI_096[]={0xCF,0xF0,0x00,0x00,0x10,0x00,0x00,0x00,0xF0,0x00,0x00,0x10,0x00,0x00,0x00};
static char OTM9608_CMI_097[]={0x00,0xB0};
static char OTM9608_CMI_098[]={0xCF,0xF0,0x00,0x00,0x10,0x00,0x00,0x00,0xF0,0x00,0x00,0x10,0x00,0x00,0x00};
static char OTM9608_CMI_099[]={0x00,0xC0};
static char OTM9608_CMI_100[]={0xCF,0x01,0x01,0x20,0x20,0x00,0x00,0x02,0x00,0x00,0x00};
static char OTM9608_CMI_101[]={0x00,0x80};
static char OTM9608_CMI_102[]={0xD6,0x00};
static char OTM9608_CMI_103[]={0x00,0x00};
static char OTM9608_CMI_104[]={0xD7,0x00};
static char OTM9608_CMI_105[]={0x00,0x00};
static char OTM9608_CMI_106[]={0xD8,0x6F,0x6F};
static char OTM9608_CMI_107[]={0x00,0x00};
static char OTM9608_CMI_108[]={0xD9,0x64};//0x21
static char OTM9608_CMI_109[]={0x00,0x00};
static char OTM9608_CMI_110[]={0xE1,0x09,0x11,0x17,0x0D,0x06,0x0E,0x0A,0x08,0x05,0x09,0x0D,0x07,0x0E,0x0E,0x0A,0x08};
static char OTM9608_CMI_111[]={0x00,0x00};
static char OTM9608_CMI_112[]={0xE2,0x09,0x11,0x17,0x0D,0x06,0x0E,0x0A,0x08,0x05,0x09,0x0D,0x07,0x0E,0x0E,0x0A,0x08};

static char OTM9608_CMI_113[]={0x00,0x88};
static char OTM9608_CMI_114[]={0xC4,0x40};

static char OTM9608_CMI_115[]={0x00,0x00};
static char OTM9608_CMI_116[]={0xFF,0xFF,0xFF,0xFF};

static char OTM9608_CMI_TE[]={0x35,0x00};//open TE
static char OTM9608_CMI_44[]={0x44,0x00,0xd0};//TE relatived

#endif
static struct dsi_cmd_desc cmi_OTM9608A_video_on_cmds[] = {
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM9608_CMI_001), OTM9608_CMI_001},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM9608_CMI_002), OTM9608_CMI_002},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM9608_CMI_003), OTM9608_CMI_003},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM9608_CMI_004), OTM9608_CMI_004},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM9608_CMI_005), OTM9608_CMI_005},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM9608_CMI_006), OTM9608_CMI_006},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM9608_CMI_007), OTM9608_CMI_007},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM9608_CMI_008), OTM9608_CMI_008},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM9608_CMI_009), OTM9608_CMI_009},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM9608_CMI_010), OTM9608_CMI_010},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM9608_CMI_011), OTM9608_CMI_011},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM9608_CMI_012), OTM9608_CMI_012},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM9608_CMI_013), OTM9608_CMI_013},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM9608_CMI_014), OTM9608_CMI_014},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM9608_CMI_015), OTM9608_CMI_015},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM9608_CMI_016), OTM9608_CMI_016},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM9608_CMI_017), OTM9608_CMI_017},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM9608_CMI_018), OTM9608_CMI_018},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM9608_CMI_019), OTM9608_CMI_019},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM9608_CMI_020), OTM9608_CMI_020},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM9608_CMI_021), OTM9608_CMI_021},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM9608_CMI_022), OTM9608_CMI_022},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM9608_CMI_023), OTM9608_CMI_023},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM9608_CMI_024), OTM9608_CMI_024},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM9608_CMI_025), OTM9608_CMI_025},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM9608_CMI_026), OTM9608_CMI_026},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM9608_CMI_027), OTM9608_CMI_027},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM9608_CMI_028), OTM9608_CMI_028},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM9608_CMI_029), OTM9608_CMI_029},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM9608_CMI_030), OTM9608_CMI_030},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM9608_CMI_031), OTM9608_CMI_031},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM9608_CMI_032), OTM9608_CMI_032},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM9608_CMI_033), OTM9608_CMI_033},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM9608_CMI_034), OTM9608_CMI_034},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM9608_CMI_035), OTM9608_CMI_035},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM9608_CMI_036), OTM9608_CMI_036},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM9608_CMI_037), OTM9608_CMI_037},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM9608_CMI_038), OTM9608_CMI_038},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM9608_CMI_039), OTM9608_CMI_039},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM9608_CMI_040), OTM9608_CMI_040},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM9608_CMI_041), OTM9608_CMI_041},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM9608_CMI_042), OTM9608_CMI_042},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM9608_CMI_043), OTM9608_CMI_043},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM9608_CMI_044), OTM9608_CMI_044},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM9608_CMI_045), OTM9608_CMI_045},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM9608_CMI_046), OTM9608_CMI_046},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM9608_CMI_047), OTM9608_CMI_047},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM9608_CMI_048), OTM9608_CMI_048},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM9608_CMI_049), OTM9608_CMI_049},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM9608_CMI_050), OTM9608_CMI_050},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM9608_CMI_051), OTM9608_CMI_051},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM9608_CMI_052), OTM9608_CMI_052},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM9608_CMI_053), OTM9608_CMI_053},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM9608_CMI_054), OTM9608_CMI_054},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM9608_CMI_055), OTM9608_CMI_055},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM9608_CMI_056), OTM9608_CMI_056},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM9608_CMI_057), OTM9608_CMI_057},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM9608_CMI_058), OTM9608_CMI_058},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM9608_CMI_059), OTM9608_CMI_059},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM9608_CMI_060), OTM9608_CMI_060},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM9608_CMI_061), OTM9608_CMI_061},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM9608_CMI_062), OTM9608_CMI_062},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM9608_CMI_063), OTM9608_CMI_063},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM9608_CMI_064), OTM9608_CMI_064},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM9608_CMI_065), OTM9608_CMI_065},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM9608_CMI_066), OTM9608_CMI_066},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM9608_CMI_067), OTM9608_CMI_067},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM9608_CMI_068), OTM9608_CMI_068},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM9608_CMI_069), OTM9608_CMI_069},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM9608_CMI_070), OTM9608_CMI_070},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM9608_CMI_071), OTM9608_CMI_071},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM9608_CMI_072), OTM9608_CMI_072},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM9608_CMI_073), OTM9608_CMI_073},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM9608_CMI_074), OTM9608_CMI_074},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM9608_CMI_075), OTM9608_CMI_075},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM9608_CMI_076), OTM9608_CMI_076},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM9608_CMI_077), OTM9608_CMI_077},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM9608_CMI_078), OTM9608_CMI_078},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM9608_CMI_079), OTM9608_CMI_079},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM9608_CMI_080), OTM9608_CMI_080},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM9608_CMI_081), OTM9608_CMI_081},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM9608_CMI_082), OTM9608_CMI_082},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM9608_CMI_083), OTM9608_CMI_083},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM9608_CMI_084), OTM9608_CMI_084},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM9608_CMI_085), OTM9608_CMI_085},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM9608_CMI_086), OTM9608_CMI_086},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM9608_CMI_087), OTM9608_CMI_087},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM9608_CMI_088), OTM9608_CMI_088},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM9608_CMI_089), OTM9608_CMI_089},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM9608_CMI_090), OTM9608_CMI_090},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM9608_CMI_091), OTM9608_CMI_091},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM9608_CMI_092), OTM9608_CMI_092},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM9608_CMI_093), OTM9608_CMI_093},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM9608_CMI_094), OTM9608_CMI_094},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM9608_CMI_095), OTM9608_CMI_095},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM9608_CMI_096), OTM9608_CMI_096},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM9608_CMI_097), OTM9608_CMI_097},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM9608_CMI_098), OTM9608_CMI_098},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM9608_CMI_099), OTM9608_CMI_099},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM9608_CMI_100), OTM9608_CMI_100},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM9608_CMI_101), OTM9608_CMI_101},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM9608_CMI_102), OTM9608_CMI_102},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM9608_CMI_103), OTM9608_CMI_103},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM9608_CMI_104), OTM9608_CMI_104},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM9608_CMI_105), OTM9608_CMI_105},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM9608_CMI_106), OTM9608_CMI_106},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM9608_CMI_107), OTM9608_CMI_107},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM9608_CMI_108), OTM9608_CMI_108},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM9608_CMI_109), OTM9608_CMI_109},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM9608_CMI_110), OTM9608_CMI_110},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM9608_CMI_111), OTM9608_CMI_111},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM9608_CMI_112), OTM9608_CMI_112},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM9608_CMI_113), OTM9608_CMI_113},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM9608_CMI_114), OTM9608_CMI_114},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM9608_CMI_115), OTM9608_CMI_115},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM9608_CMI_116), OTM9608_CMI_116},
	{DTYPE_DCS_WRITE,  1, 0, 0, 120, sizeof(exit_sleep), exit_sleep},
	{DTYPE_DCS_WRITE, 1, 0, 0, 15, sizeof(display_on), display_on},  
	
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM9608_CMI_TE), OTM9608_CMI_TE}, 
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM9608_CMI_44), OTM9608_CMI_44},
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

static char nt3516_page_f0[6] = {0xf0,0x55,0xaa,0x52,0x08,0x01};
static char nt3516_icid_rd_para[2] = {0xc5, 0x00}; 

static struct dsi_cmd_desc nt3516_setpassword_cmd[] = {	
	{DTYPE_DCS_LWRITE, 1, 0, 0, 1, sizeof(nt3516_page_f0),nt3516_page_f0},
};
static struct dsi_cmd_desc nt3516_icid_rd_cmd = {
	DTYPE_DCS_READ, 1, 0, 0, 0, sizeof(nt3516_icid_rd_para), nt3516_icid_rd_para};

static uint32 mipi_get_commic_panle_id(struct msm_fb_data_type *mfd,struct dsi_cmd_desc *para,uint32 len,int mode)
{
	uint32 panelid = 0;
	mipi_dsi_buf_init(&lead_tx_buf);
	mipi_dsi_buf_init(&lead_rx_buf);
	mipi_dsi_cmd_bta_sw_trigger(); 
	if(mode)
		mipi_set_tx_power_mode(1);
	else 
		mipi_set_tx_power_mode(0);
	mipi_dsi_cmds_rx(mfd,&lead_tx_buf, &lead_rx_buf, para,len);
	if(mode)
		mipi_set_tx_power_mode(0);
	panelid = *(uint32 *)(lead_rx_buf.data+7);
	printk("lizhiye, mipi_get_commic_panle_id panelid is %x\n", panelid);
	
	return panelid;
}

static uint32 mipi_get_panle_id(struct msm_fb_data_type *mfd)
{
	uint32 panleid =  mipi_get_commic_panle_id(mfd, &qhd_boe_icid_rd_cmd, 4, 0);
	return panleid;
}

struct mipi_manufacture_ic {
	struct dsi_cmd_desc *readid_tx;
	int readid_len_tx;
	struct dsi_cmd_desc *readid_rx;
	int readid_len_rx;
	int mode;
};

static int mipi_get_manufacture_icid(struct msm_fb_data_type *mfd)
{
	uint32 icid = 0;
	int i ;
	uint32 panelid = 0;
	
	 struct mipi_manufacture_ic mipi_manufacture_icid[] = 
	 {	 	
		{nt3516_setpassword_cmd,ARRAY_SIZE(nt3516_setpassword_cmd),&nt3516_icid_rd_cmd,3,0},
	 };

	for(i = 0; i < ARRAY_SIZE(mipi_manufacture_icid) ; i++)		
	{	lcd_panle_reset();	
		mipi_dsi_buf_init(&lead_tx_buf);
		mipi_dsi_buf_init(&lead_rx_buf);
		mipi_set_tx_power_mode(1);		
		mipi_dsi_cmds_tx(&lead_tx_buf, mipi_manufacture_icid[i].readid_tx,mipi_manufacture_icid[i].readid_len_tx);
		mipi_dsi_cmd_bta_sw_trigger(); 
		
		if(!mipi_manufacture_icid[i].mode)
			mipi_set_tx_power_mode(0);		
		mipi_dsi_cmds_rx(mfd,&lead_tx_buf, &lead_rx_buf, mipi_manufacture_icid[i].readid_rx,mipi_manufacture_icid[i].readid_len_rx);

		if(mipi_manufacture_icid[i].mode)
			mipi_set_tx_power_mode(0);
		
		icid = *(uint32 *)(lead_rx_buf.data);
		
		printk("debug read icid is %x\n",icid & 0xffffff);

		if((icid & 0xffff) == 0x1655)
			return NT35516;
	}
	
	panelid = mipi_get_panle_id(mfd);
	if((panelid & 0xffff) == 0x0896)
		return OTM9608A;
	else
		return 0;
}


u32 mipi_get_panle_vol_flag(uint32 gpio_num)
{
	u32 panelId;
	int rc, val = -1;
	
	rc =gpio_request(gpio_num, "ic_id");
	if (!rc) 
	{
		printk("lizhiye, lcd request id_pin , rc=%d\n", rc);
		gpio_tlmm_config(GPIO_CFG(gpio_num, 0, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_8MA), GPIO_CFG_ENABLE);

		rc = gpio_direction_input(gpio_num);
		if (!rc) 
		{
			val = gpio_get_value_cansleep(gpio_num);
		}
		gpio_free(gpio_num);
	}

	switch(val)
	{
		case 1:
			panelId = (u32)LCD_PANEL_4P5_BOE_OTM9608A_QHD;
			break;
		case 0:
			panelId = (u32)LCD_PANEL_4P5_YUSHUN_OTM9608A_QHD;
			break;
		default:
			panelId = (u32)LCD_PANEL_NOPANEL;
			break;
	}

	return panelId;
		 
}

static uint32 mipi_get_icpanleid(struct msm_fb_data_type *mfd )
{
	uint32 temp;
	uint32 panelId;
	lcd_panle_reset();
	temp = mipi_get_manufacture_icid(mfd);
	switch(temp)
	{
		case OTM9608A:		 
			panelId = mipi_get_panle_vol_flag(GPIO_LCD_ID);
			break;
		case NT35516:
			panelId = (uint32_t)LCD_PANEL_4P5_TIANMA_NT35516_QHD;
			break;
		default:
			panelId = (uint32_t)LCD_PANEL_NOPANEL;
			printk("warnning cann't indentify the chip\n");
			break;
	}
	return panelId;
}

static int mipi_lcd_off(struct platform_device *pdev)
{
	struct msm_fb_data_type *mfd;

	mfd = platform_get_drvdata(pdev);

	if (!mfd)
		return -ENODEV;
	if (mfd->key != MFD_KEY)
		return -EINVAL;
	mipi_set_tx_power_mode(1);

	mipi_dsi_cmds_tx(&lead_tx_buf, display_off_cmds,
			ARRAY_SIZE(display_off_cmds));
	return 0;
}
#ifdef  CONFIG_MACH_OCEANUS
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
#else
static void mipi_zte_set_backlight(struct msm_fb_data_type *mfd)
{
	int current_lel = mfd->bl_level;
	uint32 back_level = 0;
	
#ifdef CONFIG_MACH_COEUS
	uint32 min_blk_level =19,half_blk_level=102;
	uint32 min_wled_level =11,half_wled_level=35;
#endif
//	printk("lcdc_set_bl level=%d, %d\n", current_lel , mfd->panel_power_on);
	if(!mfd->panel_power_on)
		return;
	
	if(current_lel == 0)
		back_level = 0;
#ifdef CONFIG_MACH_COEUS
	else if((current_lel<half_blk_level)&&(current_lel >= min_blk_level))
		back_level = min_wled_level+( half_wled_level-min_wled_level) *(current_lel -min_blk_level)/(half_blk_level-min_blk_level) ;
#endif
	else
		back_level = (current_lel *current_lel *current_lel/100000 + 4*current_lel*current_lel/10000 + 1889*current_lel/10000 + 28288/10000);
	printk("\n[ZYF]ycm lcdc_set_bl level=%d -> wled level=%d", current_lel , back_level);
	
	if(back_level <= 0)
		back_level = 0;
	else if(back_level<10)
		back_level = 10;
	if(back_level >= 255)
		back_level = 255;
	
	if (wled_trigger_initialized) {
		led_trigger_event(bkl_led_trigger, back_level);
		return;
	 } 
}
#endif
static int first_time_panel_on = 1;
static int mipi_lcd_on(struct platform_device *pdev)
{
	struct msm_fb_data_type *mfd = platform_get_drvdata(pdev);
	if (!mfd)
		return -ENODEV;
	if (mfd->key != MFD_KEY)
		return -EINVAL;

	if(first_time_panel_on){
		first_time_panel_on = 0;
		if(LcdPanleID != LCD_PANEL_NOPANEL)
			return 0;
		else
			LcdPanleID = mipi_get_icpanleid(mfd);
	}
	lcd_panle_reset();
	printk("mipi_lcd_on mipi init start LcdPanleID=%d\n",LcdPanleID);
	mipi_set_tx_power_mode(1);
	switch(LcdPanleID){		
		case (u32)LCD_PANEL_4P5_BOE_OTM9608A_QHD:
				mipi_dsi_cmds_tx(&lead_tx_buf, boe_OTM9608A_video_on_cmds,ARRAY_SIZE(boe_OTM9608A_video_on_cmds));
				printk("LCD_PANEL_4P5_BOE_OTM9608A_QHD init ok !!\n");
				break;			
		case (u32)LCD_PANEL_4P5_YUSHUN_OTM9608A_QHD:
				mipi_dsi_cmds_tx(&lead_tx_buf, yushun_OTM9608A_video_on_cmds, ARRAY_SIZE(yushun_OTM9608A_video_on_cmds));
				printk("lizhiye, LCD_PANEL_4P5_YUSHUN_OTM9608A_QHD init ok !!\n");
				break;
		case (u32)LCD_PANEL_4P5_TIANMA_NT35516_QHD:
				mipi_dsi_cmds_tx(&lead_tx_buf, tianma_nt35516_video_on_cmds, ARRAY_SIZE(tianma_nt35516_video_on_cmds));
				printk("lizhiye, LCD_PANEL_4P5_TIANMA_NT35516_QHD init ok !!\n");
				break;	
		case (u32)LCD_PANEL_4P5_LEAD_CMI_OTM9608A_QHD:
				mipi_dsi_cmds_tx(&lead_tx_buf, cmi_OTM9608A_video_on_cmds, ARRAY_SIZE(cmi_OTM9608A_video_on_cmds));
				printk("lizhiye, LCD_PANEL_4P5_LEAD_CMI_OTM9608A_QHD init ok !!\n");
				break;
		default:
				mipi_dsi_cmds_tx(&lead_tx_buf, tianma_nt35516_video_on_cmds, ARRAY_SIZE(tianma_nt35516_video_on_cmds));
				printk("can't get panelid value\n");
				break;
				
	}	
	mipi_set_tx_power_mode(0);
	return 0;
}



static struct msm_fb_panel_data lead_panel_data = {
	.on		= mipi_lcd_on,
	.off		= mipi_lcd_off,
	.set_backlight = mipi_zte_set_backlight,
};

static int ch_used[3];

int mipi_lead_device_register(struct msm_panel_info *pinfo,
					u32 channel, u32 panel)
{
	struct platform_device *pdev = NULL;
	int ret;

	if ((channel >= 3) || ch_used[channel])
		return -ENODEV;

	ch_used[channel] = TRUE;

	pdev = platform_device_alloc("mipi_coeus_panel", (panel << 8)|channel);
	if (!pdev)
		return -ENOMEM;

	lead_panel_data.panel_info = *pinfo;

	ret = platform_device_add_data(pdev, &lead_panel_data,
		sizeof(lead_panel_data));
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


static int __devinit mipi_lead_lcd_probe(struct platform_device *pdev)
{		
	if (pdev->id == 0) 
		return 0;
	
	mipi_dsi_buf_alloc(&lead_tx_buf, DSI_BUF_SIZE);
	mipi_dsi_buf_alloc(&lead_rx_buf, DSI_BUF_SIZE);
	
	msm_fb_add_device(pdev);

	return 0;
}

static struct platform_driver this_driver = {
	.probe  = mipi_lead_lcd_probe,
	.driver = {
		.name   = "mipi_coeus_panel",
	},
};

static int __init mipi_lcd_init(void)
{	printk("mipi_lcd_init\n");
	led_trigger_register_simple("bkl_trigger", &bkl_led_trigger);
	printk("lizhiye, %s: SUCCESS (WLED TRIGGER)\n", __func__);
	wled_trigger_initialized = 1;
	return platform_driver_register(&this_driver);
}

module_init(mipi_lcd_init);
