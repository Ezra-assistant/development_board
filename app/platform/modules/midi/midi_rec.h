#ifndef _MIDI_REC_H
#define _MIDI_REC_H

enum{
    REC_STATE_NULL = 0,
    REC_STATE_START,
    //REC_STATE_STOP,
};

typedef struct{
    bool rec_first_flag;
    bool midi_rec_en;
    u8* ptr_rec_end;
    u8* ptr_rec_w;
    u8  rec_state;              //录音状态
    u32 rec_file_len;           //录音文件大小
    u32 rec_file_index;         //录音文件序号
    u32 rec_file_cur_len;       //录音文件大小当前偏移
    u32 rec_file_cur_index;     //录音文件当前播放序号
}midi_rec_cfg_t;

u8 get_midi_rec_state(void);
void midi_rec_start(void);
void midi_rec_stop(void);
void midi_rec_play(void);
void midi_rec_init(void);
void midi_rec_play_switch(void);
void midi_rec_midi_event_write(u8* buf, u8 len);


#endif
