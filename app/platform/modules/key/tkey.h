#ifndef _TKEY_H
#define _TKEY_H

typedef struct {
    u16 tkcd_pr;
} tk_init_typedef;

#define TK_RC2M_CLK      0
#define TK_RTC_RC2M_CLK  1
#define TK_XOSC24M_CLK   2
#define TK_XOSC48M_CLK  3

void bsp_tkey_init(tk_init_typedef *tk_cfg);
void tkey_gpio_config(u8 io_num);
void tkey_gpio_exit(u8 io_num);
void tkey_off(void);
void tkey_cir_scan_en(void);
void tkey_press_update(void);
void tkey_tkbuf_down(void);
extern volatile u32 tk_buf[];
extern u8 tkey_array[];
#endif
