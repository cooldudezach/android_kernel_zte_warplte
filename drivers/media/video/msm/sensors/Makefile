GCC_VERSION      := $(shell $(CONFIG_SHELL) $(PWD)/scripts/gcc-version.sh $(CROSS_COMPILE)gcc)
EXTRA_CFLAGS += -Idrivers/media/video/msm
EXTRA_CFLAGS += -Idrivers/media/video/msm/io
EXTRA_CFLAGS += -Idrivers/media/video/msm/eeprom
EXTRA_CFLAGS += -Idrivers/media/video/msm/csi
obj-$(CONFIG_MSM_CAMERA_SENSOR) += msm_sensor_common.o msm_sensor.o msm_sensor_bayer.o msm_sensor_init.o

#add by wt for camera info
obj-$(CONFIG_SENSOR_INFO) += msm_sensorinfo.o

obj-$(CONFIG_OV5647) += ov5647_v4l2.o
#obj-$(CONFIG_OV8825) += ov8825_v4l2.o
obj-$(CONFIG_IMX074) += imx074_v4l2.o
obj-$(CONFIG_S5K3L1YX) += s5k3l1yx.o
obj-$(CONFIG_OV8830) += ov8830_v4l2.o
obj-$(CONFIG_OV2720) += ov2720.o
obj-$(CONFIG_MT9M114) += mt9m114_v4l2.o
obj-$(CONFIG_S5K4E1) += s5k4e1_v4l2.o
obj-$(CONFIG_MT9E013) += mt9e013_v4l2.o
obj-$(CONFIG_WEBCAM_OV9726) += ov9726_v4l2.o
obj-$(CONFIG_OV7692) += ov7692_v4l2.o
obj-$(CONFIG_VX6953) += vx6953.o
#add by yanwei for ov5640
obj-$(CONFIG_OV5640) += ov5640_v4l2.o
#add by yanwei for ov9740
obj-$(CONFIG_OV9740) += ov9740_v4l2.o
#add by yanwei for sp0a28
obj-$(CONFIG_SP0A28) += sp0a28_v4l2.o
