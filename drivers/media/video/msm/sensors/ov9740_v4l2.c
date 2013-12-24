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
#include <mach/msm_bus.h>
#include <mach/msm_bus_board.h>
#include "msm_sensor_common.h"

#include "msm_sensor.h"
#include "msm.h"
#include "msm_ispif.h"
#include "msm_camera_i2c_mux.h"
#define SENSOR_NAME "ov9740"
#define PLATFORM_DRIVER_NAME "msm_camera_ov9740"
#define ov9740_obj ov9740_##obj
#ifdef CONFIG_MACH_COEUS
static int IsPowered;
#define OV9740_MODE_SELECT		0x0100
#endif
static int g_wb_value=0;  //yanwei
static int g_iso_value=0;  //yanwei

extern void msm_sensorinfo_set_front_sensor_id(uint16_t id);

DEFINE_MUTEX(ov9740_mut);
static struct msm_sensor_ctrl_t ov9740_s_ctrl;


static struct msm_camera_i2c_reg_conf ov9740_prev_settings[] = 
{

	//Orientation
	#if 1
	{0x0101 ,0x02},//yuansheng
	{0x0347 ,0x05},//Y start	 893U10
	{0x034b ,0xd9},// Y end	893U10	
	#else	
	{0x0101 ,0x01},//bit1 flip bit0 mirror 893U10 893A20 893A21
	{0x0347 ,0x04},//Y start top left pixel data
	{0x034b ,0xd8},// Y end  bottom right pixel data	
	#endif		
	//PLL setting
	{0x3104 ,0x20},// PLL mode control
	{0x0305 ,0x03},// PLL control
	{0x0307 ,0x4c},// PLL control
	{0x0303 ,0x01},// PLL control
	{0x0301 ,0x08},// PLL control
	{0x3010 ,0x01},// PLL control
	//Timing setting
	{0x0340 ,0x03},//VTS
	{0x0341 ,0x07},//VTS
	{0x0342 ,0x06},//HTS
	{0x0343 ,0x62},//HTS
	{0x0344 ,0x00},// X start
	{0x0345 ,0x08},//X start
	{0x0346 ,0x00},//Y start
	//{0x0347 ,0x04},//Y start top left pixel data
	{0x0348 ,0x05},//X end
	{0x0349 ,0x0c},//X end
	{0x034a ,0x02},//Y end
	//{0x034b ,0xd8},// Y end  bottom right pixel data
	{0x034c ,0x05},// H output size
	{0x034d ,0x00},//H output size
	{0x034e ,0x02},//V output size
	{0x034f ,0xd0},//V output size
	//Output select 
	{0x3002 ,0x00},// IO control
	{0x3004 ,0x00},// IO control
	{0x3005 ,0x00},// IO control
	{0x3012 ,0x70},// MIPI control
	{0x3013 ,0x60},//MIPI control
	{0x3014 ,0x01},//MIPI control
	{0x301f ,0x03},// MIPI control
	{0x3026 ,0x00},// Output select
	{0x3027 ,0x00},// Output select
	//Analog control
	{0x3601 ,0x40},// Analog control
	{0x3602 ,0x16},// Analog control
	{0x3603 ,0xaa},// Analog control
	{0x3604 ,0x0c},// Analog control
	{0x3610 ,0xa1},// Analog control
	{0x3612 ,0x24},// Analog control
	{0x3620 ,0x66},// Analog control
	{0x3621 ,0xc0},// Analog control
	{0x3622 ,0x9f},// Analog control
	{0x3630 ,0xd2},// Analog control
	{0x3631 ,0x5e},// Analog control
	{0x3632 ,0x27},// Analog control
	{0x3633 ,0x50},// Analog control
	// Sensor control
	{0x3703 ,0x42},// Sensor control 
	{0x3704 ,0x10},// Sensor control 
	{0x3705 ,0x45},// Sensor control 
	{0x3707 ,0x11},// Sensor control 
	//Timing control
	{0x3817 ,0x94},// Internal timing control
	{0x3819 ,0x6e},// Internal timing control
	{0x3831 ,0x40},// Digital gain enable
	{0x3833 ,0x04},// Internal timing control
	{0x3835 ,0x04},// Internal timing control
	{0x3837 ,0x01},
	//Internal timing control;; AEC/AGC control
	{0x3503 ,0x10},//AEC/AGC control
	{0x3a18 ,0x01},//Gain ceiling
	{0x3a19 ,0xB5},//Gain ceiling
	{0x3a1a ,0x05},//Max diff
	{0x3a11 ,0x90},// High threshold
	{0x3a1b ,0x53},// WPT 2 
	{0x3a0f ,0x50},// WPT  
	{0x3a10 ,0x4e},// BPT  
	{0x3a1e ,0x4b},// BPT 2 
	{0x3a1f ,0x22},//; Low threshold 
	//Banding filter
	{0x3a08 ,0x00},// 50Hz banding step
	{0x3a09 ,0xe8},// 50Hz banding step	
	{0x3a0e ,0x03},// 50Hz banding Max
	{0x3a14 ,0x15},// 50Hz Max exposure
	{0x3a15 ,0xc6},// 50Hz Max exposure
	{0x3a0a ,0x00},// 60Hz banding step
	{0x3a0b ,0xc0},// 60Hz banding step
	{0x3a0d ,0x04},// 60Hz banding Max
	{0x3a02 ,0x18},// 60Hz Max exposure
	{0x3a03 ,0x20},// 60Hz Max exposure

	//50/60 detection
	{0x3c0a ,0x9c},// Number of samples
	{0x3c0b ,0x3f},// Number of samples

	// BLC control
	{0x4002 ,0x45},// BLC auto enable
	{0x4005 ,0x18},// BLC mode

	// VFIFO control
	{0x4601 ,0x16},// VFIFO control
	{0x460e ,0x82},	
	// VFIFO control

	// DVP control
	{0x4702 ,0x04},// Vsync control
	{0x4704 ,0x00},// Vsync mode 
	{0x4706 ,0x08},// Vsync control

	//MIPI control
	{0x4800 ,0x44},// MIPI control
	{0x4801 ,0x0f},//
	// MIPI control
	{0x4803 ,0x05},
	// MIPI control
	{0x4805 ,0x10},
	// MIPI control
	{0x4837 ,0x20},////niujing modify mipi before 0x30
	{0x4831 ,0x07},////niujing modify mipi HS_Prepare	
	{0x482a ,0x15},//HS_Prepare+HS_zero niujing modify mipi before 
	{0x482b ,0x15},//HS-trail niujing modify mipi before 
	// MIPI control;; ISP control
	{0x5000 ,0xff},// [7] LC [6] Gamma [3] DNS [2] BPC [1] WPC [0] CIP
	{0x5001 ,0xff},// [7] SDE [6] UV adjust [4] scale [3] contrast [2] UV average [1] CMX [0] AWB
	{0x5003 ,0xff},// [7] PAD [5] Buffer [3] Vario [1] BLC [0] AWB gain
    //;;AWB
	{0x5180 ,0xf0}, //AWB setting
	{0x5181 ,0x00}, //;AWB setting
	{0x5182 ,0x41}, //AWB setting 
	{0x5183 ,0x42}, //AWB setting
	{0x5184 ,0x80}, //AWB setting
	{0x5185 ,0x68}, //AWB setting
	{0x5186 ,0x93}, //AWB setting 
	{0x5187 ,0xa8}, //AWB setting
	{0x5188 ,0x17}, //AWB setting
	{0x5189 ,0x45}, //AWB setting
	{0x518a ,0x27}, //AWB setting
	{0x518b ,0x41}, //AWB setting
	{0x518c ,0x2d}, //AWB setting
	{0x518d ,0xf0}, //AWB setting
	{0x518e ,0x10}, //;AWB setting
	{0x518f ,0xff}, //AWB setting
	{0x5190 ,0x00}, //AWB setting
	{0x5191 ,0xff}, //AWB setting 
	{0x5192 ,0x00}, //;AWB setting
	{0x5193 ,0xff}, //AWB setting 
	{0x5194 ,0x00}, //AWB setting 

	// DNS
	{0x529a ,0x02}, //DNS setting
	{0x529b ,0x08}, //DNS setting
	{0x529c ,0x0a}, //DNS setting
	{0x529d ,0x10}, //DNS setting
	{0x529e ,0x10}, //;DNS setting
	{0x529f ,0x28}, //	;;DNS setting
	{0x52a0 ,0x32}, //;DNS setting
	{0x52a2 ,0x00}, //DNS setting 
	{0x52a3 ,0x02}, //DNS setting 
	{0x52a4 ,0x00}, //DNS setting 
	{0x52a5 ,0x04}, //DNS setting  
	{0x52a6 ,0x00}, //DNS setting  
	{0x52a7 ,0x08}, //DNS setting  
	{0x52a8 ,0x00}, //DNS setting  
	{0x52a9 ,0x10}, //;;DNS setting
	{0x52aa ,0x00}, //DNS setting  
	{0x52ab ,0x38}, //;;DNS setting
	{0x52ac ,0x00}, //DNS setting  
	{0x52ad ,0x3c}, //;DNS setting 
	{0x52ae ,0x00}, //DNS setting   
	{0x52af ,0x4c}, //;DNS setting

	//CIP
	{0x530d ,0x06}, //CIP setting

	//CMX
	{0x5380 ,0x01},  //CMX setting  
	{0x5381 ,0x00},  //CMX setting  
	{0x5382 ,0x00},  //CMX setting   
	{0x5383 ,0x0d},  //CMX setting  
	{0x5384 ,0x00},  //CMX setting  
	{0x5385 ,0x2f},  //CMX setting   
	{0x5386 ,0x00},  //CMX setting  
	{0x5387 ,0x00},  //CMX setting   
	{0x5388 ,0x00},  //CMX setting   
	{0x5389 ,0xd3},  //CMX setting  
	{0x538a ,0x00},  //CMX setting   
	{0x538b ,0x0f},  //CMX setting   
	{0x538c ,0x00},  //CMX setting   
	{0x538d ,0x00},  //CMX setting  
	{0x538e ,0x00},  //CMX setting  
	{0x538f ,0x32},  //CMX setting   
	{0x5390 ,0x00},  //CMX setting  
	{0x5391 ,0x94},  //CMX setting    
	{0x5392 ,0x00},  //CMX setting  
	{0x5393 ,0xa4},  //CMX setting  
	{0x5394 ,0x18},  //CMX setting 
	//Contrast
	{0x5401 ,0x2c}, // Contrast setting
	{0x5403 ,0x28}, // Contrast setting
	{0x5404 ,0x06}, // Contrast setting	
	{0x5405 ,0xe0}, // Contrast setting
     #if 1
	//Y Gamma
	{0x5480 ,0x04}, //Y Gamma setting  
	{0x5481 ,0x12}, //Y Gamma setting 
	{0x5482 ,0x27}, //Y Gamma setting  
	{0x5483 ,0x49}, //Y Gamma setting  
	{0x5484 ,0x57}, //Y Gamma setting  
	{0x5485 ,0x66}, //Y Gamma setting  
	{0x5486 ,0x75}, //Y Gamma setting  
	{0x5487 ,0x81}, //Y Gamma setting 
	{0x5488 ,0x8c}, //Y Gamma setting 
	{0x5489 ,0x95}, //Y Gamma setting 
	{0x548a ,0xa5}, //Y Gamma setting 
	{0x548b ,0xb2}, //Y Gamma setting 
	{0x548c ,0xc8}, //Y Gamma setting 
	{0x548d ,0xd9}, //Y Gamma setting 
	{0x548e ,0xec}, //Y Gamma setting 
    {0x548f ,0x0d}, //Y Gamma setting 
     #else
	{0x5480 ,0x10}, //Y Gamma setting 
	{0x5481 ,0x23}, //Y Gamma setting 
	{0x5482 ,0x36}, //Y Gamma setting 
	{0x5483 ,0x54}, //Y Gamma setting 
	{0x5484 ,0x5f}, //Y Gamma setting 
	{0x5485 ,0x6b}, //Y Gamma setting 
	{0x5486 ,0x78}, //Y Gamma setting 
	{0x5487 ,0x84}, //Y Gamma setting 
	{0x5488 ,0x8f}, //Y Gamma setting 
	{0x5489 ,0x99}, //Y Gamma setting 
	{0x548a ,0xab}, //Y Gamma setting 
	{0x548b ,0xb9}, //Y Gamma setting 
	{0x548c ,0xd0}, //Y Gamma setting 
	{0x548d ,0xe2}, //Y Gamma setting 
	{0x548e ,0xf2}, //Y Gamma setting 
	{0x548f ,0x12}, //Y Gamma setting     
     #endif
    {0x5688 ,0x00}, 
	{0x5689 ,0x00}, 
	{0x568a ,0x11},  //0x10
	{0x568b ,0x11},  //0x01
	{0x568c ,0x11},  //0x10
	{0x568d ,0x11},  //0x01
	{0x568e ,0x00}, 
	{0x568f ,0x00},  
	//UV Gamma
	{0x5490 ,0x01}, //UV Gamma setting 
	{0x5491 ,0xc0}, //UV Gamma setting 
	{0x5492 ,0x03}, //UV Gamma setting 
	{0x5493 ,0x00}, //UV Gamma setting 
	{0x5494 ,0x03}, //UV Gamma setting 
	{0x5495 ,0xe0}, //UV Gamma setting 
	{0x5496 ,0x03}, //UV Gamma setting 
	{0x5497 ,0x10}, //UV Gamma setting 
	{0x5498 ,0x02}, //UV Gamma setting 
	{0x5499 ,0xac}, //UV Gamma setting 
	{0x549a ,0x02}, //UV Gamma setting 
	{0x549b ,0x75}, //UV Gamma setting 
	{0x549c ,0x02}, //;UV Gamma setting 
	{0x549d ,0x44}, //UV Gamma setting 
	{0x549e ,0x02}, //UV Gamma setting 
	{0x549f ,0x20}, //UV Gamma setting 
	{0x54a0 ,0x02}, //UV Gamma setting 
	{0x54a1 ,0x07}, //UV Gamma setting 
	{0x54a2 ,0x01}, //UV Gamma setting 
	{0x54a3 ,0xec}, //UV Gamma setting 
	{0x54a4 ,0x01}, //UV Gamma setting 
	{0x54a5 ,0xc0}, //UV Gamma setting 
	{0x54a6 ,0x01}, //UV Gamma setting 
	{0x54a7 ,0x9b}, //UV Gamma setting 
	{0x54a8 ,0x01}, //UV Gamma setting 
	{0x54a9 ,0x63}, //UV Gamma setting 
	{0x54aa ,0x01}, //UV Gamma setting 
	{0x54ab ,0x2b}, //UV Gamma setting 
	{0x54ac ,0x01}, //UV Gamma setting 
	{0x54ad ,0x22}, //UV Gamma setting 
	// UV adjust
	{0x5501 ,0x1c}, //UV adjust setting 
	{0x5502 ,0x00}, //;UV adjust setting 
	{0x5503 ,0x40}, //UV adjust setting 
	{0x5504 ,0x00}, //UV adjust setting 
	{0x5505 ,0x80}, //UV adjust setting 
	//Lens correction
	{0x5800 ,0x1c}, // Lens correction setting
	{0x5801 ,0x16}, // Lens correction setting
	{0x5802 ,0x15}, // Lens correction setting
	{0x5803 ,0x16}, // Lens correction setting
	{0x5804 ,0x18}, // Lens correction setting
	{0x5805 ,0x1a}, // Lens correction setting
	{0x5806 ,0x0c}, // Lens correction setting
	{0x5807 ,0x0a}, // Lens correction setting
	{0x5808 ,0x08}, // Lens correction setting
	{0x5809 ,0x08}, // Lens correction setting
	{0x580a ,0x0a}, // Lens correction setting
	{0x580b ,0x0b}, // Lens correction setting
	{0x580c ,0x05}, // Lens correction setting
	{0x580d ,0x02}, // Lens correction setting
	{0x580e ,0x00}, // Lens correction setting
	{0x580f ,0x00}, // Lens correction setting
	{0x5810 ,0x02}, // Lens correction setting
	{0x5811 ,0x05}, // Lens correction setting
	{0x5812 ,0x04}, // Lens correction setting
    {0x5813 ,0x01}, // Lens correction setting
	{0x5814 ,0x00}, // Lens correction setting
	{0x5815 ,0x00}, // Lens correction setting
	{0x5816 ,0x02}, // Lens correction setting
	{0x5817 ,0x03}, // Lens correction setting
	{0x5818 ,0x0a}, // Lens correction setting
	{0x5819 ,0x07}, // Lens correction setting
	{0x581a ,0x05}, // Lens correction setting
	{0x581b ,0x05}, // Lens correction setting
	{0x581c ,0x08}, // Lens correction setting
	{0x581d ,0x0b}, // Lens correction setting
	{0x581e ,0x15}, // Lens correction setting
	{0x581f ,0x14}, // Lens correction setting
	{0x5820 ,0x14}, // Lens correction setting
	{0x5821 ,0x13}, // Lens correction setting
	{0x5822 ,0x17}, // Lens correction setting
	{0x5823 ,0x16}, // Lens correction setting
	{0x5824 ,0x46}, // Lens correction setting
	{0x5825 ,0x4c}, // Lens correction setting
	{0x5826 ,0x6c}, // Lens correction setting
	{0x5827 ,0x4c}, // Lens correction setting
	{0x5828 ,0x80}, // Lens correction setting
	{0x5829 ,0x2e}, // Lens correction setting
	{0x582a ,0x48}, // Lens correction setting
	{0x582b ,0x46}, // Lens correction setting
	{0x582c ,0x2a}, // Lens correction setting
	{0x582d ,0x68}, // Lens correction setting
	{0x582e ,0x08}, // Lens correction setting
	{0x582f ,0x26}, // Lens correction setting
	{0x5830 ,0x44}, // Lens correction setting
	{0x5831 ,0x46}, // Lens correction setting
	{0x5832 ,0x62}, // Lens correction setting
	{0x5833 ,0x0c}, // Lens correction setting
	{0x5834 ,0x28}, // Lens correction setting
	{0x5835 ,0x46}, // Lens correction setting
	{0x5836 ,0x28}, // Lens correction setting
	{0x5837 ,0x88}, // Lens correction setting
	{0x5838 ,0x0e}, // Lens correction setting
	{0x5839 ,0x0e}, // Lens correction setting
	{0x583a ,0x2c}, // Lens correction setting
	{0x583b ,0x2e}, // Lens correction setting
	{0x583c ,0x46}, // Lens correction setting
	{0x583d ,0xca}, // Lens correction setting
	{0x583e ,0xf0}, // Lens correction setting
	{0x5842 ,0x02}, // Lens correction setting
	{0x5843 ,0x5e}, // Lens correction setting
	{0x5844 ,0x04}, // Lens correction setting
	{0x5845 ,0x32}, // Lens correction setting
	{0x5846 ,0x03}, // Lens correction setting
	{0x5847 ,0x29}, // Lens correction setting
	{0x5848 ,0x02}, // Lens correction setting
	{0x5849 ,0xcc}, // Lens correction setting
	//{0x501a ,0xe0}, //colorbar0x80
	{0x4300 ,0x32}, // YUV shunxu
  {0x0340 ,0x03},
  {0x0341 ,0xc4}, //vts to 0x3c4
  {0x3c01 ,0x80}, //band manual mode enable
  {0x3a0d ,0x05}, //60Hz banding Max
  {0x3a18 ,0x00}, //Gain Ceiling 8x
  {0x3a19 ,0x80},
	
	// Start streaming
	//{0x0100 ,0x01}, // start streaming
	};
#if 0
static struct msm_camera_i2c_reg_conf ov9740_saturation[][30] = {
{//satutaion +4
{0x5490, 0x02}, ////UV List 0[11:8]
{0x5491, 0xc7}, ////UV List 0[7:0]
{0x5492, 0x04}, ////UV List 1[11:8]
{0x5493, 0xc3}, ////UV List 1[7:0]
{0x5494, 0x06}, ////UV List 2[11:8]
{0x5495, 0x26}, ////UV List 2[7:0]
{0x5496, 0x04}, ////UV List 3[11:8]
{0x5497, 0xdc}, ////UV List 3[7:0]
{0x5498, 0x04}, ////UV List 4[11:8]
{0x5499, 0x3d}, ////UV List 4[7:0]
{0x549a, 0x03}, ////UV List 5[11:8]
{0x549b, 0xe6}, ////UV List 5[7:0]
{0x549c, 0x03}, ////UV List 6[11:8]
{0x549d, 0x98}, ////UV List 6[7:0]
{0x549e, 0x03}, ////UV List 7[11:8]
{0x549f, 0x5f}, ////UV List 7[7:0]
{0x54a0, 0x03}, ////UV List 8[11:8]
{0x54a1, 0x37}, ////UV List 8[7:0]
{0x54a2, 0x03}, ////UV List 9[11:8]
{0x54a3, 0x0d}, ////UV List 9[7:0]
{0x54a4, 0x02}, ////UV List 10[11:8]
{0x54a5, 0xc7}, ////UV List 10[7:0]
{0x54a6, 0x02}, ////UV List 11[11:8]
{0x54a7, 0x8c}, ////UV List 11[7:0]
{0x54a8, 0x02}, ////UV List 12[11:8]
{0x54a9, 0x33}, ////UV List 12[7:0]
{0x54aa, 0x01}, ////UV List 13[11:8]
{0x54ab, 0xda}, ////UV List 13[7:0]
{0x54ac, 0x01}, ////UV List 14[11:8]
{0x54ad, 0xcc}, ////UV List 14[7:0]
},

{//satutaion +3
{0x5490, 0x02},////UV List 0[11:8]
{0x5491, 0x79},////UV List 0[7:0]
{0x5492, 0x04},////UV List 1[11:8]
{0x5493, 0x3e},////UV List 1[7:0]
{0x5494, 0x05},////UV List 2[11:8]
{0x5495, 0x7a},////UV List 2[7:0]
{0x5496, 0x04},////UV List 3[11:8]
{0x5497, 0x54},////UV List 3[7:0]
{0x5498, 0x03},////UV List 4[11:8]
{0x5499, 0xc7},////UV List 4[7:0]
{0x549a, 0x03},////UV List 5[11:8]
{0x549b, 0x79},////UV List 5[7:0]
{0x549c, 0x03},////UV List 6[11:8]
{0x549d, 0x34},////UV List 6[7:0]
{0x549e, 0x03},////UV List 7[11:8]
{0x549f, 0x01},////UV List 7[7:0]
{0x54a0, 0x02},////UV List 8[11:8]
{0x54a1, 0xdd},////UV List 8[7:0]
{0x54a2, 0x02},////UV List 9[11:8]
{0x54a3, 0xb7},////UV List 9[7:0]
{0x54a4, 0x02},////UV List 10[11:8]
{0x54a5, 0x79},////UV List 10[7:0]
{0x54a6, 0x02},////UV List 11[11:8]
{0x54a7, 0x45},////UV List 11[7:0]
{0x54a8, 0x01},////UV List 12[11:8]
{0x54a9, 0xf6},////UV List 12[7:0]
{0x54aa, 0x01},////UV List 13[11:8]
{0x54ab, 0xa6},////UV List 13[7:0]
{0x54ac, 0x01},////UV List 14[11:8]
{0x54ad, 0x9a},////UV List 14[7:0]

},

{//satutaion +2
{0x5490, 0x02}, ////UV List 0[11:8]
{0x5491, 0x34}, ////UV List 0[7:0]
{0x5492, 0x03}, ////UV List 1[11:8]
{0x5493, 0xc7}, ////UV List 1[7:0]
{0x5494, 0x04}, ////UV List 2[11:8]
{0x5495, 0xe1}, ////UV List 2[7:0]
{0x5496, 0x03}, ////UV List 3[11:8]
{0x5497, 0xdb}, ////UV List 3[7:0]
{0x5498, 0x03}, ////UV List 4[11:8]
{0x5499, 0x5d}, ////UV List 4[7:0]
{0x549a, 0x03}, ////UV List 5[11:8]
{0x549b, 0x18}, ////UV List 5[7:0]
{0x549c, 0x02}, ////UV List 6[11:8]
{0x549d, 0xda}, ////UV List 6[7:0]
{0x549e, 0x02}, ////UV List 7[11:8]
{0x549f, 0xad}, ////UV List 7[7:0]
{0x54a0, 0x02}, ////UV List 8[11:8]
{0x54a1, 0x8d}, ////UV List 8[7:0]
{0x54a2, 0x02}, ////UV List 9[11:8]
{0x54a3, 0x6b}, ////UV List 9[7:0]
{0x54a4, 0x02}, ////UV List 10[11:8]
{0x54a5, 0x34}, ////UV List 10[7:0]
{0x54a6, 0x02}, ////UV List 11[11:8]
{0x54a7, 0x05}, ////UV List 11[7:0]
{0x54a8, 0x01}, ////UV List 12[11:8]
{0x54a9, 0xbf}, ////UV List 12[7:0]
{0x54aa, 0x01}, ////UV List 13[11:8]
{0x54ab, 0x78}, ////UV List 13[7:0]
{0x54ac, 0x01}, ////UV List 14[11:8]
{0x54ad, 0x6d}, ////UV List 14[7:0]

},

{//satutaion +1
{0x5490, 0x01}, ////UV List 0[11:8]
{0x5491, 0xf6}, ////UV List 0[7:0]
{0x5492, 0x03}, ////UV List 1[11:8]
{0x5493, 0x5e}, ////UV List 1[7:0]
{0x5494, 0x04}, ////UV List 2[11:8]
{0x5495, 0x59}, ////UV List 2[7:0]
{0x5496, 0x03}, ////UV List 3[11:8]
{0x5497, 0x70}, ////UV List 3[7:0]
{0x5498, 0x02}, ////UV List 4[11:8]
{0x5499, 0xff}, ////UV List 4[7:0]
{0x549a, 0x02}, ////UV List 5[11:8]
{0x549b, 0xc2}, ////UV List 5[7:0]
{0x549c, 0x02}, ////UV List 6[11:8]
{0x549d, 0x8b}, ////UV List 6[7:0]
{0x549e, 0x02}, ////UV List 7[11:8]
{0x549f, 0x62}, ////UV List 7[7:0]
{0x54a0, 0x02}, ////UV List 8[11:8]
{0x54a1, 0x46}, ////UV List 8[7:0]
{0x54a2, 0x02}, ////UV List 9[11:8]
{0x54a3, 0x28}, ////UV List 9[7:0]
{0x54a4, 0x01}, ////UV List 10[11:8]
{0x54a5, 0xf6}, ////UV List 10[7:0]
{0x54a6, 0x01}, ////UV List 11[11:8]
{0x54a7, 0xcd}, ////UV List 11[7:0]
{0x54a8, 0x01}, ////UV List 12[11:8]
{0x54a9, 0x8e}, ////UV List 12[7:0]
{0x54aa, 0x01}, ////UV List 13[11:8]
{0x54ab, 0x4f}, ////UV List 13[7:0]
{0x54ac, 0x01}, ////UV List 14[11:8]
{0x54ad, 0x45}, ////UV List 14[7:0]

},

{//satutaion 0
{0x5490, 0x01}, ////UV List 0[11:8]
{0x5491, 0xc0}, ////UV List 0[7:0]
{0x5492, 0x03}, ////UV List 1[11:8]
{0x5493, 0x00}, ////UV List 1[7:0]
{0x5494, 0x03}, ////UV List 2[11:8]
{0x5495, 0xe0}, ////UV List 2[7:0]
{0x5496, 0x03}, ////UV List 3[11:8]
{0x5497, 0x10}, ////UV List 3[7:0]
{0x5498, 0x02}, ////UV List 4[11:8]
{0x5499, 0xac}, ////UV List 4[7:0]
{0x549a, 0x02}, ////UV List 5[11:8]
{0x549b, 0x75}, ////UV List 5[7:0]
{0x549c, 0x02}, ////UV List 6[11:8]
{0x549d, 0x44}, ////UV List 6[7:0]
{0x549e, 0x02}, ////UV List 7[11:8]
{0x549f, 0x20}, ////UV List 7[7:0]
{0x54a0, 0x02}, ////UV List 8[11:8]
{0x54a1, 0x07}, ////UV List 8[7:0]
{0x54a2, 0x01}, ////UV List 9[11:8]
{0x54a3, 0xec}, ////UV List 9[7:0]
{0x54a4, 0x01}, ////UV List 10[11:8]
{0x54a5, 0xc0}, ////UV List 10[7:0]
{0x54a6, 0x01}, ////UV List 11[11:8]
{0x54a7, 0x9b}, ////UV List 11[7:0]
{0x54a8, 0x01}, ////UV List 12[11:8]
{0x54a9, 0x63}, ////UV List 12[7:0]
{0x54aa, 0x01}, ////UV List 13[11:8]
{0x54ab, 0x2b}, ////UV List 13[7:0]
{0x54ac, 0x01}, ////UV List 14[11:8]
{0x54ad, 0x22}, ////UV List 14[7:0]

},

{//satutaion -1
{0x5490, 0x01}, ////UV List 0[11:8]
{0x5491, 0x8f}, ////UV List 0[7:0]
{0x5492, 0x02}, ////UV List 1[11:8]
{0x5493, 0xac}, ////UV List 1[7:0]
{0x5494, 0x03}, ////UV List 2[11:8]
{0x5495, 0x73}, ////UV List 2[7:0]
{0x5496, 0x02}, ////UV List 3[11:8]
{0x5497, 0xba}, ////UV List 3[7:0]
{0x5498, 0x02}, ////UV List 4[11:8]
{0x5499, 0x61}, ////UV List 4[7:0]
{0x549a, 0x02}, ////UV List 5[11:8]
{0x549b, 0x30}, ////UV List 5[7:0]
{0x549c, 0x02}, ////UV List 6[11:8]
{0x549d, 0x04}, ////UV List 6[7:0]
{0x549e, 0x01}, ////UV List 7[11:8]
{0x549f, 0xe4}, ////UV List 7[7:0]
{0x54a0, 0x01}, ////UV List 8[11:8]
{0x54a1, 0xce}, ////UV List 8[7:0]
{0x54a2, 0x01}, ////UV List 9[11:8]
{0x54a3, 0xb6}, ////UV List 9[7:0]
{0x54a4, 0x01}, ////UV List 10[11:8]
{0x54a5, 0x8f}, ////UV List 10[7:0]
{0x54a6, 0x01}, ////UV List 11[11:8]
{0x54a7, 0x6e}, ////UV List 11[7:0]
{0x54a8, 0x01}, ////UV List 12[11:8]
{0x54a9, 0x3c}, ////UV List 12[7:0]
{0x54aa, 0x01}, ////UV List 13[11:8]
{0x54ab, 0x0a}, ////UV List 13[7:0]
{0x54ac, 0x01}, ////UV List 14[11:8]
{0x54ad, 0x02}, ////UV List 14[7:0]

},

{//satutaion -2
{0x5490, 0x01}, ////UV List 0[11:8]
{0x5491, 0x63}, ////UV List 0[7:0]
{0x5492, 0x02}, ////UV List 1[11:8]
{0x5493, 0x61}, ////UV List 1[7:0]
{0x5494, 0x03}, ////UV List 2[11:8]
{0x5495, 0x13}, ////UV List 2[7:0]
{0x5496, 0x02}, ////UV List 3[11:8]
{0x5497, 0x6e}, ////UV List 3[7:0]
{0x5498, 0x02}, ////UV List 4[11:8]
{0x5499, 0x1e}, ////UV List 4[7:0]
{0x549a, 0x01}, ////UV List 5[11:8]
{0x549b, 0xf3}, ////UV List 5[7:0]
{0x549c, 0x01}, ////UV List 6[11:8]
{0x549d, 0xcc}, ////UV List 6[7:0]
{0x549e, 0x01}, ////UV List 7[11:8]
{0x549f, 0xaf}, ////UV List 7[7:0]
{0x54a0, 0x01}, ////UV List 8[11:8]
{0x54a1, 0x9b}, ////UV List 8[7:0]
{0x54a2, 0x01}, ////UV List 9[11:8]
{0x54a3, 0x86}, ////UV List 9[7:0]
{0x54a4, 0x01}, ////UV List 10[11:8]
{0x54a5, 0x63}, ////UV List 10[7:0]
{0x54a6, 0x01}, ////UV List 11[11:8]
{0x54a7, 0x46}, ////UV List 11[7:0]
{0x54a8, 0x01}, ////UV List 12[11:8]
{0x54a9, 0x19}, ////UV List 12[7:0]
{0x54aa, 0x00}, ////UV List 13[11:8]
{0x54ab, 0xed}, ////UV List 13[7:0]
{0x54ac, 0x00}, ////UV List 14[11:8]
{0x54ad, 0xe6}, ////UV List 14[7:0]

},

{//satutaion -3
{0x5490, 0x01}, ////UV List 0[11:8]
{0x5491, 0x3c}, ////UV List 0[7:0]
{0x5492, 0x02}, ////UV List 1[11:8]
{0x5493, 0x1f}, ////UV List 1[7:0]
{0x5494, 0x02}, ////UV List 2[11:8]
{0x5495, 0xbd}, ////UV List 2[7:0]
{0x5496, 0x02}, ////UV List 3[11:8]
{0x5497, 0x2a}, ////UV List 3[7:0]
{0x5498, 0x01}, ////UV List 4[11:8]
{0x5499, 0xe3}, ////UV List 4[7:0]
{0x549a, 0x01}, ////UV List 5[11:8]
{0x549b, 0xbc}, ////UV List 5[7:0]
{0x549c, 0x01}, ////UV List 6[11:8]
{0x549d, 0x9a}, ////UV List 6[7:0]
{0x549e, 0x01}, ////UV List 7[11:8]
{0x549f, 0x80}, ////UV List 7[7:0]
{0x54a0, 0x01}, ////UV List 8[11:8]
{0x54a1, 0x6e}, ////UV List 8[7:0]
{0x54a2, 0x01}, ////UV List 9[11:8]
{0x54a3, 0x5b}, ////UV List 9[7:0]
{0x54a4, 0x01}, ////UV List 10[11:8]
{0x54a5, 0x3c}, ////UV List 10[7:0]
{0x54a6, 0x01}, ////UV List 11[11:8]
{0x54a7, 0x22}, ////UV List 11[7:0]
{0x54a8, 0x00}, ////UV List 12[11:8]
{0x54a9, 0xfb}, ////UV List 12[7:0]
{0x54aa, 0x00}, ////UV List 13[11:8]
{0x54ab, 0xd3}, ////UV List 13[7:0]
{0x54ac, 0x00}, ////UV List 14[11:8]
{0x54ad, 0xcd}, ////UV List 14[7:0]

},
{//satutaion -4
{0x5490, 0x01}, ////UV List 0[11:8]
{0x5491, 0x1a}, ////UV List 0[7:0]
{0x5492, 0x01}, ////UV List 1[11:8]
{0x5493, 0xe3}, ////UV List 1[7:0]
{0x5494, 0x02}, ////UV List 2[11:8]
{0x5495, 0x70}, ////UV List 2[7:0]
{0x5496, 0x01}, ////UV List 3[11:8]
{0x5497, 0xed}, ////UV List 3[7:0]
{0x5498, 0x01}, ////UV List 4[11:8]
{0x5499, 0xae}, ////UV List 4[7:0]
{0x549a, 0x01}, ////UV List 5[11:8]
{0x549b, 0x8c}, ////UV List 5[7:0]
{0x549c, 0x01}, ////UV List 6[11:8]
{0x549d, 0x6d}, ////UV List 6[7:0]
{0x549e, 0x01}, ////UV List 7[11:8]
{0x549f, 0x56}, ////UV List 7[7:0]
{0x54a0, 0x01}, ////UV List 8[11:8]
{0x54a1, 0x46}, ////UV List 8[7:0]
{0x54a2, 0x01}, ////UV List 9[11:8]
{0x54a3, 0x35}, ////UV List 9[7:0]
{0x54a4, 0x01}, ////UV List 10[11:8]
{0x54a5, 0x1a}, ////UV List 10[7:0]
{0x54a6, 0x01}, ////UV List 11[11:8]
{0x54a7, 0x02}, ////UV List 11[7:0]
{0x54a8, 0x00}, ////UV List 12[11:8]
{0x54a9, 0xdf}, ////UV List 12[7:0]
{0x54aa, 0x00}, ////UV List 13[11:8]
{0x54ab, 0xbc}, ////UV List 13[7:0]
{0x54ac, 0x00}, ////UV List 14[11:8]
{0x54ad, 0xb6}, ////UV List 14[7:0]

}


};
static struct msm_camera_i2c_conf_array ov9740_saturation_confs[][1] = {
	{{ov9740_saturation[0],
		ARRAY_SIZE(ov9740_saturation[0]), 0, MSM_CAMERA_I2C_BYTE_DATA},
	},
	{{ov9740_saturation[1],
		ARRAY_SIZE(ov9740_saturation[1]), 0, MSM_CAMERA_I2C_BYTE_DATA},
	},
	{{ov9740_saturation[2],
		ARRAY_SIZE(ov9740_saturation[2]), 0, MSM_CAMERA_I2C_BYTE_DATA},
	},
	{{ov9740_saturation[3],
		ARRAY_SIZE(ov9740_saturation[3]), 0, MSM_CAMERA_I2C_BYTE_DATA},
	},
	{{ov9740_saturation[4],
		ARRAY_SIZE(ov9740_saturation[4]), 0, MSM_CAMERA_I2C_BYTE_DATA},
	},
	{{ov9740_saturation[5],
		ARRAY_SIZE(ov9740_saturation[5]), 0, MSM_CAMERA_I2C_BYTE_DATA},
	},
	{{ov9740_saturation[6],
		ARRAY_SIZE(ov9740_saturation[6]), 0, MSM_CAMERA_I2C_BYTE_DATA},
	},
	{{ov9740_saturation[7],
		ARRAY_SIZE(ov9740_saturation[7]), 0, MSM_CAMERA_I2C_BYTE_DATA},
	},
	{{ov9740_saturation[8],
		ARRAY_SIZE(ov9740_saturation[8]), 0, MSM_CAMERA_I2C_BYTE_DATA},
	},
	
	
};
static int ov9740_saturation_enum_map[] = {
	0,
	1,
	2,
	3,
	4,
	5,
	6,
	7,
	8,
};
static struct msm_camera_i2c_enum_conf_array ov9740_saturation_enum_confs = {
	.conf = &ov9740_saturation_confs[0][0],
	.conf_enum = ov9740_saturation_enum_map,
	.num_enum = ARRAY_SIZE(ov9740_saturation_enum_map),
	.num_index = ARRAY_SIZE(ov9740_saturation_confs),
	.num_conf = ARRAY_SIZE(ov9740_saturation_confs[0]),
	.data_type = MSM_CAMERA_I2C_BYTE_DATA,
};
#endif
static struct msm_camera_i2c_reg_conf ov9740_reset_settings[] = {};

static struct msm_camera_i2c_reg_conf ov9740_recommend_settings[] = {};

static struct v4l2_subdev_info ov9740_subdev_info[] = {
	{
	.code   = V4L2_MBUS_FMT_YUYV8_2X8,
	.colorspace = V4L2_COLORSPACE_JPEG,
	.fmt    = 1,
	.order    = 0,
	},
	/* more can be supported, to be added later */
};

static void ov9740_start_stream(struct msm_sensor_ctrl_t *s_ctrl) {
	int rc=0;
	pr_err("%s E LINE=%d\n", __func__, __LINE__);		
	rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client, 0x0100, 0x01, 
		MSM_CAMERA_I2C_BYTE_DATA);
	if (rc < 0) {
	pr_err("%s: OV9740_MODE_SELECT=streaming failed\n", __func__);
	} 
	msleep(10);
	};

static void ov9740_stop_stream(struct msm_sensor_ctrl_t *s_ctrl) {
#if 0
	int rc=0;
	pr_err("%s E LINE=%d\n", __func__, __LINE__);		
	rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client, 0x0100, 0x00, 
		MSM_CAMERA_I2C_BYTE_DATA);
	if (rc < 0) {
	pr_err("%s: OV9740_MODE_SELECT=standby failed\n", __func__);
	} 
#endif
	};
	
static struct msm_camera_i2c_conf_array ov9740_init_conf[] = {
	{&ov9740_reset_settings[0],
	ARRAY_SIZE(ov9740_reset_settings), 0, MSM_CAMERA_I2C_BYTE_DATA},
	{&ov9740_recommend_settings[0],
	ARRAY_SIZE(ov9740_recommend_settings), 0, MSM_CAMERA_I2C_BYTE_DATA}
};

static struct msm_camera_i2c_conf_array ov9740_confs[] = {
	{&ov9740_prev_settings[0],
	ARRAY_SIZE(ov9740_prev_settings), 0, MSM_CAMERA_I2C_BYTE_DATA},
};

int ov9740_set_sharpness(struct msm_sensor_ctrl_t *s_ctrl,
		struct msm_sensor_v4l2_ctrl_info_t *ctrl_info, int value)
{
	int rc = 0;
	pr_err("   %s   value:  %d\n", __func__,value);
	switch (value)
    {
	    case 0:
			
		pr_err("0");
#if 0
		rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,   0x3014, 0x1d, MSM_CAMERA_I2C_WORD_DATA);
		if (rc < 0) {
		pr_err("%s: i2c write failed\n", __func__);		
		return rc;
		
		}
#endif
		break;
		
	    case 5:
			pr_err("5");
			
		break;
		
        case 10:
			pr_err("10");
			
		break;
		
		case 15:
			pr_err("15");
			
		break;

		case 20:
			pr_err("20");
			
		break;
		
		case 25:
			pr_err("25");
			
		break;
		
		case 30:
			pr_err("30");
			
		break;
		default:
			pr_err("invalide level !!");
	}

	return rc;
		
}


int ov9740_set_exposure_Compensation(struct msm_sensor_ctrl_t *s_ctrl,
		struct msm_sensor_v4l2_ctrl_info_t *ctrl_info, int value)
{
	int rc = 0;
	pr_err("isp   %s   value:  %d\n", __func__,value);
	switch (value)
    { 
	    case MSM_V4L2_EXPOSURE_N2:
			pr_err("MSM_V4L2_EXPOSURE_N2");
			
		break;
		
	    case MSM_V4L2_EXPOSURE_N1:
			pr_err("MSM_V4L2_EXPOSURE_N1");
			
		break;
		
        case MSM_V4L2_EXPOSURE_D:
			pr_err("MSM_V4L2_EXPOSURE_D");
			
		break;
		
		case MSM_V4L2_EXPOSURE_P1:
			pr_err("MSM_V4L2_EXPOSURE_P1");
			
		break;

		case MSM_V4L2_EXPOSURE_P2:
			pr_err("MSM_V4L2_EXPOSURE_P2");
			
		break;
		

			
		default:
			pr_err("invalide level !!");
	}

	return rc;
		
}

int ov9740_set_brightness(struct msm_sensor_ctrl_t *s_ctrl,
		struct msm_sensor_v4l2_ctrl_info_t *ctrl_info, int value)
{
	int rc = 0;
	int16_t  tmp_reg = 0;
	pr_err("isp   %s   value:  %d\n", __func__,value);
	switch (value)
    {
	case MSM_V4L2_BRIGHTNESS_L0:
		 pr_err("%s   MSM_V4L2_BRIGHTNESS_L0 \n",__func__);

	 rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
				0x5587, 0x40, MSM_CAMERA_I2C_BYTE_DATA);


	 rc = msm_camera_i2c_read(s_ctrl->sensor_i2c_client,
				0x5588, &tmp_reg, MSM_CAMERA_I2C_BYTE_DATA);
	 tmp_reg &= 0xf7; // set bit[3] = 0
	 rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
	 	        0x5588, tmp_reg, MSM_CAMERA_I2C_BYTE_DATA);

		break;

	case MSM_V4L2_BRIGHTNESS_L1:
		 pr_err("%s   MSM_V4L2_BRIGHTNESS_L1 \n",__func__);
		rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
				0x5587, 0x30, MSM_CAMERA_I2C_BYTE_DATA);


		rc = msm_camera_i2c_read(s_ctrl->sensor_i2c_client,
				0x5588, &tmp_reg, MSM_CAMERA_I2C_BYTE_DATA);
		tmp_reg &= 0xf7; // set bit[3] = 0
		rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
			        0x5588, tmp_reg, MSM_CAMERA_I2C_BYTE_DATA);

		break;

	case MSM_V4L2_BRIGHTNESS_L2:
		 pr_err("%s   MSM_V4L2_BRIGHTNESS_L2 \n",__func__);
        
		rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
				0x5587, 0x20, MSM_CAMERA_I2C_BYTE_DATA);


		rc = msm_camera_i2c_read(s_ctrl->sensor_i2c_client,
				0x5588, &tmp_reg, MSM_CAMERA_I2C_BYTE_DATA);
		tmp_reg &= 0xf7; // set bit[3] = 0
		rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
				0x5588, tmp_reg, MSM_CAMERA_I2C_BYTE_DATA);

		break;

	case MSM_V4L2_BRIGHTNESS_L3:
	      pr_err("%s   MSM_V4L2_BRIGHTNESS_L3 \n",__func__);
		rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
				0x5587, 0x10, MSM_CAMERA_I2C_BYTE_DATA);


		rc = msm_camera_i2c_read(s_ctrl->sensor_i2c_client,
				0x5588, &tmp_reg, MSM_CAMERA_I2C_BYTE_DATA);
		tmp_reg &= 0xf7; // set bit[3] = 0
		rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
			        0x5588, tmp_reg, MSM_CAMERA_I2C_BYTE_DATA);

		break;

	case MSM_V4L2_BRIGHTNESS_L4:
		pr_err("%s   MSM_V4L2_BRIGHTNESS_L4 \n",__func__);
		rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
				0x5587, 0x00, MSM_CAMERA_I2C_BYTE_DATA);


	 rc = msm_camera_i2c_read(s_ctrl->sensor_i2c_client,
				0x5588, &tmp_reg, MSM_CAMERA_I2C_BYTE_DATA);
	 tmp_reg &= 0xf7; // set bit[3] = 0
	 rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
	 	        0x5588, tmp_reg, MSM_CAMERA_I2C_BYTE_DATA);
        

		break;

	case MSM_V4L2_BRIGHTNESS_L5:
		pr_err("%s   MSM_V4L2_BRIGHTNESS_L5 \n",__func__);
		rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
				0x5587, 0x10, MSM_CAMERA_I2C_BYTE_DATA);


		rc = msm_camera_i2c_read(s_ctrl->sensor_i2c_client,
				0x5588, &tmp_reg, MSM_CAMERA_I2C_BYTE_DATA);
		tmp_reg |= 0x08; // set bit[3] = 1
		rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
			        0x5588, tmp_reg, MSM_CAMERA_I2C_BYTE_DATA);

		break;

	case MSM_V4L2_BRIGHTNESS_L6:
		pr_err("%s   MSM_V4L2_BRIGHTNESS_L6 \n",__func__);
        
		rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
				0x5587, 0x20, MSM_CAMERA_I2C_BYTE_DATA);


		rc = msm_camera_i2c_read(s_ctrl->sensor_i2c_client,
				0x5588, &tmp_reg, MSM_CAMERA_I2C_BYTE_DATA);
		tmp_reg |= 0x08; // set bit[3] = 1
		rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
					0x5588, tmp_reg, MSM_CAMERA_I2C_BYTE_DATA);

		break;

	default:
		pr_err("%s  brghtness value is not supported \n",__func__);
		break;
	}

	return rc;
		
}
int ov9740_set_effect(struct msm_sensor_ctrl_t *s_ctrl,
		struct msm_sensor_v4l2_ctrl_info_t *ctrl_info, int value)
{
		int rc = 0;
		pr_err("isp   %s   value:  %d\n", __func__,value);
		switch (value)
		{
		case CAMERA_EFFECT_OFF:
			 pr_err("%s   CAMERA_EFFECT_OFF 0\n",__func__);

			rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
	 	        0x5580, 0x06, MSM_CAMERA_I2C_BYTE_DATA);
			rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
				0x5583, 0x40, MSM_CAMERA_I2C_BYTE_DATA);
			rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
	 	        0x5584, 0x40, MSM_CAMERA_I2C_BYTE_DATA);
  	

	
			break;
		case CAMERA_EFFECT_MONO:
			 pr_err("%s   CAMERA_EFFECT_MONO 1\n",__func__);

			rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
	 	        0x5580, 0x26, MSM_CAMERA_I2C_BYTE_DATA);
  	
	
			break;
		case CAMERA_EFFECT_NEGATIVE:
			 pr_err("%s   CAMERA_EFFECT_NEGATIVE 2\n",__func__);
			
			rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
				0x5580, 0x46, MSM_CAMERA_I2C_BYTE_DATA);
			rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
				0x5583, 0x40, MSM_CAMERA_I2C_BYTE_DATA);
			rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
				0x5584, 0x40, MSM_CAMERA_I2C_BYTE_DATA);

	
			break;
		case CAMERA_EFFECT_SOLARIZE:
			 pr_err("%s   CAMERA_EFFECT_SOLARIZE 3\n",__func__);
			
	
			break;
		case CAMERA_EFFECT_SEPIA:
			rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
				0x5580, 0x06, MSM_CAMERA_I2C_BYTE_DATA);
			rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
				0x5583, 0x40, MSM_CAMERA_I2C_BYTE_DATA);
			rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
				0x5584, 0xa0, MSM_CAMERA_I2C_BYTE_DATA);

			 pr_err("%s   CAMERA_EFFECT_SEPIA 4\n",__func__);
			
	
			break;
		case CAMERA_EFFECT_POSTERIZE:
			 pr_err("%s   CAMERA_EFFECT_POSTERIZE 5\n",__func__);
			
	
			break;
		case CAMERA_EFFECT_WHITEBOARD:
			 pr_err("%s   CAMERA_EFFECT_WHITEBOARD 6\n",__func__);
			
	
			break;
		case CAMERA_EFFECT_BLACKBOARD:
			 pr_err("%s   CAMERA_EFFECT_BLACKBOARD 7\n",__func__);
			
	
			break;
	
		case CAMERA_EFFECT_AQUA:
			 pr_err("%s   CAMERA_EFFECT_AQUA 8\n",__func__);
	
			break;
			
		case CAMERA_EFFECT_EMBOSS:
			 pr_err("%s   CAMERA_EFFECT_EMBOSS 9\n",__func__);
			break;
			
		case CAMERA_EFFECT_SKETCH:
			pr_err("%s	 CAMERA_EFFECT_SKETCH 10\n",__func__);
			break;
			
		 case CAMERA_EFFECT_NEON:
			pr_err("%s	 CAMERA_EFFECT_NEON 11\n",__func__);
			
			break;
			
		 case CAMERA_EFFECT_MAX:
			pr_err("%s	 CAMERA_EFFECT_MAX \n",__func__);
			break; 
	
		
	
		default:
				pr_err("invalide level !!");
	
			}
			
			
			
		return rc;
	
	}
int ov9740_set_contrast(struct msm_sensor_ctrl_t *s_ctrl,
		struct msm_sensor_v4l2_ctrl_info_t *ctrl_info, int value)
{
	int rc = 0;
	pr_err("isp   %s   value:  %d\n", __func__,value);
	switch (value)
	{
	case CAMERA_CONTRAST_LV0:
		pr_err("CAMERA_CONTRAST_LV0");
		
		rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
			 0x5586, 0x0d, MSM_CAMERA_I2C_BYTE_DATA);
	break;

	case CAMERA_CONTRAST_LV1:
		pr_err("CAMERA_CONTRAST_LV1");
		rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
			 0x5586, 0x10, MSM_CAMERA_I2C_BYTE_DATA);
	break;

	  case CAMERA_CONTRAST_LV2:
		pr_err("CAMERA_CONTRAST_LV2");
		rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
			 0x5586, 0x14, MSM_CAMERA_I2C_BYTE_DATA);
	break;

	case CAMERA_CONTRAST_LV3:
		pr_err("CAMERA_CONTRAST_LV3");
		rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
			 0x5586, 0x19, MSM_CAMERA_I2C_BYTE_DATA);
		
	break;

	case CAMERA_CONTRAST_LV4:
		pr_err("CAMERA_CONTRAST_LV4");
		rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
			 0x5586, 0x20, MSM_CAMERA_I2C_BYTE_DATA);
	break;

	case CAMERA_CONTRAST_LV5:
		pr_err("CAMERA_CONTRAST_LV5");
		rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
			 0x5586, 0x28, MSM_CAMERA_I2C_BYTE_DATA);
	break;

	case CAMERA_CONTRAST_LV6:
		pr_err("CAMERA_CONTRAST_LV6");
		rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
			 0x5586, 0x30, MSM_CAMERA_I2C_BYTE_DATA);
	break;

	case CAMERA_CONTRAST_LV7:
		pr_err("CAMERA_CONTRAST_LV7");
		rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
			 0x5586, 0x38, MSM_CAMERA_I2C_BYTE_DATA);
	break;

	case CAMERA_CONTRAST_LV8:
		pr_err("CAMERA_CONTRAST_LV8");
		rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
			 0x5586, 0x40, MSM_CAMERA_I2C_BYTE_DATA);
	break;

	case CAMERA_CONTRAST_LV9:
		pr_err("CAMERA_CONTRAST_LV8");
		rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
			 0x5586, 0x48, MSM_CAMERA_I2C_BYTE_DATA);
	break;	
	default:
		pr_err("invalide level !!");

	}

	return rc;
		
}
int ov9740_set_scenemode(struct msm_sensor_ctrl_t *s_ctrl,
		struct msm_sensor_v4l2_ctrl_info_t *ctrl_info, int value)
{
	int rc = 0;
	pr_err("isp   %s   value:  %d\n", __func__,value);
	switch (value)
    {
	    case 0:
			pr_err("0");
			
		break;
		
	    case 1:
			pr_err("1");
			
		break;
		
        case 2:
			pr_err("3");
			
		break;
		
		case 3:
			pr_err("3");
			
		break;

		case 4:
			pr_err("4");
			
		break;
		
		case 5:
			pr_err("5");
			
		break;
			
		default:
			pr_err("invalide level !!");
	}

	return rc;
		
}

int ov9740_set_saturation(struct msm_sensor_ctrl_t *s_ctrl,
		struct msm_sensor_v4l2_ctrl_info_t *ctrl_info, int value)
{
	int rc = 0;
	pr_err("isp   %s   value:  %d\n", __func__,value);

	switch (value)
	{
	case 0:
		 pr_err("%s   saturation   off   \n",__func__);
		

     break;

	case 1:
		 pr_err("%s saturation  1 \n",__func__);
		 

	   break;
	   
	 case 2:
		 pr_err("%s saturation  2 \n",__func__);
		 

     break;
		 
	 case 3:
		 pr_err("%s saturation  3 \n",__func__);
		 
	
	   break;

	 case 4:
		 pr_err("%s saturation  4 \n",__func__);
		 
	
	   break;


	  case 5:
		 pr_err("%s saturation  5 \n",__func__);
		 
	
	   break;

	   case 6:
		 pr_err("%s saturation  6 \n",__func__);

	   break;
	   
	   
	default:	
	pr_err("%s  invalide value is not supported \n",__func__);
		
	}
	return rc;
}

int ov9740_set_antibanding(struct msm_sensor_ctrl_t *s_ctrl,
		struct msm_sensor_v4l2_ctrl_info_t *ctrl_info, int value)
{
	int rc = 0;
	int16_t  tmp_reg = 0;
	pr_err("%s   value:  %d\n", __func__,value);

	switch (value)
	{
	case 0:
		 pr_err("%s   antibanding   off   \n",__func__);

	 rc = msm_camera_i2c_read(s_ctrl->sensor_i2c_client,
				 0x3a00, &tmp_reg, MSM_CAMERA_I2C_BYTE_DATA);
	 tmp_reg &= 0xdf; // turn off band filter, bit[5] = 0
	 rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
	 	        0x3a00, tmp_reg, MSM_CAMERA_I2C_BYTE_DATA);
     break;

	case 1:
		tmp_reg=0;
		rc = msm_camera_i2c_read(s_ctrl->sensor_i2c_client,
			  0x3c00, &tmp_reg, MSM_CAMERA_I2C_BYTE_DATA);
		tmp_reg &= 0xfb; // 60Hz, bit[2] = 0
		rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
				 0x3c00, tmp_reg, MSM_CAMERA_I2C_BYTE_DATA);
		
		tmp_reg=0;
		rc = msm_camera_i2c_read(s_ctrl->sensor_i2c_client,
				  0x3c01, &tmp_reg, MSM_CAMERA_I2C_BYTE_DATA);
		tmp_reg |= 0x80; // manual enable, bit[7] = 1
		rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
				 0x3c01, tmp_reg, MSM_CAMERA_I2C_BYTE_DATA);
		
		tmp_reg=0;
		rc = msm_camera_i2c_read(s_ctrl->sensor_i2c_client,
			  0x3a00, &tmp_reg, MSM_CAMERA_I2C_BYTE_DATA);
		tmp_reg |= 0x20; // turn off band filter, bit[5] = 1
		rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
			 0x3a00, tmp_reg, MSM_CAMERA_I2C_BYTE_DATA);
		 pr_err("%s antibanding  60hz \n",__func__);
	   break;
	   
	 case 2:
		 pr_err("%s antibanding  50hz \n",__func__);
		 
			tmp_reg=0;
			rc = msm_camera_i2c_read(s_ctrl->sensor_i2c_client,
				  0x3c00, &tmp_reg, MSM_CAMERA_I2C_BYTE_DATA);
			tmp_reg |= 0x04; // 50Hz, bit[2] = 1
			rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
					 0x3c00, tmp_reg, MSM_CAMERA_I2C_BYTE_DATA);

			tmp_reg=0;
			rc = msm_camera_i2c_read(s_ctrl->sensor_i2c_client,
					  0x3c01, &tmp_reg, MSM_CAMERA_I2C_BYTE_DATA);
			tmp_reg |= 0x80; // manual enable, bit[7] = 1
			rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
					 0x3c01, tmp_reg, MSM_CAMERA_I2C_BYTE_DATA);
			
			tmp_reg=0;
			rc = msm_camera_i2c_read(s_ctrl->sensor_i2c_client,
				  0x3a00, &tmp_reg, MSM_CAMERA_I2C_BYTE_DATA);
			tmp_reg |= 0x20; // turn off band filter, bit[5] = 1
			rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
				 0x3a00, tmp_reg, MSM_CAMERA_I2C_BYTE_DATA); 
     break;
		 
	 case 3:
		 pr_err("%s antibanding  auto \n",__func__);		 
		 tmp_reg=0;	 
		 rc = msm_camera_i2c_read(s_ctrl->sensor_i2c_client,
			   0x3c01, &tmp_reg, MSM_CAMERA_I2C_BYTE_DATA);
		 tmp_reg &= 0x7f; // auto enable, bit[7] = 0
		 rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
				  0x3c01, tmp_reg, MSM_CAMERA_I2C_BYTE_DATA);

		 tmp_reg=0;
		 rc = msm_camera_i2c_read(s_ctrl->sensor_i2c_client,
				   0x3a00, &tmp_reg, MSM_CAMERA_I2C_BYTE_DATA);
		 tmp_reg |= 0x20; // turn on band filter, bit[5] = 1
		 rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
				  0x3a00, tmp_reg, MSM_CAMERA_I2C_BYTE_DATA);
	   break;
	   
	default:	
	pr_err("%s  invalide value is not supported \n",__func__);
		
	}
	return rc;


}

int ov9740_set_wb(struct msm_sensor_ctrl_t *s_ctrl,
		struct msm_sensor_v4l2_ctrl_info_t *ctrl_info, int value)
{
	int rc = 0;
	int16_t  tmp_reg = 0;

	g_wb_value=value;
    pr_err("g_wb_value= %d ",g_wb_value);
		
	pr_err("isp   %s   value:  %d\n", __func__,value);

    rc = msm_camera_i2c_read(s_ctrl->sensor_i2c_client,
				0x3406, &tmp_reg, MSM_CAMERA_I2C_BYTE_DATA);
		 pr_err("0x3406 value=%d ",tmp_reg);

		 msleep(30);
	switch (value)
	{	
	case MSM_V4L2_WB_AUTO:
	pr_err("MSM_V4L2_WB_AUTO");

	rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
	    0x3406, 0x00, MSM_CAMERA_I2C_BYTE_DATA);	
	                  		
	break;

	case MSM_V4L2_WB_CUSTOM:
	pr_err("MSM_V4L2_WB_CUSTOM");

	break;

	case MSM_V4L2_WB_INCANDESCENT:
	pr_err("MSM_V4L2_WB_INCANDESCENT");
	rc = msm_camera_i2c_read(s_ctrl->sensor_i2c_client,
	   0x3406, &tmp_reg, MSM_CAMERA_I2C_BYTE_DATA);
	pr_err("0x3406 value=%d  mp_reg|0x01=%d",tmp_reg,(tmp_reg|0x01));



	rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
	    0x3406, tmp_reg|0x01, MSM_CAMERA_I2C_BYTE_DATA);
	rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
	    0x3400, 0x04, MSM_CAMERA_I2C_BYTE_DATA);	 	                               
	rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
	    0x3401, 0x0a9, MSM_CAMERA_I2C_BYTE_DATA);	
	rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
	    0x3402, 0x04, MSM_CAMERA_I2C_BYTE_DATA);
	rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
	    0x3403, 0x00, MSM_CAMERA_I2C_BYTE_DATA);      
	rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
	    0x3404, 0x09, MSM_CAMERA_I2C_BYTE_DATA);
	rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
	    0x3405, 0x0e, MSM_CAMERA_I2C_BYTE_DATA); 

	break;

	case MSM_V4L2_WB_FLUORESCENT:
	pr_err("MSM_V4L2_WB_FLUORESCENT");
	rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
	    0x3406, tmp_reg|0x01, MSM_CAMERA_I2C_BYTE_DATA);
	rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
	    0x3400, 0x04, MSM_CAMERA_I2C_BYTE_DATA);	 	                               
	rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
	    0x3401, 0x058, MSM_CAMERA_I2C_BYTE_DATA);	
	rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
	    0x3402, 0x04, MSM_CAMERA_I2C_BYTE_DATA);
	rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
	    0x3403, 0x00, MSM_CAMERA_I2C_BYTE_DATA);      
	rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
	    0x3404, 0x08, MSM_CAMERA_I2C_BYTE_DATA);
	rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
	    0x3405, 0x40, MSM_CAMERA_I2C_BYTE_DATA);
	    
	break;

	case MSM_V4L2_WB_DAYLIGHT:
	pr_err("MSM_V4L2_WB_DAYLIGHT");

	rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
	    0x3406, tmp_reg|0x01, MSM_CAMERA_I2C_BYTE_DATA);
	rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
	    0x3400, 0x07, MSM_CAMERA_I2C_BYTE_DATA);	 	                               
	rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
	    0x3401, 0x02, MSM_CAMERA_I2C_BYTE_DATA);	
	rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
	    0x3402, 0x04, MSM_CAMERA_I2C_BYTE_DATA);
	rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
	    0x3403, 0x00, MSM_CAMERA_I2C_BYTE_DATA);      
	rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
	    0x3404, 0x05, MSM_CAMERA_I2C_BYTE_DATA);
	rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
	    0x3405, 0x15, MSM_CAMERA_I2C_BYTE_DATA); 

	break;

	case MSM_V4L2_WB_CLOUDY_DAYLIGHT:
	pr_err("MSM_V4L2_WB_CLOUDY_DAYLIGHT");
	rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
	    0x3406, tmp_reg|0x01, MSM_CAMERA_I2C_BYTE_DATA);
	rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
	    0x3400, 0x07, MSM_CAMERA_I2C_BYTE_DATA);	 	                               
	rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
	    0x3401, 0x88, MSM_CAMERA_I2C_BYTE_DATA);	
	rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
	    0x3402, 0x04, MSM_CAMERA_I2C_BYTE_DATA);
	rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
	    0x3403, 0x00, MSM_CAMERA_I2C_BYTE_DATA);      
	rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
	    0x3404, 0x05, MSM_CAMERA_I2C_BYTE_DATA);
	rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
	    0x3405, 0x00, MSM_CAMERA_I2C_BYTE_DATA); 
	    
	break;

	case MSM_V4L2_WB_OFF:
	pr_err("MSM_V4L2_WB_OFF");

	break;


	default:
	pr_err("invalide level !!");
	}

	return rc;
		
}

int ov9740_set_iso(struct msm_sensor_ctrl_t *s_ctrl,
		struct msm_sensor_v4l2_ctrl_info_t *ctrl_info, int value)
{
	int rc = 0;
	pr_err("isp   %s   value:  %d\n", __func__,value);
	g_iso_value=value;
	switch (value)
    {
	    case MSM_V4L2_ISO_AUTO:
			pr_err("MSM_V4L2_ISO_AUTO");
			rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
	 	        0x3a18, 0x00, MSM_CAMERA_I2C_BYTE_DATA);
			rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
      			0x3a19, 0x60, MSM_CAMERA_I2C_BYTE_DATA);				
		break;
		
	    case MSM_V4L2_ISO_DEBLUR:
			pr_err("MSM_V4L2_ISO_DEBLUR");
			
		break;
		
        case MSM_V4L2_ISO_100:
			pr_err("MSM_V4L2_ISO_100");
			rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
	 	        0x3a18, 0x00, MSM_CAMERA_I2C_BYTE_DATA);
			rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
      			0x3a19, 0x40, MSM_CAMERA_I2C_BYTE_DATA);			
		break;
		
		case MSM_V4L2_ISO_200:
			pr_err("MSM_V4L2_ISO_200");
			rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
	 	        0x3a18, 0x00, MSM_CAMERA_I2C_BYTE_DATA);
			rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
      			0x3a19, 0x60, MSM_CAMERA_I2C_BYTE_DATA);			
		break;

		case MSM_V4L2_ISO_400:
			pr_err("MSM_V4L2_ISO_400");
			rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
	 	        0x3a18, 0x00, MSM_CAMERA_I2C_BYTE_DATA);
			rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
      			0x3a19, 0x80, MSM_CAMERA_I2C_BYTE_DATA);
		break;
		
		case MSM_V4L2_ISO_800:
			pr_err("MSM_V4L2_ISO_800");
			rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
	 	        0x3a18, 0x00, MSM_CAMERA_I2C_BYTE_DATA);
			rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
      			0x3a19, 0xc0, MSM_CAMERA_I2C_BYTE_DATA);
		break;
		
		case MSM_V4L2_ISO_1600:
			pr_err("MSM_V4L2_ISO_1600");
			rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
	 	        0x3a18, 0x00, MSM_CAMERA_I2C_BYTE_DATA);
			rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
      			0x3a19, 0xf8, MSM_CAMERA_I2C_BYTE_DATA);
		break;			
		default:
			pr_err("invalide level !!");
	}

	return rc;
		
}

struct msm_sensor_v4l2_ctrl_info_t ov9740_v4l2_ctrl_info[] = {
		{
		.ctrl_id = V4L2_CID_WHITE_BALANCE_TEMPERATURE,
		.min = MSM_V4L2_WB_OFF,
		.max = MSM_V4L2_WB_CLOUDY_DAYLIGHT,
		.step = 1,
		.s_v4l2_ctrl = ov9740_set_wb,
		},
	
		{
		.ctrl_id = V4L2_CID_ISO,
		.min = 0,
		.max = 6,
		.step = 1,
		.s_v4l2_ctrl = ov9740_set_iso,
		},

		{
		.ctrl_id = V4L2_CID_ANTIBANDING,
		.min = 0,
		.max = 3,
		.step = 1,
		.s_v4l2_ctrl = ov9740_set_antibanding,
		},

		{
		.ctrl_id = V4L2_CID_SHARPNESS,
		.min = 0,
		.max = 30,
		.step = 5,
		.s_v4l2_ctrl = ov9740_set_sharpness,
		},

		{
		.ctrl_id = V4L2_CID_EXPOSURE,
		.min = MSM_V4L2_EXPOSURE_N2,
		.max = MSM_V4L2_EXPOSURE_P2,
		.step = 1,
		.s_v4l2_ctrl = ov9740_set_exposure_Compensation,
		},

		{
		.ctrl_id = V4L2_CID_BRIGHTNESS,
		.min = MSM_V4L2_BRIGHTNESS_L0,
		.max = MSM_V4L2_BRIGHTNESS_L6,
		.step = 1,
		.s_v4l2_ctrl = ov9740_set_brightness,
		},
		{
		.ctrl_id = V4L2_CID_COLORFX,
		.min = V4L2_COLORFX_NONE,
		.max = V4L2_COLORFX_VIVID,
		.step = 1,
		.s_v4l2_ctrl = ov9740_set_effect,
		},
		{
		.ctrl_id = V4L2_CID_CONTRAST,
		.min = CAMERA_CONTRAST_LV0,
		.max = CAMERA_CONTRAST_LV9,
		.step = 1,
		.s_v4l2_ctrl = ov9740_set_contrast,
		},

#if 0
		{
		.ctrl_id = V4L2_CID_BESTSHOT_MODE,
		.min = 0,
		.max = 18,
		.step = 1,
		.s_v4l2_ctrl = ov9740_set_scenemode,
		},

		{
		.ctrl_id = V4L2_CID_SATURATION,
		.min = 0,
		.max = 8,
		.step = 1,
		.enum_cfg_settings = &ov9740_saturation_enum_confs,
		.s_v4l2_ctrl = msm_sensor_s_ctrl_by_enum,
		},
#endif
};

static struct msm_sensor_output_info_t ov9740_dimensions[] = {
{
		.x_output = 0x500,
		.y_output = 0x2d0,
		.line_length_pclk = 0x500,
		.frame_length_lines = 0x2d0,
		.vt_pixel_clk = 76000000,
		.op_pixel_clk = 304000000,
		//.t_pixel_clk = 48000000,//9m114 launch fail
		//.p_pixel_clk = 128000000,//9m114 launch fail	
		.binning_factor = 1,
	},
};
static struct msm_sensor_output_reg_addr_t ov9740_reg_addr = {
	.x_output = 0xcc,
	.y_output = 0xce,
	.line_length_pclk = 0xcc,
	.frame_length_lines = 0xce,
};

static struct msm_sensor_id_info_t ov9740_id_info = {
	.sensor_id_reg_addr = 0x00,
	.sensor_id = 0x9740,
};

static const struct i2c_device_id ov9740_i2c_id[] = {
	{SENSOR_NAME, (kernel_ulong_t)&ov9740_s_ctrl},
	{ }
};

static struct msm_cam_clk_info cam_8960_clk_info[] = {
	{"cam_clk", MSM_SENSOR_MCLK_24HZ},
};
 
int32_t msm_sensor_ov9740_write_res_settings(struct msm_sensor_ctrl_t *s_ctrl,
	uint16_t res)
{
	int32_t rc;
		pr_err("%s: E\n", __func__);	
	rc = msm_sensor_write_conf_array(
		s_ctrl->sensor_i2c_client,
		s_ctrl->msm_sensor_reg->mode_settings, res);
	return rc;
}


#ifdef CONFIG_MACH_COEUS
int32_t msm_sensor_ov9740_power_up(struct msm_sensor_ctrl_t *s_ctrl)
{
	int32_t rc = 0;
	
#if defined(CONFIG_MACH_PRINDLE) 
	int16_t  tmp_reg = 0;
#endif	

	struct msm_camera_sensor_info *data = s_ctrl->sensordata;
	struct device *dev = NULL;
	if (s_ctrl->sensor_device_type == MSM_SENSOR_PLATFORM_DEVICE)
		dev = &s_ctrl->pdev->dev;
	else
		dev = &s_ctrl->sensor_i2c_client->client->dev;
	

	rc = msm_camera_request_gpio_table(data, 1);
	if (rc < 0) {
		pr_err("%s: request gpio failed\n", __func__);
		goto request_gpio_failed;
	}
       if(!IsPowered) {
	   	s_ctrl->reg_ptr = kzalloc(sizeof(struct regulator *)
			* data->sensor_platform_info->num_vreg, GFP_KERNEL);
	if (!s_ctrl->reg_ptr) {
		pr_err("%s: could not allocate mem for regulators\n",
			__func__);
		return -ENOMEM;
	}
		rc = msm_camera_config_vreg(dev,
			s_ctrl->sensordata->sensor_platform_info->cam_vreg,
			s_ctrl->sensordata->sensor_platform_info->num_vreg,
			s_ctrl->vreg_seq,
			s_ctrl->num_vreg_seq,
			s_ctrl->reg_ptr, 1);
		if (rc < 0) {
			pr_err("%s: regulator on failed\n", __func__);
			goto config_vreg_failed;
		}

		rc = msm_camera_enable_vreg(dev,
			s_ctrl->sensordata->sensor_platform_info->cam_vreg,
			s_ctrl->sensordata->sensor_platform_info->num_vreg,
			s_ctrl->vreg_seq,
			s_ctrl->num_vreg_seq,
			s_ctrl->reg_ptr, 1);
		if (rc < 0) {
			pr_err("%s: enable regulator failed\n", __func__);
			goto enable_vreg_failed;
		}
		IsPowered = 1;
   	}

	rc = msm_camera_config_gpio_table(data, 1);
	if (rc < 0) {
		pr_err("%s: config gpio failed\n", __func__);
		goto config_gpio_failed;
	}

	rc=gpio_direction_output(53, 1);
	if (rc < 0)  
		pr_err("%s  gpio  direction 53 failed\n",__func__);
	
	usleep_range(1000,2000);
	
	rc=gpio_direction_output(53, 0);
	if (rc < 0)  
		pr_err("%s  gpio  direction 53 failed\n",__func__);
	usleep_range(1000,2000);
	
	rc=gpio_direction_output(76, 1);
	if (rc < 0)  
		pr_err("%s  gpio  direction 76 failed\n",__func__);
	
	usleep_range(5000,6000);
	
	rc=gpio_direction_output(76, 0);
	if (rc < 0)  
		pr_err("%s  gpio  direction 76 failed\n",__func__);
	
	usleep_range(5000,6000);

	rc=gpio_direction_output(76, 1);
	if (rc < 0)  
		pr_err("%s  gpio  direction 76 failed\n",__func__);
	
	usleep_range(1000,2000);
	

	if (s_ctrl->sensor_device_type == MSM_SENSOR_I2C_DEVICE) {
		if (s_ctrl->clk_rate != 0)
			cam_8960_clk_info->clk_rate = s_ctrl->clk_rate;

		rc = msm_cam_clk_enable(dev, cam_8960_clk_info,
			s_ctrl->cam_clk, ARRAY_SIZE(cam_8960_clk_info), 1);
		if (rc < 0) {
			pr_err("%s: clk enable failed\n", __func__);
			goto enable_clk_failed;
		}
	} 

	if (!s_ctrl->power_seq_delay)
		usleep_range(1000, 2000);
	else if (s_ctrl->power_seq_delay < 20)
		usleep_range((s_ctrl->power_seq_delay * 1000),
			((s_ctrl->power_seq_delay * 1000) + 1000));
	else
		msleep(s_ctrl->power_seq_delay);

	rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,   0x3014, 0x05, MSM_CAMERA_I2C_WORD_DATA);
	if (rc < 0) {
	pr_err("%s: i2c write failed\n", __func__);		
	return rc;
	}

	rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client, OV9740_MODE_SELECT, 0x00, 
		MSM_CAMERA_I2C_BYTE_DATA);
	if (rc < 0) {
	pr_err("%s: i2c write failed\n", __func__);
	}
	
	msleep(10);

#if defined(CONFIG_MACH_PRINDLE) 
	//add by wxl for f10
	rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client, 0x3021, 0x20, MSM_CAMERA_I2C_BYTE_DATA);
	if (rc < 0) {
		pr_err("%s: i2c write failed\n", __func__);		
		return rc;
	}
	else
		pr_err("%s: i2c write 3021 success\n", __func__);
	
	rc = msm_camera_i2c_read(s_ctrl->sensor_i2c_client,  0x3021, &tmp_reg, MSM_CAMERA_I2C_BYTE_DATA);
	if (rc < 0) {
		pr_err("%s: i2c read failed\n", __func__);		
		return rc;
	}
	else
		pr_err("%s:tmp_reg = %d \n", __func__,tmp_reg);
#endif
	
	if (data->sensor_platform_info->ext_power_ctrl != NULL)
		data->sensor_platform_info->ext_power_ctrl(1);

	if (data->sensor_platform_info->i2c_conf &&
		data->sensor_platform_info->i2c_conf->use_i2c_mux)
		msm_sensor_enable_i2c_mux(data->sensor_platform_info->i2c_conf);

	if (s_ctrl->sensor_device_type == MSM_SENSOR_PLATFORM_DEVICE) {
		rc = msm_sensor_cci_util(s_ctrl->sensor_i2c_client,
			MSM_CCI_INIT);
		if (rc < 0) {
			pr_err("%s cci_init failed\n", __func__);
			goto cci_init_failed;
		}
	}
	s_ctrl->curr_res = MSM_SENSOR_INVALID_RES;
	return rc;

cci_init_failed:
	if (data->sensor_platform_info->i2c_conf &&
		data->sensor_platform_info->i2c_conf->use_i2c_mux)
		msm_sensor_disable_i2c_mux(
			data->sensor_platform_info->i2c_conf);
enable_clk_failed:
		msm_camera_config_gpio_table(data, 0);
config_gpio_failed:
	msm_camera_enable_vreg(dev,
			s_ctrl->sensordata->sensor_platform_info->cam_vreg,
			s_ctrl->sensordata->sensor_platform_info->num_vreg,
			s_ctrl->vreg_seq,
			s_ctrl->num_vreg_seq,
			s_ctrl->reg_ptr, 0);

enable_vreg_failed:
	msm_camera_config_vreg(dev,
		s_ctrl->sensordata->sensor_platform_info->cam_vreg,
		s_ctrl->sensordata->sensor_platform_info->num_vreg,
		s_ctrl->vreg_seq,
		s_ctrl->num_vreg_seq,
		s_ctrl->reg_ptr, 0);
config_vreg_failed:
	msm_camera_request_gpio_table(data, 0);
request_gpio_failed:
	kfree(s_ctrl->reg_ptr);
	return rc;
}


int32_t msm_sensor_ov9740_power_down(struct msm_sensor_ctrl_t *s_ctrl)
{
	struct msm_camera_sensor_info *data = s_ctrl->sensordata;
	struct device *dev = NULL;
	int32_t rc = 0;

	//add by lijing
	rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,  0x3002, 0xe8, MSM_CAMERA_I2C_WORD_DATA);
		if (rc < 0) {
		pr_err("%s: i2c write failed\n", __func__);		
		return rc;
		}
	rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,  0x3004, 0x03, MSM_CAMERA_I2C_WORD_DATA);
		if (rc < 0) {
		pr_err("%s: i2c write failed\n", __func__);		
		return rc;
		}
	rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,   0x3005, 0xff, MSM_CAMERA_I2C_WORD_DATA);
		if (rc < 0) {
		pr_err("%s: i2c write failed\n", __func__);		
		return rc;
		}
	rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,   0x3014, 0x1d, MSM_CAMERA_I2C_WORD_DATA);
		if (rc < 0) {
		pr_err("%s: i2c write failed\n", __func__);		
		return rc;
		}
		rc=gpio_direction_output(53, 1);
	if (s_ctrl->sensor_device_type == MSM_SENSOR_PLATFORM_DEVICE)
		dev = &s_ctrl->pdev->dev;
	else
		dev = &s_ctrl->sensor_i2c_client->client->dev;
	if (s_ctrl->sensor_device_type == MSM_SENSOR_PLATFORM_DEVICE) {
		msm_sensor_cci_util(s_ctrl->sensor_i2c_client,
			MSM_CCI_RELEASE);
	}

	if (data->sensor_platform_info->i2c_conf &&
		data->sensor_platform_info->i2c_conf->use_i2c_mux)
		msm_sensor_disable_i2c_mux(
			data->sensor_platform_info->i2c_conf);

	if (data->sensor_platform_info->ext_power_ctrl != NULL)
		data->sensor_platform_info->ext_power_ctrl(0);
	if (s_ctrl->sensor_device_type == MSM_SENSOR_I2C_DEVICE)
		msm_cam_clk_enable(dev, cam_8960_clk_info, s_ctrl->cam_clk,
			ARRAY_SIZE(cam_8960_clk_info), 0);
	
	msm_camera_config_gpio_table(data, 0);
	msm_camera_request_gpio_table(data, 0);
	
	s_ctrl->curr_res = MSM_SENSOR_INVALID_RES;
	return 0;
}
int32_t msm_ov9740_sensor_i2c_probe(struct i2c_client *client,
	const struct i2c_device_id *id)
{
	int rc = 0;
	struct msm_sensor_ctrl_t *s_ctrl;
	 	
	pr_err("%s %s_i2c_probe called\n", __func__, client->name);
	if (!i2c_check_functionality(client->adapter, I2C_FUNC_I2C)) {
		pr_err("%s %s i2c_check_functionality failed\n",
			__func__, client->name);
		rc = -EFAULT;
		return rc;
	}
	
	s_ctrl = (struct msm_sensor_ctrl_t *)(id->driver_data);
	s_ctrl->sensor_device_type = MSM_SENSOR_I2C_DEVICE;
	if (s_ctrl->sensor_i2c_client != NULL) {
		s_ctrl->sensor_i2c_client->client = client;
		if (s_ctrl->sensor_i2c_addr != 0)
			s_ctrl->sensor_i2c_client->client->addr =
				s_ctrl->sensor_i2c_addr;
	} else {
		pr_err("%s %s sensor_i2c_client NULL\n",
			__func__, client->name);
		rc = -EFAULT;
		return rc;
	}
	s_ctrl->sensordata = client->dev.platform_data;
	if (s_ctrl->sensordata == NULL) {
		pr_err("%s %s NULL sensor data\n", __func__, client->name);
		return -EFAULT;
	}

	rc = s_ctrl->func_tbl->sensor_power_up(s_ctrl);
	if (rc < 0) {
		pr_err("%s %s power up failed\n", __func__, client->name);
		return rc;
	}
	
	if (s_ctrl->func_tbl->sensor_match_id)
		rc = s_ctrl->func_tbl->sensor_match_id(s_ctrl);
	else
		rc = msm_sensor_match_id(s_ctrl);
	if (rc < 0)
		goto probe_fail;

#ifdef CONFIG_SENSOR_INFO 
    	msm_sensorinfo_set_front_sensor_id(s_ctrl->sensor_id_info->sensor_id);
#else
  //do nothing here
#endif

	if (!s_ctrl->wait_num_frames)
		s_ctrl->wait_num_frames = 1 * Q10;

	pr_err("%s %s probe succeeded\n", __func__, client->name);
	snprintf(s_ctrl->sensor_v4l2_subdev.name,
		sizeof(s_ctrl->sensor_v4l2_subdev.name), "%s", id->name);
	v4l2_i2c_subdev_init(&s_ctrl->sensor_v4l2_subdev, client,
		s_ctrl->sensor_v4l2_subdev_ops);
	s_ctrl->sensor_v4l2_subdev.flags |= V4L2_SUBDEV_FL_HAS_DEVNODE;
	media_entity_init(&s_ctrl->sensor_v4l2_subdev.entity, 0, NULL, 0);
	s_ctrl->sensor_v4l2_subdev.entity.type = MEDIA_ENT_T_V4L2_SUBDEV;
	s_ctrl->sensor_v4l2_subdev.entity.group_id = SENSOR_DEV;
	s_ctrl->sensor_v4l2_subdev.entity.name =
		s_ctrl->sensor_v4l2_subdev.name;
	msm_sensor_register(&s_ctrl->sensor_v4l2_subdev);
	s_ctrl->sensor_v4l2_subdev.entity.revision =
		s_ctrl->sensor_v4l2_subdev.devnode->num;

	msm_sensor_enable_debugfs(s_ctrl);
	goto power_down;

probe_fail:
	pr_err("%s %s_i2c_probe failed\n", __func__, client->name);
power_down:
	if (rc > 0)
		rc = 0;
	s_ctrl->func_tbl->sensor_power_down(s_ctrl);
	s_ctrl->sensor_state = MSM_SENSOR_POWER_DOWN;
	#if 1
	s_ctrl->func_tbl->sensor_power_up(s_ctrl);
	
	
	if (s_ctrl->func_tbl->sensor_setting(s_ctrl, MSM_SENSOR_REG_INIT, 0) < 0)
	{
		pr_err("%s  sensor_setting init  failed\n",__func__);
		return rc;
	}	

	s_ctrl->func_tbl->sensor_stop_stream(s_ctrl);

	
	msm_sensor_ov9740_write_res_settings(s_ctrl, 0);

	s_ctrl->func_tbl->sensor_start_stream(s_ctrl);
	
	msleep(10);
	s_ctrl->func_tbl->sensor_power_down(s_ctrl);
	#endif
	return rc;
}
#else
int msm_camera_ov9740_power_on(struct msm_sensor_ctrl_t *s_ctrl)
{
	int32_t rc = 0;
	struct msm_camera_sensor_info *data = s_ctrl->sensordata;
	struct device *dev = NULL;	
	pr_err("%s  %d \n",__func__,__LINE__);
	if (s_ctrl->sensor_device_type == MSM_SENSOR_PLATFORM_DEVICE)
		dev = &s_ctrl->pdev->dev;
	else
		dev = &s_ctrl->sensor_i2c_client->client->dev;

	rc = msm_camera_config_vreg(dev,
		s_ctrl->sensordata->sensor_platform_info->cam_vreg,
		s_ctrl->sensordata->sensor_platform_info->num_vreg,
		s_ctrl->vreg_seq,
		s_ctrl->num_vreg_seq,
		s_ctrl->reg_ptr, 1);
	if (rc < 0) {
		pr_err("%s: regulator on failed\n", __func__);
		goto config_vreg_failed;
	}

	rc = msm_camera_enable_vreg(dev,
		s_ctrl->sensordata->sensor_platform_info->cam_vreg,
		s_ctrl->sensordata->sensor_platform_info->num_vreg,
		s_ctrl->vreg_seq,
		s_ctrl->num_vreg_seq,
		s_ctrl->reg_ptr, 1);
	if (rc < 0) {
		pr_err("%s: enable regulator failed\n", __func__);
		goto enable_vreg_failed;
	}
	gpio_request(9,"ov9740");	
	rc=gpio_direction_output(9, 1);	
	if (rc < 0) {
		pr_err("%s: enable dvdd_ldo_en failed\n", __func__);
	}else{	
	pr_err("%s gpio9=1 dvdd_ldo=1\n",__func__);	
		}

	gpio_request(55,"ov9740");	
	rc=gpio_direction_output(55, 1);
	if (rc < 0) {
		pr_err("%s: enable avdd_ldo failed\n", __func__);
	}else{	
	pr_err("%s gpio55=1 avdd_ldo=1\n",__func__);	
		}	

	if (s_ctrl->clk_rate != 0)
		cam_8960_clk_info->clk_rate = s_ctrl->clk_rate;

	rc = msm_cam_clk_enable(dev, cam_8960_clk_info,
		s_ctrl->cam_clk, ARRAY_SIZE(cam_8960_clk_info), 1);
	if (rc < 0) {
		pr_err("%s: clk enable failed\n", __func__);
		goto enable_clk_failed;
	}
	
	rc = msm_camera_request_gpio_table(data, 1);
	if (rc < 0) {
		pr_err("%s: request gpio failed\n", __func__);
		goto request_gpio_failed;
	}
	rc = msm_camera_config_gpio_table(data, 1);
	if (rc < 0) {
		pr_err("%s: config gpio failed\n", __func__);
		goto config_gpio_failed;
	}

	if (!s_ctrl->power_seq_delay)
		usleep_range(1000, 2000);
	else if (s_ctrl->power_seq_delay < 20)
		usleep_range((s_ctrl->power_seq_delay * 1000),
			((s_ctrl->power_seq_delay * 1000) + 1000));
	else
		msleep(s_ctrl->power_seq_delay);

	if (data->sensor_platform_info->ext_power_ctrl != NULL)
		data->sensor_platform_info->ext_power_ctrl(1);

	if (data->sensor_platform_info->i2c_conf &&
		data->sensor_platform_info->i2c_conf->use_i2c_mux)
		msm_sensor_enable_i2c_mux(data->sensor_platform_info->i2c_conf);

	if (s_ctrl->sensor_device_type == MSM_SENSOR_PLATFORM_DEVICE) {
		rc = msm_sensor_cci_util(s_ctrl->sensor_i2c_client,
			MSM_CCI_INIT);
		if (rc < 0) {
			pr_err("%s cci_init failed\n", __func__);
			goto cci_init_failed;
		}
	}	
	s_ctrl->curr_res = MSM_SENSOR_INVALID_RES;
	return rc;

enable_vreg_failed:
	msm_camera_config_vreg(dev,
		s_ctrl->sensordata->sensor_platform_info->cam_vreg,
		s_ctrl->sensordata->sensor_platform_info->num_vreg,
		s_ctrl->vreg_seq,
		s_ctrl->num_vreg_seq,
		s_ctrl->reg_ptr, 0);
config_vreg_failed:
	msm_camera_request_gpio_table(data, 0);
cci_init_failed:
	if (data->sensor_platform_info->i2c_conf &&
		data->sensor_platform_info->i2c_conf->use_i2c_mux)
		msm_sensor_disable_i2c_mux(
			data->sensor_platform_info->i2c_conf);	
enable_clk_failed:
		msm_camera_config_gpio_table(data, 0);
config_gpio_failed:
	msm_camera_enable_vreg(dev,
			s_ctrl->sensordata->sensor_platform_info->cam_vreg,
			s_ctrl->sensordata->sensor_platform_info->num_vreg,
			s_ctrl->vreg_seq,
			s_ctrl->num_vreg_seq,
			s_ctrl->reg_ptr, 0);
request_gpio_failed:
	kfree(s_ctrl->reg_ptr);
			pr_err("%s X\n", __func__);	
	return rc;
}

int32_t msm_sensor_ov9740_power_up(struct msm_sensor_ctrl_t *s_ctrl)
{

	int32_t rc = 0;
		
#ifndef CONFIG_MACH_NEX
	int16_t  tmp_reg = 0;
#endif	
	struct msm_camera_sensor_info *data = s_ctrl->sensordata;
	struct device *dev = NULL;
	pr_err("%s  %d \n",__func__,__LINE__);
	if (s_ctrl->sensor_device_type == MSM_SENSOR_PLATFORM_DEVICE)
		dev = &s_ctrl->pdev->dev;
	else
		dev = &s_ctrl->sensor_i2c_client->client->dev;
	
	if (s_ctrl->clk_rate != 0)
		cam_8960_clk_info->clk_rate = s_ctrl->clk_rate;
	rc = msm_cam_clk_enable(dev, cam_8960_clk_info,
		s_ctrl->cam_clk, ARRAY_SIZE(cam_8960_clk_info), 1);
	if (rc < 0) {
		pr_err("%s: clk enable failed\n", __func__);
		goto enable_clk_failed;
	}
	
	rc=gpio_direction_output(53, 1);
	if (rc < 0)  
		pr_err("%s  gpio  direction 53 failed\n",__func__);
	
	usleep_range(1000,2000);
	
	rc=gpio_direction_output(53, 0);
	if (rc < 0)  
		pr_err("%s  gpio  direction 53 failed\n",__func__);
	usleep_range(1000,2000);
	
	rc=gpio_direction_output(76, 1);
	if (rc < 0)  
		pr_err("%s  gpio  direction 76 failed\n",__func__);
	
	usleep_range(5000,6000);
	
	rc=gpio_direction_output(76, 0);
	if (rc < 0)  
		pr_err("%s  gpio  direction 76 failed\n",__func__);
	
	usleep_range(5000,6000);

	rc=gpio_direction_output(76, 1);
	if (rc < 0)  
		pr_err("%s  gpio  direction 76 failed\n",__func__);
	
	usleep_range(1000,2000);
	
	rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,   0x3014, 0x05, 
		MSM_CAMERA_I2C_WORD_DATA);
	if (rc < 0) 
	pr_err("%s: i2c write failed\n", __func__);	
	rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client, 0x0100, 0x00, 
		MSM_CAMERA_I2C_BYTE_DATA);
	if (rc < 0) {
	pr_err("%s: OV9740_MODE_SELECT=standby failed\n", __func__);
	} 	

#ifndef CONFIG_MACH_NEX 
	//add by wxl change dvdd external supply
	rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client, 0x3021, 0x20, MSM_CAMERA_I2C_BYTE_DATA);
	if (rc < 0) {
		pr_err("%s: i2c write failed\n", __func__);		
		return rc;
	}
	else
		pr_err("%s: i2c write 3021 success\n", __func__);
	
	rc = msm_camera_i2c_read(s_ctrl->sensor_i2c_client,  0x3021, &tmp_reg, MSM_CAMERA_I2C_BYTE_DATA);
	if (rc < 0) {
		pr_err("%s: i2c read failed\n", __func__);		
		return rc;
	}
	else
		pr_err("%s:tmp_reg = %d \n", __func__,tmp_reg);
#endif

	return rc;
	msleep(10);

enable_clk_failed:
		msm_camera_config_gpio_table(data, 0);
			pr_err("%s X\n", __func__);	
	return rc;
}

int32_t msm_sensor_ov9740_power_down(struct msm_sensor_ctrl_t *s_ctrl)
{
	int32_t rc = 0;
	struct device *dev = NULL;	
	pr_err("%s\n", __func__);
	
	rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,  0x3002, 0xe8, 
	MSM_CAMERA_I2C_WORD_DATA);
		if (rc < 0) {
		pr_err("%s: i2c write failed\n", __func__);		
		return rc;
		}
	rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,  0x3004, 0x03, 
		MSM_CAMERA_I2C_WORD_DATA);
		if (rc < 0) {
		pr_err("%s: i2c write failed\n", __func__);		
		return rc;
		}
	rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,   0x3005, 0xff, 
		MSM_CAMERA_I2C_WORD_DATA);
		if (rc < 0) {
		pr_err("%s: i2c write failed\n", __func__);		
		return rc;
		}
	rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,   0x3014, 0x1d, 
		MSM_CAMERA_I2C_WORD_DATA);
		if (rc < 0) {
		pr_err("%s: i2c write failed\n", __func__);		
		return rc;
		}

	gpio_direction_output(53, 1);	
	pr_err("%s  gpio53 PWD output=0\n",__func__);	

	msm_cam_clk_enable(dev, cam_8960_clk_info, s_ctrl->cam_clk,
		ARRAY_SIZE(cam_8960_clk_info), 0);
	pr_err("%s  X\n",__func__);		
	return rc;
}
int32_t msm_ov9740_sensor_i2c_probe(struct i2c_client *client,
	const struct i2c_device_id *id)
{
	int rc = 0;
	struct msm_sensor_ctrl_t *s_ctrl;
	 	
	pr_err("%s %s_i2c_probe called\n", __func__, client->name);
	if (!i2c_check_functionality(client->adapter, I2C_FUNC_I2C)) {
		pr_err("%s %s i2c_check_functionality failed\n",
			__func__, client->name);
		rc = -EFAULT;
		return rc;
	}
	
	s_ctrl = (struct msm_sensor_ctrl_t *)(id->driver_data);
	s_ctrl->sensor_device_type = MSM_SENSOR_I2C_DEVICE;
	if (s_ctrl->sensor_i2c_client != NULL) {
		s_ctrl->sensor_i2c_client->client = client;
		if (s_ctrl->sensor_i2c_addr != 0)
			s_ctrl->sensor_i2c_client->client->addr =
				s_ctrl->sensor_i2c_addr;
	} else {
		pr_err("%s %s sensor_i2c_client NULL\n",
			__func__, client->name);
		rc = -EFAULT;
		return rc;
	}
	s_ctrl->sensordata = client->dev.platform_data;
	if (s_ctrl->sensordata == NULL) {
		pr_err("%s %s NULL sensor data\n", __func__, client->name);
		return -EFAULT;
	}

	s_ctrl->sensordata = client->dev.platform_data;
	s_ctrl->reg_ptr = kzalloc(sizeof(struct regulator *)
			* s_ctrl->sensordata->sensor_platform_info->num_vreg, GFP_KERNEL);
	if (!s_ctrl->reg_ptr) {
		pr_err("%s: could not allocate mem for regulators\n",
			__func__);
		return -ENOMEM;
	}

	msm_camera_ov9740_power_on(s_ctrl);
	if (rc < 0) {
	pr_err("%s %s power on failed\n", __func__, client->name);
	return rc;
	}

	rc = msm_sensor_match_id(s_ctrl);
	if (rc < 0)
	goto probe_fail;

#ifdef CONFIG_SENSOR_INFO
	msm_sensorinfo_set_front_sensor_id(s_ctrl->sensor_id_info->sensor_id);
#endif

	if (s_ctrl->func_tbl->sensor_setting(s_ctrl, MSM_SENSOR_REG_INIT, 0) < 0)
	{
	pr_err("%s  sensor_setting init  failed\n",__func__);
	return rc;
	}
    //added for reduceing the high electricity	in standby after sleep
	rc=msm_sensor_ov9740_write_res_settings(s_ctrl, 0);
	if (rc < 0) {
	pr_err("%s: msm_sensor_write_res_settings failed\n", __func__);
	return rc;
	}

	rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client, 0x0100, 0x01, 
		MSM_CAMERA_I2C_BYTE_DATA);
	if (rc < 0) {
	pr_err("%s: streaming on \n", __func__);
	} 
	msleep(10);

	rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client, 0x0100, 0x00, 
		MSM_CAMERA_I2C_BYTE_DATA);
	if (rc < 0) {
	pr_err("%s: streaming off \n", __func__);
	} 
	
	
	
	if (!s_ctrl->wait_num_frames)
		s_ctrl->wait_num_frames = 1 * Q10;

	pr_err("%s %s probe succeeded\n", __func__, client->name);
	snprintf(s_ctrl->sensor_v4l2_subdev.name,
		sizeof(s_ctrl->sensor_v4l2_subdev.name), "%s", id->name);
	v4l2_i2c_subdev_init(&s_ctrl->sensor_v4l2_subdev, client,
		s_ctrl->sensor_v4l2_subdev_ops);
	s_ctrl->sensor_v4l2_subdev.flags |= V4L2_SUBDEV_FL_HAS_DEVNODE;
	media_entity_init(&s_ctrl->sensor_v4l2_subdev.entity, 0, NULL, 0);
	s_ctrl->sensor_v4l2_subdev.entity.type = MEDIA_ENT_T_V4L2_SUBDEV;
	s_ctrl->sensor_v4l2_subdev.entity.group_id = SENSOR_DEV;
	s_ctrl->sensor_v4l2_subdev.entity.name =
		s_ctrl->sensor_v4l2_subdev.name;

	msm_sensor_register(&s_ctrl->sensor_v4l2_subdev);
	s_ctrl->sensor_v4l2_subdev.entity.revision =
		s_ctrl->sensor_v4l2_subdev.devnode->num;
	msm_sensor_enable_debugfs(s_ctrl);
	goto power_down;

probe_fail:
	pr_err("%s %s_i2c_probe failed\n", __func__, client->name);
power_down:
	if (rc > 0)
		rc = 0;
	s_ctrl->func_tbl->sensor_power_down(s_ctrl);
	s_ctrl->sensor_state = MSM_SENSOR_POWER_DOWN;

	return rc;
}
#endif

static struct i2c_driver ov9740_i2c_driver = {
	.id_table = ov9740_i2c_id,
	.probe  = msm_ov9740_sensor_i2c_probe,
	.driver = {
		.name = SENSOR_NAME,
	},
};

static struct msm_camera_i2c_client ov9740_sensor_i2c_client = {
	.addr_type = MSM_CAMERA_I2C_WORD_ADDR,
};

int32_t msm_ov9740_sensor_setting(struct msm_sensor_ctrl_t *s_ctrl,
			int update_type, int res)
{
	int32_t rc = 0;
	pr_err("%s E LINE=%d,res=%d\n", __func__, __LINE__,res);	

	if (update_type == MSM_SENSOR_REG_INIT) {
		rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client, 0x0103, 0x01, MSM_CAMERA_I2C_BYTE_DATA);
		if (rc < 0) {
		pr_err("%s: i2c write failed\n", __func__);		
		return rc;
		}
		msleep(10);	
	} 
	else if (update_type == MSM_SENSOR_UPDATE_PERIODIC) {
		pr_err("%s: UPDATE_PERIODIC,res=%dn", __func__,res);
		rc=msm_sensor_ov9740_write_res_settings(s_ctrl, res);
		if (rc < 0) {
		pr_err("%s: msm_sensor_write_res_settings failed\n", __func__);
		return rc;
		}
		v4l2_subdev_notify(&s_ctrl->sensor_v4l2_subdev,
		NOTIFY_PCLK_CHANGE, &s_ctrl->msm_sensor_reg->
		output_settings[res].op_pixel_clk);
	}	
	pr_err("%s X line=%d  res=%d\n",__func__,__LINE__,res);			
	return rc;
	}
static enum msm_camera_vreg_name_t ov9740_veg_seq[] = {
	//CAM_VIO,
	CAM_VDIG,
	//CAM_VANA,
};
static int __init msm_sensor_init_module(void)
{
	pr_err("msm  ov9740 %s: %d\n", __func__, __LINE__);
	return i2c_add_driver_async(&ov9740_i2c_driver);
}

static struct v4l2_subdev_core_ops ov9740_subdev_core_ops = {
	.s_ctrl = msm_sensor_v4l2_s_ctrl,
	.queryctrl = msm_sensor_v4l2_query_ctrl,
	.ioctl = msm_sensor_subdev_ioctl,
	.s_power = msm_sensor_power,
};
static struct v4l2_subdev_video_ops ov9740_subdev_video_ops = {
	.enum_mbus_fmt = msm_sensor_v4l2_enum_fmt,
};

static struct v4l2_subdev_ops ov9740_subdev_ops = {
	.core = &ov9740_subdev_core_ops,
	.video  = &ov9740_subdev_video_ops,
};

static struct msm_sensor_fn_t ov9740_func_tbl = {
	.sensor_start_stream = ov9740_start_stream,
	.sensor_stop_stream = ov9740_stop_stream,
	.sensor_setting = msm_ov9740_sensor_setting,
	.sensor_set_sensor_mode = msm_sensor_set_sensor_mode,
	.sensor_mode_init = msm_sensor_mode_init,
	.sensor_get_output_info = msm_sensor_get_output_info,
	.sensor_config = msm_sensor_config,
	.sensor_power_up = msm_sensor_ov9740_power_up,
	.sensor_power_down = msm_sensor_ov9740_power_down,
	.sensor_get_csi_params = msm_sensor_get_csi_params,

};

static struct msm_sensor_reg_t ov9740_regs = {
	.default_data_type = MSM_CAMERA_I2C_BYTE_DATA,
	.start_stream_conf = ov9740_prev_settings,
	.start_stream_conf_size = ARRAY_SIZE(ov9740_prev_settings),
	.init_settings = &ov9740_init_conf[0],
	.init_size = ARRAY_SIZE(ov9740_init_conf),
	.mode_settings = &ov9740_confs[0],
	.output_settings = &ov9740_dimensions[0],
	.num_conf = ARRAY_SIZE(ov9740_confs),
};

static struct msm_sensor_ctrl_t ov9740_s_ctrl = {
	.msm_sensor_reg = &ov9740_regs,
	.msm_sensor_v4l2_ctrl_info = ov9740_v4l2_ctrl_info,
	.num_v4l2_ctrl = ARRAY_SIZE(ov9740_v4l2_ctrl_info),
	.sensor_i2c_client = &ov9740_sensor_i2c_client,
	.sensor_i2c_addr = 0x20,
	.vreg_seq = ov9740_veg_seq,
	.num_vreg_seq = ARRAY_SIZE(ov9740_veg_seq),
	.sensor_output_reg_addr = &ov9740_reg_addr,
	.sensor_id_info = &ov9740_id_info,
	.cam_mode = MSM_SENSOR_MODE_INVALID,
	.min_delay = 30,
	.power_seq_delay = 60,	
	.msm_sensor_mutex = &ov9740_mut,
	.sensor_i2c_driver = &ov9740_i2c_driver,
	.sensor_v4l2_subdev_info = ov9740_subdev_info,
	.sensor_v4l2_subdev_info_size = ARRAY_SIZE(ov9740_subdev_info),
	.sensor_v4l2_subdev_ops = &ov9740_subdev_ops,
	.func_tbl = &ov9740_func_tbl,
	.msm_sensor_reg_default_data_type=MSM_CAMERA_I2C_BYTE_DATA,
};

module_init(msm_sensor_init_module);
MODULE_DESCRIPTION("OV 1.3MP YUV sensor driver");
MODULE_LICENSE("GPL v2");


