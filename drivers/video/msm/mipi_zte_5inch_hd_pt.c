/* Copyright (c) 2010-2011, Code Aurora Forum. All rights reserved.
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
#include "mipi_novatek.h"

#define N9810_USE_CMD_MODE
#define USE_CABC_PANEL
static struct msm_panel_info pinfo;
static struct mipi_dsi_phy_ctrl dsi_video_mode_phy_db = {	

	// my own jb
	/* 720*1280, RGB888, 413M,4 Lane 63  fps video mode */
	/* regulator */
	{0x03, 0x0a, 0x04, 0x00, 0x20},
	/* timing */
	{0x6c, 0x18, 0x0f, 0x00, 0x2b, 0x35, 0x14, 0x1c,
	0x1d, 0x03, 0x04, 0xa0},
	/* phy ctrl */
	{0x5f, 0x00, 0x00, 0x10},
	/* strength */
	{0xff, 0x00, 0x06, 0x00},
	/* pll control */
	{0x01, 0x2d, 0x30, 0xc2, 0x00, 0x5f, 0x48, 0x63,
	0x31, 0x0f, 0x03,//0x41, 0x0f, 0x01,
	0x00, 0x14, 0x03, 0x00, 0x02, 0x00, 0x20, 0x00, 0x01 },
	60,

};
static struct mipi_dsi_phy_ctrl *dsi_video_mode_phy_dbs[] = {
	&dsi_video_mode_phy_db,
};


static int __init mipi_video_novatek_qhd_pt_init(void)
{
	int ret;

	if (msm_fb_detect_client("mipi_video_novatek_qhd"))
		return 0;

	pinfo.xres =720;//480;// 540;
	pinfo.yres =1280;//800;// 960;

	pinfo.pdest = DISPLAY_1;
	pinfo.wait_cycle = 0;
	pinfo.bpp = 24;
	pinfo.lcdc.h_back_porch =60;
	pinfo.lcdc.h_front_porch =60;
	pinfo.lcdc.h_pulse_width =10;
	pinfo.lcdc.v_back_porch = 11;
	pinfo.lcdc.v_front_porch =8;
	pinfo.lcdc.v_pulse_width =3;
	pinfo.lcd.v_back_porch = 11;
	pinfo.lcd.v_front_porch =8;
	pinfo.lcd.v_pulse_width =3;
	pinfo.lcdc.border_clr = 0;	/* blk */
	pinfo.lcdc.underflow_clr = 0;//0xff;	/* blue */
	pinfo.lcdc.hsync_skew = 0;
#ifdef USE_CABC_PANEL	
	pinfo.bl_max =255;
#else
	pinfo.bl_max =28;
#endif
	pinfo.bl_min = 1;
	pinfo.fb_num = 2;
	//pinfo.clk_rate = 416000000;//418332600;//pan 
#ifdef N9810_USE_CMD_MODE
	pinfo.lcd.vsync_enable = TRUE;
	pinfo.lcd.hw_vsync_mode = TRUE;
	pinfo.lcd.refx100 = 5900; /* adjust refx100 to prevent tearing */
	pinfo.mipi.te_sel = 1; /* TE from vsycn gpio */
	pinfo.mipi.interleave_max = 1;
	pinfo.mipi.insert_dcs_cmd = TRUE;
	pinfo.mipi.wr_mem_continue = 0x3c;
	pinfo.mipi.wr_mem_start = 0x2c;

	pinfo.type =MIPI_CMD_PANEL;
	pinfo.mipi.mode =DSI_CMD_MODE;
	pinfo.mipi.dst_format = DSI_CMD_DST_FORMAT_RGB888;
	pinfo.mipi.mdp_trigger =DSI_CMD_TRIGGER_SW_TE;
#else	//video mode
	pinfo.type =MIPI_VIDEO_PANEL;
	pinfo.mipi.mode = DSI_VIDEO_MODE;
	pinfo.mipi.dst_format = DSI_VIDEO_DST_FORMAT_RGB888;
	pinfo.mipi.mdp_trigger =DSI_CMD_TRIGGER_SW;
#endif
	
	pinfo.mipi.pulse_mode_hsa_he = FALSE;
	pinfo.mipi.hfp_power_stop = FALSE;
	pinfo.mipi.hbp_power_stop = FALSE;
	pinfo.mipi.hsa_power_stop = FALSE;
	pinfo.mipi.eof_bllp_power_stop = TRUE;//nt35590 must set true
	pinfo.mipi.bllp_power_stop = FALSE;//set TURE for esd test
	pinfo.mipi.traffic_mode =DSI_NON_BURST_SYNCH_EVENT;// DSI_NON_BURST_SYNCH_PULSE;
	pinfo.mipi.frame_rate = 60;
	pinfo.mipi.vc = 0;
	//pinfo.mipi.rgb_swap = DSI_RGB_SWAP_RGB;//DSI_RGB_SWAP_BGR;
	pinfo.mipi.data_lane0 = TRUE;
	pinfo.mipi.esc_byte_ratio = 4;

	pinfo.mipi.data_lane1 = TRUE;

	pinfo.mipi.data_lane2 = TRUE;
	pinfo.mipi.data_lane3 = TRUE;
	//pinfo.mipi.tx_eot_append = TRUE;
	pinfo.mipi.t_clk_post =0x20;// 0x04;
	pinfo.mipi.t_clk_pre = 0x2f;//0x1c;
	pinfo.mipi.stream = 0; /* dma_p */
	
	pinfo.mipi.dma_trigger = DSI_CMD_TRIGGER_SW;
	pinfo.mipi.dsi_phy_db = dsi_video_mode_phy_dbs;
	pinfo.mipi.dsi_phy_db_count = ARRAY_SIZE(dsi_video_mode_phy_dbs);

	ret = mipi_novatek_device_register(&pinfo, MIPI_DSI_PRIM,
						MIPI_DSI_PANEL_720P_PT);
	if (ret)
		pr_err("%s: failed to register device!\n", __func__);

	return ret;
}

module_init(mipi_video_novatek_qhd_pt_init);
