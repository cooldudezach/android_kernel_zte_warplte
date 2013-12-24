/* ========================================================================
Copyright (c) 2001-2009 by ZTE Corporation.  All Rights Reserved.        

-------------------------------------------------------------------------------------------------
   Modify History
-------------------------------------------------------------------------------------------------
When           Who        What 
=========================================================================== */
#ifndef ZTE_MEMLOG_H
#define ZTE_MEMLOG_H

#define ZTE_SMEM_RAM_PHYS	0x88C00000
#define ZTE_SMEM_LOG_INFO_BASE    ZTE_SMEM_RAM_PHYS
#define ZTE_SMEM_LOG_GLOBAL_BASE  (ZTE_SMEM_RAM_PHYS + PAGE_SIZE)

#define ZTE_MODEM_SLEEP_LOG_LEN	0x80000
#define ZTE_MODEM_SLEEP_LOG_BASE	(ZTE_SMEM_RAM_PHYS + 0xc0000 -ZTE_MODEM_SLEEP_LOG_LEN)

typedef struct {
	unsigned char	app_suspend_state;		//0xaa:suspend, other:run
	u32	boot_flag;				//0xaa: restart, other:first boot
} zte_smem_global;

#endif
