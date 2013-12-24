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

DEFINE_MUTEX(ar0542_eeprom_mutex);
static struct msm_eeprom_ctrl_t ar0542_eeprom_t;

static const struct i2c_device_id ar0542_eeprom_i2c_id[] = {
	{"ar0542_eeprom", (kernel_ulong_t)&ar0542_eeprom_t},
	{ }
};

static struct i2c_driver ar0542_eeprom_i2c_driver = {
	.id_table = ar0542_eeprom_i2c_id,
	.probe  = msm_eeprom_i2c_probe_ar0542,
	.remove = __exit_p(ar0542_eeprom_i2c_remove),
	.driver = {
		.name = "ar0542_eeprom",
	},
};

static int __init ar0542_eeprom_i2c_add_driver(void)
{
	int rc = 0;
	rc = i2c_add_driver(ar0542_eeprom_t.i2c_driver);
	return rc;
}

static struct v4l2_subdev_core_ops ar0542_eeprom_subdev_core_ops = {
	.ioctl = msm_eeprom_subdev_ioctl,
};

static struct v4l2_subdev_ops ar0542_eeprom_subdev_ops = {
	.core = &ar0542_eeprom_subdev_core_ops,
};
static uint8_t ar0542_wbcalib_data[4];
static struct msm_calib_wb ar0542_wb_data;
static uint8_t ar0542_module_info_data[8];

  struct sensor_module_info_t ar0542_module_info;/*HT for actuator compatiable */


static struct msm_camera_eeprom_info_t ar0542_calib_supp_info = {
	{FALSE, 0, 0, 1},
	{TRUE, 8, 0, 128},
	{FALSE, 0, 0, 1},
	{FALSE, 0, 0, 1},
	{FALSE, 0, 0, 1},
};
static struct msm_camera_eeprom_data_t ar0542_eeprom_data_mod_info = {
	&ar0542_module_info, 
	sizeof(struct sensor_module_info_t),
};
static struct msm_camera_eeprom_read_t ar0542_eeprom_read_tbl[] = {
	{0x3802, &ar0542_module_info_data[0], 8, 0},
	{0x38d6, &ar0542_wbcalib_data[0], 4, 0},
};

static void ar0542_get_module_info(void)
{
	ar0542_module_info.module_vendor = ar0542_module_info_data[1] | (uint16_t)ar0542_module_info_data[0]<<8;;
	ar0542_module_info.actuator = ar0542_module_info_data[3] | (uint16_t)ar0542_module_info_data[2]<<8;;
	ar0542_module_info.driver_ic = ar0542_module_info_data[5] | (uint16_t)ar0542_module_info_data[4]<<8;;
	ar0542_module_info.len_id = ar0542_module_info_data[7] | (uint16_t)ar0542_module_info_data[6]<<8;;	 
	pr_err("module_vendor: 0x%x, actuator: 0x%x, driver_ic: 0x%x len_id: 0x%x \n",ar0542_module_info.module_vendor, ar0542_module_info.actuator, ar0542_module_info.driver_ic,ar0542_module_info.len_id);		 
}

static struct msm_camera_eeprom_data_t ar0542_eeprom_data_tbl[] = {
	{&ar0542_wb_data, sizeof(struct msm_calib_wb)},
};

static void ar0542_format_wbdata(void)
{

	ar0542_wb_data.r_over_g = (uint16_t)((ar0542_wbcalib_data[0] << 8) |	ar0542_wbcalib_data[1]);
	ar0542_wb_data.b_over_g = (uint16_t)((ar0542_wbcalib_data[2] << 8) |	ar0542_wbcalib_data[3]);	
	ar0542_wb_data.gr_over_gb = 128;
	if((ar0542_wb_data.r_over_g>=128)||(ar0542_wb_data.b_over_g>=128)||(ar0542_wb_data.r_over_g<=0)||(ar0542_wb_data.b_over_g<=0))
	{
	ar0542_wb_data.r_over_g = 84;
	ar0542_wb_data.b_over_g = 74;
	ar0542_wb_data.gr_over_gb = 128;
	}
	pr_err("ar0542_wb_data.r_over_g=: 0x%x,ar0542_wb_data.b_over_g=: 0x%x,ar0542_wb_data.gr_over_gb=: 0x%x\n",ar0542_wb_data.r_over_g,ar0542_wb_data.b_over_g,ar0542_wb_data.gr_over_gb);		 	

}


void ar0542_format_calibrationdata(void)
{
	ar0542_get_module_info();
	ar0542_format_wbdata();
}
static struct msm_eeprom_ctrl_t ar0542_eeprom_t = {
	.i2c_driver = &ar0542_eeprom_i2c_driver,
	.i2c_addr = 0x6C,
	.eeprom_v4l2_subdev_ops = &ar0542_eeprom_subdev_ops,

	.i2c_client = {
		.addr_type = MSM_CAMERA_I2C_WORD_ADDR,
	},

	.eeprom_mutex = &ar0542_eeprom_mutex,
	.func_tbl = {
		.eeprom_init = NULL,
		.eeprom_release = NULL,
		.eeprom_get_info = msm_camera_eeprom_get_info,
		.eeprom_get_data = msm_camera_eeprom_get_data,
		.eeprom_set_dev_addr = NULL,
		.eeprom_format_data = ar0542_format_calibrationdata,
	},
	.info = &ar0542_calib_supp_info,
	.info_size = sizeof(struct msm_camera_eeprom_info_t),
	.read_tbl = ar0542_eeprom_read_tbl,
	.read_tbl_size = ARRAY_SIZE(ar0542_eeprom_read_tbl),
	.data_tbl = ar0542_eeprom_data_tbl,
	.data_tbl_size = ARRAY_SIZE(ar0542_eeprom_data_tbl),
        .data_mod_info = &ar0542_eeprom_data_mod_info,
};

subsys_initcall(ar0542_eeprom_i2c_add_driver);
MODULE_DESCRIPTION("AR0542 EEPROM");
MODULE_LICENSE("GPL v2");
