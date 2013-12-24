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

#define SENSOR_NAME "ov8835"
DEFINE_MSM_MUTEX(ov8835_mut);

static struct msm_sensor_ctrl_t ov8835_s_ctrl;
extern void msm_sensorinfo_set_back_sensor_id(uint16_t id);
static struct v4l2_subdev_info ov8835_subdev_info[] = {
	{
		.code   = V4L2_MBUS_FMT_SBGGR10_1X10,
		.colorspace = V4L2_COLORSPACE_JPEG,
		.fmt    = 1,
		.order    = 0,
	},
	/* more can be supported, to be added later */
};

static struct msm_sensor_id_info_t ov8835_id_info = {
	.sensor_id_reg_addr = 0x300A,
	.sensor_id = 0x8830,
};

static struct msm_camera_power_seq_t ov8835_power_seq[] = {
	{ENABLE_VREG, 0},
	{REQUEST_GPIO, 0},
	{ENABLE_GPIO, 0},
	{CONFIG_CLK, 1}, 
};


static struct msm_cam_clk_info cam_clk_info[] = {
	{"cam_clk", MSM_SENSOR_MCLK_24HZ},
};

struct otp_struct current_ov8835_otp;

uint16_t read_sccb16(uint16_t address, struct msm_sensor_ctrl_t *s_ctrl) 
{ 
uint16_t temp, flag; 

temp= msm_camera_i2c_read( 
                        s_ctrl->sensor_i2c_client, 
                        address, &flag, 
                        MSM_CAMERA_I2C_BYTE_DATA); 
return flag; 
} 

uint16_t write_sccb16(uint16_t address, uint16_t value, struct msm_sensor_ctrl_t *s_ctrl) 
{ 
int rc; 
 
rc= msm_camera_i2c_write( 
                        s_ctrl->sensor_i2c_client, 
                        address, value, 
                        MSM_CAMERA_I2C_BYTE_DATA); 
return rc; 
}         
 
void clear_otp_buffer(struct msm_sensor_ctrl_t *s_ctrl) 
{ 
        int i; 
        // clear otp buffer 
        for (i=0;i<16;i++) { 
        write_sccb16(0x3d00 + i, 0x00,s_ctrl); 
        } 
} 
 
// index: index of otp group. ( 1, 2,3) 
// return: 0, group index is empty 
// 1, group index has invalid data 
// 2, group index has valid data 
int check_otp_wb(uint index, struct msm_sensor_ctrl_t *s_ctrl) 
{ 
        uint  flag; 
        int bank, address; 
        // select bank index 
        bank = 0xc0 | index; 
        write_sccb16(0x3d84, bank,s_ctrl); 
        // read otp into buffer 
        write_sccb16(0x3d81, 0x01,s_ctrl); 
        msleep(5); 
        // read flag 
        address = 0x3d00; 
        flag = read_sccb16(address,s_ctrl); 
	pr_err("otpwb %s  flag=0x%x",__func__,flag); 

        flag = flag & 0xc0; 
        // disable otp read 
        write_sccb16(0x3d81, 0x00,s_ctrl); 
        // clear otp buffer 
        clear_otp_buffer(s_ctrl); 
        if (flag==0) { 
        return 0; 
        } 
        else if (flag & 0x80) { 
        return 1; 
        } 
        else { 
        return 2; 
        } 
 
} 
 
// index: index of otp group. (1, 2, 3) 
// return: 0, group index is empty 
// 1, group index has invalid data 
// 2, group index has valid data 
int check_otp_lenc(uint index, struct msm_sensor_ctrl_t *s_ctrl) 
{ 
        int flag,  bank; 
        int address; 
        // select bank: index*4 =4,8,12
        bank = 0xc0 | (index*4); 
        write_sccb16(0x3d84, bank,s_ctrl); 
        // read otp into buffer 
        write_sccb16(0x3d81, 0x01,s_ctrl); 
        msleep(5); 
        // read flag 
        address = 0x3d00; 
        flag = read_sccb16(address,s_ctrl); 
	pr_err("otplenc %s  flag=0x%x",__func__,flag); 
        flag = flag & 0xc0; 
        // disable otp read 
        write_sccb16(0x3d81, 0x00,s_ctrl); 
        // clear otp buffer 
        clear_otp_buffer(s_ctrl); 
        if (flag==0) { 
        return 0; 
        } 
        else if (flag & 0x80) { 
        return 1; 
        } 
        else { 
        return 2; 
        } 
 
} 
 
 
// index: index of otp group. ( 1, 2,3) 
// return: 0, 
int read_otp_wb(uint index, struct otp_struct *otp_ptr, struct msm_sensor_ctrl_t *s_ctrl) 
{ 
int  bank,address,temp; 

// select bank index 
bank = 0xc0 | index; 
write_sccb16(0x3d84, bank,s_ctrl); 
// read otp into buffer 
write_sccb16(0x3d81, 0x01,s_ctrl); 
msleep(5); 
address = 0x3d00; 
otp_ptr->module_integrator_id = read_sccb16(address + 1,s_ctrl); 
otp_ptr->lens_id = read_sccb16(address + 2,s_ctrl); 
otp_ptr->production_year = read_sccb16(address + 3,s_ctrl);
otp_ptr->production_month = read_sccb16(address + 4,s_ctrl);
otp_ptr->production_day = read_sccb16(address + 5,s_ctrl);

temp=read_sccb16(address + 10,s_ctrl); 

otp_ptr->rg_ratio = (read_sccb16(address + 6,s_ctrl)<<2)+((temp>>6)&0x03); 
otp_ptr->bg_ratio =(read_sccb16(address + 7,s_ctrl)<<2)+((temp>>4)&0x03); 
otp_ptr->light_rg = (read_sccb16(address + 8,s_ctrl)<<2)+((temp>>2)&0x03); 
otp_ptr->light_bg = (read_sccb16(address + 9,s_ctrl)<<2)+(temp&0x03); 

otp_ptr->user_data[0] = read_sccb16(address + 11,s_ctrl); 
otp_ptr->user_data[1] = read_sccb16(address + 12,s_ctrl); 
otp_ptr->user_data[2] = read_sccb16(address + 13,s_ctrl); 
otp_ptr->user_data[3] = read_sccb16(address + 14,s_ctrl); 
otp_ptr->user_data[4] = read_sccb16(address + 15,s_ctrl); 
// disable otp read 
write_sccb16(0x3d81, 0x00,s_ctrl); 
// clear otp buffer 
clear_otp_buffer(s_ctrl); 

return 0; 
} 
 
// index: index of otp group. (1, 2, 3) 
// return: 0 
int read_otp_lenc(uint index, struct otp_struct *otp_ptr, struct msm_sensor_ctrl_t *s_ctrl) 
{ 
 
int bank, i; 
int address; 
// select bank: index*4 
bank = 0xc0 | (index*4); 
write_sccb16(0x3d84, bank,s_ctrl); 
// read otp into buffer 
write_sccb16(0x3d81, 0x01,s_ctrl); 
msleep(5); 
address = 0x3d01; 
for(i=0;i<15;i++) { 
otp_ptr->lenc[i]=read_sccb16(address,s_ctrl); 
address++; 
} 
// disable otp read 
write_sccb16(0x3d81, 0x00,s_ctrl); 
// clear otp buffer 
clear_otp_buffer(s_ctrl); 
// select 2nd bank 
bank++; 
write_sccb16(0x3d84, bank ,s_ctrl); 
// read otp 
write_sccb16(0x3d81, 0x01,s_ctrl); 
msleep(5); 
address = 0x3d00; 
for(i=15;i<31;i++) { 
otp_ptr->lenc[i]=read_sccb16(address,s_ctrl); 
address++; 
} 
// disable otp read 
write_sccb16(0x3d81, 0x00,s_ctrl); 
// clear otp buffer 
clear_otp_buffer(s_ctrl); 
// select 3rd bank 
bank++; 
write_sccb16(0x3d84, bank ,s_ctrl); 
// read otp 
write_sccb16(0x3d81, 0x01,s_ctrl); 
msleep(5); 
address = 0x3d00; 
for(i=31;i<47;i++) { 
otp_ptr->lenc[i]=read_sccb16(address,s_ctrl); 
address++; 
} 
// disable otp read 
write_sccb16(0x3d81, 0x00,s_ctrl); 
// clear otp buffer 
clear_otp_buffer(s_ctrl); 
// select 4th bank 
bank++; 
write_sccb16(0x3d84, bank ,s_ctrl); 
// read otp 
write_sccb16(0x3d81, 0x01,s_ctrl); 
msleep(5); 
address = 0x3d00; 
for(i=47;i<62;i++) { 
otp_ptr->lenc[i]=read_sccb16(address,s_ctrl); 
address++; 
} 
// disable otp read 
write_sccb16(0x3d81, 0x00,s_ctrl); 
// clear otp buffer 
clear_otp_buffer(s_ctrl); 
return 0; 
} 
 
//R_gain: red gain of sensor AWB, 0x400 = 1 
// G_gain: green gain of sensor AWB, 0x400 = 1 
// B_gain: blue gain of sensor AWB, 0x400 = 1 
// return 0 
int update_awb_gain(uint32_t R_gain, uint32_t G_gain, uint32_t B_gain, struct msm_sensor_ctrl_t *s_ctrl) 
{ 
pr_err("otpwb %s  R_gain =%x  0x3400=%x",__func__,R_gain,R_gain>>8); 
pr_err("otpwb %s  R_gain =%x  0x3401=%x",__func__,R_gain,R_gain & 0x00ff); 
pr_err("otpwb %s  G_gain =%x  0x3402=%x",__func__,G_gain,G_gain>>8); 
pr_err("otpwb %s  G_gain =%x  0x3403=%x",__func__,G_gain,G_gain & 0x00ff); 
pr_err("otpwb %s  B_gain =%x  0x3404=%x",__func__,B_gain,B_gain>>8); 
pr_err("otpwb %s  B_gain =%x  0x3405=%x",__func__,B_gain,B_gain & 0x00ff); 
 
if (R_gain>0x400) { 
write_sccb16(0x3400, R_gain>>8,s_ctrl); 
write_sccb16(0x3401, R_gain & 0x00ff,s_ctrl); 
} 
if (G_gain>0x400) { 
write_sccb16(0x3402, G_gain>>8,s_ctrl); 
write_sccb16(0x3403, G_gain & 0x00ff,s_ctrl); 
} 
if (B_gain>0x400) { 
write_sccb16(0x3404, B_gain>>8,s_ctrl); 
write_sccb16(0x3405, B_gain & 0x00ff,s_ctrl); 
} 
return 0; 
} 
 
 
 
int update_lenc(struct otp_struct *otp_ptr, struct msm_sensor_ctrl_t *s_ctrl) 
{ 
                int i, temp; 
		   temp= read_sccb16(0x5000,s_ctrl); 
                temp = 0x80 | temp; 
                write_sccb16(0x5000, temp,s_ctrl); 
                for(i=0;i<62;i++) { 
					
		pr_err("%s  lenc[%d]=0x%x",__func__,i,otp_ptr->lenc[i]); 
   
                write_sccb16(0x5800 + i, otp_ptr->lenc[i],s_ctrl); 
                } 
                return 0; 
} 
 
// R/G and B/G of typical camera module is defined here 
uint RG_Ratio_typical ; 
uint BG_Ratio_typical ; 




// call this function after OV8820 initialization 
// return value: 0 update success 
// 1, no OTP 
int load_otp_wb(struct msm_sensor_ctrl_t *s_ctrl) 
{ 
uint i, temp, otp_index; 
uint32_t  R_gain,G_gain,B_gain,G_gain_R, G_gain_B; 
int rg,bg; 

// R/G and B/G of current camera module is read out from sensor OTP 
// check first wb OTP with valid data 
for(i=1;i<=3;i++) {                             //shawn 20130408
	temp = check_otp_wb(i,s_ctrl); 
	pr_err("otpwb %s  temp =%d  i=%d",__func__,temp,i); 
	if (temp == 2) { 
		otp_index = i; 
		break; 
	} 
}               
if (i>3) {                                     //shawn 20130408
pr_err("otpwb %s  i=%d   no valid wb OTP data ",__func__,i); 
return 1; 
} 
read_otp_wb(otp_index, &current_ov8835_otp,s_ctrl); 
 
if(current_ov8835_otp.module_integrator_id==0x01){ 
//sunny 
BG_Ratio_typical=0x49; 
RG_Ratio_typical=0x4d; 
}else if(current_ov8835_otp.module_integrator_id==0x02){ 
//truly 
BG_Ratio_typical=0x53; 
RG_Ratio_typical=0x55; 
}else if(current_ov8835_otp.module_integrator_id==0x06){ 
//qtech 
BG_Ratio_typical=0x53; 
RG_Ratio_typical=0x55; 
}else if(current_ov8835_otp.module_integrator_id==0x15){ 
//liteon
BG_Ratio_typical=0x116; 
RG_Ratio_typical=0x102; 
}else if(current_ov8835_otp.module_integrator_id==0x31){ 
//mcnex
BG_Ratio_typical=0x53; 
RG_Ratio_typical=0x55; 
}else{ 
//default liteon 
BG_Ratio_typical=0x116; 
RG_Ratio_typical=0x102; 
} 
if(current_ov8835_otp.light_rg==0) { 
	// no light source information in OTP, light factor = 1 
	rg = current_ov8835_otp.rg_ratio; 
} else { 
	rg = current_ov8835_otp.rg_ratio * ((current_ov8835_otp.light_rg +512) / 1024); 
} 
if(current_ov8835_otp.light_bg==0) { 
// not light source information in OTP, light factor = 1 
	bg = current_ov8835_otp.bg_ratio; 
} else { 
	bg = current_ov8835_otp.bg_ratio * ((current_ov8835_otp.light_bg +512) / 1024); 
} 
 
if(rg==0||bg==0) return 0; 
 
//calculate gain 
//0x400 = 1x gain 
if(bg < BG_Ratio_typical) 
{ 
        if (rg < RG_Ratio_typical){ 
                // current_ov8835_otp.bg_ratio < BG_Ratio_typical && 
                // current_ov8835_otp.rg_ratio < RG_Ratio_typical 
                G_gain = 0x400; 
                B_gain = 0x400 * BG_Ratio_typical / bg; 
                R_gain = 0x400 * RG_Ratio_typical / rg; 
                } else{ 
                // current_ov8835_otp.bg_ratio < BG_Ratio_typical && 
                // current_ov8835_otp.rg_ratio >= RG_Ratio_typical 
                R_gain = 0x400; 
                G_gain = 0x400 * rg / RG_Ratio_typical; 
                B_gain = G_gain * BG_Ratio_typical / bg; 
                } 
        } else{ 
        
                if (rg < RG_Ratio_typical){ 
                // current_ov8835_otp.bg_ratio >= BG_Ratio_typical && 
                // current_ov8835_otp.rg_ratio < RG_Ratio_typical 
                B_gain = 0x400; 
                G_gain = 0x400 * bg / BG_Ratio_typical; 
                R_gain = G_gain * RG_Ratio_typical / rg; 
                } else{ 
                // current_ov8835_otp.bg_ratio >= BG_Ratio_typical && 
                // current_ov8835_otp.rg_ratio >= RG_Ratio_typical 
                G_gain_B = 0x400 * bg / BG_Ratio_typical; 
                G_gain_R = 0x400 * rg / RG_Ratio_typical; 
                if(G_gain_B > G_gain_R ) 
                { 
                B_gain = 0x400; 
                G_gain = G_gain_B; 
                R_gain = G_gain * RG_Ratio_typical / rg; 
                } 
                else 
                { 
                R_gain = 0x400; 
                G_gain = G_gain_R; 
                B_gain = G_gain * BG_Ratio_typical / bg; 
                } 
                } 
} 

	current_ov8835_otp.final_R_gain=R_gain;
	current_ov8835_otp.final_G_gain=G_gain;
	current_ov8835_otp.final_B_gain=B_gain;
	current_ov8835_otp.wbwritten=1;
// write sensor wb gain to registers 
//update_awb_gain(current_ov8835_otp.final_R_gain, current_ov8835_otp.final_G_gain, current_ov8835_otp.final_B_gain,s_ctrl); 
return 0; 
} 
 
// call this function after OV8820 initialization 
// return value: 0 update success 
// 1, no OTP 
int load_otp_lenc(struct msm_sensor_ctrl_t *s_ctrl) 
{ 
 
uint i, temp, otp_index; 
 
// check first lens correction OTP with valid data 
 
for(i=1;i<=3;i++) {         //shawn 20130408
	temp = check_otp_lenc(i,s_ctrl); 
	if (temp == 2) { 
		pr_err("otplenc %s  temp =%d  i=%d",__func__,temp,i); 
		otp_index = i; 
		break; 
	} 
} 
if (i>3) {     //shawn 20130408
	// no lens correction data 
	pr_err("otplenc %s  i=%d   no valid lenc OTP data ",__func__,i); 
	return 1; 
} 
read_otp_lenc(otp_index, &current_ov8835_otp,s_ctrl); 
current_ov8835_otp.lencwritten=1;
//update_lenc(&current_ov8835_otp,s_ctrl); 
//success 
return 0; 
} 
 
// index: index of otp group. (1, 2, 3) 
//code:0 start code,1 stop code
// return: 0, group index is empty 
// 1, group index has invalid data 
// 2, group index has valid data 
int check_otp_VCM(int index,int code, struct msm_sensor_ctrl_t *a_ctrl) 
{ 
            int flag,  bank; 
            int address; 
            // select bank: 16 
            bank = 0xc0 + 16; 
            write_sccb16(0x3d84, bank,a_ctrl); 
            // read otp into buffer 
            write_sccb16(0x3d81, 0x01,a_ctrl); 
            msleep(5); 
            // read flag 
            address = 0x3d00 + (index-1)*4+code*2; 
            flag = read_sccb16(address,a_ctrl); 
		pr_err("otpVCM %s index =%d , code=%d ,flag=0x%x",__func__,index,code,flag); 
            flag = flag & 0xc0; 
            // disable otp read 
            write_sccb16(0x3d81, 0x00,a_ctrl); 
            // clear otp buffer 
            clear_otp_buffer(a_ctrl); 
            if (flag==0) { 
            return 0; 
            } 
            else if (flag & 0x80) { 
            return 1; 
            } 
            else { 
            return 2; 
            } 
 
} 

// index: index of otp group. (1, 2, 3) 
//code:0 start code,1 stop code
// return: 0, 
int read_otp_VCM(int index,int code,struct msm_sensor_ctrl_t *a_ctrl) 
{ 
        int vcm, bank; 
        int address; 
        // select bank: 16 
        bank = 0xc0 + 16; 
        write_sccb16(0x3d84, bank,a_ctrl); 
        // read otp into buffer 
        write_sccb16(0x3d81, 0x01,a_ctrl); 
        msleep(5); 
        // read flag 
        address = 0x3d00 + (index-1)*4+code*2; 
        vcm = read_sccb16(address,a_ctrl); 
        vcm = (vcm & 0x03) + (read_sccb16(address+1,a_ctrl)<<2); 

	  if(code==1){
		current_ov8835_otp.VCM_end=vcm;
	  }else{
		current_ov8835_otp.VCM_start=vcm;
	  }
	  
        // disable otp read 
        write_sccb16(0x3d81, 0x00,a_ctrl); 
        // clear otp buffer 
        clear_otp_buffer(a_ctrl); 
        return 0; 
 
} 
 
int load_otp_VCM(struct msm_sensor_ctrl_t *a_ctrl) 
{ 
   int i, code,temp, otp_index; 
   for(code=0;code<2;code++) { 
                for(i=1;i<=3;i++) { 
                        temp = check_otp_VCM(i,code,a_ctrl); 
                        pr_err("otpaf %s  temp =%d , i=%d,code=%d",__func__,temp,i,code); 
                        if (temp == 2) { 
                                otp_index = i; 
                                break; 
                        } 
                } 
                if (i>3) { 
                        pr_err("otpaf %s  i=%d   no valid af OTP data ",__func__,i); 
                        return 1; 
                } 
 
                read_otp_VCM(otp_index,code, a_ctrl); 

   }
                return 0; 
} 
 
// return: 0 ¨Cuse module DCBLC, 
// 1 ¨Cuse sensor DCBLC 
// 2 ¨Cuse defualt DCBLC 
int update_blc_ratio(struct msm_sensor_ctrl_t *a_ctrl) 
{ 
int K,temp; 

// select bank 31 
write_sccb16(0x3d84, 0xdf,a_ctrl); 

// read otp into buffer 
write_sccb16(0x3d81, 0x01,a_ctrl); 
msleep(5); 

K  = read_sccb16(0x3d0b,a_ctrl); 
if(K!=0){
	if ((K>=0x15) && (K<=0x40)){ 
		//auto load mode
		pr_err("%s  auto load mode ",__func__);
		temp = read_sccb16(0x4008,a_ctrl); 
		temp &= 0xfb;
		write_sccb16(0x4008, temp,a_ctrl); 

		temp = read_sccb16(0x4000,a_ctrl); 
		temp &= 0xf7;
		write_sccb16(0x4000, temp,a_ctrl); 
		return 2; 
	}
}

K  = read_sccb16(0x3d0a,a_ctrl); 
if ((K>=0x10) && (K<=0x40)){ 
	//manual load mode
	pr_err("%s  manual load mode ",__func__);
	write_sccb16(0x4006, K,a_ctrl); 
	
	temp = read_sccb16(0x4008,a_ctrl); 
	temp &= 0xfb;
	write_sccb16(0x4008, temp,a_ctrl); 

	temp = read_sccb16(0x4000,a_ctrl); 
	temp |= 0x08;
	write_sccb16(0x4000, temp,a_ctrl); 
	return 1; 
}else{ 
	//set to default
	pr_err("%s  set to default ",__func__);
	write_sccb16(0x4006, 0x20,a_ctrl); 
	
	temp = read_sccb16(0x4008,a_ctrl); 
	temp &= 0xfb;
	write_sccb16(0x4008, temp,a_ctrl); 

	temp = read_sccb16(0x4000,a_ctrl); 
	temp |= 0x08;
	write_sccb16(0x4000, temp,a_ctrl); 
	return 0; 
}

} 


int32_t ov8835_msm_sensor_otp_write(struct msm_sensor_ctrl_t *s_ctrl)
{
       
	if(current_ov8835_otp.wbwritten){
		pr_err("%s: wb otp is writing\n", __func__);
		update_awb_gain(current_ov8835_otp.final_R_gain, current_ov8835_otp.final_G_gain, current_ov8835_otp.final_B_gain,s_ctrl); 
	}else{
		pr_err("%s: no wb otp \n", __func__);
	}

	if(current_ov8835_otp.lencwritten){
		pr_err("%s: lenc otp is writing\n", __func__);
		update_lenc(&current_ov8835_otp,s_ctrl); 
	}else{
		pr_err("%s: no lenc otp \n", __func__);
	}

	update_blc_ratio(s_ctrl);
	
	return 0;
}

int32_t ov8835_msm_sensor_otp_probe(struct msm_sensor_ctrl_t *s_ctrl)
{
       pr_err("%s: %d\n", __func__, __LINE__);

	load_otp_wb(s_ctrl);
	
	load_otp_lenc(s_ctrl);

	load_otp_VCM(s_ctrl);
	
	//update_blc_ratio(s_ctrl);

	if(current_ov8835_otp.module_integrator_id){
		pr_err("%s  current_ov8835_otp: module id =0x%x\n",__func__,current_ov8835_otp.module_integrator_id);
		pr_err("%s  current_ov8835_otp: lens_id =0x%x\n",__func__,current_ov8835_otp.lens_id);
		pr_err("%s  current_ov8835_otp: production_year =%d\n",__func__,current_ov8835_otp.production_year);
		pr_err("%s  current_ov8835_otp: production_month =%d\n",__func__,current_ov8835_otp.production_month);
		pr_err("%s  current_ov8835_otp: production_day =%d\n",__func__,current_ov8835_otp.production_day);
		pr_err("%s  current_ov8835_otp: rg_ratio =0x%x\n",__func__,current_ov8835_otp.rg_ratio);
		pr_err("%s  current_ov8835_otp: bg_ratio =0x%x\n",__func__,current_ov8835_otp.bg_ratio);
		pr_err("%s  current_ov8835_otp: light_rg =0x%x\n",__func__,current_ov8835_otp.light_rg);
		pr_err("%s  current_ov8835_otp: light_bg =0x%x\n",__func__,current_ov8835_otp.light_bg);
		pr_err("%s  current_ov8835_otp: user_data[0] =0x%x\n",__func__,current_ov8835_otp.user_data[0]);
		pr_err("%s  current_ov8835_otp: user_data[1] =0x%x\n",__func__,current_ov8835_otp.user_data[1]);
		pr_err("%s  current_ov8835_otp: user_data[2] =0x%x\n",__func__,current_ov8835_otp.user_data[2]);
		pr_err("%s  current_ov8835_otp: user_data[3] =0x%x\n",__func__,current_ov8835_otp.user_data[3]);
		pr_err("%s  current_ov8835_otp: user_data[4] =0x%x\n",__func__,current_ov8835_otp.user_data[4]);		
		pr_err("%s  current_ov8835_otp: VCM_start =0x%x\n",__func__,current_ov8835_otp.VCM_start);
		pr_err("%s  current_ov8835_otp: VCM_end =0x%x\n",__func__,current_ov8835_otp.VCM_end);	
	}else{
		pr_err("%s  no  otp  module \n",__func__);
	}
	
	return 0;
}

int32_t msm_ov8835_sensor_power_up(struct msm_sensor_ctrl_t *s_ctrl)
{
	int rc=0;
	pr_err("%s\n", __func__);
	
   	rc=gpio_direction_output(107, 1);
	if (rc < 0)  
		pr_err("%s pwd gpio107  direction 1   failed\n",__func__);
	msleep(3);	   	

	rc=msm_cam_clk_enable(&s_ctrl->sensor_i2c_client->client->
				dev, cam_clk_info, s_ctrl->cam_clk,
				ARRAY_SIZE(cam_clk_info), 1); 
 	if (rc < 0)  
		pr_err("%s msm_cam_clk_enable  failed\n",__func__);
	msleep(3);
	
	return rc;
}

int32_t msm_ov8835_sensor_power_down(struct msm_sensor_ctrl_t *s_ctrl)
{
	int rc=0;
	pr_err("%s\n", __func__);
	
   	rc=gpio_direction_output(107, 0);
	if (rc < 0)  
		pr_err("%s pwd gpio107  direction 0   failed\n",__func__);
	rc=msm_cam_clk_enable(&s_ctrl->sensor_i2c_client->client->
				dev, cam_clk_info, s_ctrl->cam_clk,
				ARRAY_SIZE(cam_clk_info), 0); 
	if (rc < 0)  
		pr_err("%s msm_cam_clk_enable  failed\n",__func__);
	
	return rc;
}

int32_t ov8835_msm_sensor_bayer_i2c_probe(struct i2c_client *client,
	const struct i2c_device_id *id)
{
	int rc = 0;
	struct msm_sensor_ctrl_t *s_ctrl;
	pr_err("%s %s_i2c_probe called\n", __func__, client->name);
	if (!i2c_check_functionality(client->adapter, I2C_FUNC_I2C)) {
		pr_err("%s %s i2c_check_functionality failed\n",
			__func__, client->name);
		rc = -EFAULT;
		return rc;
	}

	s_ctrl = (struct msm_sensor_ctrl_t *)(id->driver_data);
	if (s_ctrl->sensor_i2c_client != NULL) {
		s_ctrl->sensor_i2c_client->client = client;
		if (s_ctrl->sensor_i2c_addr != 0)
			s_ctrl->sensor_i2c_client->client->addr =
				s_ctrl->sensor_i2c_addr;
	} else {
		pr_err("%s %s sensor_i2c_client NULL\n",
			__func__, client->name);
		rc = -EFAULT;
		return rc;
	}

	s_ctrl->sensordata = client->dev.platform_data;
	if (s_ctrl->sensordata == NULL) {
		pr_err("%s %s NULL sensor data\n", __func__, client->name);
		return -EFAULT;
	}
   	rc=gpio_direction_output(54, 0);
	if (rc < 0)  
		pr_err("%s pwd gpio54  direction 0   failed\n",__func__);
	rc=gpio_direction_output(107, 0);
	if (rc < 0)  
		pr_err("%s pwd gpio107 direction 0   failed\n",__func__);

	rc = msm_sensor_bayer_power_up(s_ctrl);
	if (rc < 0) {
		pr_err("%s %s power on failed\n", __func__, client->name);
		return rc;
	}

	if (s_ctrl->func_tbl->sensor_match_id)
		rc = s_ctrl->func_tbl->sensor_match_id(s_ctrl);
	else
		rc = msm_sensor_bayer_match_id(s_ctrl);
	if (rc < 0)
		goto probe_fail;

	rc = msm_sensor_write_all_conf_array(
		s_ctrl->sensor_i2c_client,
		s_ctrl->msm_sensor_reg->init_settings,
		s_ctrl->msm_sensor_reg->init_size);
	if (rc < 0)
		pr_err("%s %s write init setting table failed\n", __func__, client->name);
	
       msleep(10);
	   
	   rc= msm_camera_i2c_write( 
                        s_ctrl->sensor_i2c_client, 
                        0x0100, 0x01, 
                        MSM_CAMERA_I2C_BYTE_DATA); 
	rc=ov8835_msm_sensor_otp_probe(s_ctrl);

#ifdef CONFIG_SENSOR_INFO 
    	msm_sensorinfo_set_back_sensor_id(s_ctrl->sensor_id_info->sensor_id);
#else
  //do nothing here
#endif	

	if (!s_ctrl->wait_num_frames)
		s_ctrl->wait_num_frames = 1;

	pr_err("%s %s probe succeeded\n", __func__, client->name);
	snprintf(s_ctrl->sensor_v4l2_subdev.name,
		sizeof(s_ctrl->sensor_v4l2_subdev.name), "%s", id->name);
	v4l2_i2c_subdev_init(&s_ctrl->sensor_v4l2_subdev, client,
		s_ctrl->sensor_v4l2_subdev_ops);
	s_ctrl->sensor_v4l2_subdev.flags |= V4L2_SUBDEV_FL_HAS_DEVNODE;
	media_entity_init(&s_ctrl->sensor_v4l2_subdev.entity, 0, NULL, 0);
	s_ctrl->sensor_v4l2_subdev.entity.type = MEDIA_ENT_T_V4L2_SUBDEV;
	s_ctrl->sensor_v4l2_subdev.entity.group_id = SENSOR_DEV;
	s_ctrl->sensor_v4l2_subdev.entity.name =
		s_ctrl->sensor_v4l2_subdev.name;
	msm_sensor_register(&s_ctrl->sensor_v4l2_subdev);
	s_ctrl->sensor_v4l2_subdev.entity.revision =
		s_ctrl->sensor_v4l2_subdev.devnode->num;
	msm_sensor_enable_debugfs(s_ctrl);
	goto power_down;
probe_fail:
	pr_err("%s %s_i2c_probe failed\n", __func__, client->name);
power_down:
	if (rc > 0)
		rc = 0;
	 s_ctrl->func_tbl->sensor_power_down(s_ctrl);
	return rc;
}

void msm_sensor_ov8835_start_stream(struct msm_sensor_ctrl_t *s_ctrl)
{
	msm_camera_i2c_write(
		s_ctrl->sensor_i2c_client,
		0x4202,
		0x00,
		s_ctrl->msm_sensor_reg_default_data_type);
}

void msm_sensor_ov8835_stop_stream(struct msm_sensor_ctrl_t *s_ctrl)
{
	msm_camera_i2c_write(
		s_ctrl->sensor_i2c_client,
		0x4202,
		0x0f,
		s_ctrl->msm_sensor_reg_default_data_type);	
}

static const struct i2c_device_id ov8835_i2c_id[] = {
	{SENSOR_NAME, (kernel_ulong_t)&ov8835_s_ctrl},
	{ }
};

static struct i2c_driver ov8835_i2c_driver = {
	.id_table = ov8835_i2c_id,
	.probe  = ov8835_msm_sensor_bayer_i2c_probe,
	.driver = {
		.name = SENSOR_NAME,
	},
};

static struct msm_camera_i2c_client ov8835_sensor_i2c_client = {
	.addr_type = MSM_CAMERA_I2C_WORD_ADDR,
};

static struct v4l2_subdev_core_ops ov8835_subdev_core_ops = {
	.ioctl = msm_sensor_bayer_subdev_ioctl,
	.s_power = msm_sensor_bayer_power,
};

static struct v4l2_subdev_video_ops ov8835_subdev_video_ops = {
	.enum_mbus_fmt = msm_sensor_bayer_v4l2_enum_fmt,
};

static struct v4l2_subdev_ops ov8835_subdev_ops = {
	.core = &ov8835_subdev_core_ops,
	.video  = &ov8835_subdev_video_ops,
};


static struct msm_sensor_fn_t ov8835_func_tbl = {
	.sensor_config = msm_sensor_bayer_config,
	.sensor_start_stream = msm_sensor_ov8835_start_stream,
	.sensor_stop_stream = msm_sensor_ov8835_stop_stream,
	.sensor_power_up = msm_ov8835_sensor_power_up,
	.sensor_power_down = msm_ov8835_sensor_power_down,
	.sensor_get_csi_params = msm_sensor_bayer_get_csi_params,
	.sensor_otp_func = ov8835_msm_sensor_otp_write,
};



static struct msm_sensor_reg_t ov8835_regs = {
	.default_data_type = MSM_CAMERA_I2C_BYTE_DATA,
	//.init_settings = &ov8835_init_conf[0],
	//.init_size = ARRAY_SIZE(ov8835_init_conf),
};

static struct msm_sensor_ctrl_t ov8835_s_ctrl = {
	.msm_sensor_reg = &ov8835_regs,
	.sensor_i2c_client = &ov8835_sensor_i2c_client,
	.sensor_i2c_addr = 0x6C,
	.power_seq = &ov8835_power_seq[0],
	.num_power_seq = ARRAY_SIZE(ov8835_power_seq),
	.sensor_id_info = &ov8835_id_info,
	.msm_sensor_mutex = &ov8835_mut,
	.sensor_v4l2_subdev_info = ov8835_subdev_info,
	.sensor_v4l2_subdev_info_size = ARRAY_SIZE(ov8835_subdev_info),
	.sensor_v4l2_subdev_ops = &ov8835_subdev_ops,
	.func_tbl = &ov8835_func_tbl,
	.clk_rate = MSM_SENSOR_MCLK_24HZ,
	.msm_sensor_reg_default_data_type=MSM_CAMERA_I2C_BYTE_DATA,
};
