#include "include.h"
#include "midi_rec.h"

#if MIDI_REC_EN

#if 0
#define DEBUG   printf
#else
#define DEBUG(...)
#endif

extern u32 midi_rec_scan_file(void);
extern u8 midi_rec_creat_file(void);
extern u32 midi_get_file_len(void);
extern void midi_rec_open_file(u32 index);
extern void midi_rec_close_file(void);
extern u8 midi_rec_write_file(void *buf, u32 len);
extern void midi_rec_read_file(void *buf, u32 len);
extern void cycrec_fs_init(u32 begin_addr, u32 end_addr, void(*read)(void *buf, u32 addr, u32 len),void(*write)(void *buf, u32 addr, u32 len),void (*erase)(u32 addr) );
extern void midi_rec_cfg_init(midi_rec_cfg_t *m_cfg);
uint os_spiflash_read(void *buf, u32 addr, uint len);       //SPI0 flash读取
void spiflash_program(void *buf, u32 addr, uint len);       //SPI0 flash编程
void spiflash_erase(u32 addr);                              //SPI0 flash擦除

#define BUF_LEN (128)

u32 k_delta_time = 0;
u8 rec_buf[BUF_LEN] AT(.midi_buf.rec_buf);
static midi_rec_cfg_t midi_rec_cfg AT(.m_buf.rec_cfg);

//midi录音状态
u8 get_midi_rec_state(void)
{
    return midi_rec_cfg.rec_state;
}

//midi录音符读flash数据
u8* midi_rec_midi_data_read(void)
{
    if(midi_rec_cfg.rec_file_len > BUF_LEN) {
        memset(rec_buf, 0, sizeof(rec_buf));
        midi_rec_read_file(rec_buf, BUF_LEN);
        //my_print_r(rec_buf, BUF_LEN);
        midi_rec_cfg.rec_file_len -= BUF_LEN;
    }
    else {
        memset(rec_buf, 0, sizeof(rec_buf));
        midi_rec_read_file(rec_buf, midi_rec_cfg.rec_file_len);
        //my_print_r(rec_buf, midi_rec_cfg.rec_file_len);
        midi_rec_cfg.rec_file_len = 0;
    }
    return (u8*)rec_buf;
}

//midi录音符写缓存，每BUF_LEN写一次flash
void midi_rec_midi_data_write(u8* buf, u8 len)
{
    for(int i=0; i<len; i++)
    {
        *midi_rec_cfg.ptr_rec_w++ = buf[i];
        midi_rec_cfg.rec_file_cur_len++;
        if(midi_rec_cfg.ptr_rec_w > &rec_buf[BUF_LEN-1]){
            midi_rec_write_file(rec_buf, sizeof(rec_buf));
            //DEBUG("rec_buf\n");
            //my_print_r(rec_buf, BUF_LEN);
            midi_rec_cfg.ptr_rec_w = rec_buf;
        }
    }
}

//midi录音开始
void midi_rec_start(void)
{
    u8 midi_rec_head[] = {0x4D, 0x54, 0x68, 0x64, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x01, 0x01, 0xE0,
    0x4D, 0x54, 0x72, 0x6B, 0x00, 0x00, 0x00, 0x25, 0x00,
    0xFF, 0x51, 0x03, 0x07, 0xA1, 0x20};

    midi_rec_cfg.ptr_rec_w = rec_buf;
    midi_rec_cfg.rec_state = REC_STATE_START;
    midi_rec_cfg.rec_first_flag = true;
    midi_rec_cfg.rec_file_index = midi_rec_creat_file();
    DEBUG("rec_start f_index:%d\n",midi_rec_cfg.rec_file_index);
    for(int i=0; i<sizeof(midi_rec_head); i++)
    {
        *midi_rec_cfg.ptr_rec_w++ = midi_rec_head[i];
        midi_rec_cfg.rec_file_cur_len++;
    }
}

//midi录音结束
void midi_rec_stop(void)
{
    u8 midi_rec_end[] = {0x00, 0xFF, 0x2F, 0x00};

    if(midi_rec_cfg.rec_state == REC_STATE_NULL){
        return;
    }

    for(int i=0; i<4; i++)
    {
        *midi_rec_cfg.ptr_rec_w++ = midi_rec_end[i];
    }
    midi_rec_write_file(rec_buf, sizeof(rec_buf));
    //my_print_r(rec_buf, BUF_LEN);
    midi_rec_close_file();
    midi_rec_cfg.rec_file_cur_index = midi_rec_scan_file();
    midi_rec_cfg.rec_state = REC_STATE_NULL;
}

//midi录音播放
void midi_rec_play(void)
{
    if(midi_rec_cfg.rec_file_index <= 0) {
        return;
    }
    midi_rec_open_file(midi_rec_cfg.rec_file_cur_index);
    midi_rec_cfg.rec_file_len = midi_get_file_len();
    DEBUG("open_file_index:%d, len:%d\n",midi_rec_cfg.rec_file_cur_index, midi_rec_cfg.rec_file_len);
    midi_song_play((u32)midi_rec_midi_data_read(), midi_rec_cfg.rec_file_len, MIDI_MODE_NORM, 0);
    midi_rec_cfg.rec_state = REC_STATE_NULL;
}

//midi录音初始化
void midi_rec_init(void)
{
    cycrec_fs_init(MIDI_SPIFLASH_REC_BEGIN_ADDR, MIDI_SPIFLASH_REC_END_ADDR, (void *)os_spiflash_read, (void *)os_spiflash_program, (void *)os_spiflash_erase);

    memset(&midi_rec_cfg, 0, sizeof(midi_rec_cfg_t));
    midi_rec_cfg.midi_rec_en        = MIDI_REC_EN;
    midi_rec_cfg.ptr_rec_w          = rec_buf;
    midi_rec_cfg.ptr_rec_end        = &rec_buf[127];
    midi_rec_cfg.rec_file_index     = midi_rec_scan_file();
    midi_rec_cfg.rec_file_cur_index = midi_rec_cfg.rec_file_index;
    midi_rec_cfg_init(&midi_rec_cfg);

    DEBUG("midi_rec_cfg.rec_file_index 11:%d\n",midi_rec_cfg.rec_file_index);
}

void midi_rec_play_switch(void)
{
    u32 cur_index;

    if(midi_rec_cfg.rec_file_index <= 0)
    {
        return;
    }
    cur_index = (midi_rec_cfg.rec_file_cur_index >= midi_rec_cfg.rec_file_index) ? 1 : (++midi_rec_cfg.rec_file_cur_index);
    midi_rec_cfg.rec_file_cur_index = cur_index;

    DEBUG("rec_switch :%d\n",midi_rec_cfg.rec_file_cur_index);
}

//midi事件tick转delta time
u32 midi_convert_delta_time(u32 time)
{
    u32 t_time;

    t_time = time * (1000 * 0x1e0) / 0x7a120;   //file_tempo:7a120   file_division:1e0
    DEBUG("convert d_time:%x to %x\n",time,t_time);
    if(midi_rec_cfg.rec_first_flag == true) {
        t_time = 0;
        midi_rec_cfg.rec_first_flag = false;
    }
    return t_time;
}

//midi事件delta time 分解
void midi_rec_delta_time(u8* delta_buf, u8* delta_len)
{
    u32 time;

    time = midi_convert_delta_time(k_delta_time);
    delta_buf[0] = time & 0x7F;
    *delta_len = 1;
    if(time > 0x7f && time <= 0x3FFF) {
        delta_buf[1] = delta_buf[0];
        delta_buf[0] = 0x80 | ((time>>7) & 0x7F);
        DEBUG("delta_d:%x,%x\n",delta_buf[0],delta_buf[1]);
        *delta_len = 2;
    }
    else if(time > 0x3FFF && time <= 0x1FFFFF) {
        delta_buf[2] = delta_buf[0];
        delta_buf[1] = 0x80 | ((time>>7) & 0x7F);
        delta_buf[0] = 0x80 | ((time>>14) & 0x7F);
        *delta_len = 3;
    }
    else if(time > 0x1FFFFF && time <= 0xFFFFFFF) {
        delta_buf[3] = delta_buf[0];
        delta_buf[2] = 0x80 | ((time>>7) & 0x7F);
        delta_buf[1] = 0x80 | ((time>>14) & 0x7F);
        delta_buf[0] = 0x80 | ((time>>21) & 0x7F);
        *delta_len = 4;
    }
}

//midi事件写入flash
void midi_rec_midi_event_write(u8* buf, u8 len)
{
    static u32 key_down_time = 0;
    static u32 key_up_time = 0;
    static bool up_down_flag = 0;
    u8 delta_tim[4];
    u8 delta_len;

    up_down_flag = (up_down_flag == 0) ? 1 : 0;
    if(up_down_flag == 1) {
        key_down_time = tick_get();
        k_delta_time = key_down_time - key_up_time;
    }
    else {
        key_up_time = tick_get();
        k_delta_time = key_up_time - key_down_time;
    }

    midi_rec_delta_time(delta_tim, &delta_len);
    midi_rec_midi_data_write(delta_tim, delta_len);
    midi_rec_midi_data_write(buf,len);
    if(midi_rec_cfg.rec_file_cur_len >= (MIDI_SPIFLASH_REC_LIMIT-11)){
        DEBUG("REC_LIMIT:%d\n",MIDI_SPIFLASH_REC_LIMIT);
        midi_rec_stop();
    }
}
#endif
