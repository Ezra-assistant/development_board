#ifndef _API_UPDATE_H
#define _API_UPDATE_H

int updatefile_init(const char *file);      //初始化UPDATE模块
void updateproc(void);                      //升级流程
void ota_enter(void);                       //进入OTA流程
void ota_exit(void);                        //退出OTA流程
u8 get_uart_upd_step(void);
void uart_send_recive_success(void);
void set_uart_upd_step(u8 step);
void uart_upd_init(u32 uart_port,u32 uart_baud,u8 sys_clk);
void uart_upd_isr_init(void);

#endif
