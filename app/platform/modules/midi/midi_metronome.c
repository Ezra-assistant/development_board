#include "include.h"
#include "midi_metronome.h"

#if MIDI_METRO_EN

#define METRO_DOWNBEAT          37      //强拍
#define METRO_UPBEAT            36      //弱拍
#define METRO_DEF_VELOCITY      100     //节拍器默认力度

midi_metro_t midi_metro AT(.m_buf.metro);

//控制节拍器开关使能
void set_metro_sta(u8 en_sta)
{
    if(midi_metro.metro_enable != en_sta){
        midi_metro.metro_enable = en_sta;
    }
}

//获取节拍器开关状态
u8 get_metro_sta(void)
{
    return midi_metro.metro_enable;
}

void midi_metro_reset(void)
{
    midi_metro.metro_ch   = MIDI_METRO_CH;
    midi_metro.metro_bpm  = MIDI_METRO_BPM;
}

//每拍时间间隔ms
u32 get_metro_tempo(u32 bpm)
{
    return 60000 / bpm;
}

AT(.com_text.midi_metro)
void metro_tick_up(void)
{
    midi_metro.metro_tick++;
}

void midi_metro_proc(void)
{
    if(!get_metro_sta()){
        return;
    }
    if(midi_metro.metro_tick_save != midi_metro.metro_tick){
        midi_metro.metro_tick_save = midi_metro.metro_tick;
        u32 tempo = get_metro_tempo(midi_metro.metro_bpm);
        u32 beat_deal = 1;
        beat_deal = ((midi_metro.metro_tick*100) % tempo);
        if(0 == beat_deal){
            switch(midi_metro.beat_cnt){
                case 0:
                case 1:
                case 2:
                    //发弱拍
                    midi_metro.beat_cnt++;
                    mkey_note_onoff(0x90|midi_metro.metro_ch, METRO_UPBEAT, 0);
                    mkey_note_onoff(0x90|midi_metro.metro_ch, METRO_UPBEAT, METRO_DEF_VELOCITY);
                    break;
                case 3:
                    //发强拍
                    midi_metro.beat_cnt = 0;
                    mkey_note_onoff(0x90|midi_metro.metro_ch, METRO_DOWNBEAT, 0);
                    mkey_note_onoff(0x90|midi_metro.metro_ch, METRO_DOWNBEAT, METRO_DEF_VELOCITY);
                    break;
                default:
                    break;
            }
        }
    }
}

void midi_metro_init(void)
{
    memset(&midi_metro, 0, sizeof(midi_metro_t));
    midi_metro.metro_ch   = MIDI_METRO_CH;
    midi_metro.metro_bpm  = MIDI_METRO_BPM;
    // set_metro_sta(1);
}
#endif