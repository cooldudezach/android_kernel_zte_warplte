#include <linux/module.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/pm.h>
#include <linux/input.h>
#include <linux/delay.h>
#include <linux/i2c.h>
#include <linux/gpio.h>
#include <linux/leds.h>
#include <linux/proc_fs.h>
#include <asm/uaccess.h>

struct i2c_client; /* forward declaration */
struct lm3642_leds_platform_data {
	unsigned char timer_iocfg;	/* See ADP1650_REG_TIMER_IOCFG Bits */
	unsigned char current_set;	/* See ADP1650_REG_CURRENT_SET Bits */
	unsigned char output_mode;	/* See ADP1650_REG_OUTPUT_MODE Bits */
	unsigned char control;		/* See ADP1650_REG_CONTROL Bits */
	unsigned char ad_mode;		/* See ADP1650_REG_AD_MODE Bits */
	unsigned char batt_low;		/* See ADP1650_REG_BATT_LOW Bits */

	int gpio_enable;

	/* system specific setup callback */
	int	(*setup)(struct i2c_client *client,
			 unsigned state);
};

struct i2c_client *lm3642_client;
struct lm3642_chip {
	struct i2c_client *client;
	struct led_classdev cdev;
	struct lm3642_leds_platform_data *pdata;
	unsigned char iocfg;
	unsigned char current_set;
	bool use_enable;
};

static inline int lm3642_write(struct i2c_client *client, u8 reg, u8 value)
{
	int ret = i2c_smbus_write_byte_data(client, reg, value);
	if (ret < 0)
		dev_err(&client->dev, "i2c write failed\n");
	return ret;
}

static int lm3642_read(struct i2c_client *client, u8 reg, u8 *buf)
{
	int ret = i2c_smbus_read_byte_data(client, reg);
	if (ret < 0) {
		dev_err(&client->dev, "i2c read failed\n");
		return ret;
	}
	*buf = ret;
	return 0;
}

int  lm3642_set_flash_mode(void)
{
	int rc=0;
	int8_t value  =0;
	pr_err("%s \n",__func__);
	lm3642_write(lm3642_client,0x08,0x57);//IVM-D threshold voltage=3.4v default=0x52
	lm3642_write(lm3642_client,0x09,0x29);//current control 0x28->843.75mA 0x29>937.5mA
	lm3642_write(lm3642_client,0x0A,0xA3);//flash mode
	lm3642_read(lm3642_client,0x0B,&value);
	pr_err("0x0B value =%d ",value);
	gpio_direction_output(18,1);
	if(rc<0)
		pr_err("gpio18 output1 fail");
	return 0;
}
int lm3642_set_torch_mode1(void)
{	int8_t value  =0;
	pr_err("%s \n",__func__);
	lm3642_read(lm3642_client,0x09,&value);
	lm3642_write(lm3642_client,0x09,0x08);
	lm3642_write(lm3642_client,0x0A,0x02);
	return 0;
}
int lm3642_set_torch_mode2(void)
{
	pr_err("%s \n",__func__);
	lm3642_write(lm3642_client,0x09,0x28);//current control torch=140.63mA
	lm3642_write(lm3642_client,0x0A,0x02);//torch mode
	return 0;
}

int lm3642_set_flash_and_torch_mode_off(void)
{	int rc=0;
	pr_err("%s \n",__func__);
	lm3642_write(lm3642_client,0x0A,0x00);
	gpio_direction_output(18,0);
	if(rc<0)
		pr_err("gpio18 output1 fail");
	return 0;
}

static int flash_test_write_proc(struct file *file, const char __user *buffer,
	unsigned long count, void *data)
{
	char buf[count];
	int ret;
  
	if (copy_from_user(buf, buffer, count)) {
		ret = -EFAULT;
		goto write_proc_failed;
	}
    	buf[count - 1] = '\0';
	pr_err("lijing:buf=%s",buf);
        
    	if(!strcmp(buf, "1")) {
		lm3642_set_flash_mode();
	}
	else if(!strcmp(buf, "2")) {
		lm3642_set_torch_mode1();
	}else if(!strcmp(buf, "3")) {
		lm3642_set_torch_mode2();
	} else if(!strcmp(buf, "4")) {
		lm3642_set_flash_and_torch_mode_off();
	}
  
	return count;

write_proc_failed:
	return ret;
}

static int flash_test_read_proc(char *page, char **start, off_t off,
	int count, int *eof, void *data)
{
	char *p = page;
    	int ret;
    	printk(KERN_CRIT "---lijing---msm_cam_read_proc:count=%d",count);

	if (count < 1024) {
		*start = (char *) 0;
		*eof = 0;
		return 0;
	}
	ret = snprintf(p, count,"test");
    	*eof = 1;

	return ret;
}
static int  lm3642_probe(struct i2c_client *client,
			const struct i2c_device_id *id)
			{
	struct lm3642_chip *chip;
	struct proc_dir_entry *d_entry;
	pr_err("%s: entry\n", __func__);

	if (!i2c_check_functionality(client->adapter,
			I2C_FUNC_SMBUS_BYTE_DATA)) 
			{
		pr_err("%s i2c byte data not supported\n",__func__);
		return -EIO;
	}
	chip = kzalloc(sizeof(struct lm3642_chip), GFP_KERNEL);
	if (!chip)
		return -ENOMEM;
	
	chip->client = client;
	lm3642_client = client;
	i2c_set_clientdata(client, chip);

	//struct proc_dir_entry *sensor_name_entry;
	d_entry = create_proc_entry("flash_test",
			S_IRUGO | S_IWUSR | S_IWGRP, NULL);
	if (d_entry) {
		d_entry->read_proc = flash_test_read_proc;
		d_entry->write_proc = flash_test_write_proc;
		d_entry->data = NULL;
	}
	pr_err("lijing:%s,addr=0x%x name=%s\n",__func__,lm3642_client->addr,lm3642_client->adapter->name);
	//lm3642_set_flash_mode();	
       //m3642_set_torch_mode1();	
	   
	return 0;
}

#ifdef CONFIG_PM_SLEEP
static int lm3642_suspend(struct device *dev)
{
	       lm3642_set_flash_and_torch_mode_off();

	return 0;
}

static int lm3642_resume(struct device *dev)
{
	return 0;
}
#endif

static void lm3642_shutdown(struct i2c_client *client)
{	int rc=0;
	pr_err("%s \n",__func__);
	lm3642_write(lm3642_client,0x0A,0x00);
	gpio_direction_output(18,0);
	if(rc<0)
		pr_err("gpio18 output1 fail");
	
}

static SIMPLE_DEV_PM_OPS(lm3642_pm_ops,lm3642_suspend, lm3642_resume);

static const struct i2c_device_id lm3642_id[] = {
	{"lm3642", 0},
	{ },
};
MODULE_DEVICE_TABLE(i2c, adp1650_id);

static struct i2c_driver lm3642_driver = {
	.driver = {
		.name = KBUILD_MODNAME,
		.pm = &lm3642_pm_ops,
		.owner = THIS_MODULE,
	},
	.probe = lm3642_probe,
	.shutdown=lm3642_shutdown,
	//.remove = __devexit_p(lm3642_remove),
	.id_table =lm3642_id,
};

static int __init lm3642_init(void)
{
	pr_err("%s: entry\n", __func__);
	return i2c_add_driver_async(&lm3642_driver);
}
module_init(lm3642_init);
	
static void __exit lm3642_exit(void)
{
	i2c_del_driver(&lm3642_driver);
}
module_exit(lm3642_exit);

MODULE_AUTHOR("Michael Hennerich <michael.hennerich@analog.com>");
MODULE_DESCRIPTION("LM3642 LED Flash Driver");
MODULE_LICENSE("GPL v2");
