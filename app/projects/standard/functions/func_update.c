#include "include.h"
#include "func.h"
#include "func_update.h"

#define TRACE_EN                1
#if TRACE_EN
#define TRACE(...)              my_printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

#if USB_SD_UPDATE_EN || UART_S_UPDATE ||UART_M_UPDATE
int updateproc_usbsd_bkupdate(void);

typedef struct _func_upd_t {
    uint sta;
} func_upd_t;

func_upd_t f_upd;

const char upd_filename[13] = UPD_FILENAME;

AT(.text.func.update)
void func_update(void)
{
    int res;
    res = updatefile_init(upd_filename);
    if (res == -1) {
        return;
    }
    if (res == 0) {
        TRACE("File is Same,No Need Update\n");
        // mp3_res_play(RES_BUF_UPDATE_DONE_MP3, RES_LEN_UPDATE_MP3, 0);
        return;
    }
    // mp3_res_play(RES_BUF_UPDATE_MP3, RES_LEN_UPDATE_MP3, 0);
#if SYS_KARAOK_EN
    bsp_karaok_exit(AUDIO_PATH_KARAOK);
#endif
    TRACE("\nupdate go...\n");
#if UART_S_UPDATE
    u8 get_uart_upd_step(void);
    if(get_uart_upd_step()){
        TMR1CON = 0;
    }
#endif
#if !HUART_EN || !HUART_PRINTF_EN
    my_printf_init(huart_putchar_null); //关闭高速串口打印
#endif
    updateproc();                                 //升级
    while (1);
}
#endif  //USB_SD_UPDATE_EN


#if UART_S_UPDATE
u8 get_uart_upd_step(void);
void uart_upd_param_init(void);
void set_uart_upd_step(u8 step);
void uart_s_update(void)
{
    if(get_uart_upd_step()){
        printf("to update...\n");
        delay_ms(20);//发送完start响应之后，延时一段时间等待主机进入升级模式。防止主机接收不到cmd
        uart_upd_param_init();
        func_update();                                  //尝试升级
        set_uart_upd_step(0);
    }
}
#if UPDATE_UART_SEL == UPDATE_UART0
void update_uart0baud_in_sysclk_set(u32 uart_baud) {};
#endif
#endif
