#include "include.h"

func_cb_t func_cb AT(.buf.func_cb);

void spi1_led_auto_process(void);
void dumphuart_io_test(void);
#if SYS_SLEEP_TIME
static u8 pwroff_flag = 0;
#endif

#if UART_S_UPDATE
bool deal_update_uart_msg(void);
void uart_s_update(void);
#endif // UART_S_UPDATE

AT(.text.func.process)
void print_info(void)
{
    static u32 ticks = 0;
    if (tick_check_expire(ticks,1000)) {
        ticks = tick_get();
        my_printf(".");
        //dumphuart_io_test();
    }
}






















/* =====普通KEY 相关参数===== */
#if 1
/* KEY 宏定义 */
#define BASE_BUF_ADDR 0x11000078
#define BASE_LEN_ADDR 0x1100007c
#define OFFSET        0x20

/* KEY 全局变量 */
u32 buf_addr = 0;
u32 len_addr = 0;
u16 count_tens_num = 0;
u16 count_units_num = 0;

static u8 num1_count = 0;
static u8 num2_count = 0;

u8 last_num1_count = 0;

u8 tens_num = 0;
u8 ten_num = 0;
u8 units_num = 0;

u8 play_single_flag = 0;
u8 play_cycle_flag = 0;
#endif





/* =====唤醒与休眠===== */
#if 1

/* =====唤醒与休眠 相关参数===== */
#if 1

#define SLEEP_COUNT_TIMES 5
u8 sleep_count_flag = 0;

#endif
/* =====唤醒与休眠 相关函数===== */
#if 1

/* 唤醒 标志位重置 */
void reload_flag(void) {
    u16 msg = msg_dequeue();

    /* 唤醒后 非空闲时 清除标志位 */
    if (msg == 0x0081 || msg == 0x0082 || ((msg >= 0x0A00) && (msg <= 0x0A0B)) || ((msg >= 0x0B00) && (msg <= 0x0B0B)) || (music_layer_sta_get(MSC_LAYER0) == LAYER_PLAYING)) {
        sleep_count_flag = 0;
    }

    // 用完就放回去
    msg_enqueue(msg);
}

/* 系统休眠处理函数 */
void system_sleep_process() {
    u16 msg = msg_dequeue();

    // 如果不是要进入休眠的信号 0x1111，那么重置完标志为就退出
    if (msg != 0x1111) {

        // 重置标志位
        reload_flag();

        msg_enqueue(msg);
        return;
    }


    // 如果 msg 是信号 0x1111 则进入休眠
    if (msg == 0x1111) {
        printf("system sleepping....\n");
        sfunc_pwroff();
    }

}
#endif

#endif

/* =====普通按键播放===== */
#if 1

/* =====普通按键播放 循环/累加音频 相关函数===== */
#if 1
void get_play_num(u8 num_count) {
    /* 如果是 10~19 这些数字，只播放十和个位 */
    if (num_count >= 10 && num_count < 20) {
        ten_num = 1;
        units_num = num_count % 10;
    } else {
        tens_num  = num_count / 10;
        units_num = num_count % 10;
    }
}

void switch_to_play (u8 num) {
    switch (num)
    {
        case 1:
            vmp3_song_play(RES_BUF_EN_1_VMP3, RES_LEN_EN_1_VMP3, 0);
            break;
        case 2:
            vmp3_song_play(RES_BUF_EN_2_VMP3, RES_LEN_EN_2_VMP3, 0);
            break;
        case 3:
            vmp3_song_play(RES_BUF_EN_3_VMP3, RES_LEN_EN_3_VMP3, 0);
            break;
        case 4:
            vmp3_song_play(RES_BUF_EN_4_VMP3, RES_LEN_EN_4_VMP3, 0);
            break;
        case 5:
            vmp3_song_play(RES_BUF_EN_5_VMP3, RES_LEN_EN_5_VMP3, 0);
            break;
        case 6:
            vmp3_song_play(RES_BUF_EN_6_VMP3, RES_LEN_EN_6_VMP3, 0);
            break;
        case 7:
            vmp3_song_play(RES_BUF_EN_7_VMP3, RES_LEN_EN_7_VMP3, 0);
            break;
        case 8:
            vmp3_song_play(RES_BUF_EN_8_VMP3, RES_LEN_EN_8_VMP3, 0);
            break;
        case 9:
            vmp3_song_play(RES_BUF_EN_9_VMP3, RES_LEN_EN_9_VMP3, 0);
            break;
        default:
            break;
    }
}

void play_num(void) {
    if (tens_num) {

        ten_num = 1;

        // 选择并播放
        switch_to_play(tens_num);

        tens_num = 0;

    } else if (ten_num) {

        vmp3_song_play(RES_BUF_EN_910_VMP3, RES_LEN_EN_910_VMP3, 0);
        ten_num = 0;
        if (units_num == 0) {
            play_single_flag = 0;
            num2_count++;
        }

    } else if (units_num) {
        // 选择并播放
        switch_to_play(units_num);

        units_num = 0;
        play_single_flag = 0;
        num2_count++;
    }
}

u8 check_and_play(u8 *num_count) {
    // 检查是否有音乐正在播放
    layer_sta_e sta = music_layer_sta_get(MSC_LAYER0);
    if (sta == LAYER_PLAYING) {
        return 0;
    }
    // 没有音乐播放
    else {
        // 暂停音乐
        music_layer_sta_set(MSC_LAYER0, LAYER_STOP);

        if (!(tens_num || ten_num || units_num)) {
            // 获取个位、十位数字
            get_play_num(*num_count);
            // num1_count++; // 播放完再加，这样按钮按快没有作用
        }

        if ((*num_count) == 0) {
            vmp3_song_play(RES_BUF_EN_0_VMP3, RES_LEN_EN_0_VMP3, 0);
        }

        // 播放
        play_num();
        return 1;
    }
}

void play_song_by_single(void) {
    // if (旧count != 新count) 说明count变了，count变了更新count
    if (last_num1_count != num1_count) {
        last_num1_count = num1_count;
        tens_num = 0;
        ten_num = 0;
        units_num = 0;

        // 停掉音乐
        music_layer_sta_set(MSC_LAYER0, LAYER_STOP);
        return;
    }

    check_and_play(&num1_count);
}

void play_song_by_cycle(void) {
    if (num2_count > 99) num2_count = 1;
    check_and_play(&num2_count);
}
#endif

/* 普通按键播放处理函数 */
#if 0
void key_play_process() {

    u16 msg = msg_dequeue();
    // 不是我这里能处理的msg直接送回去
    if (!((msg == 0x0081) || (msg == 0x0081) || (msg == 0x0E81) || (msg == 0x0E82))) {
        msg_enqueue(msg);
        return;
    }

    /* 短按 按键一 消息处理 */
    if (msg == 0x0081) {
        printf("Dequeued msg: 0x%04x\n", msg);
        msg = 0;

        // 清除 按键二 相关参数
        play_cycle_flag = 0;
        num2_count = 1;

        // 设置 按键一 相关参数
        play_single_flag = 1;

        num1_count++; // 按键按完立刻++，
        if (num1_count > 99) {
            num1_count = 1;
        }
    }

    /* 短按 按键二 消息处理 */
    if (msg == 0x0082) {
        printf("Dequeued msg: 0x%04x\n", msg);
        msg = 0;

        // 清除 按键一 相关参数
        play_single_flag = 0;

        // 设置 按键二 相关参数
        play_cycle_flag = 1;
        num2_count = 1;

        // 停掉音乐
        music_layer_sta_set(MSC_LAYER0, LAYER_STOP);
    }

    /* 长按 按键一 消息处理 */
    if (msg == 0x0E81) {
        msg = 0;
        printf("Long holds key1\n");
    }

    /* 长按 按键二 消息处理 */
    if (msg == 0x0E82) {
        msg = 0;
        printf("Long holds key2\n");
    }

    /* 单次播放 */
    if (play_single_flag) {
        play_song_by_single();
    }

    /* 循环播放 */
    if (play_cycle_flag) {
        play_song_by_cycle();
    }
}
#endif


#endif








/* =====矩阵按键播放 指定数字音频===== */
#if 1

/* 全局变量 */
u16 play_num_msg = 0;

void matrix_key_play_process(void) {

    // 播放次数
    static u8 play_times = 0;
    static u8 num = 0xFF;

    /* 获取播放次数 */
    if ((play_num_msg <= 0x0C99) && (play_num_msg >= 0x0C00)) {
        // 数字 0~10 只需要播放 1 次
        if ((play_num_msg >= 0x0C00) && (play_num_msg <= 0x0C10)) play_times = 1;

        // 数字 11~20 30 40 50 60 70 80 90 需要播放 2 次
        else if ((play_num_msg >= 0x0C11) && (play_num_msg <= 0x0C20)) play_times = 2;
        else if (play_num_msg == 0x0C30) play_times = 2;
        else if (play_num_msg == 0x0C40) play_times = 2;
        else if (play_num_msg == 0x0C50) play_times = 2;
        else if (play_num_msg == 0x0C60) play_times = 2;
        else if (play_num_msg == 0x0C70) play_times = 2;
        else if (play_num_msg == 0x0C80) play_times = 2;
        else if (play_num_msg == 0x0C90) play_times = 2;

        // 21~29 31~39 ... 91~99 需要播放 3 次
        else play_times = 3;


        /* 计算十进制 num，通过 play_num_msg 转换 */
        u8 bcd = play_num_msg & 0xFF;
        num = ((bcd >> 4) * 10) + (bcd & 0x0F);
    }

    if (num == 0xFF) return;

    if (play_num_msg != 0) {
        tens_num = 0;
        ten_num = 0;
        units_num = 0;

        music_layer_sta_set(MSC_LAYER0, LAYER_STOP);
    }

    /* 如果播放次数大于 0，则播放 */ // 九十九  九十八
    if (play_times > 0) {
        layer_sta_e play_sta = music_layer_sta_get(MSC_LAYER0);
        if (!(play_sta == LAYER_PLAYING)) {
            play_times--;
            check_and_play(&num);
        }
    }

    play_num_msg = 0;
}
#endif


/* 公用代码 0x0B00 ~ 0x0B0B
    1.矩阵按键 控制LED亮灭
    2.矩阵按键 控制数码管亮灭
*/
/* =====矩阵按键 控制LED亮灭===== */
#if 0
/* =====矩阵按键 控制LED亮灭 相关参数===== */
#if 0
// 全局数组变量，使用 9bit，一个bit对应一个LED当前的的状态
u16 leds_states[KEY_STATES] = {0};
#endif
/* =====矩阵按键 控制LED亮灭 相关函数===== */
#if 0
void matrix_key_led_process(u16 msg) {
    // 按键按下后，发送一串代码，在这里进行解析，解析完，设置全局数组变量
    // u16 msg = msg_dequeue();
    // // 不是我这里能处理的msg直接送回去
    // if (!((msg >= 0x0A00) && (msg <= 0x0A0B))) {
    //     msg_enqueue(msg);
    //     return;
    // }

    // 按键 1~9 控制 LED 1~9 亮灭
    if (msg == 0x0A01) {
        msg = 0x0B00;
        leds_states[0] = 0x01; // x1y1 LED 亮
    }
    else if (msg == 0x0A02) {
        msg = 0x0B01;
        leds_states[1] = 0x02; // x2y1 LED 亮
    }
    else if (msg == 0x0A03) {
        msg = 0x0B02;
        leds_states[2] = 0x04; // x3y1 LED 亮
    }
    else if (msg == 0x0A04) {
        msg = 0x0B03;
        leds_states[3] = 0x08; // x1y2 LED 亮
    }
    else if (msg == 0x0A05) {
        msg = 0x0B04;
        leds_states[4] = 0x10; // x2y2 LED 亮
    }
    else if (msg == 0x0A06) {
        msg = 0x0B05;
        leds_states[5] = 0x20; // x3y2 LED 亮
    }
    else if (msg == 0x0A07) {
        msg = 0x0B06;
        leds_states[6] = 0x40; // x1y3 LED 亮
    }
    else if (msg == 0x0A08) {
        msg = 0x0B07;
        leds_states[7] = 0x80; // x2y3 LED 亮
    }
    else if (msg == 0x0A09) {
        msg = 0x0B08;
        leds_states[8] = 0x100; // x3y3 LED 亮
    }
    // 按键 */# 不播放
    else if (msg == 0x0A0A) {
        msg = 0x0B0A;
        // 'Z'
        leds_states[10] = 0x1DF; // E
    }
    else if (msg == 0x0A00) {
        msg = 0x0B09;
        // 'E'
        leds_states[9] = 0x1D7; // Z
    }

    else if (msg == 0x0A0B) {
        msg = 0x0B0B;
        // 'R'
        leds_states[11] = 0x15F; // R
    }

    if ((msg <= 0x0B0B) && (msg >= 0x0B00)) {
        msg_enqueue(msg);
    }
}
#endif
#endif


/* =====矩阵按键 控制数码管亮灭===== */
// (按键 0~9 控制 数码管显示 0~9) (按键 * 控制数字++) (按键 # 控制数字 --)
#if 1
void matrix_key_seg7_process(void) {
    u16 msg = msg_dequeue();
    /* 不是我这里能处理的msg直接送回去 */
    if (!((msg >= 0x0A00) && (msg <= 0x0A0B))) {
        msg_enqueue(msg);
        return;
    }

    if (msg == 0x0A01) {
        msg = 0x0B01;
    }
    else if (msg == 0x0A02) {
        msg = 0x0B02;
    }
    else if (msg == 0x0A03) {
        msg = 0x0B03;
    }
    else if (msg == 0x0A04) {
        msg = 0x0B04;
    }
    else if (msg == 0x0A05) {
        msg = 0x0B05;
    }
    else if (msg == 0x0A06) {
        msg = 0x0B06;
    }
    else if (msg == 0x0A07) {
        msg = 0x0B07;
    }
    else if (msg == 0x0A08) {
        msg = 0x0B08;
    }
    else if (msg == 0x0A09) {
        msg = 0x0B09;
    }
    // 按键 */# 不播放
    else if (msg == 0x0A0A) {
        msg = 0x0B0A;
    }
    else if (msg == 0x0A00) {
        msg = 0x0B00;
    }
    else if (msg == 0x0A0B) {
        msg = 0x0B0B;
    }

    msg_enqueue(msg);
}
#endif

/* =====矩阵按键 指定数字音频响起 控制LED/Seg7 亮灭 相关函数===== */
#if 0
void matrix_key_play_and_led_process(void) {
    u16 msg = msg_dequeue();
    // 不是我这里能处理的msg直接送回去
    if (!((msg >= 0x0A00) && (msg <= 0x0A0B))) {
        msg_enqueue(msg);
        return;
    }

    /* 矩阵按键按下 播放音频处理函数 */
    matrix_key_play_process(msg);

    /* 矩阵按键按下 矩阵灯亮起处理函数 */
#if 0
    matrix_key_led_process(msg);
#endif

    /* 按键按下 数码管 亮起指定数字 处理函数 */
    matrix_key_seg7_process(msg);

    /* 按键按下 数码管 亮起指定数字 处理函数 */

}
#endif

AT(.text.func.process)
void func_process(void)
{
    WDT_CLR();
    print_info();

#if RGB_SERIAL_EN
    if (sys_cb.rgb_update_flag) {
        sys_cb.rgb_update_flag = 0;
        SPI1BAUD = get_sysclk_nhz()/SPI_LED_BAUD - 1;//防止修改时钟频率后影响SPILED
        spi1_led_auto_process();
        spi1_led_data_send_proc();
    }
    // spi1_led_auto_process();
    // spi1_led_data_send_proc();
#endif

#if MUSIC_DECODE_BK_EN
    music_decode_process();
#endif
#if VBAT_DETECT_EN
    lowpower_vbat_process();
#if VDDIO_FOLLOW_VBAT_EN
    vddio_follow_vbat_process();
#endif
#endif // VBAT_DETECT_EN

#if HUART_EN
    huart_deal_process();
#endif
#if USER_UART0_EN
    uart_deal_process();
#endif
#if MIDI_UART0_EN
    midi_uart_rx_process();
    midi_uart_tx_process();
#endif
#if MAXTRIX_TRIGLE_KEYBOARD_EN
    maxtrix_triangle_scan_process();
#endif // MAXTRIX_TRIGLE_KEYBOARD_EN
#if TKEY_PRESS_UPDATE
    tkey_press_update();
#endif
#if TKEY_BUF_DOWN_EN
    tkey_tkbuf_down();
#endif
#if (MIDI_EN && MIDI_METRO_EN)
    midi_metro_proc();
#endif
#if PWM_HW_CLK_AUTO_SHIFT
    pwm_clk_sync();
#endif
#if SPI1_AUDIO_EN
    spi1_main_process();
#endif
#if MSC_BREAK_P_SOFT_FADE
    break_play_fade_process();
#endif
    soft_fade_process();
}

//func common message process
AT(.text.func.msg)
void func_message(u16 msg)
{
    switch (msg) {
#if (MIDI_KEYS_TEST || MIDI_REC_TEST_EN)
        case K_VOL_UP:
        #if DAC_SOFT_DNR
            dac_dnr_in();
            set_dac_dnr_sta(1);
        #endif
            mkey_note_onoff(0x96, 0x4A, 0x2D);
            break;
        case KU_VOL_UP:
            mkey_note_onoff(0x96, 0x4A, 0x00);
            break;
        case KLU_VOL_UP:
            mkey_note_onoff(0x96, 0x4A, 0x00);
            break;
#else
        case KU_VOL_UP:
        case KL_VOL_UP:
        case KH_VOL_UP:
#endif
#if MIDI_TEST_EN && MIDI_EN
        #if MIDI_DEC_BK_EN
            #if MIDI_OKON_EN
                midi_song_play(RES_BUF_MID_DEMO_MIDI0_MID, RES_LEN_MID_DEMO_MIDI0_MID, MIDI_MODE_OKON, 0);     //标准midi okon播放示例,先播放再set_midi_okon_next
            #else
            if((get_msc_layer_mode(0) == MSC_MP3) || (get_msc_layer_mode(0) == MSC_MI_NONE)) {
                vmidi_song_play(RES_BUF_MID_DEMO_MIDI1_VMID, RES_LEN_MID_DEMO_MIDI1_VMID, MIDI_MODE_NORM, 0);  //vmidi大循环播放示例
            }
            else{
                mp3_song_play(RES_BUF_EN_NUM127_MP3, RES_LEN_EN_NUM127_MP3, 0);
            }
            #endif
        #else
            vmidi_res_play(RES_BUF_MID_DEMO_MIDI1_VMID, RES_LEN_MID_DEMO_MIDI1_VMID);       //vmidi独占播放示例
        #endif
#else
            bsp_set_volume(bsp_volume_inc(sys_cb.vol, 1));
#endif
            break;

        case KU_VOL_DOWN:
        case KL_VOL_DOWN:
        case KH_VOL_DOWN:
#if MIDI_REC_TEST_EN
            if(get_midi_rec_state() == REC_STATE_NULL) {
                midi_rec_start();
            }
            else{
                midi_rec_stop();
            }
#endif
#if MIDI_TEST_EN && MIDI_EN
        #if MIDI_DEC_BK_EN
            midi_song_play(RES_BUF_MID_DEMO_MIDI0_MID, RES_LEN_MID_DEMO_MIDI0_MID, MIDI_MODE_NORM, 0);         //标准midi播放示例
            #else
            vmidi_res_play(RES_BUF_MID_DEMO_MIDI2_VMID, RES_LEN_MID_DEMO_MIDI2_VMID);
        #endif
#else
            bsp_set_volume(bsp_volume_dec(sys_cb.vol, 1));
#endif
            break;

        case KU_MODE:
        case KU_MODE_POWER:
        case KL_PLAY_MODE:
            func_cb.sta = FUNC_NULL;
            break;

        case KD_VOL_DOWN:
            break;

        case KU_PLAY:
        case KL_PLAY:
        case KH_PLAY:
        case KD_PLAY:
#if MSC_BREAK_P_SOFT_FADE
            mp3_song_play(RES_BUF_EN_POWERON_MP3, RES_LEN_EN_POWERON_MP3, 0);
#endif
#if MIDI_TEST_EN && (!MIDI_OKON_EN) && MIDI_EN
            midi_control(MIDI_STOP);
#endif
#if MIDI_OKON_EN && MIDI_EN
            set_midi_okon_next(1);
            // u8 next_note, next_vel, next_ch;
            // if(get_next_note_info(&next_note, &next_vel, &next_ch)) {
            //     my_printf("next note: %d, vel: %d, ch: %d\n", next_note, next_vel, next_ch);
            // } else {
            //     my_printf("no next note\n");
            // }

            if(get_next_note() != 0xFF) {
                my_printf("next note: %d\n", get_next_note());
            } else {
                my_printf("no next note\n");
            }
#endif
            break;

#if MSC_BREAK_P_SOFT_FADE
        case KU_AB_PLAY:
            wav_song_play(MSC_LAYER1, RES_BUF_EN_DNOTE_WAV, RES_LEN_EN_DNOTE_WAV, 0);
        break;
#endif

#if MUSIC_SDCARD_EN
        case EVT_SD_INSERT:
            if (dev_is_online(DEV_SDCARD)) {
                sys_cb.cur_dev = DEV_SDCARD;
                func_cb.sta = FUNC_MUSIC;
            }
            break;
#endif // MUSIC_SDCARD_EN

#if MUSIC_SDCARD1_EN
        case EVT_SD1_INSERT:
            if (dev_is_online(DEV_SDCARD1)) {
                sys_cb.cur_dev = DEV_SDCARD1;
                func_cb.sta = FUNC_MUSIC;
            }
            break;
#endif // MUSIC_SDCARD1_EN

#if MIX_PWR_DOWN_EN
        case KLH_PLAY_POWER:
            mix_pwr_down(1);
            break;
        case KLH_MODE_POWER:
            mix_pwr_down(0);
            break;
#else
        case KU_PLAY_POWER:
            // lsbc_song_play(RES_BUF_EN_SHIN_LSBC, RES_LEN_EN_SHIN_LSBC, 0);
#if MSC_MP3_LINK_EN
            mp3_song_link_init(MSC_MP3_VMP3_LINK1_NUM);
#endif
            break;
        //长按PP/POWER软关机(通过PWROFF_PRESS_TIME控制长按时间)
        case KLH_PLAY_POWER:
        case KLH_MODE_POWER:
        case KLH_HSF_POWER:
        case KLH_POWER:
            sys_cb.pwrdwn_tone_en = 1;
            func_cb.sta = FUNC_PWROFF;
            break;
#endif
        case K_NEXT:
            #if MIDI_REC_TEST_EN
            midi_rec_stop();
            midi_rec_play();
            #endif
            break;
        case K_PREV:
            #if MIDI_REC_TEST_EN
            midi_rec_play_switch();
            #endif
            break;

        case MSG_SYS_1S:
#if SYS_SLEEP_TIME      //自动休眠
            pwroff_flag++;
            if(pwroff_flag == SYS_SLEEP_TIME){
                pwroff_flag = 0;
                sys_cb.pwrdwn_tone_en = 1;
                func_cb.sta = FUNC_PWROFF;
            }
#endif
            break;

#if IRRX_SW_EN || TKEY_SLEEP_MODE
        case KU_IR_POWER:
            func_cb.sta = FUNC_SLEEPMODE;
            break;
#endif

#if UART_S_UPDATE
        case EVT_UART_UPDATE:
            if(deal_update_uart_msg()) {
                func_cb.sta = FUNC_NULL;
            }
            break;
#endif

#if MUL_PWRON_IO_EN
        case KEY_NUM_0:
            dac_fade_in(6);
            dac_fade_out(6);
            break;
        case KEY_NUM_1:
            dac_fade_in(6);
            dac_fade_out(6);
            break;
        case KEY_NUM_2:
        case KEY_NUM_3:
        case KEY_NUM_4:
        case KEY_NUM_5:
        case KEY_NUM_6:
        case KEY_NUM_7:
        case KEY_NUM_8:
        case KEY_NUM_9:
        case KEY_NUM_P100:
            break;
#endif
        default:
            break;

    }

    //调节音量，3秒后写入flash
    if ((sys_cb.cm_vol_change) && (sys_cb.cm_times >= 6)) {
        sys_cb.cm_vol_change = 0;
        cm_sync();
    }
}



///进入一个功能的总入口
AT(.text.func)
void func_enter(void)
{
#if (GUI_SELECT != GUI_NO)
    gui_box_clear();
#endif
    param_sync();
    func_cb.mp3_res_play = NULL;
    func_cb.set_vol_callback = NULL;
//    bsp_clr_mute_sta();
}

AT(.text.func)
void func_exit(void)
{
#if UART_S_UPDATE
    uart_s_update();
#endif // UART_S_UPDATE
    u8 func_num;
    u8 funcs_total = get_funcs_total();

    for (func_num = 0; func_num != funcs_total; func_num++) {
        if (func_cb.last == func_sort_table[func_num]) {
            break;
        }
    }
    func_num++;                                     //切换到下一个任务
    if (func_num >= funcs_total) {
        func_num = 0;
    }
    func_cb.sta = func_sort_table[func_num];        //新的任务
}

AT(.text.func)
void func_run(void)
{
    printf("%s\n", __func__);
    while (1) {
        func_enter();

        // static u32 ticks = 0;
        // if (tick_check_expire(ticks, 1000)) {
        //     ticks = tick_get();

        //     // my_printf("a.");
        //     // vmp3_res_play(RES_BUF_EN_POWERON_VMP3, RES_LEN_EN_POWERON_VMP3, 0);
        //     // vmp3_res_play(RES_BUF_EN_NUM127_VMP3, RES_LEN_EN_NUM127_VMP3, 0);
        // }


        switch (func_cb.sta) {
#if FUNC_MUSIC_EN
        case FUNC_MUSIC:
            func_music();
            break;
#endif // FUNC_MUSIC_EN

#if EX_SPIFLASH_SUPPORT
        case FUNC_EXSPIFLASH_MUSIC:
            func_exspiflash_music();
            break;
#endif // EX_SPIFLASH_SUPPORT

#if FUNC_CLOCK_EN
        case FUNC_CLOCK:
            func_clock();
            break;
#endif // FUNC_CLOCK_EN

#if FUNC_IDLE_EN
        case FUNC_IDLE:
            func_idle();
            break;
#endif // FUNC_IDLE_EN

        case FUNC_PWROFF:
            func_pwroff(sys_cb.pwrdwn_tone_en);
            break;

        case FUNC_SLEEPMODE:
            func_sleepmode();
            break;

        default:
            func_exit();
            break;
        }
    }
}





AT(.com_text.key_isr) FIQ
void key1_isr() {
    if(WKUPEDG & BIT(22))       // 查下降沿标志
    {
        WKUPCPND = BIT(22);     // 清下降沿标志

        /* 代码逻辑 */
        printf("key1 interrupt!\n");
    }
}

void gpioa_2_init() {
    // ========== 1.GPIOA2 配置 ==========
    GPIOAFEN   &= ~BIT(2);                          // PA2 复用关闭 -> 作为 普通GPIO 使用
    GPIOADE    |=  BIT(2);                          // PA2 设置为 数字 IO
    GPIOADIR   |=  BIT(2);                          // PA2 方向设置为 输入
    GPIOAPU    |=  BIT(2);                          // PA2 内部上拉10k

    // ========== 2.PORT中断配置 ==========
    PORTINTEN  |=  BIT(2);                          // PA2 PORT 中断使能
    PORTINTEDG |=  BIT(2);                          // PA2 下降沿触发 产生信号 PORT_INT_FALL

    // ========== 3.唤醒系统配置 ==========
    WKUPEDG    |= BIT(6);                           // 唤醒源6 边沿选择：下降沿检测信号 PORT_INT_FALL
    WKUPCPND    = BIT(22);                          // 写 WKCPND[6] = 1，清除唤醒源6（PORT_INT_FALL）的挂起标志
    WKUPCON    |= BIT(6) | BIT(16);                 // 配置哪个唤醒源使能（控制寄存器）：下降沿的唤醒源使能，中断总使能

    // ========== 4.注册中断 ==========
    sys_irq_init(IRQ_PORT_VECTOR, 1, key1_isr);     // 注册中断
}






/* Normal Key GPIOA 2/3 初始化 */
void normal_key_gpio_init(void) {
    // 初始化 GPIOA 2 输入
    GPIOAFEN &= ~BIT(2);    // PA2 复用关闭 -> 作为 普通GPIO 使用
    GPIOADE  |=  BIT(2);    // PA2 设置为 数字 IO
    GPIOADIR |=  BIT(2);    // PA2 方向设置为 输入
    GPIOAPU  |=  BIT(2);    // PA2 内部上拉10k

    // 初始化 GPIOA 3 输入
    GPIOAFEN &= ~BIT(3);    // PA3 复用关闭 -> 作为 普通GPIO 使用
    GPIOADE  |=  BIT(3);    // PA3 设置为 数字 IO
    GPIOADIR |=  BIT(3);    // PA3 方向设置为 输入
    GPIOAPU  |=  BIT(3);    // PA3 内部上拉10k
}

/* matrix Key GPIOA 0~6 初始化 */
void matrix_key_gpio_init(void) {
    /* GPIOA 0-3 设置为输出模式 行 */
    // 初始化 GPIOA 0 输出
    GPIOAFEN &= ~BIT(0);    // PA0 复用关闭 -> 作为 普通GPIO 使用
    GPIOADE  |=  BIT(0);    // PA0 设置为 数字 IO
    GPIOADIR &= ~BIT(0);    // PA0 方向设置为 输出

    // 初始化 GPIOA 1 输出
    GPIOAFEN &= ~BIT(1);    // PA1 复用关闭 -> 作为 普通GPIO 使用
    GPIOADE  |=  BIT(1);    // PA1 设置为 数字 IO
    GPIOADIR &= ~BIT(1);    // PA1 方向设置为 输出

    // 初始化 GPIOA 2 输出
    GPIOAFEN &= ~BIT(2);    // PA2 复用关闭 -> 作为 普通GPIO 使用
    GPIOADE  |=  BIT(2);    // PA2 设置为 数字 IO
    GPIOADIR &= ~BIT(2);    // PA2 方向设置为 输出

    // 初始化 GPIOA 3 输出
    GPIOAFEN &= ~BIT(3);    // PA3 复用关闭 -> 作为 普通GPIO 使用
    GPIOADE  |=  BIT(3);    // PA3 设置为 数字 IO
    GPIOADIR &= ~BIT(3);    // PA3 方向设置为 输出


    /* GPIOA 4-6 设置为输入模式 列 */
    // 初始化 GPIOA 4 输入
    GPIOAFEN &= ~BIT(4);    // PA4 复用关闭 -> 作为 普通GPIO 使用
    GPIOADE  |=  BIT(4);    // PA4 设置为 数字 IO
    GPIOADIR |=  BIT(4);    // PA4 方向设置为 输入
    GPIOAPU  |=  BIT(4);    // PA4 内部上拉10k

    // 初始化 GPIOA 5 输入
    GPIOAFEN &= ~BIT(5);    // PA5 复用关闭 -> 作为 普通GPIO 使用
    GPIOADE  |=  BIT(5);    // PA5 设置为 数字 IO
    GPIOADIR |=  BIT(5);    // PA5 方向设置为 输入
    GPIOAPU  |=  BIT(5);    // PA5 内部上拉10k

    // 初始化 GPIOA 6 输入
    GPIOAFEN &= ~BIT(6);    // PA6 作为 GPIO 使用
    GPIOADE  |=  BIT(6);    // PA6 设置为 数字 IO
    GPIOADIR |=  BIT(6);    // PA6 方向设置为 输入
    GPIOAPU  |=  BIT(6);    // PA6 内部上拉10k
    // GPIOAPD  |=  BIT(6);    // PA6 内部下拉60k

    GPIOA |= BIT(0);
    GPIOA |= BIT(1);
    GPIOA |= BIT(2);
    GPIOA |= BIT(3);

    /* 剔除相关脏数据 */
    msg_queue_detach(0x0A00, 0);
    msg_queue_detach(0x0A01, 0);
    msg_queue_detach(0x0A02, 0);
    msg_queue_detach(0x0A03, 0);
    msg_queue_detach(0x0A04, 0);
    msg_queue_detach(0x0A05, 0);
    msg_queue_detach(0x0A06, 0);
    msg_queue_detach(0x0A07, 0);
    msg_queue_detach(0x0A08, 0);
    msg_queue_detach(0x0A09, 0);
    msg_queue_detach(0x0A0A, 0);
    msg_queue_detach(0x0A0B, 0);
}

/* matrix LED GPIOB 0-2 4-6 初始化 */
#if 0
void matrix_led_gpio_init(void) {
    /* GPIOB 0-2 设置为输出模式 行 */
    // 初始化 GPIOA 0 输出 GND
    GPIOBFEN &= ~BIT(0);    // PA0 复用关闭 -> 作为 普通GPIO 使用
    GPIOBDE  |=  BIT(0);    // PA0 设置为 数字 IO
    GPIOBDIR &= ~BIT(0);    // PA0 方向设置为 输出

    // 初始化 GPIOA 1 输出
    GPIOBFEN &= ~BIT(1);    // PA1 复用关闭 -> 作为 普通GPIO 使用
    GPIOBDE  |=  BIT(1);    // PA1 设置为 数字 IO
    GPIOBDIR &= ~BIT(1);    // PA1 方向设置为 输出

    // 初始化 GPIOA 2 输出
    GPIOBFEN &= ~BIT(2);    // PA2 复用关闭 -> 作为 普通GPIO 使用
    GPIOBDE  |=  BIT(2);    // PA2 设置为 数字 IO
    GPIOBDIR &= ~BIT(2);    // PA2 方向设置为 输出


    /* GPIOB 4-6 设置为输出模式 列 */
    // 初始化 GPIOA 4 输出
    GPIOBFEN &= ~BIT(4);    // PA0 复用关闭 -> 作为 普通GPIO 使用
    GPIOBDE  |=  BIT(4);    // PA0 设置为 数字 IO
    GPIOBDIR &= ~BIT(4);    // PA0 方向设置为 输出

    // 初始化 GPIOA 5 输出
    GPIOBFEN &= ~BIT(5);    // PA1 复用关闭 -> 作为 普通GPIO 使用
    GPIOBDE  |=  BIT(5);    // PA1 设置为 数字 IO
    GPIOBDIR &= ~BIT(5);    // PA1 方向设置为 输出

    // 初始化 GPIOA 6 输出
    GPIOBFEN &= ~BIT(6);    // PA2 复用关闭 -> 作为 普通GPIO 使用
    GPIOBDE  |=  BIT(6);    // PA2 设置为 数字 IO
    GPIOBDIR &= ~BIT(6);    // PA2 方向设置为 输出

    // 默认状态全灭
    GPIOB &= ~BIT(0);
    GPIOB &= ~BIT(1);
    GPIOB &= ~BIT(2);
    // GPIOB |=  BIT(1);
    // GPIOB |=  BIT(2);

    // GPIOB |=  BIT(4);
    GPIOB &= ~BIT(4);
    GPIOB &= ~BIT(5);
    GPIOB &= ~BIT(6);
}
#endif

/* Seg7 GPIOA11-12 GPIOB 0 9 1 2 4-7 初始化 */
void seg7_gpio_init(void) {
    // 初始化 GPIOA 11 12 共阴 输出 GND
    GPIOAFEN &= ~BIT(11);    // PA11 复用关闭 -> 作为 普通GPIO 使用
    GPIOADE  |=  BIT(11);    // PA11 设置为 数字 IO
    GPIOADIR &= ~BIT(11);    // PA11 方向设置为 输出

    GPIOAFEN &= ~BIT(12);
    GPIOADE  |=  BIT(12);
    GPIOADIR &= ~BIT(12);

    // 初始化 GPIOB 0 1 2 4 5 6 7 GPIOB 8
    GPIOBFEN &= ~BIT(0);
    GPIOBDE  |=  BIT(0);
    GPIOBDIR &= ~BIT(0);

    GPIOAFEN &= ~BIT(8);
    GPIOADE  |=  BIT(8);
    GPIOADIR &= ~BIT(8);

    GPIOBFEN &= ~BIT(1);
    GPIOBDE  |=  BIT(1);
    GPIOBDIR &= ~BIT(1);

    GPIOBFEN &= ~BIT(2);
    GPIOBDE  |=  BIT(2);
    GPIOBDIR &= ~BIT(2);

    GPIOBFEN &= ~BIT(4);
    GPIOBDE  |=  BIT(4);
    GPIOBDIR &= ~BIT(4);

    GPIOBFEN &= ~BIT(5);
    GPIOBDE  |=  BIT(5);
    GPIOBDIR &= ~BIT(5);

    GPIOBFEN &= ~BIT(6);
    GPIOBDE  |=  BIT(6);
    GPIOBDIR &= ~BIT(6);

    GPIOBFEN &= ~BIT(7);
    GPIOBDE  |=  BIT(7);
    GPIOBDIR &= ~BIT(7);

    // 设置默认 IO 默认输出状态（不亮）
    // 共阴引脚 GPIOA 11/12 默认设置 VCC
    GPIOA |= BIT(11);
    GPIOA |= BIT(12);

    // 其它引脚 GPIOB 0 9 1 2 4 5 6 7 默认设置 GND
    GPIOB &= ~BIT(0);    // A
    GPIOA &= ~BIT(8);    // B
    GPIOB &= ~BIT(1);    // C
    GPIOB &= ~BIT(2);    // D
    GPIOB &= ~BIT(4);    // E
    GPIOB &= ~BIT(5);    // F
    GPIOB &= ~BIT(6);    // G
    GPIOB &= ~BIT(7);    // DP
}



AT(.text.func)
void user_main(void)
{
    WDT_CLR();

    /* Matrix Key GPIOA 0~6 初始化 */
    matrix_key_gpio_init();

    /* Matrix Led GPIOB 0~2 4~7 初始化 */
#if 0
    matrix_led_gpio_init();
#endif

    /* Seg7 GPIOA11-12 GPIOB 0 9 1 2 4-7 初始化 */
    seg7_gpio_init();


    /* Normal Key GPIOA 2/3 初始化 */
    // normal_key_gpio_init();

    while (1)
    {
        /* 系统自带的处理函数 */
        func_process();

        /* 普通按键播放处理函数 */
        // key_play_process();

        /* 矩阵按键 单独控制LED 处理函数 */
        // matrix_key_led_process();

        /* =====矩阵按键 指定数字音频响起 控制LED亮灭 相关函数===== */
        // matrix_key_play_and_led_process();

        /* 矩阵按键 单独播放 处理函数 */
        // matrix_key_play_process();

        /* 矩阵按键 单独控制 seg7 处理函数 */
        // matrix_key_seg7_process();

        /* 系统休眠处理函数：空闲 10s 进入休眠 */
        // system_sleep_process();
    }
}
