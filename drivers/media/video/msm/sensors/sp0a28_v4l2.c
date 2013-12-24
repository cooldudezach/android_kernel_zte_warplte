/* Copyright (c) 2012, Code Aurora Forum. All rights reserved.
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
#define SENSOR_NAME "sp0a28"

#if 0

#define SP0A28_P0_0xdd	0x80
#define SP0A28_P0_0xde	0xb0//80
//sharpness                          
#define SP0A28_P1_0xe8	0x20//10//;sharp_fac_pos_outdoor
#define SP0A28_P1_0xec	0x30//20//;sharp_fac_neg_outdoor
#define SP0A28_P1_0xe9	0x10//0a//;sharp_fac_pos_nr
#define SP0A28_P1_0xed	0x30//20//;sharp_fac_neg_nr
#define SP0A28_P1_0xea	0x10//08//;sharp_fac_pos_dummy
#define SP0A28_P1_0xef	0x20//18//;sharp_fac_neg_dummy
#define SP0A28_P1_0xeb	0x10//08//;sharp_fac_pos_low
#define SP0A28_P1_0xf0	0x10//18//;sharp_fac_neg_low 
//saturation
#define SP0A28_P0_0xd3	0x90
#define SP0A28_P0_0xd4	0x90
#define SP0A28_P0_0xd6	0x70
#define SP0A28_P0_0xd7	0x60
#define SP0A28_P0_0xd8	0x90
#define SP0A28_P0_0xd9	0x90
#define SP0A28_P0_0xda	0x70
#define SP0A28_P0_0xdb	0x60
                                
//Ae target
#define SP0A28_P0_0xf7	0x78//0x80
#define SP0A28_P0_0xf8	0x70//0x78
#define SP0A28_P0_0xf9	0x78//0x80 
#define SP0A28_P0_0xfa	0x70//0x78 
#else
#define SP0A28_P0_0xdd	0x78
#define SP0A28_P0_0xde	0x98//0xa8 //zxl //0xb0//94  a8
//sharpness                          
#define SP0A28_P1_0xe8	0x14//0x10//10//;sharp_fac_pos_outdoor
#define SP0A28_P1_0xec	0x24//0x20//20//;sharp_fac_neg_outdoor
#define SP0A28_P1_0xe9	0x0e//0x0a//0a//;sharp_fac_pos_nr
#define SP0A28_P1_0xed	0x24//0x20//20//;sharp_fac_neg_nr
#define SP0A28_P1_0xea	0x0c//0x08//08//;sharp_fac_pos_dummy
#define SP0A28_P1_0xef	0x1c//0x18//18//;sharp_fac_neg_dummy
#define SP0A28_P1_0xeb	0x0c//0x08//08//;sharp_fac_pos_low
#define SP0A28_P1_0xf0	0x0c//0x08//18//;sharp_fac_neg_low   
//saturation
#define SP0A28_P0_0xd3	0x90//0x80
#define SP0A28_P0_0xd4	0x90//0x80
#define SP0A28_P0_0xd6	0x8C//0x8C//0x7c
#define SP0A28_P0_0xd7	0x60 
#define SP0A28_P0_0xd8	0x90//0x80
#define SP0A28_P0_0xd9	0x90//0x80
#define SP0A28_P0_0xda	0x8C//0x8c//0x7c
#define SP0A28_P0_0xdb	0x60
                                
//Ae target
#define SP0A28_P0_0xf7	0x84//0x80//0x80
#define SP0A28_P0_0xf8	0x80//0x78//0x78
#define SP0A28_P0_0xf9	0x80//0x7c//0x80 
#define SP0A28_P0_0xfa	0x78//0x74//0x78 
#endif

static int g_wb;
static int g_iso;

DEFINE_MUTEX(sp0a28_mut);

extern void msm_sensorinfo_set_front_sensor_id(uint16_t id);
int32_t msm_sp0a28_sensor_i2c_probe(struct i2c_client *client,
	const struct i2c_device_id *id);

static struct msm_sensor_ctrl_t sp0a28_s_ctrl;

static unsigned int SP0A28_INIT_TMP= 0;


static int effect_value = CAMERA_EFFECT_OFF;
static unsigned int SAT_U = 0x80; /* DEFAULT SATURATION VALUES*/
static unsigned int SAT_V = 0x80; /* DEFAULT SATURATION VALUES*/

static struct msm_camera_i2c_reg_conf sp0a28_start_settings[] = {
	{0xfd, 0x00},
};

static struct msm_camera_i2c_reg_conf sp0a28_stop_settings[] = {
	{0xfd, 0x00},
};

static struct msm_camera_i2c_reg_conf sp0a28_recommend_settings[] = { 
	#if 1
{0xfd,0x01},
{0x7c,0x6c},
{0xfd,0x00},
{0x1C,0x00},
{0x32,0x00},
{0x0e,0x00},
{0x0f,0x40},
{0x10,0x40},
{0x11,0x10},
{0x12,0xa0},
{0x13,0xf0},//40
{0x14,0x30},//add
{0x15,0x00},
{0x16,0x08},//add
{0x1A,0x37},
{0x1B,0x17},
{0x1C,0x2f},  //close yuv
{0x1d,0x00},
{0x1E,0x57},
{0x21,0x34},//0x36  hqz
{0x22,0x12},
{0x24,0x80},
{0x25,0x02},
{0x26,0x03},
{0x27,0xeb},
{0x28,0x5f},
{0x2f,0x01},
{0x5f,0x02},
{0xf4,0x09}, //add
{0xfb,0x33}, 
{0xe7,0x03},
{0xe7,0x00},
//    blacklevel
{0x65,0x18}, //add
{0x66,0x18}, 
{0x67,0x18},
{0x68,0x18},
#if  0
//ae setting 16M 10-12ps
{0xfd,0x00},
{0x03,0x00},
{0x04,0xb4},
{0x05,0x00},
{0x06,0x00},
{0x09,0x01},
{0x0a,0xd9},
{0xf0,0x3c},
{0xf1,0x00},
{0xfd,0x01},
{0x90,0x0a},
{0x92,0x01},
{0x98,0x3c},
{0x99,0x00},
{0x9a,0x01},
{0x9b,0x00},
//Status
{0xfd,0x01},
{0xce,0x58},
{0xcf,0x02},
{0xd0,0x58},
{0xd1,0x02},
{0xfd,0x00},
#else //16M 8-12fps
{0xfd,0x00},
{0x03,0x00},
{0x04,0xb4},
{0x05,0x00},
{0x06,0x00},
{0x09,0x01},
{0x0a,0xd9},
{0xf0,0x3c},
{0xf1,0x00},
{0xfd,0x01},
{0x90,0x0c},
{0x92,0x01},
{0x98,0x3c},
{0x99,0x00},
{0x9a,0x01},
{0x9b,0x00},
{0xfd,0x01},
{0xce,0xd0},
{0xcf,0x02},
{0xd0,0xd0},
{0xd1,0x02},
{0xfd,0x00},
#endif
{0xfd,0x01},
{0xc4,0x6c},
{0xc5,0x7c},
{0xca,0x20},//0x30 hqz
{0xcb,0x35},//0x45
{0xcc,0x60},
{0xcd,0x60},

//DP
{0xfd,0x00},
{0x45,0x00},
{0x46,0x99},
{0x79,0xff},
{0x7a,0xff},
{0x7b,0x10},
{0x7c,0x10},

//lsc  for SX5044module
{0xfd,0x01},
{0x35,0x13},//0x0a
{0x36,0x14},//0x20
{0x37,0x1a},
{0x38,0x1c},//0x22
{0x39,0x0c},//0x06
{0x3a,0x0c},//0x1a
{0x3b,0x19},
{0x3c,0x13},//0x18
{0x3d,0x0c},//0x09
{0x3e,0x0d},//0x1c
{0x3f,0x14},//0x18
{0x40,0x1f},
{0x41,0x0d},//0x00
{0x42,0x06},//0x18
{0x43,0x0f},//0x02
{0x44,0x02},
{0x45,0x06},
{0x46,0x07},//0x14
{0x47,0x06},
{0x48,0xf8},
{0x49,0x04},//0xfc
{0x4a,0x08},//0x12
{0x4b,0x0a},
{0x4c,0xf1},
{0xfd,0x00},
{0xa1,0x20},
{0xa2,0x20},
{0xa3,0x20},
{0xa4,0xff},  
//smooth  
{0xfd,0x01},
{0xde,0x0f},
{0xfd,0x00},
//单通道间平滑阈值	
{0x57,0x04},	//	;raw_dif_thr_outdoor
{0x58,0x06},//0x0a	// ;raw_dif_thr_normal   12  0a
{0x56,0x10},//0x10	// ;raw_dif_thr_dummy  12  
{0x59,0x10},	// ;raw_dif_thr_lowlight  18
//GrGb平滑阈值
{0x89,0x04},	//;raw_grgb_thr_outdoor 
{0x8a,0x06}, //;raw_grgb_thr_normal   0a
{0x9c,0x10}, //;raw_grgb_thr_dummy   
{0x9d,0x10}, //;raw_grgb_thr_lowlight  15
//Gr\Gb之间平滑强度
{0x81,0xe0}, //   ;raw_gflt_fac_outdoor
{0x82,0xa0}, //;80;raw_gflt_fac_normal  98
{0x83,0x78}, //   ;raw_gflt_fac_dummy
{0x84,0x78}, //   ;raw_gflt_fac_lowlight
//Gr、Gb单通道内平滑强度  
{0x85,0xe0}, //;raw_gf_fac_outdoor  
{0x86,0xA8}, //;raw_gf_fac_normal   98  a0
{0x87,0x78}, //;raw_gf_fac_dummy    78
{0x88,0x78}, //;raw_gf_fac_lowlight 78
//R、B平滑强度  
{0x5a,0xff},		//;raw_rb_fac_outdoor
{0x5b,0xc0}, 		//;raw_rb_fac_normal B8
{0x5c,0x98}, 	  //;raw_rb_fac_dummy 98
{0x5d,0x78}, 	  //;raw_rb_fac_lowlight 78
//adt 平滑阈值自适应
{0xa7,0xff},
{0xa8,0xff},	//;0x2f
{0xa9,0xff},	//;0x2f
{0xaa,0xff},	//;0x2f
//dem_morie_thr 去摩尔纹
{0x9e,0x10},
//sharpen 
{0xfd,0x01},	//
{0xe2,0x30},	//	;sharpen_y_base
{0xe4,0xa0},	//	;sharpen_y_max
{0xe5,0x08},	// ;rangek_neg_outdoor
{0xd3,0x10},	// ;rangek_pos_outdoor   
{0xd7,0x08},	// ;range_base_outdoor   
{0xe6,0x08},//0x08	// ;rangek_neg_normal
{0xd4,0x15},//0x10	// ;rangek_pos_normal 
{0xd8,0x08},	// ;range_base_normal  
{0xe7,0x10},	// ;rangek_neg_dummy
{0xd5,0x10},	// ;rangek_pos_dummy
{0xd9,0x10},	// ;range_base_dummy 
{0xd2,0x10},	// ;rangek_neg_lowlight
{0xd6,0x10},	// ;rangek_pos_lowlight
{0xda,0x10},	// ;range_base_lowlight
{0xe8,SP0A28_P1_0xe8},//0x20	//;sharp_fac_pos_outdoor
{0xec,SP0A28_P1_0xec},//0x30	//;sharp_fac_neg_outdoor
{0xe9,SP0A28_P1_0xe9},//0x10	//;sharp_fac_pos_nr
{0xed,SP0A28_P1_0xed},//0x30	//;sharp_fac_neg_nr
{0xea,SP0A28_P1_0xea},//0x10	//;sharp_fac_pos_dummy
{0xef,SP0A28_P1_0xef},//0x20	//;sharp_fac_neg_dummy
{0xeb,SP0A28_P1_0xeb},//0x10	//;sharp_fac_pos_low
{0xf0,SP0A28_P1_0xf0},//0x20	//;sharp_fac_neg_low 
#if 1
//CCM
{0xfd,0x01},	//
{0xa0,0x99},	//;8c;80;80;80(红色接近，肤色不理想)
{0xa1,0xf4},	//;0c;00;0 ;0 
{0xa2,0xf4},	//;e8;00;0 ;0 
{0xa3,0xf6},	//;ec;ff;f2;f3;f0
{0xa4,0x99},	//;99;9a;8e;a6
{0xa5,0xf2},	//;fb;e7;0 ;ea
{0xa6,0x0d},	//;0d;0c;0 ;0 
{0xa7,0xda},	//;da;da;e6;e6
{0xa8,0x98},	//;98;9a;9a;9a
{0xa9,0x3c},	//;30;00;0 ;0 
{0xaa,0x33},	//;33;33;3 ;33
{0xab,0x0c},	//;0c;0c;c ;c 
{0xfd,0x00},	//;00	 
#else
//CCM
{0xfd,0x01},	//
{0xa0,0x80},	//;8c;80;80;80(红色接近，肤色不理想)
{0xa1,0x00},	//;0c;00;0 ;0 
{0xa2,0x00},	//;e8;00;0 ;0 
{0xa3,0xf6},	//;ec;ff;f2;f3;f0
{0xa4,0x99},	//;99;9a;8e;a6
{0xa5,0xf2},	//;fb;e7;0 ;ea
{0xa6,0x0d},	//;0d;0c;0 ;0 
{0xa7,0xda},	//;da;da;e6;e6
{0xa8,0x98},	//;98;9a;9a;9a
{0xa9,0x00},	//;30;00;0 ;0 
{0xaa,0x33},	//;33;33;3 ;33
{0xab,0x0c},	//;0c;0c;c ;c 
{0xfd,0x00},	//;00	 
#endif
/*//gamma  
{0xfd,0x00},	//00
{0x8b,0x00},	//0       
{0x8c,0x0f},	//12
{0x8d,0x21},	//1f
{0x8e,0x3b},	//31
{0x8f,0x64},	//4c
{0x90,0x84},	//62
{0x91,0xa0},	//77
{0x92,0xb6},	//89
{0x93,0xc7},	//9b
{0x94,0xd2},	//a8
{0x95,0xda},	//b5
{0x96,0xe0},	//c0
{0x97,0xe6},	//ca
{0x98,0xea},	//d4
{0x99,0xef},	//dd
{0x9a,0xf4},	//e6
{0x9b,0xf7},	//ef
{0xfd,0x01},	//01
{0x8d,0xfb},	//f7
{0x8e,0xff},	//ff
{0xfd,0x00},	//00*/
//gamma  
{0xfd,0x00},	//00
{0x8b,0x00},	//0       
{0x8c,0x0c},	//12
{0x8d,0x19},	//1f
{0x8e,0x2c},	//31
{0x8f,0x49},	//4c
{0x90,0x61},	//62
{0x91,0x77},	//77
{0x92,0x8a},	//89
{0x93,0x9b},	//9b
{0x94,0xa9},	//a8
{0x95,0xb5},	//b5
{0x96,0xc0},	//c0
{0x97,0xca},	//ca
{0x98,0xd4},	//d4
{0x99,0xdd},	//dd
{0x9a,0xe6},	//e6
{0x9b,0xef},	//ef
{0xfd,0x01},	//01
{0x8d,0xf7},	//f7
{0x8e,0xff},	//ff
{0xfd,0x00},	//00

/*//gamma 灰阶分布好  
{0xfd,0x00},	
{0x8b,0x00},	      
{0x8c,0x11},	
{0x8d,0x24},	
{0x8e,0x3f},	
{0x8f,0x64},	
{0x90,0x7f},	
{0x91,0x93},	
{0x92,0xa4},	
{0x93,0xb2},	
{0x94,0xbb},	
{0x95,0xc4},	
{0x96,0xcb},	
{0x97,0xd1},	
{0x98,0xd5},	
{0x99,0xdc},	
{0x9a,0xe2},	
{0x9b,0xe9},	
{0xfd,0x01},	
{0x8d,0xf2},	
{0x8e,0xff},	
{0xfd,0x00},	
*/
//awb for 后摄
{0xfd,0x01},
{0x28,0xc4},
{0x29,0x9e},
{0x11,0x13},	
{0x12,0x13},
{0x2e,0x13},	
{0x2f,0x13},
{0x16,0x1c},
{0x17,0x1a},
{0x18,0x1a},	
{0x19,0x54},	
{0x1a,0xa5},  
{0x1b,0x9a}, 
{0x2a,0xef},

 /*// awb for 前摄      
 {0xfd,0x01},    
 {0x11,0x08},    
 {0x12,0x08},    
 {0x2e,0x04},    
 {0x2f,0x04},    
 {0x16,0x1c},    
 {0x17,0x1a},    
 {0x18,0x16},    
 {0x19,0x54},    
 {0x1a,0x90},    
 {0x1b,0x9b},    
 {0x2a,0xef},    
 {0x2b,0x30},    
 {0x21,0x96},    
 {0x22,0x9a},    
*/
//AE;rpc 
{0xfd,0x00},
{0xe0,0x3a},//
{0xe1,0x2c},//24,
{0xe2,0x26},//
{0xe3,0x22},//
{0xe4,0x22},//
{0xe5,0x20},//
{0xe6,0x20},//
{0xe8,0x20},//19,
{0xe9,0x20},//19,
{0xea,0x20},//19,
{0xeb,0x1e},//18,
{0xf5,0x1e},//18,
{0xf6,0x1e},//18,
//ae min gain  
{0xfd,0x01},
{0x94,0x60},
{0x95,0x1e},//0x18
{0x9c,0x60},
{0x9d,0x1e},//0x18   
//ae target
{0xfd,0x00},
{0xed,SP0A28_P0_0xf7 + 0x04},//0x84 
{0xf7,SP0A28_P0_0xf7},		//0x80 
{0xf8,SP0A28_P0_0xf8},		//0x78 
{0xec,SP0A28_P0_0xf8 - 0x04},//0x74  
{0xef,SP0A28_P0_0xf9 + 0x04},//0x84
{0xf9,SP0A28_P0_0xf9},		//0x80
{0xfa,SP0A28_P0_0xfa},		//0x78
{0xee,SP0A28_P0_0xfa - 0x04},//0x74
//gray detect
{0xfd,0x01},
{0x30,0x40},
{0x31,0x70},//10
{0x32,0x20},
{0x33,0xef},
{0x34,0x02},
{0x4d,0x40},
{0x4e,0x15},
{0x4f,0x13},
//saturation
{0xfd,0x00},
{0xbe,0xaa},  
{0xc0,0xff},
{0xc1,0xff},
{0xd3,SP0A28_P0_0xd3},
{0xd4,SP0A28_P0_0xd4}, 
{0xd6,SP0A28_P0_0xd6},
{0xd7,SP0A28_P0_0xd7},
{0xd8,SP0A28_P0_0xd8},
{0xd9,SP0A28_P0_0xd9}, 
{0xda,SP0A28_P0_0xda},
{0xdb,SP0A28_P0_0xdb},
//heq   
{0xfd,0x00},
{0xdc,0x00},	//;heq_offset
{0xdd,SP0A28_P0_0xdd},	//;ku
{0xde,SP0A28_P0_0xde},	//;90;kl 80
{0xdf,0x80},	//;heq_mean
//YCnr  
{0xfd,0x00},
{0xc2,0x08},	//Ynr_thr_outdoor
{0xc3,0x08},	//Ynr_thr_normal
{0xc4,0x08},	//Ynr_thr_dummy
{0xc5,0x10},	//Ynr_thr_lowlight
{0xc6,0x80},	//cnr_thr_outdoor
{0xc7,0x80},	//cnr_thr_normal  
{0xc8,0x80},	//cnr_thr_dummy   
{0xc9,0x80},	//cnr_thr_lowlight  
//auto lum
{0xfd,0x00},
{0xb2,0x18},
{0xb3,0x1f},
{0xb4,0x20},
{0xb5,0x35},
//func enable
{0xfd,0x00},
{0x32,0x0d},
{0x31,0x70},//0x10 mofify facebook preview orientation of front camera 20130416
{0x34,0x7e},// 1e
{0x33,0xff},
{0x35,0x00},
{0xf4,0x09},
#else
{0xfd,0x01},
{0x7c,0x6c},
{0xfd,0x00},
{0xf4,0x09},// increase AE adjustment speed
{0x1C,0x00},
{0x32,0x00},
{0x0e,0x00},
{0x0f,0x40},
{0x10,0x40},
{0x11,0x10},
{0x12,0xa0},
{0x13,0xf0},//40
{0x14,0x40},//add
{0x15,0x00},
{0x16,0x08},//add
{0x1A,0x37},
{0x1B,0x17},
{0x1C,0x2f},  //close yuv
{0x1d,0x00},
{0x1E,0x57},
{0x21,0x36},//2f
{0x22,0x12},
{0x24,0x80},
{0x25,0x02},
{0x26,0x03},
{0x27,0xeb},
{0x28,0x5f},
{0x2f,0x01},
{0x5f,0x02},
{0xfb,0x33}, 
{0xe7,0x03},
{0xe7,0x00},
//black
{0xfd,0x00},
{0x65,0x18},//add
{0x66,0x18},//add
{0x67,0x18},//add
{0x68,0x18},//add
#if 0
//ae setting
{0xfd,0x00},
{0x03,0x01},
{0x04,0x2c},
{0x05,0x00},
{0x06,0x00},
{0x09,0x01},
{0x0a,0x54},
{0xf0,0x64},
{0xf1,0x00},
{0xfd,0x01},
{0x90,0x0c},
{0x92,0x01},
{0x98,0x64},
{0x99,0x00},
{0x9a,0x03}, // 0x01 // 0x03
{0x9b,0x00},
//Status
{0xfd,0x01},
{0xce,0xb0},
{0xcf,0x04},
{0xd0,0xb0},
{0xd1,0x04},
{0xfd,0x00},
#endif
//ae setting
{0xfd,0x00},
{0x03,0x01},
{0x04,0x0e},
{0x05,0x00},
{0x06,0x00},
{0x09,0x01},
{0x0a,0xd9},
{0xf0,0x5a},
{0xf1,0x00},
{0xfd,0x01},
{0x90,0x0c},
{0x92,0x01},
{0x98,0x5a},
{0x99,0x00},
{0x9a,0x03}, // 0x01 // 0x03
{0x9b,0x00},
//Status
{0xfd,0x01},
{0xce,0x38},
{0xcf,0x04},
{0xd0,0x38},
{0xd1,0x04},
{0xfd,0x00},

{0xfd,0x01},
{0xc4,0x6c},
{0xc5,0x7c},
{0xca,0x30},
{0xcb,0x45},
{0xcc,0x60},//0x70
{0xcd,0x60},//0x70
//add by zhangxiliang
{0xfd,0x00},
{0x45,0x00},
{0x46,0x99},
{0x79,0xff},
{0x7a,0xff},
{0x7b,0x10},
{0x7c,0x10},
//end by zhangxiliang
//lsc  for SX5044module
{0xfd,0x01},
#if 1
#if 0
{0x35,0x18},//0x0a
{0x36,0x17},//0x20
{0x37,0x15},
{0x38,0x15},//0x22
{0x39,0x0f},//0x06   //15
{0x3a,0x12},//0x1a   //12
{0x3b,0x15},
{0x3c,0x0f},//0x18   //12
{0x3d,0x15},//0x09
{0x3e,0x15},//0x1c
{0x3f,0x15},//0x18
{0x40,0x15},
	{0x41,0x13},//0x00           //f//12
	{0x42,0x17},//0x18           //15
	{0x43,0x15},//0x02           //15
	{0x44,0x17},
	{0x45,0x06},            ///f //a
	{0x46,0x0a},//0x14           //f
	{0x47,0x08},           //f    //c
	{0x48,0x0a},                 //f
	{0x49,0x0a},//0xf//c
	{0x4a,0x0f},//0x12
	{0x4b,0x0f},
	{0x4c,0x0f},
#else
#if 0
{0x35,0x18},//0x0a
{0x36,0x17},//0x20
{0x37,0x15},
{0x38,0x15},//0x22
{0x39,0x15},//0x06   //15
{0x3a,0x15},//0x1a   //12
{0x3b,0x15},
{0x3c,0x12},//0x18   //12
{0x3d,0x15},//0x09
{0x3e,0x15},//0x1c
{0x3f,0x15},//0x18
{0x40,0x15},
#else
/*
{0x35,0x20},//0x0a
{0x36,0x1f},//0x20
{0x37,0x1d},
{0x38,0x1d},//0x22
{0x39,0x1d},//0x06   //15
{0x3a,0x1d},//0x1a   //12
{0x3b,0x1d},
{0x3c,0x1a},//0x18   //12
{0x3d,0x1d},//0x09
{0x3e,0x1d},//0x1c
{0x3f,0x1d},//0x18
{0x40,0x1d},
*/
	{0x35,0x1e},//0x0a
	{0x36,0x1d},//0x20
	{0x37,0x1b},
	{0x38,0x1b},//0x22
	{0x39,0x1b},//0x06	 //15
	{0x3a,0x1b},//0x1a	 //12
	{0x3b,0x1b},
	{0x3c,0x18},//0x18	 //12
	{0x3d,0x1b},//0x09
	{0x3e,0x1b},//0x1c
	{0x3f,0x1b},//0x18
	{0x40,0x1b},


#endif
	{0x41,0x05},//0x00           //f//12
	{0x42,0x08},//0x18           //15
	{0x43,0x08},//0x02           //15
	{0x44,0x09},
	{0x45,0x05},            ///f //a
	{0x46,0x05},//0x14           //f
	{0x47,0x05},           //f    //c
	{0x48,0x05},                 //f
	{0x49,0x05},//0xf//c
	{0x4a,0x05},//0x12
	{0x4b,0x05},
	{0x4c,0x05},


#endif
/*
// 效果接近
{0x35,0x18},//0x0a
{0x36,0x17},//0x20
{0x37,0x15},
{0x38,0x15},//0x22
{0x39,0x12},//0x06  15
{0x3a,0x15},//0x1a
{0x3b,0x15},
{0x3c,0x12},//0x18
{0x3d,0x15},//0x09
{0x3e,0x15},//0x1c
{0x3f,0x15},//0x18
{0x40,0x15},
	{0x41,0x0f},//0x00
	{0x42,0x15},//0x18
	{0x43,0x15},//0x02
	{0x44,0x17},
	{0x45,0x0a},            ///f
	{0x46,0x0f},//0x14
	{0x47,0x0c},           //f
	{0x48,0x0f},
	{0x49,0x0f},//0xfc
	{0x4a,0x0f},//0x12
	{0x4b,0x0f},
	{0x4c,0x0f},
*/
/*
{0x41,0x05},//0x00
{0x42,0x08},//0x18
{0x43,0x08},//0x02
{0x44,0x09},
{0x45,0x05},
{0x46,0x05},//0x14
{0x47,0x05},
{0x48,0x05},
{0x49,0x05},//0xfc
{0x4a,0x05},//0x12
{0x4b,0x05},
{0x4c,0x05},
*/
#else
{0x35,0x0e},//0x0a
{0x36,0x14},//0x20
{0x37,0x20},
{0x38,0x1c},//0x22
{0x39,0x09},//0x06
{0x3a,0x10},//0x1a
{0x3b,0x19},
{0x3c,0x1a},//0x18
{0x3d,0x08},//0x09
{0x3e,0x10},//0x1c
{0x3f,0x14},//0x18
{0x40,0x22},
{0x41,0x0a},//0x00
{0x42,0x00},//0x18
{0x43,0x0a},//0x02
{0x44,0xfc},
{0x45,0x00},
{0x46,0xfe},//0x14
{0x47,0x00},
{0x48,0xf8},
{0x49,0x00},//0xfc
{0x4a,0xf8},//0x12
{0x4b,0x00},
{0x4c,0xf2},
#endif
{0xfd,0x00},
{0xa1,0x20},
{0xa2,0x20},
{0xa3,0x20},
{0xa4,0xff},  
//smooth  
{0xfd,0x01},
{0xde,0x0f},
{0xfd,0x00},
//单通道间平滑阈值	
{0x57,0x04},	//	;raw_dif_thr_outdoor
{0x58,0x0a},//0x0a	// ;raw_dif_thr_normal
{0x56,0x10},//0x10	// ;raw_dif_thr_dummy
{0x59,0x10},	// ;raw_dif_thr_lowlight
//GrGb平滑阈值
{0x89,0x04},	//;raw_grgb_thr_outdoor 
{0x8a,0x0a}, //;raw_grgb_thr_normal  
{0x9c,0x10}, //;raw_grgb_thr_dummy   
{0x9d,0x10}, //;raw_grgb_thr_lowlight
//Gr\Gb之间平滑强度
{0x81,0xe0}, //   ;raw_gflt_fac_outdoor
{0x82,0xa0}, //;80;raw_gflt_fac_normal
{0x83,0x80}, //   ;raw_gflt_fac_dummy
{0x84,0x80}, //   ;raw_gflt_fac_lowlight
//Gr、Gb单通道内平滑强度  
{0x85,0xe0}, //;raw_gf_fac_outdoor  
{0x86,0xa0}, //;raw_gf_fac_normal  
{0x87,0x80}, //;raw_gf_fac_dummy   
{0x88,0x80}, //;raw_gf_fac_lowlight
//R、B平滑强度  
{0x5a,0xff},		//;raw_rb_fac_outdoor
{0x5b,0xc0}, 		//;raw_rb_fac_normal
{0x5c,0xa0}, 	  //;raw_rb_fac_dummy
{0x5d,0xa0}, 	  //;raw_rb_fac_lowlight
//adt 平滑阈值自适应
{0xa7,0xff},
{0xa8,0xff},	//;0x2f
{0xa9,0xff},	//;0x2f
{0xaa,0xff},	//;0x2f
//dem_morie_thr 去摩尔纹
{0x9e,0x10},
//sharpen 
{0xfd,0x01},	//
{0xe2,0x30},	//	;sharpen_y_base
{0xe4,0xa0},	//	;sharpen_y_max
{0xe5,0x08},	// ;rangek_neg_outdoor
{0xd3,0x10},	// ;rangek_pos_outdoor   
{0xd7,0x08},	// ;range_base_outdoor   
{0xe6,0x08},//0x08	// ;rangek_neg_normal
{0xd4,0x10},//0x10	// ;rangek_pos_normal 
{0xd8,0x08},	// ;range_base_normal  
{0xe7,0x10},	// ;rangek_neg_dummy
{0xd5,0x10},	// ;rangek_pos_dummy
{0xd9,0x10},	// ;range_base_dummy 
{0xd2,0x10},	// ;rangek_neg_lowlight
{0xd6,0x10},	// ;rangek_pos_lowlight
{0xda,0x10},	// ;range_base_lowlight
{0xe8,SP0A28_P1_0xe8},//0x20	//;sharp_fac_pos_outdoor
{0xec,SP0A28_P1_0xec},//0x30	//;sharp_fac_neg_outdoor
{0xe9,SP0A28_P1_0xe9},//0x10	//;sharp_fac_pos_nr
{0xed,SP0A28_P1_0xed},//0x30	//;sharp_fac_neg_nr
{0xea,SP0A28_P1_0xea},//0x10	//;sharp_fac_pos_dummy
{0xef,SP0A28_P1_0xef},//0x20	//;sharp_fac_neg_dummy
{0xeb,SP0A28_P1_0xeb},//0x10	//;sharp_fac_pos_low
{0xf0,SP0A28_P1_0xf0},//0x20	//;sharp_fac_neg_low 
//CCM
{0xfd,0x01},	//
{0xa0,0x80},	//;8c;80;80;80(红色接近，肤色不理想)
{0xa1,0x00},	//;0c;00;0 ;0 
{0xa2,0x00},	//;e8;00;0 ;0 
{0xa3,0xf6},	//;ec;ff;f2;f3;f0
{0xa4,0x99},	//;99;9a;8e;a6
{0xa5,0xf2},	//;fb;e7;0 ;ea
{0xa6,0x0d},	//;0d;0c;0 ;0 
{0xa7,0xda},	//;da;da;e6;e6
{0xa8,0x98},	//;98;9a;9a;9a
{0xa9,0x00},	//;30;00;0 ;0 
{0xaa,0x33},	//;33;33;3 ;33
{0xab,0x0c},	//;0c;0c;c ;c 
{0xfd,0x00},	//;00	 
//gamma  
{0xfd,0x00},	//00
{0x8b,0x00},	//0       
{0x8c,0x0f},	//12
{0x8d,0x21},	//1f
{0x8e,0x3b},	//31
{0x8f,0x64},	//4c
{0x90,0x84},	//62
{0x91,0xa0},	//77
{0x92,0xb6},	//89
{0x93,0xc7},	//9b
{0x94,0xd2},	//a8
{0x95,0xda},	//b5
{0x96,0xe0},	//c0
{0x97,0xe6},	//ca
{0x98,0xea},	//d4
{0x99,0xef},	//dd
{0x9a,0xf4},	//e6
{0x9b,0xf7},	//ef
{0xfd,0x01},	//01
{0x8d,0xfb},	//f7
{0x8e,0xff},	//ff
{0xfd,0x00},	//00

/*//gamma 灰阶分布好  
{0xfd,0x00},	
{0x8b,0x00},	      
{0x8c,0x11},	
{0x8d,0x24},	
{0x8e,0x3f},	
{0x8f,0x64},	
{0x90,0x7f},	
{0x91,0x93},	
{0x92,0xa4},	
{0x93,0xb2},	
{0x94,0xbb},	
{0x95,0xc4},	
{0x96,0xcb},	
{0x97,0xd1},	
{0x98,0xd5},	
{0x99,0xdc},	
{0x9a,0xe2},	
{0x9b,0xe9},	
{0xfd,0x01},	
{0x8d,0xf2},	
{0x8e,0xff},	
{0xfd,0x00},	
*/
//awb for 后摄
#if 1
#if 0
{0xfd,0x01},
{0x28,0xc4},
{0x29,0x95},
{0x11,0x16},//13	//1a // 0x20
{0x12,0x16},//13    //1a // 0x20
{0x2e,0x0e},//0d	
{0x2f,0x0e},//0d
{0x16,0x12},//1c //1c
{0x17,0x1a},
{0x18,0x1a},	
{0x19,0x54},	
{0x1a,0x98}, //0xa9 //ab
{0x1b,0xa0},//0x90 0x97 
{0x2a,0xef},
{0x59,0x04},//add
//{0x0f,0x00},//add
#else
{0xfd,0x01},
{0x28,0xc4},  
{0x29,0x9e},
{0x11,0x13},  
{0x12,0x13},
{0x2e,0x13},  
{0x2f,0x13},
{0x16,0x1c},
{0x17,0x1a},
{0x18,0x1a},  
{0x19,0x54},  
{0x1a,0xa5},  
{0x1b,0x9a},
{0x2a,0xef},

//{0x0f,0x00},//add

#endif

#endif
#if 0
{0xfd,0x01},
{0x28,0xc4},
{0x29,0x95},
{0x0f,0x30},//add
{0x11,0x22},//13	//1a // 0x20
{0x12,0x22},//13    //1a // 0x20
{0x2e,0x10},	
{0x2f,0x10},
{0x16,0x14},//1c
{0x17,0x2a},
{0x18,0x29},	
{0x19,0x54},	
{0x1a,0x92}, //0xa9 //ab
{0x1b,0xaa},//0x90 0x97 
{0x2a,0xef},
{0x59,0x12},
#endif
 /*// awb for 前摄      
 {0xfd,0x01},    
 {0x11,0x08},    
 {0x12,0x08},    
 {0x2e,0x04},    
 {0x2f,0x04},    
 {0x16,0x1c},    
 {0x17,0x1a},    
 {0x18,0x16},    
 {0x19,0x54},    
 {0x1a,0x90},    
 {0x1b,0x9b},    
 {0x2a,0xef},    
 {0x2b,0x30},    
 {0x21,0x96},    
 {0x22,0x9a},    
*/
//AE;rpc 
#if 0
{0xfd,0x00},
{0xe0,0x3a},//
{0xe1,0x2c},//24,
{0xe2,0x26},//
{0xe3,0x22},//
{0xe4,0x22},//
{0xe5,0x20},//
{0xe6,0x20},//
{0xe8,0x20},//19,
{0xe9,0x20},//19,
{0xea,0x20},//19,
{0xeb,0x1e},//18,
{0xf5,0x1e},//18,
{0xf6,0x1e},//18,
//ae min gain  
{0xfd,0x01},
{0x94,0x60},
{0x95,0x1e},//0x18
{0x9c,0x60},
{0x9d,0x1e},//0x18  
#else
{0xfd,0x00},
{0xe0,0x4c},//
{0xe1,0x3c},//24,
{0xe2,0x34},//
{0xe3,0x2e},//
{0xe4,0x2e},//
{0xe5,0x2c},//
{0xe6,0x2c},//
{0xe8,0x2a},//19,
{0xe9,0x2a},//19,
{0xea,0x2a},//19,
{0xeb,0x28},//18,
{0xf5,0x28},//18,
{0xf6,0x28},//18,
//ae min gain  
{0xfd,0x01},
{0x94,0x60},
{0x95,0x28},//0x18
{0x9c,0x60},
{0x9d,0x28},//0x18 

#endif
//ae target
{0xfd,0x00},
{0xed,SP0A28_P0_0xf7 + 0x04},//0x84 
{0xf7,SP0A28_P0_0xf7},		//0x80 
{0xf8,SP0A28_P0_0xf8},		//0x78 
{0xec,SP0A28_P0_0xf8 - 0x04},//0x74  
{0xef,SP0A28_P0_0xf9 + 0x04},//0x84
{0xf9,SP0A28_P0_0xf9},		//0x80
{0xfa,SP0A28_P0_0xfa},		//0x78
{0xee,SP0A28_P0_0xfa - 0x04},//0x74
//gray detect
{0xfd,0x01},
{0x30,0x40},
{0x31,0x70}, // 0x10
{0x32,0x20},
{0x33,0xef},
{0x34,0x05},//0x02
{0x4d,0x40},
{0x4e,0x20},//15
{0x4f,0x13},
//saturation
{0xfd,0x00},
{0xbe,0xaa},  
{0xc0,0xff},
{0xc1,0xff},
{0xd3,SP0A28_P0_0xd3},
{0xd4,SP0A28_P0_0xd4}, 
{0xd6,SP0A28_P0_0xd6},
{0xd7,SP0A28_P0_0xd7},
{0xd8,SP0A28_P0_0xd8},
{0xd9,SP0A28_P0_0xd9}, 
{0xda,SP0A28_P0_0xda},
{0xdb,SP0A28_P0_0xdb},
//heq   
{0xfd,0x00},
{0xdc,0x00},	//;heq_offset
{0xdd,SP0A28_P0_0xdd},	//;ku
{0xde,SP0A28_P0_0xde},	//;90;kl
{0xdf,0x80},	//;heq_mean
//YCnr  
{0xfd,0x00},
{0xc2,0x08},	//Ynr_thr_outdoor
{0xc3,0x08},	//Ynr_thr_normal
{0xc4,0x08},	//Ynr_thr_dummy
{0xc5,0x10},	//Ynr_thr_lowlight
{0xc6,0x80},	//cnr_thr_outdoor
{0xc7,0x80},	//cnr_thr_normal  
{0xc8,0x80},	//cnr_thr_dummy   
{0xc9,0x80},	//cnr_thr_lowlight  
//auto lum
{0xfd,0x00},
{0xb2,0x18},
{0xb3,0x1f},
{0xb4,0x30},
{0xb5,0x45},
//func enable
{0xfd,0x00},
{0x31,0x10},
{0x32,0x0d},
{0x34,0x7e},
{0x33,0xef},
{0x35,0x00},

//add by zxl
{0xfd,0x00}, //寄存器分页设置
{0x13,0xF0},
{0x14,0x30},
{0x65,0x18},
{0x66,0x18},
{0x67,0x18},
{0x68,0x18},
{0xde,0xa0},

//{0x0e,0x01}, // close mipi func 

#endif
};
#if 0
static struct msm_camera_i2c_reg_conf sp0a28_full_settings[] = {
{0xfd,0x00},
{0x47,0x00},
{0x48,0x00},
{0x49,0x01},
{0x4a,0xe0},
{0x4b,0x00},
{0x4c,0x00},
{0x4d,0x02},
{0x4e,0x80},
};
#endif
static struct v4l2_subdev_info sp0a28_subdev_info[] = {
	{
		.code   = V4L2_MBUS_FMT_YUYV8_2X8,
		.colorspace = V4L2_COLORSPACE_JPEG,
		.fmt    = 1,
		.order    = 0,
	},
	/* more can be supported, to be added later */
};


static struct msm_camera_i2c_conf_array sp0a28_init_conf[] = {
	{&sp0a28_recommend_settings[0],
	ARRAY_SIZE(sp0a28_recommend_settings), 0, MSM_CAMERA_I2C_BYTE_DATA}
};

static struct msm_camera_i2c_conf_array sp0a28_confs[] = {
	{&sp0a28_recommend_settings[0],
	ARRAY_SIZE(sp0a28_recommend_settings), 0, MSM_CAMERA_I2C_BYTE_DATA}
};
#if 0
static struct msm_camera_i2c_conf_array sp0a28_confs[] = {
	{&sp0a28_full_settings[0],
	ARRAY_SIZE(sp0a28_full_settings), 0, MSM_CAMERA_I2C_BYTE_DATA},
};
#endif
static struct msm_camera_i2c_reg_conf sp0a28_saturation[][9] = {
	{
	//Saturation level 0
	{0xfd, 0x00}, 
	{0xd3, SP0A28_P0_0xd3 - 0x40}, 
	{0xd4, SP0A28_P0_0xd4 - 0x40}, 
	{0xd6, SP0A28_P0_0xd6 - 0x40}, 
	{0xd7, SP0A28_P0_0xd7 - 0x40},
	{0xd8, SP0A28_P0_0xd8 - 0x40}, 
	{0xd9, SP0A28_P0_0xd9 - 0x40}, 
	{0xda, SP0A28_P0_0xda - 0x40}, 
	{0xdb, SP0A28_P0_0xdb - 0x40}, 
	},/* SATURATION LEVEL0*/
	{
	//Saturation level 1
	{0xfd, 0x00}, 
	{0xd3, SP0A28_P0_0xd3}, 
	{0xd4, SP0A28_P0_0xd4 - 0x30}, 
	{0xd6, SP0A28_P0_0xd6 - 0x30},
	{0xd7, SP0A28_P0_0xd7 - 0x30}, 
	{0xd8, SP0A28_P0_0xd8 - 0x30}, 
	{0xd9, SP0A28_P0_0xd9 - 0x30}, 
	{0xda, SP0A28_P0_0xda - 0x30}, 
	{0xdb, SP0A28_P0_0xdb - 0x30}, 
	},	/* SATURATION LEVEL1*/
	{
	//Saturation level 2
	{0xfd, 0x00}, 
	{0xd3, SP0A28_P0_0xd3 - 0x20}, 
	{0xd4, SP0A28_P0_0xd4 - 0x20}, 
	{0xd6, SP0A28_P0_0xd6 - 0x20}, 
	{0xd7, SP0A28_P0_0xd7 - 0x20}, 
	{0xd8, SP0A28_P0_0xd8 - 0x20}, 
	{0xd9, SP0A28_P0_0xd9 - 0x20}, 
	{0xda, SP0A28_P0_0xda - 0x20}, 
	{0xdb, SP0A28_P0_0xdb - 0x20}, 
	},	/* SATURATION LEVEL2*/
	{
	//Saturation level 3
	{0xfd, 0x00}, 
	{0xd3, SP0A28_P0_0xd3 - 0x10},
	{0xd4, SP0A28_P0_0xd4 - 0x10},
	{0xd6, SP0A28_P0_0xd6 - 0x10}, 
	{0xd7, SP0A28_P0_0xd7 - 0x10}, 
	{0xd8, SP0A28_P0_0xd8 - 0x10}, 
	{0xd9, SP0A28_P0_0xd9 - 0x10}, 
	{0xda, SP0A28_P0_0xda - 0x10}, 
	{0xdb, SP0A28_P0_0xdb - 0x10}, 

	},	/* SATURATION LEVEL3*/
	{
	//Saturation level 4 (default)  
	{0xfd, 0x00}, 
	{0xd3, SP0A28_P0_0xd3}, 
	{0xd4, SP0A28_P0_0xd4}, 
	{0xd6, SP0A28_P0_0xd6}, 
	{0xd7, SP0A28_P0_0xd7},
	{0xd8, SP0A28_P0_0xd8}, 
	{0xd9, SP0A28_P0_0xd9},
	{0xda, SP0A28_P0_0xda}, 
	{0xdb, SP0A28_P0_0xdb}, 

	},	/* SATURATION LEVEL4*/
	{
	//Saturation level 5 
	{0xfd, 0x00},
	{0xd3, SP0A28_P0_0xd3 + 0x10}, 
	{0xd4, SP0A28_P0_0xd4 + 0x10},
	{0xd6, SP0A28_P0_0xd6 + 0x10}, 
	{0xd7, SP0A28_P0_0xd7 + 0x10}, 
	{0xd8, SP0A28_P0_0xd8 + 0x10}, 
	{0xd9, SP0A28_P0_0xd9 + 0x10}, 
	{0xda, SP0A28_P0_0xda + 0x10}, 
	{0xdb, SP0A28_P0_0xdb + 0x10}, 
	},	/* SATURATION LEVEL5*/
	{
	//Saturation level 6
	{0xfd, 0x00}, 
	{0xd3, SP0A28_P0_0xd3 + 0x20}, 
	{0xd4, SP0A28_P0_0xd4 + 0x20},
	{0xd6, SP0A28_P0_0xd6 + 0x20},
	{0xd7, SP0A28_P0_0xd7 + 0x20}, 
	{0xd8, SP0A28_P0_0xd8 + 0x20}, 
	{0xd9, SP0A28_P0_0xd9 + 0x20}, 
	{0xda, SP0A28_P0_0xda + 0x20}, 
	{0xdb, SP0A28_P0_0xdb + 0x20}, 
	},	/* SATURATION LEVEL6*/
	{
	//Saturation level 7
	{0xfd, 0x00}, 
	{0xd3, SP0A28_P0_0xd3 + 0x30},
	{0xd4, SP0A28_P0_0xd4 + 0x30},
	{0xd6, SP0A28_P0_0xd6 + 0x30}, 
	{0xd7, SP0A28_P0_0xd7 + 0x30}, 
	{0xd8, SP0A28_P0_0xd8 + 0x30}, 
	{0xd9, SP0A28_P0_0xd9 + 0x30}, 
	{0xda, SP0A28_P0_0xda + 0x30}, 
	{0xdb, SP0A28_P0_0xdb + 0x30}, 
	},	/* SATURATION LEVEL7*/
	{
	//Saturation level 8
	{0xfd, 0x00}, 
	{0xd3, SP0A28_P0_0xd3 + 0x40}, 
	{0xd4, SP0A28_P0_0xd4 + 0x40},
	{0xd6, SP0A28_P0_0xd6 + 0x40}, 
	{0xd7, SP0A28_P0_0xd7 + 0x40}, 
	{0xd8, SP0A28_P0_0xd8 + 0x40}, 
	{0xd9, SP0A28_P0_0xd9 + 0x40}, 
	{0xda, SP0A28_P0_0xda + 0x40}, 
	{0xdb, SP0A28_P0_0xdb + 0x40}, 
	},	/* SATURATION LEVEL8*/
};
static struct msm_camera_i2c_conf_array sp0a28_saturation_confs[][1] = {
	{{sp0a28_saturation[0], ARRAY_SIZE(sp0a28_saturation[0]), 0,
		MSM_CAMERA_I2C_SET_BYTE_WRITE_MASK_DATA},},
	{{sp0a28_saturation[1], ARRAY_SIZE(sp0a28_saturation[1]), 0,
		MSM_CAMERA_I2C_SET_BYTE_WRITE_MASK_DATA},},
	{{sp0a28_saturation[2], ARRAY_SIZE(sp0a28_saturation[2]), 0,
		MSM_CAMERA_I2C_SET_BYTE_WRITE_MASK_DATA},},
	{{sp0a28_saturation[3], ARRAY_SIZE(sp0a28_saturation[3]), 0,
		MSM_CAMERA_I2C_SET_BYTE_WRITE_MASK_DATA},},
	{{sp0a28_saturation[4], ARRAY_SIZE(sp0a28_saturation[4]), 0,
		MSM_CAMERA_I2C_SET_BYTE_WRITE_MASK_DATA},},
	{{sp0a28_saturation[5], ARRAY_SIZE(sp0a28_saturation[5]), 0,
		MSM_CAMERA_I2C_SET_BYTE_WRITE_MASK_DATA},},
	{{sp0a28_saturation[6], ARRAY_SIZE(sp0a28_saturation[6]), 0,
		MSM_CAMERA_I2C_SET_BYTE_WRITE_MASK_DATA},},
	{{sp0a28_saturation[7], ARRAY_SIZE(sp0a28_saturation[7]), 0,
		MSM_CAMERA_I2C_SET_BYTE_WRITE_MASK_DATA},},
	{{sp0a28_saturation[8], ARRAY_SIZE(sp0a28_saturation[8]), 0,
		MSM_CAMERA_I2C_SET_BYTE_WRITE_MASK_DATA},},
};

static int sp0a28_saturation_enum_map[] = {
	MSM_V4L2_SATURATION_L0,
	MSM_V4L2_SATURATION_L1,
	MSM_V4L2_SATURATION_L2,
	MSM_V4L2_SATURATION_L3,
	MSM_V4L2_SATURATION_L4,
	MSM_V4L2_SATURATION_L5,
	MSM_V4L2_SATURATION_L6,
	MSM_V4L2_SATURATION_L7,
	MSM_V4L2_SATURATION_L8,
};
static struct msm_sensor_output_info_t sp0a28_dimensions[] = {
	{
		.x_output = 0x280,
		.y_output = 0x1E0,
		.line_length_pclk = 0x280,//0x290,
		.frame_length_lines = 0x1E0,//0x1EC,
		.vt_pixel_clk = 24000000,
		.op_pixel_clk = 24000000,//304000000,
		.binning_factor = 1,
	},
};

static struct msm_camera_i2c_enum_conf_array sp0a28_saturation_enum_confs = {
	.conf = &sp0a28_saturation_confs[0][0],
	.conf_enum = sp0a28_saturation_enum_map,
	.num_enum = ARRAY_SIZE(sp0a28_saturation_enum_map),
	.num_index = ARRAY_SIZE(sp0a28_saturation_confs),
	.num_conf = ARRAY_SIZE(sp0a28_saturation_confs[0]),
	.data_type = MSM_CAMERA_I2C_BYTE_DATA,
};

static struct msm_camera_i2c_reg_conf sp0a28_contrast[][3] = {
	{
	//Contrast -4
	{0xfd, 0x00},
	{0xdd, SP0A28_P0_0xdd - 0x20},
	{0xde, SP0A28_P0_0xde - 0x20},
	},	/* CONTRAST L0*/
	{
	//Contrast -3
	{0xfd, 0x00},
	{0xdd, SP0A28_P0_0xdd - 0x18},
	{0xde, SP0A28_P0_0xde - 0x18},
	},	/* CONTRAST L1*/
	{
	//Contrast -2
	{0xfd, 0x00},
	{0xdd, SP0A28_P0_0xdd - 0x10},
	{0xde, SP0A28_P0_0xde - 0x10},
	},	/* CONTRAST L2*/
	{
	//Contrast -1
	{0xfd, 0x00},
	{0xdd, SP0A28_P0_0xdd - 0x08},
	{0xde, SP0A28_P0_0xde - 0x08},
	},	/* CONTRAST L3*/
	 {
	//Contrast 0
	{0xfd, 0x00},
	{0xdd, SP0A28_P0_0xdd},
	{0xde, SP0A28_P0_0xde},
	},	/* CONTRAST L4*/
	{
	//Contrast 1
	{0xfd, 0x00},
	{0xdd, SP0A28_P0_0xdd + 0x08},
	{0xde, SP0A28_P0_0xde + 0x08},
	},	/* CONTRAST L5*/
	{
	//Contrast 2
	{0xfd, 0x00},
	{0xdd, SP0A28_P0_0xdd + 0x10},
	{0xde, SP0A28_P0_0xde + 0x10},
	},	/* CONTRAST L6*/
	{
	//Contrast 3
	{0xfd, 0x00},
	{0xdd, SP0A28_P0_0xdd + 0x18},
	{0xde, SP0A28_P0_0xde + 0x18},
	},	/* CONTRAST L7*/
	{
	//Contrast 4
	{0xfd, 0x00},
	{0xdd, SP0A28_P0_0xdd + 0x20},
	{0xde, SP0A28_P0_0xde + 0x20},
	},	/* CONTRAST L8*/
};

static struct msm_camera_i2c_conf_array sp0a28_contrast_confs[][1] = {
	{{sp0a28_contrast[0], ARRAY_SIZE(sp0a28_contrast[0]), 0,
		MSM_CAMERA_I2C_SET_BYTE_WRITE_MASK_DATA},},
	{{sp0a28_contrast[1], ARRAY_SIZE(sp0a28_contrast[1]), 0,
		MSM_CAMERA_I2C_SET_BYTE_WRITE_MASK_DATA},},
	{{sp0a28_contrast[2], ARRAY_SIZE(sp0a28_contrast[2]), 0,
		MSM_CAMERA_I2C_SET_BYTE_WRITE_MASK_DATA},},
	{{sp0a28_contrast[3], ARRAY_SIZE(sp0a28_contrast[3]), 0,
		MSM_CAMERA_I2C_SET_BYTE_WRITE_MASK_DATA},},
	{{sp0a28_contrast[4], ARRAY_SIZE(sp0a28_contrast[4]), 0,
		MSM_CAMERA_I2C_SET_BYTE_WRITE_MASK_DATA},},
	{{sp0a28_contrast[5], ARRAY_SIZE(sp0a28_contrast[5]), 0,
		MSM_CAMERA_I2C_SET_BYTE_WRITE_MASK_DATA},},
	{{sp0a28_contrast[6], ARRAY_SIZE(sp0a28_contrast[6]), 0,
		MSM_CAMERA_I2C_SET_BYTE_WRITE_MASK_DATA},},
	{{sp0a28_contrast[7], ARRAY_SIZE(sp0a28_contrast[7]), 0,
		MSM_CAMERA_I2C_SET_BYTE_WRITE_MASK_DATA},},
	{{sp0a28_contrast[8], ARRAY_SIZE(sp0a28_contrast[8]), 0,
		MSM_CAMERA_I2C_SET_BYTE_WRITE_MASK_DATA},},
};


static int sp0a28_contrast_enum_map[] = {
	MSM_V4L2_CONTRAST_L0,
	MSM_V4L2_CONTRAST_L1,
	MSM_V4L2_CONTRAST_L2,
	MSM_V4L2_CONTRAST_L3,
	MSM_V4L2_CONTRAST_L4,
	MSM_V4L2_CONTRAST_L5,
	MSM_V4L2_CONTRAST_L6,
	MSM_V4L2_CONTRAST_L7,
	MSM_V4L2_CONTRAST_L8,
};

static struct msm_camera_i2c_enum_conf_array sp0a28_contrast_enum_confs = {
	.conf = &sp0a28_contrast_confs[0][0],
	.conf_enum = sp0a28_contrast_enum_map,
	.num_enum = ARRAY_SIZE(sp0a28_contrast_enum_map),
	.num_index = ARRAY_SIZE(sp0a28_contrast_confs),
	.num_conf = ARRAY_SIZE(sp0a28_contrast_confs[0]),
	.data_type = MSM_CAMERA_I2C_BYTE_DATA,
};
static struct msm_camera_i2c_reg_conf sp0a28_sharpness[][9] = {
	{
//Sharpness 0
	{0xfd, 0x01},
	{0xe8, SP0A28_P1_0xe8 - 0x08},
	{0xec, SP0A28_P1_0xec - 0x08},
	{0xe9, SP0A28_P1_0xe9 - 0x08},
	{0xed, SP0A28_P1_0xed - 0x08},
	{0xea, SP0A28_P1_0xea - 0x08},
	{0xef, SP0A28_P1_0xef - 0x08},
	{0xeb, SP0A28_P1_0xeb - 0x08},
	{0xf0, SP0A28_P1_0xf0 - 0x08},
	},    /* SHARPNESS LEVEL 0*/
	{
//Sharpness 1
	{0xfd, 0x01},
	{0xe8, SP0A28_P1_0xe8 - 0x04},
	{0xec, SP0A28_P1_0xec - 0x04},
	{0xe9, SP0A28_P1_0xe9 - 0x04},
	{0xed, SP0A28_P1_0xed - 0x04},
	{0xea, SP0A28_P1_0xea - 0x04},
	{0xef, SP0A28_P1_0xef - 0x04},
	{0xeb, SP0A28_P1_0xeb - 0x04},
	{0xf0, SP0A28_P1_0xf0 - 0x04},
	},    /* SHARPNESS LEVEL 1*/
	{
//Sharpness Auto (Default)
	{0xfd, 0x01},
	{0xe8, SP0A28_P1_0xe8},
	{0xec, SP0A28_P1_0xec},
	{0xe9, SP0A28_P1_0xe9},
	{0xed, SP0A28_P1_0xed},
	{0xea, SP0A28_P1_0xea},
	{0xef, SP0A28_P1_0xef},
	{0xeb, SP0A28_P1_0xeb},
	{0xf0, SP0A28_P1_0xf0},
	},    /* SHARPNESS LEVEL 2*/
	{
//Sharpness 3
	{0xfd, 0x01},
	{0xe8, SP0A28_P1_0xe8 + 0x08},
	{0xec, SP0A28_P1_0xec + 0x08},
	{0xe9, SP0A28_P1_0xe9 + 0x08},
	{0xed, SP0A28_P1_0xed + 0x08},
	{0xea, SP0A28_P1_0xea + 0x08},
	{0xef, SP0A28_P1_0xef + 0x08},
	{0xeb, SP0A28_P1_0xeb + 0x08},
	{0xf0, SP0A28_P1_0xf0 + 0x08},
	},    /* SHARPNESS LEVEL 3*/
	{
//Sharpness 4
	{0xfd, 0x01},
	{0xe8, SP0A28_P1_0xe8 + 0x10},
	{0xec, SP0A28_P1_0xec + 0x10},
	{0xe9, SP0A28_P1_0xe9 + 0x10},
	{0xed, SP0A28_P1_0xed + 0x10},
	{0xea, SP0A28_P1_0xea + 0x10},
	{0xef, SP0A28_P1_0xef + 0x10},
	{0xeb, SP0A28_P1_0xeb + 0x10},
	{0xf0, SP0A28_P1_0xf0 + 0x10},
	},    /* SHARPNESS LEVEL 4*/
	{
//Sharpness 5
	{0xfd, 0x01},
	{0xe8, SP0A28_P1_0xe8 + 0x18},
	{0xec, SP0A28_P1_0xec + 0x18},
	{0xe9, SP0A28_P1_0xe9 + 0x18},
	{0xed, SP0A28_P1_0xed + 0x18},
	{0xea, SP0A28_P1_0xea + 0x18},
	{0xef, SP0A28_P1_0xef + 0x18},
	{0xeb, SP0A28_P1_0xeb + 0x18},
	{0xf0, SP0A28_P1_0xf0 + 0x18},
	},    /* SHARPNESS LEVEL 5*/
};

static struct msm_camera_i2c_conf_array sp0a28_sharpness_confs[][1] = {
	{{sp0a28_sharpness[0], ARRAY_SIZE(sp0a28_sharpness[0]), 0,
		MSM_CAMERA_I2C_SET_BYTE_WRITE_MASK_DATA},},
	{{sp0a28_sharpness[1], ARRAY_SIZE(sp0a28_sharpness[1]), 0,
		MSM_CAMERA_I2C_SET_BYTE_WRITE_MASK_DATA},},
	{{sp0a28_sharpness[2], ARRAY_SIZE(sp0a28_sharpness[2]), 0,
		MSM_CAMERA_I2C_SET_BYTE_WRITE_MASK_DATA},},
	{{sp0a28_sharpness[3], ARRAY_SIZE(sp0a28_sharpness[3]), 0,
		MSM_CAMERA_I2C_SET_BYTE_WRITE_MASK_DATA},},
	{{sp0a28_sharpness[4], ARRAY_SIZE(sp0a28_sharpness[4]), 0,
		MSM_CAMERA_I2C_SET_BYTE_WRITE_MASK_DATA},},
	{{sp0a28_sharpness[5], ARRAY_SIZE(sp0a28_sharpness[5]), 0,
		MSM_CAMERA_I2C_SET_BYTE_WRITE_MASK_DATA},},
};

static int sp0a28_sharpness_enum_map[] = {
	MSM_V4L2_SHARPNESS_L0,
	MSM_V4L2_SHARPNESS_L1,
	MSM_V4L2_SHARPNESS_L2,
	MSM_V4L2_SHARPNESS_L3,
	MSM_V4L2_SHARPNESS_L4,
	MSM_V4L2_SHARPNESS_L5,
};

static struct msm_camera_i2c_enum_conf_array sp0a28_sharpness_enum_confs = {
	.conf = &sp0a28_sharpness_confs[0][0],
	.conf_enum = sp0a28_sharpness_enum_map,
	.num_enum = ARRAY_SIZE(sp0a28_sharpness_enum_map),
	.num_index = ARRAY_SIZE(sp0a28_sharpness_confs),
	.num_conf = ARRAY_SIZE(sp0a28_sharpness_confs[0]),
	.data_type = MSM_CAMERA_I2C_BYTE_DATA,
};

static struct msm_camera_i2c_reg_conf sp0a28_exposure[][2] = {
	{
	//@@ +2EV
	{0xfd,0x00},
	{0xdc,0x20},
	//{0x32,0x08},
	//{0x24,0x60},
	}, /*EXPOSURECOMPENSATIONN2*/
	{
	//@@ +1EV
	{0xfd,0x00},
	{0xdc,0x10},
	//{0x32,0x08},
	//{0x24,0x50},
	}, /*EXPOSURECOMPENSATIONN1*/
	{
	//@@ default
	{0xfd,0x00},
	{0xdc,0x00},
	//{0x32,0x08},
	//{0x24,0x40},
	}, /*EXPOSURECOMPENSATIOND*/
	{
	//@@ -1EV
	{0xfd,0x00},
	{0xdc,0xf0},	
	//{0x32,0x08},
	//{0x24,0x30},
	}, /*EXPOSURECOMPENSATIONp1*/
	{
	//@@ -2EV
	{0xfd,0x00},
	{0xdc,0xe0},		
	//{0x32,0x08},
	//{0x24,0x20},
	}, /*EXPOSURECOMPENSATIONP2*/
};

static struct msm_camera_i2c_conf_array sp0a28_exposure_confs[][1] = {
	{{sp0a28_exposure[0], ARRAY_SIZE(sp0a28_exposure[0]), 0,
		MSM_CAMERA_I2C_SET_BYTE_WRITE_MASK_DATA},},
	{{sp0a28_exposure[1], ARRAY_SIZE(sp0a28_exposure[1]), 0,
		MSM_CAMERA_I2C_SET_BYTE_WRITE_MASK_DATA},},
	{{sp0a28_exposure[2], ARRAY_SIZE(sp0a28_exposure[2]), 0,
		MSM_CAMERA_I2C_SET_BYTE_WRITE_MASK_DATA},},
	{{sp0a28_exposure[3], ARRAY_SIZE(sp0a28_exposure[3]), 0,
		MSM_CAMERA_I2C_SET_BYTE_WRITE_MASK_DATA},},
	{{sp0a28_exposure[4], ARRAY_SIZE(sp0a28_exposure[4]), 0,
		MSM_CAMERA_I2C_SET_BYTE_WRITE_MASK_DATA},},
};

static int sp0a28_exposure_enum_map[] = {
	MSM_V4L2_EXPOSURE_N2,
	MSM_V4L2_EXPOSURE_N1,
	MSM_V4L2_EXPOSURE_D,
	MSM_V4L2_EXPOSURE_P1,
	MSM_V4L2_EXPOSURE_P2,
};

static struct msm_camera_i2c_enum_conf_array sp0a28_exposure_enum_confs = {
	.conf = &sp0a28_exposure_confs[0][0],
	.conf_enum = sp0a28_exposure_enum_map,
	.num_enum = ARRAY_SIZE(sp0a28_exposure_enum_map),
	.num_index = ARRAY_SIZE(sp0a28_exposure_confs),
	.num_conf = ARRAY_SIZE(sp0a28_exposure_confs[0]),
	.data_type = MSM_CAMERA_I2C_BYTE_DATA,
};
#if 1
int sp0a28_set_iso(struct msm_sensor_ctrl_t *s_ctrl,
		struct msm_sensor_v4l2_ctrl_info_t *ctrl_info, int value)
{
	int rc = 0;
	pr_err("isp   %s   value:  %d\n", __func__,value);

	g_iso = value;

	switch (value)
    {
	    case MSM_V4L2_ISO_AUTO:
			pr_err("MSM_V4L2_ISO_AUTO");
	rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,0xfd, 0x00, MSM_CAMERA_I2C_BYTE_DATA);	
	rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,0xdc, 0x10, MSM_CAMERA_I2C_BYTE_DATA);					
		break;
		
	    case MSM_V4L2_ISO_DEBLUR:
			pr_err("MSM_V4L2_ISO_DEBLUR");
			
		break;
		
        case MSM_V4L2_ISO_100:
			pr_err("MSM_V4L2_ISO_100");
	rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,0xfd, 0x00, MSM_CAMERA_I2C_BYTE_DATA);	
	rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,0xdc, 0x10, MSM_CAMERA_I2C_BYTE_DATA);					
	
		
		case MSM_V4L2_ISO_200:
			pr_err("MSM_V4L2_ISO_200");
	rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,0xfd, 0x00, MSM_CAMERA_I2C_BYTE_DATA);	
	rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,0xdc, 0x20, MSM_CAMERA_I2C_BYTE_DATA);					
					
		break;

		case MSM_V4L2_ISO_400:
			pr_err("MSM_V4L2_ISO_400");
	rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,0xfd, 0x00, MSM_CAMERA_I2C_BYTE_DATA);	
	rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,0xdc, 0x30, MSM_CAMERA_I2C_BYTE_DATA);					
				
		break;
		
		case MSM_V4L2_ISO_800:
			pr_err("MSM_V4L2_ISO_800");
	rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,0xfd, 0x00, MSM_CAMERA_I2C_BYTE_DATA);	
	rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,0xdc, 0x40, MSM_CAMERA_I2C_BYTE_DATA);					
				
		break;
		
		case MSM_V4L2_ISO_1600:
			pr_err("MSM_V4L2_ISO_1600");
	rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,0xfd, 0x00, MSM_CAMERA_I2C_BYTE_DATA);	
	rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,0xdc, 0x50, MSM_CAMERA_I2C_BYTE_DATA);					
			
		break;			
		default:
			pr_err("invalide level !!");
	}

	return rc;
		
}
#else
static struct msm_camera_i2c_reg_conf sp0a28_iso[][3] = {
	{
	{0xfd, 0x00},
	{0xfd, 0x00},
	{0x32, 0x0d},
	},   /*ISO_AUTO*/
	{
	{0xfd, 0x00},
	{0xfd, 0x00},
	{0x32, 0x0d},
	},   /*ISO_DEBLUR*/
	{
	{0xfd, 0x00},
	{0x32, 0x08},
	{0x24, 0x20},
	},   /*ISO_100*/
	{
	{0xfd, 0x00},
	{0x32, 0x08},
	{0x24, 0x30},

	},   /*ISO_200*/
	{
	{0xfd, 0x00},
	{0x32, 0x08},
	{0x24, 0x40},

	},   /*ISO_400*/
	{
	{0xfd, 0x00},
	{0x32, 0x08},
	{0x24, 0x50},

	},   /*ISO_800*/
	{
	{0xfd, 0x00},
	{0x32, 0x08},
	{0x24, 0x60},

	},   /*ISO_1600*/
};


static struct msm_camera_i2c_conf_array sp0a28_iso_confs[][1] = {
	{{sp0a28_iso[0], ARRAY_SIZE(sp0a28_iso[0]),  0,
		MSM_CAMERA_I2C_SET_BYTE_WRITE_MASK_DATA},},
	{{sp0a28_iso[1], ARRAY_SIZE(sp0a28_iso[1]),  0,
		MSM_CAMERA_I2C_SET_BYTE_WRITE_MASK_DATA},},
	{{sp0a28_iso[2], ARRAY_SIZE(sp0a28_iso[2]),  0,
		MSM_CAMERA_I2C_SET_BYTE_WRITE_MASK_DATA},},
	{{sp0a28_iso[3], ARRAY_SIZE(sp0a28_iso[3]),  0,
		MSM_CAMERA_I2C_SET_BYTE_WRITE_MASK_DATA},},
	{{sp0a28_iso[4], ARRAY_SIZE(sp0a28_iso[4]),  0,
		MSM_CAMERA_I2C_SET_BYTE_WRITE_MASK_DATA},},
	{{sp0a28_iso[5], ARRAY_SIZE(sp0a28_iso[5]),  0,
		MSM_CAMERA_I2C_SET_BYTE_WRITE_MASK_DATA},},
	{{sp0a28_iso[6], ARRAY_SIZE(sp0a28_iso[6]),  0,
		MSM_CAMERA_I2C_SET_BYTE_WRITE_MASK_DATA},},
};

static int sp0a28_iso_enum_map[] = {
	MSM_V4L2_ISO_AUTO ,
	MSM_V4L2_ISO_DEBLUR,
	MSM_V4L2_ISO_100,
	MSM_V4L2_ISO_200,
	MSM_V4L2_ISO_400,
	MSM_V4L2_ISO_800,
	MSM_V4L2_ISO_1600,
};


static struct msm_camera_i2c_enum_conf_array sp0a28_iso_enum_confs = {
	.conf = &sp0a28_iso_confs[0][0],
	.conf_enum = sp0a28_iso_enum_map,
	.num_enum = ARRAY_SIZE(sp0a28_iso_enum_map),
	.num_index = ARRAY_SIZE(sp0a28_iso_confs),
	.num_conf = ARRAY_SIZE(sp0a28_iso_confs[0]),
	.data_type = MSM_CAMERA_I2C_BYTE_DATA,
};
#endif
static struct msm_camera_i2c_reg_conf sp0a28_no_effect[] = {
	{0xfd, 0x00},
	{0x62, 0x00},
	{0x63, 0x80},
	{0x64, 0x80},
};

static struct msm_camera_i2c_conf_array sp0a28_no_effect_confs[] = {
	{&sp0a28_no_effect[0],
	ARRAY_SIZE(sp0a28_no_effect), 0,
	MSM_CAMERA_I2C_SET_BYTE_WRITE_MASK_DATA},
};

static struct msm_camera_i2c_reg_conf sp0a28_special_effect[][5] = {
	{
	{0xfd, 0x00},
	{0x62, 0x00},
	{0x63, 0x80},
	{0x64, 0x80},
	},	/*for special effect OFF*/
	{
	{0xfd, 0x00},
	{0x62, 0x20},
	{0x63, 0x80},
	{0x64, 0x80},
	},	/*for special effect MONO*/
	{
	{0xfd, 0x00},
	{0x62, 0x04},
	{0x63, 0x80},
	{0x64, 0x80},
	},	/*for special efefct Negative*/
	{
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	},/*Solarize is not supported by sensor*/
	{
	{0xfd, 0x00},
	{0x62, 0x10},
	{0x63, 0xc0},
	{0x64, 0x20},
	},	/*for sepia*/
	{
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	},		/* Posteraize not supported */
	{
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	},		/* White board not supported*/
	{
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	},		/*Blackboard not supported*/
	{
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	},		/*Aqua not supported*/
	{
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	},		/*Emboss not supported */
	{
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	},		/*sketch not supported*/
	{
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	},		/*Neon not supported*/
	{
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	},		/*MAX value*/
};

static struct msm_camera_i2c_conf_array sp0a28_special_effect_confs[][1] = {
	{{sp0a28_special_effect[0],  ARRAY_SIZE(sp0a28_special_effect[0]),  0,
		MSM_CAMERA_I2C_SET_BYTE_WRITE_MASK_DATA},},
	{{sp0a28_special_effect[1],  ARRAY_SIZE(sp0a28_special_effect[1]),  0,
		MSM_CAMERA_I2C_SET_BYTE_WRITE_MASK_DATA},},
	{{sp0a28_special_effect[2],  ARRAY_SIZE(sp0a28_special_effect[2]),  0,
		MSM_CAMERA_I2C_SET_BYTE_WRITE_MASK_DATA},},
	{{sp0a28_special_effect[3],  ARRAY_SIZE(sp0a28_special_effect[3]),  0,
		MSM_CAMERA_I2C_SET_BYTE_WRITE_MASK_DATA},},
	{{sp0a28_special_effect[4],  ARRAY_SIZE(sp0a28_special_effect[4]),  0,
		MSM_CAMERA_I2C_SET_BYTE_WRITE_MASK_DATA},},
	{{sp0a28_special_effect[5],  ARRAY_SIZE(sp0a28_special_effect[5]),  0,
		MSM_CAMERA_I2C_SET_BYTE_WRITE_MASK_DATA},},
	{{sp0a28_special_effect[6],  ARRAY_SIZE(sp0a28_special_effect[6]),  0,
		MSM_CAMERA_I2C_SET_BYTE_WRITE_MASK_DATA},},
	{{sp0a28_special_effect[7],  ARRAY_SIZE(sp0a28_special_effect[7]),  0,
		MSM_CAMERA_I2C_SET_BYTE_WRITE_MASK_DATA},},
	{{sp0a28_special_effect[8],  ARRAY_SIZE(sp0a28_special_effect[8]),  0,
		MSM_CAMERA_I2C_SET_BYTE_WRITE_MASK_DATA},},
	{{sp0a28_special_effect[9],  ARRAY_SIZE(sp0a28_special_effect[9]),  0,
		MSM_CAMERA_I2C_SET_BYTE_WRITE_MASK_DATA},},
	{{sp0a28_special_effect[10], ARRAY_SIZE(sp0a28_special_effect[10]), 0,
		MSM_CAMERA_I2C_SET_BYTE_WRITE_MASK_DATA},},
	{{sp0a28_special_effect[11], ARRAY_SIZE(sp0a28_special_effect[11]), 0,
		MSM_CAMERA_I2C_SET_BYTE_WRITE_MASK_DATA},},
	{{sp0a28_special_effect[12], ARRAY_SIZE(sp0a28_special_effect[12]), 0,
		MSM_CAMERA_I2C_SET_BYTE_WRITE_MASK_DATA},},
};

static int sp0a28_special_effect_enum_map[] = {
	MSM_V4L2_EFFECT_OFF,
	MSM_V4L2_EFFECT_MONO,
	MSM_V4L2_EFFECT_NEGATIVE,
	MSM_V4L2_EFFECT_SOLARIZE,
	MSM_V4L2_EFFECT_SEPIA,
	MSM_V4L2_EFFECT_POSTERAIZE,
	MSM_V4L2_EFFECT_WHITEBOARD,
	MSM_V4L2_EFFECT_BLACKBOARD,
	MSM_V4L2_EFFECT_AQUA,
	MSM_V4L2_EFFECT_EMBOSS,
	MSM_V4L2_EFFECT_SKETCH,
	MSM_V4L2_EFFECT_NEON,
	MSM_V4L2_EFFECT_MAX,
};

static struct msm_camera_i2c_enum_conf_array
		 sp0a28_special_effect_enum_confs = {
	.conf = &sp0a28_special_effect_confs[0][0],
	.conf_enum = sp0a28_special_effect_enum_map,
	.num_enum = ARRAY_SIZE(sp0a28_special_effect_enum_map),
	.num_index = ARRAY_SIZE(sp0a28_special_effect_confs),
	.num_conf = ARRAY_SIZE(sp0a28_special_effect_confs[0]),
	.data_type = MSM_CAMERA_I2C_BYTE_DATA,
};

static struct msm_camera_i2c_reg_conf sp0a28_antibanding[][22] = {
	{
	//Band 16m 60Hz 8-12fps  
{0xfd,0x00},
{0x03,0x00},
{0x04,0x96},
{0x05,0x00},
{0x06,0x00},
{0x09,0x01},
{0x0a,0xd9},
{0xf0,0x32},
{0xf1,0x00},
{0xfd,0x01},
{0x90,0x0f},
{0x92,0x01},
{0x98,0x32},
{0x99,0x00},
{0x9a,0x01},
{0x9b,0x00},
//Status 
{0xfd,0x01},
{0xce,0xee},
{0xcf,0x02},
{0xd0,0xee},
{0xd1,0x02},
{0xfd,0x00},
},   /*ANTIBANDING 60HZ*/
	{
	{0xfd,0x00},
{0x03,0x00},
{0x04,0xb4},
{0x05,0x00},
{0x06,0x00},
{0x09,0x01},
{0x0a,0xd9},
{0xf0,0x3c},
{0xf1,0x00},
{0xfd,0x01},
{0x90,0x0c},
{0x92,0x01},
{0x98,0x3c},
{0x99,0x00},
{0x9a,0x01},
{0x9b,0x00},
{0xfd,0x01},
{0xce,0xd0},
{0xcf,0x02},
{0xd0,0xd0},
{0xd1,0x02},
{0xfd,0x00},
},   /*ANTIBANDING 50HZ*/
	{
	{0xfd,0x00},
{0x03,0x00},
{0x04,0xb4},
{0x05,0x00},
{0x06,0x00},
{0x09,0x01},
{0x0a,0xd9},
{0xf0,0x3c},
{0xf1,0x00},
{0xfd,0x01},
{0x90,0x0c},
{0x92,0x01},
{0x98,0x3c},
{0x99,0x00},
{0x9a,0x01},
{0x9b,0x00},
{0xfd,0x01},
{0xce,0xd0},
{0xcf,0x02},
{0xd0,0xd0},
{0xd1,0x02},
{0xfd,0x00},
},   /* ANTIBANDING AUTO*/
};


static struct msm_camera_i2c_conf_array sp0a28_antibanding_confs[][1] = {
	{{sp0a28_antibanding[0], ARRAY_SIZE(sp0a28_antibanding[0]),  0,
		MSM_CAMERA_I2C_SET_BYTE_WRITE_MASK_DATA},},
	{{sp0a28_antibanding[1], ARRAY_SIZE(sp0a28_antibanding[1]),  0,
		MSM_CAMERA_I2C_SET_BYTE_WRITE_MASK_DATA},},
	{{sp0a28_antibanding[2], ARRAY_SIZE(sp0a28_antibanding[2]),  0,
		MSM_CAMERA_I2C_SET_BYTE_WRITE_MASK_DATA},},
};

static int sp0a28_antibanding_enum_map[] = {
	MSM_V4L2_POWER_LINE_60HZ,
	MSM_V4L2_POWER_LINE_50HZ,
	MSM_V4L2_POWER_LINE_AUTO,
};


static struct msm_camera_i2c_enum_conf_array sp0a28_antibanding_enum_confs = {
	.conf = &sp0a28_antibanding_confs[0][0],
	.conf_enum = sp0a28_antibanding_enum_map,
	.num_enum = ARRAY_SIZE(sp0a28_antibanding_enum_map),
	.num_index = ARRAY_SIZE(sp0a28_antibanding_confs),
	.num_conf = ARRAY_SIZE(sp0a28_antibanding_confs[0]),
	.data_type = MSM_CAMERA_I2C_BYTE_DATA,
};

int sp0a28_set_wb(struct msm_sensor_ctrl_t *s_ctrl,
		struct msm_sensor_v4l2_ctrl_info_t *ctrl_info, int value)
{
	int rc = 0;
	pr_err("%s   value=%d\n", __func__,value);


	g_wb = value;
	switch (value)
	{	
	case MSM_V4L2_WB_AUTO:
	pr_err("MSM_V4L2_WB_AUTO");
	rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,0xfd, 0x01, MSM_CAMERA_I2C_BYTE_DATA);	
	rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,0x28, 0xc4, MSM_CAMERA_I2C_BYTE_DATA);	
	rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,0x29, 0x9e, MSM_CAMERA_I2C_BYTE_DATA);	
	rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,0xfd, 0x00, MSM_CAMERA_I2C_BYTE_DATA);	
	rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,0x32, 0x0d, MSM_CAMERA_I2C_BYTE_DATA);	
	rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,0xfd, 0x00, MSM_CAMERA_I2C_BYTE_DATA);	
	rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,0xe7, 0x03, MSM_CAMERA_I2C_BYTE_DATA);	
	rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,0xe7, 0x00, MSM_CAMERA_I2C_BYTE_DATA);	

	break;

	case MSM_V4L2_WB_CUSTOM:
	pr_err("MSM_V4L2_WB_CUSTOM");
	break;

	case MSM_V4L2_WB_INCANDESCENT:
	pr_err("MSM_V4L2_WB_INCANDESCENT");
	rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,0xfd, 0x00, MSM_CAMERA_I2C_BYTE_DATA);
	//rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,0xe7, 0x03, MSM_CAMERA_I2C_BYTE_DATA);	
	rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,0x32, 0x05, MSM_CAMERA_I2C_BYTE_DATA);
	
	//rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,0xe7, 0x00, MSM_CAMERA_I2C_BYTE_DATA);	
	rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,0xfd, 0x01, MSM_CAMERA_I2C_BYTE_DATA);	
	rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,0x28, 0x7b, MSM_CAMERA_I2C_BYTE_DATA);	
	rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,0x29, 0xd3, MSM_CAMERA_I2C_BYTE_DATA);	
	rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,0xfd, 0x00, MSM_CAMERA_I2C_BYTE_DATA);
	rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,0xe7, 0x03, MSM_CAMERA_I2C_BYTE_DATA);	
	rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,0xe7, 0x00, MSM_CAMERA_I2C_BYTE_DATA);	

	break;

	case MSM_V4L2_WB_FLUORESCENT:
	pr_err("MSM_V4L2_WB_FLUORESCENT");
	rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,0xfd, 0x00, MSM_CAMERA_I2C_BYTE_DATA);	
	//rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,0xe7, 0x03, MSM_CAMERA_I2C_BYTE_DATA);
	rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,0x32, 0x05, MSM_CAMERA_I2C_BYTE_DATA);
		
	//rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,0xe7, 0x00, MSM_CAMERA_I2C_BYTE_DATA);	

	rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,0xfd, 0x01, MSM_CAMERA_I2C_BYTE_DATA);	
	rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,0x28, 0xb4, MSM_CAMERA_I2C_BYTE_DATA);	
	rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,0x29, 0xc4, MSM_CAMERA_I2C_BYTE_DATA);	
	rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,0xfd, 0x00, MSM_CAMERA_I2C_BYTE_DATA);
	rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,0xe7, 0x03, MSM_CAMERA_I2C_BYTE_DATA);	
	rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,0xe7, 0x00, MSM_CAMERA_I2C_BYTE_DATA);	

	break;

	case MSM_V4L2_WB_DAYLIGHT:
	pr_err("MSM_V4L2_WB_DAYLIGHT");
	rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,0xfd, 0x00, MSM_CAMERA_I2C_BYTE_DATA);
	//rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,0xe7, 0x03, MSM_CAMERA_I2C_BYTE_DATA);	
	rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,0x32, 0x05, MSM_CAMERA_I2C_BYTE_DATA);

	//rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,0xe7, 0x00, MSM_CAMERA_I2C_BYTE_DATA);	

	rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,0xfd, 0x01, MSM_CAMERA_I2C_BYTE_DATA);	
	rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,0x28, 0xc1, MSM_CAMERA_I2C_BYTE_DATA);	
	rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,0x29, 0x88, MSM_CAMERA_I2C_BYTE_DATA);	
	rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,0xfd, 0x00, MSM_CAMERA_I2C_BYTE_DATA);
	rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,0xe7, 0x03, MSM_CAMERA_I2C_BYTE_DATA);	
	rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,0xe7, 0x00, MSM_CAMERA_I2C_BYTE_DATA);	

	break;

	case MSM_V4L2_WB_CLOUDY_DAYLIGHT:
	pr_err("MSM_V4L2_WB_CLOUDY_DAYLIGHT");
	rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,0xfd, 0x00, MSM_CAMERA_I2C_BYTE_DATA);
	//rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,0xe7, 0x03, MSM_CAMERA_I2C_BYTE_DATA);	
	rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,0x32, 0x05, MSM_CAMERA_I2C_BYTE_DATA);
	
	//rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,0xe7, 0x00, MSM_CAMERA_I2C_BYTE_DATA);	

	rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,0xfd, 0x01, MSM_CAMERA_I2C_BYTE_DATA);	
	rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,0x28, 0xe2, MSM_CAMERA_I2C_BYTE_DATA);	
	rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,0x29, 0x82, MSM_CAMERA_I2C_BYTE_DATA);	
	rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,0xfd, 0x00, MSM_CAMERA_I2C_BYTE_DATA);
	rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,0xe7, 0x03, MSM_CAMERA_I2C_BYTE_DATA);	
	rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,0xe7, 0x00, MSM_CAMERA_I2C_BYTE_DATA);	

	break;

	case MSM_V4L2_WB_OFF:
	pr_err("MSM_V4L2_WB_OFF");
	break;
	
	
	default:
	pr_err("invalide level !!");
	}
      //msleep(100);
	return rc;	
}
#if 0
static struct msm_camera_i2c_reg_conf sp0a28_wb_oem[][6] = {
	{
	//{0xfd, 0x00},
	//{0x32, 0x05},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	},/*WHITEBALNACE OFF*/
	{
	{0xfd, 0x01},
	{0x28, 0xc4},
	{0x29, 0x9e},
	{0xfd, 0x00},
	{0x32, 0x0d},
	{0xfd, 0x00},
	}, /*WHITEBALNACE AUTO*/
	{
	//{0xfd, 0x00},
	//{0x32, 0x0d},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	},	/*WHITEBALNACE CUSTOM*/
	{
	{0xfd, 0x00},
	{0x32, 0x05},
	{0xfd, 0x01},
	{0x28, 0x7b},
	{0x29, 0xd3},
	{0xfd, 0x00},
	},	/*INCANDISCENT*/
	{
	{0xfd, 0x00},
	{0x32, 0x05},
	{0xfd, 0x01},
	{0x28, 0xb4},
	{0x29, 0xc4},
	{0xfd, 0x00},
	},	/*FLOURESECT NOT SUPPORTED */
	{
	{0xfd, 0x00},
	{0x32, 0x05},
	{0xfd, 0x01},
	{0x28, 0xc1},
	{0x29, 0x88},
	{0xfd, 0x00},
	},	/*DAYLIGHT*/
	{
	{0xfd, 0x00},
	{0x32, 0x05},
	{0xfd, 0x01},
	{0x28, 0xe2},
	{0x29, 0x82},
	{0xfd, 0x00},
},	/*CLOUDY*/
};

static struct msm_camera_i2c_conf_array sp0a28_wb_oem_confs[][1] = {
	{{sp0a28_wb_oem[0], ARRAY_SIZE(sp0a28_wb_oem[0]),  0,
		MSM_CAMERA_I2C_SET_BYTE_WRITE_MASK_DATA},},
	{{sp0a28_wb_oem[1], ARRAY_SIZE(sp0a28_wb_oem[1]),  0,
		MSM_CAMERA_I2C_SET_BYTE_WRITE_MASK_DATA},},
	{{sp0a28_wb_oem[2], ARRAY_SIZE(sp0a28_wb_oem[2]),  0,
		MSM_CAMERA_I2C_SET_BYTE_WRITE_MASK_DATA},},
	{{sp0a28_wb_oem[3], ARRAY_SIZE(sp0a28_wb_oem[3]),  0,
		MSM_CAMERA_I2C_SET_BYTE_WRITE_MASK_DATA},},
	{{sp0a28_wb_oem[4], ARRAY_SIZE(sp0a28_wb_oem[4]),  0,
		MSM_CAMERA_I2C_SET_BYTE_WRITE_MASK_DATA},},
	{{sp0a28_wb_oem[5], ARRAY_SIZE(sp0a28_wb_oem[5]),  0,
		MSM_CAMERA_I2C_SET_BYTE_WRITE_MASK_DATA},},
	{{sp0a28_wb_oem[6], ARRAY_SIZE(sp0a28_wb_oem[6]),  0,
		MSM_CAMERA_I2C_SET_BYTE_WRITE_MASK_DATA},},
};

static int sp0a28_wb_oem_enum_map[] = {
	MSM_V4L2_WB_OFF,
	MSM_V4L2_WB_AUTO ,
	MSM_V4L2_WB_CUSTOM,
	MSM_V4L2_WB_INCANDESCENT,
	MSM_V4L2_WB_FLUORESCENT,
	MSM_V4L2_WB_DAYLIGHT,
	MSM_V4L2_WB_CLOUDY_DAYLIGHT,
};

static struct msm_camera_i2c_enum_conf_array sp0a28_wb_oem_enum_confs = {
	.conf = &sp0a28_wb_oem_confs[0][0],
	.conf_enum = sp0a28_wb_oem_enum_map,
	.num_enum = ARRAY_SIZE(sp0a28_wb_oem_enum_map),
	.num_index = ARRAY_SIZE(sp0a28_wb_oem_confs),
	.num_conf = ARRAY_SIZE(sp0a28_wb_oem_confs[0]),
	.data_type = MSM_CAMERA_I2C_BYTE_DATA,
};
#endif

int sp0a28_saturation_msm_sensor_s_ctrl_by_enum(
		struct msm_sensor_ctrl_t *s_ctrl,
		struct msm_sensor_v4l2_ctrl_info_t *ctrl_info, int value)
{
	int rc = 0;
	if (effect_value == CAMERA_EFFECT_OFF) {
		rc = msm_sensor_write_enum_conf_array(
			s_ctrl->sensor_i2c_client,
			ctrl_info->enum_cfg_settings, value);
	}
	if (value <= MSM_V4L2_SATURATION_L8)
		SAT_U = SAT_V = value * 0x10;
	CDBG("--CAMERA-- %s ...(End)\n", __func__);
	return rc;
}


int sp0a28_contrast_msm_sensor_s_ctrl_by_enum(
		struct msm_sensor_ctrl_t *s_ctrl,
		struct msm_sensor_v4l2_ctrl_info_t *ctrl_info, int value)
{
	int rc = 0;
	if (effect_value == CAMERA_EFFECT_OFF) {
		rc = msm_sensor_write_enum_conf_array(
			s_ctrl->sensor_i2c_client,
			ctrl_info->enum_cfg_settings, value);
	}
	return rc;
}

int sp0a28_sharpness_msm_sensor_s_ctrl_by_enum(
		struct msm_sensor_ctrl_t *s_ctrl,
		struct msm_sensor_v4l2_ctrl_info_t *ctrl_info, int value)
{
	int rc = 0;
	if (effect_value == CAMERA_EFFECT_OFF) {
		rc = msm_sensor_write_enum_conf_array(
			s_ctrl->sensor_i2c_client,
			ctrl_info->enum_cfg_settings, value);
	}
	return rc;
}

int sp0a28_effect_msm_sensor_s_ctrl_by_enum(struct msm_sensor_ctrl_t *s_ctrl,
		struct msm_sensor_v4l2_ctrl_info_t *ctrl_info, int value)
{
	int rc = 0;
	effect_value = value;
	if (effect_value == CAMERA_EFFECT_OFF) {
		rc = msm_sensor_write_conf_array(
			s_ctrl->sensor_i2c_client,
			s_ctrl->msm_sensor_reg->no_effect_settings, 0);
		if (rc < 0) {
			CDBG("write faield\n");
			return rc;
		}

	/*	msm_camera_i2c_write(s_ctrl->sensor_i2c_client, 0xda, SAT_U,
			MSM_CAMERA_I2C_BYTE_DATA);
		msm_camera_i2c_write(s_ctrl->sensor_i2c_client, 0xdb, SAT_V,
			MSM_CAMERA_I2C_BYTE_DATA);
			*/
	} else {
		rc = msm_sensor_write_enum_conf_array(
			s_ctrl->sensor_i2c_client,
			ctrl_info->enum_cfg_settings, value);
	}
	return rc;
}

int sp0a28_antibanding_msm_sensor_s_ctrl_by_enum(
		struct msm_sensor_ctrl_t *s_ctrl,
		struct msm_sensor_v4l2_ctrl_info_t *ctrl_info, int value)
{
	int rc = 0;
		return rc;
}

int sp0a28_msm_sensor_s_ctrl_by_enum(struct msm_sensor_ctrl_t *s_ctrl,
		struct msm_sensor_v4l2_ctrl_info_t *ctrl_info, int value)
{
	int rc = 0;
	rc = msm_sensor_write_enum_conf_array(
		s_ctrl->sensor_i2c_client,
		ctrl_info->enum_cfg_settings, value);
	if (rc < 0) {
		CDBG("write faield\n");
		return rc;
	}
	return rc;
}

struct msm_sensor_v4l2_ctrl_info_t sp0a28_v4l2_ctrl_info[] = {
	{
		.ctrl_id = V4L2_CID_SATURATION,
		.min = MSM_V4L2_SATURATION_L0,
		.max = MSM_V4L2_SATURATION_L8,
		.step = 1,
		.enum_cfg_settings = &sp0a28_saturation_enum_confs,
		.s_v4l2_ctrl = sp0a28_saturation_msm_sensor_s_ctrl_by_enum,
	},
	{
		.ctrl_id = V4L2_CID_CONTRAST,
		.min = MSM_V4L2_CONTRAST_L0,
		.max = MSM_V4L2_CONTRAST_L8,
		.step = 1,
		.enum_cfg_settings = &sp0a28_contrast_enum_confs,
		.s_v4l2_ctrl = sp0a28_contrast_msm_sensor_s_ctrl_by_enum,
	},
	{
		.ctrl_id = V4L2_CID_SHARPNESS,
		.min = MSM_V4L2_SHARPNESS_L0,
		.max = MSM_V4L2_SHARPNESS_L5,
		.step = 1,
		.enum_cfg_settings = &sp0a28_sharpness_enum_confs,
		.s_v4l2_ctrl = sp0a28_sharpness_msm_sensor_s_ctrl_by_enum,
	},
	{
		.ctrl_id = V4L2_CID_EXPOSURE,
		.min = MSM_V4L2_EXPOSURE_N2,
		.max = MSM_V4L2_EXPOSURE_P2,
		.step = 1,
		.enum_cfg_settings = &sp0a28_exposure_enum_confs,
		.s_v4l2_ctrl = sp0a28_msm_sensor_s_ctrl_by_enum,
	},
	{
		.ctrl_id = V4L2_CID_ISO,
		.min = MSM_V4L2_ISO_AUTO,
		.max = MSM_V4L2_ISO_1600,
		.step = 1,
#if 1
		//.enum_cfg_settings = &sp0a28_iso_enum_confs,
		.s_v4l2_ctrl = sp0a28_set_iso,
#else
	.enum_cfg_settings = &sp0a28_iso_enum_confs,
		.s_v4l2_ctrl = sp0a28_msm_sensor_s_ctrl_by_enum,
#endif
	},
	{
		.ctrl_id = V4L2_CID_SPECIAL_EFFECT,
		.min = MSM_V4L2_EFFECT_OFF,
		.max = MSM_V4L2_EFFECT_NEGATIVE,
		.step = 1,
		.enum_cfg_settings = &sp0a28_special_effect_enum_confs,
		.s_v4l2_ctrl = sp0a28_effect_msm_sensor_s_ctrl_by_enum,
	},
	{
		.ctrl_id = V4L2_CID_POWER_LINE_FREQUENCY,
		.min = MSM_V4L2_POWER_LINE_60HZ,
		.max = MSM_V4L2_POWER_LINE_AUTO,
		.step = 1,
		.enum_cfg_settings = &sp0a28_antibanding_enum_confs,
		.s_v4l2_ctrl = sp0a28_antibanding_msm_sensor_s_ctrl_by_enum,
	},
	{
		.ctrl_id = V4L2_CID_WHITE_BALANCE_TEMPERATURE,
		.min = MSM_V4L2_WB_OFF,
		.max = MSM_V4L2_WB_CLOUDY_DAYLIGHT,
		.step = 1,
#if 1
		//.enum_cfg_settings = &sp0a28_wb_oem_enum_confs,
		.s_v4l2_ctrl = sp0a28_set_wb,
#else
	.enum_cfg_settings = &sp0a28_wb_oem_enum_confs,
		.s_v4l2_ctrl = sp0a28_msm_sensor_s_ctrl_by_enum,
#endif
	},

};

static struct msm_sensor_output_reg_addr_t sp0a28_reg_addr = {
	.x_output = 0xCC,
	.y_output = 0xCE,
	.line_length_pclk = 0xC8,
	.frame_length_lines = 0xCA,
};

static struct msm_sensor_id_info_t sp0a28_id_info = {
	.sensor_id_reg_addr = 0x02,
	.sensor_id = 0xa2,
};

static const struct i2c_device_id sp0a28_i2c_id[] = {
	{SENSOR_NAME, (kernel_ulong_t)&sp0a28_s_ctrl},
	{ }
};


static struct i2c_driver sp0a28_i2c_driver = {
	.id_table = sp0a28_i2c_id,
	.probe  = msm_sp0a28_sensor_i2c_probe,
	.driver = {
		.name = SENSOR_NAME,
	},
};

static struct msm_camera_i2c_client sp0a28_sensor_i2c_client = {
	.addr_type = MSM_CAMERA_I2C_BYTE_ADDR,
};

static int __init msm_sensor_init_module(void)
{
	int rc = 0;
	CDBG("SP0A28\n");

	rc = i2c_add_driver(&sp0a28_i2c_driver);

	return rc;
}

static struct v4l2_subdev_core_ops sp0a28_subdev_core_ops = {
	.s_ctrl = msm_sensor_v4l2_s_ctrl,
	.queryctrl = msm_sensor_v4l2_query_ctrl,
	.ioctl = msm_sensor_subdev_ioctl,
	.s_power = msm_sensor_power,
};

static struct v4l2_subdev_video_ops sp0a28_subdev_video_ops = {
	.enum_mbus_fmt = msm_sensor_v4l2_enum_fmt,
};

static struct v4l2_subdev_ops sp0a28_subdev_ops = {
	.core = &sp0a28_subdev_core_ops,
	.video  = &sp0a28_subdev_video_ops,
};

static struct msm_cam_clk_info cam_8960_clk_info[] = {
	{"cam_clk", MSM_SENSOR_MCLK_16HZ},
};
 
int32_t msm_sensor_sp0a28_write_res_settings(struct msm_sensor_ctrl_t *s_ctrl,
	uint16_t res)
{
	int32_t rc;
		pr_err("%s: E\n", __func__);	
	rc = msm_sensor_write_conf_array(
		s_ctrl->sensor_i2c_client,
		s_ctrl->msm_sensor_reg->mode_settings, res);
	return rc;
}
int msm_camera_sp0a28_power_on(struct msm_sensor_ctrl_t *s_ctrl)
{
	int32_t rc = 0;
	struct msm_camera_sensor_info *data = s_ctrl->sensordata;
	struct device *dev = NULL;	
	pr_err("jia: %s  %d \n",__func__,__LINE__);
	if (s_ctrl->sensor_device_type == MSM_SENSOR_PLATFORM_DEVICE)
		dev = &s_ctrl->pdev->dev;
	else
		dev = &s_ctrl->sensor_i2c_client->client->dev;
#if 0
	pr_err("jia: %s  %d \n",__func__,__LINE__);

	rc = msm_camera_config_vreg(dev,
		s_ctrl->sensordata->sensor_platform_info->cam_vreg,
		s_ctrl->sensordata->sensor_platform_info->num_vreg,
		s_ctrl->vreg_seq,
		s_ctrl->num_vreg_seq,
		s_ctrl->reg_ptr, 1);
	if (rc < 0) {
		pr_err("%s: regulator on failed\n", __func__);
		//goto config_vreg_failed;
	}

	rc = msm_camera_enable_vreg(dev,
		s_ctrl->sensordata->sensor_platform_info->cam_vreg,
		s_ctrl->sensordata->sensor_platform_info->num_vreg,
		s_ctrl->vreg_seq,
		s_ctrl->num_vreg_seq,
		s_ctrl->reg_ptr, 1);
	if (rc < 0) {
		pr_err("%s: enable regulator failed\n", __func__);
		//goto enable_vreg_failed;
	}
#endif
	gpio_request(55,"sp0a28");
	rc=gpio_direction_output(55, 1);
	if (rc < 0) {
		pr_err("%s: enable avdd_en failed\n", __func__);
	}else{
	pr_err("%s  avdd_en=1\n",__func__);	
		}
		
	gpio_request(43,"sp0a28");	
	rc=	gpio_direction_output(43, 1);
	if (rc < 0) {
		pr_err("%s: enable iovdd_en failed\n", __func__);
	}else{		
	pr_err("%s  iovdd_en=1\n",__func__);	
		}
	
	gpio_request(9,"sp0a28");	
	rc=gpio_direction_output(9, 1);	
	if (rc < 0) {
		pr_err("%s: enable dvdd_ldo_en failed\n", __func__);
	}else{	
	pr_err("%s  dvdd_ldo=1\n",__func__);	
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
		pr_err("%s: EXIT\n", __func__);	
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
request_gpio_failed:
	kfree(s_ctrl->reg_ptr);
			pr_err("%s X\n", __func__);	
	return rc;
}

int32_t msm_sensor_sp0a28_power_up(struct msm_sensor_ctrl_t *s_ctrl)
{

	int32_t rc = 0;
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
	gpio_request(53,"sp0a28");	
	rc=gpio_direction_output(53, 0);
	msleep(10);	
	if (rc < 0) {
		pr_err("%s: disable PWD failed\n", __func__);
	}else{	
	pr_err("%s gpio53=0 exit PWD\n",__func__);
		}
	gpio_direction_output(53, 1);
	pr_err("%s gpio53=1 entry PWD\n",__func__);	
	msleep(10);
	gpio_direction_output(53, 0);
	pr_err("%s gpio53=0 exit PWD\n",__func__);	
	
	return rc;
	msleep(10);

enable_clk_failed:
		msm_camera_config_gpio_table(data, 0);
			pr_err("%s X\n", __func__);	
	return rc;
}

int32_t msm_sensor_sp0a28_power_down(struct msm_sensor_ctrl_t *s_ctrl)
{
	int32_t rc = 0;
	struct device *dev = NULL;	
	pr_err("%s\n", __func__);
	#if 0
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
       #endif
	gpio_direction_output(53, 1);	
	pr_err("%s  gpio53 PWD output=0\n",__func__);	

	msm_cam_clk_enable(dev, cam_8960_clk_info, s_ctrl->cam_clk,
		ARRAY_SIZE(cam_8960_clk_info), 0);
	pr_err("%s  X\n",__func__);		
	return rc;
}

int32_t msm_sp0a28_sensor_match_id(struct msm_sensor_ctrl_t *s_ctrl)
{
	int32_t rc = 0;
	uint16_t chipid = 0;
	pr_err("%s %d\n",__func__,__LINE__);
	rc = msm_camera_i2c_read(
			s_ctrl->sensor_i2c_client,
			s_ctrl->sensor_id_info->sensor_id_reg_addr, &chipid,
			MSM_CAMERA_I2C_BYTE_DATA);
	if (rc < 0) {
		pr_err("%s: %s: read id failed line=%d\n", __func__,
			s_ctrl->sensordata->sensor_name,__LINE__);
		return rc;
	}

	pr_err("%s: read id: %x expected id %x:\n", __func__, chipid,
		s_ctrl->sensor_id_info->sensor_id);
	if (chipid != s_ctrl->sensor_id_info->sensor_id) {
		pr_err("msm_sensor_match_id chip id doesnot match\n");
		return -ENODEV;
	}
	return rc;
}

int32_t msm_sp0a28_sensor_i2c_probe(struct i2c_client *client,
	const struct i2c_device_id *id)
{
	int rc = 0;
	struct msm_sensor_ctrl_t *s_ctrl;
 	
	printk("lijing %s %s_i2c_probe called\n", __func__, client->name);
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
		pr_err("%s %d \n", __func__, __LINE__);		
	s_ctrl->reg_ptr = kzalloc(sizeof(struct regulator *)
			* s_ctrl->sensordata->sensor_platform_info->num_vreg, GFP_KERNEL);
	if (!s_ctrl->reg_ptr) {
		pr_err("%s: could not allocate mem for regulators\n",
			__func__);
		return -ENOMEM;
	}
		pr_err("%s %d \n", __func__, __LINE__);	
	msm_camera_sp0a28_power_on(s_ctrl);
	if (rc < 0) {
	pr_err("%s %s power on failed\n", __func__, client->name);
	return rc;
	}

	rc = msm_sp0a28_sensor_match_id(s_ctrl);
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
    #if 0
	rc=msm_sensor_sp0a28_write_res_settings(s_ctrl, 0);
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

	return rc;
}

static void sp0a28_start_stream(struct msm_sensor_ctrl_t *s_ctrl) {
	int rc=0;
	pr_err("%s E LINE=%d\n", __func__, __LINE__);	

	    if(SP0A28_INIT_TMP == 1)
	    {
		rc=msm_sensor_sp0a28_write_res_settings(s_ctrl, 0);
		if (rc < 0) {
		pr_err("%s: msm_sensor_write_res_settings failed\n", __func__);
		   }
		sp0a28_set_wb(s_ctrl,NULL, g_wb);
		msleep(50);
		sp0a28_set_iso(s_ctrl,NULL, g_iso);
		 SP0A28_INIT_TMP=0;
	    }

		pr_err("%s X LINE=%d\n", __func__, __LINE__);	

	};

static void sp0a28_stop_stream(struct msm_sensor_ctrl_t *s_ctrl) {
	#if 0
	int rc=0;
		rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client, 0xfd, 0x00, MSM_CAMERA_I2C_BYTE_DATA);
		
		rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client, 0x0e, 0x01, MSM_CAMERA_I2C_BYTE_DATA); // 0x01
		if (rc < 0) 
	{
		pr_err("%s: i2c write failed\n", __func__);		
		}
		msleep(10);
	#endif
};

int32_t msm_sp0a28_sensor_setting(struct msm_sensor_ctrl_t *s_ctrl,
			int update_type, int res)
{
	int32_t rc = 0;
	pr_err("%s E LINE=%d,res=%d\n", __func__, __LINE__,res);	

	if (update_type == MSM_SENSOR_REG_INIT) {
#if 0
		rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client, 0xfd, 0x00, MSM_CAMERA_I2C_BYTE_DATA);
		
		rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client, 0x0e, 0x01, MSM_CAMERA_I2C_BYTE_DATA); // 0x01
		if (rc < 0) 
	     {
		pr_err("%s: i2c write failed\n", __func__);		
		return rc;
		}
		msleep(10);	
#endif
	} 
	else if (update_type == MSM_SENSOR_UPDATE_PERIODIC) {
		pr_err("%s: UPDATE_PERIODIC,res=%d\n", __func__,res);
		#if 0
		rc=msm_sensor_sp0a28_write_res_settings(s_ctrl, res);
		if (rc < 0) {
		pr_err("%s: msm_sensor_write_res_settings failed\n", __func__);
		return rc;
		}
		#else
		SP0A28_INIT_TMP = 1 ;
		#endif //lijing
		v4l2_subdev_notify(&s_ctrl->sensor_v4l2_subdev,
		NOTIFY_PCLK_CHANGE, &s_ctrl->msm_sensor_reg->
		output_settings[res].op_pixel_clk);
	}	
	pr_err("%s X line=%d  res=%d\n",__func__,__LINE__,res);			
	return rc;
	}
#if 0
static enum msm_camera_vreg_name_t sp0a28_veg_seq[] = {
	//CAM_VIO,
	CAM_VDIG,
	//CAM_VANA,
};	
#endif
static struct msm_sensor_fn_t sp0a28_func_tbl = {
	.sensor_match_id = msm_sp0a28_sensor_match_id,
	.sensor_start_stream = sp0a28_start_stream,
	.sensor_stop_stream = sp0a28_stop_stream,
	.sensor_setting = msm_sp0a28_sensor_setting,
	.sensor_set_sensor_mode = msm_sensor_set_sensor_mode,
	.sensor_mode_init = msm_sensor_mode_init,
	.sensor_get_output_info = msm_sensor_get_output_info,
	.sensor_config = msm_sensor_config,
	.sensor_power_up = msm_sensor_sp0a28_power_up,
	.sensor_power_down = msm_sensor_sp0a28_power_down,
	.sensor_get_csi_params = msm_sensor_get_csi_params,

};

static struct msm_sensor_reg_t sp0a28_regs = {
	.default_data_type = MSM_CAMERA_I2C_BYTE_DATA,
	.start_stream_conf = sp0a28_start_settings,
	.start_stream_conf_size = ARRAY_SIZE(sp0a28_start_settings),
	.stop_stream_conf = sp0a28_stop_settings,
	.stop_stream_conf_size = ARRAY_SIZE(sp0a28_stop_settings),
	.init_settings = &sp0a28_init_conf[0],
	.init_size = ARRAY_SIZE(sp0a28_init_conf),
	.mode_settings = &sp0a28_confs[0],
	.no_effect_settings = &sp0a28_no_effect_confs[0],
	.output_settings = &sp0a28_dimensions[0],
	.num_conf = ARRAY_SIZE(sp0a28_confs),
};



static struct msm_sensor_ctrl_t sp0a28_s_ctrl = {
	.msm_sensor_reg = &sp0a28_regs,
	.msm_sensor_v4l2_ctrl_info = sp0a28_v4l2_ctrl_info,
	.num_v4l2_ctrl = ARRAY_SIZE(sp0a28_v4l2_ctrl_info),
	.sensor_i2c_client = &sp0a28_sensor_i2c_client,
	.sensor_i2c_addr = 0x7A,
	.sensor_output_reg_addr = &sp0a28_reg_addr,
	.sensor_id_info = &sp0a28_id_info,
	.cam_mode = MSM_SENSOR_MODE_INVALID,
	.min_delay = 30,
	.power_seq_delay = 60,	
	.msm_sensor_mutex = &sp0a28_mut,
	.sensor_i2c_driver = &sp0a28_i2c_driver,
	.sensor_v4l2_subdev_info = sp0a28_subdev_info,
	.sensor_v4l2_subdev_info_size = ARRAY_SIZE(sp0a28_subdev_info),
	.sensor_v4l2_subdev_ops = &sp0a28_subdev_ops,
	.func_tbl = &sp0a28_func_tbl,
	.msm_sensor_reg_default_data_type=MSM_CAMERA_I2C_BYTE_DATA,
	//.clk_rate = MSM_SENSOR_MCLK_16HZ,
};

module_init(msm_sensor_init_module);
MODULE_DESCRIPTION("Omnivision VGA YUV sensor driver");
MODULE_LICENSE("GPL v2");
