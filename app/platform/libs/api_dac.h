#ifndef _API_DAC_H_
#define _API_DAC_H_

enum {
    SPR_48000   = 0,
    SPR_32000,
    SPR_24000,
    SPR_16000,
    SPR_12000,
    SPR_8000,
};

void dac_cb_init(void);
void dac_spr_set(uint spr);                             //设置dac采样率
void dac_set_dvol(u16 vol);                             //设置dac数字音量
void dac_restart(void);                                 //重启dac模块
void dac_power_on(u8 dac_drv);                          //开启dac
void dac_power_off(void);                               //关闭dac
void dac_io_map(u8 ch0_map_sel,u8 ch1_map_sel);         //dac引脚重映射(AB15X不支持)
void dac_classd_dec_proc(u16 vbat_val);

bool dac_gpdma_is_ready(void);                          //获取dac pending
void dac_gpdma_kick(s16 *ptr, u32 samples);             //输入dac数据, ptr:指向音频数据, samples:16bit位宽的样点数
void dac_gpdma_reset(void);                             //复位dac数据
u16 dnr_buf_maxpow(void *ptr, u16 len);                 //计算ptr指向的音频数据能量值并返回, ptr:指向音频数据, len:8bit位宽的数据长度

//fade time(s) = (digitat_vol - 0)/(2^step)/dac_spr
//for example: 0.17s = (0x7fff - 0)/(2^2)/48000
void dac_fade_in(u8 step);                              //dac淡入
void dac_fade_out(u8 step);                             //dac淡出

void dac_dnr_in(void);                                  //dac_dnr进入
void dac_dnr_out(void);                                 //dac_dnr退出

void dac_analg_on(void);                                //使能dac模拟
void dac_analg_off(void);                               //关闭dac模拟
void dac_dc_exp_on(void);
void dac_dc_exp_off(void);

void dac_voutp_set(u8 io_sta);                          //VOUTP引脚输出,io_sta: 0-输出低电平, 1-输出高电平(vbat电压)
void dac_voutn_set(u8 io_sta);                          //VOUTN引脚输出,io_sta: 0-输出低电平, 1-输出高电平(vbat电压)
#endif
