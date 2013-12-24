/* Copyright (c) 2011-2012, The Linux Foundation. All rights reserved.
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

#include <asm/mach-types.h>
#include <linux/gpio.h>
#include <mach/camera.h>
#include <mach/msm_bus_board.h>
#include <mach/gpiomux.h>
#include "devices.h"
#include "board-8930.h"

#ifdef CONFIG_MSM_CAMERA_V4L2

#if (defined(CONFIG_GPIO_SX150X) || defined(CONFIG_GPIO_SX150X_MODULE)) && \
	defined(CONFIG_I2C)
#if 0 //remove for  camera no use . wt
static struct i2c_board_info cam_expander_i2c_info[] = {
	{
		I2C_BOARD_INFO("sx1508q", 0x22),
		.platform_data = &msm8930_sx150x_data[SX150X_CAM]
	},
};

static struct msm_cam_expander_info cam_expander_info[] = {
	{
		cam_expander_i2c_info,
		MSM_8930_GSBI4_QUP_I2C_BUS_ID,
	},
};
#endif
#endif
#ifdef CONFIG_MSM_CAMERA_V4L2
static struct gpiomux_setting out_high_np_active__cfg1 = {//53/54/76/107
	.func = GPIOMUX_FUNC_GPIO,
	.drv = GPIOMUX_DRV_2MA,
	.dir = GPIOMUX_OUT_HIGH,
	.pull = GPIOMUX_PULL_NONE,
};
static struct gpiomux_setting out_high_np_active__cfg2 = {//9/15/55
	.func = GPIOMUX_FUNC_GPIO,
	.drv = GPIOMUX_DRV_2MA,
	.dir = GPIOMUX_OUT_HIGH,
	.pull = GPIOMUX_PULL_NONE,
};
static struct gpiomux_setting out_high_np_suspend_cfg1 = {//9/15/55
	.func = GPIOMUX_FUNC_GPIO,
	.drv = GPIOMUX_DRV_2MA,
	.dir = GPIOMUX_OUT_HIGH,
	.pull = GPIOMUX_PULL_DOWN,
};
static struct gpiomux_setting out_high_np_suspend_cfg2 = { //53/54/76/107
	.func = GPIOMUX_FUNC_GPIO,
	.drv = GPIOMUX_DRV_2MA,
	.dir = GPIOMUX_OUT_HIGH,
	.pull = GPIOMUX_PULL_NONE,	
};
#if defined(CONFIG_OV8825)|| defined(CONFIG_HI542)|| defined(CONFIG_AR0542)|| defined(CONFIG_OV8835)
static struct gpiomux_setting out_high_np_suspend_cfg3 = {//54//107 hi542 standby
	.func = GPIOMUX_FUNC_GPIO,
	.drv = GPIOMUX_DRV_2MA,
	.dir = GPIOMUX_OUT_LOW,
	.pull = GPIOMUX_PULL_NONE,	
};
#endif
#endif
static struct gpiomux_setting cam_settings[] = {
	{
		.func = GPIOMUX_FUNC_GPIO, /*suspend*/
		.drv = GPIOMUX_DRV_2MA,		
		.pull = GPIOMUX_PULL_DOWN,
	},

	{
		.func = GPIOMUX_FUNC_1, /*active 1*/
		.drv = GPIOMUX_DRV_2MA,
		.pull = GPIOMUX_PULL_NONE,
	},

	{
		.func = GPIOMUX_FUNC_GPIO, /*active 2*/
		.drv = GPIOMUX_DRV_2MA,
		.pull = GPIOMUX_PULL_NONE,
	},

	{
		.func = GPIOMUX_FUNC_1, /*active 3*/
		.drv = GPIOMUX_DRV_8MA,
		.pull = GPIOMUX_PULL_NONE,
	},

	{
		.func = GPIOMUX_FUNC_5, /*active 4*/
		.drv = GPIOMUX_DRV_8MA,
		.pull = GPIOMUX_PULL_UP,
	},

	{
		.func = GPIOMUX_FUNC_6, /*active 5*/
		.drv = GPIOMUX_DRV_8MA,
		.pull = GPIOMUX_PULL_UP,
	},

	{
		.func = GPIOMUX_FUNC_2, /*active 6*/
		.drv = GPIOMUX_DRV_2MA,
		.pull = GPIOMUX_PULL_UP,
	},

	{
		.func = GPIOMUX_FUNC_3, /*active 7*/
		.drv = GPIOMUX_DRV_8MA,
		.pull = GPIOMUX_PULL_UP,
	},

	{
		.func = GPIOMUX_FUNC_GPIO, /*i2c suspend*/
		.drv = GPIOMUX_DRV_2MA,
		.pull = GPIOMUX_PULL_KEEPER,
	},
	{
		.func = GPIOMUX_FUNC_2, /*active 9*/
		.drv = GPIOMUX_DRV_2MA,
		.pull = GPIOMUX_PULL_NONE,
	},

};
#ifdef CONFIG_ADP1650
static struct gpiomux_setting cam_active = {
	.func = GPIOMUX_FUNC_GPIO,
	.drv = GPIOMUX_DRV_2MA,
	.pull = GPIOMUX_PULL_NONE,
	.dir = GPIOMUX_OUT_HIGH,
};
static struct gpiomux_setting cam_suspended_config = {
	.func = GPIOMUX_FUNC_GPIO,
	.drv = GPIOMUX_DRV_2MA,
	.pull = GPIOMUX_PULL_NONE,
	.dir = GPIOMUX_OUT_HIGH,
};
#endif

static struct msm_gpiomux_config msm8930_cam_common_configs[] = {
#if 0  //remove for  camera no use . wt
	{
		.gpio = 2,
		.settings = {
			[GPIOMUX_ACTIVE]    = &cam_settings[2],
			[GPIOMUX_SUSPENDED] = &cam_settings[0],
		},
	},
	{
		.gpio = 3,
		.settings = {
			[GPIOMUX_ACTIVE]    = &cam_settings[1],
			[GPIOMUX_SUSPENDED] = &cam_settings[0],
		},
	},
#endif	
	#ifdef CONFIG_MSM_CAMERA_V4L2 //modify by yanwei	
	{
		.gpio = 4,//front camera MCLK
		.settings = {
			[GPIOMUX_ACTIVE]    = &cam_settings[9],
			[GPIOMUX_SUSPENDED] = &cam_settings[0],
		},
	},
	{
		.gpio = 5,//back camera MCLK
		.settings = {
			[GPIOMUX_ACTIVE]    = &cam_settings[1],
			[GPIOMUX_SUSPENDED] = &cam_settings[0],
		},
	},
	{
		.gpio = 76,//front camera RST
		.settings = {
			[GPIOMUX_ACTIVE]    = &out_high_np_active__cfg1,
			[GPIOMUX_SUSPENDED] = &out_high_np_suspend_cfg2,	
		},
	},
#if defined(CONFIG_HI542) || defined(CONFIG_OV8825)|| defined(CONFIG_AR0542)|| defined(CONFIG_OV8835)
	{
		.gpio = 107,//back camera RST
		.settings = {
			[GPIOMUX_ACTIVE]    = &out_high_np_active__cfg1,
			[GPIOMUX_SUSPENDED] = &out_high_np_suspend_cfg3,	
		},
	},
#else
	{
		.gpio = 107,//back camera RST
		.settings = {
			[GPIOMUX_ACTIVE]    = &out_high_np_active__cfg1,
			[GPIOMUX_SUSPENDED] = &out_high_np_suspend_cfg2,	
		},
	},
#endif
	{
		.gpio = 53,//front camera STB
		.settings = {
			[GPIOMUX_ACTIVE]    = &out_high_np_active__cfg1,
			[GPIOMUX_SUSPENDED] = &out_high_np_suspend_cfg2,		
		},
	},	
#if defined(CONFIG_HI542) || defined(CONFIG_OV8825)|| defined(CONFIG_AR0542)|| defined(CONFIG_OV8835)
	{
		.gpio = 54,//back camera STB
		.settings = {
			[GPIOMUX_ACTIVE]    = &out_high_np_active__cfg1,
			[GPIOMUX_SUSPENDED] = &out_high_np_suspend_cfg3,		
		},
	},
#else
	{
		.gpio = 54,//back camera STB
		.settings = {
			[GPIOMUX_ACTIVE]    = &out_high_np_active__cfg1,
			[GPIOMUX_SUSPENDED] = &out_high_np_suspend_cfg2,		
		},
	},
#endif
			
	{
		.gpio = 9,//DVDD_LDO
		.settings = {
			[GPIOMUX_ACTIVE]    = &out_high_np_active__cfg2,
			[GPIOMUX_SUSPENDED] = &out_high_np_suspend_cfg1,
		},
	},	
#if defined(CONFIG_HI542) || defined(CONFIG_AR0542)|| defined(CONFIG_OV8835)
	{
		.gpio = 15,//MOTOR_LDO
		.settings = {
			[GPIOMUX_ACTIVE]    = &out_high_np_active__cfg2,
			[GPIOMUX_SUSPENDED] = &out_high_np_suspend_cfg3,
		},
	},		
#else
	{
		.gpio = 15,//MOTOR_LDO
		.settings = {
			[GPIOMUX_ACTIVE]    = &out_high_np_active__cfg2,
			[GPIOMUX_SUSPENDED] = &out_high_np_suspend_cfg1,
		},
	},
#endif
	{
		.gpio = 55,//AVDD_LDO
		.settings = {
			[GPIOMUX_ACTIVE]    = &out_high_np_active__cfg2,
			[GPIOMUX_SUSPENDED] = &out_high_np_suspend_cfg1,
		},
	},	
#if defined(CONFIG_MACH_DEMETER)|| defined(CONFIG_OV8835)//iovdd for OV8835
	{
		.gpio = 43,//AVDD_LDO
		.settings = {
			[GPIOMUX_ACTIVE]    = &out_high_np_active__cfg2,
			[GPIOMUX_SUSPENDED] = &out_high_np_suspend_cfg1,
		},
	},
#endif	

#ifdef CONFIG_ADP1650

	{
		.gpio      = 18,		/* Flash Stobe */
		.settings = {
			[GPIOMUX_SUSPENDED] = &cam_suspended_config,
			[GPIOMUX_ACTIVE] = &cam_active,
		},
	},
	
	{
		.gpio      = 64,		/* Flash En */
		.settings = {
			[GPIOMUX_SUSPENDED] = &cam_suspended_config,
			[GPIOMUX_ACTIVE] = &cam_active,
		},
	},

	{
		.gpio      = 91,		/* Flash Touch */
		.settings = {
			[GPIOMUX_SUSPENDED] = &cam_suspended_config,
			[GPIOMUX_ACTIVE] = &cam_active,
		},
	},
#endif

	#endif
};

static struct msm_gpiomux_config msm8930_cam_2d_configs[] = {
	{
		.gpio = 18,
		.settings = {
			[GPIOMUX_ACTIVE]    = &cam_settings[2],
			[GPIOMUX_SUSPENDED] = &cam_settings[8],			
		},
	},
#if 0
	{
		.gpio = 19,
		.settings = {
			[GPIOMUX_ACTIVE]    = &cam_settings[3],
			[GPIOMUX_SUSPENDED] = &cam_settings[8],
		},
	},
#endif
	{
		.gpio = 20,
		.settings = {
			[GPIOMUX_ACTIVE]    = &cam_settings[3],
			[GPIOMUX_SUSPENDED] = &cam_settings[8],
		},
	},
	{
		.gpio = 21,
		.settings = {
			[GPIOMUX_ACTIVE]    = &cam_settings[3],
			[GPIOMUX_SUSPENDED] = &cam_settings[8],
		},
	},	
};

#if 0  //remove for  camera no use . wt
#define VFE_CAMIF_TIMER1_GPIO 2
#define VFE_CAMIF_TIMER2_GPIO 3
#define VFE_CAMIF_TIMER3_GPIO_INT 4

#if 0
static struct msm_camera_sensor_strobe_flash_data strobe_flash_xenon = {
	.flash_trigger = VFE_CAMIF_TIMER2_GPIO,
	.flash_charge = VFE_CAMIF_TIMER1_GPIO,
	.flash_charge_done = VFE_CAMIF_TIMER3_GPIO_INT,
	.flash_recharge_duration = 50000,
	.irq = MSM_GPIO_TO_INT(VFE_CAMIF_TIMER3_GPIO_INT),
};
#endif
#ifdef CONFIG_MSM_CAMERA_FLASH
static struct msm_camera_sensor_flash_src msm_flash_src = {
	.flash_sr_type = MSM_CAMERA_FLASH_SRC_EXT,
	._fsrc.ext_driver_src.led_en = VFE_CAMIF_TIMER1_GPIO,
	._fsrc.ext_driver_src.led_flash_en = VFE_CAMIF_TIMER2_GPIO,
	._fsrc.ext_driver_src.flash_id = MAM_CAMERA_EXT_LED_FLASH_TPS61310,
};
#endif
#endif

static struct msm_bus_vectors cam_init_vectors[] = {
	{
		.src = MSM_BUS_MASTER_VFE,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 0,
		.ib  = 0,
	},
	{
		.src = MSM_BUS_MASTER_VPE,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 0,
		.ib  = 0,
	},
	{
		.src = MSM_BUS_MASTER_JPEG_ENC,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 0,
		.ib  = 0,
	},
};

static struct msm_bus_vectors cam_preview_vectors[] = {
	{
		.src = MSM_BUS_MASTER_VFE,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 27648000,
		.ib  = 2656000000UL,
	},
	{
		.src = MSM_BUS_MASTER_VPE,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 0,
		.ib  = 0,
	},
	{
		.src = MSM_BUS_MASTER_JPEG_ENC,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 0,
		.ib  = 0,
	},
};

static struct msm_bus_vectors cam_video_vectors[] = {
	{
		.src = MSM_BUS_MASTER_VFE,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 600000000,
		.ib  = 2656000000UL,
	},
	{
		.src = MSM_BUS_MASTER_VPE,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 206807040,
		.ib  = 488816640,
	},
	{
		.src = MSM_BUS_MASTER_JPEG_ENC,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 0,
		.ib  = 0,
	},
};

static struct msm_bus_vectors cam_snapshot_vectors[] = {
	{
		.src = MSM_BUS_MASTER_VFE,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 600000000,
		.ib  = 2656000000UL,
	},
	{
		.src = MSM_BUS_MASTER_VPE,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 0,
		.ib  = 0,
	},
	{
		.src = MSM_BUS_MASTER_JPEG_ENC,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 540000000,
		.ib  = 1350000000,
	},
};

static struct msm_bus_vectors cam_zsl_vectors[] = {
	{
		.src = MSM_BUS_MASTER_VFE,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 600000000,
		.ib  = 2656000000UL,
	},
	{
		.src = MSM_BUS_MASTER_VPE,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 0,
		.ib  = 0,
	},
	{
		.src = MSM_BUS_MASTER_JPEG_ENC,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 540000000,
		.ib  = 1350000000,
	},
};

static struct msm_bus_vectors cam_video_ls_vectors[] = {
	{
		.src = MSM_BUS_MASTER_VFE,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 600000000,
		.ib  = 4264000000UL,
	},
	{
		.src = MSM_BUS_MASTER_VPE,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 206807040,
		.ib  = 488816640,
	},
	{
		.src = MSM_BUS_MASTER_JPEG_ENC,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 540000000,
		.ib  = 1350000000,
	},
};

static struct msm_bus_vectors cam_dual_vectors[] = {
	{
		.src = MSM_BUS_MASTER_VFE,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 302071680,
		.ib  = 2656000000UL,
	},
	{
		.src = MSM_BUS_MASTER_VPE,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 206807040,
		.ib  = 488816640,
	},
	{
		.src = MSM_BUS_MASTER_JPEG_ENC,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 540000000,
		.ib  = 1350000000,
	},
};

static struct msm_bus_vectors cam_adv_video_vectors[] = {
	{
		.src = MSM_BUS_MASTER_VFE,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 274406400,
		.ib  = 2656000000UL,
	},
	{
		.src = MSM_BUS_MASTER_VPE,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 206807040,
		.ib  = 488816640,
	},
	{
		.src = MSM_BUS_MASTER_JPEG_ENC,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 0,
		.ib  = 0,
	},
};


static struct msm_bus_paths cam_bus_client_config[] = {
	{
		ARRAY_SIZE(cam_init_vectors),
		cam_init_vectors,
	},
	{
		ARRAY_SIZE(cam_preview_vectors),
		cam_preview_vectors,
	},
	{
		ARRAY_SIZE(cam_video_vectors),
		cam_video_vectors,
	},
	{
		ARRAY_SIZE(cam_snapshot_vectors),
		cam_snapshot_vectors,
	},
	{
		ARRAY_SIZE(cam_zsl_vectors),
		cam_zsl_vectors,
	},
	{
		ARRAY_SIZE(cam_video_ls_vectors),
		cam_video_ls_vectors,
	},
	{
		ARRAY_SIZE(cam_dual_vectors),
		cam_dual_vectors,
	},
	{
		ARRAY_SIZE(cam_adv_video_vectors),
		cam_adv_video_vectors,
	},

};

static struct msm_bus_scale_pdata cam_bus_client_pdata = {
		cam_bus_client_config,
		ARRAY_SIZE(cam_bus_client_config),
		.name = "msm_camera",
};

static struct msm_camera_device_platform_data msm_camera_csi_device_data[] = {
	{
		.csid_core = 0,
		.is_vpe    = 1,
		.cam_bus_scale_table = &cam_bus_client_pdata,
	},
	{
		.csid_core = 1,
		.is_vpe    = 1,
		.cam_bus_scale_table = &cam_bus_client_pdata,
	},
};

static struct camera_vreg_t msm_8930_cam_vreg[] = {
	{"cam_vdig", REG_LDO, 1800000, 1800000, 105000},	
	//{"cam_vio", REG_VS, 0, 0, 0},
	//{"cam_vana", REG_LDO, 2800000, 2850000, 85600},
	//{"cam_vaf", REG_LDO, 2800000, 2850000, 300000},
};

static struct gpio msm8930_common_cam_gpio[] = {
	{20, GPIOF_DIR_IN, "CAMIF_I2C_DATA"},
	{21, GPIOF_DIR_IN, "CAMIF_I2C_CLK"},
};
#ifdef CONFIG_OV9740
#if defined(CONFIG_HI542) || defined(CONFIG_AR0542)	
static struct gpio msm8930_front_cam_ov9740_gpio[] = {
	{4, GPIOF_DIR_IN, "CAMIF_MCLK"},
	{53, GPIOF_DIR_OUT, "CAM_STBY_N"},
	{76, GPIOF_DIR_OUT, "CAM_RESET"},	
	{9, GPIOF_DIR_OUT, "DVDD_LDO_EN"},
	{55, GPIOF_DIR_OUT, "AVDD_LDO_EN"},	
};

static struct msm_gpio_set_tbl msm8930_front_cam_ov9740_gpio_set_tbl[] = {
	{55, GPIOF_OUT_INIT_HIGH, 1000},
	{9, GPIOF_OUT_INIT_HIGH, 1000},		
	{53, GPIOF_OUT_INIT_HIGH, 1000},
	{53, GPIOF_OUT_INIT_LOW, 1000},
	{76, GPIOF_OUT_INIT_HIGH, 4000},	
	{76, GPIOF_OUT_INIT_LOW, 1000},
	{76, GPIOF_OUT_INIT_HIGH, 4000},	
};
#else
static struct gpio msm8930_front_cam_ov9740_gpio[] = {
	{4, GPIOF_DIR_IN, "CAMIF_MCLK"},
	{53, GPIOF_DIR_OUT, "CAM_STBY_N"},
	{76, GPIOF_DIR_OUT, "CAM_RESET"},	
	//{9, GPIOF_DIR_OUT, "DVDD_LDO_EN"},
	//{55, GPIOF_DIR_OUT, "AVDD_LDO_EN"},	
};

static struct msm_gpio_set_tbl msm8930_front_cam_ov9740_gpio_set_tbl[] = {
	{53, GPIOF_OUT_INIT_HIGH, 1000},
	{53, GPIOF_OUT_INIT_LOW, 1000},
	{76, GPIOF_OUT_INIT_HIGH, 4000},	
	{76, GPIOF_OUT_INIT_LOW, 1000},
	{76, GPIOF_OUT_INIT_HIGH, 4000},
	//{9, GPIOF_OUT_INIT_HIGH, 1000},	
	//{55, GPIOF_OUT_INIT_HIGH, 1000},	
};
#endif
static struct msm_camera_gpio_conf msm_8930_front_cam_ov9740_gpio_conf = {
	.cam_gpiomux_conf_tbl = msm8930_cam_2d_configs,
	.cam_gpiomux_conf_tbl_size = ARRAY_SIZE(msm8930_cam_2d_configs),
	.cam_gpio_common_tbl = msm8930_common_cam_gpio,
	.cam_gpio_common_tbl_size = ARRAY_SIZE(msm8930_common_cam_gpio),
	.cam_gpio_req_tbl = msm8930_front_cam_ov9740_gpio,
	.cam_gpio_req_tbl_size = ARRAY_SIZE(msm8930_front_cam_ov9740_gpio),
	.cam_gpio_set_tbl = msm8930_front_cam_ov9740_gpio_set_tbl,
	.cam_gpio_set_tbl_size = ARRAY_SIZE(msm8930_front_cam_ov9740_gpio_set_tbl),
};

static struct msm_camera_sensor_flash_data flash_ov9740 = {
	.flash_type = MSM_CAMERA_FLASH_NONE
};

static struct msm_camera_csi_lane_params ov9740_csi_lane_params = {
	.csi_lane_assign = 0xE4,
	.csi_lane_mask = 0x1,
};

static struct msm_camera_sensor_platform_info sensor_board_info_ov9740 = {
	.mount_angle	= 270,	
	.cam_vreg = msm_8930_cam_vreg,
	.num_vreg = ARRAY_SIZE(msm_8930_cam_vreg),
	.gpio_conf = &msm_8930_front_cam_ov9740_gpio_conf,
	.csi_lane_params = &ov9740_csi_lane_params,
};

static struct msm_camera_sensor_info msm_camera_sensor_ov9740_data = {
	.sensor_name = "ov9740",
	.pdata = &msm_camera_csi_device_data[1],
	.flash_data = &flash_ov9740,
	.sensor_platform_info = &sensor_board_info_ov9740,
	.csi_if = 1,
	.camera_type = FRONT_CAMERA_2D,
	.sensor_type = YUV_SENSOR,
};
#endif
#ifdef CONFIG_SP0A28
static struct gpio msm8930_front_cam_sp0a28_gpio[] = {
	{4, GPIOF_DIR_IN, "CAMIF_MCLK"},
	{53, GPIOF_DIR_OUT, "CAM_STBY_N"},
	{76, GPIOF_DIR_OUT, "CAM_RESET"},	
	//{9, GPIOF_DIR_OUT, "DVDD_LDO_EN"},
	//{55, GPIOF_DIR_OUT, "AVDD_LDO_EN"},
};

static struct msm_gpio_set_tbl msm8930_front_cam_sp0a28_gpio_set_tbl[] = {
	{53, GPIOF_OUT_INIT_HIGH, 1000},
	{53, GPIOF_OUT_INIT_LOW, 1000},
	{76, GPIOF_OUT_INIT_HIGH, 4000},	
	{76, GPIOF_OUT_INIT_LOW, 1000},
	{76, GPIOF_OUT_INIT_HIGH, 4000},
	//{9, GPIOF_OUT_INIT_HIGH, 1000},	
	//{55, GPIOF_OUT_INIT_HIGH, 1000},
};

static struct msm_camera_gpio_conf msm_8930_front_cam_sp0a28_gpio_conf = {
	.cam_gpiomux_conf_tbl = msm8930_cam_2d_configs,
	.cam_gpiomux_conf_tbl_size = ARRAY_SIZE(msm8930_cam_2d_configs),
	.cam_gpio_common_tbl = msm8930_common_cam_gpio,
	.cam_gpio_common_tbl_size = ARRAY_SIZE(msm8930_common_cam_gpio),
	.cam_gpio_req_tbl = msm8930_front_cam_sp0a28_gpio,
	.cam_gpio_req_tbl_size = ARRAY_SIZE(msm8930_front_cam_sp0a28_gpio),
	.cam_gpio_set_tbl = msm8930_front_cam_sp0a28_gpio_set_tbl,
	.cam_gpio_set_tbl_size = ARRAY_SIZE(msm8930_front_cam_sp0a28_gpio_set_tbl),
};

static struct msm_camera_sensor_flash_data flash_sp0a28 = {
	.flash_type = MSM_CAMERA_FLASH_NONE
};

static struct msm_camera_csi_lane_params sp0a28_csi_lane_params = {
	.csi_lane_assign = 0xE4,
	.csi_lane_mask = 0x1,
};

static struct msm_camera_sensor_platform_info sensor_board_info_sp0a28 = {
	.mount_angle = 270,
	.cam_vreg = msm_8930_cam_vreg,
	.num_vreg = ARRAY_SIZE(msm_8930_cam_vreg),
	.gpio_conf = &msm_8930_front_cam_sp0a28_gpio_conf,
	.csi_lane_params = &sp0a28_csi_lane_params,
};

static struct msm_camera_sensor_info msm_camera_sensor_sp0a28_data = {
	.sensor_name = "sp0a28",
	.pdata = &msm_camera_csi_device_data[1],
	.flash_data = &flash_sp0a28,
	.sensor_platform_info = &sensor_board_info_sp0a28,
	.csi_if = 1,
	.camera_type = FRONT_CAMERA_2D,
	.sensor_type = YUV_SENSOR,
};
#endif

#ifndef  CONFIG_HI542 
static struct gpio msm8930_back_cam_gpio[] = {
	{5, GPIOF_DIR_IN, "CAMIF_MCLK"},	
	{54, GPIOF_DIR_OUT, "CAM_STBY_N"},
	{107, GPIOF_DIR_OUT, "CAM_RESET"},	
	//{15, GPIOF_DIR_OUT, "MOTOR_LDO_EN"},
	//{55, GPIOF_DIR_OUT, "AVDD_LDO_EN"},		
};

#ifdef CONFIG_OV5640
static struct msm_gpio_set_tbl msm8930_back_cam_gpio_set_tbl[] = {
	{54, GPIOF_OUT_INIT_HIGH, 1000},
	{54, GPIOF_OUT_INIT_LOW, 1000},
	{107, GPIOF_OUT_INIT_HIGH, 4000},	
	{107, GPIOF_OUT_INIT_LOW, 1000},
	{107, GPIOF_OUT_INIT_HIGH, 4000},
	//{15, GPIOF_OUT_INIT_HIGH, 1000},	
	//{55, GPIOF_OUT_INIT_HIGH, 1000},	
};

static struct i2c_board_info msm_act_main_cam_i2c_info = {
	I2C_BOARD_INFO("msm_actuator", 0x11),
};

static struct msm_actuator_info msm_act_main_cam_0_info = {
	.board_info     = &msm_act_main_cam_i2c_info,
	.cam_name   = MSM_ACTUATOR_MAIN_CAM_0,
	.bus_id         = MSM_8930_GSBI4_QUP_I2C_BUS_ID,
	.vcm_pwd        = 0,
	.vcm_enable     = 0,
};

static struct msm_camera_gpio_conf msm_8930_back_cam_gpio_conf = {
	.cam_gpiomux_conf_tbl = msm8930_cam_2d_configs,
	.cam_gpiomux_conf_tbl_size = ARRAY_SIZE(msm8930_cam_2d_configs),
	.cam_gpio_common_tbl = msm8930_common_cam_gpio,
	.cam_gpio_common_tbl_size = ARRAY_SIZE(msm8930_common_cam_gpio),
	.cam_gpio_req_tbl = msm8930_back_cam_gpio,
	.cam_gpio_req_tbl_size = ARRAY_SIZE(msm8930_back_cam_gpio),
	.cam_gpio_set_tbl = msm8930_back_cam_gpio_set_tbl,
	.cam_gpio_set_tbl_size = ARRAY_SIZE(msm8930_back_cam_gpio_set_tbl),
};
#endif

#endif
#ifdef CONFIG_OV5640
static struct msm_camera_sensor_flash_data flash_ov5640 = {
	.flash_type	= MSM_CAMERA_FLASH_LED,
};

static struct msm_camera_csi_lane_params ov5640_csi_lane_params = {
	.csi_lane_assign = 0xE4,
	.csi_lane_mask = 0x3,
};

static struct msm_camera_sensor_platform_info sensor_board_info_ov5640 = {
	.mount_angle	= 90,
	.cam_vreg = msm_8930_cam_vreg,
	.num_vreg = ARRAY_SIZE(msm_8930_cam_vreg),
	.gpio_conf = &msm_8930_back_cam_gpio_conf,
	.csi_lane_params = &ov5640_csi_lane_params,
};

static struct msm_camera_sensor_info msm_camera_sensor_ov5640_data = {
	.sensor_name	= "ov5640",
	.pdata	= &msm_camera_csi_device_data[0],
	.flash_data	= &flash_ov5640,
	.sensor_platform_info = &sensor_board_info_ov5640,
	.csi_if	= 1,
	.camera_type = BACK_CAMERA_2D,
	.sensor_type = YUV_SENSOR,
	.actuator_info = &msm_act_main_cam_0_info,
};
#endif
#if defined(CONFIG_HI542) || defined(CONFIG_AR0542)
static struct i2c_board_info msm_act_main_cam1_i2c_info = {
	I2C_BOARD_INFO("msm_actuator", 0x18),
};

static struct msm_actuator_info msm_act_main_cam_1_info = {
	.board_info     = &msm_act_main_cam1_i2c_info,
	.cam_name       = MSM_ACTUATOR_MAIN_CAM_1,
	.bus_id         = MSM_8930_GSBI4_QUP_I2C_BUS_ID,
	.vcm_pwd        = 0,
	.vcm_enable     = 0,
};
#endif
#ifdef CONFIG_HI542
#ifdef CONFIG_HI542_EEPROM
static struct i2c_board_info hi542_eeprom_i2c_info = {
	I2C_BOARD_INFO("hi542_eeprom", 0x5a>>1),
};

static struct msm_eeprom_info hi542_eeprom_info = {
	.board_info     = &hi542_eeprom_i2c_info,
	.bus_id         = MSM_8930_GSBI4_QUP_I2C_BUS_ID,
	.eeprom_i2c_slave_addr = 0x40,
	.eeprom_reg_addr = 0x05,
	.eeprom_read_length = 6,
};
#endif
static struct gpio msm8930_hi542_cam_gpio[] = {
	{5, GPIOF_DIR_IN, "CAMIF_MCLK"},
	{9, GPIOF_DIR_OUT, "DVDD_LDO_EN"},		
	{54, GPIOF_DIR_OUT, "CAM_STBY_N"},
	{107, GPIOF_DIR_OUT, "CAM_RESET"},	
	{15, GPIOF_DIR_OUT, "MOTOR_LDO_EN"},
	{55, GPIOF_DIR_OUT, "AVDD_LDO_EN"},		
};
static struct msm_gpio_set_tbl msm8930_hi542_cam_gpio_set_tbl[] = {
	{15, GPIOF_OUT_INIT_HIGH, 1000},	
	{55, GPIOF_OUT_INIT_HIGH, 1000},	
	{9, GPIOF_OUT_INIT_HIGH, 1000},	
	{15, GPIOF_OUT_INIT_HIGH, 1000},
	{54, GPIOF_OUT_INIT_LOW, 1000},
	{54, GPIOF_OUT_INIT_HIGH, 1000},
	{107, GPIOF_OUT_INIT_HIGH, 4000},
	{107, GPIOF_OUT_INIT_LOW, 1000},
	{107, GPIOF_OUT_INIT_HIGH, 4000},		
};
static struct msm_camera_gpio_conf msm_8930_hi542_cam_gpio_conf = {
	.cam_gpiomux_conf_tbl = msm8930_cam_2d_configs,
	.cam_gpiomux_conf_tbl_size = ARRAY_SIZE(msm8930_cam_2d_configs),
	.cam_gpio_common_tbl = msm8930_common_cam_gpio,
	.cam_gpio_common_tbl_size = ARRAY_SIZE(msm8930_common_cam_gpio),
	.cam_gpio_req_tbl = msm8930_hi542_cam_gpio,
	.cam_gpio_req_tbl_size = ARRAY_SIZE(msm8930_hi542_cam_gpio),
	.cam_gpio_set_tbl = msm8930_hi542_cam_gpio_set_tbl,
	.cam_gpio_set_tbl_size = ARRAY_SIZE(msm8930_hi542_cam_gpio_set_tbl),
};
static struct msm_camera_sensor_flash_data flash_hi542 = {
	.flash_type	= MSM_CAMERA_FLASH_LED,
};

static struct msm_camera_csi_lane_params hi542_csi_lane_params = {
	.csi_lane_assign = 0xE4,
	.csi_lane_mask = 0x3,
};

static struct msm_camera_sensor_platform_info sensor_board_info_hi542 = {
	.mount_angle	= 90,
	.cam_vreg = msm_8930_cam_vreg,
	.num_vreg = ARRAY_SIZE(msm_8930_cam_vreg),
	.gpio_conf = &msm_8930_hi542_cam_gpio_conf,
	.csi_lane_params = &hi542_csi_lane_params,
};

static struct msm_camera_sensor_info msm_camera_sensor_hi542_data = {
	.sensor_name	= "hi542",
	.pdata	= &msm_camera_csi_device_data[0],
	.flash_data	= &flash_hi542,
	.sensor_platform_info = &sensor_board_info_hi542,
	.csi_if	= 1,
	.camera_type = BACK_CAMERA_2D,
	.sensor_type = BAYER_SENSOR,
	.actuator_info = &msm_act_main_cam_1_info,
#ifdef CONFIG_HI542_EEPROM
  .eeprom_info = &hi542_eeprom_info,
#endif
};
#endif
#if defined CONFIG_AR0542
#ifdef CONFIG_AR0542_EEPROM
static struct i2c_board_info ar0542_eeprom_i2c_info = {
	I2C_BOARD_INFO("ar0542_eeprom", 0x64>>1),
};

static struct msm_eeprom_info ar0542_eeprom_info = {
	.board_info     = &ar0542_eeprom_i2c_info,
	.bus_id         = MSM_8930_GSBI4_QUP_I2C_BUS_ID,
	.eeprom_i2c_slave_addr = 0x6C,
	.eeprom_reg_addr = 0x05,
	.eeprom_read_length = 6,
};
#endif
static struct gpio msm8930_ar0542_cam_gpio[] = {
	{5, GPIOF_DIR_IN, "CAMIF_MCLK"},
	{9, GPIOF_DIR_OUT, "DVDD_LDO_EN"},		
	{54, GPIOF_DIR_OUT, "CAM_STBY_N"},
	{107, GPIOF_DIR_OUT, "CAM_RESET"},	
	{15, GPIOF_DIR_OUT, "MOTOR_LDO_EN"},
	{55, GPIOF_DIR_OUT, "AVDD_LDO_EN"},		
};
static struct msm_gpio_set_tbl msm8930_ar0542_cam_gpio_set_tbl[] = {
	{15, GPIOF_OUT_INIT_HIGH, 1000},	
	{55, GPIOF_OUT_INIT_HIGH, 1000},	
	{9, GPIOF_OUT_INIT_HIGH, 1000},	
	{15, GPIOF_OUT_INIT_HIGH, 1000},
	{54, GPIOF_OUT_INIT_LOW, 1000},
	{54, GPIOF_OUT_INIT_HIGH, 1000},
	{107, GPIOF_OUT_INIT_HIGH, 4000},
	{107, GPIOF_OUT_INIT_LOW, 1000},
	{107, GPIOF_OUT_INIT_HIGH, 4000},		
};
static struct msm_camera_gpio_conf msm_8930_ar0542_cam_gpio_conf = {
	.cam_gpiomux_conf_tbl = msm8930_cam_2d_configs,
	.cam_gpiomux_conf_tbl_size = ARRAY_SIZE(msm8930_cam_2d_configs),
	.cam_gpio_common_tbl = msm8930_common_cam_gpio,
	.cam_gpio_common_tbl_size = ARRAY_SIZE(msm8930_common_cam_gpio),
	.cam_gpio_req_tbl = msm8930_ar0542_cam_gpio,
	.cam_gpio_req_tbl_size = ARRAY_SIZE(msm8930_ar0542_cam_gpio),
	.cam_gpio_set_tbl = msm8930_ar0542_cam_gpio_set_tbl,
	.cam_gpio_set_tbl_size = ARRAY_SIZE(msm8930_ar0542_cam_gpio_set_tbl),
};
static struct msm_camera_sensor_flash_data flash_ar0542 = {
	.flash_type	= MSM_CAMERA_FLASH_LED,
};

static struct msm_camera_csi_lane_params ar0542_csi_lane_params = {
	.csi_lane_assign = 0xE4,
	.csi_lane_mask = 0x3,
};

static struct msm_camera_sensor_platform_info sensor_board_info_ar0542 = {
	.mount_angle	= 90,
	.cam_vreg = msm_8930_cam_vreg,
	.num_vreg = ARRAY_SIZE(msm_8930_cam_vreg),
	.gpio_conf = &msm_8930_ar0542_cam_gpio_conf,
	.csi_lane_params = &ar0542_csi_lane_params,
};

static struct msm_camera_sensor_info msm_camera_sensor_ar0542_data = {
	.sensor_name	= "ar0542",
	.pdata	= &msm_camera_csi_device_data[0],
	.flash_data	= &flash_ar0542,
	.sensor_platform_info = &sensor_board_info_ar0542,
	.csi_if	= 1,
	.camera_type = BACK_CAMERA_2D,
	.sensor_type = BAYER_SENSOR,
	.actuator_info = &msm_act_main_cam_1_info,
#ifdef CONFIG_AR0542_EEPROM
        .eeprom_info = &ar0542_eeprom_info,
#endif
};
#endif
#ifdef CONFIG_IMX074
static struct msm_camera_sensor_flash_data flash_imx074 = {
	.flash_type	= MSM_CAMERA_FLASH_LED,
#ifdef CONFIG_MSM_CAMERA_FLASH
	//.flash_src	= &msm_flash_src  //remove for  camera no use . wt
#endif
};

static struct msm_camera_csi_lane_params imx074_csi_lane_params = {
	.csi_lane_assign = 0xE4,
	.csi_lane_mask = 0xF,
};

static struct msm_camera_sensor_platform_info sensor_board_info_imx074 = {
	.mount_angle	= 90,
	.cam_vreg = msm_8930_cam_vreg,
	.num_vreg = ARRAY_SIZE(msm_8930_cam_vreg),
	.gpio_conf = &msm_8930_back_cam_gpio_conf,
	.csi_lane_params = &imx074_csi_lane_params,
};

static struct msm_camera_sensor_info msm_camera_sensor_imx074_data = {
	.sensor_name	= "imx074",
	.pdata	= &msm_camera_csi_device_data[0],
	.flash_data	= &flash_imx074,
	.strobe_flash_data = &strobe_flash_xenon,
	.sensor_platform_info = &sensor_board_info_imx074,
	.csi_if	= 1,
	.camera_type = BACK_CAMERA_2D,
	.sensor_type = BAYER_SENSOR,
	.actuator_info = &msm_act_main_cam_0_info,
};
#endif
#ifdef CONFIG_OV8825_ACT
static struct i2c_board_info ov8825_actuator_i2c_info = {
 I2C_BOARD_INFO("msm_actuator", 0x6C>>2),
 };
static struct msm_actuator_info ov8825_actuator_info = {
  .board_info     = &ov8825_actuator_i2c_info,
  .cam_name       = MSM_ACTUATOR_MAIN_CAM_3,
  .bus_id         = MSM_8930_GSBI4_QUP_I2C_BUS_ID,
  .vcm_pwd        = 15,
  .vcm_enable    = 1,
  };
#endif

#ifdef CONFIG_OV8825_ROHM
static struct i2c_board_info ov8825_actuator_i2c_info = {
 I2C_BOARD_INFO("msm_actuator", 0x18>>1),
 };
static struct msm_actuator_info ov8825_actuator_info = {
  .board_info     = &ov8825_actuator_i2c_info,
  .cam_name       = MSM_ACTUATOR_MAIN_CAM_1,
  .bus_id         = MSM_8930_GSBI4_QUP_I2C_BUS_ID,
  .vcm_pwd        = 15,
  .vcm_enable    = 1,
  };
#endif
/* Added by ZTE_CAM-WT  for OV8825 begin */
#ifdef CONFIG_OV8825

static struct msm_gpio_set_tbl msm8930_ov8825_cam_gpio_set_tbl[] = {
	{15, GPIOF_OUT_INIT_HIGH, 1000},
	{55, GPIOF_OUT_INIT_HIGH, 1000},	
	{9, GPIOF_OUT_INIT_HIGH, 1000},
	{54, GPIOF_OUT_INIT_LOW, 1000},
	{54, GPIOF_OUT_INIT_HIGH, 1000},
	{107, GPIOF_OUT_INIT_HIGH, 4000},	
	{107, GPIOF_OUT_INIT_LOW, 1000},
	{107, GPIOF_OUT_INIT_HIGH, 4000},
	
	
	
};


static struct msm_camera_gpio_conf msm_8930_ov8825_cam_gpio_conf = {
	.cam_gpiomux_conf_tbl = msm8930_cam_2d_configs,
	.cam_gpiomux_conf_tbl_size = ARRAY_SIZE(msm8930_cam_2d_configs),
	.cam_gpio_common_tbl = msm8930_common_cam_gpio,
	.cam_gpio_common_tbl_size = ARRAY_SIZE(msm8930_common_cam_gpio),
	.cam_gpio_req_tbl = msm8930_back_cam_gpio,
	.cam_gpio_req_tbl_size = ARRAY_SIZE(msm8930_back_cam_gpio),
	.cam_gpio_set_tbl = msm8930_ov8825_cam_gpio_set_tbl,
	.cam_gpio_set_tbl_size = ARRAY_SIZE(msm8930_ov8825_cam_gpio_set_tbl),
};
static struct camera_vreg_t msm_8930_ov8825_vreg[] = {
//       {"mipi_csi_vdd", REG_LDO, 1200000, 1200000, 20000},
	//{"cam_vana", REG_LDO, 2800000, 2850000, 85600},
	//{"cam_vio", REG_VS, 0, 0, 0},
	//{"cam_vdig", REG_LDO, 1800000, 1800000, 105000},
	//{"cam_vaf", REG_LDO, 2800000, 2800000, 300000},
};

static struct msm_camera_sensor_flash_data flash_ov8825 = {
#ifdef CONFIG_LM3642
	.flash_type	= MSM_CAMERA_FLASH_LED,
#else
	.flash_type	= MSM_CAMERA_FLASH_NONE,		
#endif
	

};

static struct msm_camera_csi_lane_params ov8825_csi_lane_params = {
	.csi_lane_assign = 0xE4,
	.csi_lane_mask = 0xF,
};
static struct msm_camera_sensor_platform_info sensor_board_info_ov8825 = {
	.mount_angle	= 90,
	.cam_vreg = msm_8930_ov8825_vreg,
	.num_vreg = ARRAY_SIZE(msm_8930_ov8825_vreg),
	.gpio_conf = &msm_8930_ov8825_cam_gpio_conf,
	.csi_lane_params = &ov8825_csi_lane_params,
};
static struct msm_camera_sensor_info msm_camera_sensor_ov8825_data = {
	.sensor_name	= "ov8825",
	.pdata	= &msm_camera_csi_device_data[0],
	.flash_data	= &flash_ov8825,
	.sensor_platform_info = &sensor_board_info_ov8825,
	.csi_if	= 1,//0:Parallel interface, 1:MIPI interface 
	.camera_type = BACK_CAMERA_2D,
	.sensor_type = BAYER_SENSOR,
	.actuator_info = &ov8825_actuator_info

};
#endif
/* Added by ZTE_CAM-WT OV8825 end */

#ifdef CONFIG_OV8835

static struct msm_gpio_set_tbl msm8930_ov8835_cam_gpio_set_tbl[] = {
	{107, GPIOF_OUT_INIT_HIGH, 5000},
	{54, GPIOF_OUT_INIT_HIGH, 5000},
};

static struct msm_camera_gpio_conf msm_8930_ov8835_cam_gpio_conf = {
	.cam_gpiomux_conf_tbl = msm8930_cam_2d_configs,
	.cam_gpiomux_conf_tbl_size = ARRAY_SIZE(msm8930_cam_2d_configs),
	.cam_gpio_common_tbl = msm8930_common_cam_gpio,
	.cam_gpio_common_tbl_size = ARRAY_SIZE(msm8930_common_cam_gpio),
	.cam_gpio_req_tbl = msm8930_back_cam_gpio,
	.cam_gpio_req_tbl_size = ARRAY_SIZE(msm8930_back_cam_gpio),
	.cam_gpio_set_tbl = msm8930_ov8835_cam_gpio_set_tbl,
	.cam_gpio_set_tbl_size = ARRAY_SIZE(msm8930_ov8835_cam_gpio_set_tbl),
};
static struct camera_vreg_t msm_8930_ov8835_vreg[] = {
	{"cam_vana", REG_GPIO, 2800000, 2850000, 55,0},
	{"cam_vio",    REG_GPIO, 1800000, 1800000, 43,0},
	{"cam_vdig",  REG_GPIO, 1200000, 1200000, 9,0},
//	{"cam_vaf",   REG_GPIO, 2850000, 2850000, 15,0},
};

static struct msm_camera_sensor_flash_data flash_ov8835 = {
#ifdef CONFIG_ADP1650
	.flash_type	= MSM_CAMERA_FLASH_LED,
#else
	.flash_type	= MSM_CAMERA_FLASH_NONE,		
#endif

};

static struct msm_camera_csi_lane_params ov8835_csi_lane_params = {
	.csi_lane_assign = 0xE4,
	.csi_lane_mask = 0xF,
};
static struct msm_camera_sensor_platform_info sensor_board_info_ov8835 = {
	.mount_angle	= 90,
	.cam_vreg = msm_8930_ov8835_vreg,
	.num_vreg = ARRAY_SIZE(msm_8930_ov8835_vreg),
	.gpio_conf = &msm_8930_ov8835_cam_gpio_conf,
	.csi_lane_params = &ov8835_csi_lane_params,
};
static struct msm_camera_sensor_info msm_camera_sensor_ov8835_data = {
	.sensor_name	= "ov8835",
	.pdata	= &msm_camera_csi_device_data[0],
	.flash_data	= &flash_ov8835,
	.sensor_platform_info = &sensor_board_info_ov8835,
	.csi_if	= 1,//0:Parallel interface, 1:MIPI interface 
	.camera_type = BACK_CAMERA_2D,
	.sensor_type = BAYER_SENSOR,
#ifdef CONFIG_OV8825_ROHM
	.actuator_info = &ov8825_actuator_info,
#endif
};
#endif

#ifdef CONFIG_MT9M114
static struct msm_camera_gpio_conf msm_8930_front_cam_gpio_conf = {
	.cam_gpiomux_conf_tbl = msm8930_cam_2d_configs,
	.cam_gpiomux_conf_tbl_size = ARRAY_SIZE(msm8930_cam_2d_configs),
	.cam_gpio_common_tbl = msm8930_common_cam_gpio,
	.cam_gpio_common_tbl_size = ARRAY_SIZE(msm8930_common_cam_gpio),
	.cam_gpio_req_tbl = msm8930_front_cam_gpio,
	.cam_gpio_req_tbl_size = ARRAY_SIZE(msm8930_front_cam_gpio),
	.cam_gpio_set_tbl = msm8930_front_cam_gpio_set_tbl,
	.cam_gpio_set_tbl_size = ARRAY_SIZE(msm8930_front_cam_gpio_set_tbl),
};

static struct msm_camera_sensor_flash_data flash_mt9m114 = {
	.flash_type = MSM_CAMERA_FLASH_NONE
};

static struct msm_camera_csi_lane_params mt9m114_csi_lane_params = {
	.csi_lane_assign = 0xE4,
	.csi_lane_mask = 0x1,
};

static struct msm_camera_sensor_platform_info sensor_board_info_mt9m114 = {
	.mount_angle = 90,
	.cam_vreg = msm_8930_cam_vreg,
	.num_vreg = ARRAY_SIZE(msm_8930_cam_vreg),
	.gpio_conf = &msm_8930_front_cam_gpio_conf,
	.csi_lane_params = &mt9m114_csi_lane_params,
};

static struct msm_camera_sensor_info msm_camera_sensor_mt9m114_data = {
	.sensor_name = "mt9m114",
	.pdata = &msm_camera_csi_device_data[1],
	.flash_data = &flash_mt9m114,
	.sensor_platform_info = &sensor_board_info_mt9m114,
	.csi_if = 1,
	.camera_type = FRONT_CAMERA_2D,
	.sensor_type = YUV_SENSOR,
};
#endif

#ifdef CONFIG_OV2720

static struct msm_camera_gpio_conf msm_8930_front_cam_gpio_conf = {
	.cam_gpiomux_conf_tbl = msm8930_cam_2d_configs,
	.cam_gpiomux_conf_tbl_size = ARRAY_SIZE(msm8930_cam_2d_configs),
	.cam_gpio_common_tbl = msm8930_common_cam_gpio,
	.cam_gpio_common_tbl_size = ARRAY_SIZE(msm8930_common_cam_gpio),
	.cam_gpio_req_tbl = msm8930_front_cam_gpio,
	.cam_gpio_req_tbl_size = ARRAY_SIZE(msm8930_front_cam_gpio),
	.cam_gpio_set_tbl = msm8930_front_cam_gpio_set_tbl,
	.cam_gpio_set_tbl_size = ARRAY_SIZE(msm8930_front_cam_gpio_set_tbl),
};

static struct msm_camera_sensor_flash_data flash_ov2720 = {
	.flash_type	= MSM_CAMERA_FLASH_NONE,
};

static struct msm_camera_csi_lane_params ov2720_csi_lane_params = {
	.csi_lane_assign = 0xE4,
	.csi_lane_mask = 0x3,
};

static struct msm_camera_sensor_platform_info sensor_board_info_ov2720 = {
	.mount_angle	= 0,
	.cam_vreg = msm_8930_cam_vreg,
	.num_vreg = ARRAY_SIZE(msm_8930_cam_vreg),
	.gpio_conf = &msm_8930_front_cam_gpio_conf,
	.csi_lane_params = &ov2720_csi_lane_params,
};

static struct msm_camera_sensor_info msm_camera_sensor_ov2720_data = {
	.sensor_name	= "ov2720",
	.pdata	= &msm_camera_csi_device_data[1],
	.flash_data	= &flash_ov2720,
	.sensor_platform_info = &sensor_board_info_ov2720,
	.csi_if	= 1,
	.camera_type = FRONT_CAMERA_2D,
	.sensor_type = BAYER_SENSOR,
};
#endif
#ifdef CONFIG_S5K3L1YX
static struct msm_camera_sensor_flash_data flash_s5k3l1yx = {
	.flash_type = MSM_CAMERA_FLASH_LED,
	//.flash_src = &msm_flash_src  wt remove 
};

static struct msm_camera_csi_lane_params s5k3l1yx_csi_lane_params = {
	.csi_lane_assign = 0xE4,
	.csi_lane_mask = 0xF,
};

static struct msm_camera_sensor_platform_info sensor_board_info_s5k3l1yx = {
	.mount_angle  = 90,
	.cam_vreg = msm_8930_cam_vreg,
	.num_vreg = ARRAY_SIZE(msm_8930_cam_vreg),
	.gpio_conf = &msm_8930_back_cam_gpio_conf,
	.csi_lane_params = &s5k3l1yx_csi_lane_params,
};

static struct msm_actuator_info msm_act_main_cam_2_info = {
	.board_info     = &msm_act_main_cam_i2c_info,
	.cam_name   = MSM_ACTUATOR_MAIN_CAM_2,
	.bus_id         = MSM_8930_GSBI4_QUP_I2C_BUS_ID,
	.vcm_pwd        = 0,
	.vcm_enable     = 0,
};

static struct msm_camera_sensor_info msm_camera_sensor_s5k3l1yx_data = {
	.sensor_name          = "s5k3l1yx",
	.pdata                = &msm_camera_csi_device_data[0],
	.flash_data           = &flash_s5k3l1yx,
	.sensor_platform_info = &sensor_board_info_s5k3l1yx,
	.csi_if               = 1,
	.camera_type          = BACK_CAMERA_2D,
	.sensor_type          = BAYER_SENSOR,
	.actuator_info    = &msm_act_main_cam_2_info,
};
#endif
static struct platform_device msm_camera_server = {
	.name = "msm_cam_server",
	.id = 0,
};

void __init msm8930_init_cam(void)
{
	msm_gpiomux_install(msm8930_cam_common_configs,
			ARRAY_SIZE(msm8930_cam_common_configs));

#if 0 //remove it .wt
	if (machine_is_msm8930_cdp()) {
		struct msm_camera_sensor_info *s_info;
		//s_info = &msm_camera_sensor_s5k3l1yx_data;
		s_info->sensor_platform_info->mount_angle = 0;
		msm_flash_src._fsrc.ext_driver_src.led_en =
			GPIO_CAM_GP_LED_EN1;
		msm_flash_src._fsrc.ext_driver_src.led_flash_en =
			GPIO_CAM_GP_LED_EN2;
#if defined(CONFIG_I2C) && (defined(CONFIG_GPIO_SX150X) || \
	defined(CONFIG_GPIO_SX150X_MODULE))
		msm_flash_src._fsrc.ext_driver_src.expander_info =
			cam_expander_info;
#endif
	}
#endif	

	platform_device_register(&msm_camera_server);
	platform_device_register(&msm8960_device_csiphy0);
	platform_device_register(&msm8960_device_csiphy1);
	platform_device_register(&msm8960_device_csid0);
	platform_device_register(&msm8960_device_csid1);
	platform_device_register(&msm8960_device_ispif);
	platform_device_register(&msm8960_device_vfe);
	platform_device_register(&msm8960_device_vpe);
}

#ifdef CONFIG_I2C
struct i2c_board_info msm8930_camera_i2c_boardinfo[] = {
#ifdef CONFIG_IMX074
	{
	I2C_BOARD_INFO("imx074", 0x1A),
	.platform_data = &msm_camera_sensor_imx074_data,
	},
#endif
#ifdef CONFIG_OV8825
	{
	I2C_BOARD_INFO("ov8825", 0x6C>>1),
	.platform_data = &msm_camera_sensor_ov8825_data,
	},
#endif
#ifdef CONFIG_OV8835
	{
	I2C_BOARD_INFO("ov8835", 0x6C>>1),
	.platform_data = &msm_camera_sensor_ov8835_data,
	},
#endif

#ifdef CONFIG_OV2720
	{
	I2C_BOARD_INFO("ov2720", 0x6C),
	.platform_data = &msm_camera_sensor_ov2720_data,
	},
#endif
#ifdef CONFIG_MT9N114
	{
	I2C_BOARD_INFO("mt9m114", 0x48),
	.platform_data = &msm_camera_sensor_mt9m114_data,
	},
#endif
#ifdef CONFIG_S5K3L1YX
	{
	I2C_BOARD_INFO("s5k3l1yx", 0x20),
	.platform_data = &msm_camera_sensor_s5k3l1yx_data,
	},
#endif

#ifdef CONFIG_OV5640
	{
	I2C_BOARD_INFO("ov5640", 0x78>>1),
	.platform_data = &msm_camera_sensor_ov5640_data,
	},	
#endif
#ifdef CONFIG_AR0542
	{
	I2C_BOARD_INFO("ar0542", 0x64),
	.platform_data = &msm_camera_sensor_ar0542_data,
	},
#endif
#ifdef CONFIG_HI542
	{
	I2C_BOARD_INFO("hi542", 0x5A),
	.platform_data = &msm_camera_sensor_hi542_data,
	},
#endif
#ifdef CONFIG_OV9740
	{
	I2C_BOARD_INFO("ov9740", 0x20>>1),
	.platform_data = &msm_camera_sensor_ov9740_data,
	},	
#endif
#ifdef CONFIG_LM3642
	{
	I2C_BOARD_INFO("lm3642", 0x63),
	},
#endif
#ifdef CONFIG_ADP1650
	{
	I2C_BOARD_INFO("adp1650", 0x30),
	},
#endif
#ifdef CONFIG_SP0A28
	{
	I2C_BOARD_INFO("sp0a28", 0x7A>>1),
	.platform_data = &msm_camera_sensor_sp0a28_data,			
	},
#endif
};

struct msm_camera_board_info msm8930_camera_board_info = {
	.board_info = msm8930_camera_i2c_boardinfo,
	.num_i2c_board_info = ARRAY_SIZE(msm8930_camera_i2c_boardinfo),
};
#endif
#endif
