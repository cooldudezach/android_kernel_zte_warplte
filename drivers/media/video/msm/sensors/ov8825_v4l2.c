/* Copyright (c) 2012, The Linux Foundation. All rights reserved.
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

#include "msm_sensor.h"
#include "msm.h"
#include "msm_ispif.h"
#define SENSOR_NAME "ov8825"
#define PLATFORM_DRIVER_NAME "msm_camera_ov8825"
#define ov8825_obj ov8825_##obj
struct otp_struct current_wb_otp;
struct otp_struct current_lenc_otp;

extern void msm_sensorinfo_set_back_sensor_id(uint16_t id);

DEFINE_MUTEX(ov8825_mut);
static struct msm_sensor_ctrl_t ov8825_s_ctrl;
static struct msm_camera_i2c_reg_conf qtech_lens_settings[] = {
{0x5800,0x15},
{0x5801,0xd },
{0x5802,0xb },
{0x5803,0xb },
{0x5804,0xe },
{0x5805,0x15},
{0x5806,0x9 },
{0x5807,0x5 },
{0x5808,0x3 },
{0x5809,0x4 },
{0x580a,0x6 },
{0x580b,0x9 },
{0x580c,0x6 },
{0x580d,0x2 },
{0x580e,0x0 },
{0x580f,0x0 },
{0x5810,0x2 },
{0x5811,0x6 },
{0x5812,0x6 },
{0x5813,0x2 },
{0x5814,0x0 },
{0x5815,0x0 },
{0x5816,0x2 },
{0x5817,0x6 },
{0x5818,0x9 },
{0x5819,0x5 },
{0x581a,0x3 },
{0x581b,0x3 },
{0x581c,0x6 },
{0x581d,0x9 },
{0x581e,0x16},
{0x581f,0xe },
{0x5820,0xb },
{0x5821,0xc },
{0x5822,0xe },
{0x5823,0x18},
{0x5824,0x8 },
{0x5825,0xa },
{0x5826,0xc },
{0x5827,0xa },
{0x5828,0xa },
{0x5829,0x2a},
{0x582a,0x28},
{0x582b,0x44},
{0x582c,0x28},
{0x582d,0xc },
{0x582e,0xa },
{0x582f,0x42},
{0x5830,0x60},
{0x5831,0x42},
{0x5832,0xa },
{0x5833,0x2c},
{0x5834,0x28},
{0x5835,0x46},
{0x5836,0x28},
{0x5837,0xc },
{0x5838,0xa },
{0x5839,0xa },
{0x583a,0xc },
{0x583b,0xa },
{0x583c,0xa },
{0x583d,0xae},
};

static struct msm_camera_i2c_reg_conf ov8825_start_settings[] = {
	{0x0100, 0x01},
};

static struct msm_camera_i2c_reg_conf ov8825_stop_settings[] = {
	{0x0100, 0x00},
};

static struct msm_camera_i2c_reg_conf ov8825_groupon_settings[] = {
	{0x3208, 0x00},
};

static struct msm_camera_i2c_reg_conf ov8825_groupoff_settings[] = {
	{0x3208, 0x10},
	{0x3208, 0xA0},
};

static struct msm_camera_i2c_reg_conf ov8825_prev_settings[] ={

//@@ Capture 3264x2448 24fps 528Mbps/Lane
//@@3264_2448_4_lane_24fps_100Msysclk_528MBps/lane

{0x0100, 0x00}, // sleep
{0x3003, 0xce}, ////PLL_CTRL0
{0x3004, 0xbf}, ////PLL_CTRL1
{0x3005, 0x10}, ////PLL_CTRL2
{0x3006, 0x00}, ////PLL_CTRL3
{0x3007, 0x3b}, ////PLL_CTRL4
{0x3011, 0x02}, ////MIPI_Lane_4_Lane
{0x3012, 0x80}, ////SC_PLL CTRL_S0
{0x3013, 0x39}, ////SC_PLL CTRL_S1
{0x3104, 0x20}, ////SCCB_PLL
{0x3106, 0x15}, ////SRB_CTRL
{0x3501, 0x9a}, ////AEC_HIGH
{0x3502, 0xa0}, ////AEC_LOW
{0x350b, 0x1f}, ////AGC
{0x3600, 0x06}, //ANACTRL0
{0x3601, 0x34}, //ANACTRL1
{0x3700, 0x20}, //SENCTROL0 Sensor control
{0x3702, 0x50}, //SENCTROL2 Sensor control
{0x3703, 0xcc}, //SENCTROL3 Sensor control
{0x3704, 0x19}, //SENCTROL4 Sensor control
{0x3705, 0x14}, //SENCTROL5 Sensor control
{0x3706, 0x4b}, //SENCTROL6 Sensor control
{0x3707, 0x63}, //SENCTROL7 Sensor control
{0x3708, 0x84}, //SENCTROL8 Sensor control
{0x3709, 0x40}, //SENCTROL9 Sensor control
{0x370a, 0x12}, //SENCTROLA Sensor control
{0x370e, 0x00}, //SENCTROLE Sensor control
{0x3711, 0x0f}, //SENCTROL11 Sensor control
{0x3712, 0x9c}, //SENCTROL12 Sensor control
{0x3724, 0x01}, //Reserved
{0x3725, 0x92}, //Reserved
{0x3726, 0x01}, //Reserved
{0x3727, 0xa9}, //Reserved
{0x3800, 0x00}, //HS(HREF start High)
{0x3801, 0x00}, //HS(HREF start Low)
{0x3802, 0x00}, //VS(Vertical start High)
{0x3803, 0x00}, //VS(Vertical start Low)
{0x3804, 0x0c}, //HW = 3295
{0x3805, 0xdf}, //HW
{0x3806, 0x09}, //VH = 2459
{0x3807, 0x9b}, //VH
{0x3808, 0x0c}, //ISPHO = 3264
{0x3809, 0xc0}, //ISPHO
{0x380a, 0x09}, //ISPVO = 2448
{0x380b, 0x90}, //ISPVO
{0x380c, 0x0d}, //HTS = 3360
{0x380d, 0x20}, //HTS
{0x380e, 0x09}, //VTS = 2480
{0x380f, 0xb0}, //VTS
{0x3810, 0x00}, //HOFF = 16
{0x3811, 0x10}, //HOFF
{0x3812, 0x00}, //VOFF = 6
{0x3813, 0x06}, //VOFF
{0x3814, 0x11}, //X INC
{0x3815, 0x11}, //Y INC
 #if defined CONFIG_MACH_FROSTY || defined CONFIG_MACH_KISKA||defined (CONFIG_MACH_WARPLTE)
{0x3820, 0x80}, //Timing Reg20:Vflip
{0x3821, 0x16}, //Timing Reg21:Hmirror
#else
{0x3820, 0x86}, //Timing Reg20:Vflip
{0x3821, 0x10}, //Timing Reg21:Hmirror
#endif
{0x3f00, 0x02}, //PSRAM Ctrl0
{0x3f01, 0xfc}, //PSRAM Ctrl1
{0x3f05, 0x10}, //PSRAM Ctrl5
{0x4600, 0x04}, //VFIFO Ctrl0
{0x4601, 0x00}, //VFIFO Read ST High
{0x4602, 0x20}, //VFIFO Read ST Low
{0x4837, 0x14}, //MIPI PCLK PERIOD
{0x5068, 0x00}, //HSCALE_CTRL
{0x506a, 0x00}, //VSCALE_CTRL
{0x5c00, 0x80}, //PBLC CTRL00
{0x5c01, 0x00}, //PBLC CTRL01
{0x5c02, 0x00}, //PBLC CTRL02
{0x5c03, 0x00}, //PBLC CTRL03
{0x5c04, 0x00}, //PBLC CTRL04
{0x5c08, 0x10}, //PBLC CTRL08
{0x6900, 0x61}, //CADC CTRL00
{0x3618, 0x00}, //release af
{0x3619, 0x00}, // release af
//{0x0100, 0x01}, // wake up
{0x0100, 0x00}, // wake up

};

static struct msm_camera_i2c_reg_conf ov8825_snap_settings[] = {

  //OV8825 Camera Application Notes R1.12
  //change from 24fps to 16fps
  //change r3005 from 10 to 20, r3006 from 00 to 10 by kenxu
  //@@ Capture 3264x2448 16fps 352Mbps/Lane
  //@@3264_2448_4_lane_16fps_66.67Msysclk_352MBps/lane  
  {0x0100, 0x00}, // sleep
  {0x3003, 0xce}, ////PLL_CTRL0
  {0x3004, 0xbf}, ////PLL_CTRL1
  {0x3005, 0x10}, ////PLL_CTRL2
  {0x3006, 0x00}, ////PLL_CTRL3
  {0x3007, 0x3b}, ////PLL_CTRL4
  {0x3011, 0x02}, ////MIPI_Lane_4_Lane
  {0x3012, 0x80}, ////SC_PLL CTRL_S0
  {0x3013, 0x39}, ////SC_PLL CTRL_S1
  {0x3104, 0x20}, ////SCCB_PLL
  {0x3106, 0x15}, ////SRB_CTRL
  {0x3501, 0x9a}, ////AEC_HIGH
  {0x3502, 0xa0}, ////AEC_LOW
  {0x350b, 0x1f}, ////AGC
  {0x3600, 0x06}, //ANACTRL0
  {0x3601, 0x34}, //ANACTRL1
  {0x3700, 0x20}, //SENCTROL0 Sensor control
  {0x3702, 0x50}, //SENCTROL2 Sensor control
  {0x3703, 0xcc}, //SENCTROL3 Sensor control
  {0x3704, 0x19}, //SENCTROL4 Sensor control
  {0x3705, 0x14}, //SENCTROL5 Sensor control
  {0x3706, 0x4b}, //SENCTROL6 Sensor control
  {0x3707, 0x63}, //SENCTROL7 Sensor control
  {0x3708, 0x84}, //SENCTROL8 Sensor control
  {0x3709, 0x40}, //SENCTROL9 Sensor control
  {0x370a, 0x12}, //SENCTROLA Sensor control
  {0x370e, 0x00}, //SENCTROLE Sensor control
  {0x3711, 0x0f}, //SENCTROL11 Sensor control
  {0x3712, 0x9c}, //SENCTROL12 Sensor control
  {0x3724, 0x01}, //Reserved
  {0x3725, 0x92}, //Reserved
  {0x3726, 0x01}, //Reserved
  {0x3727, 0xa9}, //Reserved
  {0x3800, 0x00}, //HS(HREF start High)
  {0x3801, 0x00}, //HS(HREF start Low)
  {0x3802, 0x00}, //VS(Vertical start High)
  {0x3803, 0x00}, //VS(Vertical start Low)
  {0x3804, 0x0c}, //HW = 3295
  {0x3805, 0xdf}, //HW
  {0x3806, 0x09}, //VH = 2459
  {0x3807, 0x9b}, //VH
  {0x3808, 0x0c}, //ISPHO = 3264
  {0x3809, 0xc0}, //ISPHO
  {0x380a, 0x09}, //ISPVO = 2448
  {0x380b, 0x90}, //ISPVO
  {0x380c, 0x0d}, //HTS = 3360
  {0x380d, 0x20}, //HTS
  {0x380e, 0x09}, //VTS = 2480
  {0x380f, 0xb0}, //VTS
  {0x3810, 0x00}, //HOFF = 16
  {0x3811, 0x10}, //HOFF
  {0x3812, 0x00}, //VOFF = 6
  {0x3813, 0x06}, //VOFF
  {0x3814, 0x11}, //X INC
  {0x3815, 0x11}, //Y INC
  #if  defined (CONFIG_MACH_FROSTY) ||defined (CONFIG_MACH_WARPLTE)
{0x3820, 0x80}, //Timing Reg20:Vflip
{0x3821, 0x16}, //Timing Reg21:Hmirror
#else
{0x3820, 0x86}, //Timing Reg20:Vflip
{0x3821, 0x10}, //Timing Reg21:Hmirror
#endif
  {0x3f00, 0x02}, //PSRAM Ctrl0
  {0x3f01, 0xfc}, //PSRAM Ctrl1
  {0x3f05, 0x10}, //PSRAM Ctrl5
  {0x4600, 0x04}, //VFIFO Ctrl0
  {0x4601, 0x00}, //VFIFO Read ST High
  {0x4602, 0x20}, //VFIFO Read ST Low
  {0x4837, 0x14}, //MIPI PCLK PERIOD
  {0x5068, 0x00}, //HSCALE_CTRL
  {0x506a, 0x00}, //VSCALE_CTRL
  {0x5c00, 0x80}, //PBLC CTRL00
  {0x5c01, 0x00}, //PBLC CTRL01
  {0x5c02, 0x00}, //PBLC CTRL02
  {0x5c03, 0x00}, //PBLC CTRL03
  {0x5c04, 0x00}, //PBLC CTRL04
  {0x5c08, 0x10}, //PBLC CTRL08
  {0x6900, 0x61}, //CADC CTRL00
  //{0x0100, 0x01}, // wake up
  {0x0100, 0x00}, // wake up

};

static struct msm_camera_i2c_reg_conf ov8825_reset_settings[] = {
	{0x0103, 0x01},
};

static struct msm_camera_i2c_reg_conf ov8825_recommend_settings[] = {
  //OV8825 Camera Application Notes R1.12
  {0x3000, 0x16}, // strobe disable, frex disable, vsync disable
  {0x3001, 0x00}, 
  {0x3002, 0x6c}, // SCCB ID = 0x6c
  {0x300d, 0x00}, // PLL2
  {0x301f, 0x09}, // frex_mask_mipi, frex_mask_mipi_phy
  {0x3010, 0x00}, // strobe, sda, frex, vsync, shutter GPIO unselected
  {0x3018, 0x00}, // clear PHY HS TX power down and PHY LP RX power down
  {0x3300, 0x00}, 
  {0x3500, 0x00}, // exposure[19:16] = 0
  {0x3503, 0x07}, // Gain has no delay, VTS manual, AGC manual, AEC manual
  {0x3509, 0x00}, // use sensor gain
  {0x3602, 0x42}, 
  {0x3603, 0x5c}, // analog control
  {0x3604, 0x98}, // analog control
  {0x3605, 0xf5}, // analog control
  {0x3609, 0xb4}, // analog control
  {0x360a, 0x7c}, // analog control
  {0x360b, 0xc9}, // analog control
  {0x360c, 0x0b}, // analog control
  {0x3612, 0x00}, // pad drive 1x, analog control
  {0x3613, 0x02}, // analog control
  {0x3614, 0x0f}, // analog control
  {0x3615, 0x00}, // analog control
  {0x3616, 0x03}, // analog control
  {0x3617, 0xa1}, // analog control
  {0x3618, 0x00}, // VCM position & slew rate, slew rate = 0
  {0x3619, 0x00}, // VCM position = 0
  {0x361a, 0xB0}, // VCM clock divider, VCM clock = 24000000/0x4b0 = 20000
  {0x361b, 0x04}, // VCM clock divider
  {0x361c, 0x07}, //added by CDZ_CAM_ZTE FOR AF  electric current  
  {0x3701, 0x44}, // sensor control
  {0x370b, 0x01}, // sensor control
  {0x370c, 0x50}, // sensor control
  {0x370d, 0x00}, // sensor control
  {0x3816, 0x02}, // Hsync start H
  {0x3817, 0x40}, // Hsync start L
  {0x3818, 0x00}, // Hsync end H
  {0x3819, 0x40}, // Hsync end L
  {0x3b1f, 0x00}, // Frex conrol
  // clear OTP data buffer
  {0x3d00, 0x00},
  {0x3d01, 0x00},
  {0x3d02, 0x00},
  {0x3d03, 0x00},
  {0x3d04, 0x00},
  {0x3d05, 0x00},
  {0x3d06, 0x00},
  {0x3d07, 0x00},
  {0x3d08, 0x00},
  {0x3d09, 0x00},
  {0x3d0a, 0x00},
  {0x3d0b, 0x00},
  {0x3d0c, 0x00},
  {0x3d0d, 0x00},
  {0x3d0e, 0x00},
  {0x3d0f, 0x00},
  {0x3d10, 0x00},
  {0x3d11, 0x00},
  {0x3d12, 0x00},
  {0x3d13, 0x00},
  {0x3d14, 0x00},
  {0x3d15, 0x00},
  {0x3d16, 0x00},
  {0x3d17, 0x00},
  {0x3d18, 0x00},
  {0x3d19, 0x00},
  {0x3d1a, 0x00},
  {0x3d1b, 0x00},
  {0x3d1c, 0x00},
  {0x3d1d, 0x00},
  {0x3d1e, 0x00},
  {0x3d1f, 0x00},
  {0x3d80, 0x00},
  {0x3d81, 0x00},
  {0x3d84, 0x00},
  {0x3f06, 0x00},
  {0x3f07, 0x00},
  // BLC
  {0x4000, 0x29},
  {0x4001, 0x02}, // BLC start line
  {0x4002, 0x45}, // BLC auto, reset 5 frames
  {0x4003, 0x08}, // BLC redo at 8 frames
  {0x4004, 0x04}, // 4 black lines are used for BLC
  {0x4005, 0x18}, // no black line output, apply one channel offiset (0x400c, 0x400d) to all manual BLC channels
  {0x4300, 0xff}, // max
  {0x4303, 0x00}, // format control
  {0x4304, 0x08}, // output {data[7:0], data[9:8]}
  {0x4307, 0x00}, // embeded control
  //MIPI
  {0x4800, 0x04},
  {0x4801, 0x0f}, // ECC configure
  {0x4843, 0x02}, // manual set pclk divider
  // ISP
  {0x5000, 0x06}, // LENC off, BPC on, WPC on
  {0x5001, 0x00}, // MWB off
  {0x5002, 0x00},
  {0x501f, 0x00}, // enable ISP
  {0x5780, 0xfc},
  {0x5c05, 0x00}, // pre BLC
  {0x5c06, 0x00}, // pre BLC
  {0x5c07, 0x80}, // pre BLC
  // temperature sensor
  {0x6700, 0x05},
  {0x6701, 0x19},
  {0x6702, 0xfd},
  {0x6703, 0xd7},
  {0x6704, 0xff},
  {0x6705, 0xff},
  {0x6800, 0x10},
  {0x6801, 0x02},
  {0x6802, 0x90},
  {0x6803, 0x10},
  {0x6804, 0x59},
  {0x6901, 0x04}, // CADC control
  #if 0
  //Lens Control
  {0x5800, 0x0f},
  {0x5801, 0x0d},
  {0x5802, 0x09},
  {0x5803, 0x0a},
  {0x5804, 0x0d},
  {0x5805, 0x14},
  {0x5806, 0x0a},
  {0x5807, 0x04},
  {0x5808, 0x03},
  {0x5809, 0x03},
  {0x580a, 0x05},
  {0x580b, 0x0a},
  {0x580c, 0x05},
  {0x580d, 0x02},
  {0x580e, 0x00},
  {0x580f, 0x00},
  {0x5810, 0x03},
  {0x5811, 0x05},
  {0x5812, 0x09},
  {0x5813, 0x03},
  {0x5814, 0x01},
  {0x5815, 0x01},
  {0x5816, 0x04},
  {0x5817, 0x09},
  {0x5818, 0x09},
  {0x5819, 0x08},
  {0x581a, 0x06},
  {0x581b, 0x06},
  {0x581c, 0x08},
  {0x581d, 0x06},
  {0x581e, 0x33},
  {0x581f, 0x11},
  {0x5820, 0x0e},
  {0x5821, 0x0f},
  {0x5822, 0x11},
  {0x5823, 0x3f},
  {0x5824, 0x08},
  {0x5825, 0x46},
  {0x5826, 0x46},
  {0x5827, 0x46},
  {0x5828, 0x46},
  {0x5829, 0x46},
  {0x582a, 0x42},
  {0x582b, 0x42},
  {0x582c, 0x44},
  {0x582d, 0x46},
  {0x582e, 0x46},
  {0x582f, 0x60},
  {0x5830, 0x62},
  {0x5831, 0x42},
  {0x5832, 0x46},
  {0x5833, 0x46},
  {0x5834, 0x44},
  {0x5835, 0x44},
  {0x5836, 0x44},
  {0x5837, 0x48},
  {0x5838, 0x28},
  {0x5839, 0x46},
  {0x583a, 0x48},
  {0x583b, 0x68},
  {0x583c, 0x28},
  {0x583d, 0xae},
  #endif
  {0x5842, 0x00},
  {0x5843, 0xef},
  {0x5844, 0x01},
  {0x5845, 0x3f},
  {0x5846, 0x01},
  {0x5847, 0x3f},
  {0x5848, 0x00},
  {0x5849, 0xd5},
  // Exposure
  {0x3503, 0x07}, // Gain has no delay, VTS manual, AGC manual, AEC manual
  {0x3500, 0x00}, // expo[19:16] = lines/16
  {0x3501, 0x27}, // expo[15:8]
  {0x3502, 0x00}, // expo[7:0]
  {0x350b, 0xff}, // gain
  // MWB
  {0x3400, 0x04}, // red h
  {0x3401, 0x00}, // red l
  {0x3402, 0x04}, // green h
  {0x3403, 0x00}, // green l
  {0x3404, 0x04}, // blue h
  {0x3405, 0x00}, // blue l
  {0x3406, 0x01}, // MWB manual
  // ISP
  {0x5001, 0x01}, // MWB on
  {0x5000, 0x06}, // LENC off, BPC on, WPC on
//  {0x361c, 0x07},//added by CDZ_CAM_ZTE FOR AF  electric current  
{0x3608, 0x40},//0x40-->external DVDD,, 0x00-->internal DVDD
 // {0x4303,0x08},//colorbar
 
 //ov8825 LENC setting
{0x5000,0x86},//enable lenc
#if 0
{0x5800,0x14},
{0x5801,0xc },
{0x5802,0xa },
{0x5803,0xb },
{0x5804,0xd },
{0x5805,0x16},
{0x5806,0x9 },
{0x5807,0x5 },
{0x5808,0x3 },
{0x5809,0x4 },
{0x580a,0x6 },
{0x580b,0xa },
{0x580c,0x6 },
{0x580d,0x2 },
{0x580e,0x0 },
{0x580f,0x0 },
{0x5810,0x2 },
{0x5811,0x6 },
{0x5812,0x6 },
{0x5813,0x2 },
{0x5814,0x0 },
{0x5815,0x0 },
{0x5816,0x2 },
{0x5817,0x7 },
{0x5818,0x9 },
{0x5819,0x6 },
{0x581a,0x4 },
{0x581b,0x4 },
{0x581c,0x7 },
{0x581d,0xa },
{0x581e,0x18},
{0x581f,0xd },
{0x5820,0xb },
{0x5821,0xb },
{0x5822,0xe },
{0x5823,0x1c},
{0x5824,0x28},
{0x5825,0x2a},
{0x5826,0x2a},
{0x5827,0x2a},
{0x5828,0xe },
{0x5829,0x48},
{0x582a,0x66},
{0x582b,0x64},
{0x582c,0x44},
{0x582d,0x48},
{0x582e,0x48},
{0x582f,0x82},
{0x5830,0x80},
{0x5831,0x62},
{0x5832,0x48},
{0x5833,0x48},
{0x5834,0x66},
{0x5835,0x64},
{0x5836,0x66},
{0x5837,0x48},
{0x5838,0x4a},
{0x5839,0x4a},
{0x583a,0x2c},
{0x583b,0x4a},
{0x583c,0x2e},
{0x583d,0x8e},
#endif
};



static struct v4l2_subdev_info ov8825_subdev_info[] = {
	{
	.code   = V4L2_MBUS_FMT_SBGGR10_1X10,
	.colorspace = V4L2_COLORSPACE_JPEG,
	.fmt    = 1,
	.order    = 0,
	},
	/* more can be supported, to be added later */
};

static struct msm_camera_i2c_conf_array ov8825_init_conf[] = {
	{&ov8825_reset_settings[0],
	ARRAY_SIZE(ov8825_reset_settings), 50, MSM_CAMERA_I2C_BYTE_DATA},
	{&ov8825_recommend_settings[0],
	ARRAY_SIZE(ov8825_recommend_settings), 0, MSM_CAMERA_I2C_BYTE_DATA}
};

static struct msm_camera_i2c_conf_array ov8825_confs[] = {
	{&ov8825_snap_settings[0],
	ARRAY_SIZE(ov8825_snap_settings), 0, MSM_CAMERA_I2C_BYTE_DATA},
	{&ov8825_prev_settings[0],
	ARRAY_SIZE(ov8825_prev_settings), 0, MSM_CAMERA_I2C_BYTE_DATA},
};

static struct msm_sensor_output_info_t ov8825_dimensions[] = {
	{
		.x_output = 0xCC0,
		.y_output = 0x990,
		.line_length_pclk = 0xD20,
		.frame_length_lines = 0x9B0,
		.vt_pixel_clk = 100000000,
		.op_pixel_clk = 264000000,
		.binning_factor = 1,
	},
	{
		.x_output = 0xCC0,
		.y_output = 0x990,
		.line_length_pclk = 0xD20,
		.frame_length_lines = 0x9B0,
		.vt_pixel_clk = 100000000,
		.op_pixel_clk = 264000000,
		.binning_factor = 1,
	},
};

#if 0
static struct msm_camera_csid_vc_cfg ov8825_cid_cfg[] = {
	{0, CSI_RAW10, CSI_DECODE_10BIT},
	{1, CSI_EMBED_DATA, CSI_DECODE_8BIT},
};

static struct msm_camera_csi2_params ov8825_csi_params = {
	.csid_params = {
		.lane_assign = 0xe4,
		.lane_cnt = 4,
		.lut_params = {
			.num_cid = 2,
			.vc_cfg = ov8825_cid_cfg,
		},
	},
	.csiphy_params = {
		.lane_cnt = 4,
		.settle_cnt = 0x14,
	},
};


static struct msm_camera_csi2_params *ov8825_csi_params_array[] = {
	&ov8825_csi_params,
	&ov8825_csi_params,
};
#endif

static struct msm_sensor_output_reg_addr_t ov8825_reg_addr = {
	.x_output = 0x3808,
	.y_output = 0x380a,
	.line_length_pclk = 0x380c,
	.frame_length_lines = 0x380e,
};

static enum msm_camera_vreg_name_t ov8825_veg_seq[] = {
	//CAM_VDIG,
	//CAM_VANA,
	//CAM_VIO,
	//CAM_VAF,
};

static struct msm_sensor_id_info_t ov8825_id_info = {
	.sensor_id_reg_addr = 0x300A,
	.sensor_id = 0x8825,
};

static struct msm_sensor_exp_gain_info_t ov8825_exp_gain_info = {
	.coarse_int_time_addr = 0x3501,
	.global_gain_addr = 0x350A,
	.vert_offset = 6,
};
static struct msm_cam_clk_info cam_clk_info[] = {
	{"cam_clk", MSM_SENSOR_MCLK_24HZ},
};


int read_sccb16(uint32_t address, struct msm_sensor_ctrl_t *s_ctrl)
{
uint16_t temp, flag;
temp= msm_camera_i2c_read(
			s_ctrl->sensor_i2c_client,
			address, &flag,
			MSM_CAMERA_I2C_BYTE_DATA);
return flag;
}

int write_sccb16(uint32_t address, uint32_t value, struct msm_sensor_ctrl_t *s_ctrl)
{
int rc;


rc= msm_camera_i2c_write(
			s_ctrl->sensor_i2c_client,
			address, value,
			MSM_CAMERA_I2C_BYTE_DATA);
return rc;
}	
// index: index of otp group. (0, 1, 2)
// return: 0, group index is empty
// 1, group index has invalid data
// 2, group index has valid data
int check_otp_wb(uint index, struct msm_sensor_ctrl_t *s_ctrl)
{

uint  flag,i;
uint32_t address;
// select bank 0
write_sccb16(0x3d84, 0x08,s_ctrl);
// load otp to buffer
write_sccb16(0x3d81, 0x01,s_ctrl);
msleep(3);
// read flag
address = 0x3d05 + index*9;
flag = read_sccb16(address,s_ctrl);
// disable otp read
write_sccb16(0x3d81, 0x00,s_ctrl);
// clear otp buffer
address = 0x3d00;
for (i=0; i<32;i++) {
write_sccb16(address + i, 0x00,s_ctrl);
}
if (!flag) {
return 0;
} else if ((!(flag &0x80)) && (flag&0x7f)) {
return 2;
} else {
return 1;
}

return 1;
}

// index: index of otp group. (0, 1, 2)
// return: 0, group index is empty
// 1, group index has invalid data
// 2, group index has valid data
int check_otp_lenc(uint index, struct msm_sensor_ctrl_t *s_ctrl)
{

uint  flag,i;
uint32_t address;
// select bank: index*2 + 1
write_sccb16(0x3d84, 0x09 + index*2,s_ctrl);
// read otp
write_sccb16(0x3d81, 0x01,s_ctrl);
msleep(3);
address = 0x3d00;
flag = read_sccb16(address,s_ctrl);
flag = flag &0xc0;
// disable otp read
write_sccb16(0x3d81, 0x00,s_ctrl);
// clear otp buffer
address = 0x3d00;
for (i=0; i<32;i++) {
write_sccb16(address + i, 0x00,s_ctrl);
}
if (!flag) {
return 0;
} else if (
flag == 0x40) {
return 2;
} else {
return 1;
}


}


// index: index of otp group. (0, 1, 2)
// return: 0,
int read_otp_wb(uint index, struct otp_struct *p_otp, struct msm_sensor_ctrl_t *s_ctrl)
{

int i;
uint32_t address;
// select bank 0
write_sccb16(0x3d84, 0x08,s_ctrl);
// read otp
write_sccb16(0x3d81, 0x01,s_ctrl);
msleep(3);
address = 0x3d05 + index*9;
p_otp->module_integrator_id = (read_sccb16(address,s_ctrl) & 0x7f);
p_otp->lens_id = read_sccb16(address + 1,s_ctrl);
p_otp->rg_ratio = read_sccb16(address + 2,s_ctrl);
p_otp->bg_ratio = read_sccb16(address + 3,s_ctrl);
p_otp->user_data[0] = read_sccb16(address + 4,s_ctrl);
p_otp->user_data[1] = read_sccb16(address + 5,s_ctrl);
p_otp->user_data[2] = read_sccb16(address + 6,s_ctrl);
p_otp->user_data[3] = read_sccb16(address + 7,s_ctrl);
p_otp->user_data[4] = read_sccb16(address + 8,s_ctrl);
pr_err("otpwb %s  module_integrator_id =0x%x",__func__,p_otp->module_integrator_id);
pr_err("otpwb %s  lens_id =0x%x",__func__,p_otp->lens_id);
pr_err("otpwb %s  rg_ratio =0x%x",__func__,p_otp->rg_ratio);
pr_err("otpwb %s  bg_ratio =0x%x",__func__,p_otp->bg_ratio);
pr_err("otpwb %s  user_data[0] =0x%x",__func__,p_otp->user_data[0]);
pr_err("otpwb %s  user_data[1] =0x%x",__func__,p_otp->user_data[1]);
pr_err("otpwb %s  user_data[2] =0x%x",__func__,p_otp->user_data[2]);
pr_err("otpwb %s  user_data[3] =0x%x",__func__,p_otp->user_data[3]);
pr_err("otpwb %s  user_data[4] =0x%x",__func__,p_otp->user_data[4]);
// disable otp read
write_sccb16(0x3d81, 0x00,s_ctrl);
// clear otp buffer
address = 0x3d00;
for (i=0;i<32;i++) {
write_sccb16(address + i, 0x00,s_ctrl);
}


return 0;
}

// index: index of otp group. (0, 1, 2)
// return: 0
int read_otp_lenc(uint index, struct otp_struct *p_otp, struct msm_sensor_ctrl_t *s_ctrl)
{

uint bank, temp, i;
uint32_t address;
// select bank: index*2 + 1
bank = index*2 + 1;
temp = 0x08 | bank;
write_sccb16(0x3d84, temp,s_ctrl);
// read otp
write_sccb16(0x3d81, 0x01,s_ctrl);
msleep(3);
address = 0x3d01;
for (i=0; i<31; i++) {
p_otp->lenc[i] = read_sccb16(address,s_ctrl);
address++;
}
// disable otp read
write_sccb16(0x3d81, 0x00,s_ctrl);
// clear otp buffer
address = 0x3d00;
for (i=0; i<32;i++) {
write_sccb16(address + i, 0x00,s_ctrl);
}
// select next bank
bank++;
temp = 0x08 | bank;
write_sccb16(0x3d84, temp,s_ctrl);
// read otp
write_sccb16(0x3d81, 0x01,s_ctrl);
msleep(3);
address = 0x3d00;
for (i=31; i<62; i++) {
p_otp->lenc[i] = read_sccb16(address,s_ctrl);
address++;
}
// disable otp read
write_sccb16(0x3d81, 0x00,s_ctrl);
// clear otp buffer
address = 0x3d00;
for (i=0; i<32;i++) {
write_sccb16(address + i, 0x00,s_ctrl);
}

return 0;
}

//R_gain: red gain of sensor AWB, 0x400 = 1
// G_gain: green gain of sensor AWB, 0x400 = 1
// B_gain: blue gain of sensor AWB, 0x400 = 1
// return 0
int update_awb_gain(uint32_t R_gain, uint32_t G_gain, uint32_t B_gain, struct msm_sensor_ctrl_t *s_ctrl)
{

pr_err("otpwb %s  R_gain =%x  0x3400=%d",__func__,R_gain,R_gain>>8);
pr_err("otpwb %s  R_gain =%x  0x3401=%d",__func__,R_gain,R_gain & 0x00ff);
pr_err("otpwb %s  G_gain =%x  0x3402=%d",__func__,G_gain,G_gain>>8);
pr_err("otpwb %s  G_gain =%x  0x3403=%d",__func__,G_gain,G_gain & 0x00ff);
pr_err("otpwb %s  B_gain =%x  0x3404=%d",__func__,B_gain,B_gain>>8);
pr_err("otpwb %s  B_gain =%x  0x3405=%d",__func__,B_gain,B_gain & 0x00ff);

if (R_gain>0x400) {
write_sccb16(0x3400, R_gain>>8,s_ctrl);
write_sccb16(0x3401, R_gain & 0x00ff,s_ctrl);
}
if (G_gain>0x400) {
write_sccb16(0x3402, G_gain>>8,s_ctrl);
write_sccb16(0x3403, G_gain & 0x00ff,s_ctrl);
}
if (B_gain>0x400) {
write_sccb16(0x3404, B_gain>>8,s_ctrl);
write_sccb16(0x3405, B_gain & 0x00ff,s_ctrl);
}


return 0;
}



int update_lenc(struct otp_struct otp, struct msm_sensor_ctrl_t *s_ctrl)
{

uint i, temp;
temp = read_sccb16(0x5000,s_ctrl);
temp |= 0x80;
write_sccb16(0x5000, temp,s_ctrl);
for(i=0;i<62;i++) {
write_sccb16(0x5800+i, otp.lenc[i],s_ctrl);
}
 return 0;
}
#if 0
#define RG_TYPICAL 0x5a
#define BG_TYPICAL 0x57
#endif
// R/G and B/G of typical camera module is defined here
uint RG_Ratio_typical ;
uint BG_Ratio_typical ;

// call this function after OV8825 initialization
// return value: 0 update success
// 1, no OTP
int update_otp_wb(struct msm_sensor_ctrl_t *s_ctrl) 
{
uint i, temp, otp_index;

uint32_t R_gain, G_gain, B_gain, G_gain_R, G_gain_B;
// R/G and B/G of current camera module is read out from sensor OTP
// check first wb OTP with valid data
for(i=0;i<3;i++) {
temp = check_otp_wb(i,s_ctrl);
pr_err("otpwb %s  temp =%d  i=%d",__func__,temp,i);
if (temp == 2) {
otp_index = i;
break;
}
}
if (i==3) {
pr_err("otpwb %s  i=%d   no valid wb OTP data ",__func__,i);
return 1;
}
read_otp_wb(otp_index, &current_wb_otp,s_ctrl);

if(current_wb_otp.module_integrator_id==0x02){
//truly
BG_Ratio_typical=0x49;
RG_Ratio_typical=0x4d;
}else if(current_wb_otp.module_integrator_id==0x31){
//mcnex
BG_Ratio_typical=0x53;
RG_Ratio_typical=0x55;
}else{
//qtech
BG_Ratio_typical=0x53;
RG_Ratio_typical=0x57;
}


//calculate gain
//0x400 = 1x gain
if(current_wb_otp.bg_ratio < BG_Ratio_typical)
{
if (current_wb_otp.rg_ratio < RG_Ratio_typical)
{
// current_otp.bg_ratio < BG_Ratio_typical &&
// current_otp.rg_ratio < RG_Ratio_typical
G_gain = 0x400;
B_gain = 0x400 * BG_Ratio_typical / current_wb_otp.bg_ratio;
R_gain = 0x400 * RG_Ratio_typical / current_wb_otp.rg_ratio;
} else
{
// current_otp.bg_ratio < BG_Ratio_typical &&
// current_otp.rg_ratio >= RG_Ratio_typical
R_gain = 0x400;
G_gain = 0x400 * current_wb_otp.rg_ratio / RG_Ratio_typical;
B_gain = G_gain * BG_Ratio_typical / current_wb_otp.bg_ratio;
}
} else
{
if (current_wb_otp.rg_ratio < RG_Ratio_typical)
{
// current_otp.bg_ratio >= BG_Ratio_typical &&
// current_otp.rg_ratio < RG_Ratio_typical
B_gain = 0x400;
G_gain = 0x400 * current_wb_otp.bg_ratio / BG_Ratio_typical;
R_gain = G_gain * RG_Ratio_typical / current_wb_otp.rg_ratio;
} else
{
// current_otp.bg_ratio >= BG_Ratio_typical &&
// current_otp.rg_ratio >= RG_Ratio_typical
G_gain_B = 0x400 * current_wb_otp.bg_ratio / BG_Ratio_typical;
G_gain_R = 0x400 * current_wb_otp.rg_ratio / RG_Ratio_typical;
if(G_gain_B > G_gain_R )
{
B_gain = 0x400;
G_gain = G_gain_B;
R_gain = G_gain * RG_Ratio_typical / current_wb_otp.rg_ratio;
}
else
{
R_gain = 0x400;
G_gain = G_gain_R;
B_gain = G_gain * BG_Ratio_typical / current_wb_otp.bg_ratio;
}
}
}
// write sensor wb gain to registers
update_awb_gain(R_gain, G_gain, B_gain,s_ctrl);
return 0;
}

// call this function after ov8825 initialization
// return value: 0 update success
// 1, no OTP
int update_otp_lenc(struct msm_sensor_ctrl_t *s_ctrl) 
{

uint i, temp, otp_index;

// check first lens correction OTP with valid data

for(i=0;i<3;i++) {
temp = check_otp_lenc(i,s_ctrl);
if (temp == 2) {
pr_err("otplenc %s  temp =%d  i=%d",__func__,temp,i);
otp_index = i;
break;
}
}
if (i==3) {
// no lens correction data
pr_err("otplenc %s  i=%d   no valid lenc OTP data ",__func__,i);
return 1;
}
read_otp_lenc(otp_index, &current_lenc_otp,s_ctrl);
update_lenc(current_lenc_otp,s_ctrl);
//success

return 0;
}


int32_t msm_ov8825_sensor_mode_init(struct msm_sensor_ctrl_t *s_ctrl,
			int mode, struct sensor_init_cfg *init_info)
{
	int32_t rc = 0;
	s_ctrl->fps_divider = Q10;
	s_ctrl->cam_mode = MSM_SENSOR_MODE_INVALID;

	pr_err("%s: %d\n", __func__, __LINE__);
	if (mode != s_ctrl->cam_mode) {
		s_ctrl->curr_res = MSM_SENSOR_INVALID_RES;
		s_ctrl->cam_mode = mode;
	}
	return rc;
}
int32_t msm_ov8825_sensor_match_id(struct msm_sensor_ctrl_t *s_ctrl)
{
	int32_t rc = 0;
	uint16_t chipid = 0;
	rc = msm_camera_i2c_read(
			s_ctrl->sensor_i2c_client,
			s_ctrl->sensor_id_info->sensor_id_reg_addr, &chipid,
			MSM_CAMERA_I2C_WORD_DATA);
	if (rc < 0) {
		pr_err("%s: read id failed\n", __func__);
		return rc;
	}

	pr_err("msm_sensor id: 0x%0x\n", chipid);
	if ((chipid != 0x8820)&&(chipid != 0x8825)) {
		pr_err("msm_sensor_match_id chip id doesnot match\n");
		return -ENODEV;
	}
	return rc;
}
static int32_t ov8825_write_exp_gain(struct msm_sensor_ctrl_t *s_ctrl,
		uint16_t gain, uint32_t line)
{
	uint32_t fl_lines, offset;
	uint8_t int_time[3];

	fl_lines =
		(s_ctrl->curr_frame_length_lines * s_ctrl->fps_divider) / Q10;
	offset = s_ctrl->sensor_exp_gain_info->vert_offset;
	if (line > (fl_lines - offset))
		fl_lines = line + offset;

	s_ctrl->func_tbl->sensor_group_hold_on(s_ctrl);
	msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
		s_ctrl->sensor_output_reg_addr->frame_length_lines, fl_lines,
		MSM_CAMERA_I2C_WORD_DATA);
	int_time[0] = line >> 12;
	int_time[1] = line >> 4;
	int_time[2] = line << 4;
	msm_camera_i2c_write_seq(s_ctrl->sensor_i2c_client,
		s_ctrl->sensor_exp_gain_info->coarse_int_time_addr-1,
		&int_time[0], 3);
	msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
		s_ctrl->sensor_exp_gain_info->global_gain_addr, gain,
		MSM_CAMERA_I2C_WORD_DATA);
	s_ctrl->func_tbl->sensor_group_hold_off(s_ctrl);
	return 0;
}


int32_t msm_ov8825_sensor_i2c_probe(struct i2c_client *client,
	const struct i2c_device_id *id)
{
	int rc = 0;
	struct msm_sensor_ctrl_t *s_ctrl;
	pr_err("%s_i2c_probe called\n", client->name);
	if (!i2c_check_functionality(client->adapter, I2C_FUNC_I2C)) {
		pr_err("i2c_check_functionality failed\n");
		rc = -EFAULT;
		return rc;
	}

	s_ctrl = (struct msm_sensor_ctrl_t *)(id->driver_data);
	if (s_ctrl->sensor_i2c_client != NULL) {
		s_ctrl->sensor_i2c_client->client = client;
		if (s_ctrl->sensor_i2c_addr != 0)
			s_ctrl->sensor_i2c_client->client->addr =
				s_ctrl->sensor_i2c_addr;
	} else {
		rc = -EFAULT;
		return rc;
	}

	s_ctrl->sensordata = client->dev.platform_data;
	if (s_ctrl->sensordata == NULL) {
		pr_err("%s: NULL sensor data\n", __func__);
		return -EFAULT;
	}
	
	rc=gpio_direction_output(54, 0);
	if (rc < 0)  
	pr_err("%s pwd gpio54  direction 0   failed\n",__func__);

	mdelay(20);
	
	s_ctrl->reg_ptr = kzalloc(sizeof(struct regulator *)
			* s_ctrl->sensordata->sensor_platform_info->num_vreg, GFP_KERNEL);
	if (!s_ctrl->reg_ptr) {
		pr_err("%s: could not allocate mem for regulators\n",
			__func__);
		return -ENOMEM;
	}
	
	//wangtao msm_camera_power_on(&s_ctrl->sensor_i2c_client->client->dev,s_ctrl->sensordata,s_ctrl->reg_ptr);
	//msm_camera_ov8825_power_on(s_ctrl);
	
	if (s_ctrl->clk_rate != 0)
		cam_clk_info->clk_rate = s_ctrl->clk_rate;
	
	rc = msm_cam_clk_enable(&s_ctrl->sensor_i2c_client->client->dev,
		cam_clk_info, s_ctrl->cam_clk, ARRAY_SIZE(cam_clk_info), 1);
	if (rc < 0) {
		pr_err("%s: clk enable failed\n", __func__);
		goto enable_clk_failed;
	}
	mdelay(5);
	
	rc = msm_camera_request_gpio_table(s_ctrl->sensordata, 1);
	if (rc < 0) {
		pr_err("%s: request gpio failed\n", __func__);
		goto request_gpio_failed;
	}
	rc = msm_camera_config_gpio_table(s_ctrl->sensordata, 1);
	if (rc < 0) {
		pr_err("%s: config gpio failed\n", __func__);
		goto config_gpio_failed;
	}
	
	mdelay(20);
	
#ifdef CONFIG_SENSOR_INFO
    	msm_sensorinfo_set_back_sensor_id(s_ctrl->sensor_id_info->sensor_id);
#else
  //do nothing here
#endif

	rc = msm_ov8825_sensor_match_id(s_ctrl);
	if (rc < 0)
		goto probe_fail;

	if (s_ctrl->func_tbl->sensor_setting(s_ctrl, MSM_SENSOR_REG_INIT, 1) < 0)
	{
		pr_err("%s  sensor_setting init  failed\n",__func__);
		return rc;
	}	
	msm_sensor_write_res_settings(s_ctrl, 1);//added for reduceing the high electricity	in standby after sleep
	rc=msm_camera_i2c_write(s_ctrl->sensor_i2c_client,0x0100, 0x01,
		      MSM_CAMERA_I2C_BYTE_DATA);
	msleep(10);
	update_otp_wb(s_ctrl);//added for wb otp

	pr_err("%s  current_wb_otp id =0x%x\n",__func__,current_wb_otp.module_integrator_id);
	if(current_wb_otp.module_integrator_id==0x02||current_wb_otp.module_integrator_id==0x31){
		
		update_otp_lenc(s_ctrl);
		
	}else{
	
	pr_err("%s  this is qtech module \n",__func__);
	rc = msm_camera_i2c_write_tbl(s_ctrl->sensor_i2c_client, qtech_lens_settings,
		ARRAY_SIZE(qtech_lens_settings), MSM_CAMERA_I2C_BYTE_DATA);	
	
	}
	
	snprintf(s_ctrl->sensor_v4l2_subdev.name,
		sizeof(s_ctrl->sensor_v4l2_subdev.name), "%s", id->name);
	v4l2_i2c_subdev_init(&s_ctrl->sensor_v4l2_subdev, client,
		s_ctrl->sensor_v4l2_subdev_ops);

	msm_sensor_register(&s_ctrl->sensor_v4l2_subdev);

	msm_sensor_enable_debugfs(s_ctrl);
	goto power_down;
config_gpio_failed:
		msm_camera_enable_vreg(&s_ctrl->sensor_i2c_client->client->dev,
			s_ctrl->sensordata->sensor_platform_info->cam_vreg,
			s_ctrl->sensordata->sensor_platform_info->num_vreg,
			s_ctrl->vreg_seq,
			s_ctrl->num_vreg_seq,
			s_ctrl->reg_ptr, 0);
request_gpio_failed:
	kfree(s_ctrl->reg_ptr);
enable_clk_failed:
		msm_camera_config_gpio_table(s_ctrl->sensordata, 0);
probe_fail:
	pr_err("%s_i2c_probe failed\n", client->name);
power_down:
	if (rc > 0)
		rc = 0;
	s_ctrl->func_tbl->sensor_power_down(s_ctrl);
    s_ctrl->sensor_state = MSM_SENSOR_POWER_DOWN;
	return rc;
}
static const struct i2c_device_id ov8825_i2c_id[] = {
	{SENSOR_NAME, (kernel_ulong_t)&ov8825_s_ctrl},
	{ }
};

static struct i2c_driver ov8825_i2c_driver = {
	.id_table = ov8825_i2c_id,
	.probe  = msm_ov8825_sensor_i2c_probe,
	.driver = {
		.name = SENSOR_NAME,
	},
};

static struct msm_camera_i2c_client ov8825_sensor_i2c_client = {
	.addr_type = MSM_CAMERA_I2C_WORD_ADDR,
};


int32_t msm_ov8825_sensor_power_up(struct msm_sensor_ctrl_t *s_ctrl)
{
	int32_t rc = 0;

	pr_err("%s: E\n", __func__);


	if (s_ctrl->clk_rate != 0)
		cam_clk_info->clk_rate = s_ctrl->clk_rate;
	
	rc = msm_cam_clk_enable(&s_ctrl->sensor_i2c_client->client->dev,
		cam_clk_info, s_ctrl->cam_clk, ARRAY_SIZE(cam_clk_info), 1);
	if (rc < 0) {
		pr_err("%s: clk enable failed\n", __func__);
		goto enable_clk_failed;
	}
	mdelay(1);
	
	rc=gpio_direction_output(54, 1);
	if (rc < 0)  
	pr_err("%s pwd gpio54  direction 1   failed\n",__func__);

	mdelay(10);
	
	rc=msm_camera_i2c_write(s_ctrl->sensor_i2c_client,0x3018, 0x00,
		      MSM_CAMERA_I2C_BYTE_DATA);
     if (rc < 0) {
      pr_err("%s: i2c write failed\n", __func__);
      return rc;
      }
	return rc;

enable_clk_failed:
	msm_camera_config_gpio_table(s_ctrl->sensordata, 0);
	kfree(s_ctrl->reg_ptr);

	return rc;
}


int32_t msm_ov8825_sensor_power_down(struct msm_sensor_ctrl_t *s_ctrl)
{
	int32_t rc = 0;
	pr_err("%s\n", __func__);


	rc=msm_camera_i2c_write(s_ctrl->sensor_i2c_client,0x3018, 0x18,
		      MSM_CAMERA_I2C_BYTE_DATA);

	
      if (rc < 0) {
      pr_err("%s: i2c write failed\n", __func__);
      return rc;
      }
	
	rc=gpio_direction_output(54, 0);
	if (rc < 0)  
		pr_err("%s pwd gpio54  direction 0   failed\n",__func__);
	usleep_range(1000, 2000);
	
	msm_cam_clk_enable(&s_ctrl->sensor_i2c_client->client->dev,
		cam_clk_info, s_ctrl->cam_clk, ARRAY_SIZE(cam_clk_info), 0);
	kfree(s_ctrl->reg_ptr);

	return rc;
}



static int __init msm_sensor_init_module(void)
{
	return i2c_add_driver(&ov8825_i2c_driver);
}

static struct v4l2_subdev_core_ops ov8825_subdev_core_ops = {
	.ioctl = msm_sensor_subdev_ioctl,
	.s_power = msm_sensor_power,
};


static struct v4l2_subdev_video_ops ov8825_subdev_video_ops = {
	.enum_mbus_fmt = msm_sensor_v4l2_enum_fmt,
};

static struct v4l2_subdev_ops ov8825_subdev_ops = {
	.core = &ov8825_subdev_core_ops,
	.video  = &ov8825_subdev_video_ops,
};

static struct msm_sensor_fn_t ov8825_func_tbl = {
	.sensor_start_stream = msm_sensor_start_stream,
	.sensor_stop_stream = msm_sensor_stop_stream,
	.sensor_group_hold_on = msm_sensor_group_hold_on,
	.sensor_group_hold_off = msm_sensor_group_hold_off,
	.sensor_set_fps = msm_sensor_set_fps,
	.sensor_write_exp_gain = ov8825_write_exp_gain,
	.sensor_write_snapshot_exp_gain = ov8825_write_exp_gain,
	.sensor_setting =msm_sensor_setting, //msm_ov8825_sensor_setting,
	.sensor_set_sensor_mode = msm_sensor_set_sensor_mode,
	.sensor_mode_init = msm_ov8825_sensor_mode_init,
	.sensor_get_output_info = msm_sensor_get_output_info,
	.sensor_config = msm_sensor_config,
	.sensor_power_up = msm_ov8825_sensor_power_up,
	.sensor_power_down = msm_ov8825_sensor_power_down,
	.sensor_match_id=msm_ov8825_sensor_match_id,
	.sensor_get_csi_params = msm_sensor_get_csi_params,
};

static struct msm_sensor_reg_t ov8825_regs = {
	.default_data_type = MSM_CAMERA_I2C_BYTE_DATA,
	.start_stream_conf = ov8825_start_settings,
	.start_stream_conf_size = ARRAY_SIZE(ov8825_start_settings),
	.stop_stream_conf = ov8825_stop_settings,
	.stop_stream_conf_size = ARRAY_SIZE(ov8825_stop_settings),
	.group_hold_on_conf = ov8825_groupon_settings,
	.group_hold_on_conf_size = ARRAY_SIZE(ov8825_groupon_settings),
	.group_hold_off_conf = ov8825_groupoff_settings,
	.group_hold_off_conf_size =	ARRAY_SIZE(ov8825_groupoff_settings),
	.init_settings = &ov8825_init_conf[0],
	.init_size = ARRAY_SIZE(ov8825_init_conf),
	.mode_settings = &ov8825_confs[0],
	.output_settings = &ov8825_dimensions[0],
	.num_conf = ARRAY_SIZE(ov8825_confs),
};

static struct msm_sensor_ctrl_t ov8825_s_ctrl = {
	.msm_sensor_reg = &ov8825_regs,
	.sensor_i2c_client = &ov8825_sensor_i2c_client,
	.sensor_i2c_addr = 0x6C,
	.vreg_seq = ov8825_veg_seq,
	.num_vreg_seq = ARRAY_SIZE(ov8825_veg_seq),
	.sensor_output_reg_addr = &ov8825_reg_addr,
	.sensor_id_info = &ov8825_id_info,
	.sensor_exp_gain_info = &ov8825_exp_gain_info,
	.cam_mode = MSM_SENSOR_MODE_INVALID,
	//.csi_params = &ov8825_csi_params_array[0],
	.msm_sensor_mutex = &ov8825_mut,
	.sensor_i2c_driver = &ov8825_i2c_driver,
	.sensor_v4l2_subdev_info = ov8825_subdev_info,
	.sensor_v4l2_subdev_info_size = ARRAY_SIZE(ov8825_subdev_info),
	.sensor_v4l2_subdev_ops = &ov8825_subdev_ops,
	.func_tbl = &ov8825_func_tbl,
	.clk_rate = MSM_SENSOR_MCLK_24HZ,
	.msm_sensor_reg_default_data_type=MSM_CAMERA_I2C_BYTE_DATA,
};

module_init(msm_sensor_init_module);
MODULE_DESCRIPTION("OV 8MP Bayer sensor driver");
MODULE_LICENSE("GPL v2");


