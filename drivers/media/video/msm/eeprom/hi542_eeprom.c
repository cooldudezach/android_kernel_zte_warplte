/* Copyright (c) 2011-2012, Code Aurora Forum. All rights reserved.
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

#include <linux/module.h>
#include "msm_camera_eeprom.h"
#include "msm_camera_i2c.h"
#include "../sensors/msm_sensor_common.h" /*HT for actuator compatiable */

DEFINE_MUTEX(hi542_eeprom_mutex);
static struct msm_eeprom_ctrl_t hi542_eeprom_t;

static const struct i2c_device_id hi542_eeprom_i2c_id[] = {
	{"hi542_eeprom", (kernel_ulong_t)&hi542_eeprom_t},
	{ }
};

static struct i2c_driver hi542_eeprom_i2c_driver = {
	.id_table = hi542_eeprom_i2c_id,
	.probe  = msm_eeprom_i2c_probe_hi542,
	.remove = __exit_p(hi542_eeprom_i2c_remove),
	.driver = {
		.name = "hi542_eeprom",
	},
};

static int __init hi542_eeprom_i2c_add_driver(void)
{
	int rc = 0;
	rc = i2c_add_driver(hi542_eeprom_t.i2c_driver);
	return rc;
}

static struct v4l2_subdev_core_ops hi542_eeprom_subdev_core_ops = {
	.ioctl = msm_eeprom_subdev_ioctl,
};

static struct v4l2_subdev_ops hi542_eeprom_subdev_ops = {
	.core = &hi542_eeprom_subdev_core_ops,
};
static uint8_t hi542_wbcalib_data[2];
static struct msm_calib_wb hi542_wb_data;
static uint8_t hi542_module_info_data[4];

 struct sensor_module_info_t hi542_module_info; /*HT for actuator compatiable */

static struct msm_camera_eeprom_info_t hi542_calib_supp_info = {
	{FALSE, 0, 0, 1},
	{TRUE, 3, 0, 128},
	{FALSE, 0, 0, 1},
	{FALSE, 0, 0, 1},
	{FALSE, 0, 0, 1},
};
static struct msm_camera_eeprom_data_t hi542_eeprom_data_mod_info = {
	&hi542_module_info, 
	sizeof(struct sensor_module_info_t),
};
static struct msm_camera_eeprom_read_t hi542_eeprom_read_tbl[] = {
	{0x0722, &hi542_module_info_data[0], 4, 0},
	{0x0722, &hi542_wbcalib_data[0], 2, 0},
};

static void hi542_get_module_info(void)
{
	hi542_module_info.module_vendor = hi542_module_info_data[0];
	hi542_module_info.actuator = hi542_module_info_data[1];
	hi542_module_info.driver_ic = hi542_module_info_data[2];
	hi542_module_info.len_id = hi542_module_info_data[3];

	pr_err("module_vendor: 0x%x, actuator: 0x%x, driver_ic: 0x%x len_id: 0x%x \n",hi542_module_info.module_vendor, hi542_module_info.actuator, hi542_module_info.driver_ic,hi542_module_info.len_id);		 
}

static struct msm_camera_eeprom_data_t hi542_eeprom_data_tbl[] = {
	{&hi542_wb_data, sizeof(struct msm_calib_wb)},
};

static void hi542_format_wbdata(void)
{
	hi542_wb_data.r_over_g = (uint16_t)(hi542_wbcalib_data[0]);
	hi542_wb_data.b_over_g = (uint16_t)(hi542_wbcalib_data[1]);
	hi542_wb_data.gr_over_gb = 128;
	if((hi542_wb_data.r_over_g>=128)||(hi542_wb_data.b_over_g>=128)||(hi542_wb_data.r_over_g<=0)||(hi542_wb_data.b_over_g<=0))
	{
	hi542_wb_data.r_over_g = 85;
	hi542_wb_data.b_over_g = 76;
	hi542_wb_data.gr_over_gb = 128;
	}
	pr_err("hi542_wb_data.r_over_g=: 0x%x,hi542_wb_data.b_over_g=: 0x%x,hi542_wb_data.gr_over_gb=: 0x%x\n",hi542_wb_data.r_over_g,hi542_wb_data.b_over_g,hi542_wb_data.gr_over_gb);		 	
}


void hi542_format_calibrationdata(void)
{
	hi542_get_module_info();
	hi542_format_wbdata();
}
static struct msm_eeprom_ctrl_t hi542_eeprom_t = {
	.i2c_driver = &hi542_eeprom_i2c_driver,
	.i2c_addr = 0x40,
	.eeprom_v4l2_subdev_ops = &hi542_eeprom_subdev_ops,

	.i2c_client = {
		.addr_type = MSM_CAMERA_I2C_WORD_ADDR,
	},

	.eeprom_mutex = &hi542_eeprom_mutex,
	.func_tbl = {
		.eeprom_init = NULL,
		.eeprom_release = NULL,
		.eeprom_get_info = msm_camera_eeprom_get_info,
		.eeprom_get_data = msm_camera_eeprom_get_data,
		.eeprom_set_dev_addr = NULL,
		.eeprom_format_data = hi542_format_calibrationdata,
	},
	.info = &hi542_calib_supp_info,
	.info_size = sizeof(struct msm_camera_eeprom_info_t),
	.read_tbl = hi542_eeprom_read_tbl,
	.read_tbl_size = ARRAY_SIZE(hi542_eeprom_read_tbl),
	.data_tbl = hi542_eeprom_data_tbl,
	.data_tbl_size = ARRAY_SIZE(hi542_eeprom_data_tbl),
	.data_mod_info = &hi542_eeprom_data_mod_info,
};

subsys_initcall(hi542_eeprom_i2c_add_driver);
MODULE_DESCRIPTION("HI542 EEPROM");
MODULE_LICENSE("GPL v2");
