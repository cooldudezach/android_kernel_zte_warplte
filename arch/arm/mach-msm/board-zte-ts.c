#include <linux/i2c.h>
#include <linux/i2c/sx150x.h>
#include <linux/gpio.h>
#include <linux/kernel.h>
#include <linux/platform_device.h>
#include <asm/mach-types.h>
#include <mach/msm_iomap.h>
#include <linux/delay.h>
#include <mach/board.h>

//#include <mach/board.h>
//#include <mach/vreg.h>
//#include "board-msm7x30-regulator.h"
#include <linux/regulator/consumer.h>
//#include <linux/regulator/gpio-regulator.h>


#ifdef CONFIG_TOUCHSCREEN_MXT224
#include <linux/input/atmel_qt602240.h>
#endif
#ifdef CONFIG_TOUCHSCREEN_SYNAPTICS
#include <linux/input/synaptics_rmi.h> 
#endif
#ifdef CONFIG_TOUCHSCREEN_FOCALTECH
#include <linux/input/focaltech_ft5x0x.h>
#endif
#ifdef CONFIG_TOUCHSCREEN_CYTTSP
#include <linux/input/touch_platform.h>

#endif



//
//  touchscreen gpio definition
//
#define VREG_VDD		"8038_l9"		// 3V
#define VREG_VBUS		"8038_lvs2"		// 1.8V
#define GPIO_TS_IRQ		11
#define GPIO_TS_RST		6
static struct regulator *vdd, *vbus;


//
//	touchscreen firmware file name
//
#if defined (CONFIG_MACH_WARP2)
#define FTC_FW_NAME ""
#define SYN_FW_NAME ""
#define ATM_FW_NAME ""
#elif defined (CONFIG_MACH_DANA)
#define FTC_FW_NAME ""
#define SYN_FW_NAME "PR1101200-s2202_zte_32313032.img"
#define ATM_FW_NAME ""
#elif defined (CONFIG_MACH_FROSTY)
#define FTC_FW_NAME ""
#define SYN_FW_NAME "PR1115996-s2202_32343032.img"
#define ATM_FW_NAME ""
#elif defined (CONFIG_MACH_COEUS) || defined (CONFIG_MACH_HERA)|| defined (CONFIG_MACH_OCEANUS)//P893A21 P893C01  P893V22
#define FTC_FW_NAME "Ver10_P893A21_FT5306_0x55_20130424_app.bin"
//#define SYN_FW_NAME "ZTE_P893A21_Success_PR1183396-s2202_32333039.img"
#define SYN_FW_NAME "ZTE_P893A21_Success_PR1183396-s2202_32333041.img"
#define ATM_FW_NAME ""
#elif defined(CONFIG_MACH_DEMETER)
#define FTC_FW_NAME ""
#define SYN_FW_NAME "T83_Truly_ZTE_PR1380113-s2202_32323033.img"
#define ATM_FW_NAME ""
#elif defined(CONFIG_MACH_WARPLTE)
#define FTC_FW_NAME "Ver0F_P188F03_ID0x5A_FT5326_20130605_app.bin"
#define SYN_FW_NAME "ZTE_P177F03_Laibao_PR1183396-s2202_32373039.img"
#define ATM_FW_NAME ""
#elif defined(CONFIG_MACH_APOLLO) || defined(CONFIG_MACH_NEX)
#define FTC_FW_NAME "V12_P893U10_ID0x53_FT5326_127C2_20130419_app.bin"
#define SYN_FW_NAME "ZTE_P893U10_Junda_PR1183396-s2202_32453035.img"
#define ATM_FW_NAME ""
#elif defined(CONFIG_MACH_IRIS)
#define FTC_FW_NAME ""
#define SYN_FW_NAME "Z753_Truly_PR1350177-s3203_zte_35323037.img"
#define ATM_FW_NAME ""
#elif defined(CONFIG_MACH_METIS)
#define FTC_FW_NAME "Ver14_N880E_ID0x55_20130604_app.bin"
#define SYN_FW_NAME ""
#define ATM_FW_NAME ""
#else
#define FTC_FW_NAME ""
#define SYN_FW_NAME ""
#define ATM_FW_NAME ""
#endif



//
//	touchscreen virtual key definition
//

#ifdef CONFIG_TOUCHSCREEN_VIRTUAL_KEYS
//#define CAP_TS_VKEY_CYPRESS "virtualkeys.cyttsp-i2c"
#define CAP_TS_VKEY_SYNAPTICS "virtualkeys.syna-touchscreen"
#define CAP_TS_VKEY_ATMEL "virtualkeys.atmel-touchscreen"
#define CAP_TS_VKEY_FTS "virtualkeys.Fts-touchscreen"

#if defined (CONFIG_MACH_APOLLO) 
#define SYNAPTICS_MAX_Y_POSITION	1800
static ssize_t cap_ts_vkeys_show(struct device *dev,	struct device_attribute *attr, char *buf)
{
	return sprintf(
		buf,__stringify(EV_KEY) ":" __stringify(KEY_BACK) ":85:846:100:70"
		":" __stringify(EV_KEY) ":" __stringify(KEY_HOME) ":240:846:100:70"
		":" __stringify(EV_KEY) ":" __stringify(KEY_MENU) ":390:846:100:70"
		"\n");	
}
#elif defined(CONFIG_MACH_NEX) //N800
#define SYNAPTICS_MAX_Y_POSITION	1800
static ssize_t cap_ts_vkeys_show(struct device *dev,	struct device_attribute *attr, char *buf)
{
	return sprintf(
		buf,__stringify(EV_KEY) ":" __stringify(KEY_BACK) ":85:846:100:70"
		":" __stringify(EV_KEY) ":" __stringify(KEY_HOME) ":240:856:100:70"
		":" __stringify(EV_KEY) ":" __stringify(KEY_MENU) ":390:846:100:70"
		"\n");	
}
#elif defined(CONFIG_MACH_DEMETER)
#define SYNAPTICS_MAX_Y_POSITION	1726
static ssize_t cap_ts_vkeys_show(struct device *dev,	struct device_attribute *attr, char *buf)
{
	return sprintf(
		buf,__stringify(EV_KEY) ":" __stringify(KEY_BACK) ":85:846:100:70"
		":" __stringify(EV_KEY) ":" __stringify(KEY_HOME) ":240:846:100:70"
		":" __stringify(EV_KEY) ":" __stringify(KEY_MENU) ":390:846:100:70"
		"\n");	
}

#elif defined(CONFIG_MACH_WARPLTE)
#define SYNAPTICS_MAX_Y_POSITION	1962
static ssize_t cap_ts_vkeys_show(struct device *dev,	struct device_attribute *attr, char *buf)
{
	return sprintf(
		buf,__stringify(EV_KEY) ":" __stringify(KEY_BACK) ":120:1360:200:140"
		":" __stringify(EV_KEY) ":" __stringify(KEY_HOME) ":360:1380:200:140"
		":" __stringify(EV_KEY) ":" __stringify(KEY_MENU) ":600:1360:200:140"
		"\n");	
}
#elif defined (CONFIG_MACH_GAEA)||defined(CONFIG_MACH_NESTOR)
#define SYNAPTICS_MAX_Y_POSITION	1962
static ssize_t cap_ts_vkeys_show(struct device *dev,	struct device_attribute *attr, char *buf)
{
	return sprintf(
		buf,__stringify(EV_KEY) ":" __stringify(KEY_BACK) ":120:1360:200:140"
		":" __stringify(EV_KEY) ":" __stringify(KEY_HOME) ":360:1360:200:140"
		":" __stringify(EV_KEY) ":" __stringify(KEY_MENU) ":600:1360:200:140"
		"\n");	
}

#elif defined (CONFIG_MACH_HERA)//P893C01 LCD 480*854   TP 1138*2159
#define SYNAPTICS_MAX_Y_POSITION	2007 //calculated from 2185
static ssize_t cap_ts_vkeys_show(struct device *dev,	struct device_attribute *attr, char *buf)
{
	return sprintf(
		buf,__stringify(EV_KEY) ":" __stringify(KEY_BACK) ":85:907:120:70"
		":" __stringify(EV_KEY) ":" __stringify(KEY_HOME) ":240:907:120:70"
		":" __stringify(EV_KEY) ":" __stringify(KEY_MENU) ":395:907:120:70"
		"\n");	
}

#elif defined(CONFIG_MACH_COEUS) || defined (CONFIG_MACH_OCEANUS)//P893A21 LCD 540*960  YUSHUN TP  1150*2185  ( LAIBAO TP 707*1267) P893V22  
#define SYNAPTICS_MAX_Y_POSITION	2007 //calculated from 2185
static ssize_t cap_ts_vkeys_show(struct device *dev,	struct device_attribute *attr, char *buf)
{
	return sprintf(
		buf,__stringify(EV_KEY) ":" __stringify(KEY_BACK) ":90:1020:150:100"
		":" __stringify(EV_KEY) ":" __stringify(KEY_HOME) ":270:1020:150:100"
		":" __stringify(EV_KEY) ":" __stringify(KEY_MENU) ":450:1020:150:100"
		"\n");	
}


#elif defined(CONFIG_MACH_BECKY)||defined(CONFIG_MACH_METIS)
#define SYNAPTICS_MAX_Y_POSITION	1728
static ssize_t cap_ts_vkeys_show(struct device *dev,	struct device_attribute *attr, char *buf)
{
	return sprintf(
		buf,__stringify(EV_KEY) ":" __stringify(KEY_BACK) ":85:846:100:70"
		":" __stringify(EV_KEY) ":" __stringify(KEY_HOME) ":240:846:100:70"
		":" __stringify(EV_KEY) ":" __stringify(KEY_MENU) ":390:846:100:70"
		"\n");	
}
//wly add P893D10
#elif defined(CONFIG_MACH_IRIS)
#define SYNAPTICS_MAX_Y_POSITION	2216
static ssize_t cap_ts_vkeys_show(struct device *dev,	struct device_attribute *attr, char *buf)
{
	return sprintf(
		buf,__stringify(EV_KEY) ":" __stringify(KEY_BACK) ":136:1325:190:80"
		":" __stringify(EV_KEY) ":" __stringify(KEY_HOME) ":365:1325:190:80"
		":" __stringify(EV_KEY) ":" __stringify(KEY_MENU) ":608:1325:190:80"
		"\n");	
}
#else
#define SYNAPTICS_MAX_Y_POSITION	1740
static ssize_t cap_ts_vkeys_show(struct device *dev,	struct device_attribute *attr, char *buf)
{
	return sprintf(
		buf,__stringify(EV_KEY) ":" __stringify(KEY_BACK) ":85:836:100:50"
		":" __stringify(EV_KEY) ":" __stringify(KEY_HOME) ":240:836:100:50"
		":" __stringify(EV_KEY) ":" __stringify(KEY_MENU) ":390:836:100:50"
		"\n");	
}

#endif

static struct device_attribute cap_ts_device_attr[] = {
#if defined(CONFIG_TOUCHSCREEN_FOCALTECH)
	{
		.attr = {
			.name = CAP_TS_VKEY_FTS,
			.mode = S_IRUGO,
		},
		.show	= &cap_ts_vkeys_show,
		.store	= NULL,
	},
#endif
#if defined(CONFIG_TOUCHSCREEN_SYNAPTICS)
{
	.attr = {
		.name = CAP_TS_VKEY_SYNAPTICS,
		.mode = S_IRUGO,
	},
	.show	= &cap_ts_vkeys_show,
	.store	= NULL,
},
#endif
#if defined(CONFIG_TOUCHSCREEN_MXT224)
	{
		.attr = {
			.name = CAP_TS_VKEY_ATMEL,
			.mode = S_IRUGO,
		},
		.show	= &cap_ts_vkeys_show,
		.store	= NULL,
	},
#endif
};

struct kobject *android_touch_kobj;
static int cap_ts_vkeys_init(void)
{
	int rc,i;
	struct kobject * cap_ts_properties_kobj=NULL;

	cap_ts_properties_kobj = kobject_create_and_add("board_properties", NULL);
	if (cap_ts_properties_kobj == NULL) {
		printk("%s: subsystem_register failed\n", __func__);
		rc = -ENOMEM;
		return rc;
	}
	android_touch_kobj = cap_ts_properties_kobj;

	for ( i=0; i < ARRAY_SIZE(cap_ts_device_attr); i++ ){
		rc = sysfs_create_file(cap_ts_properties_kobj, &cap_ts_device_attr[i].attr);
		if (rc) {
			printk("%s: sysfs_create_file failed\n", __func__);
			return rc;
		}
	}

	return 0;
}
#else

#define SYNAPTICS_MAX_Y_POSITION	0

#endif


//
//	touchscreen gpio functions
//
static void touchscreen_irq( int hl, bool io_flag )
{
	//io_flag: true- default input, false - output

	if ( io_flag == true )
		gpio_direction_input(GPIO_TS_IRQ);
	else
		gpio_direction_output(GPIO_TS_IRQ, hl);

	return;
}

static void touchscreen_reset( int hl )
{
	gpio_direction_output(GPIO_TS_RST, hl);
	return;
}


static void touchscreen_power( int on )
{
#if 1
	int rc = -EINVAL;

	if ( !vdd || !vbus )
		return;
	
	if (on){
		rc = regulator_enable(vdd);
		if (rc) {
			pr_err("vdd enable failed\n");
			return;
		}
		rc = regulator_enable(vbus);
		if (rc) {
			pr_err("vbus enable failed\n");
			return;
		}
	}
	else 
	{
		rc = regulator_disable(vdd);
		if (rc) {
			pr_err("vdd disable failed\n");
			return;
		}
		rc = regulator_disable(vbus);
		if (rc) {
			pr_err("vbus disable failed\n");
			return;
		}
	}
#endif
	return;
}


static int touchscreen_gpio_init(struct device *dev, int flag)
{
	int ret = -EINVAL;


	//init
	if ( flag == 1 )
	{
	#if 1
		vdd = vbus = NULL;

		vdd = regulator_get(NULL, VREG_VDD);
		if (!(vdd)) {
			pr_err("%s get failed\n", VREG_VDD);
			return -1;
		}
		if ( regulator_set_voltage(vdd, 3000000,3000000) ){
			pr_err("%s set failed\n", VREG_VDD);
			return -1;
		}

		vbus =regulator_get(NULL, VREG_VBUS);
		if (!(vbus)) {
			pr_err("%s get failed\n", VREG_VBUS);
			return -1;
		}
/*
		if ( regulator_set_voltage(vbus, 1800000,1800000)) {
			pr_err(" %s set failed\n", VREG_VBUS);
			return -1;
		}
*/
#endif
		ret = gpio_request(GPIO_TS_RST, "touch voltage");
		if (ret){
			pr_err(" gpio %d request is error!\n", GPIO_TS_RST);
			return -1;
		}

		ret = gpio_request(GPIO_TS_IRQ, "touch voltage");
		if (ret){
			pr_err("gpio %d request is error!\n", GPIO_TS_IRQ);
			return -1;
		}

	}


	//deinit
	if ( flag == 0)
	{
	#if 0
		regulator_put(vdd);
		regulator_put(vbus);
	#endif
		gpio_free(GPIO_TS_IRQ);
		gpio_free(GPIO_TS_RST);
	}

	return 0;

}


//
// i2c device definition
//

#if defined (CONFIG_TOUCHSCREEN_SYNAPTICS)
static struct synaptics_rmi_data synaptics_ts_data = {
	.gpio_init = touchscreen_gpio_init,
	.power	= touchscreen_power,
	.reset	= touchscreen_reset,
	.irq = touchscreen_irq,
	.max_y_position = SYNAPTICS_MAX_Y_POSITION,	// 0 - no vkey, do nothing
	.fwfile = SYN_FW_NAME,
};
#endif

#if defined (CONFIG_TOUCHSCREEN_FOCALTECH)
static struct focaltech_ts_platform_data focaltech_ts_data = {
	.gpio_init = touchscreen_gpio_init,
	.power	= touchscreen_power,
	.reset	= touchscreen_reset,
	.irq = touchscreen_irq,
	.fwfile	 = FTC_FW_NAME,
};
#endif

#if defined (CONFIG_TOUCHSCREEN_MXT224)
static struct atmel_platform_data atmel_ts_data = {
	.gpio_init = touchscreen_gpio_init,
	.power	= touchscreen_power,
	.reset	= touchscreen_reset,
	.irq = touchscreen_irq,
	.fwfile	 = ATM_FW_NAME,
};
#endif

#ifdef CONFIG_TOUCHSCREEN_CYTTSP
/* default is to build for Txx3xx */
#if !defined(CY_USE_GEN2) && !defined(CY_USE_GEN3)
#define CY_USE_GEN3
#elif defined(CY_USE_GEN2) && defined(CY_USE_GEN3)
#undef CY_USE_GEN2
#endif

/* Use the following define if including an autoload firmware image
#define CY_USE_AUTOLOAD_FW
 */

#define CY_I2C_NAME     "cysp-touchscreen"
#define CY_I2C_TCH_ADR	0x24
#define CY_I2C_LDR_ADR	0x24
#ifdef CY_USE_GEN2
#define CY_MAXX 170
#define CY_MAXY 310
#endif /* --CY_USE_GEN2 */
#ifdef CY_USE_GEN3
#define CY_MAXX 880
#define CY_MAXY 1280
#endif /* --CY_USE_GEN3 */

#if 0
static int cyttsp3_i2c_init(void)
{
#if 0
	return cyttsp3_vkey_init(CY_I2C_VKEY_NAME);
#else
	return 0;
#endif
}
#endif

static int cyttsp3_hw_reset(void)
{
	int retval = 0;

	retval = gpio_request(GPIO_TS_RST, NULL);
	if (retval < 0) {
		pr_err("%s: Fail request RST pin r=%d\n", __func__, retval);
		pr_err("%s: Try free RST gpio=%d\n", __func__,
			GPIO_TS_RST);
		gpio_free(GPIO_TS_RST);
		retval = gpio_request(GPIO_TS_RST, NULL);
		if (retval < 0) {
			pr_err("%s: Fail 2nd request RST pin r=%d\n", __func__,
				retval);
		}
	}

	if (!(retval < 0)) {
		pr_info("%s: strobe RST(%d) pin\n", __func__,
			GPIO_TS_RST);
		gpio_set_value(GPIO_TS_RST, 1);
		msleep(20);
		gpio_set_value(GPIO_TS_RST, 0);
		msleep(40);
		gpio_set_value(GPIO_TS_RST, 1);
		msleep(20);
		gpio_free(GPIO_TS_RST);
	}

	return retval;
}

#define CY_WAKE_DFLT                99	/* causes wake strobe on INT line
					 * in sample board configuration
					 * platform data->hw_recov() function
					 */
static int cyttsp3_hw_recov(int on)
{
	int retval = 0;

	switch (on) {
	case 0:
		cyttsp3_hw_reset();
		retval = 0;
		break;
	case CY_WAKE_DFLT:
		retval = gpio_request(GPIO_TS_IRQ, NULL);
		if (retval < 0) {
			pr_err("%s: Fail request IRQ pin r=%d\n",
				__func__, retval);
			pr_err("%s: Try free IRQ gpio=%d\n", __func__,
				GPIO_TS_IRQ);
			gpio_free(GPIO_TS_IRQ);
			retval = gpio_request(GPIO_TS_IRQ, NULL);
			if (retval < 0) {
				pr_err("%s: Fail 2nd request IRQ pin r=%d\n",
					__func__, retval);
			}
		}

		if (!(retval < 0)) {
			retval = gpio_direction_output
				(GPIO_TS_IRQ, 0);
			if (retval < 0) {
				pr_err("%s: Fail switch IRQ pin to OUT r=%d\n",
					__func__, retval);
			} else {
				udelay(2000);
				retval = gpio_direction_input
					(GPIO_TS_IRQ);
				if (retval < 0) {
					pr_err("%s: Fail switch IRQ pin to IN"
						" r=%d\n", __func__, retval);
				}
			}
			gpio_free(GPIO_TS_IRQ);
		}
		break;
	default:
		retval = -ENOSYS;
		break;
	}

	return retval;
}

static int cyttsp3_irq_stat(void)
{
	int irq_stat = 0;
	int retval = 0;

	retval = gpio_request(GPIO_TS_IRQ, NULL);
	if (retval < 0) {
		pr_err("%s: Fail request IRQ pin r=%d\n", __func__, retval);
		pr_err("%s: Try free IRQ gpio=%d\n", __func__,
			GPIO_TS_IRQ);
		gpio_free(GPIO_TS_IRQ);
		retval = gpio_request(GPIO_TS_IRQ, NULL);
		if (retval < 0) {
			pr_err("%s: Fail 2nd request IRQ pin r=%d\n",
				__func__, retval);
		}
	}

	if (!(retval < 0)) {
		irq_stat = gpio_get_value(GPIO_TS_IRQ);
		gpio_free(GPIO_TS_IRQ);
	}

	return irq_stat;
}

#include <linux/input.h>
#define CY_ABS_MIN_X 0
#define CY_ABS_MIN_Y 0
#define CY_ABS_MIN_P 0
#define CY_ABS_MIN_W 0
#ifdef CY_USE_GEN2
#define CY_ABS_MIN_T 0
#endif /* --CY_USE_GEN2 */
#ifdef CY_USE_GEN3
#define CY_ABS_MIN_T 1
#endif /* --CY_USE_GEN3 */
#define CY_ABS_MAX_X CY_MAXX
#define CY_ABS_MAX_Y CY_MAXY
#define CY_ABS_MAX_P 255
#define CY_ABS_MAX_W 255
#ifdef CY_USE_GEN2
#define CY_ABS_MAX_T 1
#endif /* --CY_USE_GEN2 */
#ifdef CY_USE_GEN3
#define CY_ABS_MAX_T 14
#endif /* --CY_USE_GEN3 */
#define CY_IGNORE_VALUE 0xFFFF

/* do not arbitrarity set the feature register which includes the CA bit
 * at startup, the feature bits tell what is available
 * to enable a feature; write a 1 to the available bit.
static const uint8_t cyttsp_op_regs[] = {0, 0, 0, 0x08, 0x01};
 */
static const uint8_t cyttsp_op_regs[] = {0, 0, 0, 0x08};
static const uint8_t cyttsp_si_regs[] = {0, 0, 0, 0, 0, 0, 0x00, 0xFF, 0x0A};
static const uint8_t cyttsp_bl_keys[] = {0, 1, 2, 3, 4, 5, 6, 7};
static const char cyttsp_use_name[] = CY_I2C_NAME;

static struct touch_settings cyttsp_sett_op_regs = {
	.data = (uint8_t *)&cyttsp_op_regs[0],
	.size = sizeof(cyttsp_op_regs),
	.tag = 3,
};

static struct touch_settings cyttsp_sett_si_regs = {
	.data = (uint8_t *)&cyttsp_si_regs[0],
	.size = sizeof(cyttsp_si_regs),
	.tag = 6,
};

static struct touch_settings cyttsp_sett_bl_keys = {
	.data = (uint8_t *)&cyttsp_bl_keys[0],
	.size = sizeof(cyttsp_bl_keys),
	.tag = 0,
};

#ifdef CY_USE_AUTOLOAD_FW
#include "cyttsp3_img.h"
static struct touch_firmware cyttsp3_firmware = {
	.img = cyttsp3_img,
	.size = sizeof(cyttsp3_img),
	.ver = cyttsp3_ver,
	.vsize = sizeof(cyttsp3_ver),
};
#else
static struct touch_firmware cyttsp3_firmware = {
	.img = NULL,
	.size = 0,
	.ver = NULL,
	.vsize = 0,
};
#endif

#if 0 /* Use this block for pre-Gingerbread integrations */
static const uint16_t cyttsp3_abs[] = {
	ABS_MT_POSITION_X, CY_ABS_MIN_X, CY_ABS_MAX_X, 0, 0,
	ABS_MT_POSITION_Y, CY_ABS_MIN_Y, CY_ABS_MAX_Y, 0, 0,
	ABS_MT_TOUCH_MAJOR, CY_ABS_MIN_P, CY_ABS_MAX_P, 0, 0,
	CY_IGNORE_VALUE, CY_ABS_MIN_W, CY_ABS_MAX_W, 0, 0,
	ABS_MT_TRACKING_ID, CY_ABS_MIN_T, CY_ABS_MAX_T, 0, 0,
};
#else /* Use this block for Gingerbread and later integrations */
static const uint16_t cyttsp3_abs[] = {
	ABS_MT_POSITION_X, CY_ABS_MIN_X, CY_ABS_MAX_X, 0, 0,
	ABS_MT_POSITION_Y, CY_ABS_MIN_Y, CY_ABS_MAX_Y, 0, 0,
	ABS_MT_PRESSURE, CY_ABS_MIN_P, CY_ABS_MAX_P, 0, 0,
	ABS_MT_TOUCH_MAJOR, CY_ABS_MIN_W, CY_ABS_MAX_W, 0, 0,
	ABS_MT_TRACKING_ID, CY_ABS_MIN_T, CY_ABS_MAX_T, 0, 0,
};
#endif

struct touch_framework cyttsp3_framework = {
	.abs = (uint16_t *)&cyttsp3_abs[0],
	.size = ARRAY_SIZE(cyttsp3_abs),
	.enable_vkeys = 1,
};

struct touch_platform_data cyttsp3_i2c_touch_platform_data = {
	.sett = {
		NULL,
		&cyttsp_sett_op_regs,
		&cyttsp_sett_si_regs,
		&cyttsp_sett_bl_keys,
	},
	.gpio_init = touchscreen_gpio_init,
	.power	= touchscreen_power,
	.fw = &cyttsp3_firmware,
	.frmwrk = &cyttsp3_framework,
	.addr = {CY_I2C_TCH_ADR, CY_I2C_LDR_ADR},
	.flags = 0x00,
	.hw_reset = cyttsp3_hw_reset,
	.hw_recov = cyttsp3_hw_recov,
	.irq_stat = cyttsp3_irq_stat,
};
#endif

static struct i2c_board_info i2c_touch_devices[] = {
#ifdef CONFIG_TOUCHSCREEN_FOCALTECH	
	{				
		I2C_BOARD_INFO("ft5x0x_ts", 0x3E ),
		.irq = MSM_GPIO_TO_INT(GPIO_TS_IRQ),
		.platform_data = &focaltech_ts_data,
	},	
#endif
#ifdef CONFIG_TOUCHSCREEN_SYNAPTICS
	{
		I2C_BOARD_INFO("syna-touchscreen", 0x22 ),
		.irq = MSM_GPIO_TO_INT(GPIO_TS_IRQ),
		.platform_data = &synaptics_ts_data,
	},
#endif
#ifdef CONFIG_TOUCHSCREEN_MXT224
	{    
		I2C_BOARD_INFO("atmel_qt602240", 0x4a ),
		.platform_data = &atmel_ts_data,
		.irq = MSM_GPIO_TO_INT(GPIO_TS_IRQ),
	},
#endif
#ifdef CONFIG_TOUCHSCREEN_CYTTSP
{
	I2C_BOARD_INFO(CY_I2C_NAME, CY_I2C_TCH_ADR),
	.irq = MSM_GPIO_TO_INT(GPIO_TS_IRQ),
	.platform_data = &cyttsp3_i2c_touch_platform_data,
},


#endif
};


void __init msm8960_ts_init(int bus)
{

#ifdef CONFIG_TOUCHSCREEN_VIRTUAL_KEYS
	cap_ts_vkeys_init();
#endif

	i2c_register_board_info(bus, 
		i2c_touch_devices,ARRAY_SIZE(i2c_touch_devices));

	return ;
}

