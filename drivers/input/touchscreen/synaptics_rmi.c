/* drivers/input/keyboard/synaptics_i2c_rmi.c
 *
 * Copyright (C) 2007 Google, Inc.
 * Copyright (C) 2008 Texas Instrument Inc.
 * Copyright (C) 2009 Synaptics, Inc.
 *
 * provides device files /dev/input/event#
 * for named device files, use udev
 * 2D sensors report ABS_X_FINGER(0), ABS_Y_FINGER(0) through ABS_X_FINGER(7), ABS_Y_FINGER(7)
 * NOTE: requires updated input.h, which should be included with this driver
 * 1D/Buttons report BTN_0 through BTN_0 + button_count
 * TODO: report REL_X, REL_Y for flick, BTN_TOUCH for tap (on 1D/0D; done for 2D)
 * TODO: check ioctl (EVIOCGABS) to query 2D max X & Y, 1D button count
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include <linux/module.h>
#include <linux/input.h>
#include <linux/interrupt.h>
#include <linux/hrtimer.h>
#include <linux/platform_device.h>
#include <linux/i2c.h>
#include <linux/delay.h>
#include <linux/io.h>
#include <linux/gpio.h>
#include <linux/earlysuspend.h>
#include <linux/input/synaptics_rmi.h>
#include <linux/proc_fs.h>
#include <linux/fb.h>
#include <linux/bitops.h>




#define NR_FINGERS	10
DECLARE_BITMAP(pre_fingers, NR_FINGERS);
#if defined (CONFIG_TOUCHSCREEN_RESUME_LOG) || defined (CONFIG_TOUCHSCREEN_UP_TIMER)
static DECLARE_BITMAP(pre_pre_fingers, NR_FINGERS);
#endif
typedef struct _report_data {
	int z;
	int w;
	int x;
	int y;
} report_data;

report_data old_report_data[NR_FINGERS];
report_data new_report_data[NR_FINGERS];

#ifdef CONFIG_TOUCHSCREEN_RESUME_LOG
typedef struct {
	report_data report_data;
	int index;
} log_data;
static log_data log_report_data[NR_FINGERS*2];
static int log_index=0;
static bool synaptics_resume_flag = true;
#endif

extern int g_zte_vid;
extern int g_zte_fw_ver;
extern int zte_fw_info_show(char *page, int len);
extern int zte_fw_latest(void);

static struct workqueue_struct *synaptics_wq;
static struct synaptics_rmi_data *syn_ts = NULL;

#if defined(CONFIG_TOUCHSCREEN_SYNAPTICS_FW)
int syna_update_flag = 0;
extern int syna_fwupdate(struct i2c_client *client, char *pfwfile);
extern int syna_fwupdate_init(struct i2c_client *client);
extern int syna_fwupdate_deinit(struct i2c_client *client);
#endif 
static void synaptics_get_configid(struct synaptics_rmi_data *ts,char *p_chip_type,char *p_sensor, int *p_fw_ver);

#ifdef CONFIG_HAS_EARLYSUSPEND
static void synaptics_early_suspend(struct early_suspend *h);
static void synaptics_late_resume(struct early_suspend *h);
#endif

//#define TOUCHSCREEN_DUPLICATED_FILTER

#ifdef TOUCHSCREEN_DUPLICATED_FILTER
static inline int duplicated_filter(struct synaptics_rmi_data *ts, 
						int i, int z, int w, int x, int y)
{
	int drift_x, drift_y;
	
	/* up pointer, report anyway and clean the entry */
	if (z == 0) {
		memset((void *)&old_report_data[i], 0, sizeof(old_report_data[0]));
		return 0;
	}

	/* drift small enough, don't report */
	drift_x = abs(old_report_data[i].x - x);
	drift_y = abs(old_report_data[i].y - y);
	if (drift_x < ts->dup_threshold && drift_y < ts->dup_threshold)
		return 1;

	/* normal case, report it and cache the new data */
	old_report_data[i].z = z;
	old_report_data[i].w = w;
	old_report_data[i].x = x;
	old_report_data[i].y = y;

	return 0;
}

#endif /* TOUCHSCREEN_DUPLICATED_FILTER */



static int detect_device(struct i2c_client *client)
{
	int i, buf;
	bool ret = 0;

	if ( client == NULL )
		return ret;

	for (i=0; i<3; i++ )
	{
		// 0xFF: synaptics rmi page select register
		buf = i2c_smbus_read_byte_data(client, 0xFF);
		if ( buf >= 0 ){
			ret = 1;
			break;
		}
		msleep(10);
	}

	return ret;
}

static int get_screeninfo(uint *xres, uint *yres)
{
	struct fb_info *info;

	info = registered_fb[0];
	if (!info) {
		pr_err("%s: Can not access lcd info \n",__func__);
		return -ENODEV;
	}

	*xres = info->var.xres;
	*yres = info->var.yres;
	printk("lcd ( %d, %d ) \n", *xres, *yres );

	return 1;
}


static int
proc_read_val(char *page, char **start, off_t off, int count, int *eof,
			  void *data)
{
	int len = 0;
	char chiptype[16], sensor[16];
	int fw_ver;
	
	if ( syn_ts == NULL)
		return -1;
	
	len += sprintf(page + len, "manufacturer : %s\n", "Synaptics");

	synaptics_get_configid( syn_ts, (char *)&chiptype,(char *)&sensor, &fw_ver);
	len += sprintf(page + len, "chip type : %s\n", chiptype );
	len += sprintf(page + len, "i2c address : %02x\n", 0x22);
	len += sprintf(page + len, "module : %s\n", sensor );
	len += sprintf(page + len, "fw version : %c%c\n", fw_ver&0x000000ff, (fw_ver&0x0000ff00)>>8);
#ifdef CONFIG_TOUCHSCREEN_SYNAPTICS_FW
	len += sprintf(page + len, "update flag : %x\n", syna_update_flag);
#endif
	len += sprintf(page + len, "lastest flag : %x\n", zte_fw_latest());
	len = zte_fw_info_show(page, len);	

	if (off + count >= len)
		*eof = 1;

	if (len < off)
		return 0;

	*start = page + off;
	return ((count < len - off) ? count : len - off);
}
		
static int proc_write_val(struct file *file, const char *buffer,
		   unsigned long count, void *data)
{
	unsigned long val;
	sscanf(buffer, "%lu", &val);

#if defined(CONFIG_TOUCHSCREEN_SYNAPTICS_FW)
	syna_update_flag = 0;

	disable_irq(syn_ts->client->irq);
	if(syna_fwupdate(syn_ts->client, syn_ts->fwfile)<0)
	{
		enable_irq(syn_ts->client->irq);
		syna_update_flag = 1;
		pr_info("syna fw update fail! \n" );
		return -EINVAL;
	}
	
	//msleep(500);
	//升级完成后需要对一些参数再做配置
	//尤其是坐标系最大值的配置，否则会导致升级后触摸屏不可用
	//后续需要添加其他机型的配置
	//i2c_smbus_write_byte_data(syn_ts->client, syn_ts->f11.ctrl_base+2, 0x9);
	//i2c_smbus_write_byte_data(syn_ts->client, syn_ts->f11.ctrl_base+3, 0x9);	
	enable_irq(syn_ts->client->irq);
	
	syna_update_flag = 2;
	pr_info("syna fw update Ok! \n" );
#endif

	return -EINVAL;
}


static int
proc_read_fw_lastest_flag(char *page, char **start, off_t off, int count, int *eof,
	  void *data)
{
	int len = 0;
	
	if ( syn_ts == NULL)
		return -1;

	synaptics_get_configid( syn_ts, NULL, NULL, NULL);
	len += sprintf(page + len, "lastest flag : %x\n", zte_fw_latest());

	if (off + count >= len)
		*eof = 1;
	if (len < off)
		return 0;
	*start = page + off;
	return ((count < len - off) ? count : len - off);
}

static int synaptics_get_pdt(struct synaptics_rmi_data *ts)
{
	int ret=0;
	int nFd =1;
	int page_select=0;
	int pdt_addr_base=0;
	int intr_count=0;
	__u8 query[11];
	struct rmi_function_descriptor fd;
	int data_len=0;

	ts->data_base= 0xff;
	ts->data_len = 0;

	//pr_info("synaptics_ts: synaptics_read_pdt\n");
	do {

		//now we do nothing to pages other than PAGE 0. 
		/*
		if (page_select!=0){
			ret = i2c_smbus_write_byte_data(ts->client, PDT_PAGE_SELECT, page_select);
			if (ret<0)
				pr_err("i2c_smbus_write_byte_data failed\n");
		}
		*/
		//pr_info("--------------------------------------\n");
		pdt_addr_base=PDT_ADDR_BASE;  //page description table
		ret = i2c_smbus_read_i2c_block_data(ts->client,
			pdt_addr_base-PDT_BYTE_COUNT*nFd+1, sizeof(struct rmi_function_descriptor),(uint8_t *)&fd);
		//pr_info("synaptics_ts: page=%d,pdt_addr_base=0x%x,nfd=%d,addr=0x%x\n",
		//	page_select,pdt_addr_base,nFd,PDT_ADDR_BASE-PDT_BYTE_COUNT*nFd+1);
		//pr_info("function_number=0x%x, query_base=0x%x, cmd_base=0x%x, "
		//	"ctrl_base=0x%x, data_base=0x%x, intr_src_count=0x%x\n",
		//	fd.function_number,fd.query_base,fd.cmd_base,
		//	fd.ctrl_base,fd.data_base,fd.intr_src_count);
		if (ret < 0){
			pr_err("failed to get ts intr state\n");
			return ret;
		}else{
			/*
			 * -----  RMI functions  -----
			 * Function		Purpose
			 *	$01			RMI Device Control
			 *  $05			Analog
			 *	$08			BIST
			 *	$09			BIST
			 *	$11			2-D TouchPad sensors
			 *	$19			0-D capacitive button sensors
			 *	$30			GPIO/LEDs (includes mechanical buttons)
			 *	$32			Timer
			 *	$34			Flash Memory Management
			*/
			switch(fd.function_number){
			case 0x01:
				ts->f01.flag		= fd.function_number;
				ts->f01.query_base  = fd.query_base;
				ts->f01.ctrl_base	= fd.ctrl_base;
				ts->f01.data_offset	= fd.data_base;	//intr status
				ts->f01.intr_offset = intr_count/8;
				ts->f01.intr_mask 	= ((1<<(fd.intr_src_count &0x7))-1)<<(intr_count%8);
				//pr_info("synaptics_ts:intr_offset=0x%x, intr_mask=0x%x\n",
				//	ts->f01.intr_offset,ts->f01.intr_mask);
				ts->f01.data_len = data_len = 0 ;
				break;
			case 0x11:
				ts->f11.flag		= fd.function_number;
				ts->f11.query_base	= fd.query_base;
				ts->f11.ctrl_base 	= fd.ctrl_base;
				ts->f11.data_offset = fd.data_base;
				ts->f11.intr_offset = intr_count/8;
				ts->f11.intr_mask 	= ((1<<(fd.intr_src_count &0x7))-1)<<(intr_count%8);
				//pr_info("synaptics_ts:intr_offset=0x%x, intr_mask=0x%x\n",
				//	ts->f11.intr_offset,ts->f11.intr_mask);
				//i2c_smbus_write_word_data(ts->client, fd.ctrl_base+6,480);
				//i2c_smbus_write_word_data(ts->client, fd.ctrl_base+8,800);

				ts->max[0]=i2c_smbus_read_word_data(ts->client, fd.ctrl_base+6);
				ts->max[1]=i2c_smbus_read_word_data(ts->client, fd.ctrl_base+8);

				// border pixel filter
				pr_info("ts max(%d,%d)\n", ts->max[0], ts->max[1]);
				ts->max[0] -= 2*SYNAPTICS_BORDER_PIXEL;
				ts->max[1] -= 2*SYNAPTICS_BORDER_PIXEL;
				pr_info("ts max(%d,%d)\n", ts->max[0], ts->max[1]);

				//ts->f11_fingers = kcalloc(ts->f11.points_supported,
				//	sizeof(*ts->f11_fingers), 0);

				ret=i2c_smbus_read_word_data(ts->client, fd.query_base);
				//printk("%s: read query register=0x%X\n", __func__, ret);
				ret = i2c_smbus_read_i2c_block_data(ts->client,
					fd.query_base,sizeof(query),query);
				if (ret < 0){
					pr_err("%s: i2c_smbus_read_byte_data failed\n",__func__);
					return ret;
				}
				/*{
					int i;
					for (i=0;i<=11;i++)
						pr_info("%s: read f11.query[%d]=0x%X\n", __func__, i ,query[i]);
				}*/

				ts->f11.points_supported=(query[1]&0x7)+1;
				if (ts->f11.points_supported >5)
					ts->points_supported=10;
				else
					ts->points_supported=ts->f11.points_supported;
				//pr_info("synaptics_ts: max_p_s=%d, max_p_n=%d\n",
				//	ts->f11.points_supported,ts->points_supported);

				/*
				// We now consider only ONE sensor situation.
				if (query[0]&0x7 !=1){
					pr_err("i2c_smbus_read_byte_data failed\n");
					return -1;//synaptics_ts: need a better error code
				}
				pr_info("synaptics_ts: sensor=%d\n",query[0]&0x7);
				*/
				ts->f11.data_len = data_len =
					// DATA0 : finger status, four fingers per register 
					(ts->points_supported/4+1)
					// DATA1.*~ DATA5.*: hasAbs =1, F11_2D_Query1.
					//absolute finger position data, 5 per finger 
					+ ( (query[1]&(0x1<<4)) ? (5*ts->points_supported):0)
					// DATA6.*~ DATA7.*: hasRel = 1, F11_2D_Query1:
					+ ( (query[1]&(0x1<<3)) ? (2*ts->points_supported):0)
					// DATA8: F11_2D_Query7 !=0
					+ ( query[7] ? 1 : 0)
					// DATA9: F11_2D_Query7 or 8 !=0
					+ ((query[7] || query[8]) ? 1 : 0)
					// DATA10: 	hasPinch=1 of hasFlick=1, F11_2D_Query7
					+ ( (query[7]&(0x1<<6))||(query[7]&(0x1<<4)) ? 1:0)
					// DATA11,12: hasRotate=1,F11_2D_Query8 or hasFlick=1 ,F11_2D_Query7
					+ ( (query[7]&(0x1<<4)|| query[8]&(0x1<<2)) ? 2:0)
					// DATA13.*:	hasTouchShapes =1 ,F11_2D_Query8
					+ ( (query[8]&(0x1<<3))? ((query[10]&0x1F)/8+1):0 )
					// DATA14,15:	HasScrollZones=1, F11_2D_Query8
					+ ( (query[8]&(0x1<<3)) ? 2 : 0)
					// DATA16,17:	IndividualScrollZones=1, F11_2D_Query8
					+ ( (query[8]&(0x1<<4)) ? 2 : 0)
					;
				//pr_info("synaptics_ts: data len=%d\n",ts->f11.data_len);
				break;
/*			case 0x05:
			case 0x08:
			case 0x09:
			case 0x19:
			case 0x30:*/
			case 0x34:
				ts->f34.flag		 = fd.function_number;
				ts->f34.query_base = fd.query_base;
				ts->f34.ctrl_base 	 = fd.ctrl_base;
				ts->f34.data_offset  = fd.data_base;
				ts->f34.intr_mask 	 = ((1<<(fd.intr_src_count &0x7))-1)<<(intr_count%8);
				break;	
			case 0x00://page discription table end
				page_select++;
				nFd=0;
				//pr_info("synaptics_ts: next page?\n");
			default:
				data_len=0;
				break;
			}
			
		}
		
		intr_count+=fd.intr_src_count;

		//pr_info("1 data_base =0x%x, len=%d\n", ts->data_base,ts->data_len);
		if ( fd.data_base && (ts->data_base > fd.data_base) )
			ts->data_base = fd.data_base;

		ts->data_len += data_len;
		//pr_info("data_len=%d\n",data_len);
		//pr_info("3 data_base =0x%x, len=%d\n", ts->data_base,ts->data_len);

		nFd++;

	}while( page_select<1);

	ts->f01.data_len = 1+ (intr_count+7)/8;
	//pr_info("synaptics_ts: f01 data len =%d\n",ts->f01.data_len);

	//check data base & data len
	ts->data_len += ts->f01.data_len;
	//pr_info("%s: data_base=0x%x, data_len=%d\n",__func__,ts->data_base,ts->data_len);

	ts->f01.data_offset -= ts->data_base;
	ts->f11.data_offset -= ts->data_base;
	//pr_info("synaptics_ts: f01 offset =%d, f11 offset=%d\n",ts->f01.data_offset, ts->f11.data_offset);

	return ret;
}


static void synaptics_get_configid(
	struct synaptics_rmi_data *ts,
	char *p_chip_type,
	char *p_sensor,
	int *p_fw_ver )
{
	int ret;

	if ( !ts )
		return;

	ret = i2c_smbus_read_i2c_block_data(ts->client, ts->f34.ctrl_base, 4, (char *)&ts->config_id);
	if (ret < 0){
		pr_err("%s: failed to get ts f34.ctrl_base\n",__func__);
	}	

	pr_info("chip_type=0x%x, sensor=0x%x, fw_ver=0x%x\n", 		
		ts->config_id.chip_type,
		ts->config_id.sensor,
		ts->config_id.fw_ver);
	
	g_zte_vid = ts->config_id.sensor;
	g_zte_fw_ver = ts->config_id.fw_ver;
	g_zte_fw_ver =((g_zte_fw_ver&0x000000ff)<<8)|( (g_zte_fw_ver&0x0000ff00)>>8);//adjust fw_ver byte order	

	if ( !p_chip_type || !p_sensor || !p_fw_ver )
		return;
	
	switch (ts->config_id.chip_type){
	case '1':
		sprintf(p_chip_type,"S2200");
		break;
	case '2':
		sprintf(p_chip_type,"S2202");
		break;
	case '3':
		sprintf(p_chip_type,"S3200");
		break;
	case '4':
		sprintf(p_chip_type,"S3202");
		break;
	case '5':
		sprintf(p_chip_type,"S3203");
		break;
	case '6':
		sprintf(p_chip_type,"S7020");
		break;
	case '7':
		sprintf(p_chip_type,"S7300");
		break;
	default:
		sprintf(p_chip_type,"unknown");
		break;
	}

	switch(ts->config_id.sensor){
	case '1':
		sprintf(p_sensor, "TPK(0x%x)",ts->config_id.sensor );
		break;
	case '2':
		sprintf(p_sensor, "Truly(0x%x)",ts->config_id.sensor);
		break;
	case '3':
		sprintf(p_sensor, "Success(0x%x)",ts->config_id.sensor);
		break;
	case '4':
		sprintf(p_sensor, "Ofilm(0x%x)",ts->config_id.sensor);
		break;
	case '5':
		sprintf(p_sensor, "Lead(0x%x)",ts->config_id.sensor);
		break;
	case '6':
		sprintf(p_sensor, "Wintek(0x%x)",ts->config_id.sensor);
		break;
	case '7':
		sprintf(p_sensor, "Laibao(0x%x)",ts->config_id.sensor);
		break;
	case '8':
		sprintf(p_sensor, "CMI(0x%x)",ts->config_id.sensor);
		break;
	case '9':
		sprintf(p_sensor, "ECW(0x%x)",ts->config_id.sensor);
		break;
	case 'A':
		sprintf(p_sensor, "Goworld(0x%x)",ts->config_id.sensor);
		break;
	case 'B':
		sprintf(p_sensor, "Baoming(0x%x)",ts->config_id.sensor);
		break;				
	case 'E':
		sprintf(p_sensor, "JUNDA(0x%x)",ts->config_id.sensor);
		break;
	default:
		sprintf(p_sensor, "unknown(0x%x)",ts->config_id.sensor);
		break;
	}

	*p_fw_ver = ts->config_id.fw_ver;

	pr_info("chip: %s, sensor %s, fw 0x%x \n", p_chip_type, p_sensor, *p_fw_ver);

	return;
}

static int synaptics_set_panel_state(
	struct synaptics_rmi_data *ts, 
	int state
	)
{
	int ret=0;
	uint8_t mode=0;

	if (!ts)
		return ret;

	switch (state){
	case TS_POWER_ON:
		// set panel maxmum X,Y position
		//set_max_y( ts,ts->max[0], ts->max[1]);

		/*
		 * ReportingMode = ‘001’: 
		 * Reduced reporting mode In this mode, the absolute data
		 * source interrupt is asserted whenever a finger arrives 
		 * or leaves. Fingers that are present but basically 
		 * stationary do not generate additional interrupts 
		 * unless their positions change significantly. 
		 * In specific, for fingers already touching the pad, 
		 * the interrupt is asserted whenever the change in finger 
		 * position exceeds either DeltaXPosThreshold or DeltaYPosThreshold.
		*/
		ret = i2c_smbus_write_byte_data(ts->client, ts->f11.ctrl_base+2, 0x2);
		if (ret<0) pr_err("%s: i2c_smbus_write_byte_data failed\n",__func__);
		ret = i2c_smbus_write_byte_data(ts->client, ts->f11.ctrl_base+3, 0x2);
		if (ret<0) pr_err("%s: i2c_smbus_write_byte_data failed\n",__func__);
		mode = i2c_smbus_read_byte_data(ts->client, ts->f11.ctrl_base);
		mode &=~0x7;
		mode |=0x01;
		ret = i2c_smbus_write_byte_data(ts->client, ts->f11.ctrl_base, mode);
		if (ret<0) pr_err("%s: i2c_smbus_write_byte_data failed\n",__func__);
	case TS_RESUME:	
		// set nomal mode
		mode = i2c_smbus_read_byte_data(ts->client, ts->f01.ctrl_base);
		mode &=~0x7;
		ret = i2c_smbus_write_byte_data(ts->client, ts->f01.ctrl_base, mode);
		if (ret<0) pr_err("%s: i2c_smbus_write_byte_data failed\n",__func__);
		//i2c_smbus_write_word_data(ts->client, ts->f11.ctrl_base+6,480);
		//i2c_smbus_write_word_data(ts->client, ts->f11.ctrl_base+8,800);
		//i2c_smbus_write_byte_data(ts->client, ts->f11.ctrl_base+2,0x10);
		//i2c_smbus_write_byte_data(ts->client, ts->f11.ctrl_base+3,0x10);

		//enable irq
		ret = i2c_smbus_write_byte_data(ts->client,ts->f01.ctrl_base+1, 0x7f);
		break;
	case TS_SUSPEND:
		// disable irq
		ret = i2c_smbus_write_byte_data(ts->client, ts->f01.ctrl_base+1, 0);
		if (ret <0) pr_err("%s: i2c_smbus_write_byte_data failed\n",__func__);

		// deep sleep
		mode = i2c_smbus_read_byte_data(ts->client, ts->f01.ctrl_base);
		mode &=~0x3;
		mode |=0x01;
		ret = i2c_smbus_write_byte_data(ts->client, ts->f01.ctrl_base, mode);
		if (ret<0) pr_err("%s: i2c_smbus_write_byte_data failed\n",__func__);
		break;
	default:
		break;
	}
	return ret;
}

#ifdef CONFIG_TOUCHSCREEN_RESUME_LOG
static void synaptics_log1(int i)
{	
	memcpy(&(log_report_data[log_index].report_data), &(new_report_data[i]), sizeof(report_data));
	log_report_data[log_index].index = i;
	log_index++;	
}

static void synaptics_log2(void)
{	
	int index_max;
	//struct timespec ts1, ts2, ts;
	//ktime_get_ts(&ts1);
	index_max = log_index;
	
	if(log_index>=10) 
	{
		while(log_index!=0)
		{
			printk("pointer %d (x=0x%x, y=0x%x, z=0x%x)\n", 
				log_report_data[index_max-log_index].index,
				log_report_data[index_max-log_index].report_data.x, 
				log_report_data[index_max-log_index].report_data.y, 
				log_report_data[index_max-log_index].report_data.z
				);
			log_index--;			
		}
		//ktime_get_ts(&ts2);
		//ts = timespec_sub(ts2, ts1);
		//printk("xym : %d data, %ld ns------\n", index_max, ts.tv_nsec);	
		synaptics_resume_flag = false;
	}
}

static void synaptics_log3(void)
{	
	int index_max;
	//struct timespec ts1, ts2, ts;
	//ktime_get_ts(&ts1);
	index_max = log_index;

	{
		while(log_index!=0)
		{
			//printk("pointer %d (x=%d, y=%d, z=%d)\n", 
			printk("pointer %d (x=0x%x, y=0x%x, z=0x%x)\n", 
				log_report_data[index_max-log_index].index,
				log_report_data[index_max-log_index].report_data.x, 
				log_report_data[index_max-log_index].report_data.y, 
				log_report_data[index_max-log_index].report_data.z
				);
			log_index--;			
		}
		//ktime_get_ts(&ts2);
		//ts = timespec_sub(ts2, ts1);
		//printk("xym : %d data, %ld ns------\n", index_max, ts.tv_nsec);	
		synaptics_resume_flag = false;
	}
}

#endif

static void synaptics_work_func(struct work_struct *work)
{
	int ret=0;
	struct synaptics_rmi_data *ts = container_of(work, struct synaptics_rmi_data, work);
	__u16 interrupt	= 0;
	int buf_len		= ts->data_len;
	__u8 buf[buf_len];
#if defined (CONFIG_MACH_WARPLTE)|| defined (CONFIG_MACH_COEUS)	|| defined (CONFIG_MACH_GAEA)||defined(CONFIG_MACH_NESTOR)
	int buttonx=0, buttony=0;
#endif
	struct f11_2d_point_data *point_data=NULL;
	int i,x,y,w,z,finger,will_report,read_num=0;

#if defined (CONFIG_TOUCHSCREEN_UP_TIMER)
	//int will_start_up_timer=0;
	int down_point_num;
#endif		
	
	DECLARE_BITMAP(pointers_to_report, NR_FINGERS);

	//time_mono_start= ktime_get();
	//printk("synaptics_work_func start %ld\n",(long int )ktime_to_ns(time_mono));
/*
	if(buf_len<=30)
	ret = i2c_smbus_read_i2c_block_data(ts->client,ts->data_base, buf_len, buf);	
	else
		if((buf_len>30)&&(buf_len<=60))
		{
		ret = i2c_smbus_read_i2c_block_data(ts->client,ts->data_base, 30, buf);
		ret = i2c_smbus_read_i2c_block_data(ts->client,ts->data_base+30, buf_len-30, buf+30);
		}
		else 
			if(buf_len>60)
			{
        		ret = i2c_smbus_read_i2c_block_data(ts->client,ts->data_base, 30, buf);
        		ret = i2c_smbus_read_i2c_block_data(ts->client,ts->data_base+30, 30, buf+30);
			ret = i2c_smbus_read_i2c_block_data(ts->client,ts->data_base+60, buf_len-60, buf+60);
			}
*/
	ret = i2c_smbus_read_i2c_block_data(ts->client,ts->data_base, 
	ts->f11.data_offset+ts->points_supported/4+1, buf);	

	/*for (i=0;i<(ts->f11.data_offset+ts->points_supported/4+1);i++)
		pr_info("%s: addr=0x%x, buf[%d]=0x%x\n",__func__, ts->data_base+i,i,buf[i]);
*/

	// now only consider 
	interrupt=buf[1];
#if defined (CONFIG_TOUCHSCREEN_UP_TIMER)
	if (interrupt!=4)
	printk("synaptics_ts: intr=0x%x--------------\n",interrupt);
#endif
	
#if defined (CONFIG_MACH_WARPLTE)	|| defined (CONFIG_MACH_GAEA)||defined(CONFIG_MACH_NESTOR)
	if (interrupt&0x20)
	{
		i2c_smbus_write_byte_data(ts->client,0xff,2);
		ret = i2c_smbus_read_i2c_block_data(ts->client,0x00, buf_len, buf);
		printk("buf[0]:0x%x \n",buf[0]);
		if(buf[0]&0x01)
			buttonx=ts->max[0]/6;
		if(buf[0]&0x02)
			buttonx=ts->max[0]*3/6;		
		if(buf[0]&0x04)
			buttonx=ts->max[0]*5/6;	
		printk("ts->max[1]:%d\n",ts->max[1]);
		buttony=ts->max[1]*1360/1280;
		printk("buttony:%d\n",buttony);
		if(buf[0]&0x7){
			input_report_key(ts->input_dev, BTN_TOUCH, 1);
			input_report_abs(ts->input_dev, ABS_MT_TRACKING_ID, 0);
			input_report_abs(ts->input_dev, ABS_MT_TOUCH_MAJOR, 10);
			input_report_abs(ts->input_dev, ABS_MT_WIDTH_MAJOR, 10);//default 10
			input_report_abs(ts->input_dev, ABS_MT_POSITION_X, buttonx);
			input_report_abs(ts->input_dev, ABS_MT_POSITION_Y, buttony);
			input_report_abs(ts->input_dev, ABS_MT_PRESSURE, 10);
			}
		input_mt_sync(ts->input_dev);
		input_sync(ts->input_dev);
		
		i2c_smbus_write_byte_data(ts->client,0xff,0);
	}else

#elif defined (CONFIG_MACH_COEUS) //P893A21 LCD 540*960 only for LAIBAO TP 707*1267
	if (interrupt&0x20)
	{
		i2c_smbus_write_byte_data(ts->client,0xff,2);
		ret = i2c_smbus_read_i2c_block_data(ts->client,0x00, buf_len, buf);
		printk("buf[0]:0x%x \n",buf[0]);
		if(buf[0]&0x01)
			buttonx=ts->max[0]/6;
		if(buf[0]&0x02)
			buttonx=ts->max[0]*3/6;		
		if(buf[0]&0x04)
			buttonx=ts->max[0]*5/6;	
		printk("ts->max[1]:%d\n",ts->max[1]);
		buttony=ts->max[1]*1020/960;
		printk("buttonx:%d, buttony:%d\n", buttonx, buttony);
		if(buf[0]&0x7){
			input_report_key(ts->input_dev, BTN_TOUCH, 1);
			input_report_abs(ts->input_dev, ABS_MT_TRACKING_ID, 0);
			input_report_abs(ts->input_dev, ABS_MT_TOUCH_MAJOR, 10);
			input_report_abs(ts->input_dev, ABS_MT_WIDTH_MAJOR, 10);//default 10
			input_report_abs(ts->input_dev, ABS_MT_POSITION_X, buttonx);
			input_report_abs(ts->input_dev, ABS_MT_POSITION_Y, buttony);
			input_report_abs(ts->input_dev, ABS_MT_PRESSURE, 10);
			}
		input_mt_sync(ts->input_dev);
		input_sync(ts->input_dev);
		
		i2c_smbus_write_byte_data(ts->client,0xff,0);
	}else
	
#endif

	if ( ts->f11.flag && (interrupt & ts->f11.intr_mask) ) 
	{
		// Gesture:google likes to compute guestures itself instead of hw reports.
		/* Figure out which pointer(s) should be reported */
		bitmap_zero(pointers_to_report, NR_FINGERS);
		will_report = 0;
#if defined (CONFIG_TOUCHSCREEN_RESUME_LOG) || defined (CONFIG_TOUCHSCREEN_UP_TIMER)
		memcpy(pre_pre_fingers, pre_fingers, sizeof(*pre_fingers));
#endif
		for (i=0; i<ts->points_supported;i++)
		{
			finger = 0x3 &(buf[ts->f11.data_offset+i/4] >> ((i%4)*2));
			if (finger)
				read_num = i+1;
		}
		if (read_num > 0&&read_num <= 6)
		ret = i2c_smbus_read_i2c_block_data(ts->client,
		ts->data_base+ts->f11.data_offset + (1 + (ts->points_supported - 1)/4), 
		read_num*sizeof(struct f11_2d_point_data), 
		buf+ts->f11.data_offset+(1 + (ts->points_supported - 1)/4));
		else if(read_num > 6&&read_num <= 12){
			ret = i2c_smbus_read_i2c_block_data(ts->client,
			ts->data_base+ts->f11.data_offset + (1 + (ts->points_supported - 1)/4), 
			30, 
			buf+ts->f11.data_offset+(1 + (ts->points_supported - 1)/4));
			
			ret = i2c_smbus_read_i2c_block_data(ts->client,
			ts->data_base+ts->f11.data_offset + (1 + (ts->points_supported - 1)/4)+30, 
			read_num*sizeof(struct f11_2d_point_data)-30, 
			buf+ts->f11.data_offset+(1 + (ts->points_supported - 1)/4)+30);
			}
			
		for (i=0; i<ts->points_supported;i++)
		{
			finger = 0x3 &(buf[ts->f11.data_offset+i/4] >> ((i%4)*2));
			if (finger)	
			{/* active or down pointers, should report */
				__set_bit(i, pre_fingers);
				__set_bit(i, pointers_to_report);
			} 
			else if (test_bit(i, pre_fingers)) 
			{/* finger == 0, but previous finger == 1, up pointers, should report */
				__clear_bit(i, pre_fingers);
				__set_bit(i, pointers_to_report);
			} 
			else	/* inactive pointers, do nothing */
				continue;
		if(test_bit(i, pre_fingers)){	
		//ret = i2c_smbus_read_i2c_block_data(ts->client,
		//ts->data_base+ts->f11.data_offset + (1 + (ts->points_supported - 1)/4)+ i * sizeof(struct f11_2d_point_data), 
		//sizeof(struct f11_2d_point_data), 
		//buf+ts->f11.data_offset+(1 + (ts->points_supported - 1)/4)+i * sizeof(struct f11_2d_point_data));
			point_data = (struct f11_2d_point_data *)
					( buf 
					+ ts->f11.data_offset 
					+ (1 + (ts->points_supported - 1)/4)
					+ i * sizeof(struct f11_2d_point_data)
					);

			x =((__u16)((point_data->xh<<0x4)|(point_data->xyl &0xF)));
			y =((__u16)((point_data->yh<<0x4)|(point_data->xyl &0xF0)>>0x4));
			w = ( point_data->wxy & 0x0F ) + ( point_data->wxy >>4 & 0x0F );
			z = point_data->z;
 
			// border pixel filter
			x -= SYNAPTICS_BORDER_PIXEL;
			y -= SYNAPTICS_BORDER_PIXEL;

			x = ( x < 0 ) ? 0 : x ;
			x = ( x > ts->max[0] ) ? ts->max[0] : x ;
			y = ( y < 0 ) ? 0 : y ;
			y = ( y > ts->max[1] ) ? ts->max[1] : y ;

			new_report_data[i].w = w;
			new_report_data[i].z = z;
			new_report_data[i].x = x;
			new_report_data[i].y = y;

#ifdef TOUCHSCREEN_DUPLICATED_FILTER
			if (duplicated_filter(ts, i, z, w, x, y))
				continue;
#endif
			}
			will_report = 1;
		}
		
#if defined (CONFIG_TOUCHSCREEN_UP_TIMER)
		down_point_num=0;
		for(i=0; i<ts->points_supported;i++)
		{
			if(test_bit(i, pre_fingers))  down_point_num++;				
		}
		//printk("synaptics_ts: down_point_num = %d\n", down_point_num);
		if(0==down_point_num)
		{//所有的点都已经抬起，则取消up_timer
			hrtimer_cancel(&ts->up_timer);
			//printk("synaptics_ts: CANCEL up_timer!!! \n");
		}		
		//if(will_start_up_timer)
		//{//所有的点中任意一个点有新按下动作，则启动up_timer
		//	ret = hrtimer_start(&ts->up_timer, ktime_set(1, 0), HRTIMER_MODE_REL);
			//if(1==ret)
			//	printk("synaptics_ts: RESTART up_timer\n");
			//else
			//	printk("synaptics_ts: START up_timer\n");
		//}				
#endif
		if (will_report) {
			for_each_set_bit(i, pointers_to_report, NR_FINGERS) {
				/*ergate-008*/
				if(test_bit(i,pre_fingers))
				{
				input_report_key(ts->input_dev, BTN_TOUCH, 1);
				input_report_abs(ts->input_dev, ABS_MT_TRACKING_ID, i);
				input_report_abs(ts->input_dev, ABS_MT_TOUCH_MAJOR, new_report_data[i].z);
				input_report_abs(ts->input_dev, ABS_MT_WIDTH_MAJOR, new_report_data[i].w);//default 10
				input_report_abs(ts->input_dev, ABS_MT_POSITION_X, new_report_data[i].x);
				input_report_abs(ts->input_dev, ABS_MT_POSITION_Y, new_report_data[i].y);
				input_report_abs(ts->input_dev, ABS_MT_PRESSURE, new_report_data[i].z);
				}
				input_mt_sync(ts->input_dev);
#ifdef CONFIG_TOUCHSCREEN_RESUME_LOG
				if(synaptics_resume_flag )
				{
					if(test_bit(i, pre_fingers) != test_bit(i, pre_pre_fingers))
					{
						synaptics_log1(i);
					}
				}
#endif
			}
			
			input_sync(ts->input_dev);
#ifdef CONFIG_TOUCHSCREEN_RESUME_LOG
			if(synaptics_resume_flag)
				synaptics_log2();
#endif
		}
	}

	if (ts->use_irq)
		enable_irq(ts->client->irq);

}

static enum hrtimer_restart synaptics_timer_func(struct hrtimer *timer)
{
	struct synaptics_rmi_data *ts = container_of(timer,struct synaptics_rmi_data, timer);
	queue_work(synaptics_wq, &ts->work);
	hrtimer_start(&ts->timer, ktime_set(0, 12 * NSEC_PER_MSEC),HRTIMER_MODE_REL);
	return HRTIMER_NORESTART;
}
#if defined (CONFIG_TOUCHSCREEN_UP_TIMER)
static enum hrtimer_restart synaptics_up_timer_func(struct hrtimer *timer)
{
	struct synaptics_rmi_data *ts = container_of(timer,struct synaptics_rmi_data, up_timer);
	disable_irq_nosync(ts->client->irq);	
	hrtimer_start(&ts->up_timer, ktime_set(1, 0), HRTIMER_MODE_REL);
	queue_work(synaptics_wq, &ts->work);	
	printk("synaptics_up_timer_func\n");
	return HRTIMER_NORESTART;
}
#endif
irqreturn_t synaptics_irq_handler(int irq, void *dev_id)
{
	struct synaptics_rmi_data *ts = dev_id;
#if defined (CONFIG_TOUCHSCREEN_UP_TIMER)	
	hrtimer_start(&ts->up_timer, ktime_set(1, 0), HRTIMER_MODE_REL);
#endif
	disable_irq_nosync(ts->client->irq);
	queue_work(synaptics_wq, &ts->work);
	return IRQ_HANDLED;
}

static int synaptics_probe(
	struct i2c_client *client, 
	const struct i2c_device_id *id)
{
	struct synaptics_rmi_data *ts;
	struct synaptics_rmi_data *pdata;
	struct proc_dir_entry *dir, *refresh, *fw_lastest_flag;
	int ret = 0;
	uint max_x,max_y;
	int xres, yres;	// LCD x,y resolution


	pr_info("%s enter\n", __func__);

	if (!i2c_check_functionality(client->adapter, I2C_FUNC_I2C)) {
		pr_err("%s: need I2C_FUNC_I2C\n", __func__);
		ret = -ENODEV;
		goto err_check_functionality_failed;
	}

	ts = kzalloc(sizeof(*ts), GFP_KERNEL);
	if (ts == NULL) {
		ret = -ENOMEM;
		goto err_alloc_data_failed;
	}


	pdata = client->dev.platform_data;
	if (pdata){
		ts->gpio_init = pdata->gpio_init;
		ts->power 	= pdata->power;
		ts->reset	= pdata->reset;
		ts->irq	= NULL;
		ts->max_y_position= pdata->max_y_position;
		memcpy(ts->fwfile,pdata->fwfile,sizeof(pdata->fwfile));
	}else{
		pr_err("%s: error!\n",__func__);
		goto err_power_failed; 
	}

	if ( ts->gpio_init ) {
		ret = ts->gpio_init(&client->dev,1);
		
		if ( ret < 0 ){
			pr_err("%s, gpio init failed! %d\n", __func__, ret);
			goto err_power_failed;
		}

		if (ts->reset) ts->reset(0);
		if (ts->power) ts->power(0);
		msleep(100);
		if (ts->power) ts->power(1);
		msleep(10);
		if (ts->reset) ts->reset(1);
		msleep(100);

	}


	if ( !detect_device( client )){
		pr_info("%s, device is not exsit.\n", __func__);
		goto err_detect_failed;
	}

	synaptics_wq = create_singlethread_workqueue("synaptics_wq");
	if (!synaptics_wq){
		pr_err("Could not create work queue synaptics_wq: no memory");
		ret = -ESRCH;
		goto err_create_singlethread;
	}
	INIT_WORK(&ts->work, synaptics_work_func);
	ts->client = client;
	i2c_set_clientdata(client, ts);


	ret = synaptics_get_pdt(ts);
	if (ret <= 0) {
		pr_err("%s: Error identifying device (%d)\n", __func__, ret);
		ret = -ENODEV;
		goto err_detect_failed;
	}

	synaptics_get_configid(ts,NULL,NULL, NULL);
	max_x = ts->max[0];
	max_y = ts->max[1];
	get_screeninfo(&xres, &yres);
	if ( ts->max_y_position != 0 )
	{
#if defined (CONFIG_MACH_COEUS) ||defined (CONFIG_MACH_WARPLTE) || defined (CONFIG_MACH_GAEA)||defined(CONFIG_MACH_NESTOR)//P893A21 LCD 540*960 (laibao TS 707*1267)  (yushun TS 1150*2185)
		if(ts->config_id.sensor=='7')//laibao
			max_y =  ts->max[1];
		else
#endif			
			max_y = ts->max_y_position - SYNAPTICS_BORDER_PIXEL;
	}
#if defined (CONFIG_MACH_WARPLTE)	|| defined (CONFIG_MACH_GAEA)||defined(CONFIG_MACH_NESTOR)
		if ((ts->config_id.sensor=='3')||(ts->config_id.sensor=='2'))//yushun&turly 2185
				max_y = 2017;
#endif
#ifdef TOUCHSCREEN_DUPLICATED_FILTER
	ts->dup_threshold=(max_y*10/yres+5)/10; 
	pr_info("dup_threshold %d\n", ts->dup_threshold);

#endif

	ret = synaptics_set_panel_state(ts, TS_POWER_ON); 
	if (ret < 0) {
		pr_err("%s: synaptics_set_panel_state failed\n",__func__);
		goto err_detect_failed;
	}


	ts->input_dev = input_allocate_device();
	if (ts->input_dev == NULL) {
		ret = -ENOMEM;
		pr_err("%s: Failed to allocate input device\n",__func__);
		goto err_input_dev_alloc_failed;
	}
	ts->input_dev->name = SYNAPTICS_I2C_RMI4_NAME;
	ts->input_dev->phys = client->name;


	set_bit(EV_SYN, ts->input_dev->evbit);
	set_bit(EV_KEY, ts->input_dev->evbit);
	set_bit(EV_ABS, ts->input_dev->evbit);
	//set_bit(BTN_9, ts->input_dev->keybit);
	set_bit(KEY_HOME, ts->input_dev->keybit);
	set_bit(KEY_MENU, ts->input_dev->keybit);
	set_bit(KEY_BACK, ts->input_dev->keybit);
	set_bit(KEY_SEARCH, ts->input_dev->keybit);
	//set_bit(ABS_X, ts->input_dev->absbit);
	//set_bit(ABS_Y, ts->input_dev->absbit);
	set_bit(ABS_MT_TRACKING_ID, ts->input_dev->absbit);
	set_bit(ABS_MT_TOUCH_MAJOR, ts->input_dev->absbit);
	set_bit(ABS_MT_POSITION_X, ts->input_dev->absbit);
	set_bit(ABS_MT_POSITION_Y, ts->input_dev->absbit);
	set_bit(ABS_MT_WIDTH_MAJOR, ts->input_dev->absbit);
	set_bit(ABS_MT_ORIENTATION, ts->input_dev->absbit);
	set_bit(ABS_MT_PRESSURE, ts->input_dev->absbit);
	/*ergate-008*/
	set_bit(BTN_TOUCH, ts->input_dev->keybit);

	//input_set_abs_params(ts->input_dev, ABS_X, 0, max_x, 0, 0);
	//input_set_abs_params(ts->input_dev, ABS_Y, 0, max_y, 0, 0);
	input_set_abs_params(ts->input_dev, ABS_MT_ORIENTATION, 0, 0, 0, 0);
	input_set_abs_params(ts->input_dev, ABS_MT_TOUCH_MAJOR, 0, 0xFF, 0, 0);
	input_set_abs_params(ts->input_dev, ABS_MT_PRESSURE, 0, 0xFF, 0, 0);
	//input_set_abs_params(ts->input_dev, ABS_MT_TOUCH_MINOR, 0, 0xF, 0, 0);
	input_set_abs_params(ts->input_dev, ABS_MT_POSITION_X, 0, max_x+1, 0, 0);
	input_set_abs_params(ts->input_dev, ABS_MT_POSITION_Y, 0, max_y+1, 0, 0);
	input_set_abs_params(ts->input_dev, ABS_MT_WIDTH_MAJOR, 0, 30, 0, 0);
	input_set_abs_params(ts->input_dev, ABS_MT_TRACKING_ID, 0, 10, 0, 0);	


	ret = input_register_device(ts->input_dev);
	if (ret)	{
		pr_err("%s: Unable to register %s input device\n", 
			__func__, ts->input_dev->name);
		goto err_input_register_device_failed;
	}
	
	ts->use_irq = 1;
	if (client->irq) 
	{
        if (request_irq(client->irq, synaptics_irq_handler, 
			IRQF_TRIGGER_LOW, client->name, ts)==0)
        {
			//pr_info("Received IRQ!\n");
			ts->use_irq = 1;
		}else{
			ts->use_irq = 0;
			dev_err(&client->dev, "request_irq failed\n");
		}
	}
	
	if (!ts->use_irq){
		hrtimer_init(&ts->timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
		ts->timer.function = synaptics_timer_func;
		hrtimer_start(&ts->timer, ktime_set(1, 0), HRTIMER_MODE_REL);
	}
	
#if defined (CONFIG_TOUCHSCREEN_UP_TIMER)
	hrtimer_init(&ts->up_timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
	ts->up_timer.function = synaptics_up_timer_func;
#endif

#ifdef CONFIG_HAS_EARLYSUSPEND
	ts->early_suspend.level = EARLY_SUSPEND_LEVEL_BLANK_SCREEN + 1;
	ts->early_suspend.suspend = synaptics_early_suspend;
	ts->early_suspend.resume = synaptics_late_resume;
	register_early_suspend(&ts->early_suspend);
#endif

  	dir = proc_mkdir("touchscreen", NULL);
	refresh = create_proc_entry("ts_information", 0664, dir);
	if (refresh) {
		refresh->data		= NULL;
		refresh->read_proc  = proc_read_val;
		refresh->write_proc = proc_write_val;
	}	
	fw_lastest_flag = create_proc_entry("fw_lastest_flag", 0444, dir);
	if (fw_lastest_flag) {
		fw_lastest_flag->data		= NULL;
		fw_lastest_flag->read_proc  = proc_read_fw_lastest_flag;
		fw_lastest_flag->write_proc = NULL;
	}	


	syn_ts=ts;
#if defined(CONFIG_TOUCHSCREEN_SYNAPTICS_FW)
	syna_fwupdate_init(client);
#endif

	bitmap_zero(pre_fingers, NR_FINGERS);
#if defined (CONFIG_TOUCHSCREEN_RESUME_LOG) ||defined (CONFIG_TOUCHSCREEN_UP_TIMER)
	bitmap_zero(pre_pre_fingers, NR_FINGERS);
#endif
	memset((void *)old_report_data, 0, sizeof(old_report_data));
	memset((void *)new_report_data, 0, sizeof(new_report_data));

	pr_info("%s: Start ts %s in %s mode\n",
		__func__,ts->input_dev->name, ts->use_irq ? "interrupt" : "polling");
	return 0;

err_input_register_device_failed:
	input_free_device(ts->input_dev);
err_input_dev_alloc_failed:
err_detect_failed:
	if (ts->gpio_init)
		ts->gpio_init(&client->dev,0);
err_power_failed:
err_create_singlethread:
	kfree(ts);
err_alloc_data_failed:
err_check_functionality_failed:
	pr_info("%s exit\n",__func__);
	return ret;

}

static int synaptics_remove(struct i2c_client *client)
{
	struct synaptics_rmi_data *ts = i2c_get_clientdata(client);

#if defined(CONFIG_TOUCHSCREEN_SYNAPTICS_FW)
	syna_fwupdate_deinit(client);
#endif

	unregister_early_suspend(&ts->early_suspend);
	if (ts->use_irq)
		free_irq(client->irq, ts);
	else
		hrtimer_cancel(&ts->timer);
	input_unregister_device(ts->input_dev);

	// power off
	if (ts->power) ts->power(0);
	if (ts->gpio_init) ts->gpio_init(&client->dev,0);

	kfree(ts);

	return 0;
}

static int synaptics_suspend(struct i2c_client *client, pm_message_t mesg)
{
	int ret;

	struct synaptics_rmi_data *ts = i2c_get_clientdata(client);

	if (ts->use_irq){
		disable_irq(client->irq);
#if defined (CONFIG_TOUCHSCREEN_UP_TIMER)
		hrtimer_cancel(&ts->up_timer);
		//printk("synaptics_ts: suspend CANCEL up_timer!!! \n");
#endif
	}
	else
		hrtimer_cancel(&ts->timer);

	ret = cancel_work_sync(&ts->work);
	/* if work was pending disable-count is now 2 */
	if (ret && ts->use_irq)
		enable_irq(client->irq);

	synaptics_set_panel_state(ts, TS_SUSPEND);

#ifdef CONFIG_TOUCHSCREEN_RESUME_LOG
	if(synaptics_resume_flag)
		synaptics_log3();
#endif

	return 0;
}

static int synaptics_resume(struct i2c_client *client)
{

	struct synaptics_rmi_data *ts = i2c_get_clientdata(client);

	synaptics_set_panel_state(ts, TS_RESUME);
	
	if (ts->use_irq){
		enable_irq(client->irq);
	}else{
		hrtimer_start(&ts->timer, ktime_set(1, 0), HRTIMER_MODE_REL);
	}
#ifdef CONFIG_TOUCHSCREEN_RESUME_LOG
	synaptics_resume_flag=true;
#endif
	
	return 0;
}

#ifdef CONFIG_HAS_EARLYSUSPEND
static void synaptics_early_suspend(struct early_suspend *h)
{
	struct synaptics_rmi_data *ts;
	ts = container_of(h, struct synaptics_rmi_data, early_suspend);
	synaptics_suspend(ts->client, PMSG_SUSPEND);
}

static void synaptics_late_resume(struct early_suspend *h)
{
	struct synaptics_rmi_data *ts;
	ts = container_of(h, struct synaptics_rmi_data, early_suspend);
	synaptics_resume(ts->client);

}
#endif

static const struct i2c_device_id synaptics_id[] = {
	{ SYNAPTICS_I2C_RMI4_NAME, 0 },
	{ }
};
static struct i2c_driver synaptics_driver = {
	.probe		= synaptics_probe,
	.remove		= synaptics_remove,
#ifndef CONFIG_HAS_EARLYSUSPEND
	.suspend	= synaptics_suspend,
	.resume		= synaptics_resume,
#endif
	.id_table	= synaptics_id,
	.driver = {
		.name	= SYNAPTICS_I2C_RMI4_NAME,
	},
};

static int __devinit synaptics_init(void)
{
	#if 1  //ZTE_XJB_20130216 for power_off charging
	extern int  offcharging_flag;
	if(offcharging_flag)
	{
		printk("%s boot is in offcharging_flag mode! return 0 derect\n", __func__);//ZTE
		return  0;
	}
	
	#endif
	return i2c_add_driver_async(&synaptics_driver);
}

static void __exit synaptics_exit(void)
{
	i2c_del_driver(&synaptics_driver);
	if (synaptics_wq)
		destroy_workqueue(synaptics_wq);

}

module_init(synaptics_init);
module_exit(synaptics_exit);

MODULE_DESCRIPTION("Synaptics RMI4 Driver");
MODULE_LICENSE("GPL");

