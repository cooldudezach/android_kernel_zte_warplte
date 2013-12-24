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

#include <mach/irqs.h>
#include "mdp4.h"
#include <linux/leds.h>
#include <linux/spi/spi.h>


#if 0
#ifdef CONFIG_ZTE_PLATFORM
#include <mach/zte_memlog.h>
#endif
#endif

static int wled_trigger_initialized;
DEFINE_LED_TRIGGER(bkl_led_trigger);


#define PM8921_GPIO_BASE		NR_GPIO_IRQS
#define PM8921_GPIO_PM_TO_SYS(pm_gpio)	(pm_gpio - 1 + PM8921_GPIO_BASE)
#define PM8921_MPP_BASE			(PM8921_GPIO_BASE + PM8921_NR_GPIOS)
#define PM8921_MPP_PM_TO_SYS(pm_gpio)	(pm_gpio - 1 + PM8921_MPP_BASE)
#define PM8921_IRQ_BASE			(NR_MSM_IRQS + NR_GPIO_IRQS)

static struct dsi_buf lead_tx_buf;
static struct dsi_buf lead_rx_buf;

//< 2012/5/18-N9210_add_lcd_factory_mode-lizhiye- < short commond here >
extern u32 LcdPanleID;
//>2012/5/18-N9210_add_lcd_factory_mode-lizhiye


static int lcd_bkl_ctl=0xff;
//#ifdef CONFIG_FB_MSM_GPIO
//#define GPIO_LCD_RESET 129
//#else
//#define GPIO_LCD_RESET 84
//#endif
#define GPIO_LCD_RESET 58
static bool onewiremode = false;

/*ic define*/
#define HIMAX_8363 		1
#define HIMAX_8369 		2
#define NOVATEK_35510	3
#define RENESAS_R61408	4
#define CMI_8001		5
#define HIMAX_8369B 	6


#define HIMAX8369_TIANMA_TN_ID		0xB1
#define HIMAX8369_TIANMA_IPS_ID		0xA5
#define HIMAX8369_LEAD_ID				0
#define HIMAX8369_LEAD_HANNSTAR_ID	0x88
#define NT35510_YUSHUN_ID				0
#define NT35510_LEAD_ID				0xA0
#define NT35510_BOE_ID					0xB0
#define HIMAX8369_YUSHUN_IVO_ID	       0x85


/*about icchip sleep and display on */
static char display_off[2] = {0x28, 0x00};
static char enter_sleep[2] = {0x10, 0x00};
static char exit_sleep[2] = {0x11, 0x00};
static char display_on[2] = {0x29, 0x00};
static struct dsi_cmd_desc display_off_cmds[] = {
	{DTYPE_DCS_WRITE, 1, 0, 0, 50, sizeof(display_off), display_off},
	{DTYPE_DCS_WRITE, 1, 0, 0, 120, sizeof(enter_sleep), enter_sleep}
};


/*about himax8363 chip id */
static char hx8363_setpassword_para[4]={0xB9,0xFF,0x83,0x63};
static char hx8363_icid_rd_para[2] = {0xB9, 0x00};   
static char hx8363_panleid_rd_para[2] = {0xda, 0x00};

static struct dsi_cmd_desc hx8363_icid_rd_cmd = 
{
	DTYPE_DCS_READ, 1, 0, 0, 0, sizeof(hx8363_icid_rd_para), hx8363_icid_rd_para
};
static struct dsi_cmd_desc hx8363_setpassword_cmd[] = 
{	
	{DTYPE_DCS_LWRITE, 1, 0, 0, 1, sizeof(hx8363_setpassword_para),hx8363_setpassword_para},

};
static struct dsi_cmd_desc hx8363_panleid_rd_cmd = 
{
	DTYPE_DCS_READ, 1, 0, 0, 0, sizeof(hx8363_panleid_rd_para), hx8363_panleid_rd_para
};

/*about himax8369 chip id */
static char hx8369_setpassword_para[4]={0xB9,0xFF,0x83,0x69};
static char hx8369_icid_rd_para[2] = {0xB9, 0x00}; 
static char hx8369_panleid_rd_para[2] = {0xda, 0x00};    


static struct dsi_cmd_desc hx8369_icid_rd_cmd = 
{
	DTYPE_DCS_READ, 1, 0, 0, 0, sizeof(hx8369_icid_rd_para), hx8369_icid_rd_para
};
static struct dsi_cmd_desc hx8369_setpassword_cmd[] = 
{	
	{DTYPE_DCS_LWRITE, 1, 0, 0, 1, sizeof(hx8369_setpassword_para),hx8369_setpassword_para},

};
static struct dsi_cmd_desc hx8369_panleid_rd_cmd = 
{
	DTYPE_DCS_READ, 1, 0, 0, 0, sizeof(hx8369_panleid_rd_para), hx8369_panleid_rd_para
};


/*about Novatek3511 chip id */
static char nt3511_page_ff[5] = {0xff, 0xaa,0x55,0x25,0x01};
static char nt3511_page_f8[20] = {0xF8,0x01,0x12,0x00,0x20,0x33,0x13,0x00,0x40,0x00,0x00,0x23,0x02,0x99,0xC8,0x00,0x00,0x01,0x00,0x00};
static char nt3511_icid_rd_para[2] = {0xc5, 0x00}; 
static char nt3511_panleid_rd_para[2] = {0xDA, 0x00};    //added by zte_gequn091966 for lead_nt35510,20111226

static struct dsi_cmd_desc nt3511_setpassword_cmd[] = {	
	{DTYPE_DCS_LWRITE, 1, 0, 0, 1, sizeof(nt3511_page_ff),nt3511_page_ff},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 1, sizeof(nt3511_page_f8),nt3511_page_f8}
};
static struct dsi_cmd_desc nt3511_icid_rd_cmd = {
	DTYPE_DCS_READ, 1, 0, 0, 0, sizeof(nt3511_icid_rd_para), nt3511_icid_rd_para};


static struct dsi_cmd_desc nt3511_panleid_rd_cmd = {
	DTYPE_DCS_READ, 1, 0, 0, 0, sizeof(nt3511_panleid_rd_para), nt3511_panleid_rd_para
};   //added by zte_gequn091966 for lead_nt35510,20111226

/*about RENESAS r61408 chip id */
static char r61408_setpassword_para[2]={0xb0,0x04};
static struct dsi_cmd_desc r61408_setpassword_cmd[] = 
{	
	{DTYPE_GEN_LWRITE, 1, 0, 0, 1, sizeof(r61408_setpassword_para),r61408_setpassword_para},

};
static char r61408_icid_rd_para[2] = {0xbf, 0x00}; 
static struct dsi_cmd_desc r61408_icid_rd_cmd = 
{
	DTYPE_GEN_READ1, 1, 0, 0, 1, sizeof(r61408_icid_rd_para), r61408_icid_rd_para
};


/*about himax8369b chip id */
static char hx8369b_setpassword_para[4]={};
static char hx8369b_icid_rd_para[2] = {0x04, 0x00};   
static char hx8369b_panleid_rd_para[2] = {0xdb, 0x00};  

static struct dsi_cmd_desc hx8369b_setpassword_cmd[] = 
{	
	{DTYPE_DCS_LWRITE, 1, 0, 0, 1, sizeof(hx8369b_setpassword_para),hx8369b_setpassword_para},
};
static struct dsi_cmd_desc hx8369b_icid_rd_cmd = 
{
	DTYPE_DCS_READ, 1, 0, 0, 0, sizeof(hx8369b_icid_rd_para), hx8369b_icid_rd_para
};
static struct dsi_cmd_desc hx8369b_panleid_rd_cmd = 
{
	DTYPE_DCS_READ, 1, 0, 0, 0, sizeof(hx8369b_panleid_rd_para), hx8369b_panleid_rd_para
};

static char cmi_icid_rd_para[] = {0xa1,0x00}; //8009
static struct dsi_cmd_desc cmi_icid_rd_cmd = 
{
	DTYPE_DCS_READ, 1, 0, 0, 0, sizeof(cmi_icid_rd_para), cmi_icid_rd_para
};

/**************************************
1. hx8363 yassy start 
**************************************/
static char hx8363_yassy_para_0xb1[13]={0xB1,0x78,0x34,0x08,0x34,0x02,0x13,
								0x11,0x11,0x2d,0x35,0x3F,0x3F};  
static char hx8363_yassy_para_0xba[14]={0xBA,0x80,0x00,0x10,0x08,0x08,0x10,0x7c,0x6e,
								0x6d,0x0a,0x01,0x84,0x43};   //TWO LANE
static char hx8363_yassy_para_0x3a[2]={0x3a,0x77};
//static char hx8363_para_0x36[2]={0x36,0x0a};
static char hx8363_yassy_para_0xb2[4]={0xb2,0x33,0x33,0x22};
static char hx8363_yassy_para_0xb3[2]={0xb3,0x00};
static char hx8363_yassy_para_0xb4[10]={0xb4,0x08,0x12,0x72,0x12,0x06,0x03,0x54,0x03,0x4e};
static char hx8363_yassy_para_0xb6[2]={0xb6,0x2c};
static char hx8363_yassy_para_0xcc[2]={0xcc,0x09};
static char hx8363_yassy_para_0xe0[31]={0xe0,0x01,0x09,0x17,0x10,0x10,0x3e,0x07,
	0x8d,0x90,0x54,0x16,0xd5,0x55,0x53,0x19,0x01,0x09,0x17,0x10,0x10,0x3e,0x07,
	0x8d,0x90,0x54,0x16,0xd5,0x55,0x53,0x19};	

static struct dsi_cmd_desc hx8363_yassy_display_on_cmds[] = 
{

	{DTYPE_DCS_WRITE, 1, 0, 0, 120, sizeof(exit_sleep), exit_sleep},
	
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(hx8363_setpassword_para),hx8363_setpassword_para},	
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(hx8363_yassy_para_0xb1), hx8363_yassy_para_0xb1},	
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(hx8363_yassy_para_0xb2), hx8363_yassy_para_0xb2},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(hx8363_yassy_para_0xba), hx8363_yassy_para_0xba},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(hx8363_yassy_para_0x3a), hx8363_yassy_para_0x3a},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(hx8363_yassy_para_0xb3), hx8363_yassy_para_0xb3},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(hx8363_yassy_para_0xb4), hx8363_yassy_para_0xb4},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(hx8363_yassy_para_0xb6), hx8363_yassy_para_0xb6},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(hx8363_yassy_para_0xcc), hx8363_yassy_para_0xcc},
	//{DTYPE_DCS_WRITE1, 1, 0, 0, 10, sizeof(hx8363_para_0x36), hx8363_para_0x36},		
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(hx8363_yassy_para_0xe0), hx8363_yassy_para_0xe0},
	//{DTYPE_DCS_LWRITE, 1, 0, 0, 1, sizeof(hx8363_para_0xc1), hx8363_para_0xc1},
	//{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(hx8363_para_0xc2), hx8363_para_0xc2},	
	
	{DTYPE_DCS_WRITE, 1, 0, 0, 10, sizeof(display_on), display_on}

};


/**************************************
9. hx8363 YUSHUN IVO start 
**************************************/
static char hx8363_yushun_para_0xb1[13]={0xB1,0x78,0x34,0x07,0x33,0x02,0x13,0x0F,0x00,0x1C,0x24,0x3F,0x3F};  
static char hx8363_yushun_para_0xba[14]={0xBA,0x80,0x00,0x10,0x08,0x08,0x70,0x7C,0x6E,0x6D,0x0A,0x01,0x84,0x43};   //TWO LANE
static char hx8363_yushun_para_0x3a[2]={0x3A,0x70};
static char hx8363_yushun_para_0xb2[5]={0xB2,0x33,0x33,0x22,0xFF};
static char hx8363_yushun_para_0xb3[2]={0xB3,0x01};
static char hx8363_yushun_para_0xb4[10]={0xB4,0x04,0x12,0x72,0x12,0x06,0x03,0x54,0x03,0x4E};
static char hx8363_yushun_para_0xb6[2]={0xB6,0x1C};
static char hx8363_yushun_para_0xcc[2]={0xCC,0x09};
static char hx8363_yushun_para_0xe0[31]={0xE0,0x00,0x1F,0x61,0x0D,0x0C,0x1F,0x0D,0x91,0x50,0x15,0x18,0xD6,0x16,0x52,0x17,0x00,0x1F,0x61,0x0D,0x0C,0x1F,0x0D,0x91,0x50,0x15,0x18,0xD6,0x16,0x52,0x17};	
static struct dsi_cmd_desc hx8363_yushun_display_on_cmds[] = 
{
	{DTYPE_DCS_WRITE, 1, 0, 0, 120, sizeof(exit_sleep), exit_sleep},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(hx8363_setpassword_para),hx8363_setpassword_para},	
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(hx8363_yushun_para_0xb1), hx8363_yushun_para_0xb1},	
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(hx8363_yushun_para_0xb2), hx8363_yushun_para_0xb2},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(hx8363_yushun_para_0xba), hx8363_yushun_para_0xba},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(hx8363_yushun_para_0x3a), hx8363_yushun_para_0x3a},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(hx8363_yushun_para_0xb3), hx8363_yushun_para_0xb3},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(hx8363_yushun_para_0xb4), hx8363_yushun_para_0xb4},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(hx8363_yushun_para_0xb6), hx8363_yushun_para_0xb6},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(hx8363_yushun_para_0xcc), hx8363_yushun_para_0xcc},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(hx8363_yushun_para_0xe0), hx8363_yushun_para_0xe0},
	{DTYPE_DCS_WRITE, 1, 0, 0, 10, sizeof(display_on), display_on}
};

/**************************************
2. hx8369 lead start 
**************************************/
static char hx8369_lead_tn_para_0xb0[3]={0xb0,0x01,0x09};
static char hx8369_lead_tn_para_0xb1[20]={0xB1,0x01,0x00,0x34,0x07,0x00,0x0F,0x0F,
	0x21,0x28,0x3F,0x3F,0x07,0x23,0x01,0xE6,0xE6,0xE6,0xE6,0xE6};  
static char hx8369_lead_tn_para_0xb2[16]={0xB2,0x00,0x23,0x0A,0x0A,0x70,0x00,0xFF,
	0x00,0x00,0x00,0x00,0x03,0x03,0x00,0x01};  //VIDEO MODE
//static char para_0xb2[16]={0xB2,0x00,0x20,0x0A,0x0A,0x70,0x00,0xFF,
//	0x00,0x00,0x00,0x00,0x03,0x03,0x00,0x01};  //CMD MODE
static char hx8369_lead_tn_para_0xb4[6]={0xB4,0x00,0x0C,0x84,0x0C,0x01}; 
static char hx8369_lead_tn_para_0xb6[3]={0xB6,0x2c,0x2c};
static char hx8369_lead_tn_para_0xd5[27]={0xD5,0x00,0x05,0x03,0x00,0x01,0x09,0x10,
	0x80,0x37,0x37,0x20,0x31,0x46,0x8A,0x57,0x9B,0x20,0x31,0x46,0x8A,
	0x57,0x9B,0x07,0x0F,0x07,0x00}; 
static char hx8369_lead_tn_para_0xe0[35]={0xE0,0x00,0x06,0x06,0x29,0x2d,0x3F,0x13,0x32,
	0x08,0x0c,0x0D,0x11,0x14,0x11,0x14,0x0e,0x15,0x00,0x06,0x06,0x29,0x2d,
	0x3F,0x13,0x32,0x08,0x0c,0x0D,0x11,0x14,0x11,0x14,0x0e,0x15};
static char hx8369_lead_tn_para_0x3a[2]={0x3A,0x77}; 
static char hx8369_lead_tn_para_0xba[14]={0xBA,0x00,0xA0,0xC6,0x00,0x0A,0x00,0x10,0x30,
	0x6C,0x02,0x11,0x18,0x40};   //TWO LANE
//static char para_0xba[14]={0xBA,0x00,0xA0,0xC6,0x00,0x0A,0x00,0x10,0x30,
	//0x6C,0x02,0x10,0x18,0x40};   //ONE LANE

static struct dsi_cmd_desc hx8369_lead_display_on_cmds[] = {
	 
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(hx8369_setpassword_para),hx8369_setpassword_para},
	
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(hx8369_lead_tn_para_0xb0), hx8369_lead_tn_para_0xb0},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(hx8369_lead_tn_para_0xb1), hx8369_lead_tn_para_0xb1},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(hx8369_lead_tn_para_0xb2), hx8369_lead_tn_para_0xb2},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(hx8369_lead_tn_para_0xb4), hx8369_lead_tn_para_0xb4},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(hx8369_lead_tn_para_0xb6), hx8369_lead_tn_para_0xb6},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(hx8369_lead_tn_para_0xd5), hx8369_lead_tn_para_0xd5},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(hx8369_lead_tn_para_0xe0), hx8369_lead_tn_para_0xe0},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(hx8369_lead_tn_para_0x3a), hx8369_lead_tn_para_0x3a},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(hx8369_lead_tn_para_0xba), hx8369_lead_tn_para_0xba},

	{DTYPE_DCS_WRITE, 1, 0, 0, 120, sizeof(exit_sleep), exit_sleep},
	{DTYPE_DCS_WRITE, 1, 0, 0, 10, sizeof(display_on), display_on}

};
/**************************************
3. HANNSTAR hx8369 lead start 
**************************************/
static char hx8369_lead_hannstar_para_0xb1[20]={0xB1,0x01,0x00,0x34,0x07,0x00,0x0E,0x0E,
	0x21,0x29,0x3F,0x3F,0x01,0x63,0x01,0xE6,0xE6,0xE6,0xE6,0xE6};  
static char hx8369_lead_hannstar_para_0xb2[16]={0xB2,0x00,0x23,0x07,0x07,0x70,0x00,0xFF,
	0x00,0x00,0x00,0x00,0x03,0x03,0x00,0x01};  //VIDEO MODE
static char hx8369_lead_hannstar_para_0xb4[6]={0xB4,0x02,0x18,0x80,0x13,0x05}; 
static char hx8369_lead_hannstar_para_0xb6[3]={0xB6,0x1F,0x1F};
static char hx8369_lead_hannstar_para_0xcc[2]={0xcc,0x00}; 
static char hx8369_lead_hannstar_para_0xd5[27]={0xD5,0x00,0x0c,0x00,0x00,0x01,0x0f,0x10,
	0x60,0x33,0x37,0x23,0x01,0xB9,0x75,0xA8,0x64,0x00,0x00,0x41,0x06,
	0x50,0x07,0x07,0x0F,0x07,0x00};
static char hx8369_lead_hannstar_para_0xe0[35]={0xE0,0x00,0x03,0x00,0x09,0x09,0x21,0x1B,0x2D,
	0x06,0x0c,0x10,0x15,0x16,0x14,0x16,0x12,0x18,0x00,0x03,0x00,0x09,0x09,
	0x21,0x1B,0x2D,0x06,0x0c,0x10,0x15,0x16,0x14,0x16,0x12,0x18};
static char hx8369_lead_hannstar_para_0x3a[2]={0x3A,0x77}; 
static char hx8369_lead_hannstar_para_0xba[14]={0xBA,0x00,0xA0,0xC6,0x00,0x0A,0x02,0x10,0x30,
	0x6F,0x02,0x11,0x18,0x40};   //TWO LANE

static struct dsi_cmd_desc hx8369_lead_hannstar_display_on_cmds[] = {
	 
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(hx8369_setpassword_para),hx8369_setpassword_para},
	
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(hx8369_lead_hannstar_para_0xb1), hx8369_lead_hannstar_para_0xb1},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(hx8369_lead_hannstar_para_0xb2), hx8369_lead_hannstar_para_0xb2},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(hx8369_lead_hannstar_para_0xb4), hx8369_lead_hannstar_para_0xb4},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(hx8369_lead_hannstar_para_0xb6), hx8369_lead_hannstar_para_0xb6},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(hx8369_lead_hannstar_para_0xcc), hx8369_lead_hannstar_para_0xcc},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(hx8369_lead_hannstar_para_0xd5), hx8369_lead_hannstar_para_0xd5},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(hx8369_lead_hannstar_para_0xe0), hx8369_lead_hannstar_para_0xe0},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(hx8369_lead_hannstar_para_0x3a), hx8369_lead_hannstar_para_0x3a},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(hx8369_lead_hannstar_para_0xba), hx8369_lead_hannstar_para_0xba},
	{DTYPE_DCS_WRITE, 1, 0, 0, 120, sizeof(exit_sleep), exit_sleep},
	{DTYPE_DCS_WRITE, 1, 0, 0, 10, sizeof(display_on), display_on}

};

/**************************************
4. hx8369 tianma TN start 
**************************************/
static char hx8369_tianma_tn_para_0xb1[20]={0xB1,0x01,0x00,0x34,0x0A,0x00,0x11,0x12,0x21,0x29,0x3F,0x3F,
	0x01,0x1a,0x01,0xE6,0xE6,0xE6,0xE6,0xE6}; 
static char hx8369_tianma_tn_para_0xb2[16]={0xB2,0x00,0x23,0x03,0x03,0x70,0x00,0xFF,0x00,0x00,0x00,0x00,
	0x03,0x03,0x00,0x01};  //VIDEO MODE
static char hx8369_tianma_tn_para_0xb4[6]={0xB4,0x02,0x18,0x70,0x13,0x05}; 
static char hx8369_tianma_tn_para_0xb6[3]={0xB6,0x4a,0x4a};
static char hx8369_tianma_tn_para_0xd5[27]={0xD5,0x00,0x0e,0x03,0x29,0x01,0x0f,0x28,0x60,0x11,0x13,0x00,
	0x00,0x40,0x26,0x51,0x37,0x00,0x00,0x71,0x35,0x60,0x24,0x07,0x0F,0x04,0x04}; 
static char hx8369_tianma_tn_para_0xe0[35]={0xE0,0x00,0x02,0x0b,0x0a,0x09,0x18,0x1d,0x2a,0x08,0x11,0x0d,
	0x13,0x15,0x14,0x15,0x0f,0x14,0x00,0x02,0x0b,0x0a,0x09,0x18,0x1d,0x2a,0x08,0x11,0x0d,0x13,0x15,
	0x14,0x15,0x0f,0x14};
static char hx8369_tianma_tn_para_0xcc[2]={0xcc,0x00}; 
static char hx8369_tianma_tn_para_0x3a[2]={0x3A,0x77}; 
static char hx8369_tianma_tn_para_0xba[14]={0xBA,0x00,0xA0,0xC6,0x00,0x0A,0x00,0x10,0x30,0x6F,0x02,0x11,0x18,0x40};   //TWO LANE
static struct dsi_cmd_desc hx8369_tianma_tn_display_on_cmds[] = 
{

	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(hx8369_setpassword_para),hx8369_setpassword_para},
	
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(hx8369_tianma_tn_para_0xb1), hx8369_tianma_tn_para_0xb1},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(hx8369_tianma_tn_para_0xb2), hx8369_tianma_tn_para_0xb2},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(hx8369_tianma_tn_para_0xb4), hx8369_tianma_tn_para_0xb4},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(hx8369_tianma_tn_para_0xb6), hx8369_tianma_tn_para_0xb6},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(hx8369_tianma_tn_para_0xd5),hx8369_tianma_tn_para_0xd5},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(hx8369_tianma_tn_para_0xe0), hx8369_tianma_tn_para_0xe0},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(hx8369_tianma_tn_para_0x3a), hx8369_tianma_tn_para_0x3a},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(hx8369_tianma_tn_para_0xcc), hx8369_tianma_tn_para_0xcc},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(hx8369_tianma_tn_para_0xba), hx8369_tianma_tn_para_0xba},

	{DTYPE_DCS_WRITE, 1, 0, 0, 120, sizeof(exit_sleep), exit_sleep},
	{DTYPE_DCS_WRITE, 1, 0, 0, 10, sizeof(display_on), display_on}

};
/**************************************
5. hx8369 tianma IPS start 
**************************************/
static char hx8369_tianma_ips_para_0xb1[20]={0xB1,0x01,0x00,0x34,0x07,0x00,0x11,0x11,0x2f,0x37,0x3F, 
        0x3F,0x07,0x3a,0x01,0xE6,0xE6,0xE6,0xE6,0xE6}; 
static char hx8369_tianma_ips_para_0xb2[16]={0xB2,0x00,0x2b,0x03,0x03,0x70,0x00,0xFF, 
        0x00,0x00,0x00,0x00,0x03,0x03,0x00,0x01};  //VIDEO MODE 
static char hx8369_tianma_ips_para_0xb4[6]={0xB4,0x00,0x18,0x70,0x00,0x00}; 
static char hx8369_tianma_ips_para_0xb6[3]={0xB6,0x32,0x32}; //flick
static char hx8369_tianma_ips_para_0xd5[27]={0xD5,0x00,0x0e,0x03,0x2b,0x01,0x11,0x28,0x60, 
        0x11,0x13,0x00,0x00,0x60,0xc4,0x71,0xc5,0x00,0x00,0x71,0x35,0x60,0x24,0x07,0x0F,0x04,0x04}; 
static char hx8369_tianma_ips_para_0xe0[35]={0xE0,0x00,0x0d,0x19,0x2f,0x3b,0x3d,0x2e,0x4a,0x08,0x0e,0x0F, 
        0x14,0x16,0x14,0x14,0x14,0x1e,0x00,0x0d,0x19,0x2f,0x3b,0x3d,0x2e,0x4a,0x08,0x0e,0x0F, 
        0x14,0x16,0x14,0x14,0x14,0x1e};  
static char hx8369_tianma_ips_para_0x3a[2]={0x3A,0x77}; 
static char hx8369_tianma_ips_para_0xba[14]={0xBA,0x00,0xA0,0xC6,0x00,0x0A,0x00,0x10,0x30,0x6F,0x02,0x11,0x18,0x40};   //TWO LANE

static struct dsi_cmd_desc hx8369_tianma_ips_display_on_cmds[] = 
{

	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(hx8369_setpassword_para),hx8369_setpassword_para},
	
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(hx8369_tianma_ips_para_0xb1), hx8369_tianma_ips_para_0xb1},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(hx8369_tianma_ips_para_0xb2), hx8369_tianma_ips_para_0xb2},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(hx8369_tianma_ips_para_0xb4), hx8369_tianma_ips_para_0xb4},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(hx8369_tianma_ips_para_0xb6), hx8369_tianma_ips_para_0xb6},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(hx8369_tianma_ips_para_0xd5), hx8369_tianma_ips_para_0xd5},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(hx8369_tianma_ips_para_0xe0), hx8369_tianma_ips_para_0xe0},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(hx8369_tianma_ips_para_0x3a), hx8369_tianma_ips_para_0x3a},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(hx8369_tianma_ips_para_0xba), hx8369_tianma_ips_para_0xba},

	{DTYPE_DCS_WRITE, 1, 0, 0, 120, sizeof(exit_sleep), exit_sleep},
	{DTYPE_DCS_WRITE, 1, 0, 0, 10, sizeof(display_on), display_on}

};


/**************************************
6. nt35510 lead start 
**************************************/

static char nt35510_lead_cmd_page1_f0[6] = {0xf0, 0x55,0xaa,0x52,0x08,0x01};
static char nt35510_lead_cmd_page1_bc[4] = {0xbc, 0x00,0x78,0x1A};
static char nt35510_lead_cmd_page1_bd[4] = {0xbd, 0x00,0x78,0x1A};
static char nt35510_lead_cmd_page1_be[3] = {0xbe, 0x00,0x51};
static char nt35510_lead_cmd_page1_b0[4] = {0xb0, 0x00,0x00,0x00};
static char nt35510_lead_cmd_page1_b1[4] = {0xb1, 0x00,0x00,0x00};
//static char lead_cmd_page1_b7[4] = {0xb7, 0x44,0x44,0x44};
//static char lead_cmd_page1_b3[4] = {0xb1, 0x0B,0x0B,0x0B};
static char nt35510_lead_cmd_page1_b9[4] = {0xb9, 0x34,0x34,0x34};
static char nt35510_lead_cmd_page1_ba[4] = {0xba, 0x16,0x16,0x16};

static char nt35510_lead_cmd_page1_b6[4] = {0xb6, 0x36,0x36,0x36};
static char nt35510_lead_cmd_page1_b7[4] = {0xb7, 0x26,0x26,0x26};
static char nt35510_lead_cmd_page1_b8[4] = {0xb8, 0x26,0x26,0x26};
static char nt35510_lead_cmd_page1_d1[] = {0xD1,0x00,0x00,0x00,0x07,0x00,0x29,0x00,0x58,0x00,0x7F,0x00,0xC4,0x00,0xF6,0x01,0x3D,0x01,0x6C,0x01,0xB2,0x01,0xE3,0x02,0x26,0x02,0x5B,0x02,0x5D,0x02,0x8B,0x02,0xBB,0x02,0xD4,0x02,0xF2,0x03,0x04,0x03,0x1B,0x03,0x29,0x03,0x3C,0x03,0x48,0x03,0x59,0x03,0xE8,0x03,0xFF};
static char nt35510_lead_cmd_page1_d2[] = {0xD2,0x00,0x00,0x00,0x07,0x00,0x29,0x00,0x58,0x00,0x7F,0x00,0xC4,0x00,0xF6,0x01,0x3D,0x01,0x6C,0x01,0xB2,0x01,0xE3,0x02,0x26,0x02,0x5B,0x02,0x5D,0x02,0x8B,0x02,0xBB,0x02,0xD4,0x02,0xF2,0x03,0x04,0x03,0x1B,0x03,0x29,0x03,0x3C,0x03,0x48,0x03,0x59,0x03,0xE8,0x03,0xFF};
static char nt35510_lead_cmd_page1_d3[] = {0xD3,0x00,0x00,0x00,0x07,0x00,0x29,0x00,0x58,0x00,0x7F,0x00,0xC4,0x00,0xF6,0x01,0x3D,0x01,0x6C,0x01,0xB2,0x01,0xE3,0x02,0x26,0x02,0x5B,0x02,0x5D,0x02,0x8B,0x02,0xBB,0x02,0xD4,0x02,0xF2,0x03,0x04,0x03,0x1B,0x03,0x29,0x03,0x3C,0x03,0x48,0x03,0x59,0x03,0xE8,0x03,0xFF};
static char nt35510_lead_cmd_page1_d4[] = {0xD4,0x00,0x00,0x00,0x07,0x00,0x29,0x00,0x58,0x00,0x7F,0x00,0xC4,0x00,0xF6,0x01,0x3D,0x01,0x6C,0x01,0xB2,0x01,0xE3,0x02,0x26,0x02,0x5B,0x02,0x5D,0x02,0x8B,0x02,0xBB,0x02,0xD4,0x02,0xF2,0x03,0x04,0x03,0x1B,0x03,0x29,0x03,0x3C,0x03,0x48,0x03,0x59,0x03,0xE8,0x03,0xFF};
static char nt35510_lead_cmd_page1_d5[] = {0xD5,0x00,0x00,0x00,0x07,0x00,0x29,0x00,0x58,0x00,0x7F,0x00,0xC4,0x00,0xF6,0x01,0x3D,0x01,0x6C,0x01,0xB2,0x01,0xE3,0x02,0x26,0x02,0x5B,0x02,0x5D,0x02,0x8B,0x02,0xBB,0x02,0xD4,0x02,0xF2,0x03,0x04,0x03,0x1B,0x03,0x29,0x03,0x3C,0x03,0x48,0x03,0x59,0x03,0xE8,0x03,0xFF};
static char nt35510_lead_cmd_page1_d6[] = {0xD6,0x00,0x00,0x00,0x07,0x00,0x29,0x00,0x58,0x00,0x7F,0x00,0xC4,0x00,0xF6,0x01,0x3D,0x01,0x6C,0x01,0xB2,0x01,0xE3,0x02,0x26,0x02,0x5B,0x02,0x5D,0x02,0x8B,0x02,0xBB,0x02,0xD4,0x02,0xF2,0x03,0x04,0x03,0x1B,0x03,0x29,0x03,0x3C,0x03,0x48,0x03,0x59,0x03,0xE8,0x03,0xFF};

static char nt35510_lead_cmd_page0_f0[6] = {0xf0, 0x55,0xaa,0x52,0x08,0x00};
static char nt35510_lead_cmd_page0_b1[2] = {0xb1, 0xfc};   //0xcc
//static char nt35510_lead_cmd_page0_b4[2] = {0xb4, 0x10};
static char nt35510_lead_cmd_page0_b6[2] = {0xb6, 0x07};
static char nt35510_lead_cmd_page0_b7[3] = {0xb7, 0x77,0x77};
static char nt35510_lead_cmd_page0_b8[5] = {0xb8, 0x01,0x0A,0x0A,0x0A};
static char nt35510_lead_cmd_page0_bc[4] = {0xbc, 0x05,0x05,0x05};
static char nt35510_lead_cmd_page0_bd[6] = {0xbd, 0x01,0x84,0x07,0x31,0x00};
static char nt35510_lead_cmd_page0_be[6] = {0xbe, 0x01,0x84,0x07,0x31,0x00};
static char nt35510_lead_cmd_page0_bf[6] = {0xbf, 0x01,0x84,0x07,0x31,0x00};

static char nt35510_lead_cmd_page0_c7[2] = {0xc7, 0x02};
static char nt35510_lead_cmd_page0_c9[5] = {0xc9, 0x11,0x00,0x00,0x00};
static char nt35510_lead_cmd_page0_3a[2] = {0x3a, 0x77};
static char nt35510_lead_cmd_page0_35[2] = {0x35, 0x00};
static char nt35510_lead_cmd_page0_21[2] = {0x21, 0x00};




static struct dsi_cmd_desc nt35510_lead_display_on_cmds[] = {

{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt35510_lead_cmd_page1_f0), nt35510_lead_cmd_page1_f0},
{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt35510_lead_cmd_page1_bc), nt35510_lead_cmd_page1_bc},
{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt35510_lead_cmd_page1_bd), nt35510_lead_cmd_page1_bd},
{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt35510_lead_cmd_page1_be), nt35510_lead_cmd_page1_be},
{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt35510_lead_cmd_page1_b0), nt35510_lead_cmd_page1_b0},
{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt35510_lead_cmd_page1_b1), nt35510_lead_cmd_page1_b1},
{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt35510_lead_cmd_page1_b9), nt35510_lead_cmd_page1_b9},
{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt35510_lead_cmd_page1_ba), nt35510_lead_cmd_page1_ba},

{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt35510_lead_cmd_page1_b6), nt35510_lead_cmd_page1_b6},
{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt35510_lead_cmd_page1_b7), nt35510_lead_cmd_page1_b7},
{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt35510_lead_cmd_page1_b8), nt35510_lead_cmd_page1_b8},
{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt35510_lead_cmd_page1_d1), nt35510_lead_cmd_page1_d1},
{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt35510_lead_cmd_page1_d2), nt35510_lead_cmd_page1_d2},
{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt35510_lead_cmd_page1_d3), nt35510_lead_cmd_page1_d3},
{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt35510_lead_cmd_page1_d4), nt35510_lead_cmd_page1_d4},
{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt35510_lead_cmd_page1_d5), nt35510_lead_cmd_page1_d5},
{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt35510_lead_cmd_page1_d6), nt35510_lead_cmd_page1_d6},

{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt35510_lead_cmd_page0_f0), nt35510_lead_cmd_page0_f0},
{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(nt35510_lead_cmd_page0_b1), nt35510_lead_cmd_page0_b1},
//{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(nt35510_lead_cmd_page0_b4), nt35510_lead_cmd_page0_b4},
{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(nt35510_lead_cmd_page0_b6), nt35510_lead_cmd_page0_b6},
{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt35510_lead_cmd_page0_b7), nt35510_lead_cmd_page0_b7},
{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt35510_lead_cmd_page0_b8), nt35510_lead_cmd_page0_b8},
{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt35510_lead_cmd_page0_bc), nt35510_lead_cmd_page0_bc},
{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt35510_lead_cmd_page0_bd), nt35510_lead_cmd_page0_bd},
{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt35510_lead_cmd_page0_be), nt35510_lead_cmd_page0_be},
{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt35510_lead_cmd_page0_bf), nt35510_lead_cmd_page0_bf},

{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt35510_lead_cmd_page0_f0), nt35510_lead_cmd_page0_f0},
{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(nt35510_lead_cmd_page0_c7), nt35510_lead_cmd_page0_c7},
{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt35510_lead_cmd_page0_c9), nt35510_lead_cmd_page0_c9},
{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(nt35510_lead_cmd_page0_3a), nt35510_lead_cmd_page0_3a},
{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(nt35510_lead_cmd_page0_35), nt35510_lead_cmd_page0_35},
{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(nt35510_lead_cmd_page0_21), nt35510_lead_cmd_page0_21},
//{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(lead_cmd_page0_36), lead_cmd_page0_36},
{DTYPE_DCS_WRITE, 1, 0, 0, 120, sizeof(exit_sleep), exit_sleep},
{DTYPE_DCS_WRITE, 1, 0, 0, 10, sizeof(display_on), display_on}

};

/**************************************
7. nt35510 yushun start 
**************************************/
//static char cmd_page_f3[9] = {0xf3, 0x00,0x32,0x00,0x38,0x31,0x08,0x11,0x00};
static char nt3511_yushun_cmd_page0_f0[6] = {0xf0, 0x55,0xaa,0x52,0x08,0x00};
static char nt3511_yushun_cmd_page0_b0[6] = {0xb0, 0x04,0x0a,0x0e,0x09,0x04};
static char nt3511_yushun_cmd_page0_b1[3] = {0xb1, 0x18,0x04};
static char nt3511_yushun_cmd_page0_36[2] = {0x36, 0x90};
static char nt3511_yushun_cmd_page0_b3[2] = {0xb3, 0x00};
static char nt3511_yushun_cmd_page0_b6[2] = {0xb6, 0x03};
static char nt3511_yushun_cmd_page0_b7[3] = {0xb7, 0x70,0x70};
static char nt3511_yushun_cmd_page0_b8[5] = {0xb8, 0x00,0x06,0x06,0x06};
static char nt3511_yushun_cmd_page0_bc[4] = {0xbc, 0x00,0x00,0x00};
static char nt3511_yushun_cmd_page0_bd[6] = {0xbd, 0x01,0x84,0x06,0x50,0x00};
static char nt3511_yushun_cmd_page0_cc[4] = {0xcc, 0x03,0x01,0x06};

static char nt3511_yushun_cmd_page1_f0[6] = {0xf0, 0x55,0xaa,0x52,0x08,0x01};
static char nt3511_yushun_cmd_page1_b0[4] = {0xb0, 0x05,0x05,0x05};
static char nt3511_yushun_cmd_page1_b1[4] = {0xb1, 0x05,0x05,0x05};
static char nt3511_yushun_cmd_page1_b2[4] = {0xb2, 0x03,0x03,0x03};
static char nt3511_yushun_cmd_page1_b8[4] = {0xb8, 0x25,0x25,0x25};
static char nt3511_yushun_cmd_page1_b3[4] = {0xb3, 0x0b,0x0b,0x0b};
static char nt3511_yushun_cmd_page1_b9[4] = {0xb9, 0x34,0x34,0x34};
static char nt3511_yushun_cmd_page1_bf[2] = {0xbf, 0x01};
static char nt3511_yushun_cmd_page1_b5[4] = {0xb5, 0x08,0x08,0x08};
static char nt3511_yushun_cmd_page1_ba[4] = {0xba, 0x24,0x24,0x24};
static char nt3511_yushun_cmd_page1_b4[4] = {0xb4, 0x2e,0x2e,0x2e};
static char nt3511_yushun_cmd_page1_bc[4] = {0xbc, 0x00,0x68,0x00};
static char nt3511_yushun_cmd_page1_bd[4] = {0xbd, 0x00,0x7c,0x00};
static char nt3511_yushun_cmd_page1_be[3] = {0xbe, 0x00,0x45};
static char nt3511_yushun_cmd_page1_d0[5] = {0xd0, 0x0c,0x15,0x0b,0x0e};

static char nt3511_yushun_cmd_page1_d1[53] = {0xd1, 0x00,0x37,0x00,0x61,0x00,0x92,0x00,0xB4,0x00,0xCF,0x00
,0xF6,0x01,0x2F,0x01,0x7F,0x01,0x97,0x01,0xC0,0x01,0xE5,0x02,0x25,0x02,0x5E,0x02,0x60,0x02
,0x87,0x02,0xBE,0x02,0xE2,0x03,0x0F,0x03,0x30,0x03,0x5C,0x03,0x77,0x03,0x94,0x03,0x9F,0x03
,0xAC,0x03,0xBA,0x03,0xF1};
static char nt3511_yushun_cmd_page1_d2[53] = {0xd2, 0x00,0x37,0x00,0x61,0x00,0x92,0x00,0xB4,0x00,0xCF,0x00
,0xF6,0x01,0x2F,0x01,0x7F,0x01,0x97,0x01,0xC0,0x01,0xE5,0x02,0x25,0x02,0x5E,0x02,0x60,0x02
,0x87,0x02,0xBE,0x02,0xE2,0x03,0x0F,0x03,0x30,0x03,0x5C,0x03,0x77,0x03,0x94,0x03,0x9F,0x03
,0xAC,0x03,0xBA,0x03,0xF1};
static char nt3511_yushun_cmd_page1_d3[53] = {0xd3, 0x00,0x37,0x00,0x61,0x00,0x92,0x00,0xB4,0x00,0xCF,0x00
,0xF6,0x01,0x2F,0x01,0x7F,0x01,0x97,0x01,0xC0,0x01,0xE5,0x02,0x25,0x02,0x5E,0x02,0x60,0x02
,0x87,0x02,0xBE,0x02,0xE2,0x03,0x0F,0x03,0x30,0x03,0x5C,0x03,0x77,0x03,0x94,0x03,0x9F,0x03
,0xAC,0x03,0xBA,0x03,0xF1};
static char nt3511_yushun_cmd_page1_d4[53] = {0xd4, 0x00,0x37,0x00,0x50,0x00,0x89,0x00,0xA9,0x00,0xC0,0x00
,0xF6,0x01,0x14,0x01,0x48,0x01,0x6B,0x01,0xA7,0x01,0xD3,0x02,0x17,0x02,0x4F,0x02,0x51,0x02
,0x86,0x02,0xBD,0x02,0xE2,0x03,0x0F,0x03,0x30,0x03,0x5C,0x03,0x77,0x03,0x94,0x03,0x9F,0x03
,0xAC,0x03,0xBA,0x03,0xF1};
static char nt3511_yushun_cmd_page1_d5[53] = {0xd5, 0x00,0x37,0x00,0x50,0x00,0x89,0x00,0xA9,0x00,0xC0,0x00
,0xF6,0x01,0x14,0x01,0x48,0x01,0x6B,0x01,0xA7,0x01,0xD3,0x02,0x17,0x02,0x4F,0x02,0x51,0x02
,0x86,0x02,0xBD,0x02,0xE2,0x03,0x0F,0x03,0x30,0x03,0x5C,0x03,0x77,0x03,0x94,0x03,0x9F,0x03
,0xAC,0x03,0xBA,0x03,0xF1};
static char nt3511_yushun_cmd_page1_d6[53] = {0xd6, 0x00,0x37,0x00,0x50,0x00,0x89,0x00,0xA9,0x00,0xC0,0x00
,0xF6,0x01,0x14,0x01,0x48,0x01,0x6B,0x01,0xA7,0x01,0xD3,0x02,0x17,0x02,0x4F,0x02,0x51,0x02
,0x86,0x02,0xBD,0x02,0xE2,0x03,0x0F,0x03,0x30,0x03,0x5C,0x03,0x77,0x03,0x94,0x03,0x9F,0x03
,0xAC,0x03,0xBA,0x03,0xF1};

static struct dsi_cmd_desc nt3511_yushun_display_on_cmds[] = {

       // yushun nt35510
	//{DTYPE_DCS_LWRITE, 1, 0, 0, 1, sizeof(cmd_page_ff),cmd_page_ff},
	//{DTYPE_DCS_LWRITE, 1, 0, 0, 1, sizeof(cmd_page_f3),cmd_page_f3},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt3511_yushun_cmd_page0_f0),nt3511_yushun_cmd_page0_f0},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt3511_yushun_cmd_page0_b0),nt3511_yushun_cmd_page0_b0},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt3511_yushun_cmd_page0_b1),nt3511_yushun_cmd_page0_b1},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(nt3511_yushun_cmd_page0_36),nt3511_yushun_cmd_page0_36},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(nt3511_yushun_cmd_page0_b3),nt3511_yushun_cmd_page0_b3},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(nt3511_yushun_cmd_page0_b6),nt3511_yushun_cmd_page0_b6},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt3511_yushun_cmd_page0_b7),nt3511_yushun_cmd_page0_b7},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt3511_yushun_cmd_page0_b8),nt3511_yushun_cmd_page0_b8},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt3511_yushun_cmd_page0_bc),nt3511_yushun_cmd_page0_bc},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt3511_yushun_cmd_page0_bd),nt3511_yushun_cmd_page0_bd},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt3511_yushun_cmd_page0_cc),nt3511_yushun_cmd_page0_cc},

	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt3511_yushun_cmd_page1_f0),nt3511_yushun_cmd_page1_f0},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt3511_yushun_cmd_page1_b0),nt3511_yushun_cmd_page1_b0},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt3511_yushun_cmd_page1_b1),nt3511_yushun_cmd_page1_b1},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt3511_yushun_cmd_page1_b2),nt3511_yushun_cmd_page1_b2},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt3511_yushun_cmd_page1_b8),nt3511_yushun_cmd_page1_b8},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt3511_yushun_cmd_page1_b3),nt3511_yushun_cmd_page1_b3},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt3511_yushun_cmd_page1_b9),nt3511_yushun_cmd_page1_b9},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(nt3511_yushun_cmd_page1_bf),nt3511_yushun_cmd_page1_bf},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt3511_yushun_cmd_page1_b5),nt3511_yushun_cmd_page1_b5},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt3511_yushun_cmd_page1_ba),nt3511_yushun_cmd_page1_ba},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt3511_yushun_cmd_page1_b4),nt3511_yushun_cmd_page1_b4},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt3511_yushun_cmd_page1_bc),nt3511_yushun_cmd_page1_bc},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt3511_yushun_cmd_page1_bd),nt3511_yushun_cmd_page1_bd},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt3511_yushun_cmd_page1_be),nt3511_yushun_cmd_page1_be},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt3511_yushun_cmd_page1_d0),nt3511_yushun_cmd_page1_d0},

	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt3511_yushun_cmd_page1_d1), nt3511_yushun_cmd_page1_d1},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt3511_yushun_cmd_page1_d2), nt3511_yushun_cmd_page1_d2},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt3511_yushun_cmd_page1_d3), nt3511_yushun_cmd_page1_d3},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt3511_yushun_cmd_page1_d4), nt3511_yushun_cmd_page1_d4},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt3511_yushun_cmd_page1_d5), nt3511_yushun_cmd_page1_d5},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt3511_yushun_cmd_page1_d6), nt3511_yushun_cmd_page1_d6},

	{DTYPE_DCS_WRITE, 1, 0, 0, 120, sizeof(exit_sleep), exit_sleep},
	{DTYPE_DCS_WRITE, 1, 0, 0, 10, sizeof(display_on), display_on}

};


/**************************************
8. 461408 truly start 
**************************************/
static char r61408_truly_lg_para_0xb0[2]={0xB0,0x04}; 
static char r61408_truly_lg_para_0xb3[3]={0xB3,0x10,0x00}; 
static char r61408_truly_lg_para_0xbd[2]={0xbd,0x00}; 
static char r61408_truly_lg_para_0xc0[3]={0xc0,0x00,0x66};
static char r61408_truly_lg_para_0xc1[16]={0xc1,0x23,0x31,0x99,0x26,0x25,0x00,
	0x10,0x28,0x0c,0x0c,0x00,0x00,0x00,0x21,0x01};
static char r61408_truly_lg_para_0xc2[7]={0xc2,0x10,0x06,0x06,0x01,0x03,0x00};
static char r61408_truly_lg_para_0xc8[25]={0xc8,0x00,0x0e,0x17,0x20,0x2e,0x4b,
	0x3b,0x28,0x19,0x11,0x0a,0x02,0x00,0x0e,0x15,0x20,0x2e,0x47,0x3b,0x28,0x19,
	0x11,0x0a,0x02};
static char r61408_truly_lg_para_0xc9[25]={0xc9,0x00,0x0e,0x17,0x20,0x2e,0x4b,
	0x3b,0x28,0x19,0x11,0x0a,0x02,0x00,0x0e,0x15,0x20,0x2e,0x47,0x3b,0x28,0x19,
	0x11,0x0a,0x02};
static char r61408_truly_lg_para_0xca[25]={0xca,0x00,0x0e,0x17,0x20,0x2e,0x4b,
	0x3b,0x28,0x19,0x11,0x0a,0x02,0x00,0x0e,0x15,0x20,0x2e,0x47,0x3b,0x28,0x19,
	0x11,0x0a,0x02};
static char r61408_truly_lg_para_0xd0[17]={0xd0,0x29,0x03,0xce,0xa6,0x0c,0x43,
	0x20,0x10,0x01,0x00,0x01,0x01,0x00,0x03,0x01,0x00};
static char r61408_truly_lg_para_0xd1[8]={0xd1,0x18,0x0c,0x23,0x03,0x75,0x02,0x50};
static char r61408_truly_lg_para_0xd3[2]={0xd3,0x33};
static char r61408_truly_lg_para_0xd5[3]={0xd5,0x2a,0x2a};
static char r61408_truly_lg_para_0xde[3]={0xde,0x01,0x51};
static char r61408_truly_lg_para_0xe6[2]={0xe6,0x51};//vcomdc flick
static char r61408_truly_lg_para_0xfa[2]={0xfa,0x03};
static char r61408_truly_lg_para_0xd6[2]={0xd6,0x28};
static char r61408_truly_lg_para_0x2a[5]={0x2a,0x00,0x00,0x01,0xdf};
static char r61408_truly_lg_para_0x2b[5]={0x2b,0x00,0x00,0x03,0x1f};
static char r61408_truly_lg_para_0x36[2]={0x36,0x00};
static char r61408_truly_lg_para_0x3a[2]={0x3a,0x77};


static struct dsi_cmd_desc r61408_truly_lg_display_on_cmds[] = 
{
	
	
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(r61408_truly_lg_para_0xb0), r61408_truly_lg_para_0xb0},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(r61408_truly_lg_para_0xb3), r61408_truly_lg_para_0xb3},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(r61408_truly_lg_para_0xbd), r61408_truly_lg_para_0xbd},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(r61408_truly_lg_para_0xc0), r61408_truly_lg_para_0xc0},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(r61408_truly_lg_para_0xc1), r61408_truly_lg_para_0xc1},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(r61408_truly_lg_para_0xc2), r61408_truly_lg_para_0xc2},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(r61408_truly_lg_para_0xc8), r61408_truly_lg_para_0xc8},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(r61408_truly_lg_para_0xc9), r61408_truly_lg_para_0xc9},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(r61408_truly_lg_para_0xca), r61408_truly_lg_para_0xca},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(r61408_truly_lg_para_0xd0), r61408_truly_lg_para_0xd0},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(r61408_truly_lg_para_0xd1), r61408_truly_lg_para_0xd1},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(r61408_truly_lg_para_0xd3), r61408_truly_lg_para_0xd3},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(r61408_truly_lg_para_0xd5), r61408_truly_lg_para_0xd5},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(r61408_truly_lg_para_0xde), r61408_truly_lg_para_0xde},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(r61408_truly_lg_para_0xe6), r61408_truly_lg_para_0xe6},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(r61408_truly_lg_para_0xfa), r61408_truly_lg_para_0xfa},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(r61408_truly_lg_para_0xd6), r61408_truly_lg_para_0xd6},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(r61408_truly_lg_para_0x2a), r61408_truly_lg_para_0x2a},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(r61408_truly_lg_para_0x2b), r61408_truly_lg_para_0x2b},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(r61408_truly_lg_para_0x36), r61408_truly_lg_para_0x36},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(r61408_truly_lg_para_0x3a), r61408_truly_lg_para_0x3a},

	{DTYPE_DCS_WRITE, 1, 0, 0, 120, sizeof(exit_sleep), exit_sleep},
	{DTYPE_DCS_WRITE, 1, 0, 0, 10, sizeof(display_on), display_on}

};

static char HX8369B_TM04YVHP12_para_01[]= {0xB9,0xFF,0x83,0x69};
static char HX8369B_TM04YVHP12_para_02[]= {0xB1,0x13,0x83,0x77,0x00,0x0F,0x0F,0x1F,0x1F};
static char HX8369B_TM04YVHP12_para_03[]= {0xB2,0x00,0x10,0x02};
static char HX8369B_TM04YVHP12_para_04[]= {0xB3,0x83,0x00,0x31,0x03};
static char HX8369B_TM04YVHP12_para_05[]= {0xB4,0x12};
static char HX8369B_TM04YVHP12_para_06[]= {0xB5,0x15,0x15,0x3E};
static char HX8369B_TM04YVHP12_para_07[]= {0xB6,0xA7,0xA7};
static char HX8369B_TM04YVHP12_para_08[]= 
{
0xE0,0x00,0x11,0x1C,0x35,0x3B,0x3F,0x33,
0x49,0x08,0x0D,0x10,0x14,0x16,0x13,0x14,
0x19,0x1F,0x00,0x11,0x1C,0x35,0x3B,0x3F,
0x33,0x49,0x08,0x0D,0x10,0x14,0x16,0x13,
0x14,0x19,0x1F,0x01
};
static char HX8369B_TM04YVHP12_para_09[]= {0xBC,0x5E};
static char HX8369B_TM04YVHP12_para_10[]= {0xC6,0x40};
static char HX8369B_TM04YVHP12_para_11[]= 
{
0xD5,0x00,0x00,0x12,0x03,0x33,0x00,0x00,
0x10,0x01,0x00,0x00,0x00,0x10,0x40,0x14,
0x00,0x00,0x23,0x10,0x3e,0x13,0x00,0x00,
0x00,0xC3,0x00,0x00,0x00,0x00,0x03,0x00,
0x00,0x03,0x00,0x00,0x00,0x00,0x00,0x89,
0x00,0x66,0x11,0x33,0x11,0x00,0x00,0x00,
0x98,0x00,0x00,0x22,0x00,0x44,0x00,0x00,
0x00,0x89,0x00,0x44,0x00,0x22,0x99,0x00,
0x00,0x00,0x98,0x00,0x11,0x33,0x11,0x55,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x01,0x5A
};
static char HX8369B_TM04YVHP12_para_12[]= {0xEA,0x62};
static char HX8369B_TM04YVHP12_para_13[]= 
{
0xBA,0x41,0x00,0x16,0xC6,0x80,0x0A,0x00,
0x10,0x24,0x02,0x21,0x21,0x9A,0x11,0x14
};
static char HX8369B_TM04YVHP12_para_14[]= 
{
0xcc,0x02
};


static struct dsi_cmd_desc HX8369B_TM04YVHP12_display_on_cmds[] = 
{
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(HX8369B_TM04YVHP12_para_01), HX8369B_TM04YVHP12_para_01},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(HX8369B_TM04YVHP12_para_02), HX8369B_TM04YVHP12_para_02},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(HX8369B_TM04YVHP12_para_03), HX8369B_TM04YVHP12_para_03},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(HX8369B_TM04YVHP12_para_04), HX8369B_TM04YVHP12_para_04},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(HX8369B_TM04YVHP12_para_05), HX8369B_TM04YVHP12_para_05},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(HX8369B_TM04YVHP12_para_06), HX8369B_TM04YVHP12_para_06},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(HX8369B_TM04YVHP12_para_07), HX8369B_TM04YVHP12_para_07},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(HX8369B_TM04YVHP12_para_08), HX8369B_TM04YVHP12_para_08},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(HX8369B_TM04YVHP12_para_09), HX8369B_TM04YVHP12_para_09},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(HX8369B_TM04YVHP12_para_10), HX8369B_TM04YVHP12_para_10},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(HX8369B_TM04YVHP12_para_11), HX8369B_TM04YVHP12_para_11},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(HX8369B_TM04YVHP12_para_12), HX8369B_TM04YVHP12_para_12},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(HX8369B_TM04YVHP12_para_13), HX8369B_TM04YVHP12_para_13},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(HX8369B_TM04YVHP12_para_14), HX8369B_TM04YVHP12_para_14},
	{DTYPE_DCS_WRITE, 1, 0, 0, 150, sizeof(exit_sleep), exit_sleep},
	{DTYPE_DCS_WRITE, 1, 0, 0, 50, sizeof(display_on), display_on}
};

/**************************************
9. boe NT35510 start 
**************************************/
	static char boe_nt35510_para01[]={0xF0,0x55,0xAA,0x52,0x08,0x01};																																																									
	static char boe_nt35510_para02[]={0xB6,0x34,0x34,0x34}; 																																																													 
	static char boe_nt35510_para03[]={0xB0,0x09,0x09,0x09}; 																																																													 
	static char boe_nt35510_para04[]={0xB7,0x24,0x24,0x24}; 																																																													 
	static char boe_nt35510_para05[]={0xB1,0x09,0x09,0x09}; 																																																													 
	static char boe_nt35510_para06[]={0xB8,0x34};																																																													   
	static char boe_nt35510_para07[]={0xB2,0x00};																																																													   
	static char boe_nt35510_para08[]={0xB9,0x24,0x24,0x24}; 																																																													 
	static char boe_nt35510_para09[]={0xB3,0x05,0x05,0x05}; 																																																													 
	static char boe_nt35510_para10[]={0xBF,0x01};																																																													   
	static char boe_nt35510_para11[]={0xBA,0x34,0x34,0x34}; 																																																													 
	static char boe_nt35510_para12[]={0xB5,0x0B,0x0B,0x0B}; 																																																													 
	static char boe_nt35510_para13[]={0xBC,0x00,0xA3,0x00}; 																																																													 
	static char boe_nt35510_para14[]={0xBD,0x00,0xA3,0x00}; 																																																													 
	static char boe_nt35510_para15[]={0xBE,0x00,0x50};																																																														
	static char boe_nt35510_para16[]={0xD1,0x00,0x37,0x00,0x52,0x00,0x7B,0x00,0x99,0x00,0xB1,0x00,0xD2,0x00,0xF6,0x01,0x27,0x01,0x4E,0x01,0x8C,0x01,0xBE,0x02,0x0B,0x02,0x48,0x02,0x4A,0x02,0x7E,0x02,0xBC,0x02,0xE1,0x03,0x10,0x03,0x31,0x03,0x5A,0x03,0x73,0x03,0x94,0x03,0x9F,0x03,0xB3,0x03,0xB9,0x03,0xC1}; 
	static char boe_nt35510_para17[]={0xD2,0x00,0x37,0x00,0x52,0x00,0x7B,0x00,0x99,0x00,0xB1,0x00,0xD2,0x00,0xF6,0x01,0x27,0x01,0x4E,0x01,0x8C,0x01,0xBE,0x02,0x0B,0x02,0x48,0x02,0x4A,0x02,0x7E,0x02,0xBC,0x02,0xE1,0x03,0x10,0x03,0x31,0x03,0x5A,0x03,0x73,0x03,0x94,0x03,0x9F,0x03,0xB3,0x03,0xB9,0x03,0xC1}; 
	static char boe_nt35510_para18[]={0xD3,0x00,0x37,0x00,0x52,0x00,0x7B,0x00,0x99,0x00,0xB1,0x00,0xD2,0x00,0xF6,0x01,0x27,0x01,0x4E,0x01,0x8C,0x01,0xBE,0x02,0x0B,0x02,0x48,0x02,0x4A,0x02,0x7E,0x02,0xBC,0x02,0xE1,0x03,0x10,0x03,0x31,0x03,0x5A,0x03,0x73,0x03,0x94,0x03,0x9F,0x03,0xB3,0x03,0xB9,0x03,0xC1}; 
	static char boe_nt35510_para19[]={0xD4,0x00,0x37,0x00,0x52,0x00,0x7B,0x00,0x99,0x00,0xB1,0x00,0xD2,0x00,0xF6,0x01,0x27,0x01,0x4E,0x01,0x8C,0x01,0xBE,0x02,0x0B,0x02,0x48,0x02,0x4A,0x02,0x7E,0x02,0xBC,0x02,0xE1,0x03,0x10,0x03,0x31,0x03,0x5A,0x03,0x73,0x03,0x94,0x03,0x9F,0x03,0xB3,0x03,0xB9,0x03,0xC1}; 
	static char boe_nt35510_para20[]={0xD5,0x00,0x37,0x00,0x52,0x00,0x7B,0x00,0x99,0x00,0xB1,0x00,0xD2,0x00,0xF6,0x01,0x27,0x01,0x4E,0x01,0x8C,0x01,0xBE,0x02,0x0B,0x02,0x48,0x02,0x4A,0x02,0x7E,0x02,0xBC,0x02,0xE1,0x03,0x10,0x03,0x31,0x03,0x5A,0x03,0x73,0x03,0x94,0x03,0x9F,0x03,0xB3,0x03,0xB9,0x03,0xC1}; 
	static char boe_nt35510_para21[]={0xD6,0x00,0x37,0x00,0x52,0x00,0x7B,0x00,0x99,0x00,0xB1,0x00,0xD2,0x00,0xF6,0x01,0x27,0x01,0x4E,0x01,0x8C,0x01,0xBE,0x02,0x0B,0x02,0x48,0x02,0x4A,0x02,0x7E,0x02,0xBC,0x02,0xE1,0x03,0x10,0x03,0x31,0x03,0x5A,0x03,0x73,0x03,0x94,0x03,0x9F,0x03,0xB3,0x03,0xB9,0x03,0xC1}; 
	static char boe_nt35510_para22[]={0xF0,0x55,0xAA,0x52,0x08,0x00};																																																								   
	static char boe_nt35510_para23[]={0xB0,0x00,0x05,0x02,0x05,0x02};																																																								   
	static char boe_nt35510_para24[]={0xB6,0x0A};																																																													   
	static char boe_nt35510_para25[]={0xB7,0x00,0x00};																																																														
	static char boe_nt35510_para26[]={0xB8,0x01,0x05,0x05,0x05};																																																								  
	static char boe_nt35510_para27[]={0xBC,0x00,0x00,0x00}; 																																																													 
	static char boe_nt35510_para28[]={0xCC,0x03,0x00,0x00}; 																																																													 
	static char boe_nt35510_para29[]={0xBD,0x01,0x84,0x07,0x31,0x00};																																																								   
	static char boe_nt35510_para30[]={0xBA,0x01};																																																													   
	static char boe_nt35510_para31[]={0x3A,0x77};																																																													   
	static char boe_nt35510_para32[]={0xB4,0x10,0x00};																																																								   
	static char boe_nt35510_para33[]={0xB1,0xf8,0x00};

	static struct dsi_cmd_desc boe_nt35510_display_on_cmds[] = {

	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(boe_nt35510_para01), boe_nt35510_para01}, 
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(boe_nt35510_para02), boe_nt35510_para02}, 
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(boe_nt35510_para03), boe_nt35510_para03}, 
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(boe_nt35510_para04), boe_nt35510_para04}, 
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(boe_nt35510_para05), boe_nt35510_para05}, 
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(boe_nt35510_para06), boe_nt35510_para06}, 
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(boe_nt35510_para07), boe_nt35510_para07}, 
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(boe_nt35510_para08), boe_nt35510_para08}, 
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(boe_nt35510_para09), boe_nt35510_para09}, 
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(boe_nt35510_para10), boe_nt35510_para10}, 
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(boe_nt35510_para11), boe_nt35510_para11}, 
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(boe_nt35510_para12), boe_nt35510_para12}, 
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(boe_nt35510_para13), boe_nt35510_para13}, 
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(boe_nt35510_para14), boe_nt35510_para14}, 
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(boe_nt35510_para15), boe_nt35510_para15}, 
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(boe_nt35510_para16), boe_nt35510_para16}, 
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(boe_nt35510_para17), boe_nt35510_para17}, 
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(boe_nt35510_para18), boe_nt35510_para18}, 
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(boe_nt35510_para19), boe_nt35510_para19}, 
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(boe_nt35510_para20), boe_nt35510_para20}, 
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(boe_nt35510_para21), boe_nt35510_para21}, 
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(boe_nt35510_para22), boe_nt35510_para22}, 
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(boe_nt35510_para23), boe_nt35510_para23}, 
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(boe_nt35510_para24), boe_nt35510_para24}, 
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(boe_nt35510_para25), boe_nt35510_para25}, 
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(boe_nt35510_para26), boe_nt35510_para26}, 
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(boe_nt35510_para27), boe_nt35510_para27}, 
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(boe_nt35510_para28), boe_nt35510_para28}, 
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(boe_nt35510_para29), boe_nt35510_para29}, 
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(boe_nt35510_para30), boe_nt35510_para30}, 
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(boe_nt35510_para31), boe_nt35510_para31}, 
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(boe_nt35510_para32), boe_nt35510_para32}, 
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(boe_nt35510_para33), boe_nt35510_para33}, 
	
	{DTYPE_DCS_WRITE, 1, 0, 0, 120, sizeof(exit_sleep), exit_sleep},
	{DTYPE_DCS_WRITE, 1, 0, 0, 10, sizeof(display_on), display_on}

};

static char OTM_8009_CMI_para001[] = {0x00,0x00};
static char OTM_8009_CMI_para002[] = {0xff,0x80,0x09,0x01};
static char OTM_8009_CMI_para003[] = {0x00,0x80};
static char OTM_8009_CMI_para004[] = {0xff,0x80,0x09};
static char OTM_8009_CMI_para005[] = {0x00,0xb4};//colum inversion
static char OTM_8009_CMI_para006[] = {0xc0,0x55};
static char OTM_8009_CMI_para007[] = {0x00,0x82};
static char OTM_8009_CMI_para008[] = {0xc5,0xa3};
static char OTM_8009_CMI_para009[] = {0x00,0x90};
static char OTM_8009_CMI_para010[] = {0xc5,0xd6,0x76};
static char OTM_8009_CMI_para011[] = {0x00,0x00};//gvdd w1
static char OTM_8009_CMI_para012[] = {0xd8,0xa7,0xa7};
static char OTM_8009_CMI_para013[] = {0x00,0x81};//frame frequency 65Hz
static char OTM_8009_CMI_para014[] = {0xc1,0x66};
static char OTM_8009_CMI_para015[] = {0x00,0xa1};
static char OTM_8009_CMI_para016[] = {0xc1,0x08};
static char OTM_8009_CMI_para017[] = {0x00,0x8A}; //add 2013/6/11  inl
static char OTM_8009_CMI_para018[] = {0xc0,0x40};
static char OTM_8009_CMI_para019[] = {0x00,0xa3};
static char OTM_8009_CMI_para020[] = {0xc0,0x1b};
static char OTM_8009_CMI_para021[] = {0x00,0x81};
static char OTM_8009_CMI_para022[] = {0xc4,0x83};
static char OTM_8009_CMI_para023[] = {0x00,0x92};
static char OTM_8009_CMI_para024[] = {0xc5,0x01};
static char OTM_8009_CMI_para025[] = {0x00,0xb1};
static char OTM_8009_CMI_para026[] = {0xc5,0xa9};
static char OTM_8009_CMI_para027[] = {0x00,0x81};
static char OTM_8009_CMI_para028[] = {0xf5,0x14};
static char OTM_8009_CMI_para029[] = {0x00,0x83};
static char OTM_8009_CMI_para030[] = {0xf5,0x14};
static char OTM_8009_CMI_para031[] = {0x00,0x85};
static char OTM_8009_CMI_para032[] = {0xf5,0x14};
static char OTM_8009_CMI_para033[] = {0x00,0x87};
static char OTM_8009_CMI_para034[] = {0xf5,0x14};
static char OTM_8009_CMI_para035[] = {0x00,0x89};
static char OTM_8009_CMI_para036[] = {0xf5,0x14};
static char OTM_8009_CMI_para037[] = {0x00,0x91};
static char OTM_8009_CMI_para038[] = {0xf5,0x14};
static char OTM_8009_CMI_para039[] = {0x00,0x93};
static char OTM_8009_CMI_para040[] = {0xf5,0x14};
static char OTM_8009_CMI_para041[] = {0x00,0x95};
static char OTM_8009_CMI_para042[] = {0xf5,0x14};
static char OTM_8009_CMI_para043[] = {0x00,0x97};
static char OTM_8009_CMI_para044[] = {0xf5,0x14};
static char OTM_8009_CMI_para045[] = {0x00,0x99};
static char OTM_8009_CMI_para046[] = {0xf5,0x14};
static char OTM_8009_CMI_para047[] = {0x00,0xa1};
static char OTM_8009_CMI_para048[] = {0xf5,0x14};
static char OTM_8009_CMI_para049[] = {0x00,0xa3};
static char OTM_8009_CMI_para050[] = {0xf5,0x14};
static char OTM_8009_CMI_para051[] = {0x00,0xa5};
static char OTM_8009_CMI_para052[] = {0xf5,0x14};
static char OTM_8009_CMI_para053[] = {0x00,0xa7};
static char OTM_8009_CMI_para054[] = {0xf5,0x14};
static char OTM_8009_CMI_para055[] = {0x00,0xb1};
static char OTM_8009_CMI_para056[] = {0xf5,0x14};
static char OTM_8009_CMI_para057[] = {0x00,0xb3};
static char OTM_8009_CMI_para058[] = {0xf5,0x14};
static char OTM_8009_CMI_para059[] = {0x00,0xb5};
static char OTM_8009_CMI_para060[] = {0xf5,0x14};
static char OTM_8009_CMI_para061[] = {0x00,0xb7};
static char OTM_8009_CMI_para062[] = {0xf5,0x14};
static char OTM_8009_CMI_para063[] = {0x00,0xb9};
static char OTM_8009_CMI_para064[] = {0xf5,0x14};
static char OTM_8009_CMI_para065[] = {0x00,0x80};
static char OTM_8009_CMI_para066[] = {0xc4,0x30};
static char OTM_8009_CMI_para067[] = {0x00,0x80};
static char OTM_8009_CMI_para068[] = {0xce,0x84,0x03,0x0a,0x83,0x03,0x0a};
static char OTM_8009_CMI_para069[] = {0x00,0x90};
static char OTM_8009_CMI_para070[] = {0xce,0x33,0x27,0x0a,0x33,0x28,0x0a};
static char OTM_8009_CMI_para071[] = {0x00,0xa0};
static char OTM_8009_CMI_para072[] = {0xce,0x38,0x02,0x03,0x21,0x00,0x0a,0x00,0x38,0x01,0x03,0x22,0x00,0x0a,0x00};
static char OTM_8009_CMI_para073[] = {0x00,0xb0};
static char OTM_8009_CMI_para074[] = {0xce,0x38,0x00,0x03,0x23,0x00,0x0a,0x00,0x30,0x00,0x03,0x24,0x00,0x0a,0x00};
static char OTM_8009_CMI_para075[] = {0x00,0xc0};
static char OTM_8009_CMI_para076[] = {0xce,0x30,0x01,0x03,0x25,0x00,0x0a,0x00,0x30,0x02,0x03,0x26,0x00,0x0a,0x00};
static char OTM_8009_CMI_para077[] = {0x00,0xd0};
static char OTM_8009_CMI_para078[] = {0xce,0x30,0x03,0x03,0x27,0x00,0x0a,0x00,0x30,0x04,0x03,0x28,0x00,0x0a,0x00};
static char OTM_8009_CMI_para079[] = {0x00,0xc7};
static char OTM_8009_CMI_para080[] = {0xcf,0x00};
static char OTM_8009_CMI_para081[] = {0x00,0xc0};
static char OTM_8009_CMI_para082[] = {0xcb,0x00,0x00,0x00,0x00,0x14,0x14,0x14,0x14,0x00,0x14,0x00,0x14,0x00,0x00,0x00};
static char OTM_8009_CMI_para083[] = {0x00,0xd0};
static char OTM_8009_CMI_para084[] = {0xcb,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x14,0x14,0x14,0x14,0x00,0x14};
static char OTM_8009_CMI_para085[] = {0x00,0xe0};
static char OTM_8009_CMI_para086[] = {0xcb,0x00,0x14,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
static char OTM_8009_CMI_para087[] = {0x00,0x80};
static char OTM_8009_CMI_para088[] = {0xcc,0x00,0x00,0x00,0x00,0x0c,0x0a,0x10,0x0e,0x00,0x02};
static char OTM_8009_CMI_para089[] = {0x00,0x90};
static char OTM_8009_CMI_para090[] = {0xcc,0x00,0x06,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0b};
static char OTM_8009_CMI_para091[] = {0x00,0xa0};
static char OTM_8009_CMI_para092[] = {0xcc,0x09,0x0f,0x0d,0x00,0x01,0x00,0x05,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
static char OTM_8009_CMI_para093[] = {0x00,0xb0};
static char OTM_8009_CMI_para094[] = {0xcc,0x00,0x00,0x00,0x00,0x0d,0x0f,0x09,0x0b,0x00,0x05};
static char OTM_8009_CMI_para095[] = {0x00,0xc0};
static char OTM_8009_CMI_para096[] = {0xcc,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0e};
static char OTM_8009_CMI_para097[] = {0x00,0xd0};
static char OTM_8009_CMI_para098[] = {0xcc,0x10,0x0a,0x0c,0x00,0x06,0x00,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
static char OTM_8009_CMI_para099[] = {0x00,0x00};
static char OTM_8009_CMI_para100[] = {0x26,0x01};
static char OTM_8009_CMI_para101[] = {0x00,0x00};
static char OTM_8009_CMI_para102[] = {0xe1,0x00,0x0e,0x13,0x0e,0x07,0x10,0x0d,0x0c,0x04,0x07,0x0d,0x08,0x0f,0x0f,0x08,0x03};
static char OTM_8009_CMI_para103[] = {0x00,0x00};
static char OTM_8009_CMI_para104[] = {0xe2,0x00,0x0e,0x13,0x0e,0x07,0x10,0x0d,0x0c,0x04,0x07,0x0d,0x08,0x0f,0x0f,0x08,0x03};

static struct dsi_cmd_desc OTM_8009_CMI_display_on_cmds[] = 
{
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM_8009_CMI_para001), OTM_8009_CMI_para001},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM_8009_CMI_para002), OTM_8009_CMI_para002},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM_8009_CMI_para003), OTM_8009_CMI_para003},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM_8009_CMI_para004), OTM_8009_CMI_para004},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM_8009_CMI_para005), OTM_8009_CMI_para005},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM_8009_CMI_para006), OTM_8009_CMI_para006},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM_8009_CMI_para007), OTM_8009_CMI_para007},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM_8009_CMI_para008), OTM_8009_CMI_para008},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM_8009_CMI_para009), OTM_8009_CMI_para009},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM_8009_CMI_para010), OTM_8009_CMI_para010},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM_8009_CMI_para011), OTM_8009_CMI_para011},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM_8009_CMI_para012), OTM_8009_CMI_para012},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM_8009_CMI_para013), OTM_8009_CMI_para013},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM_8009_CMI_para014), OTM_8009_CMI_para014},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM_8009_CMI_para015), OTM_8009_CMI_para015},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM_8009_CMI_para016), OTM_8009_CMI_para016},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM_8009_CMI_para017), OTM_8009_CMI_para017},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM_8009_CMI_para018), OTM_8009_CMI_para018},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM_8009_CMI_para019), OTM_8009_CMI_para019},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM_8009_CMI_para020), OTM_8009_CMI_para020},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM_8009_CMI_para021), OTM_8009_CMI_para021},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM_8009_CMI_para022), OTM_8009_CMI_para022},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM_8009_CMI_para023), OTM_8009_CMI_para023},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM_8009_CMI_para024), OTM_8009_CMI_para024},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM_8009_CMI_para025), OTM_8009_CMI_para025},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM_8009_CMI_para026), OTM_8009_CMI_para026},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM_8009_CMI_para027), OTM_8009_CMI_para027},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM_8009_CMI_para028), OTM_8009_CMI_para028},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM_8009_CMI_para029), OTM_8009_CMI_para029},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM_8009_CMI_para030), OTM_8009_CMI_para030},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM_8009_CMI_para031), OTM_8009_CMI_para031},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM_8009_CMI_para032), OTM_8009_CMI_para032},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM_8009_CMI_para033), OTM_8009_CMI_para033},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM_8009_CMI_para034), OTM_8009_CMI_para034},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM_8009_CMI_para035), OTM_8009_CMI_para035},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM_8009_CMI_para036), OTM_8009_CMI_para036},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM_8009_CMI_para037), OTM_8009_CMI_para037},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM_8009_CMI_para038), OTM_8009_CMI_para038},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM_8009_CMI_para039), OTM_8009_CMI_para039},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM_8009_CMI_para040), OTM_8009_CMI_para040},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM_8009_CMI_para041), OTM_8009_CMI_para041},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM_8009_CMI_para042), OTM_8009_CMI_para042},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM_8009_CMI_para043), OTM_8009_CMI_para043},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM_8009_CMI_para044), OTM_8009_CMI_para044},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM_8009_CMI_para045), OTM_8009_CMI_para045},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM_8009_CMI_para046), OTM_8009_CMI_para046},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM_8009_CMI_para047), OTM_8009_CMI_para047},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM_8009_CMI_para048), OTM_8009_CMI_para048},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM_8009_CMI_para049), OTM_8009_CMI_para049},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM_8009_CMI_para050), OTM_8009_CMI_para050},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM_8009_CMI_para051), OTM_8009_CMI_para051},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM_8009_CMI_para052), OTM_8009_CMI_para052},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM_8009_CMI_para053), OTM_8009_CMI_para053},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM_8009_CMI_para054), OTM_8009_CMI_para054},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM_8009_CMI_para055), OTM_8009_CMI_para055},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM_8009_CMI_para056), OTM_8009_CMI_para056},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM_8009_CMI_para057), OTM_8009_CMI_para057},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM_8009_CMI_para058), OTM_8009_CMI_para058},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM_8009_CMI_para059), OTM_8009_CMI_para059},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM_8009_CMI_para060), OTM_8009_CMI_para060},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM_8009_CMI_para061), OTM_8009_CMI_para061},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM_8009_CMI_para062), OTM_8009_CMI_para062},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM_8009_CMI_para063), OTM_8009_CMI_para063},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM_8009_CMI_para064), OTM_8009_CMI_para064},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM_8009_CMI_para065), OTM_8009_CMI_para065},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM_8009_CMI_para066), OTM_8009_CMI_para066},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM_8009_CMI_para067), OTM_8009_CMI_para067},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM_8009_CMI_para068), OTM_8009_CMI_para068},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM_8009_CMI_para069), OTM_8009_CMI_para069},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM_8009_CMI_para070), OTM_8009_CMI_para070},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM_8009_CMI_para071), OTM_8009_CMI_para071},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM_8009_CMI_para072), OTM_8009_CMI_para072},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM_8009_CMI_para073), OTM_8009_CMI_para073},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM_8009_CMI_para074), OTM_8009_CMI_para074},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM_8009_CMI_para075), OTM_8009_CMI_para075},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM_8009_CMI_para076), OTM_8009_CMI_para076},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM_8009_CMI_para077), OTM_8009_CMI_para077},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM_8009_CMI_para078), OTM_8009_CMI_para078},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM_8009_CMI_para079), OTM_8009_CMI_para079},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM_8009_CMI_para080), OTM_8009_CMI_para080},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM_8009_CMI_para081), OTM_8009_CMI_para081},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM_8009_CMI_para082), OTM_8009_CMI_para082},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM_8009_CMI_para083), OTM_8009_CMI_para083},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM_8009_CMI_para084), OTM_8009_CMI_para084},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM_8009_CMI_para085), OTM_8009_CMI_para085},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM_8009_CMI_para086), OTM_8009_CMI_para086},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM_8009_CMI_para087), OTM_8009_CMI_para087},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM_8009_CMI_para088), OTM_8009_CMI_para088},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM_8009_CMI_para089), OTM_8009_CMI_para089},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM_8009_CMI_para090), OTM_8009_CMI_para090},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM_8009_CMI_para091), OTM_8009_CMI_para091},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM_8009_CMI_para092), OTM_8009_CMI_para092},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM_8009_CMI_para093), OTM_8009_CMI_para093},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM_8009_CMI_para094), OTM_8009_CMI_para094},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM_8009_CMI_para095), OTM_8009_CMI_para095},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM_8009_CMI_para096), OTM_8009_CMI_para096},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM_8009_CMI_para097), OTM_8009_CMI_para097},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM_8009_CMI_para098), OTM_8009_CMI_para098},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM_8009_CMI_para099), OTM_8009_CMI_para099},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM_8009_CMI_para100), OTM_8009_CMI_para100},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM_8009_CMI_para101), OTM_8009_CMI_para101},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM_8009_CMI_para102), OTM_8009_CMI_para102},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM_8009_CMI_para103), OTM_8009_CMI_para103},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(OTM_8009_CMI_para104), OTM_8009_CMI_para104},

	{DTYPE_DCS_WRITE, 1, 0, 0, 120, sizeof(exit_sleep), exit_sleep},
	{DTYPE_DCS_WRITE, 1, 0, 0, 10, sizeof(display_on), display_on}
};

static char nt35512_tianma_para_01[]={0xF0,0x55,0xAA,0x52,0x08,0x01};
static char nt35512_tianma_para_02[]={0xB0,0x0A};
static char nt35512_tianma_para_03[]={0xB6,0x44};
static char nt35512_tianma_para_04[]={0xB1,0x0A};
static char nt35512_tianma_para_05[]={0xB7,0x34};
static char nt35512_tianma_para_06[]={0xB2,0x00};
static char nt35512_tianma_para_07[]={0xB8,0x34};
static char nt35512_tianma_para_08[]={0xB3,0x06};
static char nt35512_tianma_para_09[]={0xB9,0x24};
static char nt35512_tianma_para_10[]={0xB5,0x08};
static char nt35512_tianma_para_11[]={0xBA,0x14};
static char nt35512_tianma_para_12[]={0xBC,0x00,0x78,0x00};
static char nt35512_tianma_para_13[]={0xBD,0x00,0x78,0x00};
static char nt35512_tianma_para_14[]={0xBE,0x00,0x7a};
static char nt35512_tianma_para_15[]={0xD1,0x00,0x2C,0x00,0x82,0x00,0xB3,0x00,0xD2,0x00,0xE4,0x01,0x0B,0x01,0x2C,0x01,0x5B,0x01,0x7E,0x01,0xB8,0x01,0xE4,0x02,0x2B,0x02,0x63,0x02,0x65,0x02,0x97,0x02,0xCF,0x02,0xF3,0x03,0x1E,0x03,0x3E,0x03,0x63,0x03,0x77,0x03,0xA4,0x03,0xBF,0x03,0xCF,0x03,0xDD,0x03,0xFF};
static char nt35512_tianma_para_16[]={0xD2,0x00,0x2C,0x00,0x82,0x00,0xB3,0x00,0xD2,0x00,0xE4,0x01,0x0B,0x01,0x2C,0x01,0x5B,0x01,0x7E,0x01,0xB8,0x01,0xE4,0x02,0x2B,0x02,0x63,0x02,0x65,0x02,0x97,0x02,0xCF,0x02,0xF3,0x03,0x1E,0x03,0x3E,0x03,0x63,0x03,0x77,0x03,0xA4,0x03,0xBF,0x03,0xCF,0x03,0xDD,0x03,0xFF};
static char nt35512_tianma_para_17[]={0xD3,0x00,0x2C,0x00,0x82,0x00,0xB3,0x00,0xD2,0x00,0xE4,0x01,0x0B,0x01,0x2C,0x01,0x5B,0x01,0x7E,0x01,0xB8,0x01,0xE4,0x02,0x2B,0x02,0x63,0x02,0x65,0x02,0x97,0x02,0xCF,0x02,0xF3,0x03,0x1E,0x03,0x3E,0x03,0x63,0x03,0x77,0x03,0xA4,0x03,0xBF,0x03,0xCF,0x03,0xDD,0x03,0xFF};
static char nt35512_tianma_para_18[]={0xD4,0x00,0x2C,0x00,0x82,0x00,0xB3,0x00,0xD2,0x00,0xE4,0x01,0x0B,0x01,0x2C,0x01,0x5B,0x01,0x7E,0x01,0xB8,0x01,0xE4,0x02,0x2B,0x02,0x63,0x02,0x65,0x02,0x97,0x02,0xCF,0x02,0xF3,0x03,0x1E,0x03,0x3E,0x03,0x63,0x03,0x77,0x03,0xA4,0x03,0xBF,0x03,0xCF,0x03,0xDD,0x03,0xFF};
static char nt35512_tianma_para_19[]={0xD5,0x00,0x2C,0x00,0x82,0x00,0xB3,0x00,0xD2,0x00,0xE4,0x01,0x0B,0x01,0x2C,0x01,0x5B,0x01,0x7E,0x01,0xB8,0x01,0xE4,0x02,0x2B,0x02,0x63,0x02,0x65,0x02,0x97,0x02,0xCF,0x02,0xF3,0x03,0x1E,0x03,0x3E,0x03,0x63,0x03,0x77,0x03,0xA4,0x03,0xBF,0x03,0xCF,0x03,0xDD,0x03,0xFF};
static char nt35512_tianma_para_20[]={0xD6,0x00,0x2C,0x00,0x82,0x00,0xB3,0x00,0xD2,0x00,0xE4,0x01,0x0B,0x01,0x2C,0x01,0x5B,0x01,0x7E,0x01,0xB8,0x01,0xE4,0x02,0x2B,0x02,0x63,0x02,0x65,0x02,0x97,0x02,0xCF,0x02,0xF3,0x03,0x1E,0x03,0x3E,0x03,0x63,0x03,0x77,0x03,0xA4,0x03,0xBF,0x03,0xCF,0x03,0xDD,0x03,0xFF};
static char nt35512_tianma_para_21[]={0xF0,0x55,0xAA,0x52,0x08,0x00};
static char nt35512_tianma_para_22[]={0xB1,0xFC,0x00,0x01};
static char nt35512_tianma_para_23[]={0xB5,0x50};
static char nt35512_tianma_para_24[]={0xB6,0x03}; 
static char nt35512_tianma_para_25[]={0xB7,0x80,0x80};
static char nt35512_tianma_para_26[]={0xB8,0x01,0x02,0x02,0x02};  
static char nt35512_tianma_para_27[]={0xBC,0x03};
static char nt35512_tianma_para_28[]={0xC9,0x00,0x02,0x50,0x50,0x50};
static char nt35512_tianma_para_29[]={0xE2,0x07,0xFF};
static char nt35512_tianma_para_30[]={0xE4,0x87,0x78,0x02,0x20};
static char nt35512_tianma_para_31[]={0x21,0x00};

static struct dsi_cmd_desc nt35512_tianma_display_on_cmds[] = {

	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(nt35512_tianma_para_01), nt35512_tianma_para_01}, 
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(nt35512_tianma_para_02), nt35512_tianma_para_02}, 
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(nt35512_tianma_para_03), nt35512_tianma_para_03}, 
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(nt35512_tianma_para_04), nt35512_tianma_para_04}, 
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(nt35512_tianma_para_05), nt35512_tianma_para_05}, 
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(nt35512_tianma_para_06), nt35512_tianma_para_06}, 
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(nt35512_tianma_para_07), nt35512_tianma_para_07}, 
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(nt35512_tianma_para_08), nt35512_tianma_para_08}, 
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(nt35512_tianma_para_09), nt35512_tianma_para_09}, 
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(nt35512_tianma_para_10), nt35512_tianma_para_10}, 
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(nt35512_tianma_para_11), nt35512_tianma_para_11}, 
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(nt35512_tianma_para_12), nt35512_tianma_para_12}, 
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(nt35512_tianma_para_13), nt35512_tianma_para_13}, 
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(nt35512_tianma_para_14), nt35512_tianma_para_14}, 
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(nt35512_tianma_para_15), nt35512_tianma_para_15}, 
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(nt35512_tianma_para_16), nt35512_tianma_para_16}, 
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(nt35512_tianma_para_17), nt35512_tianma_para_17}, 
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(nt35512_tianma_para_18), nt35512_tianma_para_18}, 
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(nt35512_tianma_para_19), nt35512_tianma_para_19}, 
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(nt35512_tianma_para_20), nt35512_tianma_para_20}, 
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(nt35512_tianma_para_21), nt35512_tianma_para_21}, 
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(nt35512_tianma_para_22), nt35512_tianma_para_22}, 
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(nt35512_tianma_para_23), nt35512_tianma_para_23}, 
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(nt35512_tianma_para_24), nt35512_tianma_para_24}, 
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(nt35512_tianma_para_25), nt35512_tianma_para_25}, 
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(nt35512_tianma_para_26), nt35512_tianma_para_26}, 
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(nt35512_tianma_para_27), nt35512_tianma_para_27}, 
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(nt35512_tianma_para_28), nt35512_tianma_para_28}, 
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(nt35512_tianma_para_29), nt35512_tianma_para_29}, 
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(nt35512_tianma_para_30), nt35512_tianma_para_30}, 
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(nt35512_tianma_para_31), nt35512_tianma_para_31}, 
	
	{DTYPE_DCS_WRITE, 1, 0, 0, 120, sizeof(exit_sleep), exit_sleep},
	{DTYPE_DCS_WRITE, 1, 0, 0, 10, sizeof(display_on), display_on}

};

void myudelay(unsigned int usec)
{
udelay(usec);
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

static uint32 mipi_get_commic_panleid_cmi(struct msm_fb_data_type *mfd,struct dsi_cmd_desc *para,uint32 len,int mode)
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
	printk("lizhiye, debug mipi_get_commic_panleid_cmi panelid is %x\n",panelid);
	
	return panelid;
}

static uint32 mipi_get_CMI_panleid(struct msm_fb_data_type *mfd)
{
	uint32 panleid =  mipi_get_commic_panleid_cmi(mfd,&cmi_icid_rd_cmd,4,0);
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
	uint32 icid_cmi = 0;
	

	 struct mipi_manufacture_ic mipi_manufacture_icid[] = {	 	
	 	
		{hx8363_setpassword_cmd,ARRAY_SIZE(hx8363_setpassword_cmd),&hx8363_icid_rd_cmd,3,1},
		{nt3511_setpassword_cmd,ARRAY_SIZE(nt3511_setpassword_cmd),&nt3511_icid_rd_cmd,3,0},
		{hx8369b_setpassword_cmd,ARRAY_SIZE(hx8369b_setpassword_cmd),&hx8369b_icid_rd_cmd,3,1},
		{hx8369_setpassword_cmd,ARRAY_SIZE(hx8369_setpassword_cmd),&hx8369_icid_rd_cmd,3,1},
		{r61408_setpassword_cmd,ARRAY_SIZE(r61408_setpassword_cmd),&r61408_icid_rd_cmd,4,1},
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

		switch(icid & 0xffffff){
			case 0x1055:
						return NOVATEK_35510;
			case 0x6383ff:
						return HIMAX_8363;
						
			case 0x6983ff:
						return HIMAX_8369;
			case 0x142201:
						return RENESAS_R61408;
			case 0x5a6983:
						return HIMAX_8369B;
			default:
						break;			
		}

	}
	icid_cmi = mipi_get_CMI_panleid(mfd);
	if((icid_cmi & 0xffff) == 0x0980)
		return CMI_8001;
	else
	return 0;
}

static uint32 mipi_get_commic_panleid(struct msm_fb_data_type *mfd,struct dsi_cmd_desc *para,uint32 len,int mode)
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
	panelid = *(uint32 *)(lead_rx_buf.data);
	printk("debug read panelid is %x\n",panelid & 0xffffffff);
	return panelid;
}

static uint32 mipi_get_himax8369_panleid(struct msm_fb_data_type *mfd)
{

	uint32 panleid;
	panleid =  mipi_get_commic_panleid(mfd,&hx8369_panleid_rd_cmd,1,1);
	switch((panleid>>8) & 0xff){
		case HIMAX8369_TIANMA_TN_ID:
				return (u32)LCD_PANEL_4P0_HIMAX8369_TIANMA_TN;
		case HIMAX8369_TIANMA_IPS_ID:
				return (u32)LCD_PANEL_4P0_HIMAX8369_TIANMA_IPS;
		case HIMAX8369_LEAD_ID:
				return (u32)LCD_PANEL_4P0_HIMAX8369_LEAD;
		case HIMAX8369_LEAD_HANNSTAR_ID:
				return (u32)LCD_PANEL_4P0_HIMAX8369_LEAD_HANNSTAR;
		default:
				return (u32)LCD_PANEL_NOPANEL;
	}
}

static uint32 mipi_get_himax8369B_panleid(struct msm_fb_data_type *mfd)
{

	uint32 panleid;
	panleid =  mipi_get_commic_panleid(mfd,&hx8369b_panleid_rd_cmd,1,1);
	switch((panleid>>8) & 0xff){
		case 0x69:
				return (u32)LCD_PANEL_4P0_HX8369B_TM04YVHP12;
		default:
				return (u32)LCD_PANEL_NOPANEL;
	}
}


static uint32 mipi_get_nt35510_panleid(struct msm_fb_data_type *mfd)
{

	uint32 panleid =  mipi_get_commic_panleid(mfd,&nt3511_panleid_rd_cmd,1,0);
	switch(panleid&0xff){
		case NT35510_YUSHUN_ID:
				return  (u32)LCD_PANEL_4P0_NT35510_HYDIS_YUSHUN	;
		case NT35510_LEAD_ID:
				return (u32)LCD_PANEL_4P0_NT35510_LEAD;
		case NT35510_BOE_ID:
				return (u32)LCD_PANEL_4P0_NT35510_BOE_BOE;
		default:
				return (u32)LCD_PANEL_NOPANEL;
	}
}

static uint32 mipi_get_himax8363_panleid(struct msm_fb_data_type *mfd)
{

	uint32 panleid =  mipi_get_commic_panleid(mfd, &hx8363_panleid_rd_cmd,1,1);
	
	switch((panleid >> 8) & 0xff)
	{
		case HIMAX8369_YUSHUN_IVO_ID:	//85	--
			return (u32)LCD_PANEL_4P0_HX8363_IVO_YUSHUN;
		default:
			return (u32)LCD_PANEL_4P0_HX8363_CMI_YASSY;		//00	--
	}
}

static uint32 mipi_get_icpanleid(struct msm_fb_data_type *mfd )
{
	int icid = 0;

	lcd_panle_reset();

	icid = mipi_get_manufacture_icid(mfd);


	switch(icid){
		case HIMAX_8363:					
           				 LcdPanleID = mipi_get_himax8363_panleid(mfd);
					break;
		case HIMAX_8369:
					LcdPanleID = mipi_get_himax8369_panleid(mfd);
					break;
		case NOVATEK_35510:
					LcdPanleID = mipi_get_nt35510_panleid(mfd);
					break;
		case RENESAS_R61408:
					LcdPanleID = LCD_PANEL_4P0_R61408_TRULY_LG;
			break;
		case HIMAX_8369B:
					LcdPanleID = mipi_get_himax8369B_panleid(mfd);
					break;
		case CMI_8001:
					LcdPanleID = LCD_PANEL_4P0_OTM_8009_CMI;
					break;
		default:
					LcdPanleID = (u32)LCD_PANEL_NOPANEL;
					printk("warnning cann't indentify the chip\n");
					break;
	}
		
	return LcdPanleID;
}
#if 0
#ifdef CONFIG_ZTE_PLATFORM
static u32 __init get_lcdpanleid_from_bootloader(void)
{
	smem_global*	msm_lcd_global = (smem_global*) ioremap(SMEM_LOG_GLOBAL_BASE, sizeof(smem_global));
	
	printk("debug chip id 0x%x\n",msm_lcd_global->lcd_id);
	
	if (((msm_lcd_global->lcd_id) & 0xffff0000) == 0x09830000) 
	{
		
		switch(msm_lcd_global->lcd_id & 0x0000ffff)
		{	
			case 0x0001:
				return (u32)LCD_PANEL_4P0_HX8363_CMI_YASSY;
			case 0x0002:
				return (u32)LCD_PANEL_4P0_HIMAX8369_LEAD;
			case 0x0003:
				return (u32)LCD_PANEL_4P0_HIMAX8369_LEAD_HANNSTAR;
			case 0x0004:
				return (u32)LCD_PANEL_4P0_HIMAX8369_TIANMA_TN;
			case 0x0005:
				return (u32)LCD_PANEL_4P0_HIMAX8369_TIANMA_IPS;
			case 0x0006:
				return (u32)LCD_PANEL_4P0_NT35510_LEAD;
			case 0x0007:
				return (u32)LCD_PANEL_4P0_NT35510_HYDIS_YUSHUN;
			case 0x0008:
				return (u32)LCD_PANEL_4P0_R61408_TRULY_LG;	
			default:
				break;
		}		
	}
	return (u32)LCD_PANEL_NOPANEL;
}
#endif
#endif
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
	gpio_direction_output(GPIO_LCD_RESET,0);
	msleep(5);
	gpio_direction_output(GPIO_LCD_RESET,1);
	msleep(10);
//	gpio_direction_output(121,0);
	return 0;
}

//added by zte_gequn091966,20110428
static void select_1wire_mode(void)
{
return;
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
return;
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
return;
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
	uint32 back_level = 0;
	unsigned long flags;
#ifdef CONFIG_MACH_APOLLO
	uint32 min_blk_level =20,half_blk_level=102;
	uint32 min_wled_level =11,half_wled_level=35;
#endif
	//printk("[ZYF] lcdc_set_bl level=%d, %d\n", current_lel , mfd->panel_power_on);
	if(!mfd->panel_power_on)
		return;
	
	if(current_lel == 0)
		back_level = 0;
#ifdef CONFIG_MACH_APOLLO
	else if((current_lel<half_blk_level)&&(current_lel >= min_blk_level))
		back_level = min_wled_level+( half_wled_level-min_wled_level) *(current_lel -min_blk_level)/(half_blk_level-min_blk_level) ;
#endif
	else
		back_level = (current_lel *current_lel *current_lel/100000 + 4*current_lel*current_lel/10000 + 1889*current_lel/10000 + 28288/10000);

	printk("\n[ZYF] lcdc_set_bl level=%d -> wled level=%d", current_lel , back_level);
	
	if(back_level <= 0)
		back_level = 0;
	if(back_level >= 255)
		back_level = 255;
	
	if (wled_trigger_initialized) {
		led_trigger_event(bkl_led_trigger, back_level);
		return;
	 } 
	    	return;

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
		
    	if(current_lel > 32)
    	{
        	current_lel = 32;
    	}
    
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

	printk("lizhiye, mipi_lcd_on mipi init start LcdPanleID=%d\n",LcdPanleID);
	mipi_set_tx_power_mode(1);
	switch(LcdPanleID){
		case (u32)LCD_PANEL_4P0_HX8363_CMI_YASSY:
				mipi_dsi_cmds_tx(&lead_tx_buf, hx8363_yassy_display_on_cmds,ARRAY_SIZE(hx8363_yassy_display_on_cmds));
				printk("HIMAX8363_YASS init ok !!\n");
				break;
		case (u32)LCD_PANEL_4P0_HX8363_IVO_YUSHUN:
				mipi_dsi_cmds_tx(&lead_tx_buf, hx8363_yushun_display_on_cmds,ARRAY_SIZE(hx8363_yushun_display_on_cmds));
				printk("HIMAX8363_yushun init ok !!\n");
				break;
				
		case (u32)LCD_PANEL_4P0_HIMAX8369_LEAD:
				mipi_dsi_cmds_tx(&lead_tx_buf, hx8369_lead_display_on_cmds,ARRAY_SIZE(hx8369_lead_display_on_cmds));
				printk("HIMAX8369_LEAD init ok !!\n");
				break;
				
		case (u32)LCD_PANEL_4P0_HIMAX8369_LEAD_HANNSTAR:
				mipi_dsi_cmds_tx(&lead_tx_buf, hx8369_lead_hannstar_display_on_cmds,ARRAY_SIZE(hx8369_lead_hannstar_display_on_cmds));
				printk("HIMAX8369_LEAD_HANNSTAR init ok !!\n");
				break;
				
		case (u32)LCD_PANEL_4P0_HIMAX8369_TIANMA_TN:
				mipi_dsi_cmds_tx(&lead_tx_buf, hx8369_tianma_tn_display_on_cmds,ARRAY_SIZE(hx8369_tianma_tn_display_on_cmds));
				printk("HIMAX8369_TIANMA_TN init ok !!\n");
				break;
				
		case (u32)LCD_PANEL_4P0_HIMAX8369_TIANMA_IPS:
				mipi_dsi_cmds_tx(&lead_tx_buf, hx8369_tianma_ips_display_on_cmds,ARRAY_SIZE(hx8369_tianma_ips_display_on_cmds));
				printk("HIMAX8369_TIANMA_IPS init ok !!\n");
				break;
				
		case (u32)LCD_PANEL_4P0_NT35510_LEAD:
				mipi_dsi_cmds_tx(&lead_tx_buf, nt35510_lead_display_on_cmds,ARRAY_SIZE(nt35510_lead_display_on_cmds));
				printk("NT35510_LEAD init ok !!\n");
				break;
				
		case (u32)LCD_PANEL_4P0_NT35510_HYDIS_YUSHUN:
				mipi_dsi_cmds_tx(&lead_tx_buf, nt3511_yushun_display_on_cmds,ARRAY_SIZE(nt3511_yushun_display_on_cmds));
				printk("NT35510_HYDIS_YUSHUN init ok !!\n");
				break;
		case (u32)LCD_PANEL_4P0_R61408_TRULY_LG:
				mipi_dsi_cmds_tx( &lead_tx_buf, r61408_truly_lg_display_on_cmds,ARRAY_SIZE(r61408_truly_lg_display_on_cmds));
				printk("R61408 TRULY LG  init ok !!\n");
			break;
		case (u32)LCD_PANEL_4P0_HX8369B_TM04YVHP12:
				mipi_dsi_cmds_tx( &lead_tx_buf, HX8369B_TM04YVHP12_display_on_cmds,ARRAY_SIZE(HX8369B_TM04YVHP12_display_on_cmds));
				printk("lizhiye, LCD_PANEL_4P0_HX8369B_TM04YVHP12  init ok !!\n");
			break;
		case (u32)LCD_PANEL_4P0_NT35510_BOE_BOE:
				mipi_dsi_cmds_tx( &lead_tx_buf, boe_nt35510_display_on_cmds,ARRAY_SIZE(boe_nt35510_display_on_cmds));
				printk(" boe boe net5510  init ok !!\n");
			break;	
		case (u32)LCD_PANEL_4P0_OTM_8009_CMI:
				mipi_dsi_cmds_tx( &lead_tx_buf, OTM_8009_CMI_display_on_cmds,ARRAY_SIZE(OTM_8009_CMI_display_on_cmds));
				printk("lizhiye, OTM_8009_CMI_display_on_cmds init ok !!\n");
				break;	
		case (u32)LCD_PANEL_4P0_NT35512_TM:
				mipi_dsi_cmds_tx( &lead_tx_buf, nt35512_tianma_display_on_cmds,ARRAY_SIZE(nt35512_tianma_display_on_cmds));
				printk("nt35512_tianma_display_on_cmds init ok !!\n");
				break;	
		default:
				printk("can't get jfldjfldicpanelid value\n");
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

	pdev = platform_device_alloc("mipi_lead", (panel << 8)|channel);
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
	struct msm_panel_info   *pinfo =&( ((struct msm_fb_panel_data  *)(pdev->dev.platform_data))->panel_info);
	
	if (pdev->id == 0) return 0;
	
	mipi_dsi_buf_alloc(&lead_tx_buf, DSI_BUF_SIZE);
	mipi_dsi_buf_alloc(&lead_rx_buf, DSI_BUF_SIZE);
	
	if (LcdPanleID ==LCD_PANEL_4P0_R61408_TRULY_LG)//this panel is different from others
	{
		pinfo->lcdc.h_back_porch = 50;
		pinfo->lcdc.h_front_porch = 160;	
		pinfo->lcdc.h_pulse_width = 50;
		pinfo->lcdc.v_back_porch = 10;	
		pinfo->lcdc.v_front_porch = 12;
		pinfo->lcdc.v_pulse_width = 2;
		pinfo->clk_rate = 402380000;
		pinfo->mipi.frame_rate = 60;
	}
	else if(LcdPanleID ==LCD_PANEL_4P0_NT35512_TM)
	{
		pinfo->lcdc.h_back_porch = 150;
		pinfo->lcdc.h_front_porch = 100;	
		pinfo->mipi.frame_rate = 60;
	}
	else if(LcdPanleID ==LCD_PANEL_4P0_NT35510_LEAD)
	{
		pinfo->lcdc.h_back_porch = 120;
		pinfo->lcdc.h_front_porch = 120;	
		pinfo->mipi.frame_rate = 60;
	}
#if 0
	if (LcdPanleID ==LCD_PANEL_4P0_R61408_TRULY_LG)//this panel is different from others
	{
		pinfo->lcdc.h_back_porch = 80;
		pinfo->lcdc.h_front_porch = 180;	
		pinfo->lcdc.v_back_porch = 12;	
	}
#endif
	msm_fb_add_device(pdev);

	return 0;
}

static struct platform_driver this_driver = {
	.probe  = mipi_lead_lcd_probe,
	.driver = {
		.name   = "mipi_lead",
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
