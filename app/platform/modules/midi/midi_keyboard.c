#include "include.h"
#include "midi_keyboard.h"

// #if MIDI_KEYBOARD_EN

#define TRACE_EN                1
#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

void mkey_mode_enter(void)
{
    if((!layer_mode_check(MSC_LAYER0, MSC_MIDI)) && (!layer_mode_check(MSC_LAYER0, MSC_VMIDI))){
        music_mode_enter(MSC_LAYER0, MSC_MIDI);
    }
    if(get_music_dec_sta() != MUSIC_MSG_PLAY){
        music_control(MUSIC_MSG_PLAY);
    }
}

//mkey_note_onoff回调
void mkey_note_callback(void)
{

}

//midi功能按键处理
void mkey_func_proc(u16 msg)
{
    // printf("press button2333333!\n");   
    u32 up_tmp, dw_tmp;
    switch(msg){
        case KU_MIDI_VOICE1:
        case KLU_MIDI_VOICE1:
            TRACE("voice grandpiano\n");
            midi_voice_kill();
            mkey_prog_change(0xC0|MKEY_DEF_CH, VOICE_PROG_GRANDPNO);
            break;
        case KU_MIDI_VOICE2:
        case KLU_MIDI_VOICE2:
            TRACE("voice musicbox\n");
            midi_voice_kill();
            mkey_prog_change(0xC0|MKEY_DEF_CH, VOICE_PROG_MUSICBOX);
            break;

        case KU_MIDI_TICK_UP:
            up_tmp = get_midi_tick();
            TRACE("midi tick:%d\n", (up_tmp+5));
            set_midi_tick((up_tmp+5));
            break;
        case KU_MIDI_TICK_DW:
            dw_tmp = get_midi_tick();
            TRACE("midi tick:%d\n", (dw_tmp-5));
            set_midi_tick((dw_tmp-5));
            break;
        default:
            break;
    }
}

//midi琴键发音消息处理
void mkey_note_proc(u16 key_type, u16 key_num)
{
    switch(key_type){
        case KEY_SHORT:
            // mkey_note_onoff(0x90|MKEY_DEF_CH, (MKEY_NOTE_START + key_num), MKEY_DEF_VELOCITY);
            // vmp3_res_play(RES_BUF_EN_NUM127_VMP3, RES_LEN_EN_NUM127_VMP3, 0); 
            
            break;
        case KEY_SHORT_UP:
        case KEY_LONG_UP:
            mkey_note_onoff(0x90|MKEY_DEF_CH, (MKEY_NOTE_START + key_num), 0x00);
            break;
        default:
            break;
    }
}

void midi_keyboard_msg_proc(u16 msg)
{
    u16 key_type = (msg & 0xFF00); 
    u16 key_num  = (msg & 0x00FF);

    printf("midi_msg: 0x%04x\n", msg); // 你原来的测试日志

    if(key_num >= MKEY_RANGE_MIN && key_num <= MKEY_RANGE_MAX){
        mkey_note_proc(key_type, (key_num - KEY_MAT_1));
    }else{
        mkey_func_proc(msg);
    }
}
// #endif
