/* Copyright (c) 2010-2012, Code Aurora Forum. All rights reserved.
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
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/completion.h>
#include <linux/cpuidle.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/ktime.h>
#include <linux/pm.h>
#include <linux/pm_qos.h>
#include <linux/smp.h>
#include <linux/suspend.h>
#include <linux/tick.h>
#include <linux/seq_file.h>
#include <mach/msm_iomap.h>
#include <mach/socinfo.h>
#include <mach/system.h>
#include <mach/scm.h>
#include <mach/socinfo.h>
#include <mach/msm-krait-l2-accessors.h>
#include <asm/cacheflush.h>
#include <asm/hardware/gic.h>
#include <asm/pgtable.h>
#include <asm/pgalloc.h>
#include <asm/hardware/cache-l2x0.h>
#ifdef CONFIG_VFP
#include <asm/vfp.h>
#endif

//tcd
#include <trace/events/power.h>

#include "acpuclock.h"
#include "clock.h"
#include "avs.h"
#include <mach/cpuidle.h>
#include "idle.h"
#include "pm.h"
#include "scm-boot.h"
#include "spm.h"
#include "timer.h"
#include "pm-boot.h"
#include <mach/event_timer.h>

#include <mach/zte_memlog.h>		//zte jiangfeng

/******************************************************************************
 * Debug Definitions
 *****************************************************************************/

enum {
	MSM_PM_DEBUG_SUSPEND = BIT(0),
	MSM_PM_DEBUG_POWER_COLLAPSE = BIT(1),
	MSM_PM_DEBUG_SUSPEND_LIMITS = BIT(2),
	MSM_PM_DEBUG_CLOCK = BIT(3),
	MSM_PM_DEBUG_RESET_VECTOR = BIT(4),
	MSM_PM_DEBUG_IDLE_CLK = BIT(5),
	MSM_PM_DEBUG_IDLE = BIT(6),
	MSM_PM_DEBUG_IDLE_LIMITS = BIT(7),
	MSM_PM_DEBUG_HOTPLUG = BIT(8),
	MSM_PM_DEBUG_ZTE_LOGS = BIT(9),//zte's customize log,default not open
};

//static int msm_pm_debug_mask = 1;
static int msm_pm_debug_mask = MSM_PM_DEBUG_SUSPEND | MSM_PM_DEBUG_POWER_COLLAPSE | MSM_PM_DEBUG_SUSPEND_LIMITS;
module_param_named(
	debug_mask, msm_pm_debug_mask, int, S_IRUGO | S_IWUSR | S_IWGRP
);
static int msm_pm_retention_tz_call;


/******************************************************************************
 * Sleep Modes and Parameters
 *****************************************************************************/
enum {
	MSM_PM_MODE_ATTR_SUSPEND,
	MSM_PM_MODE_ATTR_IDLE,
	MSM_PM_MODE_ATTR_NR,
};

#define SCM_L2_RETENTION	(0x2)
#define SCM_CMD_TERMINATE_PC	(0x2)

static char *msm_pm_mode_attr_labels[MSM_PM_MODE_ATTR_NR] = {
	[MSM_PM_MODE_ATTR_SUSPEND] = "suspend_enabled",
	[MSM_PM_MODE_ATTR_IDLE] = "idle_enabled",
};

struct msm_pm_kobj_attribute {
	unsigned int cpu;
	struct kobj_attribute ka;
};

#define GET_CPU_OF_ATTR(attr) \
	(container_of(attr, struct msm_pm_kobj_attribute, ka)->cpu)

struct msm_pm_sysfs_sleep_mode {
	struct kobject *kobj;
	struct attribute_group attr_group;
	struct attribute *attrs[MSM_PM_MODE_ATTR_NR + 1];
	struct msm_pm_kobj_attribute kas[MSM_PM_MODE_ATTR_NR];
};

static char *msm_pm_sleep_mode_labels[MSM_PM_SLEEP_MODE_NR] = {
	[MSM_PM_SLEEP_MODE_POWER_COLLAPSE] = "power_collapse",
	[MSM_PM_SLEEP_MODE_WAIT_FOR_INTERRUPT] = "wfi",
	[MSM_PM_SLEEP_MODE_RETENTION] = "retention",
	[MSM_PM_SLEEP_MODE_POWER_COLLAPSE_STANDALONE] =
		"standalone_power_collapse",
};

static struct hrtimer pm_hrtimer;
static struct msm_pm_sleep_ops pm_sleep_ops;
/*
 * Write out the attribute.
 */
static ssize_t msm_pm_mode_attr_show(
	struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	int ret = -EINVAL;
	int i;

	for (i = 0; i < MSM_PM_SLEEP_MODE_NR; i++) {
		struct kernel_param kp;
		unsigned int cpu;
		struct msm_pm_platform_data *mode;

		if (msm_pm_sleep_mode_labels[i] == NULL)
			continue;

		if (strcmp(kobj->name, msm_pm_sleep_mode_labels[i]))
			continue;

		cpu = GET_CPU_OF_ATTR(attr);
		mode = &msm_pm_sleep_modes[MSM_PM_MODE(cpu, i)];

		if (!strcmp(attr->attr.name,
			msm_pm_mode_attr_labels[MSM_PM_MODE_ATTR_SUSPEND])) {
			u32 arg = mode->suspend_enabled;
			kp.arg = &arg;
			ret = param_get_ulong(buf, &kp);
		} else if (!strcmp(attr->attr.name,
			msm_pm_mode_attr_labels[MSM_PM_MODE_ATTR_IDLE])) {
			u32 arg = mode->idle_enabled;
			kp.arg = &arg;
			ret = param_get_ulong(buf, &kp);
		}

		break;
	}

	if (ret > 0) {
		strlcat(buf, "\n", PAGE_SIZE);
		ret++;
	}

	return ret;
}

/*
 * Read in the new attribute value.
 */
static ssize_t msm_pm_mode_attr_store(struct kobject *kobj,
	struct kobj_attribute *attr, const char *buf, size_t count)
{
	int ret = -EINVAL;
	int i;

	for (i = 0; i < MSM_PM_SLEEP_MODE_NR; i++) {
		struct kernel_param kp;
		unsigned int cpu;
		struct msm_pm_platform_data *mode;

		if (msm_pm_sleep_mode_labels[i] == NULL)
			continue;

		if (strcmp(kobj->name, msm_pm_sleep_mode_labels[i]))
			continue;

		cpu = GET_CPU_OF_ATTR(attr);
		mode = &msm_pm_sleep_modes[MSM_PM_MODE(cpu, i)];

		if (!strcmp(attr->attr.name,
			msm_pm_mode_attr_labels[MSM_PM_MODE_ATTR_SUSPEND])) {
			kp.arg = &mode->suspend_enabled;
			ret = param_set_byte(buf, &kp);
		} else if (!strcmp(attr->attr.name,
			msm_pm_mode_attr_labels[MSM_PM_MODE_ATTR_IDLE])) {
			kp.arg = &mode->idle_enabled;
			ret = param_set_byte(buf, &kp);
		}

		break;
	}

	return ret ? ret : count;
}

/*
 * Add sysfs entries for one cpu.
 */
static int __init msm_pm_mode_sysfs_add_cpu(
	unsigned int cpu, struct kobject *modes_kobj)
{
	char cpu_name[8];
	struct kobject *cpu_kobj;
	struct msm_pm_sysfs_sleep_mode *mode = NULL;
	int i, j, k;
	int ret;

	snprintf(cpu_name, sizeof(cpu_name), "cpu%u", cpu);
	cpu_kobj = kobject_create_and_add(cpu_name, modes_kobj);
	if (!cpu_kobj) {
		pr_err("%s: cannot create %s kobject\n", __func__, cpu_name);
		ret = -ENOMEM;
		goto mode_sysfs_add_cpu_exit;
	}

	for (i = 0; i < MSM_PM_SLEEP_MODE_NR; i++) {
		int idx = MSM_PM_MODE(cpu, i);

		if ((!msm_pm_sleep_modes[idx].suspend_supported)
			&& (!msm_pm_sleep_modes[idx].idle_supported))
			continue;

		if (!msm_pm_sleep_mode_labels[i] ||
				!msm_pm_sleep_mode_labels[i][0])
			continue;

		mode = kzalloc(sizeof(*mode), GFP_KERNEL);
		if (!mode) {
			pr_err("%s: cannot allocate memory for attributes\n",
				__func__);
			ret = -ENOMEM;
			goto mode_sysfs_add_cpu_exit;
		}

		mode->kobj = kobject_create_and_add(
				msm_pm_sleep_mode_labels[i], cpu_kobj);
		if (!mode->kobj) {
			pr_err("%s: cannot create kobject\n", __func__);
			ret = -ENOMEM;
			goto mode_sysfs_add_cpu_exit;
		}

		for (k = 0, j = 0; k < MSM_PM_MODE_ATTR_NR; k++) {
			if ((k == MSM_PM_MODE_ATTR_IDLE) &&
				!msm_pm_sleep_modes[idx].idle_supported)
				continue;
			if ((k == MSM_PM_MODE_ATTR_SUSPEND) &&
			     !msm_pm_sleep_modes[idx].suspend_supported)
				continue;
			sysfs_attr_init(&mode->kas[j].ka.attr);
			mode->kas[j].cpu = cpu;
			mode->kas[j].ka.attr.mode = 0644;
			mode->kas[j].ka.show = msm_pm_mode_attr_show;
			mode->kas[j].ka.store = msm_pm_mode_attr_store;
			mode->kas[j].ka.attr.name = msm_pm_mode_attr_labels[k];
			mode->attrs[j] = &mode->kas[j].ka.attr;
			j++;
		}
		mode->attrs[j] = NULL;

		mode->attr_group.attrs = mode->attrs;
		ret = sysfs_create_group(mode->kobj, &mode->attr_group);
		if (ret) {
			pr_err("%s: cannot create kobject attribute group\n",
				__func__);
			goto mode_sysfs_add_cpu_exit;
		}
	}

	ret = 0;

mode_sysfs_add_cpu_exit:
	if (ret) {
		if (mode && mode->kobj)
			kobject_del(mode->kobj);
		kfree(mode);
	}

	return ret;
}

/*
 * Add sysfs entries for the sleep modes.
 */
static int __init msm_pm_mode_sysfs_add(void)
{
	struct kobject *module_kobj;
	struct kobject *modes_kobj;
	unsigned int cpu;
	int ret;

	module_kobj = kset_find_obj(module_kset, KBUILD_MODNAME);
	if (!module_kobj) {
		pr_err("%s: cannot find kobject for module %s\n",
			__func__, KBUILD_MODNAME);
		ret = -ENOENT;
		goto mode_sysfs_add_exit;
	}

	modes_kobj = kobject_create_and_add("modes", module_kobj);
	if (!modes_kobj) {
		pr_err("%s: cannot create modes kobject\n", __func__);
		ret = -ENOMEM;
		goto mode_sysfs_add_exit;
	}

	for_each_possible_cpu(cpu) {
		ret = msm_pm_mode_sysfs_add_cpu(cpu, modes_kobj);
		if (ret)
			goto mode_sysfs_add_exit;
	}

	ret = 0;

mode_sysfs_add_exit:
	return ret;
}

/******************************************************************************
 * Configure Hardware before/after Low Power Mode
 *****************************************************************************/

/*
 * Configure hardware registers in preparation for Apps power down.
 */
static void msm_pm_config_hw_before_power_down(void)
{
	return;
}

/*
 * Clear hardware registers after Apps powers up.
 */
static void msm_pm_config_hw_after_power_up(void)
{
}

/*
 * Configure hardware registers in preparation for SWFI.
 */
static void msm_pm_config_hw_before_swfi(void)
{
	return;
}

/*
 * Configure/Restore hardware registers in preparation for Retention.
 */

static void msm_pm_config_hw_after_retention(void)
{
	int ret;
	ret = msm_spm_set_low_power_mode(MSM_SPM_MODE_CLOCK_GATING, false);
	WARN_ON(ret);
}

static void msm_pm_config_hw_before_retention(void)
{
	return;
}


/******************************************************************************
 * Suspend Max Sleep Time
 *****************************************************************************/

#ifdef CONFIG_MSM_SLEEP_TIME_OVERRIDE
static int msm_pm_sleep_time_override;
module_param_named(sleep_time_override,
	msm_pm_sleep_time_override, int, S_IRUGO | S_IWUSR | S_IWGRP);
#endif

#define SCLK_HZ (32768)
#define MSM_PM_SLEEP_TICK_LIMIT (0x6DDD000)

static uint32_t msm_pm_max_sleep_time;

/*
 * Convert time from nanoseconds to slow clock ticks, then cap it to the
 * specified limit
 */
static int64_t msm_pm_convert_and_cap_time(int64_t time_ns, int64_t limit)
{
	do_div(time_ns, NSEC_PER_SEC / SCLK_HZ);
	return (time_ns > limit) ? limit : time_ns;
}

/*
 * Set the sleep time for suspend.  0 means infinite sleep time.
 */
void msm_pm_set_max_sleep_time(int64_t max_sleep_time_ns)
{
	if (max_sleep_time_ns == 0) {
		msm_pm_max_sleep_time = 0;
	} else {
		msm_pm_max_sleep_time = (uint32_t)msm_pm_convert_and_cap_time(
			max_sleep_time_ns, MSM_PM_SLEEP_TICK_LIMIT);

		if (msm_pm_max_sleep_time == 0)
			msm_pm_max_sleep_time = 1;
	}

	if (msm_pm_debug_mask & MSM_PM_DEBUG_SUSPEND)
		pr_info("%s: Requested %lld ns Giving %u sclk ticks\n",
			__func__, max_sleep_time_ns, msm_pm_max_sleep_time);
}
EXPORT_SYMBOL(msm_pm_set_max_sleep_time);

struct reg_data {
	uint32_t reg;
	uint32_t val;
};

static struct reg_data reg_saved_state[] = {
	{ .reg = 0x4501, },
	{ .reg = 0x5501, },
	{ .reg = 0x6501, },
	{ .reg = 0x7501, },
	{ .reg = 0x0500, },
};

static unsigned int active_vdd;
static bool msm_pm_save_cp15;
static const unsigned int pc_vdd = 0x98;

static void msm_pm_save_cpu_reg(void)
{
	int i;

	/* Only on core0 */
	if (smp_processor_id())
		return;

	/**
	 * On some targets, L2 PC will turn off may reset the core
	 * configuration for the mux and the default may not make the core
	 * happy when it resumes.
	 * Save the active vdd, and set the core vdd to QSB max vdd, so that
	 * when the core resumes, it is capable of supporting the current QSB
	 * rate. Then restore the active vdd before switching the acpuclk rate.
	 */
	if (msm_pm_get_l2_flush_flag() == 1) {
		active_vdd = msm_spm_get_vdd(0);
		for (i = 0; i < ARRAY_SIZE(reg_saved_state); i++)
			reg_saved_state[i].val =
				get_l2_indirect_reg(reg_saved_state[i].reg);
		msm_spm_set_vdd(0, pc_vdd);
	}
}

static void msm_pm_restore_cpu_reg(void)
{
	int i;

	/* Only on core0 */
	if (smp_processor_id())
		return;

	if (msm_pm_get_l2_flush_flag() == 1) {
		for (i = 0; i < ARRAY_SIZE(reg_saved_state); i++)
			set_l2_indirect_reg(reg_saved_state[i].reg,
					reg_saved_state[i].val);
		msm_spm_set_vdd(0, active_vdd);
	}
}

static void *msm_pm_idle_rs_limits;
static bool msm_pm_use_qtimer;

static void msm_pm_swfi(void)
{
	msm_pm_config_hw_before_swfi();
	msm_arch_idle();
}


static void msm_pm_retention(void)
{
	int ret = 0;

	msm_pm_config_hw_before_retention();
	ret = msm_spm_set_low_power_mode(MSM_SPM_MODE_POWER_RETENTION, false);
	WARN_ON(ret);

	if (msm_pm_retention_tz_call)
		scm_call_atomic1(SCM_SVC_BOOT, SCM_CMD_TERMINATE_PC,
					SCM_L2_RETENTION);
	else
		msm_arch_idle();

	msm_pm_config_hw_after_retention();
}

#ifdef CONFIG_CACHE_L2X0
static inline bool msm_pm_l2x0_power_collapse(void)
{
	bool collapsed = 0;

	l2cc_suspend();
	collapsed = msm_pm_collapse();
	l2cc_resume();

	return collapsed;
}
#else
static inline bool msm_pm_l2x0_power_collapse(void)
{
	return msm_pm_collapse();
}
#endif

#ifndef CONFIG_ZTE_PLATFORM_RECORD_APP_AWAKE_SUSPEND_TIME 
#define CONFIG_ZTE_PLATFORM_RECORD_APP_AWAKE_SUSPEND_TIME 1
#endif

#ifdef CONFIG_ZTE_PLATFORM_RECORD_APP_AWAKE_SUSPEND_TIME

#define MSM_PM_DPRINTK(mask, level, message, ...) \
	do { \
		if ((mask) & msm_pm_debug_mask) \
			printk(level message, ## __VA_ARGS__); \
	} while (0)


long lateresume_2_earlysuspend_time_s = 0;		//LHX_PM_20110411_01 time to record how long it takes to earlysuspend after last resume. namely,to record how long the LCD keeps on.
void zte_update_lateresume_2_earlysuspend_time(bool resume_or_earlysuspend)	// LHX_PM_20110411_01 resume_or_earlysuspend? lateresume : earlysuspend
{
	if(resume_or_earlysuspend)//lateresume,need to record when the lcd is turned on
	{
		lateresume_2_earlysuspend_time_s = current_kernel_time().tv_sec;
	}
	else	//earlysuspend,need to record when the lcd is turned off
	{
		lateresume_2_earlysuspend_time_s = current_kernel_time().tv_sec - lateresume_2_earlysuspend_time_s;	//record how long the lcd keeps on
	}
}


/*BEGIN LHX_PM_20110324_01 add code to record how long the APP sleeps or keeps awake*/
extern unsigned pm_modem_sleep_time_get(void);
extern unsigned pm_modem_awake_time_get(int * current_sleep);
extern unsigned pm_modem_phys_link_time_get(void);
#define AMSS_NEVER_ENTER_SLEEP 0x4//lianghouxing 20121119 add to indicate modem is sleep or awake,0 sleep, 1 awake,4 means never enter sleep yet
#define AMSS_NOW_SLEEP 0x0
#define AMSS_NOW_AWAKE 0x1
#define THRESOLD_FOR_OFFLINE_AWAKE_TIME 100 //ms,modem awake time is less than this,conside modem is set as offline.
#define THRESOLD_FOR_OFFLINE_TIME 5000 //5s

struct timespec time_updated_when_sleep_awake;
void record_sleep_awake_time(bool record_sleep_awake)
{
	//record_sleep_awake?: true?record awake time, else record  sleep time
	struct timespec ts;
	unsigned time_updated_when_sleep_awake_s;
	unsigned time_updated_when_sleep_awake_ms;
	unsigned time_updated_when_sleep_awake_ms_temp;
	static unsigned amss_sleep_time_ms = 0;
	static unsigned amss_physlink_current_total_time_s = 0;/////liukejing 20130606 add
	static unsigned amss_physlink_last_total_time_s = 0;/////liukejing 20130606 add
	unsigned amss_sleep_time_ms_temp = 0;
	unsigned deta_sleep_ms = 0;
	unsigned deta_awake_ms = 0;
	unsigned deta_physlink_s = 0;/////liukejing 20130606 add
//	bool offlinemode = false;
	unsigned amss_awake_last =0;
	
	unsigned amss_current_sleep_or_awake=0;//indicate modem is sleep or awake,1 means never enter sleep yet,2 sleep, 3 awake
	static unsigned  amss_current_sleep_or_awake_previous=0;
	
	static unsigned amss_awake_time_ms = 0;
	unsigned amss_awake_time_ms_temp = 0;
	bool get_amss_awake_ok = false;// true:get amss_awake time,false get amss_sleep_time

	unsigned percentage_amss_not_sleep_while_app_suspend = 0;	//the percentage of modem awake while app suspend in %o;
	static bool sleep_success_flag = false;  //set true while msm_pm_collapse returned 1 by passing record_sleep_awake as true;

//	if (!((MSM_PM_DEBUG_SUSPEND|MSM_PM_DEBUG_POWER_COLLAPSE) & msm_pm_debug_mask) )
	//	return ;

	ts = current_kernel_time();
	
	time_updated_when_sleep_awake_ms_temp =(unsigned) ((ts.tv_sec - time_updated_when_sleep_awake.tv_sec) * MSEC_PER_SEC + ((ts.tv_nsec / NSEC_PER_MSEC) - (time_updated_when_sleep_awake.tv_nsec / NSEC_PER_MSEC)));
	time_updated_when_sleep_awake_s = (time_updated_when_sleep_awake_ms_temp/MSEC_PER_SEC);
	time_updated_when_sleep_awake_ms = (time_updated_when_sleep_awake_ms_temp - time_updated_when_sleep_awake_s * MSEC_PER_SEC);

	if(record_sleep_awake)//record app awake time
	{
		sleep_success_flag = true;

		amss_sleep_time_ms_temp = amss_sleep_time_ms;	//backup previous total sleep time
		amss_sleep_time_ms  = pm_modem_sleep_time_get();	//get new total sleep time
		deta_sleep_ms = amss_sleep_time_ms - amss_sleep_time_ms_temp;
		//printk("  during this %s: modem sleep pre: %d  new %d s,modem sleep this time: %d ms\n",record_sleep_awake?"awake":"sleep",(int)amss_sleep_time_ms_temp ,amss_sleep_time_ms,deta_sleep_ms);

		amss_awake_time_ms_temp = amss_awake_time_ms;	//backup previous total sleep time
		amss_awake_time_ms  = pm_modem_awake_time_get(&amss_current_sleep_or_awake);	//get new total sleep time
		deta_awake_ms = amss_awake_time_ms - amss_awake_time_ms_temp;
		
		amss_physlink_current_total_time_s=pm_modem_phys_link_time_get();///liukejing 20130606 add to get total phys link time
		deta_physlink_s=amss_physlink_current_total_time_s - amss_physlink_last_total_time_s;
		//pr_info("print phys link time------awake------:amss_physlink_current_total_time_s %10d s amss_physlink_last_total_time_s %10d s\n", 
						//amss_physlink_current_total_time_s,amss_physlink_last_total_time_s);
		amss_physlink_last_total_time_s = amss_physlink_current_total_time_s;
//		printk("  during this %s: modem awake pre: %d  new %d s,modem awake this time: %d ms \n",record_sleep_awake?"awake":"sleep",(int)amss_awake_time_ms_temp ,amss_awake_time_ms,deta_awake_ms);
	//	printk(" modem total sleep: %d  ms,modem total awake: %d ms \n",amss_sleep_time_ms,amss_awake_time_ms);
/*
amss_current_sleep_or_awake_previous  amss_current_sleep_or_awake 
X 4 ---modem not enter sleep yet
0 0 ---previous is sleep,curret is sleep, modem awake time is updated,get awake deta directly.
otherwise get modem sleep time.
if modem is set to offline,print offline in the log
*/
		if((AMSS_NOW_SLEEP ==amss_current_sleep_or_awake_previous)&&(AMSS_NOW_SLEEP ==amss_current_sleep_or_awake))//00,get modem awake time
		{
			if((deta_awake_ms < THRESOLD_FOR_OFFLINE_AWAKE_TIME ) && (THRESOLD_FOR_OFFLINE_TIME < time_updated_when_sleep_awake_ms_temp))//if sleep time is 0 and awake is 0,offline mode
			{
				pr_info(" offline mode \n");
			}
			get_amss_awake_ok = true;
			amss_awake_last = deta_awake_ms;
		}
		else if(AMSS_NEVER_ENTER_SLEEP == amss_current_sleep_or_awake)
		{
			pr_info(" modem not enter sleep yet \n");
		}
		
		if(!get_amss_awake_ok)
		{
			amss_awake_last = time_updated_when_sleep_awake_ms_temp - deta_sleep_ms;
		}
		//printk("during this  %s: amss_awake_last: %d ms,deta_awake_ms %d ms,deta_sleep_ms this time %d ms\n",record_sleep_awake?"awake":"sleep",amss_awake_last,deta_awake_ms,deta_sleep_ms);
		percentage_amss_not_sleep_while_app_suspend = (amss_awake_last * 1000/(time_updated_when_sleep_awake_ms_temp + 1));

		pr_info("APP keep: %6d.%03d s !!!!!!!!!!awake, lcd on %6d s,%3d %%,,modem awake(%s) %10d ms %4d %%o,modem_sleep %10d ----%d%d\n", 
			time_updated_when_sleep_awake_s,time_updated_when_sleep_awake_ms,(int)lateresume_2_earlysuspend_time_s,(int)(lateresume_2_earlysuspend_time_s*100/(time_updated_when_sleep_awake_s + 1))
			,get_amss_awake_ok?"get_directly ":"from sleep_time",amss_awake_last,percentage_amss_not_sleep_while_app_suspend,deta_sleep_ms,amss_current_sleep_or_awake_previous,amss_current_sleep_or_awake);//in case Division by zero, +1
		pr_info("print phys link time:------awake------modem_phys_link_total_time %4d min %4d s deta_physlink_s %4d min %4d s\n", 
				amss_physlink_current_total_time_s/60,amss_physlink_current_total_time_s%60,deta_physlink_s/60,deta_physlink_s%60);//liukejing 20130606 add to count phys link time

		time_updated_when_sleep_awake = ts; 
		lateresume_2_earlysuspend_time_s = 0;	//LHX_PM_20110411_01 clear how long the lcd keeps on

	}
	else	//record app sleep time
	{
		if(!sleep_success_flag) //only record sleep time while really resume after successfully suspend/sleep;
		{
			printk("%s: modem sleep: resume after fail to suspend\n",__func__);
			return;
		}
		sleep_success_flag = false;

		amss_sleep_time_ms_temp = amss_sleep_time_ms;	//backup previous total sleep time
//		amss_sleep_time_ms  = pm_modem_sleep_time_get() / MSEC_PER_SEC;	//get new total sleep time
		amss_sleep_time_ms  = pm_modem_sleep_time_get();	//get new total sleep time
		deta_sleep_ms = amss_sleep_time_ms - amss_sleep_time_ms_temp;
		//printk("  during this %s: modem sleep pre: %d  new %d s,modem sleep this time: %d ms\n",record_sleep_awake?"awake":"sleep",(int)amss_sleep_time_ms_temp ,amss_sleep_time_ms,deta_sleep_ms);
		amss_awake_time_ms_temp = amss_awake_time_ms;	//backup previous total sleep time
	//	amss_awake_time_ms  = pm_modem_awake_time_get(&amss_current_sleep_or_awake) / MSEC_PER_SEC;	//get new total sleep time
		amss_awake_time_ms  = pm_modem_awake_time_get(&amss_current_sleep_or_awake);	//get new total sleep time
		deta_awake_ms = amss_awake_time_ms - amss_awake_time_ms_temp;
		
		amss_physlink_current_total_time_s=pm_modem_phys_link_time_get();///liukejing 20130606 add to get total phys link time
		deta_physlink_s=amss_physlink_current_total_time_s - amss_physlink_last_total_time_s;
				//pr_info("print phys link time------sleep------:amss_physlink_current_total_time_s %10d s amss_physlink_last_total_time_s %10d s\n", 
						//amss_physlink_current_total_time_s,amss_physlink_last_total_time_s);
		amss_physlink_last_total_time_s = amss_physlink_current_total_time_s;
		//printk("  during this %s: modem awake pre: %d  new %d s,modem awake this time: %d ms \n",record_sleep_awake?"awake":"sleep",(int)amss_awake_time_ms_temp ,amss_awake_time_ms,deta_awake_ms);
		if((AMSS_NOW_SLEEP ==amss_current_sleep_or_awake_previous)&&(AMSS_NOW_SLEEP ==amss_current_sleep_or_awake))//00,get modem awake time
		{
			if((deta_awake_ms < THRESOLD_FOR_OFFLINE_AWAKE_TIME ) && (THRESOLD_FOR_OFFLINE_TIME < time_updated_when_sleep_awake_ms_temp))//if sleep time is 0 and awake is 0,offline mode
			{
				pr_info(" offline mode \n");
			}
			get_amss_awake_ok = true;
			amss_awake_last = deta_awake_ms;
		}
		else if(AMSS_NEVER_ENTER_SLEEP == amss_current_sleep_or_awake)
		{
			pr_info(" modem not enter sleep yet \n");
		}
		
		if(!get_amss_awake_ok)
		{
			amss_awake_last = time_updated_when_sleep_awake_ms_temp - deta_sleep_ms;
		}
		printk(" modem total sleep: %d  ms,modem total awake: %d ms \n",amss_sleep_time_ms,amss_awake_time_ms);
		//printk("during this  %s: amss_awake_last: %d ms,deta_awake_ms %d ms,deta_sleep_ms this time %d ms\n",record_sleep_awake?"awake":"sleep",amss_awake_last,deta_awake_ms,deta_sleep_ms);
		percentage_amss_not_sleep_while_app_suspend = (amss_awake_last * 1000/(time_updated_when_sleep_awake_ms_temp + 1));
		pr_info( " APP keep: %10d.%03d s !!!!!!!!!!%s! modem awake(%s) %10d ms %4d %%o,modem_sleep %10d ----%d%d\n",time_updated_when_sleep_awake_s,time_updated_when_sleep_awake_ms, record_sleep_awake?"awake":"sleep",get_amss_awake_ok?"get_directly ":" from sleep_time",amss_awake_last,percentage_amss_not_sleep_while_app_suspend,deta_sleep_ms,amss_current_sleep_or_awake_previous,amss_current_sleep_or_awake);
		pr_info("print phys link time:------sleep------modem_phys_link_total_time %4d min %4d s deta_physlink_s %4d min %4d s\n", 
				amss_physlink_current_total_time_s/60,amss_physlink_current_total_time_s%60,deta_physlink_s/60,deta_physlink_s%60);//liukejing 20130606 add to count phys link time
		time_updated_when_sleep_awake = ts; 
	}
	
	amss_current_sleep_or_awake_previous = amss_current_sleep_or_awake;
}
/*End LHX_PM_20110324_01 add code to record how long the APP sleeps or keeps awake*/
#endif

static bool __ref msm_pm_spm_power_collapse(
	unsigned int cpu, bool from_idle, bool notify_rpm)
{
	void *entry;
	bool collapsed = 0;
	int ret;
	unsigned int saved_gic_cpu_ctrl;

	//tcd
	trace_cpu_pws_start(cpu, from_idle);

	saved_gic_cpu_ctrl = readl_relaxed(MSM_QGIC_CPU_BASE + GIC_CPU_CTRL);
	mb();

	if (!from_idle && (MSM_PM_DEBUG_POWER_COLLAPSE & msm_pm_debug_mask))
		pr_info("CPU%u: %s: notify_rpm %d\n",
			cpu, __func__, (int) notify_rpm);

	ret = msm_spm_set_low_power_mode(
			MSM_SPM_MODE_POWER_COLLAPSE, notify_rpm);
	WARN_ON(ret);

	entry = (!cpu || from_idle) ?
		msm_pm_collapse_exit : msm_secondary_startup;
	msm_pm_boot_config_before_pc(cpu, virt_to_phys(entry));

	if (MSM_PM_DEBUG_RESET_VECTOR & msm_pm_debug_mask)
		pr_info("CPU%u: %s: program vector to %p\n",
			cpu, __func__, entry);

#ifdef CONFIG_VFP
	vfp_pm_suspend();
#endif

	if (!from_idle && smp_processor_id() == 0)
		printk(KERN_INFO "[PM] suspend end\n");
	collapsed = msm_pm_l2x0_power_collapse();
	if (!from_idle)
		printk(KERN_INFO "[PM] resume start\n");
	msm_pm_boot_config_after_pc(cpu);

	if (collapsed) {
#ifdef CONFIG_VFP
		vfp_pm_resume();
#endif
		cpu_init();
		writel(0xF0, MSM_QGIC_CPU_BASE + GIC_CPU_PRIMASK);
		writel_relaxed(saved_gic_cpu_ctrl,
				MSM_QGIC_CPU_BASE + GIC_CPU_CTRL);
		mb();
		local_fiq_enable();
	}

	if (!from_idle && (MSM_PM_DEBUG_POWER_COLLAPSE & msm_pm_debug_mask))
		pr_info("CPU%u: %s: msm_pm_collapse returned, collapsed %d\n",
			cpu, __func__, collapsed);

	ret = msm_spm_set_low_power_mode(MSM_SPM_MODE_CLOCK_GATING, false);
	WARN_ON(ret);
	//tcd
	trace_cpu_pws_exit(0);
	return collapsed;
}

static bool msm_pm_power_collapse_standalone(bool from_idle)
{
	unsigned int cpu = smp_processor_id();
	unsigned int avsdscr;
	unsigned int avscsr;
	bool collapsed;

	avsdscr = avs_get_avsdscr();
	avscsr = avs_get_avscsr();
	avs_set_avscsr(0); /* Disable AVS */

	collapsed = msm_pm_spm_power_collapse(cpu, from_idle, false);

	avs_set_avsdscr(avsdscr);
	avs_set_avscsr(avscsr);
	return collapsed;
}

static bool msm_pm_power_collapse(bool from_idle)
{
	unsigned int cpu = smp_processor_id();
	unsigned long saved_acpuclk_rate;
	unsigned int avsdscr;
	unsigned int avscsr;
	bool collapsed;

	if (!from_idle && (MSM_PM_DEBUG_POWER_COLLAPSE & msm_pm_debug_mask))
		pr_info("CPU%u: %s: idle %d\n",
			cpu, __func__, (int)from_idle);

	msm_pm_config_hw_before_power_down();
	if (!from_idle && (MSM_PM_DEBUG_POWER_COLLAPSE & msm_pm_debug_mask))
		pr_info("CPU%u: %s: pre power down\n", cpu, __func__);

	avsdscr = avs_get_avsdscr();
	avscsr = avs_get_avscsr();
	avs_set_avscsr(0); /* Disable AVS */

	if (cpu_online(cpu))
		saved_acpuclk_rate = acpuclk_power_collapse();
	else
		saved_acpuclk_rate = 0;

	if (MSM_PM_DEBUG_CLOCK & msm_pm_debug_mask)
		pr_info("CPU%u: %s: change clock rate (old rate = %lu)\n",
			cpu, __func__, saved_acpuclk_rate);

	if (msm_pm_save_cp15)
		msm_pm_save_cpu_reg();

	collapsed = msm_pm_spm_power_collapse(cpu, from_idle, true);


	if (msm_pm_save_cp15)
		msm_pm_restore_cpu_reg();

	if (cpu_online(cpu)) {
		if (MSM_PM_DEBUG_CLOCK & msm_pm_debug_mask)
			pr_info("CPU%u: %s: restore clock rate to %lu\n",
				cpu, __func__, saved_acpuclk_rate);
		if (acpuclk_set_rate(cpu, saved_acpuclk_rate, SETRATE_PC) < 0)
			pr_err("CPU%u: %s: failed to restore clock rate(%lu)\n",
				cpu, __func__, saved_acpuclk_rate);
	} else {
		unsigned int gic_dist_enabled;
		unsigned int gic_dist_pending;
		gic_dist_enabled = readl_relaxed(
				MSM_QGIC_DIST_BASE + GIC_DIST_ENABLE_CLEAR);
		gic_dist_pending = readl_relaxed(
				MSM_QGIC_DIST_BASE + GIC_DIST_PENDING_SET);
		mb();
		gic_dist_pending &= gic_dist_enabled;

		if (gic_dist_pending)
			pr_err("CPU %d interrupted during hotplug.Pending int 0x%x\n",
					cpu, gic_dist_pending);
	}


	avs_set_avsdscr(avsdscr);
	avs_set_avscsr(avscsr);
	msm_pm_config_hw_after_power_up();
	if (!from_idle && (MSM_PM_DEBUG_POWER_COLLAPSE & msm_pm_debug_mask))
		pr_info("CPU%u: %s: post power up\n", cpu, __func__);

	if (!from_idle && (MSM_PM_DEBUG_POWER_COLLAPSE & msm_pm_debug_mask))
		pr_info("CPU%u: %s: return\n", cpu, __func__);
	return collapsed;
}

static void msm_pm_target_init(void)
{
	if (cpu_is_apq8064())
		msm_pm_save_cp15 = true;

	if (machine_is_msm8974())
		msm_pm_use_qtimer = true;
}

static int64_t msm_pm_timer_enter_idle(void)
{
	if (msm_pm_use_qtimer)
		return ktime_to_ns(tick_nohz_get_sleep_length());

	return msm_timer_enter_idle();
}

static void msm_pm_timer_exit_idle(bool timer_halted)
{
	if (msm_pm_use_qtimer)
		return;

	msm_timer_exit_idle((int) timer_halted);
}

static int64_t msm_pm_timer_enter_suspend(int64_t *period)
{
	int64_t time = 0;

	if (msm_pm_use_qtimer)
		return sched_clock();

	time = msm_timer_get_sclk_time(period);
	if (!time)
		pr_err("%s: Unable to read sclk.\n", __func__);

	return time;
}

static int64_t msm_pm_timer_exit_suspend(int64_t time, int64_t period)
{
	if (msm_pm_use_qtimer)
		return sched_clock() - time;

	if (time != 0) {
		int64_t end_time = msm_timer_get_sclk_time(NULL);
		if (end_time != 0) {
			time = end_time - time;
			if (time < 0)
				time += period;
		} else
			time = 0;
	}

	return time;
}

/**
 * pm_hrtimer_cb() : Callback function for hrtimer created if the
 *                   core needs to be awake to handle an event.
 * @hrtimer : Pointer to hrtimer
 */
static enum hrtimer_restart pm_hrtimer_cb(struct hrtimer *hrtimer)
{
	return HRTIMER_NORESTART;
}

/**
 * msm_pm_set_timer() : Set an hrtimer to wakeup the core in time
 *                      to handle an event.
 */
static void msm_pm_set_timer(uint32_t modified_time_us)
{
	u64 modified_time_ns = modified_time_us * NSEC_PER_USEC;
	ktime_t modified_ktime = ns_to_ktime(modified_time_ns);
	pm_hrtimer.function = pm_hrtimer_cb;
	hrtimer_start(&pm_hrtimer, modified_ktime, HRTIMER_MODE_ABS);
}

/******************************************************************************
 * External Idle/Suspend Functions
 *****************************************************************************/

void arch_idle(void)
{
	return;
}

int msm_pm_idle_prepare(struct cpuidle_device *dev,
		struct cpuidle_driver *drv, int index)
{
	int i;
	unsigned int power_usage = -1;
	int ret = 0;
	uint32_t modified_time_us = 0;
	struct msm_pm_time_params time_param;

	time_param.latency_us =
		(uint32_t) pm_qos_request(PM_QOS_CPU_DMA_LATENCY);
	time_param.sleep_us =
		(uint32_t) (ktime_to_us(tick_nohz_get_sleep_length())
								& UINT_MAX);
	time_param.modified_time_us = 0;

	if (!dev->cpu)
		time_param.next_event_us =
			(uint32_t) (ktime_to_us(get_next_event_time())
								& UINT_MAX);
	else
		time_param.next_event_us = 0;

	for (i = 0; i < dev->state_count; i++) {
		struct cpuidle_state *state = &drv->states[i];
		struct cpuidle_state_usage *st_usage = &dev->states_usage[i];
		enum msm_pm_sleep_mode mode;
		bool allow;
		void *rs_limits = NULL;
		uint32_t power;
		int idx;

		mode = (enum msm_pm_sleep_mode) cpuidle_get_statedata(st_usage);
		idx = MSM_PM_MODE(dev->cpu, mode);

		allow = msm_pm_sleep_modes[idx].idle_enabled &&
				msm_pm_sleep_modes[idx].idle_supported;

		switch (mode) {
		case MSM_PM_SLEEP_MODE_POWER_COLLAPSE:
		case MSM_PM_SLEEP_MODE_RETENTION:
			if (!allow)
				break;

			if (num_online_cpus() > 1) {
				allow = false;
				break;
			}
			/* fall through */

		case MSM_PM_SLEEP_MODE_POWER_COLLAPSE_STANDALONE:
			if (!allow)
				break;
			/* fall through */

		case MSM_PM_SLEEP_MODE_WAIT_FOR_INTERRUPT:
			if (!allow)
				break;
			/* fall through */

			if (pm_sleep_ops.lowest_limits)
				rs_limits = pm_sleep_ops.lowest_limits(true,
						mode, &time_param, &power);

			if (MSM_PM_DEBUG_IDLE & msm_pm_debug_mask)
				pr_info("CPU%u: %s: %s, latency %uus, "
					"sleep %uus, limit %p\n",
					dev->cpu, __func__, state->desc,
					time_param.latency_us,
					time_param.sleep_us, rs_limits);

			if (!rs_limits)
				allow = false;
			break;

		default:
			allow = false;
			break;
		}

		if (MSM_PM_DEBUG_IDLE & msm_pm_debug_mask)
			pr_info("CPU%u: %s: allow %s: %d\n",
				dev->cpu, __func__, state->desc, (int)allow);

		if (allow) {
			if (power < power_usage) {
				power_usage = power;
				modified_time_us = time_param.modified_time_us;
				ret = mode;
			}

			if (MSM_PM_SLEEP_MODE_POWER_COLLAPSE == mode)
				msm_pm_idle_rs_limits = rs_limits;
		}
	}

	if (modified_time_us && !dev->cpu)
		msm_pm_set_timer(modified_time_us);
	return ret;
}

int msm_pm_idle_enter(enum msm_pm_sleep_mode sleep_mode)
{
	int64_t time;
	int exit_stat;

	if (MSM_PM_DEBUG_IDLE & msm_pm_debug_mask)
		pr_info("CPU%u: %s: mode %d\n",
			smp_processor_id(), __func__, sleep_mode);

	time = ktime_to_ns(ktime_get());

	switch (sleep_mode) {
	case MSM_PM_SLEEP_MODE_WAIT_FOR_INTERRUPT:
		msm_pm_swfi();
		exit_stat = MSM_PM_STAT_IDLE_WFI;
		break;

	case MSM_PM_SLEEP_MODE_RETENTION:
		msm_pm_retention();
		exit_stat = MSM_PM_STAT_RETENTION;
		break;

	case MSM_PM_SLEEP_MODE_POWER_COLLAPSE_STANDALONE:
		msm_pm_power_collapse_standalone(true);
		exit_stat = MSM_PM_STAT_IDLE_STANDALONE_POWER_COLLAPSE;
		break;

	case MSM_PM_SLEEP_MODE_POWER_COLLAPSE: {
		int64_t timer_expiration = 0;
		bool timer_halted = false;
		uint32_t sleep_delay;
		int ret = -ENODEV;
		int notify_rpm =
			(sleep_mode == MSM_PM_SLEEP_MODE_POWER_COLLAPSE);
		int collapsed;

		timer_expiration = msm_pm_timer_enter_idle();

		sleep_delay = (uint32_t) msm_pm_convert_and_cap_time(
			timer_expiration, MSM_PM_SLEEP_TICK_LIMIT);
		if (sleep_delay == 0) /* 0 would mean infinite time */
			sleep_delay = 1;

		if (MSM_PM_DEBUG_IDLE_CLK & msm_pm_debug_mask)
			clock_debug_print_enabled();

		if (pm_sleep_ops.enter_sleep)
			ret = pm_sleep_ops.enter_sleep(sleep_delay,
					msm_pm_idle_rs_limits,
					true, notify_rpm);
		if (!ret) {
			collapsed = msm_pm_power_collapse(true);
			timer_halted = true;

			if (pm_sleep_ops.exit_sleep)
				pm_sleep_ops.exit_sleep(msm_pm_idle_rs_limits,
						true, notify_rpm, collapsed);
		}
		msm_pm_timer_exit_idle(timer_halted);
		exit_stat = MSM_PM_STAT_IDLE_POWER_COLLAPSE;
		break;
	}

	default:
		__WARN();
		goto cpuidle_enter_bail;
	}

	time = ktime_to_ns(ktime_get()) - time;
	msm_pm_add_stat(exit_stat, time);

	do_div(time, 1000);
	return (int) time;

cpuidle_enter_bail:
	return 0;
}


#ifndef ZTE_GPIO_DEBUG
#define ZTE_GPIO_DEBUG
#endif

#ifdef ZTE_GPIO_DEBUG
extern int msm_dump_gpios(struct seq_file *m, int curr_len, char *gpio_buffer);
extern int pm8xxx_dump_gpios(struct seq_file *m, int curr_len, char *gpio_buffer);
static char *gpio_sleep_status_info;

int print_gpio_buffer(struct seq_file *m)
{
	if (gpio_sleep_status_info)
		seq_printf(m, gpio_sleep_status_info);
	else
		seq_printf(m, "Device haven't suspended yet!\n");

	return 0;
}
EXPORT_SYMBOL(print_gpio_buffer);

int free_gpio_buffer(void)
{
	kfree(gpio_sleep_status_info);
	gpio_sleep_status_info = NULL;

	return 0;
}
EXPORT_SYMBOL(free_gpio_buffer);
#endif

void msm_pm_cpu_enter_lowpower(unsigned int cpu)
{
	int i;
	bool allow[MSM_PM_SLEEP_MODE_NR];

	for (i = 0; i < MSM_PM_SLEEP_MODE_NR; i++) {
		struct msm_pm_platform_data *mode;

		mode = &msm_pm_sleep_modes[MSM_PM_MODE(cpu, i)];
		allow[i] = mode->suspend_supported && mode->suspend_enabled;
	}

	if (MSM_PM_DEBUG_HOTPLUG & msm_pm_debug_mask)
		pr_notice("CPU%u: %s: shutting down cpu\n", cpu, __func__);

	if (allow[MSM_PM_SLEEP_MODE_POWER_COLLAPSE])
		msm_pm_power_collapse(false);
	else if (allow[MSM_PM_SLEEP_MODE_POWER_COLLAPSE_STANDALONE])
		msm_pm_power_collapse_standalone(false);
	else if (allow[MSM_PM_SLEEP_MODE_RETENTION])
		msm_pm_retention();
	else
		msm_pm_swfi();
}

static zte_smem_global*    zte_global  =   NULL;   //zte jiangfeng
static int msm_pm_enter(suspend_state_t state)
{
	bool allow[MSM_PM_SLEEP_MODE_NR];
	int i;
#ifdef ZTE_GPIO_DEBUG
	int curr_len = 0;
#endif

	int64_t period = 0;
	int64_t time = msm_pm_timer_enter_suspend(&period);
	struct msm_pm_time_params time_param;

	time_param.latency_us = -1;
	time_param.sleep_us = -1;
	time_param.next_event_us = 0;

	if (MSM_PM_DEBUG_SUSPEND & msm_pm_debug_mask)
		pr_info("%s\n", __func__);
#ifdef ZTE_GPIO_DEBUG
	/*
	 * Default not open the gpio dump,too much logs...
	*/
	if (MSM_PM_DEBUG_ZTE_LOGS & msm_pm_debug_mask) {
		if (gpio_sleep_status_info) {
			memset(gpio_sleep_status_info, 0,
				sizeof(gpio_sleep_status_info));
		} else {
			gpio_sleep_status_info = kmalloc(25000, GFP_KERNEL);
			if (!gpio_sleep_status_info) {
				pr_err("[PM] kmalloc memory failed in %s\n",
					__func__);
				goto skip_dump;
			}
		}

		curr_len = msm_dump_gpios(NULL, curr_len,	gpio_sleep_status_info);
		curr_len = pm8xxx_dump_gpios(NULL, curr_len,
						gpio_sleep_status_info);
#endif
	}

skip_dump:
	if (smp_processor_id()) {
		__WARN();
		goto enter_exit;
	}


	for (i = 0; i < MSM_PM_SLEEP_MODE_NR; i++) {
		struct msm_pm_platform_data *mode;

		mode = &msm_pm_sleep_modes[MSM_PM_MODE(0, i)];
		allow[i] = mode->suspend_supported && mode->suspend_enabled;
	}

	if (allow[MSM_PM_SLEEP_MODE_POWER_COLLAPSE]) {
		void *rs_limits = NULL;
		int ret = -ENODEV;
		uint32_t power;

		if (MSM_PM_DEBUG_SUSPEND & msm_pm_debug_mask)
			pr_info("%s: power collapse\n", __func__);

		clock_debug_print_enabled();

#ifdef CONFIG_MSM_SLEEP_TIME_OVERRIDE
		if (msm_pm_sleep_time_override > 0) {
			int64_t ns = NSEC_PER_SEC *
				(int64_t) msm_pm_sleep_time_override;
			msm_pm_set_max_sleep_time(ns);
			msm_pm_sleep_time_override = 0;
		}
#endif /* CONFIG_MSM_SLEEP_TIME_OVERRIDE */
		if (pm_sleep_ops.lowest_limits)
			rs_limits = pm_sleep_ops.lowest_limits(false,
			MSM_PM_SLEEP_MODE_POWER_COLLAPSE, &time_param, &power);

		//zte jiangfeng
		if(zte_global	==	NULL)
			zte_global= ioremap(ZTE_SMEM_LOG_GLOBAL_BASE, 
				sizeof(zte_smem_global));
		if(zte_global)
			zte_global->app_suspend_state	=	0xAA;
		//zte jiangfeng, end

		if (rs_limits) {
			if (pm_sleep_ops.enter_sleep)
				ret = pm_sleep_ops.enter_sleep(
						msm_pm_max_sleep_time,
						rs_limits, false, true);
#ifdef CONFIG_ZTE_PLATFORM_RECORD_APP_AWAKE_SUSPEND_TIME
				record_sleep_awake_time(true);//LHX_PM_20110324_01 add code to record how long the APP sleeps or keeps awake 
#endif
			if (!ret) {
				int collapsed = msm_pm_power_collapse(false);
				if (pm_sleep_ops.exit_sleep) {
					pm_sleep_ops.exit_sleep(rs_limits,
						false, true, collapsed);
				}
			}
		} else {
			pr_err("%s: cannot find the lowest power limit\n",
				__func__);
		}
		
		if(zte_global)
			zte_global->app_suspend_state	=	0;		//zte jiangfeng

		time = msm_pm_timer_exit_suspend(time, period);
		msm_pm_add_stat(MSM_PM_STAT_SUSPEND, time);
	} else if (allow[MSM_PM_SLEEP_MODE_POWER_COLLAPSE_STANDALONE]) {
		if (MSM_PM_DEBUG_SUSPEND & msm_pm_debug_mask)
			pr_info("%s: standalone power collapse\n", __func__);
		msm_pm_power_collapse_standalone(false);
	} else if (allow[MSM_PM_SLEEP_MODE_RETENTION]) {
		if (MSM_PM_DEBUG_SUSPEND & msm_pm_debug_mask)
			pr_info("%s: retention\n", __func__);
		msm_pm_retention();
	} else if (allow[MSM_PM_SLEEP_MODE_WAIT_FOR_INTERRUPT]) {
		if (MSM_PM_DEBUG_SUSPEND & msm_pm_debug_mask)
			pr_info("%s: swfi\n", __func__);
		msm_pm_swfi();
	}


enter_exit:
	if (MSM_PM_DEBUG_SUSPEND & msm_pm_debug_mask)
		pr_info("%s: return\n", __func__);

	return 0;
}

static struct platform_suspend_ops msm_pm_ops = {
	.enter = msm_pm_enter,
	.valid = suspend_valid_only_mem,
};

/******************************************************************************
 * Initialization routine
 *****************************************************************************/
void msm_pm_set_sleep_ops(struct msm_pm_sleep_ops *ops)
{
	if (ops)
		pm_sleep_ops = *ops;
}

void __init msm_pm_set_tz_retention_flag(unsigned int flag)
{
	msm_pm_retention_tz_call = flag;
}

static int __init msm_pm_init(void)
{
	pgd_t *pc_pgd;
	pmd_t *pmd;
	unsigned long pmdval;
	enum msm_pm_time_stats_id enable_stats[] = {
		MSM_PM_STAT_IDLE_WFI,
		MSM_PM_STAT_RETENTION,
		MSM_PM_STAT_IDLE_STANDALONE_POWER_COLLAPSE,
		MSM_PM_STAT_IDLE_POWER_COLLAPSE,
		MSM_PM_STAT_SUSPEND,
	};
	unsigned long exit_phys;

	/* Page table for cores to come back up safely. */
	pc_pgd = pgd_alloc(&init_mm);
	if (!pc_pgd)
		return -ENOMEM;

	exit_phys = virt_to_phys(msm_pm_collapse_exit);

	pmd = pmd_offset(pud_offset(pc_pgd + pgd_index(exit_phys),exit_phys),
					exit_phys);
	pmdval = (exit_phys & PGDIR_MASK) |
		     PMD_TYPE_SECT | PMD_SECT_AP_WRITE;
	pmd[0] = __pmd(pmdval);
	pmd[1] = __pmd(pmdval + (1 << (PGDIR_SHIFT - 1)));

	msm_saved_state_phys =
		allocate_contiguous_ebi_nomap(CPU_SAVED_STATE_SIZE *
					      num_possible_cpus(), 4);
	if (!msm_saved_state_phys)
		return -ENOMEM;
	msm_saved_state = ioremap_nocache(msm_saved_state_phys,
					  CPU_SAVED_STATE_SIZE *
					  num_possible_cpus());
	if (!msm_saved_state)
		return -ENOMEM;

	/* It is remotely possible that the code in msm_pm_collapse_exit()
	 * which turns on the MMU with this mapping is in the
	 * next even-numbered megabyte beyond the
	 * start of msm_pm_collapse_exit().
	 * Map this megabyte in as well.
	 */
	pmd[2] = __pmd(pmdval + (2 << (PGDIR_SHIFT - 1)));
	flush_pmd_entry(pmd);
	msm_pm_pc_pgd = virt_to_phys(pc_pgd);
	clean_caches((unsigned long)&msm_pm_pc_pgd, sizeof(msm_pm_pc_pgd),
		     virt_to_phys(&msm_pm_pc_pgd));

	msm_pm_mode_sysfs_add();
	msm_pm_add_stats(enable_stats, ARRAY_SIZE(enable_stats));

	suspend_set_ops(&msm_pm_ops);
	msm_pm_target_init();
	hrtimer_init(&pm_hrtimer, CLOCK_MONOTONIC, HRTIMER_MODE_ABS);
	msm_cpuidle_init();
	
	zte_global	=	ioremap(ZTE_SMEM_LOG_GLOBAL_BASE,
			sizeof(zte_smem_global));	//zte jiangfeng
	if(zte_global)
		zte_global->app_suspend_state	=	0;		//zte jiangfeng
	printk("pm initted");

	return 0;
}

late_initcall(msm_pm_init);
