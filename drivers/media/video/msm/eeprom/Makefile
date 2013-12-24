GCC_VERSION      := $(shell $(CONFIG_SHELL) $(PWD)/scripts/gcc-version.sh $(CROSS_COMPILE)gcc)
EXTRA_CFLAGS += -Idrivers/media/video/msm/io
obj-$(CONFIG_MSM_EEPROM) += msm_camera_eeprom.o
obj-$(CONFIG_IMX074_EEPROM) += imx074_eeprom.o
obj-$(CONFIG_IMX091_EEPROM) += imx091_eeprom.o
obj-$(CONFIG_OV8830_EEPROM) += ov8830_eeprom.o
obj-$(CONFIG_HI542_EEPROM) += hi542_eeprom.o
obj-$(CONFIG_AR0542_EEPROM) += ar0542_eeprom.o