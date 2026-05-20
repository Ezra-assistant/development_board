#include "include.h"
#include "tkey.h"

#if TKEY_MUL_SCAN_EN
#define TKEY_ADJ_LEVEL_MAX    100

//两个按键同时按下触发的消息
#define TKEY_MUL_CHANNEL_MSG       0

#define TKEY_WRITE_FLASH_TIME      5

#define TKEY_DEBUG_EN            0
#if     TKEY_DEBUG_EN
#define TRACE(...)          my_printf(__VA_ARGS__)
AT(.com_text.rodata)
const char tk_test1[] = "buf[0] %d\n";

AT(.com_text.rodata)
const char tk_test2[] = "buf[1] %d\n";

AT(.com_text.rodata)
const char tk_test3[] = "buf[2] %d\n";

AT(.com_text.rodata)
const char tk_test4[] = "buf[3] %d\n";
#else
#define TRACE(...)
#endif

//定义touchkey按键按下灵敏度
u16 tkey_press_cnt[TKEY_MUL_SCAN_NUM];
u8 tkey_array[TKEY_MUL_SCAN_NUM] = TKEY_ARRAY;
u8 tkey_mul_scan_num = TKEY_MUL_SCAN_NUM;
u8 tkey_tkcnt_adj_en = TKEY_TKCNT_ADJ_EN;
u16 tkey_tkcnt_adj_times = TKEY_TKCNT_ADJ_TIMES;
u8 tkey_tkcnt_adj_level = TKEY_TKCNT_ADJ_LEVEL;
u8 tkey_clk_sel = TKEY_CLK_SEL;

u16 tkey_val_clr_cnt = 0;
u32 tkey_adj_val[TKEY_MUL_SCAN_NUM];
extern const u16 tbl_double_key[];
extern bool tkey_finish_flag;

AT(.com_rodata.tkey.table)
const u8 tkey_table[] = {
    NO_KEY,      //第一位不能改
    KEY_VOL_DOWN,
    KEY_PLAY,
    KEY_VOL_UP,
    KEY_PREV,
    KEY_NEXT,
    KEY_MODE,
    KEY_HSF,
    KEY_VOL_DOWN,
    KEY_PLAY,
    KEY_VOL_UP,
    KEY_PREV,
    KEY_NEXT,
    KEY_MODE,
    KEY_HSF,
    KEY_VOL_DOWN,
    KEY_PLAY,
    KEY_IR_POWER,   //进sleep_mode关机
};

void tkey_press_update(void)
{
    static u32 ticks = 0;
    if (tick_check_expire(ticks,TKEY_PRESS_UPDATE_TIME)) {
        ticks = tick_get();
        tkey_val_clr_cnt = 0;
    }
}

#if TKEY_MUL_CHANNEL_MSG
#define TKEY_CHANNEL0       0
#define TKEY_CHANNEL1       1
#define MUL_MSG             2

AT(.com_text.tkey.proc)
u8 mul_channel_proc(void)
{
    if((tk_buf[TKEY_CHANNEL1] > tkey_press_cnt[TKEY_CHANNEL1]) && (tk_buf[TKEY_CHANNEL0] > tkey_press_cnt[TKEY_CHANNEL0])){
        return tkey_table[MUL_MSG];
    }
    return tkey_table[0];
}
#endif

#if TKEY_SCAN_SWITCH_EN
AT(.com_text.tkey.proc)
void tkey_cir_scan_en(void)
{
    u8 key_val = NO_KEY;

    if(tkey_finish_flag) {
        if(key_val == NO_KEY) {
            key_val = get_tkey();
        }
        msg_enqueue(key_val);
    }
}
#endif

AT(.com_text.tkey.proc)
bool tkey_get_scan_num(void)
{
    for(u8 i = 0; i<= tkey_mul_scan_num; i++) {
        if(tk_buf[i] > tkey_press_cnt[i]) {
        return true;
        }
    }
    return false;
}

AT(.com_text.tkey.proc)
u8 tkey_get_scan(void)
{
    while(tkey_get_scan_num()) {
#if TKEY_MUL_CHANNEL_MSG
        return mul_channel_proc();
#endif // TKEY_MUL_CHANNEL_MSG
        for(u8 i = 0; i < TKEY_MUL_SCAN_NUM; i++) {
#if TKEY_FILTER_EN
            static u16 tkey_val =0;
            for(u8 j = 0; j < TKEY_FILTER_TIMES; j++) {
                tkey_val += tk_buf[i];
                if(j == (TKEY_FILTER_TIMES - 1)) {
                    tkey_val = tkey_val / TKEY_FILTER_TIMES;
                }
            }
            if(tkey_val > tkey_press_cnt[i]) {
                tkey_val = 0;
                return tkey_table[i+1];
            }
            tkey_val = 0;
#else
            if(tk_buf[i] > tkey_press_cnt[i]) {
                return tkey_table[i+1];
            }
#endif
        }
    }
    return tkey_table[0];
}

#if TKEY_TKCNT_ADJ_EN

//自动检测tkey的值
AT(.com_text.tkey)
void tkey_val_clr(u32 *key_val)
{
    u32 *p = key_val;
    if(tkey_val_clr_cnt < tkey_tkcnt_adj_times)
    {
        for(u8 i = 0; i < tkey_mul_scan_num; i++) {                         //累加tkey_adj_val
            tkey_adj_val[i] = tkey_adj_val[i] + (*(p + i));
        }
    }
    if(tkey_val_clr_cnt == tkey_tkcnt_adj_times) {
        for(u8 i = 0; i < tkey_mul_scan_num; i++) {
            tkey_adj_val[i] = tkey_adj_val[i] / tkey_tkcnt_adj_times;       //求平均
            tkey_press_cnt[i] = tkey_adj_val[i] + (tkey_adj_val[i] / TKEY_ADJ_LEVEL_MAX) * tkey_tkcnt_adj_level;   //设定灵敏度
            //my_printf(testa2,tkey_press_cnt[i]);
         }
    }
}
#endif // TKEY_TKCNT_ADJ_EN

static u32 cnt = 0;
AT(.text.func.process)
void tkey_tkbuf_down(void)
{
    static u32 ticks = 0;
    if (tick_check_expire(ticks,TKEY_WRITE_FLASH_TIME)) {
        ticks = tick_get();
        //my_printf("%d\n",tk_buf[0]);
        spiflash_program((void *)&tk_buf[0],0x20000 + cnt*2,2);  //一个tkbuf 2byte
        cnt++;
    }
}

//tkey中断
AT(.com_text.irq)
void tkey_isr(void)
{
    if(TKCON & BIT(16)){
        TKCON |= BIT(17);                      //clear pending
        TRACE(tk_test1,tk_buf[0]);
        TRACE(tk_test2,tk_buf[1]);
        TRACE(tk_test3,tk_buf[2]);
        TRACE(tk_test4,tk_buf[3]);
#if TKEY_TKCNT_ADJ_EN
        if(tkey_val_clr_cnt <tkey_tkcnt_adj_times) {
            tkey_val_clr_cnt++;
            tkey_val_clr((u32*)tk_buf);
        }
#endif // TKEY_TKCNT_ADJ_EN
    }
}

AT(.com_text.bsp.tkey)
u8 get_tkey(void)
{
    u8 value = 0;
    value = tkey_get_scan();
    return value;
}

void tkey_exit(void)
{
    printf("%s\n",__func__);
    for(u8 i = 0; i < TKEY_MUL_SCAN_NUM; i++) {
        tkey_gpio_exit(tkey_array[i]);
    }
    tkey_off();
}

void tkey_init(void)
{
    printf("%s\n",__func__);
#if TKEY_BUF_DOWN_EN      //若复位，要导出bin看flash使用情况
    u8 tkbuf[0x200];
    //u16 *ptr = (u16 *)&tkbuf;
    for(u8 i = 0; i< 4; i++) {
        os_spiflash_read(tkbuf,0x20000 + i * 0x200,0x200);
        my_print_r(&tkbuf,0x200);
        for(u16 j = 0; j < 512; j++){
            //my_spi_putc(tkbuf[j]);
            //my_printf(" 0x%x ",tkbuf[j]);
        }

//2byte打印，方便数据分析
//        for(u16 j = 0; j < 512/2; j++){
//            my_spi_putc(ptr[j]);
//            //my_printf(" 0x%x ",tkbuf[j]);
//        }
    }
    spiflash_erase(0x20000);  //打印完就擦除
#endif // TKEY_BUF_DOWN_EN

//开始扫描时，先关闭ClassD，避免触控扫描不灵敏
#if TKEY_CLASSD_FIX
    DI_EN_OUT(0);
#endif

    for(u8 i = 0; i < TKEY_MUL_SCAN_NUM; i++) {
        tkey_gpio_config(tkey_array[i]);
    }
    memset(tkey_adj_val,0,sizeof(tkey_adj_val));
    tk_init_typedef tk_cfg_init;
    tk_cfg_init.tkcd_pr = TKEY_TKCD_PR;
    bsp_tkey_init(&tk_cfg_init);
    WDT_DIS();
}

#endif // TKEY_MUL_SCAN_EN
