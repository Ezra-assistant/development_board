#include "include.h"
#include "midi.h"

#if MIDI_EN

#define MIDI_FIFO_LEN       0x200

u8 midi_fifo[MIDI_FIFO_LEN] AT(.midi_buf.synth);            //midi输入Fifo
cbuf_t midi_ptr AT(.midi_buf.synth);                        //midi输入Fifo指针

static midi_cfg_t midi_cfg AT(.m_buf.midi_cfg);             //midi参数配置

#if MIDI_OKON_EN
static midi_okon_t okon_cfg AT(.m_buf.midi_cfg);            //midi okon参数配置

//key_cnt:累加播放下一段音符的次数
void set_midi_okon_next(u8 key_cnt)
{
    midi_okon_t *o_cfg = &okon_cfg;
    u32 cnt_tmp = o_cfg->okon_key_cnt + key_cnt;
    if(cnt_tmp <= 0xFF){
        o_cfg->okon_key_cnt += key_cnt;
    }
}

// 获取下一个音符信息
// 返回值: 1=有效, 0=无效(已播完或未预读)
u8 get_next_note_info(u8 *note, u8 *velocity, u8 *channel)
{
    midi_okon_t *o_cfg = &okon_cfg;
    if(o_cfg->next_note.valid) {
        if(note) *note = o_cfg->next_note.note;
        if(velocity) *velocity = o_cfg->next_note.velocity;
        if(channel) *channel = o_cfg->next_note.channel;
        return 1;
    }
    return 0;
}

// 简化版：只获取下一个音符值
u8 get_next_note(void)
{
    midi_okon_t *o_cfg = &okon_cfg;
    if(o_cfg->next_note.valid) {
        return o_cfg->next_note.note;
    }
    return 0xFF;  // 无效返回 0xFF
}
#endif

void midi_mode_set(midi_mode_e mode)
{
    if(midi_cfg.midi_mode != mode){
        midi_cfg.midi_mode = mode;
    }
}

//midi解码循环
void midi_decode_proc(msc_layer_e layer_num)
{
#if MIDI_DEC_BK_EN
    if(!(music_layer_sta_get(layer_num) == LAYER_PLAYING)){
        midi_dec_pause_do();
        return;
    }
    #if MIDI_KEYBOARD_EN
    midi_fifo_decode(&midi_ptr);
    #endif
    if(!midi_decode()){
        midi_song_play_end();
    }
#endif
}

void midi_init(void)
{
    memset(&midi_cfg, 0, sizeof(midi_cfg_t));
    circular_buf_init(&midi_ptr, (void *)midi_fifo, MIDI_FIFO_LEN);

    midi_cfg.voice_num      = MIDI_VOICE_NUM;
    midi_cfg.track_num      = MIDI_TRACK_NUM;
    midi_cfg.channel_num    = MIDI_CHANNEL_NUM;
    midi_cfg.smpl_rate      = SMPL_32000;
    midi_cfg.midi_mode      = MIDI_MODE_NORM;
    midi_cfg_init(&midi_cfg);
#if MIDI_OKON_EN
    memset(&okon_cfg, 0, sizeof(midi_okon_t));
    midi_cfg.midi_mode      = MIDI_MODE_OKON;
    okon_cfg.okon_mode      = OKON_MODE_B;
    okon_cfg.okon_dtime     = 1;            //设置1为1秒淡出,2为1/2秒淡出
    midi_okon_init(&okon_cfg);
#endif
    midi_synth_init(VOICE_BIN_ADDR, VOICE_DATA_LEN);
}
#endif
