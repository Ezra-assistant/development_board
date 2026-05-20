#include "include.h"

sys_cb_t sys_cb AT(.buf.bsp.sys_cb);

/* 自定义休眠相关变量 */
#if 1
AT(.com_text.sleep_str)
const char sleep_count_str[] = "Count to 10 and enter sleep mode: %d\n";
AT(.com_text.sleep_str)
const char sleep_msg_str[] = "system sleepping...\n";

#endif


/* 普通IO 按键扫描变量 与宏 */
#if 1
#define KEY1_PRESS_SHORT 0x0081
#define KEY1_PRESS_LONG  0x0E81

#define KEY2_PRESS_SHORT 0x0082
#define KEY2_PRESS_LONG  0x0E82
#endif


/* 普通IO 按键扫描 函数 */
#if 1
AT(.com_text.normal_key)
void normal_key_scan(void) {
    static u16 key1_press_count = 0;
    static u16 key2_press_count = 0;
    static u8 last_key1_state = 0;
    static u8 last_key2_state = 0;
    static u8 key1_processed = 0;  // 防止长按时重复触发
    static u8 key2_processed = 0;  // 防止长按时重复触发

    u16 key1_return = 0;
    u16 key2_return = 0;
    u8 current_key1_state = (GPIOA & BIT(2)) == 0;  // 按下为1，松开为0
    u8 current_key2_state = (GPIOA & BIT(3)) == 0;  // 按下为1，松开为0

    // 下降沿检测（按键按下）
    if (current_key1_state == 1 && last_key1_state == 0) {
        key1_press_count = 0;
        key1_processed = 0;
    }
    if (current_key2_state == 1 && last_key2_state == 0) {
        key2_press_count = 0;
        key2_processed = 0;
    }

    // 按键按住时累加（取消注释）
    if (current_key1_state == 1) {
        key1_press_count++;

        // 长按触发（到达200次且未处理）
        if (key1_press_count >= 200 && !key1_processed) {
            key1_return = KEY1_PRESS_LONG;
            key1_processed = 1;  // 防止继续触发
        }
    }
    if (current_key2_state == 1) {
        key2_press_count++;

        // 长按触发（到达200次且未处理）
        if (key2_press_count >= 200 && !key2_processed) {
            key2_return = KEY2_PRESS_LONG;
            key2_processed = 1;  // 防止继续触发
        }
    }

    // 上升沿检测（按键松开）
    if (current_key1_state == 0 && last_key1_state == 1) {
        // 如果还没触发过长按，且满足短按条件
        if (!key1_processed && key1_press_count >= 6) {
            key1_return = KEY1_PRESS_SHORT;
        }
        key1_press_count = 0;  // 复位计数
        key1_processed = 0;
    }
    if (current_key2_state == 0 && last_key2_state == 1) {
        // 如果还没触发过长按，且满足短按条件
        if (!key2_processed && key2_press_count >= 6) {
            key2_return = KEY2_PRESS_SHORT;
        }
        key2_press_count = 0;  // 复位计数
        key2_processed = 0;
    }

    last_key1_state = current_key1_state;
    last_key2_state = current_key2_state;

    // 发送消息
    if (key1_return != 0) {
        msg_enqueue(key1_return);
        key1_return = 0;
    }
    if (key2_return != 0) {
        msg_enqueue(key2_return);
        key2_return = 0;
    }
}
#endif


/* 正式代码 矩阵扫描KEY 变量与宏 */
#if 1
/*
 * 矩阵键盘配置
 * ================================================
 * 引脚映射:                   按键布局:
 *   行线:   列线:                  PA4  PA5  PA6
 *   PA0=y1  PA4=x1            PA0| 1    2    3
 *   PA1=y2  PA5=x2            PA1| 4    5    6
 *   PA2=y3  PA6=x3            PA2| 7    8    9
 *   PA3=y4                    PA3| *    0    #
 *
 * 按键编码:
 *   0:0x0A00   1:0x0A01   2:0x0A02   3:0x0A03
 *   4:0x0A04   5:0x0A05   6:0x0A06   7:0x0A07
 *   8:0x0A08   9:0x0A09   *:0x0A0A   #:0x0A0B
 * ================================================
 */

/* 矩阵 相关 宏定义 */
// 行列定义
#define ROWS_KEY 4  // 行数 (y1~y4: PA0~PA3)
#define COLS_KEY 3  // 列数 (x1~x3: PA4~PA6)


/* 矩阵 相关 结构体 */
// 按键状态结构体
typedef struct {
    u16 last_state;      // 上一次状态 (0=抬起, 1=按下)
    u16 current_state;      // 上一次状态 (0=抬起, 1=按下)
    u16 press_count;     // 按下计数
    u8 processed;        // 是否已处理
} KeyState;

/* 矩阵 相关 数组 */
// 使用二维数组存储所有按键状态 [行][列]
static KeyState key_states[ROWS_KEY][COLS_KEY] = {0};

// 按键编码映射表 [行][列]
// const u16 key_code_map[ROWS_KEY][COLS_KEY] = {       /* *重点*：const 默认不会放在 com区，所有中断的变量不能用 */
u16 key_code_map[ROWS_KEY][COLS_KEY] = {
    {0x0A01, 0x0A02, 0x0A03},  // 行0: 1, 2, 3
    {0x0A04, 0x0A05, 0x0A06},  // 行1: 4, 5, 6
    {0x0A07, 0x0A08, 0x0A09},  // 行2: 7, 8, 9
    {0x0A0A, 0x0A00, 0x0A0B}   // 行3: *, 0, #
};

// 当前 row 各 col 状态记录
u16 current_xs_key_state[COLS_KEY] = {1};


/* 矩阵 相关 全局变量 */
u8 gnd_count_flag = 0;
u8 circle_count_flag = 0;
u16 key_return = 0;
KeyState *state = 0;

// u16 gpioa0_3_state = 0;
u16 gpioa4_7_state = 0;
#endif

/* 正式代码 矩阵扫描KEY 函数 */
#if 1
/* 矩阵 每 5ms 循环 切换输出 函数 */
AT(.com_text.matrix_key)
void circle_change_output_to_GND(u8 count_flag) {
    GPIOADE |= BIT(count_flag);
    GPIOADIR &= ~BIT(count_flag);
    GPIOA &= ~BIT(count_flag++);

    if (count_flag > 3) count_flag = 0;
    GPIOADE &= ~BIT(count_flag);
    GPIOADIR |= BIT(count_flag++);
    // GPIOA |=  BIT(count_flag++);

    if (count_flag > 3) count_flag = 0;
    GPIOADE &= ~BIT(count_flag);
    GPIOADIR |= BIT(count_flag++);

    if (count_flag > 3) count_flag = 0;
    GPIOADE &= ~BIT(count_flag);
    GPIOADIR |= BIT(count_flag++);
}

AT(.com_text.matrix_key)
void matrix_key_scan() {

    for (int i = 0; i < ROWS_KEY; i++) {

        // 1.先设置 要扫描的 输出 row
        circle_change_output_to_GND(i);
        delay_us(10);

        // 2.获取当前 row 各列的 端口值
        for (int j = 0; j < COLS_KEY; j++) {
            state = &key_states[i][j];

            gpioa4_7_state = (GPIOA >> (4 + j)) & 0x01;
            state->current_state = (gpioa4_7_state == 0);

            // 1.如果 某个 按键 是没有被执行的：第一种状态，空闲；第二种状态，有人按下了
            if (!state->processed) {
                // 空闲状态：过滤1：如果当前是按下的状态，才能继续往下走，如果是抬手状态则跳出
                // 没执行但是松手了
                if (!state->current_state) {
                    state->last_state = state->current_state;
                    continue;
                }

                // 如果不是空闲状态
                // 过滤2：如果是第一次按下（下降沿检测）
                if (!state->last_state) {   // 如果是 1 就初始化
                    state->press_count = 0;
                    state->last_state = state->current_state;
                }

                state->press_count++;

                // 连续扫描到两次都是这个按钮，则触发（20ms ~ 40ms）
                if (state->press_count == 6) {
                    key_return = key_code_map[i][j];

                    key_states[i][j].processed = 1;
                }
            }
            // 2.如果 某个 按键 被执行的：说明按钮一定被按下过，需要确定本次按钮的状态：
                // 2.1 如果本次按钮的状态是抬起的：那么可以把被执行的状态清空，计数器情况，等待下一次执行；同时需要把指针指向下一个地方
                // 2.2 如果本次按钮的状态依旧是按下的：直接把指针指向下一个地方
            else if (state->processed && !state->current_state) {
                // 2.1 抬手了
                if (!state->current_state && state->last_state) {
                    state->processed = 0;
                    state->press_count = 0;
                    state->last_state = state->current_state;
                }
            }
            // 2.2 没抬手
            else if (state->processed && state->current_state) {
                continue;
            }

            /* =====消息发送===== */
            if (key_return != NO_MSG) {
                msg_enqueue(key_return);
                key_return = 0;
            }
        }
    }
}
#endif


/* 正式代码 矩阵扫描LED 变量与宏 */
#if 0
#define ROWS_LED 3
#define COLS_LED 3
#define IO_OFFSET 4

u16 led_states = 0;
u8 matrix_array_index = 0;
u8 led_row_count_flag = 0;

#endif


/* 正式代码 矩阵扫描LED 函数 */
#if 0

AT(.com_text.matrix_led)
void circle_change_output_to_gnd_led(u8 count_flag) {
    GPIOBDE |= BIT(count_flag);
    GPIOBDIR &= ~BIT(count_flag);
    GPIOB &= ~BIT(count_flag++);

    if (count_flag > 2) count_flag = 0;
    GPIOBDE &= ~BIT(count_flag);
    GPIOBDIR |= BIT(count_flag++);

    if (count_flag > 2) count_flag = 0;
    GPIOBDE &= ~BIT(count_flag);
    GPIOBDIR |= BIT(count_flag++);
}

AT(.com_text.matrix_led)
void matrix_leds_scan(void) {

    u16 msg = msg_dequeue();

    // 1.如果是在这个范围，更新 LED 寄存器（自定义的状态寄存器）
    if ((msg <= 0x0B0B) && (msg >= 0x0B00)) {
        matrix_array_index = msg & 0x0F;
        led_states = 0;
        led_states = leds_states[matrix_array_index];
    }
    // 休眠时，把 LED 全部关掉
    else if (msg == 0x0B0C) {
        led_states = 0;

        // 0x1111 进入休眠
        msg_enqueue(0x1111);
        return;
    }
    // 如果不是在这个范围就放回去
    else {
        msg_enqueue(msg);
    }

    //
    circle_change_output_to_gnd_led(led_row_count_flag);

    for (int i = 0; i < COLS_LED; i++) {
        // 如果该位寄存器是 1，则点亮，反之不点亮
        if (led_states & BIT(led_row_count_flag * COLS_LED + i)) {
            GPIOB |= BIT(i + IO_OFFSET);
        }
        else {
            GPIOB &= ~BIT(i + IO_OFFSET);
        }
    }

    led_row_count_flag++;
    if (led_row_count_flag > 2) led_row_count_flag = 0;


/* 错误用法 */
#if 0
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            // 如果该位寄存器是 1，则点亮，反之不点亮
            if ((led_states == 0x1DF) || (led_states == 0x1D7) || (led_states == 0x15F)) {
                if (led_states & BIT(i * ROWS_LED + j)) {
                    open_led(i, j);
                    delay_us(10);
                }
            } else {
                if (led_states & BIT(i * ROWS_LED + j)) {
                    open_led(i, j);
                    delay_us(10);
                }
            }

        }
    }

    /* 关闭掉，防止最后一个灯过亮 */
    GPIOB |= BIT(0);
    GPIOB |= BIT(1);
    GPIOB |= BIT(3);
    GPIOB &= ~BIT(4);
    GPIOB &= ~BIT(5);
    GPIOB &= ~BIT(6);

#endif
}
#endif



/* 测试打印 */
AT(.com_rodata.bsp.test)
const char str1[] = "\n\nseg7_state = 0x%02X\n";
AT(.com_rodata.bsp.test)
const char str2[] = "1";
AT(.com_rodata.bsp.test)
const char str3[] = "0";


/* 测试代码 矩阵扫描数码管 变量与宏 */
#if 1
#define NUM_STATES 21
#define GND_SEG_OFFSET 11

// 重新定义GPIO地址宏
#define IS_GPIOA  1
#define IS_GPIOB  2





int8_t tens_index = 0;
int8_t units_index = 0;

u8 gnd_flag = 0;

// 定义段码对应的GPIO端口和引脚
typedef struct {
    uint32_t is_portx;  // 存储GPIO端口基地址
    uint16_t pin;        // 引脚号（BIT0、BIT8等）
} SegPinMap;

// 建立 A~DP 的映射表（索引0~7对应A~DP）
AT(.com_text.rodata)
const SegPinMap seg_pin[8] = {
    {IS_GPIOB, 0},  // A
    {IS_GPIOA, 8},  // B
    {IS_GPIOB, 1},  // C
    {IS_GPIOB, 2},  // D
    {IS_GPIOB, 4},  // E
    {IS_GPIOB, 5},  // F
    {IS_GPIOB, 6},  // G
    {IS_GPIOB, 7}  // DP
};

// 全局数组变量，使用 8bit，表示 数码管 第一个位 各个段依次(ABCDEFG、DP)之间的关系，另一个8bit同理
// 20种状态，比如：0 0. 1 1. 9 9.
AT(.com_text.rodata)
const u8 seg7_states[NUM_STATES] = {
    0x3F,  // 0
    0x06,  // 1
    0x5B,  // 2
    0x4F,  // 3
    0x66,  // 4
    0x6D,  // 5
    0x7D,  // 6
    0x07,  // 7
    0x7F,  // 8
    0x6F,  // 9
    0xBF,  // 0.
    0x86,  // 1.
    0xDB,  // 2.
    0xCF,  // 3.
    0xE6,  // 4.
    0xED,  // 5.
    0xFD,  // 6.
    0x87,  // 7.
    0xFF,  // 8.
    0xEF,  // 9.
    0x00   // 清空 用于关闭数码管（如休眠）
};
#endif


/* 测试代码 矩阵扫描数码管 函数 */
#if 1
void circle_change_output_to_gnd_seg7(u8 pin_num);
void set_io_with_seg7_state(u8 seg7_state);
void seg7_num_add(void);
void seg7_num_subtract(void);

AT(.com_text.matrix_seg)
void seg7_scan(void) {
    // 判断代码是什么，如果有最新的代码传进来，则需要改变一些参数
    u16 msg = msg_dequeue();

    // 1.如果是在这个范围，更新 LED 寄存器（自定义的状态寄存器）
    if ((msg <= 0x0B09) && (msg >= 0x0B00)) {
        tens_index = 0;
        units_index = msg & 0x0F;

        /* 发送给 main 给 播放处理函数播放 */
        play_num_msg = (0x0C << 8) | (tens_index << 4) | units_index;
    }
    else if (msg == 0x0B0A) { // 数字++
        seg7_num_add();
        /* 发送给 main 给 播放处理函数播放 */
        play_num_msg = (0x0C << 8) | (tens_index << 4) | units_index;
    }
    else if (msg == 0x0B0B) { // 数字--
        seg7_num_subtract();
        /* 发送给 main 给 播放处理函数播放 */
        play_num_msg = (0x0C << 8) | (tens_index << 4) | units_index;
    }
    // 休眠时，把 seg 全部关掉
    else if (msg == 0x0B0C) {
        tens_index = 20;
        units_index = 20;

        // 0x1111 进入休眠
        msg_enqueue(0x1111);
    }
    // 如果 msg 用不上就放回去
    else {
        msg_enqueue(msg);
    }


    // 改变 GND 持有者
    circle_change_output_to_gnd_seg7(GND_SEG_OFFSET + gnd_flag);
    delay_us(10);

    if (gnd_flag == 0) {
        u8 seg7_state = seg7_states[tens_index];
        set_io_with_seg7_state(seg7_state);
    }
    if (gnd_flag == 1) {
        u8 seg7_state = seg7_states[units_index];
        set_io_with_seg7_state(seg7_state);
    }

    // GND 持有者往前一步
    gnd_flag++;
    if (gnd_flag > 2) gnd_flag = 0;
}

AT(.com_text.matrix_seg)
// pin_num: 11 / 12
void circle_change_output_to_gnd_seg7(u8 pin_num) {
    // // 输出 GND
    // GPIOADE |= BIT(pin_num);
    // GPIOADIR &= ~BIT(pin_num);
    // GPIOA &= ~BIT(pin_num++);

    // if (pin_num > 12) pin_num = 11;
    // // 高阻态
    // GPIOADE &= ~BIT(pin_num);
    // GPIOADIR |= BIT(pin_num);

    if (pin_num == 11) {
        GPIOADE |= BIT(11);
        GPIOADIR &= ~BIT(11);
        GPIOA &= ~BIT(11);

        GPIOADE &= ~BIT(12);
        GPIOADIR |= BIT(12);
    }
    if (pin_num == 12) {
        GPIOADE |= BIT(12);
        GPIOADIR &= ~BIT(12);
        GPIOA &= ~BIT(12);

        GPIOADE &= ~BIT(11);
        GPIOADIR |= BIT(11);
    }
}

AT(.com_text.matrix_seg)
void set_io_with_seg7_state(u8 seg7_state) {
    for (int i = 0; i < 8; i++) {
        if (seg7_state & (1 << i)) { // 该bit为1
            if (seg_pin[i].is_portx == IS_GPIOA) {
                GPIOADE |= BIT(seg_pin[i].pin);
                GPIOADIR &= ~BIT(seg_pin[i].pin);
                GPIOA |= BIT(seg_pin[i].pin);
            }
            if (seg_pin[i].is_portx == IS_GPIOB) {
                GPIOBDE |= BIT(seg_pin[i].pin);
                GPIOBDIR &= ~BIT(seg_pin[i].pin);
                GPIOB |= BIT(seg_pin[i].pin);
            }
        }
        else {
            if (seg_pin[i].is_portx == IS_GPIOA) {
                GPIOA &= ~BIT(seg_pin[i].pin);
                GPIOADE &= ~BIT(seg_pin[i].pin);
                GPIOADIR |= BIT(seg_pin[i].pin);
            }
            if (seg_pin[i].is_portx == IS_GPIOB) {
                GPIOB &= ~BIT(seg_pin[i].pin);
                GPIOBDE &= ~BIT(seg_pin[i].pin);
                GPIOBDIR |= BIT(seg_pin[i].pin);
            }
        }
    }
}

AT(.com_text.matrix_seg)
void seg7_num_add(void) {
    units_index++;
    if (units_index >= 10) {
        units_index = 0;
        tens_index++;
        if (tens_index >= 10) {
            tens_index = 0;
        }
    }
}

AT(.com_text.matrix_seg)
void seg7_num_subtract(void) {
    units_index--;
    if (units_index < 0) {
        units_index = 9;
        tens_index--;
        if (tens_index < 0) {
            tens_index = 9;
        }
    }
}

#endif















void freqdet_init(void);

//timer tick interrupt(5ms)
AT(.com_text.timer)
void usr_tmr5ms_isr(void)
{

    /* 普通IO 按键扫描函数 */
    // normal_key_scan();

    /* 正式代码 矩阵扫描KEY函数 */
    matrix_key_scan();

    /* 数码管 扫描 函数 */
    seg7_scan();       


    





    sys_cb.tmr5ms_cnt++;
#if !USER_KEY_KNOB2_EN && !MATRIX_XY_KEY_SCAN_SEL
    bsp_key_scan();
#endif

#if MUSIC_SDCARD_EN
    sd_detect();
#endif // MUSIC_SDCARD_EN

#if MUSIC_SDCARD1_EN
    sd1_detect();
#endif // MUSIC_SDCARD1_EN

#if RGB_SERIAL_EN
    // spi1_led_data_send_proc();
    sys_cb.rgb_update_flag = 1;
#endif

#if DAC_SOFT_DNR
    if ((sys_cb.tmr5ms_cnt % 2) == 0) {    //10ms检测dac dnr
        dac_dnr_detect();
    }
#endif

    //500ms timer process
    if ((sys_cb.tmr5ms_cnt % 100) == 0) {
        sys_cb.cm_times++;
    }

    //1s timer process
    if ((sys_cb.tmr5ms_cnt % 200) == 0) {
        printf(sleep_count_str, sleep_count_flag + 1);
        sleep_count_flag++;

        msg_enqueue(MSG_SYS_1S);
        sys_cb.lpwr_warning_cnt++;

        #if (DAC_CLASSD_VDET && VBAT_DETECT_EN)
            dac_classd_dec_proc(sys_cb.vbat);
        #endif
    }

    // 5s timer process 后面改成 10s
    if (sleep_count_flag == 10) {
        sleep_count_flag = 0;

        // 计时 10s，发送数据给 led_scan，让他把灯关掉
        // msg_enqueue(0x0B0C);
    }
}

//timer tick interrupt(1ms)
AT(.com_text.timer)
void usr_tmr1ms_isr(void)
{

    /* 矩阵 LED扫描 函数 */
    // matrix_leds_scan();

                                                                                                     


    sys_cb.tmr1ms_cnt++;

    if ((sys_cb.tmr1ms_cnt % 5) == 0) {
        usr_tmr5ms_isr();
    }

#if LED_DISP_EN
    if ((sys_cb.tmr1ms_cnt % 50) == 0) {
        led_scan();
    }
#endif // LED_DISP_EN

#if !USER_KEY_KNOB2_EN && MATRIX_XY_KEY_SCAN_SEL
    bsp_key_scan();
#endif

    //100ms timer process
    if ((sys_cb.tmr1ms_cnt % 100) == 0) {
        sys_cb.tmr1ms_cnt = 0;
        if (sys_cb.lpwr_cnt > 0) {
            sys_cb.lpwr_cnt++;
        }
#if MIDI_METRO_EN
        metro_tick_up();
#endif
    }

#if TKEY_SCAN_SWITCH_EN
    if ((sys_cb.tmr1ms_cnt % TKEY_SCAN_TIME) == 0) {
    	tkey_cir_scan_en();
    }
#endif
}

AT(.text.bsp.sys.init)
void power_on_off_init(void)
{
#if SOFT_POWER_ON_OFF
    //避免ADkey睡眠唤醒后状态异常
    // if(POWER_ON_FALL_IO > IO_PA15){
    //     GPIOBFEN &= ~BIT(POWER_ON_FALL_IO-1-IO_PA15);
    //     GPIOBDE  |=  BIT(POWER_ON_FALL_IO-1-IO_PA15);
    //     GPIOBDIR |=  BIT(POWER_ON_FALL_IO-1-IO_PA15);
    // }else{
    //     GPIOAFEN &= ~BIT(POWER_ON_FALL_IO-1);
    //     GPIOADE  |=  BIT(POWER_ON_FALL_IO-1);
    //     GPIOADIR |=  BIT(POWER_ON_FALL_IO-1);
    // }
#endif
}

AT(.rodata.vol)
const u8 maxvol_tbl[4] = {16, 32, 50};

//开user timer前初始化的内容
AT(.text.bsp.sys.init)
static void bsp_var_init(void)
{
    memset(&sys_cb, 0, sizeof(sys_cb));
    sys_cb.ms_ticks = tick_get();
    sys_cb.vol_max = maxvol_tbl[xcfg_cb.vol_max];
    if (SYS_INIT_VOLUME > sys_cb.vol_max) {
        SYS_INIT_VOLUME = sys_cb.vol_max;
    }
    if (WARNING_VOLUME > sys_cb.vol_max) {
        WARNING_VOLUME = sys_cb.vol_max;
    }

    // key_var_init();
    plugin_var_init();

    msg_queue_init();

    dac_cb_init();
#if MUSIC_SDCARD_EN || MUSIC_SDCARD1_EN
    dev_init(is_sd_support());
#endif

#if MUSIC_SDCARD_EN
//    if((xcfg_cb.sddet_iosel == IO_MUX_SDCLK) || (xcfg_cb.sddet_iosel == IO_MUX_SDCMD)) {
//        dev_delay_offline_times(DEV_SDCARD, 3); //复用时, 加快拔出检测. 这里拔出检测为3次.
//    }
#endif

#if MUSIC_SDCARD1_EN
//    if((xcfg_cb.sd1det_iosel == IO_MUX_SDCLK) || (xcfg_cb.sd1det_iosel == IO_MUX_SDCMD)) {
//        dev_delay_offline_times(DEV_SDCARD1, 3); //复用时, 加快拔出检测. 这里拔出检测为3次.
//    }
#endif

#if (MUSIC_UDISK_EN || MUSIC_SDCARD_EN || MUSIC_SDCARD1_EN)
    //fs_var_init();
#endif
}

AT(.text.bsp.sys.init)
static void bsp_io_init(void)
{
    GPIOADE = 0;
    GPIOBDE = 0;
    GPIOGDE = 0x3F;             //MCP FLASH
    uart0_mapping_sel();        //调试UART IO选择或关闭
    mclr_reset_init();

#if MUSIC_SDCARD_EN
    SD_DETECT_INIT();
#endif // MUSIC_SDCARD_EN

#if MUSIC_SDCARD1_EN
    SD1_DETECT_INIT();
#endif // MUSIC_SDCARD1_EN
}

AT(.text.bsp.sys.init)
void bsp_update_init(void)
{
    /// config
    if (!xcfg_init(&xcfg_cb, sizeof(xcfg_cb))) {           //获取配置参数
        printf("xcfg init error\n");
    }

    // io init
    bsp_io_init();

    // var init
    bsp_var_init();
    sys_cb.lang_id = 0;

    // peripheral init
    rtc_init();
    param_init(sys_cb.rtc_first_pwron);

    plugin_init();
    sys_set_tmr_enable(1);

    adpll_init(SYS_CLK_SEL);
#if DAC_EN
    dac_init();
#endif
#if WARNING_UPDATE_DONE
    mp3_res_play(RES_BUF_UPDATE_DONE_MP3, RES_LEN_UPDATE_DONE_MP3, 0);
#endif
}

AT(.text.bsp.sys.init)
void bsp_sys_init(void)
{
    /// config
    if (!xcfg_init(&xcfg_cb, sizeof(xcfg_cb))) {           //获取配置参数
        printf("xcfg init error\n");
    }

    // io init
    bsp_io_init();

    // var init
    bsp_var_init();

    // power init
    pmu_init(0);

    // clock init
    adpll_init(SYS_CLK_SEL);
    set_sys_clk(SYS_CLK_SEL);
    // dbg_clk_out(9, 10);

    // peripheral init
    rtc_init();
    param_init(sys_cb.rtc_first_pwron);
    plugin_init();
    power_on_off_init();        //开关机io初始化

#if IRRX_SW_EN
    irrx_sw_init();
#endif // IRRX_SW_EN

    led_init();
    key_init();

    /// enable user timer for display & dac
    sys_set_tmr_enable(1);

#if LED_DISP_EN
    led_power_up();
#endif // LED_DISP_EN

#if DAC_EN
    dac_init();
#endif

    bsp_change_volume(sys_cb.vol);

#if DAC_DRC_EN
    drc_v3_init((u8 *)RES_BUF_DRC_DAC_MUSIC_DRC, RES_LEN_DRC_DAC_MUSIC_DRC);
#endif

#if EX_SPIFLASH_SUPPORT
    exspiflash_init();
#endif

#if SUPPORT_SDCARD0_MUSIC
    sdcard_music_init();
#endif // SUPPORT_SDCARD0_MUSIC

#if SPI_DUMP_EN
    my_spi_init();
#endif
#if HUART_DEUMP_EN
    huart_dump_init();
#endif

#if HUART_EN
    bsp_huart_init();
#endif

#if USER_UART0_EN
    bsp_uart_init();
#endif

#if MIDI_UART0_EN
    midi_uart_init();
#endif

#if TKEY_MUL_SCAN_EN
    tkey_init();
#endif // TKEY_MUL_SCAN_EN

#if MUSIC_SDCARD_EN
    sd_soft_detect_poweron_check();
#endif // MUSIC_SDCARD_EN

#if SPI1_AUDIO_EN
    spi1_audio_init();
#endif

#if DAC_SOFT_EQ_EN
    dac_set_eq_by_res(&RES_BUF_EQ_DAC_16K_EQ, &RES_LEN_EQ_DAC_16K_EQ);
    // dac_set_eq_by_res(&RES_BUF_EQ_DAC_48K_EQ, &RES_LEN_EQ_DAC_48K_EQ);
#endif

#if WARNING_TONE_EN || MIDI_DEC_BK_EN
    music_res_init();
#endif

#if WARNING_POWER_ON && !SPI1_AUDIO_TEST_EN
    // mp3_res_play(RES_BUF_EN_POWERON_MP3, RES_LEN_EN_POWERON_MP3, 0);
    vmp3_res_play(RES_BUF_EN_POWERON_VMP3, RES_LEN_EN_POWERON_VMP3, 0);
#endif

#if PWM_HW_EN
    pwm_hw_cfg_init();
#endif // PWM_HW_EN

#if PWM_TMR2_EN
    tmr2pwm_cfg_init();
#endif // PWM_TMR2_EN

#if FREQ_DET_EN
    freqdet_init();
#endif // FREQ_DET_EN

#if TMR2_US_EN
    timer2_init();
#endif  //TMR2_US_EN

#if RGB_SERIAL_EN
    spi1_led_init();
    spi1_test_set();
#endif

#if LCD_DISPLAY_EN
    lcd_display_init();
#endif

#if MIDI_EN
    midi_init();
    #if MIDI_REC_EN
        midi_rec_init();
    #endif
    #if MIDI_METRO_EN
        midi_metro_init();
    #endif
#endif

#if UART_S_UPDATE
    uart_upd_init(UART_UPD_PORT_SEL,UART_UPD_BAUD,SYS_CLK_SEL);
    uart_upd_isr_init();
#endif // UART_S_UPDATE
}
