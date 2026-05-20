#ifndef _MIDI_METRONOME_H
#define _MIDI_METRONOME_H

typedef struct{
    u8  metro_enable;
    u8  metro_ch;
    u16 metro_bpm;
    u32 metro_tick;
    u32 metro_tick_save;
    u8  beat_cnt;
}midi_metro_t;

void metro_tick_up(void);
void midi_metro_proc(void);
void midi_metro_init(void);
#endif
