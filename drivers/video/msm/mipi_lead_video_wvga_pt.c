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

extern u32 LcdPanleID;
static struct msm_panel_info pinfo;

static struct mipi_dsi_phy_ctrl dsi_video_mode_phy_db ={
#if  0
		/* DSI Bit Clock at 500 MHz, 2 lane, RGB888 */
	/* regulator */
	{0x03, 0x01, 0x01, 0x00},
	/* timing   */
	{0x79, 0x2e, 0x10, 0x00, 0x3e, 0x45, 0x15, 0x32,
	0x14, 0x03, 0x04},
	/* phy ctrl */
	{0x7f, 0x00, 0x00, 0x00},
	/* strength */
	{0xbb, 0x02, 0x06, 0x00},
	/* pll control */
	{0x01, 0x9e, 0x31, 0xd2, 0x00, 0x40, 0x37, 0x62,
	0x01, 0x0f, 0x07,
	0x05, 0x14, 0x03, 0x0, 0x0, 0x0, 0x20, 0x0, 0x02, 0x0},
#endif
#if 1
	/* 600*1024, RGB888, 3 Lane 55 fps video mode */
    /* regulator */
	{0x03, 0x0a, 0x04, 0x00, 0x20},
	/* timing */
	{0xab, 0x28, 0x18, 0x00, 0x92, 0x97, 0x1b, 0x28,
	0x0c, 0x03, 0x04, 0xa0},
    /* phy ctrl */
	{0x5f, 0x00, 0x00, 0x10},
    /* strength */
	{0xff, 0x00, 0x06, 0x00},
	/* pll control */
	{0x0, 0x7f, 0x1, 0x1a, 0x00, 0x50, 0x48, 0x63,
	0x30, 0x07, 0x03,//0x41, 0x0f, 0x01,
	0x00, 0x14, 0x03, 0x00, 0x02, 0x00, 0x20, 0x00, 0x01 },
	63,
#endif

};

//60hz
/* static struct mipi_dsi_phy_ctrl dsi_video_mode_phy_db_60 = { */
/* //480 800 2lane 60fps 418.5Mhz */
/* 	{0x09, 0x08, 0x05, 0x00, 0x20}, /\* regulator *\/ */
/* /\* timing *\/ */
/* 	{0x6d, 0x19, 0xf, 0x00, 0x2b, 0x35, 0x14, 0x1d, */
/* 	 0x1d, 0x03, 0x04, 0xa0}, */
/* 	{0x5f, 0x00, 0x00, 0x10}, /\* phy ctrl *\/ */
/* 	{0xff, 0x00, 0x06, 0x00}, /\* strength *\/ */
/* /\* pll control *\/ */
/* 	{0x0, 0x1e, 0x30, 0xc1, 0x00, 0x10, 0x0f, 0x61, */
/* 	 0x40, 0x07, 0x03, */
/* 	 0x00, 0x1a, 0x00, 0x00, 0x02, 0x00, 0x20, 0x00, 0x02}, */
/* 	60, */
/* }; */

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
	//&dsi_video_mode_phy_db_60,
	&dsi_video_mode_phy_db_45,
	&dsi_video_mode_phy_db_30
};
static struct mipi_dsi_phy_ctrl dsi_video_mode_phy_db_for_truly={
	#if 1
	/* 600*1024, RGB888, 3 Lane 55 fps video mode */
    /* regulator */
	{0x03, 0x0a, 0x04, 0x00, 0x20},
	/* timing */
	{0x66, 0x17, 0xe, 0x00, 0x29, 0x30, 0x13, 0x1b,
	0x1b, 0x03, 0x04, 0xa0},
    /* phy ctrl */
	{0x5f, 0x00, 0x00, 0x10},
    /* strength */
	{0xff, 0x00, 0x06, 0x00},
	/* pll control */
	{0x0, 0x1c, 0x0, 0x1, 0x00, 0x50, 0x48, 0x63,
	0x30, 0x07, 0x03,//0x41, 0x0f, 0x01,
	0x00, 0x14, 0x03, 0x00, 0x02, 0x00, 0x20, 0x00, 0x01 },
	63
#endif
};
static struct mipi_dsi_phy_ctrl * dsi_video_mode_phy_dbs_for_truly[] = {
	&dsi_video_mode_phy_db_for_truly,
	&dsi_video_mode_phy_db_45,
	&dsi_video_mode_phy_db_30
};
static int __init mipi_video_lead_wvga_pt_init(void)
{
	int ret;

#ifdef CONFIG_FB_MSM_MIPI_PANEL_DETECT
	if (msm_fb_detect_client("mipi_video_lead_wvga"))
		return 0;
#endif

#if 0
	pinfo.xres = 480;
	pinfo.yres = 800;
	pinfo.type = MIPI_VIDEO_PANEL;
	pinfo.pdest = DISPLAY_1;
	pinfo.wait_cycle = 0;
	pinfo.bpp = 24;

	pinfo.lcdc.h_back_porch = 100;
	pinfo.lcdc.h_front_porch = 100;
	pinfo.lcdc.h_pulse_width = 10;
	pinfo.lcdc.v_back_porch = 26;//26;//lead panel must use 26
	pinfo.lcdc.v_front_porch = 10;
	pinfo.lcdc.v_pulse_width = 10;
	pinfo.clk_rate = 420000000;//499000000;
#if 0 //LCD_PANEL_4P0_R61408_TRULY_LG is different from others	
	{
		pinfo.lcdc.h_back_porch = 80;
		pinfo.lcdc.h_front_porch = 160;	
		pinfo.lcdc.v_back_porch = 12;	
	}
#endif

	pinfo.lcdc.border_clr = 0;	/* blk */
	pinfo.lcdc.underflow_clr = 0xff;	/* blue */
	pinfo.lcdc.hsync_skew = 0;
	pinfo.bl_max = 32;
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
	pinfo.mipi.dst_format =  DSI_VIDEO_DST_FORMAT_RGB888;
	pinfo.mipi.vc = 0;
	pinfo.mipi.rgb_swap = DSI_RGB_SWAP_RGB;
	pinfo.mipi.data_lane0 = TRUE;
	pinfo.mipi.data_lane1 = TRUE;
	pinfo.mipi.t_clk_post = 0x20;
	pinfo.mipi.t_clk_pre = 0x2F;
	pinfo.mipi.stream = 0; /* dma_p */


	pinfo.mipi.mdp_trigger = DSI_CMD_TRIGGER_NONE;
	pinfo.mipi.dma_trigger = DSI_CMD_TRIGGER_SW;
	pinfo.mipi.frame_rate = 60;
	pinfo.mipi.dsi_phy_db = &dsi_video_mode_phy_db;
	pinfo.mipi.dlane_swap = 0x01;
	pinfo.mipi.tx_eot_append = 0x01;

#endif
		pinfo.xres = 480;
		pinfo.yres = 800;
		pinfo.type = MIPI_VIDEO_PANEL;
		pinfo.pdest = DISPLAY_1;
		pinfo.wait_cycle = 0;
		pinfo.bpp = 24;
#if 0	
		pinfo.lcdc.h_back_porch = 70;//100;
		pinfo.lcdc.h_front_porch = 30;//50;
		pinfo.lcdc.h_pulse_width = 5;
		pinfo.lcdc.v_back_porch = 10;//10;//75;
		pinfo.lcdc.v_front_porch = 5;//5;
		pinfo.lcdc.v_pulse_width = 1;
#endif
	pinfo.lcdc.h_back_porch = 100;
	pinfo.lcdc.h_front_porch = 100;
	pinfo.lcdc.h_pulse_width = 10;
	pinfo.lcdc.v_back_porch = 26;//26;//lead panel must use 26
	pinfo.lcdc.v_front_porch = 10;
	pinfo.lcdc.v_pulse_width = 10;

		pinfo.lcdc.border_clr = 0;	/* blk */
		pinfo.lcdc.underflow_clr = 0xff;	/* blue */
		pinfo.lcdc.hsync_skew = 0;
#ifdef CONFIG_MACH_ILIAMNA
		pinfo.bl_max = 255;
#else
		pinfo.bl_max = 255;
#endif
		pinfo.bl_min = 1;
		pinfo.fb_num = 2; 
		pinfo.clk_rate = 420292800;//384000000;     cat=420007744     compute:420292800   
	
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
		pinfo.mipi.frame_rate = 63;
		pinfo.mipi.esc_byte_ratio = 4;
		pinfo.mipi.dlane_swap = 0x01;

		pinfo.mipi.dsi_phy_db = dsi_video_mode_phy_dbs;
		pinfo.mipi.dsi_phy_db_count = ARRAY_SIZE(dsi_video_mode_phy_dbs);

		if(LcdPanleID==LCD_PANEL_4P0_R61408_TRULY_LG)
		{
			pinfo.mipi.dsi_phy_db = dsi_video_mode_phy_dbs_for_truly;
					pinfo.mipi.dsi_phy_db_count = ARRAY_SIZE(dsi_video_mode_phy_dbs_for_truly);
			printk(KERN_ERR "raplace=%d\n", LcdPanleID);	
		}
	ret = mipi_lead_device_register(&pinfo, MIPI_DSI_PRIM,
						MIPI_DSI_PANEL_WVGA_PT);
	if (ret)
		printk(KERN_ERR "%s: failed to register device!\n", __func__);

	return ret;
}

module_init(mipi_video_lead_wvga_pt_init);
