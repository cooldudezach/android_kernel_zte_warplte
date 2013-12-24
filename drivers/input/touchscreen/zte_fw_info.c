#include <linux/proc_fs.h>

int g_zte_vid;
int g_zte_fw_ver;

int zte_fw_info_show(char *page, int len)
{
	len += sprintf(page + len, "latest fw info : \n");	
#if defined (CONFIG_MACH_APOLLO) || defined(CONFIG_MACH_NEX) 
	len += sprintf(page + len, "\t\t\t\t S2202+ECW : 01\n");  //EELY
	len += sprintf(page + len, "\t\t\t\t S2202+junda : 05\n");
	len += sprintf(page + len, "\t\t\t\t FT5326+mudong : 12\n");
#elif defined(CONFIG_MACH_DEMETER)
	len += sprintf(page + len, "\t\t\t\t S2202+Truly : 03\n");
	len += sprintf(page + len, "\t\t\t\t S2202+success : 02\n");
#elif defined(CONFIG_MACH_WARPLTE)|| defined (CONFIG_MACH_GAEA)||defined(CONFIG_MACH_NESTOR)
	len += sprintf(page + len, "\t\t\t\t S2202+laibao : 06\n");
	len += sprintf(page + len, "\t\t\t\t S2202+winteck : 02\n");
	len += sprintf(page + len, "\t\t\t\t S2202+TPK : 02\n");
	len += sprintf(page + len, "\t\t\t\t S2202+success : 02\n");	
	len += sprintf(page + len, "\t\t\t\t S2202+Truly : 05\n");
	len += sprintf(page + len, "\t\t\t\t FT5326+Truly : 0F\n");
#elif defined(CONFIG_MACH_COEUS)||defined(CONFIG_MACH_OCEANUS)||defined (CONFIG_MACH_HERA)
	len += sprintf(page + len, "\t\t\t\t S2202+success : 0A\n");
	len += sprintf(page + len, "\t\t\t\t S2202+Truly : 05\n");
	len += sprintf(page + len, "\t\t\t\t S2202+TPK : 04\n");
	len += sprintf(page + len, "\t\t\t\t FT5306+laibao : 10\n");
#elif defined(CONFIG_MACH_BECKY)||defined(CONFIG_MACH_METIS)
	len += sprintf(page + len, "\t\t\t\t S2202+Truly : 08\n");
	len += sprintf(page + len, "\t\t\t\t FT5306+Goworld : 0a\n");
	len += sprintf(page + len, "\t\t\t\t FT5306+laibao : 14\n");	
#elif defined(CONFIG_MACH_IRIS)
	len += sprintf(page + len, "\t\t\t\t S3203+Truly : 07\n");
	len += sprintf(page + len, "\t\t\t\t S3203+laibao : 08\n");
	len += sprintf(page + len, "\t\t\t\t S3203+baoming : 04\n");
#endif
  return len;
}

/*
return 0, outdated fw
return 1, latest fw
return 2, unknown module
return 3, unknown project
*/
int zte_fw_latest(void)
{
	printk("%s: g_zte_fw_ver=0x%x\n",__func__, g_zte_fw_ver);	
	
	switch (g_zte_vid){	
#if defined (CONFIG_MACH_APOLLO) || defined(CONFIG_MACH_NEX) 
	case '9'://EELY(ECW) sy
		return ((g_zte_fw_ver>=0x3031)?1:0); break;
	case 'E'://junda sy
		return ((g_zte_fw_ver>=0x3035)?1:0); break;
	case 0x55:// mudong ft
		return ((g_zte_fw_ver>=0x12)?1:0); break;		
	default:
		return 2;	break;//unknown module
		
#elif defined(CONFIG_MACH_DEMETER)
	case '2'://Truly sy
		return ((g_zte_fw_ver>=0x3033)?1:0); break;
	case '3'://Success sy
		return ((g_zte_fw_ver>=0x3032)?1:0); break;
	default:
		return 2;	break;//unknown module
		
#elif defined(CONFIG_MACH_WARPLTE)|| defined (CONFIG_MACH_GAEA)||defined(CONFIG_MACH_NESTOR)
	case '1'://TPK sy
		return ((g_zte_fw_ver>=0x3032)?1:0); break;
	case '2'://Truly sy
		return ((g_zte_fw_ver>=0x3035)?1:0); break;
	case '3'://Success sy
		return ((g_zte_fw_ver>=0x3032)?1:0); break;
	case '6'://wintek sy
		return ((g_zte_fw_ver>=0x3032)?1:0); break;
	case '7'://laibao sy
		return ((g_zte_fw_ver>=0x3036)?1:0); break;
	case 0x5A:// TRULY  ft
		return ((g_zte_fw_ver>=0x0F)?1:0); break;		
	default:
		return 2;	break;//unknown module
		
#elif defined(CONFIG_MACH_COEUS)||defined(CONFIG_MACH_OCEANUS)||defined(CONFIG_MACH_HERA)		
	case '1'://TPK sy
		return ((g_zte_fw_ver>=0x3034)?1:0); break;
	case '2'://Truly sy
		return ((g_zte_fw_ver>=0x3035)?1:0); break;
	case '3'://Success sy
		return ((g_zte_fw_ver>=0x3041)?1:0); break;
	case 0x55://laibao  ft
		return ((g_zte_fw_ver>=0x10)?1:0); break;		
	default:
		return 2;	break;//unknown module
		
#elif defined(CONFIG_MACH_BECKY)||defined(CONFIG_MACH_METIS)
	case '2'://Truly sy
		return ((g_zte_fw_ver>=0x3038)?1:0); break;
	case 0x55://laibao  ft
		return ((g_zte_fw_ver>=0x14)?1:0); break;		
	case 57://Goworld  ft
		return ((g_zte_fw_ver>=0x0a)?1:0); break;		
	default:
		return 2;	break;//unknown module
	
#elif defined(CONFIG_MACH_IRIS)
	case '2'://Truly sy
		return ((g_zte_fw_ver>=0x3037)?1:0); break;
	case '7'://laibao sy
		return ((g_zte_fw_ver>=0x3038)?1:0); break;
	case 'B'://Baoming sy
		return ((g_zte_fw_ver>=0x3034)?1:0); break;
	default:
		return 2;	break;//unknown module

#else
	default:
		return 3;//unknown project
#endif
	}

}



