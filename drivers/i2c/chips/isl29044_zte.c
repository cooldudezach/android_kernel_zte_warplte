/******************************************************************************
 * isl29044.h - Linux kernel module for Intersil isl29044 ambient light sensor
 *				and proximity sensor
 *
 * Copyright 2008-2012 Intersil Inc..
 *
 * DESCRIPTION:
 *	- This is the linux driver for isl29044.
 *		Kernel version 3.0.8
 *
 * modification history
 * --------------------
 * v1.0   2010/04/06, Shouxian Chen(Simon Chen) create this file
 * v1.1   2012/06/05, Shouxian Chen(Simon Chen) modified for Android 4.0 and 
 *			linux 3.0.8
 * V1.2	  2013/03/07, Shouxian Chen(Simon Chen) for ZTE changed.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 ******************************************************************************/

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/slab.h>
#include <linux/i2c.h>
#include <linux/init.h>
#include <linux/input.h>
#include <linux/idr.h>
#include <linux/fs.h>
#include <linux/timer.h>
#include <linux/sched.h>
#include <linux/seq_file.h>
#include <asm/io.h>
#include <linux/ioctl.h>
#include <linux/types.h>
#include <linux/spinlock.h>
#include <asm/uaccess.h>
#include <linux/workqueue.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/delay.h>
#include <linux/irq.h>
#include <asm/gpio.h>
#include <linux/miscdevice.h>
#include <linux/interrupt.h>
#include <asm/uaccess.h>
#include <asm/errno.h>
#include <asm/delay.h>

/* only for taso */
struct taos_cfg{
	u32 calibrate_target;
	u16 als_time;
	u16 scale_factor;
	u16 gain_trim;
	u8 filter_history;
	u8 filter_count;
	u8 gain;
	u16 prox_threshold_hi;
	u16 prox_threshold_lo;
	u8 prox_int_time;
	u8 prox_adc_time;
	u8 prox_wait_time;
	u8 prox_intr_filter;
	u8 prox_config;
	u8 prox_pulse_int;
	u8 prox_gain;
};

struct taos_prox_info {
	u16 prox_clear;
	u16 prox_data;
	int prox_event;
};

#define ISL_TAG        "[isl]"

#define ISL_IOCTL_MAGIC		0xCF
#define ISL_IOCTL_ALS_ON	_IO(ISL_IOCTL_MAGIC, 1)
#define ISL_IOCTL_ALS_OFF		_IO(ISL_IOCTL_MAGIC, 2)
#define ISL_IOCTL_ALS_DATA		_IOR(ISL_IOCTL_MAGIC, 3, short)
#define ISL_IOCTL_ALS_CALIBRATE	_IO(ISL_IOCTL_MAGIC, 4)
#define ISL_IOCTL_CONFIG_GET	_IOR(ISL_IOCTL_MAGIC, 5, struct taos_cfg)
#define ISL_IOCTL_CONFIG_SET	_IOW(ISL_IOCTL_MAGIC, 6, struct taos_cfg)
#define ISL_IOCTL_PROX_ON		_IO(ISL_IOCTL_MAGIC, 7)
#define ISL_IOCTL_PROX_OFF		_IO(ISL_IOCTL_MAGIC, 8)
#define ISL_IOCTL_PROX_DATA		_IOR(ISL_IOCTL_MAGIC, 9, struct taos_prox_info)
#define ISL_IOCTL_PROX_EVENT	_IO(ISL_IOCTL_MAGIC, 10)
#define ISL_IOCTL_PROX_CALIBRATE	_IO(ISL_IOCTL_MAGIC, 11)

#define ISL29044_ADDR	0x44
#define	DEVICE_NAME		"isl29044"
#define	DRIVER_VERSION	"1.1"

#define ALS_EN_MSK		(1 << 0)
#define PS_EN_MSK		(1 << 1)

#define PS_POLL_TIME	100	 	/* unit is ms */

#define PROX_THRESHOLD_DELTA_LO		8
#define PROX_THRESHOLD_DELTA_HI		8
#define MAX_STARTUP_CALIB_OVER_RANGE	30

#define IS_DO_START_UP_CALIB		1

#define ISL_INT_GPIO 49

static int isl29044_probe(struct i2c_client *client, const struct i2c_device_id *id);

/* char device number */
dev_t dev_num;

/* class structure for this device */
struct class *isl_class;

/* chip config struct */
struct isl29044_cfg_t {
	u8 als_range;		/* als range, 0: 125 Lux, 1: 2000Lux */
	u8 als_mode;		/* als mode, 0: Visible light, 1: IR light */
	u8 ps_lt;			/* ps low limit */
	u8 ps_ht;			/* ps high limit */
	u8 ps_led_drv_cur;	/* led driver current, 0: 110mA, 1: 220mA */
	int glass_factor;	/* glass factor for als, percent */
	u8 prox_th_delta_lo; /* proximity threshold low limit delta */
	u8 prox_th_delta_hi; /* proximity threshold low limit delta */
};

/* calibaration output data */
struct isl29044_calib_out_t {
	int ret;
	u8 ps_lt;
	u8 ps_ht;
};

struct isl_intr_data {
    int int_gpio;
    int irq;
};

/* Each client has this additional data */
struct isl29044_data_t {
	struct i2c_client* client;
	struct isl29044_cfg_t* cfg;
	u8 als_pwr_status;
	u8 ps_pwr_status;
	int poll_delay;		/* poll delay set by hal */
	u8 show_als_raw;	/* show als raw data flag, used for debug */
	u8 show_ps_raw;	/* show als raw data flag, used for debug */
	struct timer_list als_timer;	/* als poll timer */
	struct timer_list ps_timer;	/* ps poll timer */
	spinlock_t als_timer_lock;
	spinlock_t ps_timer_lock;
	struct work_struct als_work;
	struct work_struct ps_work;
	struct work_struct calib_work;
	struct workqueue_struct *als_wq;
	struct workqueue_struct *ps_wq;
	struct input_dev *als_input_dev;
	struct input_dev *ps_input_dev;
	int last_ps;
	u8 als_range_using;		/* the als range using now */
	u8 als_pwr_before_suspend;
	u8 ps_pwr_before_suspend;
	struct isl_intr_data *intr_data;

	u8 ps_filter_cnt;
	int last_lux; 
	int last_ps_raw;

	int als_chg_range_delay_cnt;
	
	struct cdev cdev;
};

static struct isl_intr_data isl_irq= {
    .int_gpio = ISL_INT_GPIO,
    .irq = MSM_GPIO_TO_INT(ISL_INT_GPIO),
};

/* Do not scan isl29044 automatic */
static const unsigned short normal_i2c[] = {ISL29044_ADDR, I2C_CLIENT_END };

/* board and address info */
/*
struct i2c_board_info taos_board_info[] = {
	{I2C_BOARD_INFO(DEVICE_NAME, ISL29044_ADDR)}
};*/

/* data struct for isl29044 device */
static struct isl29044_cfg_t isl29044_cfg = {
	.als_range = 1,
	.als_mode = 0,
	.ps_lt = 120,	
	.ps_ht = 150,		
	.ps_led_drv_cur = 0,
	.glass_factor = 20,
	.prox_th_delta_lo = PROX_THRESHOLD_DELTA_LO,
	.prox_th_delta_hi = PROX_THRESHOLD_DELTA_HI
};

static struct isl29044_data_t isl29044_data = {
	.cfg = &isl29044_cfg,
	.client = NULL,
	.als_pwr_status = 0,
	.ps_pwr_status = 0,
	.als_input_dev = NULL,
	.ps_input_dev = NULL
};

/* gobal var */
struct isl29044_calib_out_t calib_out;
//static atomic_t is_calib_done = ATOMIC_INIT(0);
static struct taos_cfg cfg_taos;
static struct taos_prox_info prox_info;

static int set_als_pwr_st(u8 state, struct isl29044_data_t *dat);
static int set_ps_pwr_st(u8 state, struct isl29044_data_t *dat);

static void do_als_timer(unsigned long arg)
{
	struct isl29044_data_t *dev_dat;

	dev_dat = (struct isl29044_data_t *)arg;

	/* timer handler is atomic context, so don't need sinp_lock() */
	if(dev_dat->als_pwr_status == 0)
	{
		return ;
	}

	/* start a work queue, I cannot do i2c oepration in timer context for
	   this context is atomic and i2c function maybe sleep. */
	queue_work(dev_dat->als_wq, &dev_dat->als_work);
}

#if 0
static void do_ps_timer(unsigned long arg)
{
	struct isl29044_data_t *dev_dat;

	dev_dat = (struct isl29044_data_t *)arg;

	if(dev_dat->ps_pwr_status == 0)
	{
		return ;
	}
	
	/* start a work queue, I cannot do i2c oepration in timer context for
	   this context is atomic and i2c function maybe sleep. */
	//schedule_work(&dev_dat->ps_work);
	queue_work(dev_dat->ps_wq, &dev_dat->ps_work);
}
#endif

static irqreturn_t ps_interrupt(int irq, void *data)
{
    disable_irq_nosync(isl29044_data.intr_data->irq);
    //schedule_work(&taos_datap->taos_work);
	queue_work(isl29044_data.ps_wq, &isl29044_data.ps_work);
    
    return IRQ_HANDLED;
}

static void do_calib_work(struct work_struct *work)
{
	struct isl29044_data_t *dev_dat;
	struct isl29044_cfg_t *cfg;
	int ret;
	u8 reg;
	int calib_cnt = 10;
	int i;
	u16 sum = 0;
	u8 ps_dat;
	u16 ps_lt, ps_ht;

	printk(KERN_DEBUG "start isl29044 calibration...");
	
	dev_dat = container_of(work, struct isl29044_data_t, calib_work);
	cfg = dev_dat->cfg;

	reg = 0xe0; /* the ps sleep time is 12.5ms */
	if(cfg->ps_led_drv_cur) reg |= 0x08;
	ret = i2c_smbus_write_byte_data(dev_dat->client, 0x01, reg);
	if(ret < 0) goto err_wr;

	for(i = 0; i < calib_cnt; i++)
	{
		msleep(20);
		ret = i2c_smbus_read_byte_data(dev_dat->client, 0x08);
		if(ret < 0) goto err_rd;
		sum = sum + (u8)ret;
	}

	ps_dat = sum / i;
	ps_lt = ps_dat + cfg->prox_th_delta_lo;
	ps_ht = ps_lt + cfg->prox_th_delta_hi;
	if(ps_lt > 255) 
	{
		ps_lt = 255;
		calib_out.ret = -3;
	}
	if(ps_ht > 255) 
	{
		ps_ht = 255;
		calib_out.ret = -3;
	}

	calib_out.ret = 0;
	calib_out.ps_ht = ps_ht;
	calib_out.ps_lt = ps_lt;
	
	cfg->ps_ht = ps_ht;
	cfg->ps_lt = ps_lt;

	//atomic_set(&is_calib_done, 1);

	printk(KERN_INFO "end isl29044 calibration, ps_lt=%d, ps_ht=%d", ps_lt, ps_ht);
	return ;
	
err_rd:
	calib_out.ret = -1;
	printk(KERN_ERR "calibration failed, read sensor reg error, ret = %d\n", ret);
	//atomic_set(&is_calib_done, 1);
	return ;
	
err_wr:
	calib_out.ret = -1;
	printk(KERN_ERR "calibration failed, write sensor reg error, ret = %d\n", ret);
	//atomic_set(&is_calib_done, 1);
	return ;	
}

static void do_calib(struct isl29044_data_t *dev_dat)
{
	u8 als_pwr;
	u8 ps_pwr;
	
	ps_pwr = dev_dat->ps_pwr_status;
	als_pwr = dev_dat->als_pwr_status;
	
	if(dev_dat->als_pwr_status)
	{
		set_als_pwr_st(0, dev_dat);
	}
	
	if(dev_dat->ps_pwr_status)
	{
		set_ps_pwr_st(0, dev_dat);
	}
	
	msleep(400);
	
	do_calib_work(&dev_dat->calib_work);
	
	set_als_pwr_st(als_pwr, dev_dat);
	set_ps_pwr_st(ps_pwr, dev_dat);
}

#if(IS_DO_START_UP_CALIB)
static int do_startup_calib(struct isl29044_data_t *dev_dat)
{
	do_calib(dev_dat);

	if(calib_out.ret < 0) return -1;
	
	if((calib_out.ps_lt > (dev_dat->cfg->ps_lt + MAX_STARTUP_CALIB_OVER_RANGE)) || 
		(calib_out.ps_ht > (dev_dat->cfg->ps_ht + MAX_STARTUP_CALIB_OVER_RANGE)))
	{
		printk(KERN_WARNING "startup calibration result is too large, use "
			"default value");
		return -1;
	}
	else
	{
		dev_dat->cfg->ps_lt = calib_out.ps_lt;
		dev_dat->cfg->ps_ht = calib_out.ps_ht;
		return 0;
	}
}
#endif

static void do_als_work(struct work_struct *work)
{
	struct isl29044_data_t *dev_dat;
	struct isl29044_cfg_t *cfg;
	int ret;
	static int als_dat;
	u8 show_raw_dat;
	int lux;
	u8 als_range;
	int is_chg_range = 0;
	u8 new_range;
	
	dev_dat = container_of(work, struct isl29044_data_t, als_work);
	cfg = dev_dat->cfg;

	spin_lock(&dev_dat->ps_timer_lock);
	show_raw_dat = dev_dat->show_als_raw;
	spin_unlock(&dev_dat->ps_timer_lock);

	als_range = dev_dat->als_range_using;

	ret = i2c_smbus_read_byte_data(dev_dat->client, 0x09);
	if(ret < 0) goto err_rd;
	als_dat = (u8)ret;
	
	ret = i2c_smbus_read_byte_data(dev_dat->client, 0x0a);
	if(ret < 0) goto err_rd;
	als_dat = als_dat + ( ((u8)ret & 0x0f) << 8 );

	
	if(dev_dat->als_chg_range_delay_cnt == 0)
	{
		/* als measurment is done */
		if(als_range)
		{
			lux = (als_dat * 2000) / 4096;
		}
		else
		{
			lux = (als_dat * 125) / 4096;
		}

		/* change range */
		if((als_range > 0) && (als_dat < 200))
		{
			cfg->als_range = 0;
			dev_dat->als_range_using = 0;
			is_chg_range = 1;
		}
		else if((als_range == 0) && (als_dat > 3500))
		{
			cfg->als_range = 1;
			dev_dat->als_range_using = 1;
			is_chg_range = 1;
		}

		if(is_chg_range)
		{
			ret = i2c_smbus_read_byte_data(dev_dat->client, 0x01);
			if(ret < 0) goto err_rd;

			new_range = (u8)ret;
			new_range &= ~(0x02);
			if(cfg->als_range > 0) new_range |= 0x02;
			
			ret = i2c_smbus_write_byte_data(dev_dat->client, 0x01, new_range);
			if(ret < 0) goto err_wr;

			dev_dat->als_chg_range_delay_cnt = 2;
		}

		lux = lux * 100 / cfg->glass_factor;
		input_report_abs(dev_dat->als_input_dev, ABS_MISC, lux);
		input_sync(dev_dat->als_input_dev);
		if(show_raw_dat)
		{
			printk(KERN_INFO "now als raw data is = %d, LUX = %d\n", als_dat, lux);
		}
		dev_dat->last_lux = lux;
	}
	else if(dev_dat->als_chg_range_delay_cnt > 0)
	{
		dev_dat->als_chg_range_delay_cnt--;
	}

	/* restart timer */			
	spin_lock(&dev_dat->als_timer_lock);
	if(dev_dat->als_pwr_status == 0)
	{
		spin_unlock(&dev_dat->als_timer_lock);
		return ;
	}
	dev_dat->als_timer.expires = jiffies + (HZ * dev_dat->poll_delay) / 1000;
	spin_unlock(&dev_dat->als_timer_lock);
	add_timer(&dev_dat->als_timer);	

	return ;

err_rd:
	printk(KERN_ERR "Read ps sensor error, ret = %d\n", ret);
	return ;

err_wr:
	printk(KERN_ERR "write sensor reg error, ret = %d\n", ret);
	return ;
}

static void do_ps_work(struct work_struct *work)
{
	struct isl29044_data_t *dev_dat;
	struct isl29044_cfg_t *cfg;
	//int last_ps;
	int ret;
	u8 show_raw_dat;
	int status;

	dev_dat = container_of(work, struct isl29044_data_t, ps_work);
	cfg = dev_dat->cfg;

	spin_lock(&dev_dat->ps_timer_lock);
	show_raw_dat = dev_dat->show_ps_raw;
	spin_unlock(&dev_dat->ps_timer_lock);

	ret = i2c_smbus_read_byte_data(dev_dat->client, 0x08);
	if(ret < 0) goto err_rd;

	status = i2c_smbus_read_byte_data(dev_dat->client, 0x02);
	if(status < 0) goto err_rd;
	
	if(status & 0x80) dev_dat->last_ps = 0;
	else dev_dat->last_ps = 1;
	
	//last_ps = dev_dat->last_ps;
		
	//if(ret > cfg->ps_ht) dev_dat->last_ps = 0;
	//else dev_dat->last_ps = 1;
	//else if(ret < cfg->ps_lt) dev_dat->last_ps = 1;
	//else if(dev_dat->last_ps == -1) dev_dat->last_ps = 1;

	dev_dat->last_ps_raw = ret;

	if(show_raw_dat)
	{
		printk(KERN_INFO "ps raw data = %d\n", dev_dat->last_ps_raw);
	}

	if(dev_dat->ps_pwr_status > 0)
	{
		if(dev_dat->last_ps == 0)
		{
			input_report_abs(dev_dat->ps_input_dev, ABS_DISTANCE, 0);
		}
		else
		{
			input_report_abs(dev_dat->ps_input_dev, ABS_DISTANCE, 5);
		}
		input_sync(dev_dat->ps_input_dev);
		if(show_raw_dat)
		{
			printk(KERN_INFO "ps status changed, now = %d\n",dev_dat->last_ps);
		}
	}
	

	/* restart timer */
	/*
	spin_lock(&dev_dat->ps_timer_lock);
	if(dev_dat->ps_pwr_status == 0)
	{
		spin_unlock(&dev_dat->ps_timer_lock);
		return ;
	}
	dev_dat->ps_timer.expires = jiffies + (HZ * PS_POLL_TIME) / 1000;
	spin_unlock(&dev_dat->ps_timer_lock);
	add_timer(&dev_dat->ps_timer);
	*/
	
	enable_irq(dev_dat->intr_data->irq);
	return ;

err_rd:
	printk(KERN_ERR "Read ps sensor error, ret = %d\n", ret);
	enable_irq(dev_dat->intr_data->irq);
	return ;
}

/* enable to run als */
static int set_sensor_reg(struct isl29044_data_t *dev_dat)
{
	struct isl29044_cfg_t* cfg;
	u8 reg_dat[5];
	int i, ret;

	cfg = dev_dat->cfg;
	
	reg_dat[2] = 0x22;
	if(dev_dat->ps_pwr_status)
	{
		reg_dat[3] = cfg->ps_lt;
		reg_dat[4] = cfg->ps_ht;
	}
	else
	{
		reg_dat[3] = 0;
		reg_dat[4] = 255;
	}

	reg_dat[1] = 0x50;	/* set ps sleep time to 50ms */
	spin_lock(&dev_dat->als_timer_lock);
	if(dev_dat->als_pwr_status)
	{
		/* measurement als */
		reg_dat[1] |= 0x04;
	}
	spin_unlock(&dev_dat->als_timer_lock);
	
	spin_lock(&dev_dat->ps_timer_lock);
	if(dev_dat->ps_pwr_status) reg_dat[1] |= 0x80;
	if(cfg->als_range) reg_dat[1] |= 0x02;
	if(cfg->ps_led_drv_cur) reg_dat[1] |= 0x08;
	spin_unlock(&dev_dat->ps_timer_lock);
	
	for(i = 2 ; i <= 4; i++)
	{
		ret = i2c_smbus_write_byte_data(dev_dat->client, i, reg_dat[i]);
		if(ret < 0) 
		{
			return ret;
		}
	}
	
	ret = i2c_smbus_write_byte_data(dev_dat->client, 0x01, reg_dat[1]);
	if(ret < 0) 
	{
		return ret;	
	}

	return 0;
}

/* set power status */
static int set_als_pwr_st(u8 state, struct isl29044_data_t *dat)
{
	int ret = 0;
	
	if(state)
	{
		spin_lock(&dat->als_timer_lock);
		if(dat->als_pwr_status)
		{
			spin_unlock(&dat->als_timer_lock);
			return ret;
		}
		dat->als_pwr_status = 1;
		spin_unlock(&dat->als_timer_lock);
		ret = set_sensor_reg(dat);
		if(ret < 0)
		{
			printk(KERN_ERR "set light sensor reg error, ret = %d\n", ret);
			return ret;
		}
		
		/* start timer */
		dat->als_timer.function = &do_als_timer;
		dat->als_timer.data = (unsigned long)dat;
		spin_lock(&dat->als_timer_lock);
		dat->als_timer.expires = jiffies + (HZ * dat->poll_delay) / 1000;
		spin_unlock(&dat->als_timer_lock);
		dat->als_range_using = dat->cfg->als_range;

		add_timer(&dat->als_timer);
	}
	else
	{
		spin_lock(&dat->als_timer_lock);
		if(dat->als_pwr_status == 0)
		{
			spin_unlock(&dat->als_timer_lock);
			return ret;
		}
		dat->als_pwr_status = 0;
		spin_unlock(&dat->als_timer_lock);
		ret = set_sensor_reg(dat);

		/* delete timer */
		del_timer_sync(&dat->als_timer);
	}

	return ret;
}

static int set_ps_pwr_st(u8 state, struct isl29044_data_t *dat)
{
	int ret = 0;
	
	if(state)
	{
		spin_lock(&dat->ps_timer_lock);
		if(dat->ps_pwr_status)
		{
			spin_unlock(&dat->ps_timer_lock);
			return ret;
		}
		dat->ps_pwr_status = 1;
		spin_unlock(&dat->ps_timer_lock);
		
		dat->last_ps = -1;
		dat->ps_filter_cnt = 0;
		ret = set_sensor_reg(dat);
		if(ret < 0)
		{
			printk(KERN_ERR "set proximity sensor reg error, ret = %d\n", ret);
			return ret;
		}	
		
		/* start timer */
		/*
		dat->ps_timer.function = &do_ps_timer;
		dat->ps_timer.data = (unsigned long)dat;
		dat->ps_timer.expires = jiffies + (HZ * PS_POLL_TIME) / 1000;
		add_timer(&dat->ps_timer);
		*/
	}
	else
	{
		spin_lock(&dat->ps_timer_lock);
		if(dat->ps_pwr_status == 0)
		{
			spin_unlock(&dat->ps_timer_lock);
			return ret;
		}
		dat->ps_pwr_status = 0;
		spin_unlock(&dat->ps_timer_lock);

		ret = set_sensor_reg(dat);
		
		/* delete timer */
		/*
		del_timer_sync(&dat->ps_timer);
		*/
	}

	return ret;
}

/* device attribute */
/* enable als attribute */
static ssize_t show_enable_als_sensor(struct device *dev,
			  struct device_attribute *attr, char *buf)
{
	struct isl29044_data_t *dat;
	u8 pwr_status;

	dat = (struct isl29044_data_t *)dev->platform_data;
	spin_lock(&dat->als_timer_lock);
	pwr_status = dat->als_pwr_status;
	spin_unlock(&dat->als_timer_lock);

	return snprintf(buf, PAGE_SIZE, "%d\n", pwr_status);
}
static ssize_t store_enable_als_sensor(struct device *dev, 
	struct device_attribute *attr, const char *buf, size_t count)
{
	struct isl29044_data_t *dat;
	ssize_t ret;
	unsigned long val;

	dat = (struct isl29044_data_t *)dev->platform_data;
		
	val = simple_strtoul(buf, NULL, 10);
	ret = set_als_pwr_st(val, dat);

	if(ret == 0) ret = count;
	return ret;
}
static DEVICE_ATTR(enable_als_sensor, S_IWUGO|S_IRUGO, show_enable_als_sensor,
	store_enable_als_sensor);

/* enable ps attribute */
static ssize_t show_enable_ps_sensor(struct device *dev,
			  struct device_attribute *attr, char *buf)
{
	struct isl29044_data_t *dat;
	u8 pwr_status;

	dat = (struct isl29044_data_t *)dev->platform_data;
	spin_lock(&dat->ps_timer_lock);
	pwr_status = dat->ps_pwr_status;
	spin_unlock(&dat->ps_timer_lock);

	return snprintf(buf, PAGE_SIZE, "%d\n", pwr_status);
}
static ssize_t store_enable_ps_sensor(struct device *dev, 
	struct device_attribute *attr, const char *buf, size_t count)
{
	struct isl29044_data_t *dat;
	ssize_t ret;
	unsigned long val;

	dat = (struct isl29044_data_t *)dev->platform_data;
		
	val = simple_strtoul(buf, NULL, 10);
	ret = set_ps_pwr_st(val, dat);

	if(ret == 0) ret = count;
	return ret;
}
static DEVICE_ATTR(enable_ps_sensor, S_IWUGO|S_IRUGO, show_enable_ps_sensor,
	store_enable_ps_sensor);

/* ps led driver current attribute */
static ssize_t show_ps_led_drv(struct device *dev,
			  struct device_attribute *attr, char *buf)
{
	struct isl29044_data_t *dat;

	dat = (struct isl29044_data_t *)dev->platform_data;
	return snprintf(buf, PAGE_SIZE, "%d\n", dat->cfg->ps_led_drv_cur);
}
static ssize_t store_ps_led_drv(struct device *dev, 
	struct device_attribute *attr, const char *buf, size_t count)
{
	struct isl29044_data_t *dat;
	int val;

	if(sscanf(buf, "%d", &val) != 1)
	{
		return -EINVAL;
	}
	
	dat = (struct isl29044_data_t *)dev->platform_data;
	if(val) dat->cfg->ps_led_drv_cur = 1;
	else dat->cfg->ps_led_drv_cur = 0;
	
	return count;
}
static DEVICE_ATTR(ps_led_driver_current, S_IWUGO|S_IRUGO, show_ps_led_drv,
	store_ps_led_drv);

/* als range attribute */
static ssize_t show_als_range(struct device *dev,
			  struct device_attribute *attr, char *buf)
{
	struct isl29044_data_t *dat;
	u8 range;
	
	dat = (struct isl29044_data_t *)dev->platform_data;
	spin_lock(&dat->als_timer_lock);
	range = dat->cfg->als_range;
	spin_unlock(&dat->als_timer_lock);
	
	return snprintf(buf, PAGE_SIZE, "%d\n", range);
}
static ssize_t store_als_range(struct device *dev, 
	struct device_attribute *attr, const char *buf, size_t count)
{
	struct isl29044_data_t *dat;
	int val;

	if(sscanf(buf, "%d", &val) != 1)
	{
		return -EINVAL;
	}
	
	dat = (struct isl29044_data_t *)dev->platform_data;
	
	spin_lock(&dat->als_timer_lock);
	if(val) dat->cfg->als_range = 1;
	else dat->cfg->als_range = 0;
	spin_unlock(&dat->als_timer_lock);
	
	return count;
}
static DEVICE_ATTR(als_range, S_IWUGO|S_IRUGO, show_als_range, store_als_range);

/* als mode attribute */
static ssize_t show_als_mode(struct device *dev,
			  struct device_attribute *attr, char *buf)
{
	struct isl29044_data_t *dat;

	dat = (struct isl29044_data_t *)dev->platform_data;
	return snprintf(buf, PAGE_SIZE, "%d\n", dat->cfg->als_mode);
}
static ssize_t store_als_mode(struct device *dev, 
	struct device_attribute *attr, const char *buf, size_t count)
{
	struct isl29044_data_t *dat;
	int val;

	if(sscanf(buf, "%d", &val) != 1)
	{
		return -EINVAL;
	}
	
	dat = (struct isl29044_data_t *)dev->platform_data;
	if(val) dat->cfg->als_mode = 1;
	else dat->cfg->als_mode = 0;
	
	return count;
}
static DEVICE_ATTR(als_mode, S_IWUGO|S_IRUGO, show_als_mode, store_als_mode);

/* ps limit range attribute */
static ssize_t show_ps_limit(struct device *dev,
			  struct device_attribute *attr, char *buf)
{
	struct isl29044_data_t *dat;

	dat = (struct isl29044_data_t *)dev->platform_data;
	return snprintf(buf, PAGE_SIZE, "%d %d\n", dat->cfg->ps_lt, dat->cfg->ps_ht);
}
static ssize_t store_ps_limit(struct device *dev, 
	struct device_attribute *attr, const char *buf, size_t count)
{
	struct isl29044_data_t *dat;
	int lt, ht;

	if(sscanf(buf, "%d %d", &lt, &ht) != 2)
	{
		return -EINVAL;
	}
	
	dat = (struct isl29044_data_t *)dev->platform_data;
	
	if(lt > 255) dat->cfg->ps_lt = 255;
	else if(lt < 0) dat->cfg->ps_lt = 0;
	else  dat->cfg->ps_lt = lt;
	
	if(ht > 255) dat->cfg->ps_ht = 255;
	else if(ht < 0) dat->cfg->ps_ht = 0;
	else  dat->cfg->ps_ht = ht;
	
	return count;
}
static DEVICE_ATTR(ps_limit, S_IWUGO|S_IRUGO, show_ps_limit, store_ps_limit);

/* poll delay attribute */
static ssize_t show_poll_delay (struct device *dev,
			  struct device_attribute *attr, char *buf)
{
	struct isl29044_data_t *dat;
	int delay;

	dat = (struct isl29044_data_t *)dev->platform_data;
	spin_lock(&dat->als_timer_lock);
	delay = dat->poll_delay;
	spin_unlock(&dat->als_timer_lock);
	
	return snprintf(buf, PAGE_SIZE, "%d\n", delay);
}
static ssize_t store_poll_delay (struct device *dev, 
	struct device_attribute *attr, const char *buf, size_t count)
{
	struct isl29044_data_t *dat;
	int delay;

	if(sscanf(buf, "%d", &delay) != 1)
	{
		return -EINVAL;
	}
	
	dat = (struct isl29044_data_t *)dev->platform_data;

	spin_lock(&dat->als_timer_lock);
	if(delay  < 120) dat->poll_delay = 120;
	else if(delay > 65535) dat->poll_delay = 65535;
	else dat->poll_delay = delay;
	spin_unlock(&dat->als_timer_lock);
	
	return count;
}
static DEVICE_ATTR(poll_delay, S_IWUGO|S_IRUGO, show_poll_delay, 
	store_poll_delay);

/* show als raw data attribute */
static ssize_t show_als_show_raw (struct device *dev,
			  struct device_attribute *attr, char *buf)
{
	struct isl29044_data_t *dat;
	u8 flag;

	dat = (struct isl29044_data_t *)dev->platform_data;
	spin_lock(&dat->als_timer_lock);
	flag = dat->show_als_raw;
	spin_unlock(&dat->als_timer_lock);
	
	return snprintf(buf, PAGE_SIZE, "%d\n", flag);
}
static ssize_t store_als_show_raw (struct device *dev, 
	struct device_attribute *attr, const char *buf, size_t count)
{
	struct isl29044_data_t *dat;
	int flag;

	if(sscanf(buf, "%d", &flag) != 1)
	{
		return -EINVAL;
	}
	
	dat = (struct isl29044_data_t *)dev->platform_data;

	spin_lock(&dat->als_timer_lock);
	if(flag == 0) dat->show_als_raw = 0;
	else dat->show_als_raw = 1;
	spin_unlock(&dat->als_timer_lock);
	
	return count;
}
static DEVICE_ATTR(als_show_raw, S_IWUGO|S_IRUGO, show_als_show_raw, 
	store_als_show_raw);

/* show ps raw data attribute */
static ssize_t show_ps_show_raw (struct device *dev,
			  struct device_attribute *attr, char *buf)
{
	struct isl29044_data_t *dat;
	u8 flag;

	dat = (struct isl29044_data_t *)dev->platform_data;
	spin_lock(&dat->als_timer_lock);
	flag = dat->show_ps_raw;
	spin_unlock(&dat->als_timer_lock);
	
	return snprintf(buf, PAGE_SIZE, "%d\n", flag);
}
static ssize_t store_ps_show_raw (struct device *dev, 
	struct device_attribute *attr, const char *buf, size_t count)
{
	struct isl29044_data_t *dat;
	int flag;

	if(sscanf(buf, "%d", &flag) != 1)
	{
		return -EINVAL;
	}
	
	dat = (struct isl29044_data_t *)dev->platform_data;

	spin_lock(&dat->als_timer_lock);
	if(flag == 0) dat->show_ps_raw = 0;
	else dat->show_ps_raw = 1;
	spin_unlock(&dat->als_timer_lock);
	
	return count;
}
static DEVICE_ATTR(ps_show_raw, S_IWUGO|S_IRUGO, show_ps_show_raw, 
	store_ps_show_raw);

static struct attribute *als_attr[] = {
	&dev_attr_enable_als_sensor.attr,
	&dev_attr_als_range.attr,
	&dev_attr_als_mode.attr,
	&dev_attr_poll_delay.attr,
	&dev_attr_als_show_raw.attr,
	NULL
};

static struct attribute_group als_attr_grp = {
	.name = "light sensor",
	.attrs = als_attr
};

static struct attribute *ps_attr[] = {
	&dev_attr_enable_ps_sensor.attr,
	&dev_attr_ps_led_driver_current.attr,
	&dev_attr_ps_limit.attr,
	&dev_attr_ps_show_raw.attr,
	NULL
};

static struct attribute_group ps_attr_grp = {
	.name = "proximity sensor",
	.attrs = ps_attr
};

/* initial and register a input device for sensor */
static int init_input_dev(struct isl29044_data_t *dev_dat)
{
	int err;
	struct input_dev *als_dev;
	struct input_dev *ps_dev;
	
	als_dev = input_allocate_device();
	if (!als_dev)
	{
		return -ENOMEM;
	}

	ps_dev = input_allocate_device();
	if (!ps_dev)
	{
		err = -ENOMEM;	
		goto err_free_als;
	}

	als_dev->name = "light";
	als_dev->id.bustype = BUS_I2C;
	als_dev->id.vendor  = 0x0001;
	als_dev->id.product = 0x0001;
	als_dev->id.version = 0x0100;
	als_dev->evbit[0] = BIT_MASK(EV_ABS);
	als_dev->absbit[BIT_WORD(ABS_MISC)] |= BIT_MASK(ABS_MISC);
	als_dev->dev.platform_data = &isl29044_data;
	input_set_abs_params(als_dev, ABS_MISC, 0, 65535, 0, 0);
	
	ps_dev->name = "proximity";
	ps_dev->id.bustype = BUS_I2C;
	ps_dev->id.vendor  = 0x0001;
	ps_dev->id.product = 0x0002;
	ps_dev->id.version = 0x0100;
	ps_dev->evbit[0] = BIT_MASK(EV_ABS);
	ps_dev->absbit[BIT_WORD(ABS_DISTANCE)] |= BIT_MASK(ABS_DISTANCE);
	ps_dev->dev.platform_data = &isl29044_data;
	input_set_abs_params(ps_dev, ABS_DISTANCE, 0, 65535, 0, 0);
	
	err = input_register_device(als_dev);
	if (err)
	{
		goto err_free_ps;
	}

	err = input_register_device(ps_dev);
	if (err)
	{
		goto err_free_ps;
	}	

	err = sysfs_create_group(&als_dev->dev.kobj, &als_attr_grp);
	if (err) {
		dev_err(&als_dev->dev, "isl29044: device create als file failed\n");
		goto err_free_ps;
	}

	err = sysfs_create_group(&ps_dev->dev.kobj, &ps_attr_grp);
	if (err) {
		dev_err(&ps_dev->dev, "isl29044: device create ps file failed\n");
		goto err_free_ps;
	}

	dev_dat->als_input_dev = als_dev;
	dev_dat->ps_input_dev = ps_dev;
	
	return 0;

err_free_ps:
	input_free_device(ps_dev);
err_free_als:
	input_free_device(als_dev);

	return err;
}

/* Return 0 if detection is successful, -ENODEV otherwise */
static int isl29044_detect(struct i2c_client *client,
	struct i2c_board_info *info)
{
	struct i2c_adapter *adapter = client->adapter;

	printk(KERN_DEBUG "In isl29044_detect()\n");
	if (!i2c_check_functionality(adapter, I2C_FUNC_SMBUS_WRITE_BYTE_DATA
				     | I2C_FUNC_SMBUS_READ_BYTE))
	{
		printk(KERN_WARNING "I2c adapter don't support ISL29044\n");
		return -ENODEV;
	}

	/* probe that if isl29044 is at the i2 address */
	if (i2c_smbus_xfer(adapter, client->addr, 0,I2C_SMBUS_WRITE,
		0,I2C_SMBUS_QUICK,NULL) < 0)
		return -ENODEV;

	strlcpy(info->type, "isl29044", I2C_NAME_SIZE);
	printk(KERN_INFO "%s is found at i2c device address %d\n", 
		info->type, client->addr);

	return 0;
}

static int isl29044_remove(struct i2c_client *client)
{
	struct input_dev *als_dev;
	struct input_dev *ps_dev;

	printk(KERN_INFO "%s at address %d is removed\n",client->name,client->addr);

	/* clean the isl29044 data struct when isl29044 device remove */
	isl29044_data.client = NULL;
	isl29044_data.als_pwr_status = 0;
	isl29044_data.ps_pwr_status = 0;

	als_dev = isl29044_data.als_input_dev;
	ps_dev = isl29044_data.ps_input_dev;

	sysfs_remove_group(&als_dev->dev.kobj, &als_attr_grp);
	sysfs_remove_group(&ps_dev->dev.kobj, &ps_attr_grp);

	input_unregister_device(als_dev);
	input_unregister_device(ps_dev);
	
	destroy_workqueue(isl29044_data.ps_wq);
	destroy_workqueue(isl29044_data.als_wq);

	isl29044_data.als_input_dev = NULL;
	isl29044_data.ps_input_dev = NULL;

	return 0;
}

#ifdef CONFIG_PM	
/* if define power manager, define suspend and resume function */
static int isl29044_suspend(struct i2c_client *client, pm_message_t mesg)
{
	struct isl29044_data_t *dat;
	int ret;

	dat = i2c_get_clientdata(client);
	
	spin_lock(&dat->als_timer_lock);
	dat->als_pwr_before_suspend = dat->als_pwr_status;
	spin_unlock(&dat->als_timer_lock);
	ret = set_als_pwr_st(0, dat);
	if(ret < 0) return ret;
	
	spin_lock(&dat->ps_timer_lock);
	dat->ps_pwr_before_suspend = dat->ps_pwr_status;
	spin_unlock(&dat->ps_timer_lock);
	ret = set_ps_pwr_st(0, dat);
	if(ret < 0) return ret;

	return 0;
}

static int isl29044_resume(struct i2c_client *client)
{
	struct isl29044_data_t *dat;
	int ret;

	dat = i2c_get_clientdata(client);

	ret = set_als_pwr_st(dat->als_pwr_before_suspend, dat);
	if(ret < 0) return ret;
	
	ret = set_ps_pwr_st(dat->ps_pwr_before_suspend, dat);
	if(ret < 0) return ret;
	
	return 0;
}
#else
#define	isl29044_suspend 	NULL
#define isl29044_resume		NULL
#endif		/*ifdef CONFIG_PM end*/

static const struct i2c_device_id isl29044_id[] = {
	{DEVICE_NAME, 0 },
	{}
};
MODULE_DEVICE_TABLE(i2c, isl29044_id);

static struct i2c_driver isl29044_driver = {
	.driver = {
		.name	= "isl29044",
	},
	.probe			= isl29044_probe,
	.remove			= isl29044_remove,
	.id_table		= isl29044_id,
	.detect			= isl29044_detect,
	//.address_list	= normal_i2c,
	.suspend		= isl29044_suspend,
	.resume			= isl29044_resume
};

int config_int_gpio(int int_gpio)
{
    int rc=0;
    uint32_t  gpio_config_data = GPIO_CFG(int_gpio,  0, GPIO_CFG_INPUT, GPIO_CFG_PULL_UP, GPIO_CFG_2MA);

    rc = gpio_request(int_gpio, "gpio_sensor");
    if (rc) {
        printk(ISL_TAG "%s: gpio_request(%#x)=%d\n",
                __func__, int_gpio, rc);
        return rc;
    }

    rc = gpio_tlmm_config(gpio_config_data, GPIO_CFG_ENABLE);
    if (rc) {
        printk(ISL_TAG "%s: gpio_tlmm_config(%#x)=%d\n",
                __func__, gpio_config_data, rc);
        return rc;
    }

    mdelay(1);

    rc = gpio_direction_input(int_gpio);
    if (rc) {
        printk(ISL_TAG "%s: gpio_direction_input(%#x)=%d\n",
                __func__, int_gpio, rc);
        return rc;
    }

    return 0;
}

/* isl29044 probed */
static int isl29044_probe(struct i2c_client *client,
			 const struct i2c_device_id *id)
{
	int err, i;
	u8 reg_dat[8];
	int ret;

	printk(KERN_DEBUG "In isl29044_probe()\n");
	
	/* initial device data struct */	
	isl29044_data.client = client;
	isl29044_data.als_pwr_status = 0;
	isl29044_data.ps_pwr_status = 0;
	isl29044_data.poll_delay = 200;
	isl29044_data.show_als_raw = 0;
	isl29044_data.show_ps_raw = 0;
	isl29044_data.ps_filter_cnt = 0;
	isl29044_data.last_lux = 10; 
	isl29044_data.last_ps_raw = 255;
	isl29044_data.als_chg_range_delay_cnt = 0;
	isl29044_data.intr_data = &isl_irq;
	cfg_taos.prox_gain=0x60;
	
	spin_lock_init(&isl29044_data.als_timer_lock);
	spin_lock_init(&isl29044_data.ps_timer_lock);
	INIT_WORK(&isl29044_data.als_work, &do_als_work);
	INIT_WORK(&isl29044_data.ps_work, &do_ps_work);
	INIT_WORK(&isl29044_data.calib_work, &do_calib_work);
	init_timer(&isl29044_data.als_timer);
	init_timer(&isl29044_data.ps_timer);
	isl29044_data.als_wq = create_workqueue("als wq");
	if(!isl29044_data.als_wq) 
	{
		err= -ENOMEM;
		return err;
	}

	isl29044_data.ps_wq = create_workqueue("ps wq");
	if(!isl29044_data.ps_wq) 
	{
		destroy_workqueue(isl29044_data.als_wq);
		err= -ENOMEM;
		return err;
	}
	
	i2c_set_clientdata(client,&isl29044_data);
	
	/* register irq */
	if(client->irq)
	{		
		isl29044_data.intr_data->irq = client->irq;
		isl29044_data.intr_data->int_gpio=INT_TO_MSM_GPIO(isl29044_data.intr_data->irq);
	}
	printk(KERN_CRIT "ISL use gpio %d  irq %d\n",isl29044_data.intr_data->int_gpio,isl29044_data.intr_data->irq);

	ret=config_int_gpio(isl29044_data.intr_data->int_gpio);
	if (ret) 
	{
		printk(KERN_CRIT "ISL configure int_gpio%d failed\n",	isl29044_data.intr_data->int_gpio);
		return ret;
	}


	ret = request_irq(isl29044_data.intr_data->irq, ps_interrupt, IRQF_TRIGGER_FALLING | IRQF_TRIGGER_RISING, 
		"isl29044", NULL);
	if (ret) 
	{
		printk(KERN_CRIT "ISL request interrupt failed\n");
		return ret;
	}

	/* initial isl29044 */
	err = set_sensor_reg(&isl29044_data);
	if(err < 0) return err;
	
	/* initial als interrupt limit to low = 0, high = 4095, so als cannot
	   trigger a interrupt. We use ps interrupt only */
	reg_dat[5] = 0x00;
	reg_dat[6] = 0xf0;
	reg_dat[7] = 0xff;
	for(i = 5; i <= 7; i++)
	{
		err = i2c_smbus_write_byte_data(client, i, reg_dat[i]);
		if(err < 0) return err;
	}

	/* Add input device register here */
	err = init_input_dev(&isl29044_data);
	if(err < 0)
	{
		destroy_workqueue(isl29044_data.als_wq);
		destroy_workqueue(isl29044_data.ps_wq);
		return err;
	}

	device_create(isl_class, NULL, MKDEV(MAJOR(dev_num), MINOR(dev_num)), 
		&isl29044_driver, "taos");
	return err;
}

/* define IOCTL interface */
static int isl29044_open(struct inode *inode, struct file *filp)
{
	return 0;
}

static int isl29044_close(struct inode *inode, struct file *filp)
{
	return 0;
}

static long isl29044_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	long ret = 0;
	struct isl29044_data_t *dat;
	struct isl29044_cfg_t *cfg;
	dat = container_of(filp->f_dentry->d_inode->i_cdev, struct isl29044_data_t, 
		cdev);
	cfg = dat->cfg;

	switch (cmd) 
	{
		case ISL_IOCTL_ALS_ON:
			printk(KERN_INFO "isl29044 als start");
			ret = set_als_pwr_st(1, dat);
			return (ret);
			break;
			
        case ISL_IOCTL_ALS_OFF:	
			printk(KERN_INFO "isl29044 als stop");
			ret = set_als_pwr_st(0, dat);
			return (ret);
			break;
			
		case ISL_IOCTL_ALS_DATA:
			return (dat->last_lux);
			break;
			
		case ISL_IOCTL_ALS_CALIBRATE:
			return 0;
			break;

		case ISL_IOCTL_CONFIG_GET:
			cfg_taos.prox_threshold_hi = cfg->ps_ht;
			cfg_taos.prox_threshold_lo = cfg->ps_lt;
			cfg_taos.prox_pulse_int = cfg->prox_th_delta_hi;	//use prox_pulse_int to save prox_th_delta_hi, not >30 for HAL limit
			
			ret = copy_to_user((struct taos_cfg *)arg, &cfg_taos, sizeof(struct taos_cfg));
			if (ret) 
			{
				printk(KERN_ERR "ISL: copy_to_user failed in ioctl config_get\n");
				return -ENODATA;
			}
			return (ret);
			break;
			
        case ISL_IOCTL_CONFIG_SET:
            ret = copy_from_user(&cfg_taos, (struct taos_cfg *)arg, sizeof(struct taos_cfg));
			cfg->ps_ht = cfg_taos.prox_threshold_hi;
			
			if(cfg->ps_ht < cfg->prox_th_delta_hi) cfg->ps_lt = 0;
			else cfg->ps_lt = cfg->ps_ht - cfg->prox_th_delta_hi;
			
			cfg->prox_th_delta_hi = cfg_taos.prox_pulse_int;	//use prox_pulse_int to save prox_th_delta_hi, not >30 for HAL limit
			cfg->prox_th_delta_lo = cfg_taos.prox_pulse_int; //use prox_gain to save prox_th_delta_hi

#if(IS_DO_START_UP_CALIB)
			do_startup_calib(dat);
#endif
			return ret;
           	break;
			
		case ISL_IOCTL_PROX_ON:			
			printk(KERN_INFO "isl29044 ps start");
			ret = set_ps_pwr_st(1, dat);
			return (ret);
			break;
			
        case ISL_IOCTL_PROX_OFF:
			printk(KERN_INFO "isl29044 ps stop");
			ret = set_ps_pwr_st(0, dat);
			return (ret);
			break;

		case ISL_IOCTL_PROX_DATA:
			ret = copy_to_user((struct taos_prox_info *)arg, &prox_info, 
				sizeof(struct taos_prox_info));
			if (ret) 
			{
				printk(KERN_ERR "ISL: copy_to_user failed in ioctl config_get\n");
				return -ENODATA;
			}
			return (ret);
            break;
			
        case ISL_IOCTL_PROX_EVENT:
			if(dat->last_ps > 0) return 5;
			else return 0;
	        break;
			
		case ISL_IOCTL_PROX_CALIBRATE:
			printk(KERN_INFO "isl29044 ps start calib");
			do_calib(dat);
			return (calib_out.ret);
			break;

/*
		case ISL_IOCTL_PROX_STARTUP_CALIBRATE:
			ret = do_startup_calib(dat);
			ret = put_user(ret, (unsigned int __user *)arg);
			return (ret);
			break;
					
		case ISL_IOCTL_PROX_GET_ENABLED:
			return put_user(dat->ps_pwr_status, (unsigned long __user *)arg);
			break;
			
		case ISL_IOCTL_ALS_GET_ENABLED:
			return put_user(dat->als_pwr_status, (unsigned long __user *)arg);
			break;
*/

		default:
			return -EINVAL;
			break;
	}
	
	return ret;
}

static struct file_operations isl29044_fops = {
	.owner = THIS_MODULE,
	.open = isl29044_open,
	.release = isl29044_close,
	.unlocked_ioctl = isl29044_ioctl
};


struct i2c_client *isl29044_client;

static int __init isl29044_init(void)
{
	int ret;
		
	printk(KERN_ERR "ISL: comes into isl29044_init\n");

	/* register char device */
	if ((ret = (alloc_chrdev_region(&dev_num, 0, 1, DEVICE_NAME))) < 0) 
	{
		printk(KERN_ERR "ISL: alloc_chrdev_region() failed in isl29044_init()\n");
        return (ret);
	}
	
    isl_class = class_create(THIS_MODULE, DEVICE_NAME);
	//isl_class = class_create(THIS_MODULE, "isl29044");
    cdev_init(&isl29044_data.cdev, &isl29044_fops);
    isl29044_data.cdev.owner = THIS_MODULE;
    if ((ret = (cdev_add(&isl29044_data.cdev, dev_num, 1))) < 0) 
	{
		printk(KERN_ERR "ISL: cdev_add() failed in isl29044_init()\n");
		return (ret);
	}
	
	//device_create(isl_class, NULL, MKDEV(MAJOR(dev_num), MINOR(dev_num)), 
	//	&isl29044_driver, DEVICE_NAME);
	
	/* register the i2c driver for isl29044 */
	ret = i2c_add_driver(&isl29044_driver);
	if(ret < 0) printk(KERN_ERR "Add isl29044 driver error, ret = %d\n", ret);
	printk(KERN_DEBUG "init isl29044 module\n");
	
	return ret;
}

static void __exit isl29044_exit(void)
{
	printk(KERN_DEBUG "exit isl29044 module\n");
	i2c_del_driver(&isl29044_driver);
	cdev_del(&isl29044_data.cdev);
	unregister_chrdev_region(dev_num, 1);
}


MODULE_AUTHOR("Chen Shouxian");
MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("isl29044 ambient light sensor driver");
MODULE_VERSION(DRIVER_VERSION);

module_init(isl29044_init);
module_exit(isl29044_exit);
