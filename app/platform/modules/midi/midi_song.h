#ifndef _MIDI_SONG_H
#define _MIDI_SONG_H

void vmidi_res_play(u32 addr, u32 len);
void vmidi_song_play(u32 addr, u32 len, midi_mode_e midi_mode, u8 loop_cnt);
void midi_song_play(u32 addr, u32 len, midi_mode_e midi_mode, u8 loop_cnt);
void midi_song_play_end(void);
#endif
