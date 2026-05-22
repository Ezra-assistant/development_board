#ifndef _FUNC_H
#define _FUNC_H

#include "func_idle.h"
#include "func_music.h"
#include "func_lowpwr.h"
#include "func_uart_upd.h"
#include "func_exspiflash_music.h"
#include "func_clock.h"

//task number
enum {
    FUNC_NULL = 0,
    FUNC_MUSIC,
    FUNC_FMRX,
    FUNC_CLOCK,
    FUNC_USBDEV,
    FUNC_AUX,
    FUNC_SPDIF,
    FUNC_SPEAKER,
    FUNC_PWROFF,
    FUNC_SLEEPMODE,
    FUNC_EXSPIFLASH_MUSIC,
    FUNC_I2S,
    FUNC_IDLE,
};

//task control block
typedef struct {
    u8 sta;                                         //cur working task number
    u8 last;                                        //lask task number
    void (*mp3_res_play)(u32 addr, u32 len, u8 loop_cnt);        //各任务的语音播报函数接口
    void (*set_vol_callback)(u8 dir);               //设置音量的回调函数，用于各任务的音量事件处理。
} func_cb_t;
extern func_cb_t func_cb;
extern const u8 func_sort_table[];     //任务切换排序table

/* 自定义的内容 */
#if 1
#define KEY_STATES 12

extern u16 leds_states[KEY_STATES];

// 全局 msg 变量
extern u16 play_num_msg;

extern u32 check_irrx_register; // irrx 测试寄存器
extern u8 irrx_signal_capture_flag;
 


#endif

ALWAYS_INLINE void func_mp3_res_play(u32 addr, u32 len, u8 loop_cnt)
{
    if (func_cb.mp3_res_play) {
        func_cb.mp3_res_play(addr, len, loop_cnt);
    }
}

ALWAYS_INLINE void func_set_vol_callback(u8 dir)
{
    if (func_cb.set_vol_callback) {
        func_cb.set_vol_callback(dir);
    }
}

u8 get_funcs_total(void);
void func_process(void);
void func_message(u16 msg);

// void func_run(void);
void user_main(void);
void func_music(void);
void func_idle(void);
void func_clock(void);
void func_fmrx(void);
void func_usbdev(void);
void func_aux(void);
void func_spdif(void);
void func_exspiflash_music(void);
#endif // _FUNC_H
