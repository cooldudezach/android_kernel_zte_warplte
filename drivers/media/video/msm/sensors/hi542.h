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
#define HI542_SENSOR_NAME "hi542"
DEFINE_MSM_MUTEX(hi542_mut);

static struct msm_sensor_ctrl_t hi542_s_ctrl;

static struct v4l2_subdev_info hi542_subdev_info[] = {
	{
		.code   = V4L2_MBUS_FMT_SBGGR10_1X10,
		.colorspace = V4L2_COLORSPACE_JPEG,
		.fmt    = 1,
		.order    = 0,
	},
	/* more can be supported, to be added later */
};

static struct msm_sensor_id_info_t hi542_id_info = {
	.sensor_id_reg_addr = 0x0004,
	.sensor_id = 0x00b1,
};

static enum msm_camera_vreg_name_t hi542_veg_seq[] = {
    //CAM_VIO,
    CAM_VDIG,	
    //CAM_VAF,
    //CAM_VANA,
};

static struct msm_camera_power_seq_t hi542_power_seq[] = {
	{REQUEST_GPIO, 0},
	{REQUEST_VREG, 0},
	{ENABLE_VREG, 0},
	{ENABLE_GPIO, 0},
	{CONFIG_CLK, 1},
	{CONFIG_I2C_MUX, 0},
};

static const struct i2c_device_id hi542_i2c_id[] = {
	{HI542_SENSOR_NAME, (kernel_ulong_t)&hi542_s_ctrl},
	{ }
};

static struct i2c_driver hi542_i2c_driver = {
	.id_table = hi542_i2c_id,
	.probe  = msm_sensor_bayer_i2c_probe,
	.driver = {
		.name = HI542_SENSOR_NAME,
	},
};

static struct msm_camera_i2c_client hi542_sensor_i2c_client = {
	.addr_type = MSM_CAMERA_I2C_WORD_ADDR,
};

static struct v4l2_subdev_core_ops hi542_subdev_core_ops = {
	.ioctl = msm_sensor_bayer_subdev_ioctl,
	.s_power = msm_sensor_bayer_power,
};

static struct v4l2_subdev_video_ops hi542_subdev_video_ops = {
	.enum_mbus_fmt = msm_sensor_bayer_v4l2_enum_fmt,
};

static struct v4l2_subdev_ops hi542_subdev_ops = {
	.core = &hi542_subdev_core_ops,
	.video  = &hi542_subdev_video_ops,
};

static struct msm_sensor_fn_t hi542_func_tbl = {
	.sensor_start_stream = msm_sensor_bayer_stream_on,
	.sensor_stop_stream = msm_sensor_bayer_stream_off,
	.sensor_config = msm_sensor_bayer_config,
	.sensor_power_up = msm_sensor_bayer_power_up,
	.sensor_power_down = msm_sensor_bayer_power_down,
	.sensor_get_csi_params = msm_sensor_bayer_get_csi_params,
};

static struct msm_sensor_ctrl_t hi542_s_ctrl = {
	.sensor_i2c_client = &hi542_sensor_i2c_client,
	.sensor_i2c_addr = 0x40,
	.vreg_seq = hi542_veg_seq,
	.num_vreg_seq = ARRAY_SIZE(hi542_veg_seq),
	.power_seq = &hi542_power_seq[0],
	.num_power_seq = ARRAY_SIZE(hi542_power_seq),
	.sensor_id_info = &hi542_id_info,
	.msm_sensor_mutex = &hi542_mut,
	.sensor_v4l2_subdev_info = hi542_subdev_info,
	.sensor_v4l2_subdev_info_size = ARRAY_SIZE(hi542_subdev_info),
	.sensor_v4l2_subdev_ops = &hi542_subdev_ops,
	.func_tbl = &hi542_func_tbl,
	.clk_rate = MSM_SENSOR_MCLK_24HZ,
};
