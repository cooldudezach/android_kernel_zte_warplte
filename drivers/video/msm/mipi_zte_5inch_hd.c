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
#include "mipi_novatek.h"
#include "mdp4.h"
#include <mach/gpio.h>
#define USE_CABC_PANEL
#define N9810_USE_CMD_MODE
#ifdef USE_CABC_PANEL

static char cabc_cmd_C1_51[] = {0x51,0xff};
static char cabc_cmd_C1_53[] = {0x53,0x2c};
static char cabc_cmd_C1_55[] = {0x55,0x01};
 
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
	cmdreq.flags = CMD_REQ_COMMIT|CMD_CLK_CTRL;
	cmdreq.rlen = 0;
	cmdreq.cb = NULL;
	mipi_dsi_cmdlist_put(&cmdreq);	
}
#endif


static struct dsi_buf novatek_tx_buf;
static struct dsi_buf novatek_rx_buf;
extern void mipi_set_tx_power_mode(int mode);

static int mipi_novatek_lcd_init(void);
static void lcd_panle_reset(void);



static char enter_sleep[2] = {0x10, 0x00}; /* DTYPE_DCS_WRITE */
static char exit_sleep[2] = {0x11, 0x00}; /* DTYPE_DCS_WRITE */
static char display_off[2] = {0x28, 0x00}; /* DTYPE_DCS_WRITE */
static char display_on[2] = {0x29, 0x00}; /* DTYPE_DCS_WRITE */



static char cmi_nt35590_para_add_01[]={0xFF,0xee};
static char cmi_nt35590_para_add_02[]={0x26,0x08};
static char cmi_nt35590_para_add_03[]={0x26,0x00};
static char cmi_nt35590_para_add_04[]={0xFF,0x00};
static struct dsi_cmd_desc cmi_nt35590_display_on_add_cmds[] = {

	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para_add_01), cmi_nt35590_para_add_01},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 5, sizeof(cmi_nt35590_para_add_02), cmi_nt35590_para_add_02},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para_add_03), cmi_nt35590_para_add_03},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 10, sizeof(cmi_nt35590_para_add_04), cmi_nt35590_para_add_04},
};

static char cmi_nt35590_para001[]={0xFF,0x00};
static char cmi_nt35590_para002[]={0xBA,0x03};
#ifdef N9810_USE_CMD_MODE
static char cmi_nt35590_para003[]={0xC2,0x08};
#else
static char cmi_nt35590_para003[]={0xC2,0x03};
#endif
static char cmi_nt35590_para_color[]={0x55,0xb0};//color_enhancement  0x80 low;0x90 middle;0xa0 high
static char cmi_nt35590_para0031[]={0x3b,0x03,0x07,0x04,0x46,0x3c};//mode,(vbp+v_width)/2,vfp/2,(hbp+h_width),hfp
static char cmi_nt35590_para004[]={0xFF,0x01};
static char cmi_nt35590_para005[]={0x00,0x3A};
static char cmi_nt35590_para006[]={0x01,0x33};
static char cmi_nt35590_para007[]={0x02,0x53};
static char cmi_nt35590_para008[]={0x09,0x85};
static char cmi_nt35590_para009[]={0x0e,0x25};
static char cmi_nt35590_para010[]={0x0f,0x0a};
static char cmi_nt35590_para011[]={0x0b,0x97};
static char cmi_nt35590_para012[]={0x0c,0x97};
static char cmi_nt35590_para013[]={0x11,0x8a};
static char cmi_nt35590_para014[]={0x36,0x7b};
static char cmi_nt35590_para015[]={0x71,0x2c};
static char cmi_nt35590_para016[]={0xff,0x05};
static char cmi_nt35590_para017[]={0x01,0x00};
static char cmi_nt35590_para018[]={0x02,0x8D};
static char cmi_nt35590_para019[]={0x03,0x8D};
static char cmi_nt35590_para020[]={0x04,0x8D};
static char cmi_nt35590_para021[]={0x05,0x30};
static char cmi_nt35590_para022[]={0x06,0x33};
static char cmi_nt35590_para023[]={0x07,0x77};
static char cmi_nt35590_para024[]={0x08,0x00};
static char cmi_nt35590_para025[]={0x09,0x00};
static char cmi_nt35590_para026[]={0x0A,0x00};
static char cmi_nt35590_para027[]={0x0B,0x80};
static char cmi_nt35590_para028[]={0x0C,0xC8};
static char cmi_nt35590_para029[]={0x0D,0x00};
static char cmi_nt35590_para030[]={0x0E,0x1B};
static char cmi_nt35590_para031[]={0x0F,0x07};
static char cmi_nt35590_para032[]={0x10,0x57};
static char cmi_nt35590_para033[]={0x11,0x00};
static char cmi_nt35590_para034[]={0x12,0x00};
static char cmi_nt35590_para035[]={0x13,0x1E};
static char cmi_nt35590_para036[]={0x14,0x00};
static char cmi_nt35590_para037[]={0x15,0x1A};
static char cmi_nt35590_para038[]={0x16,0x05};
static char cmi_nt35590_para039[]={0x17,0x00};
static char cmi_nt35590_para040[]={0x18,0x1E};
static char cmi_nt35590_para041[]={0x19,0xFF};
static char cmi_nt35590_para042[]={0x1A,0x00};
static char cmi_nt35590_para043[]={0x1B,0xFC};
static char cmi_nt35590_para044[]={0x1C,0x80};
static char cmi_nt35590_para045[]={0x1D,0x00};
static char cmi_nt35590_para046[]={0x1E,0x10};
static char cmi_nt35590_para047[]={0x1F,0x77};
static char cmi_nt35590_para048[]={0x20,0x00};
static char cmi_nt35590_para049[]={0x21,0x00};
static char cmi_nt35590_para050[]={0x22,0x55};
static char cmi_nt35590_para051[]={0x23,0x0D};
static char cmi_nt35590_para052[]={0x31,0xA0};
static char cmi_nt35590_para053[]={0x32,0x00};
static char cmi_nt35590_para054[]={0x33,0xB8};
static char cmi_nt35590_para055[]={0x34,0xBB};
static char cmi_nt35590_para056[]={0x35,0x11};
static char cmi_nt35590_para057[]={0x36,0x01};
static char cmi_nt35590_para058[]={0x37,0x0B};
static char cmi_nt35590_para059[]={0x38,0x01};
static char cmi_nt35590_para060[]={0x39,0x0B};
static char cmi_nt35590_para061[]={0x44,0x08};
static char cmi_nt35590_para062[]={0x45,0x80};
static char cmi_nt35590_para063[]={0x46,0xCC};
static char cmi_nt35590_para064[]={0x47,0x04};
static char cmi_nt35590_para065[]={0x48,0x00};
static char cmi_nt35590_para066[]={0x49,0x00};
static char cmi_nt35590_para067[]={0x4A,0x01};
static char cmi_nt35590_para068[]={0x6C,0x03};
static char cmi_nt35590_para069[]={0x6D,0x03};
static char cmi_nt35590_para070[]={0x6E,0x2F};
static char cmi_nt35590_para071[]={0x43,0x00};
static char cmi_nt35590_para072[]={0x4B,0x23};
static char cmi_nt35590_para073[]={0x4C,0x01};
static char cmi_nt35590_para074[]={0x50,0x23};
static char cmi_nt35590_para075[]={0x51,0x01};
static char cmi_nt35590_para076[]={0x58,0x23};
static char cmi_nt35590_para077[]={0x59,0x01};
static char cmi_nt35590_para078[]={0x5D,0x23};
static char cmi_nt35590_para079[]={0x5E,0x01};
static char cmi_nt35590_para080[]={0x62,0x23};
static char cmi_nt35590_para081[]={0x63,0x01};
static char cmi_nt35590_para082[]={0x67,0x23};
static char cmi_nt35590_para083[]={0x68,0x01};
static char cmi_nt35590_para084[]={0x89,0x00};
static char cmi_nt35590_para085[]={0x8D,0x01};
static char cmi_nt35590_para086[]={0x8E,0x64};
static char cmi_nt35590_para087[]={0x8F,0x20};
static char cmi_nt35590_para088[]={0x97,0x8E};
static char cmi_nt35590_para089[]={0x82,0x8C};
static char cmi_nt35590_para090[]={0x83,0x02};
static char cmi_nt35590_para091[]={0xBB,0x0A};
static char cmi_nt35590_para092[]={0xBC,0x0A};
static char cmi_nt35590_para093[]={0x24,0x25};
static char cmi_nt35590_para094[]={0x25,0x55};
static char cmi_nt35590_para095[]={0x26,0x05};
static char cmi_nt35590_para096[]={0x27,0x23};
static char cmi_nt35590_para097[]={0x28,0x01};
static char cmi_nt35590_para098[]={0x29,0x31};
static char cmi_nt35590_para099[]={0x2A,0x5D};
static char cmi_nt35590_para100[]={0x2B,0x01};
static char cmi_nt35590_para101[]={0x2F,0x00};
static char cmi_nt35590_para102[]={0x30,0x10};
static char cmi_nt35590_para103[]={0xA7,0x12};
static char cmi_nt35590_para104[]={0x2D,0x03};
static char cmi_nt35590_para105[]={0xFF,0x01};
static char cmi_nt35590_para106[]={0x75,0x00};
static char cmi_nt35590_para107[]={0x76,0x42};
static char cmi_nt35590_para108[]={0x77,0x00};
static char cmi_nt35590_para109[]={0x78,0x56};
static char cmi_nt35590_para110[]={0x79,0x00};
static char cmi_nt35590_para111[]={0x7A,0x79};
static char cmi_nt35590_para112[]={0x7B,0x00};
static char cmi_nt35590_para113[]={0x7C,0x97};
static char cmi_nt35590_para114[]={0x7D,0x00};
static char cmi_nt35590_para115[]={0x7E,0xb1};
static char cmi_nt35590_para116[]={0x7F,0x00};
static char cmi_nt35590_para117[]={0x80,0xc8};
static char cmi_nt35590_para118[]={0x81,0x00};
static char cmi_nt35590_para119[]={0x82,0xdb};
static char cmi_nt35590_para120[]={0x83,0x00};
static char cmi_nt35590_para121[]={0x84,0xec};
static char cmi_nt35590_para122[]={0x85,0x00};
static char cmi_nt35590_para123[]={0x86,0xfb};
static char cmi_nt35590_para124[]={0x87,0x01};
static char cmi_nt35590_para125[]={0x88,0x26};
static char cmi_nt35590_para126[]={0x89,0x01};
static char cmi_nt35590_para127[]={0x8A,0x49};
static char cmi_nt35590_para128[]={0x8B,0x01};
static char cmi_nt35590_para129[]={0x8C,0x86};
static char cmi_nt35590_para130[]={0x8D,0x01};
static char cmi_nt35590_para131[]={0x8E,0xb3};
static char cmi_nt35590_para132[]={0x8F,0x01};
static char cmi_nt35590_para133[]={0x90,0xfc};
static char cmi_nt35590_para134[]={0x91,0x02};
static char cmi_nt35590_para135[]={0x92,0x37};
static char cmi_nt35590_para136[]={0x93,0x02};
static char cmi_nt35590_para137[]={0x94,0x39};
static char cmi_nt35590_para138[]={0x95,0x02};
static char cmi_nt35590_para139[]={0x96,0x6f};
static char cmi_nt35590_para140[]={0x97,0x02};
static char cmi_nt35590_para141[]={0x98,0xaa};
static char cmi_nt35590_para142[]={0x99,0x02};
static char cmi_nt35590_para143[]={0x9A,0xc9};
static char cmi_nt35590_para144[]={0x9B,0x02};
static char cmi_nt35590_para145[]={0x9C,0xfc};
static char cmi_nt35590_para146[]={0x9D,0x03};
static char cmi_nt35590_para147[]={0x9E,0x20};
static char cmi_nt35590_para148[]={0x9F,0x03};
static char cmi_nt35590_para149[]={0xA0,0x52};
static char cmi_nt35590_para150[]={0xA2,0x03};
static char cmi_nt35590_para151[]={0xA3,0x62};
static char cmi_nt35590_para152[]={0xA4,0x03};
static char cmi_nt35590_para153[]={0xA5,0x75};
static char cmi_nt35590_para154[]={0xA6,0x03};
static char cmi_nt35590_para155[]={0xA7,0x8a};
static char cmi_nt35590_para156[]={0xA9,0x03};
static char cmi_nt35590_para157[]={0xAA,0xa1};
static char cmi_nt35590_para158[]={0xAB,0x03};
static char cmi_nt35590_para159[]={0xAC,0xb5};
static char cmi_nt35590_para160[]={0xAD,0x03};
static char cmi_nt35590_para161[]={0xAE,0xc6};
static char cmi_nt35590_para162[]={0xAF,0x03};
static char cmi_nt35590_para163[]={0xB0,0xcf};
static char cmi_nt35590_para164[]={0xB1,0x03};
static char cmi_nt35590_para165[]={0xB2,0xd1};
static char cmi_nt35590_para166[]={0xB3,0x00};
static char cmi_nt35590_para167[]={0xB4,0x42};
static char cmi_nt35590_para168[]={0xB5,0x00};
static char cmi_nt35590_para169[]={0xB6,0x56};
static char cmi_nt35590_para170[]={0xB7,0x00};
static char cmi_nt35590_para171[]={0xB8,0x79};
static char cmi_nt35590_para172[]={0xB9,0x00};
static char cmi_nt35590_para173[]={0xBA,0x97};
static char cmi_nt35590_para174[]={0xBB,0x00};
static char cmi_nt35590_para175[]={0xBC,0xB1};
static char cmi_nt35590_para176[]={0xBD,0x00};
static char cmi_nt35590_para177[]={0xBE,0xC8};
static char cmi_nt35590_para178[]={0xBF,0x00};
static char cmi_nt35590_para179[]={0xC0,0xDB};
static char cmi_nt35590_para180[]={0xC1,0x00};
static char cmi_nt35590_para181[]={0xC2,0xEC};
static char cmi_nt35590_para182[]={0xC3,0x00};
static char cmi_nt35590_para183[]={0xC4,0xFB};
static char cmi_nt35590_para184[]={0xC5,0x01};
static char cmi_nt35590_para185[]={0xC6,0x26};
static char cmi_nt35590_para186[]={0xC7,0x01};
static char cmi_nt35590_para187[]={0xC8,0x49};
static char cmi_nt35590_para188[]={0xC9,0x01};
static char cmi_nt35590_para189[]={0xCA,0x86};
static char cmi_nt35590_para190[]={0xCB,0x01};
static char cmi_nt35590_para191[]={0xCC,0xB3};
static char cmi_nt35590_para192[]={0xCD,0x01};
static char cmi_nt35590_para193[]={0xCE,0xFC};
static char cmi_nt35590_para194[]={0xCF,0x02};
static char cmi_nt35590_para195[]={0xD0,0x37};
static char cmi_nt35590_para196[]={0xD1,0x02};
static char cmi_nt35590_para197[]={0xD2,0x39};
static char cmi_nt35590_para198[]={0xD3,0x02};
static char cmi_nt35590_para199[]={0xD4,0x6F};
static char cmi_nt35590_para200[]={0xD5,0x02};
static char cmi_nt35590_para201[]={0xD6,0xAA};
static char cmi_nt35590_para202[]={0xD7,0x02};
static char cmi_nt35590_para203[]={0xD8,0xC9};
static char cmi_nt35590_para204[]={0xD9,0x02};
static char cmi_nt35590_para205[]={0xDA,0xFC};
static char cmi_nt35590_para206[]={0xDB,0x03};
static char cmi_nt35590_para207[]={0xDC,0x20};
static char cmi_nt35590_para208[]={0xDD,0x03};
static char cmi_nt35590_para209[]={0xDE,0x52};
static char cmi_nt35590_para210[]={0xDF,0x03};
static char cmi_nt35590_para211[]={0xE0,0x62};
static char cmi_nt35590_para212[]={0xE1,0x03};
static char cmi_nt35590_para213[]={0xE2,0x75};
static char cmi_nt35590_para214[]={0xE3,0x03};
static char cmi_nt35590_para215[]={0xE4,0x8A};
static char cmi_nt35590_para216[]={0xE5,0x03};
static char cmi_nt35590_para217[]={0xE6,0xA1};
static char cmi_nt35590_para218[]={0xE7,0x03};
static char cmi_nt35590_para219[]={0xE8,0xB5};
static char cmi_nt35590_para220[]={0xE9,0x03};
static char cmi_nt35590_para221[]={0xEA,0xC6};
static char cmi_nt35590_para222[]={0xEB,0x03};
static char cmi_nt35590_para223[]={0xEC,0xCF};
static char cmi_nt35590_para224[]={0xED,0x03};
static char cmi_nt35590_para225[]={0xEE,0xD1};
static char cmi_nt35590_para226[]={0xEF,0x00};
static char cmi_nt35590_para227[]={0xF0,0x42};
static char cmi_nt35590_para228[]={0xF1,0x00};
static char cmi_nt35590_para229[]={0xF2,0x56};
static char cmi_nt35590_para230[]={0xF3,0x00};
static char cmi_nt35590_para231[]={0xF4,0x79};
static char cmi_nt35590_para232[]={0xF5,0x00};
static char cmi_nt35590_para233[]={0xF6,0x97};
static char cmi_nt35590_para234[]={0xF7,0x00};
static char cmi_nt35590_para235[]={0xF8,0xB1};
static char cmi_nt35590_para236[]={0xF9,0x00};
static char cmi_nt35590_para237[]={0xFA,0xC8};
static char cmi_nt35590_para238[]={0xFF,0x02};
static char cmi_nt35590_para239[]={0x00,0x00};
static char cmi_nt35590_para240[]={0x01,0xDB};
static char cmi_nt35590_para241[]={0x02,0x00};
static char cmi_nt35590_para242[]={0x03,0xEC};
static char cmi_nt35590_para243[]={0x04,0x00};
static char cmi_nt35590_para244[]={0x05,0xFB};
static char cmi_nt35590_para245[]={0x06,0x01};
static char cmi_nt35590_para246[]={0x07,0x26};
static char cmi_nt35590_para247[]={0x08,0x01};
static char cmi_nt35590_para248[]={0x09,0x49};
static char cmi_nt35590_para249[]={0x0A,0x01};
static char cmi_nt35590_para250[]={0x0B,0x86};
static char cmi_nt35590_para251[]={0x0C,0x01};
static char cmi_nt35590_para252[]={0x0D,0xB3};
static char cmi_nt35590_para253[]={0x0E,0x01};
static char cmi_nt35590_para254[]={0x0F,0xFC};
static char cmi_nt35590_para255[]={0x10,0x02};
static char cmi_nt35590_para256[]={0x11,0x37};
static char cmi_nt35590_para257[]={0x12,0x02};
static char cmi_nt35590_para258[]={0x13,0x39};
static char cmi_nt35590_para259[]={0x14,0x02};
static char cmi_nt35590_para260[]={0x15,0x6F};
static char cmi_nt35590_para261[]={0x16,0x02};
static char cmi_nt35590_para262[]={0x17,0xAA};
static char cmi_nt35590_para263[]={0x18,0x02};
static char cmi_nt35590_para264[]={0x19,0xC9};
static char cmi_nt35590_para265[]={0x1A,0x02};
static char cmi_nt35590_para266[]={0x1B,0xFC};
static char cmi_nt35590_para267[]={0x1C,0x03};
static char cmi_nt35590_para268[]={0x1D,0x20};
static char cmi_nt35590_para269[]={0x1E,0x03};
static char cmi_nt35590_para270[]={0x1F,0x52};
static char cmi_nt35590_para271[]={0x20,0x03};
static char cmi_nt35590_para272[]={0x21,0x62};
static char cmi_nt35590_para273[]={0x22,0x03};
static char cmi_nt35590_para274[]={0x23,0x75};
static char cmi_nt35590_para275[]={0x24,0x03};
static char cmi_nt35590_para276[]={0x25,0x8A};
static char cmi_nt35590_para277[]={0x26,0x03};
static char cmi_nt35590_para278[]={0x27,0xA1};
static char cmi_nt35590_para279[]={0x28,0x03};
static char cmi_nt35590_para280[]={0x29,0xB5};
static char cmi_nt35590_para281[]={0x2A,0x03};
static char cmi_nt35590_para282[]={0x2B,0xC6};
static char cmi_nt35590_para283[]={0x2D,0x03};
static char cmi_nt35590_para284[]={0x2F,0xCF};
static char cmi_nt35590_para285[]={0x30,0x03};
static char cmi_nt35590_para286[]={0x31,0xD1};
static char cmi_nt35590_para287[]={0x32,0x00};
static char cmi_nt35590_para288[]={0x33,0x42};
static char cmi_nt35590_para289[]={0x34,0x00};
static char cmi_nt35590_para290[]={0x35,0x56};
static char cmi_nt35590_para291[]={0x36,0x00};
static char cmi_nt35590_para292[]={0x37,0x79};
static char cmi_nt35590_para293[]={0x38,0x00};
static char cmi_nt35590_para294[]={0x39,0x97};
static char cmi_nt35590_para295[]={0x3A,0x00};
static char cmi_nt35590_para296[]={0x3B,0xB1};
static char cmi_nt35590_para297[]={0x3D,0x00};
static char cmi_nt35590_para298[]={0x3F,0xC8};
static char cmi_nt35590_para299[]={0x40,0x00};
static char cmi_nt35590_para300[]={0x41,0xDB};
static char cmi_nt35590_para301[]={0x42,0x00};
static char cmi_nt35590_para302[]={0x43,0xEC};
static char cmi_nt35590_para303[]={0x44,0x00};
static char cmi_nt35590_para304[]={0x45,0xFB};
static char cmi_nt35590_para305[]={0x46,0x01};
static char cmi_nt35590_para306[]={0x47,0x26};
static char cmi_nt35590_para307[]={0x48,0x01};
static char cmi_nt35590_para308[]={0x49,0x49};
static char cmi_nt35590_para309[]={0x4A,0x01};
static char cmi_nt35590_para310[]={0x4B,0x86};
static char cmi_nt35590_para311[]={0x4C,0x01};
static char cmi_nt35590_para312[]={0x4D,0xB3};
static char cmi_nt35590_para313[]={0x4E,0x01};
static char cmi_nt35590_para314[]={0x4F,0xFC};
static char cmi_nt35590_para315[]={0x50,0x02};
static char cmi_nt35590_para316[]={0x51,0x37};
static char cmi_nt35590_para317[]={0x52,0x02};
static char cmi_nt35590_para318[]={0x53,0x39};
static char cmi_nt35590_para319[]={0x54,0x02};
static char cmi_nt35590_para320[]={0x55,0x6F};
static char cmi_nt35590_para321[]={0x56,0x02};
static char cmi_nt35590_para322[]={0x58,0xAA};
static char cmi_nt35590_para323[]={0x59,0x02};
static char cmi_nt35590_para324[]={0x5A,0xC9};
static char cmi_nt35590_para325[]={0x5B,0x02};
static char cmi_nt35590_para326[]={0x5C,0xFC};
static char cmi_nt35590_para327[]={0x5D,0x03};
static char cmi_nt35590_para328[]={0x5E,0x20};
static char cmi_nt35590_para329[]={0x5F,0x03};
static char cmi_nt35590_para330[]={0x60,0x52};
static char cmi_nt35590_para331[]={0x61,0x03};
static char cmi_nt35590_para332[]={0x62,0x62};
static char cmi_nt35590_para333[]={0x63,0x03};
static char cmi_nt35590_para334[]={0x64,0x75};
static char cmi_nt35590_para335[]={0x65,0x03};
static char cmi_nt35590_para336[]={0x66,0x8A};
static char cmi_nt35590_para337[]={0x67,0x03};
static char cmi_nt35590_para338[]={0x68,0xA1};
static char cmi_nt35590_para339[]={0x69,0x03};
static char cmi_nt35590_para340[]={0x6A,0xB5};
static char cmi_nt35590_para341[]={0x6B,0x03};
static char cmi_nt35590_para342[]={0x6C,0xC6};
static char cmi_nt35590_para343[]={0x6D,0x03};
static char cmi_nt35590_para344[]={0x6E,0xCF};
static char cmi_nt35590_para345[]={0x6F,0x03};
static char cmi_nt35590_para346[]={0x70,0xD1};
static char cmi_nt35590_para347[]={0x71,0x00};
static char cmi_nt35590_para348[]={0x72,0x42};
static char cmi_nt35590_para349[]={0x73,0x00};
static char cmi_nt35590_para350[]={0x74,0x56};
static char cmi_nt35590_para351[]={0x75,0x00};
static char cmi_nt35590_para352[]={0x76,0x79};
static char cmi_nt35590_para353[]={0x77,0x00};
static char cmi_nt35590_para354[]={0x78,0x97};
static char cmi_nt35590_para355[]={0x79,0x00};
static char cmi_nt35590_para356[]={0x7A,0xB1};
static char cmi_nt35590_para357[]={0x7B,0x00};
static char cmi_nt35590_para358[]={0x7C,0xC8};
static char cmi_nt35590_para359[]={0x7D,0x00};
static char cmi_nt35590_para360[]={0x7E,0xDB};
static char cmi_nt35590_para361[]={0x7F,0x00};
static char cmi_nt35590_para362[]={0x80,0xEC};
static char cmi_nt35590_para363[]={0x81,0x00};
static char cmi_nt35590_para364[]={0x82,0xFB};
static char cmi_nt35590_para365[]={0x83,0x01};
static char cmi_nt35590_para366[]={0x84,0x26};
static char cmi_nt35590_para367[]={0x85,0x01};
static char cmi_nt35590_para368[]={0x86,0x49};
static char cmi_nt35590_para369[]={0x87,0x01};
static char cmi_nt35590_para370[]={0x88,0x86};
static char cmi_nt35590_para371[]={0x89,0x01};
static char cmi_nt35590_para372[]={0x8A,0xB3};
static char cmi_nt35590_para373[]={0x8B,0x01};
static char cmi_nt35590_para374[]={0x8C,0xFC};
static char cmi_nt35590_para375[]={0x8D,0x02};
static char cmi_nt35590_para376[]={0x8E,0x37};
static char cmi_nt35590_para377[]={0x8F,0x02};
static char cmi_nt35590_para378[]={0x90,0x39};
static char cmi_nt35590_para379[]={0x91,0x02};
static char cmi_nt35590_para380[]={0x92,0x6F};
static char cmi_nt35590_para381[]={0x93,0x02};
static char cmi_nt35590_para382[]={0x94,0xAA};
static char cmi_nt35590_para383[]={0x95,0x02};
static char cmi_nt35590_para384[]={0x96,0xC9};
static char cmi_nt35590_para385[]={0x97,0x02};
static char cmi_nt35590_para386[]={0x98,0xFC};
static char cmi_nt35590_para387[]={0x99,0x03};
static char cmi_nt35590_para388[]={0x9A,0x20};
static char cmi_nt35590_para389[]={0x9B,0x03};
static char cmi_nt35590_para390[]={0x9C,0x52};
static char cmi_nt35590_para391[]={0x9D,0x03};
static char cmi_nt35590_para392[]={0x9E,0x62};
static char cmi_nt35590_para393[]={0x9F,0x03};
static char cmi_nt35590_para394[]={0xA0,0x75};
static char cmi_nt35590_para395[]={0xA2,0x03};
static char cmi_nt35590_para396[]={0xA3,0x8A};
static char cmi_nt35590_para397[]={0xA4,0x03};
static char cmi_nt35590_para398[]={0xA5,0xA1};
static char cmi_nt35590_para399[]={0xA6,0x03};
static char cmi_nt35590_para400[]={0xA7,0xB5};
static char cmi_nt35590_para401[]={0xA9,0x03};
static char cmi_nt35590_para402[]={0xAA,0xC6};
static char cmi_nt35590_para403[]={0xAB,0x03};
static char cmi_nt35590_para404[]={0xAC,0xCF};
static char cmi_nt35590_para405[]={0xAD,0x03};
static char cmi_nt35590_para406[]={0xAE,0xD1};
static char cmi_nt35590_para407[]={0xAF,0x00};
static char cmi_nt35590_para408[]={0xB0,0x42};
static char cmi_nt35590_para409[]={0xB1,0x00};
static char cmi_nt35590_para410[]={0xB2,0x56};
static char cmi_nt35590_para411[]={0xB3,0x00};
static char cmi_nt35590_para412[]={0xB4,0x79};
static char cmi_nt35590_para413[]={0xB5,0x00};
static char cmi_nt35590_para414[]={0xB6,0x97};
static char cmi_nt35590_para415[]={0xB7,0x00};
static char cmi_nt35590_para416[]={0xB8,0xB1};
static char cmi_nt35590_para417[]={0xB9,0x00};
static char cmi_nt35590_para418[]={0xBA,0xC8};
static char cmi_nt35590_para419[]={0xBB,0x00};
static char cmi_nt35590_para420[]={0xBC,0xDB};
static char cmi_nt35590_para421[]={0xBD,0x00};
static char cmi_nt35590_para422[]={0xBE,0xEC};
static char cmi_nt35590_para423[]={0xBF,0x00};
static char cmi_nt35590_para424[]={0xC0,0xFB};
static char cmi_nt35590_para425[]={0xC1,0x01};
static char cmi_nt35590_para426[]={0xC2,0x26};
static char cmi_nt35590_para427[]={0xC3,0x01};
static char cmi_nt35590_para428[]={0xC4,0x49};
static char cmi_nt35590_para429[]={0xC5,0x01};
static char cmi_nt35590_para430[]={0xC6,0x86};
static char cmi_nt35590_para431[]={0xC7,0x01};
static char cmi_nt35590_para432[]={0xC8,0xB3};
static char cmi_nt35590_para433[]={0xC9,0x01};
static char cmi_nt35590_para434[]={0xCA,0xFC};
static char cmi_nt35590_para435[]={0xCB,0x02};
static char cmi_nt35590_para436[]={0xCC,0x37};
static char cmi_nt35590_para437[]={0xCD,0x02};
static char cmi_nt35590_para438[]={0xCE,0x39};
static char cmi_nt35590_para439[]={0xCF,0x02};
static char cmi_nt35590_para440[]={0xD0,0x6F};
static char cmi_nt35590_para441[]={0xD1,0x02};
static char cmi_nt35590_para442[]={0xD2,0xAA};
static char cmi_nt35590_para443[]={0xD3,0x02};
static char cmi_nt35590_para444[]={0xD4,0xC9};
static char cmi_nt35590_para445[]={0xD5,0x02};
static char cmi_nt35590_para446[]={0xD6,0xFC};
static char cmi_nt35590_para447[]={0xD7,0x03};
static char cmi_nt35590_para448[]={0xD8,0x20};
static char cmi_nt35590_para449[]={0xD9,0x03};
static char cmi_nt35590_para450[]={0xDA,0x52};
static char cmi_nt35590_para451[]={0xDB,0x03};
static char cmi_nt35590_para452[]={0xDC,0x62};
static char cmi_nt35590_para453[]={0xDD,0x03};
static char cmi_nt35590_para454[]={0xDE,0x75};
static char cmi_nt35590_para455[]={0xDF,0x03};
static char cmi_nt35590_para456[]={0xE0,0x8A};
static char cmi_nt35590_para457[]={0xE1,0x03};
static char cmi_nt35590_para458[]={0xE2,0xA1};
static char cmi_nt35590_para459[]={0xE3,0x03};
static char cmi_nt35590_para460[]={0xE4,0xB5};
static char cmi_nt35590_para461[]={0xE5,0x03};
static char cmi_nt35590_para462[]={0xE6,0xC6};
static char cmi_nt35590_para463[]={0xE7,0x03};
static char cmi_nt35590_para464[]={0xE8,0xCF};
static char cmi_nt35590_para465[]={0xE9,0x03};
static char cmi_nt35590_para466[]={0xEA,0xD1};
static char cmi_nt35590_para467[]={0xFF,0x00};
static char cmi_nt35590_para468[]={0xFB,0x01};
static char cmi_nt35590_para469[]={0xFF,0x01};
static char cmi_nt35590_para470[]={0xFB,0x01};
static char cmi_nt35590_para471[]={0xFF,0x02};
static char cmi_nt35590_para472[]={0xFB,0x01};
static char cmi_nt35590_para473[]={0xFF,0x03};
static char cmi_nt35590_para474[]={0xFB,0x01};
static char cmi_nt35590_para475[]={0xFF,0x04};
static char cmi_nt35590_para476[]={0xFB,0x01};
static char cmi_nt35590_para477[]={0xFF,0x05};
static char cmi_nt35590_para478[]={0xFB,0x01};
static char cmi_nt35590_para479[]={0xFF,0x00};
//static char cmi_nt35590_para480[]={0x51,0xFF};
//static char cmi_nt35590_para481[]={0x53,0x2C};
//static char cmi_nt35590_para482[]={0x55,0x00};
static char cmi_nt35590_para483[]={0xFF,0x00};
static char cmi_nt35590_para484[]={0x35,0x00};


static struct dsi_cmd_desc cmi_nt35590_display_on_cmds[] = {

	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para001), cmi_nt35590_para001},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para002), cmi_nt35590_para002},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para003), cmi_nt35590_para003},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para_color), cmi_nt35590_para_color},//color enhancement
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(cmi_nt35590_para0031), cmi_nt35590_para0031},//porch setting
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para004), cmi_nt35590_para004},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para005), cmi_nt35590_para005},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para006), cmi_nt35590_para006},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para007), cmi_nt35590_para007},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para008), cmi_nt35590_para008},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para009), cmi_nt35590_para009},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para010), cmi_nt35590_para010},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para011), cmi_nt35590_para011},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para012), cmi_nt35590_para012},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para013), cmi_nt35590_para013},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para014), cmi_nt35590_para014},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para015), cmi_nt35590_para015},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para016), cmi_nt35590_para016},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para017), cmi_nt35590_para017},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para018), cmi_nt35590_para018},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para019), cmi_nt35590_para019},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para020), cmi_nt35590_para020},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para021), cmi_nt35590_para021},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para022), cmi_nt35590_para022},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para023), cmi_nt35590_para023},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para024), cmi_nt35590_para024},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para025), cmi_nt35590_para025},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para026), cmi_nt35590_para026},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para027), cmi_nt35590_para027},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para028), cmi_nt35590_para028},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para029), cmi_nt35590_para029},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para030), cmi_nt35590_para030},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para031), cmi_nt35590_para031},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para032), cmi_nt35590_para032},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para033), cmi_nt35590_para033},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para034), cmi_nt35590_para034},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para035), cmi_nt35590_para035},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para036), cmi_nt35590_para036},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para037), cmi_nt35590_para037},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para038), cmi_nt35590_para038},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para039), cmi_nt35590_para039},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para040), cmi_nt35590_para040},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para041), cmi_nt35590_para041},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para042), cmi_nt35590_para042},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para043), cmi_nt35590_para043},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para044), cmi_nt35590_para044},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para045), cmi_nt35590_para045},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para046), cmi_nt35590_para046},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para047), cmi_nt35590_para047},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para048), cmi_nt35590_para048},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para049), cmi_nt35590_para049},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para050), cmi_nt35590_para050},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para051), cmi_nt35590_para051},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para052), cmi_nt35590_para052},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para053), cmi_nt35590_para053},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para054), cmi_nt35590_para054},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para055), cmi_nt35590_para055},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para056), cmi_nt35590_para056},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para057), cmi_nt35590_para057},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para058), cmi_nt35590_para058},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para059), cmi_nt35590_para059},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para060), cmi_nt35590_para060},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para061), cmi_nt35590_para061},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para062), cmi_nt35590_para062},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para063), cmi_nt35590_para063},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para064), cmi_nt35590_para064},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para065), cmi_nt35590_para065},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para066), cmi_nt35590_para066},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para067), cmi_nt35590_para067},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para068), cmi_nt35590_para068},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para069), cmi_nt35590_para069},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para070), cmi_nt35590_para070},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para071), cmi_nt35590_para071},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para072), cmi_nt35590_para072},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para073), cmi_nt35590_para073},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para074), cmi_nt35590_para074},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para075), cmi_nt35590_para075},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para076), cmi_nt35590_para076},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para077), cmi_nt35590_para077},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para078), cmi_nt35590_para078},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para079), cmi_nt35590_para079},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para080), cmi_nt35590_para080},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para081), cmi_nt35590_para081},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para082), cmi_nt35590_para082},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para083), cmi_nt35590_para083},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para084), cmi_nt35590_para084},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para085), cmi_nt35590_para085},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para086), cmi_nt35590_para086},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para087), cmi_nt35590_para087},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para088), cmi_nt35590_para088},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para089), cmi_nt35590_para089},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para090), cmi_nt35590_para090},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para091), cmi_nt35590_para091},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para092), cmi_nt35590_para092},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para093), cmi_nt35590_para093},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para094), cmi_nt35590_para094},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para095), cmi_nt35590_para095},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para096), cmi_nt35590_para096},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para097), cmi_nt35590_para097},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para098), cmi_nt35590_para098},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para099), cmi_nt35590_para099},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para100), cmi_nt35590_para100},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para101), cmi_nt35590_para101},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para102), cmi_nt35590_para102},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para103), cmi_nt35590_para103},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para104), cmi_nt35590_para104},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para105), cmi_nt35590_para105},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para106), cmi_nt35590_para106},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para107), cmi_nt35590_para107},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para108), cmi_nt35590_para108},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para109), cmi_nt35590_para109},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para110), cmi_nt35590_para110},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para111), cmi_nt35590_para111},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para112), cmi_nt35590_para112},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para113), cmi_nt35590_para113},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para114), cmi_nt35590_para114},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para115), cmi_nt35590_para115},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para116), cmi_nt35590_para116},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para117), cmi_nt35590_para117},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para118), cmi_nt35590_para118},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para119), cmi_nt35590_para119},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para120), cmi_nt35590_para120},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para121), cmi_nt35590_para121},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para122), cmi_nt35590_para122},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para123), cmi_nt35590_para123},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para124), cmi_nt35590_para124},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para125), cmi_nt35590_para125},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para126), cmi_nt35590_para126},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para127), cmi_nt35590_para127},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para128), cmi_nt35590_para128},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para129), cmi_nt35590_para129},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para130), cmi_nt35590_para130},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para131), cmi_nt35590_para131},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para132), cmi_nt35590_para132},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para133), cmi_nt35590_para133},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para134), cmi_nt35590_para134},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para135), cmi_nt35590_para135},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para136), cmi_nt35590_para136},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para137), cmi_nt35590_para137},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para138), cmi_nt35590_para138},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para139), cmi_nt35590_para139},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para140), cmi_nt35590_para140},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para141), cmi_nt35590_para141},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para142), cmi_nt35590_para142},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para143), cmi_nt35590_para143},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para144), cmi_nt35590_para144},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para145), cmi_nt35590_para145},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para146), cmi_nt35590_para146},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para147), cmi_nt35590_para147},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para148), cmi_nt35590_para148},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para149), cmi_nt35590_para149},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para150), cmi_nt35590_para150},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para151), cmi_nt35590_para151},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para152), cmi_nt35590_para152},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para153), cmi_nt35590_para153},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para154), cmi_nt35590_para154},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para155), cmi_nt35590_para155},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para156), cmi_nt35590_para156},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para157), cmi_nt35590_para157},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para158), cmi_nt35590_para158},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para159), cmi_nt35590_para159},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para160), cmi_nt35590_para160},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para161), cmi_nt35590_para161},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para162), cmi_nt35590_para162},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para163), cmi_nt35590_para163},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para164), cmi_nt35590_para164},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para165), cmi_nt35590_para165},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para166), cmi_nt35590_para166},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para167), cmi_nt35590_para167},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para168), cmi_nt35590_para168},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para169), cmi_nt35590_para169},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para170), cmi_nt35590_para170},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para171), cmi_nt35590_para171},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para172), cmi_nt35590_para172},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para173), cmi_nt35590_para173},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para174), cmi_nt35590_para174},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para175), cmi_nt35590_para175},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para176), cmi_nt35590_para176},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para177), cmi_nt35590_para177},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para178), cmi_nt35590_para178},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para179), cmi_nt35590_para179},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para180), cmi_nt35590_para180},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para181), cmi_nt35590_para181},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para182), cmi_nt35590_para182},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para183), cmi_nt35590_para183},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para184), cmi_nt35590_para184},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para185), cmi_nt35590_para185},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para186), cmi_nt35590_para186},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para187), cmi_nt35590_para187},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para188), cmi_nt35590_para188},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para189), cmi_nt35590_para189},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para190), cmi_nt35590_para190},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para191), cmi_nt35590_para191},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para192), cmi_nt35590_para192},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para193), cmi_nt35590_para193},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para194), cmi_nt35590_para194},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para195), cmi_nt35590_para195},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para196), cmi_nt35590_para196},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para197), cmi_nt35590_para197},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para198), cmi_nt35590_para198},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para199), cmi_nt35590_para199},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para200), cmi_nt35590_para200},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para201), cmi_nt35590_para201},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para202), cmi_nt35590_para202},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para203), cmi_nt35590_para203},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para204), cmi_nt35590_para204},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para205), cmi_nt35590_para205},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para206), cmi_nt35590_para206},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para207), cmi_nt35590_para207},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para208), cmi_nt35590_para208},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para209), cmi_nt35590_para209},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para210), cmi_nt35590_para210},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para211), cmi_nt35590_para211},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para212), cmi_nt35590_para212},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para213), cmi_nt35590_para213},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para214), cmi_nt35590_para214},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para215), cmi_nt35590_para215},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para216), cmi_nt35590_para216},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para217), cmi_nt35590_para217},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para218), cmi_nt35590_para218},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para219), cmi_nt35590_para219},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para220), cmi_nt35590_para220},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para221), cmi_nt35590_para221},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para222), cmi_nt35590_para222},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para223), cmi_nt35590_para223},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para224), cmi_nt35590_para224},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para225), cmi_nt35590_para225},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para226), cmi_nt35590_para226},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para227), cmi_nt35590_para227},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para228), cmi_nt35590_para228},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para229), cmi_nt35590_para229},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para230), cmi_nt35590_para230},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para231), cmi_nt35590_para231},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para232), cmi_nt35590_para232},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para233), cmi_nt35590_para233},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para234), cmi_nt35590_para234},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para235), cmi_nt35590_para235},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para236), cmi_nt35590_para236},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para237), cmi_nt35590_para237},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para238), cmi_nt35590_para238},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para239), cmi_nt35590_para239},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para240), cmi_nt35590_para240},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para241), cmi_nt35590_para241},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para242), cmi_nt35590_para242},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para243), cmi_nt35590_para243},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para244), cmi_nt35590_para244},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para245), cmi_nt35590_para245},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para246), cmi_nt35590_para246},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para247), cmi_nt35590_para247},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para248), cmi_nt35590_para248},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para249), cmi_nt35590_para249},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para250), cmi_nt35590_para250},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para251), cmi_nt35590_para251},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para252), cmi_nt35590_para252},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para253), cmi_nt35590_para253},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para254), cmi_nt35590_para254},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para255), cmi_nt35590_para255},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para256), cmi_nt35590_para256},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para257), cmi_nt35590_para257},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para258), cmi_nt35590_para258},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para259), cmi_nt35590_para259},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para260), cmi_nt35590_para260},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para261), cmi_nt35590_para261},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para262), cmi_nt35590_para262},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para263), cmi_nt35590_para263},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para264), cmi_nt35590_para264},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para265), cmi_nt35590_para265},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para266), cmi_nt35590_para266},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para267), cmi_nt35590_para267},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para268), cmi_nt35590_para268},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para269), cmi_nt35590_para269},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para270), cmi_nt35590_para270},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para271), cmi_nt35590_para271},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para272), cmi_nt35590_para272},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para273), cmi_nt35590_para273},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para274), cmi_nt35590_para274},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para275), cmi_nt35590_para275},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para276), cmi_nt35590_para276},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para277), cmi_nt35590_para277},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para278), cmi_nt35590_para278},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para279), cmi_nt35590_para279},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para280), cmi_nt35590_para280},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para281), cmi_nt35590_para281},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para282), cmi_nt35590_para282},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para283), cmi_nt35590_para283},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para284), cmi_nt35590_para284},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para285), cmi_nt35590_para285},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para286), cmi_nt35590_para286},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para287), cmi_nt35590_para287},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para288), cmi_nt35590_para288},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para289), cmi_nt35590_para289},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para290), cmi_nt35590_para290},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para291), cmi_nt35590_para291},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para292), cmi_nt35590_para292},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para293), cmi_nt35590_para293},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para294), cmi_nt35590_para294},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para295), cmi_nt35590_para295},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para296), cmi_nt35590_para296},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para297), cmi_nt35590_para297},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para298), cmi_nt35590_para298},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para299), cmi_nt35590_para299},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para300), cmi_nt35590_para300},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para301), cmi_nt35590_para301},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para302), cmi_nt35590_para302},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para303), cmi_nt35590_para303},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para304), cmi_nt35590_para304},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para305), cmi_nt35590_para305},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para306), cmi_nt35590_para306},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para307), cmi_nt35590_para307},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para308), cmi_nt35590_para308},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para309), cmi_nt35590_para309},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para310), cmi_nt35590_para310},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para311), cmi_nt35590_para311},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para312), cmi_nt35590_para312},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para313), cmi_nt35590_para313},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para314), cmi_nt35590_para314},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para315), cmi_nt35590_para315},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para316), cmi_nt35590_para316},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para317), cmi_nt35590_para317},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para318), cmi_nt35590_para318},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para319), cmi_nt35590_para319},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para320), cmi_nt35590_para320},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para321), cmi_nt35590_para321},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para322), cmi_nt35590_para322},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para323), cmi_nt35590_para323},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para324), cmi_nt35590_para324},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para325), cmi_nt35590_para325},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para326), cmi_nt35590_para326},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para327), cmi_nt35590_para327},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para328), cmi_nt35590_para328},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para329), cmi_nt35590_para329},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para330), cmi_nt35590_para330},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para331), cmi_nt35590_para331},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para332), cmi_nt35590_para332},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para333), cmi_nt35590_para333},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para334), cmi_nt35590_para334},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para335), cmi_nt35590_para335},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para336), cmi_nt35590_para336},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para337), cmi_nt35590_para337},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para338), cmi_nt35590_para338},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para339), cmi_nt35590_para339},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para340), cmi_nt35590_para340},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para341), cmi_nt35590_para341},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para342), cmi_nt35590_para342},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para343), cmi_nt35590_para343},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para344), cmi_nt35590_para344},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para345), cmi_nt35590_para345},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para346), cmi_nt35590_para346},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para347), cmi_nt35590_para347},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para348), cmi_nt35590_para348},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para349), cmi_nt35590_para349},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para350), cmi_nt35590_para350},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para351), cmi_nt35590_para351},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para352), cmi_nt35590_para352},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para353), cmi_nt35590_para353},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para354), cmi_nt35590_para354},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para355), cmi_nt35590_para355},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para356), cmi_nt35590_para356},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para357), cmi_nt35590_para357},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para358), cmi_nt35590_para358},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para359), cmi_nt35590_para359},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para360), cmi_nt35590_para360},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para361), cmi_nt35590_para361},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para362), cmi_nt35590_para362},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para363), cmi_nt35590_para363},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para364), cmi_nt35590_para364},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para365), cmi_nt35590_para365},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para366), cmi_nt35590_para366},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para367), cmi_nt35590_para367},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para368), cmi_nt35590_para368},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para369), cmi_nt35590_para369},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para370), cmi_nt35590_para370},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para371), cmi_nt35590_para371},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para372), cmi_nt35590_para372},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para373), cmi_nt35590_para373},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para374), cmi_nt35590_para374},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para375), cmi_nt35590_para375},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para376), cmi_nt35590_para376},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para377), cmi_nt35590_para377},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para378), cmi_nt35590_para378},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para379), cmi_nt35590_para379},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para380), cmi_nt35590_para380},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para381), cmi_nt35590_para381},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para382), cmi_nt35590_para382},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para383), cmi_nt35590_para383},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para384), cmi_nt35590_para384},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para385), cmi_nt35590_para385},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para386), cmi_nt35590_para386},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para387), cmi_nt35590_para387},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para388), cmi_nt35590_para388},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para389), cmi_nt35590_para389},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para390), cmi_nt35590_para390},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para391), cmi_nt35590_para391},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para392), cmi_nt35590_para392},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para393), cmi_nt35590_para393},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para394), cmi_nt35590_para394},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para395), cmi_nt35590_para395},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para396), cmi_nt35590_para396},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para397), cmi_nt35590_para397},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para398), cmi_nt35590_para398},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para399), cmi_nt35590_para399},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para400), cmi_nt35590_para400},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para401), cmi_nt35590_para401},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para402), cmi_nt35590_para402},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para403), cmi_nt35590_para403},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para404), cmi_nt35590_para404},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para405), cmi_nt35590_para405},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para406), cmi_nt35590_para406},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para407), cmi_nt35590_para407},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para408), cmi_nt35590_para408},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para409), cmi_nt35590_para409},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para410), cmi_nt35590_para410},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para411), cmi_nt35590_para411},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para412), cmi_nt35590_para412},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para413), cmi_nt35590_para413},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para414), cmi_nt35590_para414},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para415), cmi_nt35590_para415},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para416), cmi_nt35590_para416},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para417), cmi_nt35590_para417},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para418), cmi_nt35590_para418},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para419), cmi_nt35590_para419},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para420), cmi_nt35590_para420},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para421), cmi_nt35590_para421},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para422), cmi_nt35590_para422},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para423), cmi_nt35590_para423},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para424), cmi_nt35590_para424},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para425), cmi_nt35590_para425},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para426), cmi_nt35590_para426},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para427), cmi_nt35590_para427},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para428), cmi_nt35590_para428},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para429), cmi_nt35590_para429},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para430), cmi_nt35590_para430},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para431), cmi_nt35590_para431},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para432), cmi_nt35590_para432},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para433), cmi_nt35590_para433},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para434), cmi_nt35590_para434},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para435), cmi_nt35590_para435},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para436), cmi_nt35590_para436},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para437), cmi_nt35590_para437},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para438), cmi_nt35590_para438},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para439), cmi_nt35590_para439},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para440), cmi_nt35590_para440},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para441), cmi_nt35590_para441},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para442), cmi_nt35590_para442},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para443), cmi_nt35590_para443},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para444), cmi_nt35590_para444},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para445), cmi_nt35590_para445},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para446), cmi_nt35590_para446},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para447), cmi_nt35590_para447},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para448), cmi_nt35590_para448},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para449), cmi_nt35590_para449},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para450), cmi_nt35590_para450},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para451), cmi_nt35590_para451},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para452), cmi_nt35590_para452},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para453), cmi_nt35590_para453},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para454), cmi_nt35590_para454},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para455), cmi_nt35590_para455},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para456), cmi_nt35590_para456},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para457), cmi_nt35590_para457},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para458), cmi_nt35590_para458},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para459), cmi_nt35590_para459},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para460), cmi_nt35590_para460},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para461), cmi_nt35590_para461},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para462), cmi_nt35590_para462},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para463), cmi_nt35590_para463},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para464), cmi_nt35590_para464},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para465), cmi_nt35590_para465},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para466), cmi_nt35590_para466},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para467), cmi_nt35590_para467},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para468), cmi_nt35590_para468},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para469), cmi_nt35590_para469},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para470), cmi_nt35590_para470},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para471), cmi_nt35590_para471},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para472), cmi_nt35590_para472},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para473), cmi_nt35590_para473},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para474), cmi_nt35590_para474},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para475), cmi_nt35590_para475},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para476), cmi_nt35590_para476},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para477), cmi_nt35590_para477},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para478), cmi_nt35590_para478},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para479), cmi_nt35590_para479},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cabc_cmd_C1_51), cabc_cmd_C1_51},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cabc_cmd_C1_53), cabc_cmd_C1_53},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cabc_cmd_C1_55), cabc_cmd_C1_55},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para483), cmi_nt35590_para483},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35590_para484), cmi_nt35590_para484},
	{DTYPE_DCS_WRITE, 1, 0, 0, 120, sizeof(exit_sleep), exit_sleep},
	{DTYPE_DCS_WRITE, 1, 0, 0, 10, sizeof(display_on), display_on},
};


static char boe_hx8394_para1[]={0xB9,0xFF,0x83,0x94};
static char boe_hx8394_para2[]={0xBA,0x13};
static char boe_hx8394_para3[]={0xB1,0x7C,0x00,0x34,0x09,0x01,0x11,0x11,0x36,0x3E,0x26,0x26,0x57,0x12,0x01,0xE6};
static char boe_hx8394_para4[]={0xB4,0x00,0x00,0x00,0x05,0x06,0x41,0x42,0x02,0x41,0x42,0x43,0x47,0x19,0x58,0x60,0x08,0x85,0x10};
static char boe_hx8394_para5[]={0xD5,0x4C,0x01,0x00,0x01,0xCD,0x23,0xEF,0x45,0x67,0x89,0xAB,0x11,0x00,0xDC,0x10,0xFE,0x32,0xBA,0x98,0x76,0x54,0x00,0x11,0x40};
static char boe_hx8394_para6[]={0xE0,0x24,0x33,0x36,0x3F,0x3F,0x3F,0x3C,0x56,0x05,0x0C,0x0E,0x11,0x13,0x12,0x14,0x12,0x1E,0x24,0x33,0x36,0x3F,0x3F,0x3F,0x3C,0x56,0x05,
																0x0C,0x0E,0x11,0x13,0x12,0x14,0x12,0x1E};
static char boe_hx8394_para7[]={0xCC,0x01};
static char boe_hx8394_para8[]={0xB6,0x2A};
static char boe_hx8394_para9[]={0x36,0x02};

static struct dsi_cmd_desc boe_hx8394_video_on_cmds[] = {

	{DTYPE_DCS_WRITE, 1, 0, 0, 200, sizeof(exit_sleep), exit_sleep},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(boe_hx8394_para1), boe_hx8394_para1}, 
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(boe_hx8394_para2), boe_hx8394_para2},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(boe_hx8394_para3), boe_hx8394_para3},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(boe_hx8394_para4), boe_hx8394_para4},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(boe_hx8394_para5), boe_hx8394_para5},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(boe_hx8394_para6), boe_hx8394_para6},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 50, sizeof(boe_hx8394_para7), boe_hx8394_para7},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(boe_hx8394_para8), boe_hx8394_para8},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(boe_hx8394_para9), boe_hx8394_para9},
	
	
	{DTYPE_DCS_WRITE, 1, 0, 0, 50, sizeof(display_on), display_on}, 
	 
};

static char auo_nt35590_para001[]={0xFF,0xEE};
static char auo_nt35590_para002[]={0x26,0x08};
static char auo_nt35590_para003[]={0x26,0x00};
static char auo_nt35590_para004[]={0xFF,0x00};
static char auo_nt35590_para005[]={0xFF,0x01};
static char auo_nt35590_para006[]={0xFB,0x01};
static char auo_nt35590_para007[]={0x00,0x4A};
static char auo_nt35590_para008[]={0x01,0x33};
static char auo_nt35590_para009[]={0x02,0x53};
static char auo_nt35590_para010[]={0x03,0x55};
static char auo_nt35590_para011[]={0x04,0x55};
static char auo_nt35590_para012[]={0x05,0x33};
static char auo_nt35590_para013[]={0x06,0x22};
static char auo_nt35590_para014[]={0x08,0x56};
static char auo_nt35590_para015[]={0x09,0x8F};
static char auo_nt35590_para016[]={0x0B,0xCF};
static char auo_nt35590_para017[]={0x0C,0xCF};
static char auo_nt35590_para018[]={0x0D,0x2F};
static char auo_nt35590_para019[]={0x0E,0x29};
static char auo_nt35590_para020[]={0x36,0x73};
static char auo_nt35590_para021[]={0x0F,0x0A};
static char auo_nt35590_para022[]={0xFF,0xEE};
static char auo_nt35590_para023[]={0xFB,0x01};
static char auo_nt35590_para024[]={0x12,0x50};
static char auo_nt35590_para025[]={0x13,0x02};
static char auo_nt35590_para026[]={0x6A,0x60};
static char auo_nt35590_para027[]={0xFF,0x05};
static char auo_nt35590_para028[]={0xFB,0x01};
static char auo_nt35590_para029[]={0x01,0x00};
static char auo_nt35590_para030[]={0x02,0x82};
static char auo_nt35590_para031[]={0x03,0x82};
static char auo_nt35590_para032[]={0x04,0x82};
static char auo_nt35590_para033[]={0x06,0x33};
static char auo_nt35590_para034[]={0x07,0x01};
static char auo_nt35590_para035[]={0x08,0x00};
static char auo_nt35590_para036[]={0x09,0x46};
static char auo_nt35590_para037[]={0x0A,0x46};
static char auo_nt35590_para038[]={0x0D,0x0B};
static char auo_nt35590_para039[]={0x0E,0x1D};
static char auo_nt35590_para040[]={0x0F,0x08};
static char auo_nt35590_para041[]={0x10,0x53};
static char auo_nt35590_para042[]={0x11,0x00};
static char auo_nt35590_para043[]={0x12,0x00};
static char auo_nt35590_para044[]={0x14,0x01};
static char auo_nt35590_para045[]={0x15,0x00};
static char auo_nt35590_para046[]={0x16,0x05};
static char auo_nt35590_para047[]={0x17,0x00};
static char auo_nt35590_para048[]={0x19,0x7F};
static char auo_nt35590_para049[]={0x1A,0xFF};
static char auo_nt35590_para050[]={0x1B,0x0F};
static char auo_nt35590_para051[]={0x1C,0x00};
static char auo_nt35590_para052[]={0x1D,0x00};
static char auo_nt35590_para053[]={0x1E,0x00};
static char auo_nt35590_para054[]={0x1F,0x07};
static char auo_nt35590_para055[]={0x20,0x00};
static char auo_nt35590_para056[]={0x21,0x00};
static char auo_nt35590_para057[]={0x22,0x55};
static char auo_nt35590_para058[]={0x23,0x4D};
static char auo_nt35590_para059[]={0x2D,0x02};
static char auo_nt35590_para060[]={0x83,0x01};
static char auo_nt35590_para061[]={0x9E,0x58};
static char auo_nt35590_para062[]={0x9F,0x6A};
static char auo_nt35590_para063[]={0xA0,0x01};
static char auo_nt35590_para064[]={0xA2,0x10};
static char auo_nt35590_para065[]={0xBB,0x0A};
static char auo_nt35590_para066[]={0xBC,0x0A};
static char auo_nt35590_para067[]={0x28,0x01};
static char auo_nt35590_para068[]={0x2F,0x02};
static char auo_nt35590_para069[]={0x32,0x08};
static char auo_nt35590_para070[]={0x33,0xB8};
static char auo_nt35590_para071[]={0x36,0x01};
static char auo_nt35590_para072[]={0x37,0x00};
static char auo_nt35590_para073[]={0x43,0x00};
static char auo_nt35590_para074[]={0x4B,0x21};
static char auo_nt35590_para075[]={0x4C,0x03};
static char auo_nt35590_para076[]={0x50,0x21};
static char auo_nt35590_para077[]={0x51,0x03};
static char auo_nt35590_para078[]={0x58,0x21};
static char auo_nt35590_para079[]={0x59,0x03};
static char auo_nt35590_para080[]={0x5D,0x21};
static char auo_nt35590_para081[]={0x5E,0x03};
static char auo_nt35590_para082[]={0x6C,0x00};
static char auo_nt35590_para083[]={0x6D,0x00};
static char auo_nt35590_para084[]={0xFF,0x00};
static char auo_nt35590_para085[]={0xFB,0x01};
static char auo_nt35590_para086[]={0xBA,0x03};
static char auo_nt35590_para087[]={0xC2,0x08};
static char auo_nt35590_para088[]={0xFF,0x03};
static char auo_nt35590_para089[]={0xFE,0x08};
static char auo_nt35590_para090[]={0x18,0x04};
static char auo_nt35590_para091[]={0x19,0x0C};
static char auo_nt35590_para092[]={0x1A,0x14};
static char auo_nt35590_para093[]={0x25,0x26};
static char auo_nt35590_para094[]={0xFB,0x01};
static char auo_nt35590_para095[]={0xFF,0x00};
static char auo_nt35590_para096[]={0xFE,0x01};
static char auo_nt35590_para097[]={0xFF,0x00};
static char auo_nt35590_para098[]={0x3A,0x77};
static char auo_nt35590_para099[]={0xFF,0x00};
static char auo_nt35590_para100[]={0x35,0x00};

static struct dsi_cmd_desc auo_nt35590_display_on_cmds[] = {
	
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(auo_nt35590_para001), auo_nt35590_para001},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(auo_nt35590_para002), auo_nt35590_para002},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(auo_nt35590_para003), auo_nt35590_para003},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(auo_nt35590_para004), auo_nt35590_para004},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(auo_nt35590_para005), auo_nt35590_para005},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(auo_nt35590_para006), auo_nt35590_para006},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(auo_nt35590_para007), auo_nt35590_para007},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(auo_nt35590_para008), auo_nt35590_para008},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(auo_nt35590_para009), auo_nt35590_para009},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(auo_nt35590_para010), auo_nt35590_para010},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(auo_nt35590_para011), auo_nt35590_para011},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(auo_nt35590_para012), auo_nt35590_para012},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(auo_nt35590_para013), auo_nt35590_para013},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(auo_nt35590_para014), auo_nt35590_para014},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(auo_nt35590_para015), auo_nt35590_para015},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(auo_nt35590_para016), auo_nt35590_para016},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(auo_nt35590_para017), auo_nt35590_para017},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(auo_nt35590_para018), auo_nt35590_para018},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(auo_nt35590_para019), auo_nt35590_para019},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(auo_nt35590_para020), auo_nt35590_para020},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(auo_nt35590_para021), auo_nt35590_para021},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(auo_nt35590_para022), auo_nt35590_para022},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(auo_nt35590_para023), auo_nt35590_para023},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(auo_nt35590_para024), auo_nt35590_para024},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(auo_nt35590_para025), auo_nt35590_para025},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(auo_nt35590_para026), auo_nt35590_para026},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(auo_nt35590_para027), auo_nt35590_para027},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(auo_nt35590_para028), auo_nt35590_para028},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(auo_nt35590_para029), auo_nt35590_para029},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(auo_nt35590_para030), auo_nt35590_para030},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(auo_nt35590_para031), auo_nt35590_para031},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(auo_nt35590_para032), auo_nt35590_para032},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(auo_nt35590_para033), auo_nt35590_para033},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(auo_nt35590_para034), auo_nt35590_para034},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(auo_nt35590_para035), auo_nt35590_para035},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(auo_nt35590_para036), auo_nt35590_para036},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(auo_nt35590_para037), auo_nt35590_para037},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(auo_nt35590_para038), auo_nt35590_para038},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(auo_nt35590_para039), auo_nt35590_para039},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(auo_nt35590_para040), auo_nt35590_para040},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(auo_nt35590_para041), auo_nt35590_para041},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(auo_nt35590_para042), auo_nt35590_para042},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(auo_nt35590_para043), auo_nt35590_para043},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(auo_nt35590_para044), auo_nt35590_para044},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(auo_nt35590_para045), auo_nt35590_para045},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(auo_nt35590_para046), auo_nt35590_para046},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(auo_nt35590_para047), auo_nt35590_para047},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(auo_nt35590_para048), auo_nt35590_para048},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(auo_nt35590_para049), auo_nt35590_para049},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(auo_nt35590_para050), auo_nt35590_para050},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(auo_nt35590_para051), auo_nt35590_para051},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(auo_nt35590_para052), auo_nt35590_para052},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(auo_nt35590_para053), auo_nt35590_para053},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(auo_nt35590_para054), auo_nt35590_para054},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(auo_nt35590_para055), auo_nt35590_para055},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(auo_nt35590_para056), auo_nt35590_para056},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(auo_nt35590_para057), auo_nt35590_para057},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(auo_nt35590_para058), auo_nt35590_para058},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(auo_nt35590_para059), auo_nt35590_para059},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(auo_nt35590_para060), auo_nt35590_para060},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(auo_nt35590_para061), auo_nt35590_para061},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(auo_nt35590_para062), auo_nt35590_para062},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(auo_nt35590_para063), auo_nt35590_para063},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(auo_nt35590_para064), auo_nt35590_para064},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(auo_nt35590_para065), auo_nt35590_para065},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(auo_nt35590_para066), auo_nt35590_para066},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(auo_nt35590_para067), auo_nt35590_para067},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(auo_nt35590_para068), auo_nt35590_para068},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(auo_nt35590_para069), auo_nt35590_para069},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(auo_nt35590_para070), auo_nt35590_para070},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(auo_nt35590_para071), auo_nt35590_para071},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(auo_nt35590_para072), auo_nt35590_para072},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(auo_nt35590_para073), auo_nt35590_para073},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(auo_nt35590_para074), auo_nt35590_para074},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(auo_nt35590_para075), auo_nt35590_para075},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(auo_nt35590_para076), auo_nt35590_para076},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(auo_nt35590_para077), auo_nt35590_para077},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(auo_nt35590_para078), auo_nt35590_para078},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(auo_nt35590_para079), auo_nt35590_para079},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(auo_nt35590_para080), auo_nt35590_para080},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(auo_nt35590_para081), auo_nt35590_para081},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(auo_nt35590_para082), auo_nt35590_para082},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(auo_nt35590_para083), auo_nt35590_para083},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(auo_nt35590_para084), auo_nt35590_para084},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(auo_nt35590_para085), auo_nt35590_para085},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(auo_nt35590_para086), auo_nt35590_para086},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(auo_nt35590_para087), auo_nt35590_para087},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(auo_nt35590_para088), auo_nt35590_para088},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(auo_nt35590_para089), auo_nt35590_para089},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(auo_nt35590_para090), auo_nt35590_para090},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(auo_nt35590_para091), auo_nt35590_para091},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(auo_nt35590_para092), auo_nt35590_para092},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(auo_nt35590_para093), auo_nt35590_para093},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(auo_nt35590_para094), auo_nt35590_para094},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(auo_nt35590_para095), auo_nt35590_para095},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(auo_nt35590_para096), auo_nt35590_para096},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(auo_nt35590_para097), auo_nt35590_para097},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(auo_nt35590_para098), auo_nt35590_para098},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(auo_nt35590_para099), auo_nt35590_para099},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(auo_nt35590_para100), auo_nt35590_para100},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cabc_cmd_C1_51), cabc_cmd_C1_51},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cabc_cmd_C1_53), cabc_cmd_C1_53},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cabc_cmd_C1_55), cabc_cmd_C1_55},
	{DTYPE_DCS_WRITE, 1, 0, 0, 120, sizeof(exit_sleep), exit_sleep},
	{DTYPE_DCS_WRITE, 1, 0, 0, 10, sizeof(display_on), display_on},
};

static char tm_nt35590_para001[]={0xFF,0x00};
static char tm_nt35590_para002[]={0xFB,0x01};
static char tm_nt35590_para003[]={0x3B,0x03,0x06,0x03,0x02,0x02};
static char tm_nt35590_para004[]={0xFF,0x01};
static char tm_nt35590_para005[]={0xFB,0x01};
static char tm_nt35590_para006[]={0x00,0x3A};
static char tm_nt35590_para007[]={0x01,0x33};
static char tm_nt35590_para008[]={0x08,0x66};
static char tm_nt35590_para009[]={0x09,0x8F};
static char tm_nt35590_para010[]={0x0B,0xC2};
static char tm_nt35590_para011[]={0x0C,0xC2};
static char tm_nt35590_para012[]={0x0D,0x24};
static char tm_nt35590_para013[]={0x0E,0x33};
static char tm_nt35590_para014[]={0x11,0x94};
static char tm_nt35590_para015[]={0x12,0x03};
static char tm_nt35590_para016[]={0x0F,0x0A};
static char tm_nt35590_para017[]={0x36,0x73};
static char tm_nt35590_para018[]={0x71,0x2C};
static char tm_nt35590_para019[]={0xFF,0x05};
static char tm_nt35590_para020[]={0xFB,0x01};
static char tm_nt35590_para021[]={0x01,0x00};
static char tm_nt35590_para022[]={0x02,0x8D};
static char tm_nt35590_para023[]={0x03,0x8D};
static char tm_nt35590_para024[]={0x04,0x8D};
static char tm_nt35590_para025[]={0x05,0x30};
static char tm_nt35590_para026[]={0x06,0x33};
static char tm_nt35590_para027[]={0x07,0x77};
static char tm_nt35590_para028[]={0x08,0x00};
static char tm_nt35590_para029[]={0x09,0x00};
static char tm_nt35590_para030[]={0x0A,0x00};
static char tm_nt35590_para031[]={0x0B,0x80};
static char tm_nt35590_para032[]={0x0C,0xC8};
static char tm_nt35590_para033[]={0x0D,0x0A};
static char tm_nt35590_para034[]={0x0E,0x1B};
static char tm_nt35590_para035[]={0x0F,0x06};
static char tm_nt35590_para036[]={0x10,0x56};
static char tm_nt35590_para037[]={0x11,0x00};
static char tm_nt35590_para038[]={0x12,0x00};
static char tm_nt35590_para039[]={0x13,0x1E};
static char tm_nt35590_para040[]={0x14,0x00};
static char tm_nt35590_para041[]={0x15,0x1A};
static char tm_nt35590_para042[]={0x16,0x05};
static char tm_nt35590_para043[]={0x17,0x00};
static char tm_nt35590_para044[]={0x18,0x1E};
static char tm_nt35590_para045[]={0x19,0xFF};
static char tm_nt35590_para046[]={0x1A,0x00};
static char tm_nt35590_para047[]={0x1B,0xFC};
static char tm_nt35590_para048[]={0x1C,0x80};
static char tm_nt35590_para049[]={0x1D,0x00};
static char tm_nt35590_para050[]={0x1E,0x00};
static char tm_nt35590_para051[]={0x1F,0x77};
static char tm_nt35590_para052[]={0x20,0x00};
static char tm_nt35590_para053[]={0x21,0x00};
static char tm_nt35590_para054[]={0x22,0x55};
static char tm_nt35590_para055[]={0x23,0x0D};
static char tm_nt35590_para056[]={0x31,0xA0};
static char tm_nt35590_para057[]={0x32,0x00};
static char tm_nt35590_para058[]={0x33,0xB8};
static char tm_nt35590_para059[]={0x34,0xBB};
static char tm_nt35590_para060[]={0x35,0x11};
static char tm_nt35590_para061[]={0x36,0x02};
static char tm_nt35590_para062[]={0x37,0x00};
static char tm_nt35590_para063[]={0x38,0x01};
static char tm_nt35590_para064[]={0x39,0x0B};
static char tm_nt35590_para065[]={0x44,0x08};
static char tm_nt35590_para066[]={0x45,0x80};
static char tm_nt35590_para067[]={0x46,0xCC};
static char tm_nt35590_para068[]={0x47,0x04};
static char tm_nt35590_para069[]={0x48,0x00};
static char tm_nt35590_para070[]={0x49,0x00};
static char tm_nt35590_para071[]={0x4A,0x01};
static char tm_nt35590_para072[]={0x6C,0x03};
static char tm_nt35590_para073[]={0x6D,0x03};
static char tm_nt35590_para074[]={0x6E,0x2F};
static char tm_nt35590_para075[]={0x43,0x00};
static char tm_nt35590_para076[]={0x4B,0x23};
static char tm_nt35590_para077[]={0x4C,0x01};
static char tm_nt35590_para078[]={0x50,0x23};
static char tm_nt35590_para079[]={0x51,0x01};
static char tm_nt35590_para080[]={0x58,0x23};
static char tm_nt35590_para081[]={0x59,0x01};
static char tm_nt35590_para082[]={0x5D,0x23};
static char tm_nt35590_para083[]={0x5E,0x01};
static char tm_nt35590_para084[]={0x89,0x00};
static char tm_nt35590_para085[]={0x8D,0x01};
static char tm_nt35590_para086[]={0x8E,0x64};
static char tm_nt35590_para087[]={0x8F,0x20};
static char tm_nt35590_para088[]={0x97,0x8E};
static char tm_nt35590_para089[]={0x82,0x8C};
static char tm_nt35590_para090[]={0x83,0x02};
static char tm_nt35590_para091[]={0xBB,0x0A};
static char tm_nt35590_para092[]={0xBC,0x0A};
static char tm_nt35590_para093[]={0x24,0x25};
static char tm_nt35590_para094[]={0x25,0x55};
static char tm_nt35590_para095[]={0x26,0x05};
static char tm_nt35590_para096[]={0x27,0x23};
static char tm_nt35590_para097[]={0x28,0x01};
static char tm_nt35590_para098[]={0x29,0x31};
static char tm_nt35590_para099[]={0x2A,0x5D};
static char tm_nt35590_para100[]={0x2B,0x01};
static char tm_nt35590_para101[]={0x2F,0x00};
static char tm_nt35590_para102[]={0x30,0x10};
static char tm_nt35590_para103[]={0xA7,0x12};
static char tm_nt35590_para104[]={0x2D,0x03};
static char tm_nt35590_para105[]={0xFF,0x00};
static char tm_nt35590_para106[]={0xFF,0x01};
static char tm_nt35590_para107[]={0x75,0x01};
static char tm_nt35590_para108[]={0x76,0x3E};
static char tm_nt35590_para109[]={0x77,0x01};
static char tm_nt35590_para110[]={0x78,0x40};
static char tm_nt35590_para111[]={0x79,0x01};
static char tm_nt35590_para112[]={0x7A,0x42};
static char tm_nt35590_para113[]={0x7B,0x01};
static char tm_nt35590_para114[]={0x7C,0x50};
static char tm_nt35590_para115[]={0x7D,0x01};
static char tm_nt35590_para116[]={0x7E,0x55};
static char tm_nt35590_para117[]={0x7F,0x01};
static char tm_nt35590_para118[]={0x80,0x58};
static char tm_nt35590_para119[]={0x81,0x01};
static char tm_nt35590_para120[]={0x82,0x60};
static char tm_nt35590_para121[]={0x83,0x01};
static char tm_nt35590_para122[]={0x84,0x6A};
static char tm_nt35590_para123[]={0x85,0x01};
static char tm_nt35590_para124[]={0x86,0x6E};
static char tm_nt35590_para125[]={0x87,0x01};
static char tm_nt35590_para126[]={0x88,0x8A};
static char tm_nt35590_para127[]={0x89,0x01};
static char tm_nt35590_para128[]={0x8A,0x9D};
static char tm_nt35590_para129[]={0x8B,0x01};
static char tm_nt35590_para130[]={0x8C,0xC7};
static char tm_nt35590_para131[]={0x8D,0x01};
static char tm_nt35590_para132[]={0x8E,0xEA};
static char tm_nt35590_para133[]={0x8F,0x02};
static char tm_nt35590_para134[]={0x90,0x20};
static char tm_nt35590_para135[]={0x91,0x02};
static char tm_nt35590_para136[]={0x92,0x4E};
static char tm_nt35590_para137[]={0x93,0x02};
static char tm_nt35590_para138[]={0x94,0x51};
static char tm_nt35590_para139[]={0x95,0x02};
static char tm_nt35590_para140[]={0x96,0x81};
static char tm_nt35590_para141[]={0x97,0x02};
static char tm_nt35590_para142[]={0x98,0xB9};
static char tm_nt35590_para143[]={0x99,0x02};
static char tm_nt35590_para144[]={0x9A,0xDD};
static char tm_nt35590_para145[]={0x9B,0x03};
static char tm_nt35590_para146[]={0x9C,0x0F};
static char tm_nt35590_para147[]={0x9D,0x03};
static char tm_nt35590_para148[]={0x9E,0x2D};
static char tm_nt35590_para149[]={0x9F,0x03};
static char tm_nt35590_para150[]={0xA0,0x61};
static char tm_nt35590_para151[]={0xA2,0x03};
static char tm_nt35590_para152[]={0xA3,0x75};
static char tm_nt35590_para153[]={0xA4,0x03};
static char tm_nt35590_para154[]={0xA5,0x89};
static char tm_nt35590_para155[]={0xA6,0x03};
static char tm_nt35590_para156[]={0xA7,0xAD};
static char tm_nt35590_para157[]={0xA9,0x03};
static char tm_nt35590_para158[]={0xAA,0xC7};
static char tm_nt35590_para159[]={0xAB,0x03};
static char tm_nt35590_para160[]={0xAC,0xD7};
static char tm_nt35590_para161[]={0xAD,0x03};
static char tm_nt35590_para162[]={0xAE,0xE2};
static char tm_nt35590_para163[]={0xAF,0x03};
static char tm_nt35590_para164[]={0xB0,0xEB};
static char tm_nt35590_para165[]={0xB1,0x03};
static char tm_nt35590_para166[]={0xB2,0xF2};
static char tm_nt35590_para167[]={0xB3,0x01};
static char tm_nt35590_para168[]={0xB4,0x3E};
static char tm_nt35590_para169[]={0xB5,0x01};
static char tm_nt35590_para170[]={0xB6,0x40};
static char tm_nt35590_para171[]={0xB7,0x01};
static char tm_nt35590_para172[]={0xB8,0x42};
static char tm_nt35590_para173[]={0xB9,0x01};
static char tm_nt35590_para174[]={0xBA,0x50};
static char tm_nt35590_para175[]={0xBB,0x01};
static char tm_nt35590_para176[]={0xBC,0x55};
static char tm_nt35590_para177[]={0xBD,0x01};
static char tm_nt35590_para178[]={0xBE,0x58};
static char tm_nt35590_para179[]={0xBF,0x01};
static char tm_nt35590_para180[]={0xC0,0x60};
static char tm_nt35590_para181[]={0xC1,0x01};
static char tm_nt35590_para182[]={0xC2,0x6A};
static char tm_nt35590_para183[]={0xC3,0x01};
static char tm_nt35590_para184[]={0xC4,0x6E};
static char tm_nt35590_para185[]={0xC5,0x01};
static char tm_nt35590_para186[]={0xC6,0x8A};
static char tm_nt35590_para187[]={0xC7,0x01};
static char tm_nt35590_para188[]={0xC8,0x9D};
static char tm_nt35590_para189[]={0xC9,0x01};
static char tm_nt35590_para190[]={0xCA,0xC7};
static char tm_nt35590_para191[]={0xCB,0x01};
static char tm_nt35590_para192[]={0xCC,0xEA};
static char tm_nt35590_para193[]={0xCD,0x02};
static char tm_nt35590_para194[]={0xCE,0x20};
static char tm_nt35590_para195[]={0xCF,0x02};
static char tm_nt35590_para196[]={0xD0,0x4E};
static char tm_nt35590_para197[]={0xD1,0x02};
static char tm_nt35590_para198[]={0xD2,0x51};
static char tm_nt35590_para199[]={0xD3,0x02};
static char tm_nt35590_para200[]={0xD4,0x81};
static char tm_nt35590_para201[]={0xD5,0x02};
static char tm_nt35590_para202[]={0xD6,0xB9};
static char tm_nt35590_para203[]={0xD7,0x02};
static char tm_nt35590_para204[]={0xD8,0xDD};
static char tm_nt35590_para205[]={0xD9,0x03};
static char tm_nt35590_para206[]={0xDA,0x0F};
static char tm_nt35590_para207[]={0xDB,0x03};
static char tm_nt35590_para208[]={0xDC,0x2D};
static char tm_nt35590_para209[]={0xDD,0x03};
static char tm_nt35590_para210[]={0xDE,0x61};
static char tm_nt35590_para211[]={0xDF,0x03};
static char tm_nt35590_para212[]={0xE0,0x75};
static char tm_nt35590_para213[]={0xE1,0x03};
static char tm_nt35590_para214[]={0xE2,0x89};
static char tm_nt35590_para215[]={0xE3,0x03};
static char tm_nt35590_para216[]={0xE4,0xAD};
static char tm_nt35590_para217[]={0xE5,0x03};
static char tm_nt35590_para218[]={0xE6,0xC7};
static char tm_nt35590_para219[]={0xE7,0x03};
static char tm_nt35590_para220[]={0xE8,0xD7};
static char tm_nt35590_para221[]={0xE9,0x03};
static char tm_nt35590_para222[]={0xEA,0xE2};
static char tm_nt35590_para223[]={0xEB,0x03};
static char tm_nt35590_para224[]={0xEC,0xEB};
static char tm_nt35590_para225[]={0xED,0x03};
static char tm_nt35590_para226[]={0xEE,0xF2};
static char tm_nt35590_para227[]={0xEF,0x01};
static char tm_nt35590_para228[]={0xF0,0x3E};
static char tm_nt35590_para229[]={0xF1,0x01};
static char tm_nt35590_para230[]={0xF2,0x40};
static char tm_nt35590_para231[]={0xF3,0x01};
static char tm_nt35590_para232[]={0xF4,0x42};
static char tm_nt35590_para233[]={0xF5,0x01};
static char tm_nt35590_para234[]={0xF6,0x50};
static char tm_nt35590_para235[]={0xF7,0x01};
static char tm_nt35590_para236[]={0xF8,0x55};
static char tm_nt35590_para237[]={0xF9,0x01};
static char tm_nt35590_para238[]={0xFA,0x58};
static char tm_nt35590_para239[]={0xFF,0x00};
static char tm_nt35590_para240[]={0xFF,0x02};
static char tm_nt35590_para241[]={0x00,0x01};
static char tm_nt35590_para242[]={0x01,0x60};
static char tm_nt35590_para243[]={0x02,0x01};
static char tm_nt35590_para244[]={0x03,0x6A};
static char tm_nt35590_para245[]={0x04,0x01};
static char tm_nt35590_para246[]={0x05,0x6E};
static char tm_nt35590_para247[]={0x06,0x01};
static char tm_nt35590_para248[]={0x07,0x8A};
static char tm_nt35590_para249[]={0x08,0x01};
static char tm_nt35590_para250[]={0x09,0x9D};
static char tm_nt35590_para251[]={0x0A,0x01};
static char tm_nt35590_para252[]={0x0B,0xC7};
static char tm_nt35590_para253[]={0x0C,0x01};
static char tm_nt35590_para254[]={0x0D,0xEA};
static char tm_nt35590_para255[]={0x0E,0x02};
static char tm_nt35590_para256[]={0x0F,0x20};
static char tm_nt35590_para257[]={0x10,0x02};
static char tm_nt35590_para258[]={0x11,0x4E};
static char tm_nt35590_para259[]={0x12,0x02};
static char tm_nt35590_para260[]={0x13,0x51};
static char tm_nt35590_para261[]={0x14,0x02};
static char tm_nt35590_para262[]={0x15,0x81};
static char tm_nt35590_para263[]={0x16,0x02};
static char tm_nt35590_para264[]={0x17,0xB9};
static char tm_nt35590_para265[]={0x18,0x02};
static char tm_nt35590_para266[]={0x19,0xDD};
static char tm_nt35590_para267[]={0x1A,0x03};
static char tm_nt35590_para268[]={0x1B,0x0F};
static char tm_nt35590_para269[]={0x1C,0x03};
static char tm_nt35590_para270[]={0x1D,0x2D};
static char tm_nt35590_para271[]={0xAE,0x03};
static char tm_nt35590_para272[]={0x1F,0x61};
static char tm_nt35590_para273[]={0x20,0x03};
static char tm_nt35590_para274[]={0x21,0x75};
static char tm_nt35590_para275[]={0x22,0x03};
static char tm_nt35590_para276[]={0x23,0x89};
static char tm_nt35590_para277[]={0x24,0x03};
static char tm_nt35590_para278[]={0x25,0xAD};
static char tm_nt35590_para279[]={0x26,0x03};
static char tm_nt35590_para280[]={0x27,0xC7};
static char tm_nt35590_para281[]={0x28,0x03};
static char tm_nt35590_para282[]={0x29,0xD7};
static char tm_nt35590_para283[]={0x2A,0x03};
static char tm_nt35590_para284[]={0x2B,0xE2};
static char tm_nt35590_para285[]={0x2D,0x03};
static char tm_nt35590_para286[]={0x2F,0xEB};
static char tm_nt35590_para287[]={0x30,0x03};
static char tm_nt35590_para288[]={0x31,0xF2};
static char tm_nt35590_para289[]={0x32,0x01};
static char tm_nt35590_para290[]={0x33,0x3E};
static char tm_nt35590_para291[]={0x34,0x01};
static char tm_nt35590_para292[]={0x35,0x40};
static char tm_nt35590_para293[]={0x36,0x01};
static char tm_nt35590_para294[]={0x37,0x42};
static char tm_nt35590_para295[]={0x38,0x01};
static char tm_nt35590_para296[]={0x39,0x50};
static char tm_nt35590_para297[]={0x3A,0x01};
static char tm_nt35590_para298[]={0x3B,0x55};
static char tm_nt35590_para299[]={0x3D,0x01};
static char tm_nt35590_para300[]={0x3F,0x58};
static char tm_nt35590_para301[]={0x40,0x01};
static char tm_nt35590_para302[]={0x41,0x60};
static char tm_nt35590_para303[]={0x42,0x01};
static char tm_nt35590_para304[]={0x43,0x6A};
static char tm_nt35590_para305[]={0x44,0x01};
static char tm_nt35590_para306[]={0x45,0x6E};
static char tm_nt35590_para307[]={0x46,0x01};
static char tm_nt35590_para308[]={0x47,0x8A};
static char tm_nt35590_para309[]={0x48,0x01};
static char tm_nt35590_para310[]={0x49,0x9D};
static char tm_nt35590_para311[]={0x4A,0x01};
static char tm_nt35590_para312[]={0x4B,0xC7};
static char tm_nt35590_para313[]={0x4C,0x01};
static char tm_nt35590_para314[]={0x4D,0xEA};
static char tm_nt35590_para315[]={0x4E,0x02};
static char tm_nt35590_para316[]={0x4F,0x20};
static char tm_nt35590_para317[]={0x50,0x02};
static char tm_nt35590_para318[]={0x51,0x4E};
static char tm_nt35590_para319[]={0x52,0x02};
static char tm_nt35590_para320[]={0x53,0x51};
static char tm_nt35590_para321[]={0x54,0x02};
static char tm_nt35590_para322[]={0x55,0x81};
static char tm_nt35590_para323[]={0x56,0x02};
static char tm_nt35590_para324[]={0x58,0xB9};
static char tm_nt35590_para325[]={0x59,0x02};
static char tm_nt35590_para326[]={0x5A,0xDD};
static char tm_nt35590_para327[]={0x5B,0x03};
static char tm_nt35590_para328[]={0x5C,0x0F};
static char tm_nt35590_para329[]={0x5D,0x03};
static char tm_nt35590_para330[]={0x5E,0x2D};
static char tm_nt35590_para331[]={0x5F,0x03};
static char tm_nt35590_para332[]={0x60,0x61};
static char tm_nt35590_para333[]={0x61,0x03};
static char tm_nt35590_para334[]={0x62,0x75};
static char tm_nt35590_para335[]={0x63,0x03};
static char tm_nt35590_para336[]={0x64,0x89};
static char tm_nt35590_para337[]={0x65,0x03};
static char tm_nt35590_para338[]={0x66,0xAD};
static char tm_nt35590_para339[]={0x67,0x03};
static char tm_nt35590_para340[]={0x68,0xC7};
static char tm_nt35590_para341[]={0x69,0x03};
static char tm_nt35590_para342[]={0x6A,0xD7};
static char tm_nt35590_para343[]={0x6B,0x03};
static char tm_nt35590_para344[]={0x6C,0xE2};
static char tm_nt35590_para345[]={0x6D,0x03};
static char tm_nt35590_para346[]={0x6E,0xEB};
static char tm_nt35590_para347[]={0x6F,0x03};
static char tm_nt35590_para348[]={0x70,0xF2};
static char tm_nt35590_para349[]={0x71,0x01};
static char tm_nt35590_para350[]={0x72,0x3E};
static char tm_nt35590_para351[]={0x73,0x01};
static char tm_nt35590_para352[]={0x74,0x40};
static char tm_nt35590_para353[]={0x75,0x01};
static char tm_nt35590_para354[]={0x76,0x42};
static char tm_nt35590_para355[]={0x77,0x01};
static char tm_nt35590_para356[]={0x78,0x50};
static char tm_nt35590_para357[]={0x79,0x01};
static char tm_nt35590_para358[]={0x7A,0x55};
static char tm_nt35590_para359[]={0x7B,0x01};
static char tm_nt35590_para360[]={0x7C,0x58};
static char tm_nt35590_para361[]={0x7D,0x01};
static char tm_nt35590_para362[]={0x7E,0x60};
static char tm_nt35590_para363[]={0x7F,0x01};
static char tm_nt35590_para364[]={0x80,0x6A};
static char tm_nt35590_para365[]={0x81,0x01};
static char tm_nt35590_para366[]={0x82,0x6E};
static char tm_nt35590_para367[]={0x83,0x01};
static char tm_nt35590_para368[]={0x84,0x8A};
static char tm_nt35590_para369[]={0x85,0x01};
static char tm_nt35590_para370[]={0x86,0x9D};
static char tm_nt35590_para371[]={0x87,0x01};
static char tm_nt35590_para372[]={0x88,0xC7};
static char tm_nt35590_para373[]={0x89,0x01};
static char tm_nt35590_para374[]={0x8A,0xEA};
static char tm_nt35590_para375[]={0x8B,0x02};
static char tm_nt35590_para376[]={0x8C,0x20};
static char tm_nt35590_para377[]={0x8D,0x02};
static char tm_nt35590_para378[]={0x8E,0x4E};
static char tm_nt35590_para379[]={0x8F,0x02};
static char tm_nt35590_para380[]={0x90,0x51};
static char tm_nt35590_para381[]={0x91,0x02};
static char tm_nt35590_para382[]={0x92,0x81};
static char tm_nt35590_para383[]={0x93,0x02};
static char tm_nt35590_para384[]={0x94,0xB9};
static char tm_nt35590_para385[]={0x95,0x02};
static char tm_nt35590_para386[]={0x96,0xDD};
static char tm_nt35590_para387[]={0x97,0x03};
static char tm_nt35590_para388[]={0x98,0x0F};
static char tm_nt35590_para389[]={0x99,0x03};
static char tm_nt35590_para390[]={0x9A,0x2D};
static char tm_nt35590_para391[]={0x9B,0x03};
static char tm_nt35590_para392[]={0x9C,0x61};
static char tm_nt35590_para393[]={0x9D,0x03};
static char tm_nt35590_para394[]={0x9E,0x75};
static char tm_nt35590_para395[]={0x9F,0x03};
static char tm_nt35590_para396[]={0xA0,0x89};
static char tm_nt35590_para397[]={0xA2,0x03};
static char tm_nt35590_para398[]={0xA3,0xAD};
static char tm_nt35590_para399[]={0xA4,0x03};
static char tm_nt35590_para400[]={0xA5,0xC7};
static char tm_nt35590_para401[]={0xA6,0x03};
static char tm_nt35590_para402[]={0xA7,0xD7};
static char tm_nt35590_para403[]={0xA9,0x03};
static char tm_nt35590_para404[]={0xAA,0xE2};
static char tm_nt35590_para405[]={0xAB,0x03};
static char tm_nt35590_para406[]={0xAC,0xEB};
static char tm_nt35590_para407[]={0xAD,0x03};
static char tm_nt35590_para408[]={0xAE,0xF2};
static char tm_nt35590_para409[]={0xAF,0x01};
static char tm_nt35590_para410[]={0xB0,0x3E};
static char tm_nt35590_para411[]={0xB1,0x01};
static char tm_nt35590_para412[]={0xB2,0x40};
static char tm_nt35590_para413[]={0xB3,0x01};
static char tm_nt35590_para414[]={0xB4,0x42};
static char tm_nt35590_para415[]={0xB5,0x01};
static char tm_nt35590_para416[]={0xB6,0x50};
static char tm_nt35590_para417[]={0xB7,0x01};
static char tm_nt35590_para418[]={0xB8,0x55};
static char tm_nt35590_para419[]={0xB9,0x01};
static char tm_nt35590_para420[]={0xBA,0x58};
static char tm_nt35590_para421[]={0xBB,0x01};
static char tm_nt35590_para422[]={0xBC,0x60};
static char tm_nt35590_para423[]={0xBD,0x01};
static char tm_nt35590_para424[]={0xBE,0x6A};
static char tm_nt35590_para425[]={0xBF,0x01};
static char tm_nt35590_para426[]={0xC0,0x6E};
static char tm_nt35590_para427[]={0xC1,0x01};
static char tm_nt35590_para428[]={0xC2,0x8A};
static char tm_nt35590_para429[]={0xC3,0x01};
static char tm_nt35590_para430[]={0xC4,0x9D};
static char tm_nt35590_para431[]={0xC5,0x01};
static char tm_nt35590_para432[]={0xC6,0xC7};
static char tm_nt35590_para433[]={0xC7,0x01};
static char tm_nt35590_para434[]={0xC8,0xEA};
static char tm_nt35590_para435[]={0xC9,0x02};
static char tm_nt35590_para436[]={0xCA,0x20};
static char tm_nt35590_para437[]={0xCB,0x02};
static char tm_nt35590_para438[]={0xCC,0x4E};
static char tm_nt35590_para439[]={0xCD,0x02};
static char tm_nt35590_para440[]={0xCE,0x51};
static char tm_nt35590_para441[]={0xCF,0x02};
static char tm_nt35590_para442[]={0xD0,0x81};
static char tm_nt35590_para443[]={0xD1,0x02};
static char tm_nt35590_para444[]={0xD2,0xB9};
static char tm_nt35590_para445[]={0xD3,0x02};
static char tm_nt35590_para446[]={0xD4,0xDD};
static char tm_nt35590_para447[]={0xD5,0x03};
static char tm_nt35590_para448[]={0xD6,0x0F};
static char tm_nt35590_para449[]={0xD7,0x03};
static char tm_nt35590_para450[]={0xD8,0x2D};
static char tm_nt35590_para451[]={0xD9,0x03};
static char tm_nt35590_para452[]={0xDA,0x61};
static char tm_nt35590_para453[]={0xDB,0x03};
static char tm_nt35590_para454[]={0xDC,0x75};
static char tm_nt35590_para455[]={0xDD,0x03};
static char tm_nt35590_para456[]={0xDE,0x89};
static char tm_nt35590_para457[]={0xDF,0x03};
static char tm_nt35590_para458[]={0xE0,0xAD};
static char tm_nt35590_para459[]={0xE1,0x03};
static char tm_nt35590_para460[]={0xE2,0xC7};
static char tm_nt35590_para461[]={0xE3,0x03};
static char tm_nt35590_para462[]={0xE4,0xD7};
static char tm_nt35590_para463[]={0xE5,0x03};
static char tm_nt35590_para464[]={0xE6,0xE2};
static char tm_nt35590_para465[]={0xE7,0x03};
static char tm_nt35590_para466[]={0xE8,0xEB};
static char tm_nt35590_para467[]={0xE9,0x03};
static char tm_nt35590_para468[]={0xEA,0xF2};
static char tm_nt35590_para469[]={0xFF,0x00};
static char tm_nt35590_para470[]={0xFB,0x01};
static char tm_nt35590_para471[]={0xC2,0x08};
static char tm_nt35590_para472[]={0xBA,0x03};

static struct dsi_cmd_desc tm_nt35590_display_on_cmds[] = {
	
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para001), tm_nt35590_para001},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(tm_nt35590_para002), tm_nt35590_para002},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para003), tm_nt35590_para003},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para004), tm_nt35590_para004},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para005), tm_nt35590_para005},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para006), tm_nt35590_para006},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para007), tm_nt35590_para007},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para008), tm_nt35590_para008},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para009), tm_nt35590_para009},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para010), tm_nt35590_para010},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para011), tm_nt35590_para011},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para012), tm_nt35590_para012},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para013), tm_nt35590_para013},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para014), tm_nt35590_para014},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para015), tm_nt35590_para015},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para016), tm_nt35590_para016},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para017), tm_nt35590_para017},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para018), tm_nt35590_para018},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para019), tm_nt35590_para019},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para020), tm_nt35590_para020},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para021), tm_nt35590_para021},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para022), tm_nt35590_para022},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para023), tm_nt35590_para023},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para024), tm_nt35590_para024},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para025), tm_nt35590_para025},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para026), tm_nt35590_para026},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para027), tm_nt35590_para027},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para028), tm_nt35590_para028},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para029), tm_nt35590_para029},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para030), tm_nt35590_para030},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para031), tm_nt35590_para031},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para032), tm_nt35590_para032},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para033), tm_nt35590_para033},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para034), tm_nt35590_para034},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para035), tm_nt35590_para035},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para036), tm_nt35590_para036},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para037), tm_nt35590_para037},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para038), tm_nt35590_para038},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para039), tm_nt35590_para039},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para040), tm_nt35590_para040},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para041), tm_nt35590_para041},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para042), tm_nt35590_para042},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para043), tm_nt35590_para043},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para044), tm_nt35590_para044},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para045), tm_nt35590_para045},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para046), tm_nt35590_para046},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para047), tm_nt35590_para047},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para048), tm_nt35590_para048},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para049), tm_nt35590_para049},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para050), tm_nt35590_para050},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para051), tm_nt35590_para051},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para052), tm_nt35590_para052},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para053), tm_nt35590_para053},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para054), tm_nt35590_para054},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para055), tm_nt35590_para055},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para056), tm_nt35590_para056},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para057), tm_nt35590_para057},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para058), tm_nt35590_para058},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para059), tm_nt35590_para059},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para060), tm_nt35590_para060},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para061), tm_nt35590_para061},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para062), tm_nt35590_para062},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para063), tm_nt35590_para063},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para064), tm_nt35590_para064},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para065), tm_nt35590_para065},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para066), tm_nt35590_para066},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para067), tm_nt35590_para067},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para068), tm_nt35590_para068},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para069), tm_nt35590_para069},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para070), tm_nt35590_para070},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para071), tm_nt35590_para071},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para072), tm_nt35590_para072},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para073), tm_nt35590_para073},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para074), tm_nt35590_para074},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para075), tm_nt35590_para075},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para076), tm_nt35590_para076},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para077), tm_nt35590_para077},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para078), tm_nt35590_para078},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para079), tm_nt35590_para079},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para080), tm_nt35590_para080},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para081), tm_nt35590_para081},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para082), tm_nt35590_para082},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para083), tm_nt35590_para083},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para084), tm_nt35590_para084},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para085), tm_nt35590_para085},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para086), tm_nt35590_para086},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para087), tm_nt35590_para087},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para088), tm_nt35590_para088},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para089), tm_nt35590_para089},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para090), tm_nt35590_para090},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para091), tm_nt35590_para091},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para092), tm_nt35590_para092},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para093), tm_nt35590_para093},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para094), tm_nt35590_para094},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para095), tm_nt35590_para095},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para096), tm_nt35590_para096},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para097), tm_nt35590_para097},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para098), tm_nt35590_para098},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para099), tm_nt35590_para099},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para100), tm_nt35590_para100},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para101), tm_nt35590_para101},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para102), tm_nt35590_para102},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para103), tm_nt35590_para103},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para104), tm_nt35590_para104},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para105), tm_nt35590_para105},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para106), tm_nt35590_para106},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para107), tm_nt35590_para107},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para108), tm_nt35590_para108},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para109), tm_nt35590_para109},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para110), tm_nt35590_para110},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para111), tm_nt35590_para111},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para112), tm_nt35590_para112},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para113), tm_nt35590_para113},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para114), tm_nt35590_para114},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para115), tm_nt35590_para115},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para116), tm_nt35590_para116},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para117), tm_nt35590_para117},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para118), tm_nt35590_para118},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para119), tm_nt35590_para119},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para120), tm_nt35590_para120},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para121), tm_nt35590_para121},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para122), tm_nt35590_para122},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para123), tm_nt35590_para123},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para124), tm_nt35590_para124},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para125), tm_nt35590_para125},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para126), tm_nt35590_para126},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para127), tm_nt35590_para127},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para128), tm_nt35590_para128},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para129), tm_nt35590_para129},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para130), tm_nt35590_para130},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para131), tm_nt35590_para131},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para132), tm_nt35590_para132},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para133), tm_nt35590_para133},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para134), tm_nt35590_para134},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para135), tm_nt35590_para135},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para136), tm_nt35590_para136},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para137), tm_nt35590_para137},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para138), tm_nt35590_para138},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para139), tm_nt35590_para139},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para140), tm_nt35590_para140},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para141), tm_nt35590_para141},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para142), tm_nt35590_para142},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para143), tm_nt35590_para143},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para144), tm_nt35590_para144},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para145), tm_nt35590_para145},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para146), tm_nt35590_para146},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para147), tm_nt35590_para147},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para148), tm_nt35590_para148},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para149), tm_nt35590_para149},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para150), tm_nt35590_para150},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para151), tm_nt35590_para151},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para152), tm_nt35590_para152},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para153), tm_nt35590_para153},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para154), tm_nt35590_para154},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para155), tm_nt35590_para155},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para156), tm_nt35590_para156},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para157), tm_nt35590_para157},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para158), tm_nt35590_para158},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para159), tm_nt35590_para159},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para160), tm_nt35590_para160},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para161), tm_nt35590_para161},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para162), tm_nt35590_para162},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para163), tm_nt35590_para163},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para164), tm_nt35590_para164},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para165), tm_nt35590_para165},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para166), tm_nt35590_para166},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para167), tm_nt35590_para167},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para168), tm_nt35590_para168},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para169), tm_nt35590_para169},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para170), tm_nt35590_para170},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para171), tm_nt35590_para171},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para172), tm_nt35590_para172},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para173), tm_nt35590_para173},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para174), tm_nt35590_para174},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para175), tm_nt35590_para175},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para176), tm_nt35590_para176},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para177), tm_nt35590_para177},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para178), tm_nt35590_para178},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para179), tm_nt35590_para179},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para180), tm_nt35590_para180},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para181), tm_nt35590_para181},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para182), tm_nt35590_para182},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para183), tm_nt35590_para183},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para184), tm_nt35590_para184},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para185), tm_nt35590_para185},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para186), tm_nt35590_para186},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para187), tm_nt35590_para187},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para188), tm_nt35590_para188},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para189), tm_nt35590_para189},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para190), tm_nt35590_para190},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para191), tm_nt35590_para191},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para192), tm_nt35590_para192},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para193), tm_nt35590_para193},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para194), tm_nt35590_para194},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para195), tm_nt35590_para195},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para196), tm_nt35590_para196},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para197), tm_nt35590_para197},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para198), tm_nt35590_para198},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para199), tm_nt35590_para199},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para200), tm_nt35590_para200},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para201), tm_nt35590_para201},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para202), tm_nt35590_para202},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para203), tm_nt35590_para203},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para204), tm_nt35590_para204},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para205), tm_nt35590_para205},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para206), tm_nt35590_para206},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para207), tm_nt35590_para207},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para208), tm_nt35590_para208},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para209), tm_nt35590_para209},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para210), tm_nt35590_para210},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para211), tm_nt35590_para211},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para212), tm_nt35590_para212},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para213), tm_nt35590_para213},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para214), tm_nt35590_para214},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para215), tm_nt35590_para215},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para216), tm_nt35590_para216},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para217), tm_nt35590_para217},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para218), tm_nt35590_para218},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para219), tm_nt35590_para219},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para220), tm_nt35590_para220},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para221), tm_nt35590_para221},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para222), tm_nt35590_para222},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para223), tm_nt35590_para223},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para224), tm_nt35590_para224},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para225), tm_nt35590_para225},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para226), tm_nt35590_para226},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para227), tm_nt35590_para227},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para228), tm_nt35590_para228},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para229), tm_nt35590_para229},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para230), tm_nt35590_para230},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para231), tm_nt35590_para231},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para232), tm_nt35590_para232},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para233), tm_nt35590_para233},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para234), tm_nt35590_para234},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para235), tm_nt35590_para235},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para236), tm_nt35590_para236},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para237), tm_nt35590_para237},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para238), tm_nt35590_para238},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para239), tm_nt35590_para239},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para240), tm_nt35590_para240},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para241), tm_nt35590_para241},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para242), tm_nt35590_para242},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para243), tm_nt35590_para243},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para244), tm_nt35590_para244},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para245), tm_nt35590_para245},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para246), tm_nt35590_para246},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para247), tm_nt35590_para247},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para248), tm_nt35590_para248},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para249), tm_nt35590_para249},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para250), tm_nt35590_para250},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para251), tm_nt35590_para251},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para252), tm_nt35590_para252},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para253), tm_nt35590_para253},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para254), tm_nt35590_para254},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para255), tm_nt35590_para255},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para256), tm_nt35590_para256},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para257), tm_nt35590_para257},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para258), tm_nt35590_para258},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para259), tm_nt35590_para259},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para260), tm_nt35590_para260},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para261), tm_nt35590_para261},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para262), tm_nt35590_para262},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para263), tm_nt35590_para263},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para264), tm_nt35590_para264},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para265), tm_nt35590_para265},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para266), tm_nt35590_para266},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para267), tm_nt35590_para267},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para268), tm_nt35590_para268},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para269), tm_nt35590_para269},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para270), tm_nt35590_para270},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para271), tm_nt35590_para271},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para272), tm_nt35590_para272},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para273), tm_nt35590_para273},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para274), tm_nt35590_para274},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para275), tm_nt35590_para275},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para276), tm_nt35590_para276},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para277), tm_nt35590_para277},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para278), tm_nt35590_para278},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para279), tm_nt35590_para279},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para280), tm_nt35590_para280},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para281), tm_nt35590_para281},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para282), tm_nt35590_para282},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para283), tm_nt35590_para283},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para284), tm_nt35590_para284},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para285), tm_nt35590_para285},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para286), tm_nt35590_para286},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para287), tm_nt35590_para287},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para288), tm_nt35590_para288},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para289), tm_nt35590_para289},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para290), tm_nt35590_para290},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para291), tm_nt35590_para291},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para292), tm_nt35590_para292},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para293), tm_nt35590_para293},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para294), tm_nt35590_para294},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para295), tm_nt35590_para295},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para296), tm_nt35590_para296},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para297), tm_nt35590_para297},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para298), tm_nt35590_para298},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para299), tm_nt35590_para299},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para300), tm_nt35590_para300},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para301), tm_nt35590_para301},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para302), tm_nt35590_para302},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para303), tm_nt35590_para303},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para304), tm_nt35590_para304},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para305), tm_nt35590_para305},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para306), tm_nt35590_para306},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para307), tm_nt35590_para307},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para308), tm_nt35590_para308},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para309), tm_nt35590_para309},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para310), tm_nt35590_para310},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para311), tm_nt35590_para311},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para312), tm_nt35590_para312},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para313), tm_nt35590_para313},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para314), tm_nt35590_para314},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para315), tm_nt35590_para315},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para316), tm_nt35590_para316},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para317), tm_nt35590_para317},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para318), tm_nt35590_para318},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para319), tm_nt35590_para319},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para320), tm_nt35590_para320},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para321), tm_nt35590_para321},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para322), tm_nt35590_para322},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para323), tm_nt35590_para323},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para324), tm_nt35590_para324},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para325), tm_nt35590_para325},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para326), tm_nt35590_para326},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para327), tm_nt35590_para327},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para328), tm_nt35590_para328},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para329), tm_nt35590_para329},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para330), tm_nt35590_para330},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para331), tm_nt35590_para331},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para332), tm_nt35590_para332},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para333), tm_nt35590_para333},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para334), tm_nt35590_para334},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para335), tm_nt35590_para335},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para336), tm_nt35590_para336},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para337), tm_nt35590_para337},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para338), tm_nt35590_para338},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para339), tm_nt35590_para339},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para340), tm_nt35590_para340},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para341), tm_nt35590_para341},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para342), tm_nt35590_para342},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para343), tm_nt35590_para343},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para344), tm_nt35590_para344},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para345), tm_nt35590_para345},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para346), tm_nt35590_para346},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para347), tm_nt35590_para347},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para348), tm_nt35590_para348},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para349), tm_nt35590_para349},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para350), tm_nt35590_para350},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para351), tm_nt35590_para351},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para352), tm_nt35590_para352},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para353), tm_nt35590_para353},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para354), tm_nt35590_para354},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para355), tm_nt35590_para355},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para356), tm_nt35590_para356},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para357), tm_nt35590_para357},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para358), tm_nt35590_para358},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para359), tm_nt35590_para359},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para360), tm_nt35590_para360},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para361), tm_nt35590_para361},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para362), tm_nt35590_para362},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para363), tm_nt35590_para363},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para364), tm_nt35590_para364},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para365), tm_nt35590_para365},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para366), tm_nt35590_para366},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para367), tm_nt35590_para367},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para368), tm_nt35590_para368},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para369), tm_nt35590_para369},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para370), tm_nt35590_para370},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para371), tm_nt35590_para371},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para372), tm_nt35590_para372},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para373), tm_nt35590_para373},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para374), tm_nt35590_para374},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para375), tm_nt35590_para375},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para376), tm_nt35590_para376},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para377), tm_nt35590_para377},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para378), tm_nt35590_para378},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para379), tm_nt35590_para379},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para380), tm_nt35590_para380},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para381), tm_nt35590_para381},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para382), tm_nt35590_para382},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para383), tm_nt35590_para383},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para384), tm_nt35590_para384},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para385), tm_nt35590_para385},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para386), tm_nt35590_para386},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para387), tm_nt35590_para387},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para388), tm_nt35590_para388},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para389), tm_nt35590_para389},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para390), tm_nt35590_para390},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para391), tm_nt35590_para391},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para392), tm_nt35590_para392},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para393), tm_nt35590_para393},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para394), tm_nt35590_para394},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para395), tm_nt35590_para395},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para396), tm_nt35590_para396},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para397), tm_nt35590_para397},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para398), tm_nt35590_para398},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para399), tm_nt35590_para399},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para400), tm_nt35590_para400},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para401), tm_nt35590_para401},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para402), tm_nt35590_para402},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para403), tm_nt35590_para403},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para404), tm_nt35590_para404},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para405), tm_nt35590_para405},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para406), tm_nt35590_para406},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para407), tm_nt35590_para407},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para408), tm_nt35590_para408},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para409), tm_nt35590_para409},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para410), tm_nt35590_para410},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para411), tm_nt35590_para411},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para412), tm_nt35590_para412},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para413), tm_nt35590_para413},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para414), tm_nt35590_para414},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para415), tm_nt35590_para415},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para416), tm_nt35590_para416},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para417), tm_nt35590_para417},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para418), tm_nt35590_para418},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para419), tm_nt35590_para419},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para420), tm_nt35590_para420},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para421), tm_nt35590_para421},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para422), tm_nt35590_para422},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para423), tm_nt35590_para423},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para424), tm_nt35590_para424},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para425), tm_nt35590_para425},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para426), tm_nt35590_para426},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para427), tm_nt35590_para427},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para428), tm_nt35590_para428},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para429), tm_nt35590_para429},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para430), tm_nt35590_para430},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para431), tm_nt35590_para431},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para432), tm_nt35590_para432},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para433), tm_nt35590_para433},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para434), tm_nt35590_para434},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para435), tm_nt35590_para435},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para436), tm_nt35590_para436},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para437), tm_nt35590_para437},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para438), tm_nt35590_para438},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para439), tm_nt35590_para439},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para440), tm_nt35590_para440},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para441), tm_nt35590_para441},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para442), tm_nt35590_para442},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para443), tm_nt35590_para443},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para444), tm_nt35590_para444},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para445), tm_nt35590_para445},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para446), tm_nt35590_para446},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para447), tm_nt35590_para447},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para448), tm_nt35590_para448},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para449), tm_nt35590_para449},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para450), tm_nt35590_para450},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para451), tm_nt35590_para451},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para452), tm_nt35590_para452},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para453), tm_nt35590_para453},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para454), tm_nt35590_para454},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para455), tm_nt35590_para455},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para456), tm_nt35590_para456},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para457), tm_nt35590_para457},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para458), tm_nt35590_para458},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para459), tm_nt35590_para459},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para460), tm_nt35590_para460},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para461), tm_nt35590_para461},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para462), tm_nt35590_para462},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para463), tm_nt35590_para463},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para464), tm_nt35590_para464},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para465), tm_nt35590_para465},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para466), tm_nt35590_para466},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para467), tm_nt35590_para467},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para468), tm_nt35590_para468},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para469), tm_nt35590_para469},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para470), tm_nt35590_para470},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para471), tm_nt35590_para471},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(tm_nt35590_para472), tm_nt35590_para472},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cabc_cmd_C1_51), cabc_cmd_C1_51},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cabc_cmd_C1_53), cabc_cmd_C1_53},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cabc_cmd_C1_55), cabc_cmd_C1_55},
	{DTYPE_DCS_WRITE, 1, 0, 0, 120, sizeof(exit_sleep), exit_sleep},
	{DTYPE_DCS_WRITE, 1, 0, 0, 10, sizeof(display_on), display_on},

};


static struct dsi_cmd_desc novatek_display_off_cmds[] = {
	{DTYPE_DCS_WRITE, 1, 0, 0, 10,
		sizeof(display_off), display_off},
	{DTYPE_DCS_WRITE, 1, 0, 0, 120,
		sizeof(enter_sleep), enter_sleep}
};

extern u32 LcdPanleID ;
#define GPIO_LCD_RESET 58
static void lcd_panle_reset(void)
{	
	
	gpio_direction_output(GPIO_LCD_RESET, 1); /* disp enable */
	msleep(20);
	gpio_direction_output(GPIO_LCD_RESET, 0); /* disp enable */
	msleep(20);
	gpio_direction_output(GPIO_LCD_RESET, 1); /* disp enable */
	msleep(20);

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

	mipi  = &mfd->panel_info.mipi;	
	
	if (first_time_panel_on)
	{		
		first_time_panel_on = 0;
		return 0;
	}
	
	lcd_panle_reset();
	

	switch(LcdPanleID) 
	{
		
		case LCD_PANEL_5P0_HX8394_BOE_BOE:
			cmdreq.cmds = boe_hx8394_video_on_cmds;
			cmdreq.cmds_cnt = ARRAY_SIZE(boe_hx8394_video_on_cmds);
			cmdreq.flags = CMD_REQ_COMMIT;
			cmdreq.rlen = 0;
			cmdreq.cb = NULL;
			mipi_dsi_cmdlist_put(&cmdreq);
			printk("LCD_PANEL_5P0_HX8394_BOE_BOE  initialize ");
			return 0;
			
			mipi_dsi_cmds_tx(&novatek_tx_buf, boe_hx8394_video_on_cmds,
			ARRAY_SIZE(boe_hx8394_video_on_cmds));
			printk("\n lcd boe(hx8394)  initialize ");
			break;
		case LCD_PANEL_5P0_NT35590_CMI_CMI:				
			cmdreq.cmds = cmi_nt35590_display_on_add_cmds;
			cmdreq.cmds_cnt = ARRAY_SIZE(cmi_nt35590_display_on_add_cmds);
			cmdreq.flags = CMD_REQ_COMMIT;
			cmdreq.rlen = 0;
			cmdreq.cb = NULL;
			mipi_dsi_cmdlist_put(&cmdreq);
			lcd_panle_reset();			
			cmdreq.cmds = cmi_nt35590_display_on_cmds;
			cmdreq.cmds_cnt = ARRAY_SIZE(cmi_nt35590_display_on_cmds);
			mipi_dsi_cmdlist_put(&cmdreq);
			printk("LCD_PANEL_5P0_NT35590_CMI_CMI  initialize ");
			return 0;
			
			mipi_dsi_cmds_tx(&novatek_tx_buf, cmi_nt35590_display_on_cmds,
			ARRAY_SIZE(cmi_nt35590_display_on_cmds));
			printk("\n lcd cmi(nt35590) initialize ");
			break;
		case LCD_PANEL_5P0_NT35590_AUO_YUSHUN:				
			cmdreq.cmds = auo_nt35590_display_on_cmds;
			cmdreq.cmds_cnt = ARRAY_SIZE(auo_nt35590_display_on_cmds);
			cmdreq.flags = CMD_REQ_COMMIT;
			cmdreq.rlen = 0;
			cmdreq.cb = NULL;
			mipi_dsi_cmdlist_put(&cmdreq);
			printk("LCD_PANEL_5P0_NT35590_AUO_YUSHUN  initialize ");
			break;
		case LCD_PANEL_5P0_NT35590_TM_TM:				
			cmdreq.cmds = tm_nt35590_display_on_cmds;
			cmdreq.cmds_cnt = ARRAY_SIZE(tm_nt35590_display_on_cmds);
			cmdreq.flags = CMD_REQ_COMMIT;
			cmdreq.rlen = 0;
			cmdreq.cb = NULL;
			mipi_dsi_cmdlist_put(&cmdreq);
			printk("LCD_PANEL_5P0_NT35590_TM_TM  initialize ");
			break;
		default:
			printk("LCD_NO_PANEL initialize ");
			break;

	}
		

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
	
	printk("LCD OFF");
	
	cmdreq.cmds = novatek_display_off_cmds;
	cmdreq.cmds_cnt = ARRAY_SIZE(novatek_display_off_cmds);
	cmdreq.flags = CMD_REQ_COMMIT;
	cmdreq.rlen = 0;
	cmdreq.cb = NULL;

	mipi_dsi_cmdlist_put(&cmdreq);

	return 0;

	
	
}

static bool onewiremode = false;
static int lcd_bkl_ctl=12;
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

#ifdef CONFIG_MACH_QUANTUM		//9810 
    	if(current_lel > 32)
    	{
        	current_lel = 32;
    	}
#else
    	if(current_lel > 28)
    	{
        	current_lel = 28;
    	}
#endif

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

static int __devinit mipi_novatek_lcd_probe(struct platform_device *pdev)
{
	struct msm_fb_data_type *mfd;
	struct platform_device *current_pdev;
	if (pdev->id == 0) 
	{
		return 0;
	}

	current_pdev = msm_fb_add_device(pdev);

	if (current_pdev) 
	{
		mfd = platform_get_drvdata(current_pdev);
		if (!mfd)
			return -ENODEV;
		if (mfd->key != MFD_KEY)
			return -EINVAL;
	}
	return 0;
}

static struct platform_driver this_driver = {
	.probe  = mipi_novatek_lcd_probe,
	.driver = {
		.name   = "mipi_novatek",
	},
};

static struct msm_fb_panel_data novatek_panel_data = {
	.on		= mipi_novatek_lcd_on,
	.off		= mipi_novatek_lcd_off,
	.set_backlight = mipi_zte_set_backlight,//mipi_novatek_set_backlight,
};

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

	pdev = platform_device_alloc("mipi_novatek", (panel << 8)|channel);
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
	pr_err("mipi_novatek_device_register\n");
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
