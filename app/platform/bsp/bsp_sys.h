#ifndef _BSP_SYS_H
#define _BSP_SYS_H

bool rtc_init(void);
void bsp_sys_mute(void);
void bsp_sys_unmute(void);
void bsp_clr_mute_sta(void);
void uart0_mapping_sel(void);
void linein_detect(void);
bool linein_micl_is_online(void);
void tws_lr_xcfg_sel(void);
void tws_get_lr_channel(u8 tws_status);
void get_usb_chk_sta_convert(void);
void sd_soft_cmd_detect(u32 check_ms);
void dac_pin_gpio_init(bool bo_en);
void dac_voutp_set(u8 io_sta);
void dac_voutn_set(u8 io_sta);
void sys_ram_info_dump(void);
void pwm_hw_cfg_init(void);
void tmr2pwm_cfg_init(void);
void mclr_reset_init(void);
#endif // _BSP_SYS_H

