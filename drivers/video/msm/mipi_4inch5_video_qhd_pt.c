/* Copyright (c) 2009-2011, Code Aurora Forum. All rights reserved.
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

#define USE_CMD_MODE

static struct msm_panel_info pinfo;

static struct mipi_dsi_phy_ctrl dsi_video_mode_phy_db ={
#if 0
#if 0
	//540*960,2lane,60 fps, same as frosty
	/* regulator */
	{0x03, 0x0a, 0x04, 0x00, 0x20},
	/* timing */
	{0x66, 0x26, 0x16, 0x00, 0x19, 0x8e, 0x1e, 0x8c,
	0x19, 0x03, 0x04, 0xa0},
    /* phy ctrl */
	{0x5f, 0x00, 0x00, 0x10},
    /* strength */
	{0xff, 0x00, 0x06, 0x00},
	/* pll control */
	{0x0, 0xf2, 0x1, 0x1a, 0x00, 0x50, 0x48, 0x63,
	0x31, 0x0f, 0x07,//0x41, 0x0f, 0x01,
	0x00, 0x14, 0x03, 0x00, 0x02, 0x00, 0x20, 0x00, 0x01 },
#else
	//540*960,2lane,60 fps, same as frosty
	/* regulator */
	{0x03, 0x0a, 0x04, 0x00, 0x20},
	/* timing */
	{0x66, 0x26, 0x16, 0x00, 0x19, 0x8e, 0x1e, 0x8c,
	0x19, 0x03, 0x04, 0xa0},
	/* phy ctrl */
	{0x5f, 0x00, 0x00, 0x10},
	/* strength */
	{0xff, 0x00, 0x06, 0x00},
	/* pll control */
	{0x0, 0x61, 0x1, 0x1a, 0x00, 0x50, 0x48, 0x63,
	0x30, 0x07, 0x03,//0x41, 0x0f, 0x01,
	0x00, 0x14, 0x03, 0x00, 0x02, 0x00, 0x20, 0x00, 0x01 },
#endif
#endif
	/* bit_clk 526.50Mhz, 540*960, RGB888, 2 Lane 60 fps video mode */ 
	/* regulator */ 
	{0x09, 0x08, 0x05, 0x00, 0x20}, 
	/* timing */ 
	{0x88, 0x1f, 0x16, 0x00, 0x36, 0x40, 0x1c, 0x25, 
	0x25, 0x03, 0x04, 0xa0}, 
	/* phy ctrl */ 
	{0x5f, 0x00, 0x00, 0x10}, 
	/* strength */ 
	{0xff, 0x00, 0x06, 0x00}, 
	/* pll control */ 
	{0x0, 0x26, 0x30, 0xc1, 0x00, 0x50, 0x48, 0x63, 
	0x41, 0x0f, 0x01, 
	0x00, 0x14, 0x03, 0x00, 0x02, 0x00, 0x20, 0x00, 0x01 },
	60,
};

static struct mipi_dsi_phy_ctrl *dsi_video_mode_phy_dbs[] = {
	&dsi_video_mode_phy_db,
};


static int __init mipi_video_lead_wvga_pt_init(void)
{
	int ret;

#ifdef CONFIG_FB_MSM_MIPI_PANEL_DETECT
	if (msm_fb_detect_client("mipi_video_coeus_wvga"))
		return 0;
#endif

	pinfo.xres = 540;
	pinfo.yres = 960;
	pinfo.pdest = DISPLAY_1;
	pinfo.wait_cycle = 0;
	pinfo.bpp = 24;
	pinfo.lcdc.h_back_porch = 80;
	pinfo.lcdc.h_front_porch = 80;
	pinfo.lcdc.h_pulse_width = 8;
	pinfo.lcdc.v_back_porch =32;
	pinfo.lcdc.v_front_porch = 32;
	pinfo.lcdc.v_pulse_width = 4;
	pinfo.lcdc.border_clr = 0;	/* blk */
	pinfo.lcdc.underflow_clr = 0xff;	/* blue */
	pinfo.lcdc.hsync_skew = 0;
#ifdef CONFIG_MACH_OCEANUS
#ifdef USE_CABC_PANEL
	pinfo.bl_max = 254;
#else
	pinfo.bl_max = 31;
#endif
#else
	pinfo.bl_max = 240;
#endif
	pinfo.bl_min = 1;
	pinfo.fb_num = 2; 
//	pinfo.clk_rate = 405756000;
#ifdef USE_CMD_MODE
		pinfo.lcd.vsync_enable = TRUE;
		pinfo.lcd.hw_vsync_mode = TRUE;
		pinfo.lcd.refx100 = 5900; /* adjust refx100 to prevent tearing */
		pinfo.mipi.te_sel = 1; /* TE from vsycn gpio */
		pinfo.mipi.interleave_max = 1;
		pinfo.mipi.insert_dcs_cmd = TRUE;
		pinfo.mipi.wr_mem_continue = 0x3c;
		pinfo.mipi.wr_mem_start = 0x2c;
		pinfo.lcd.v_back_porch = 32;
		pinfo.lcd.v_front_porch = 32;
		pinfo.lcd.v_pulse_width = 4;
	
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

	pinfo.mipi.pulse_mode_hsa_he = TRUE;
	pinfo.mipi.hfp_power_stop = TRUE;
	pinfo.mipi.hbp_power_stop = TRUE;
	pinfo.mipi.hsa_power_stop = TRUE;
	pinfo.mipi.eof_bllp_power_stop = TRUE;
	pinfo.mipi.bllp_power_stop = TRUE;
	pinfo.mipi.traffic_mode = DSI_BURST_MODE;
	pinfo.mipi.vc = 0;
	pinfo.mipi.rgb_swap = DSI_RGB_SWAP_BGR;
	pinfo.mipi.data_lane0 = TRUE;
	pinfo.mipi.data_lane1 = TRUE;
	pinfo.mipi.t_clk_post = 0x22;	//0x03;
	pinfo.mipi.t_clk_pre = 0x3d;	//0x24;
	pinfo.mipi.stream = 0; /* dma_p */
	pinfo.mipi.dma_trigger = DSI_CMD_TRIGGER_SW;
	pinfo.mipi.frame_rate = 60;
	pinfo.mipi.esc_byte_ratio = 4;
	pinfo.mipi.dlane_swap = 0x01;
	pinfo.mipi.rx_eot_ignore = 0;
	pinfo.mipi.tx_eot_append = TRUE;	
	pinfo.mipi.dsi_phy_db = dsi_video_mode_phy_dbs;
	pinfo.mipi.dsi_phy_db_count = ARRAY_SIZE(dsi_video_mode_phy_dbs);
	ret = mipi_lead_device_register(&pinfo, MIPI_DSI_PRIM, MIPI_DSI_PANEL_WVGA_PT);
	
	if (ret)
		printk(KERN_ERR "%s: failed to register device!\n", __func__);

	return ret;
}

module_init(mipi_video_lead_wvga_pt_init);
