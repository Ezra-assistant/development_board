#ifndef _MIDI_H
#define _MIDI_H

//MIDI channel num list
#define MIDI_CHANNEL_1      0
#define MIDI_CHANNEL_2      1
#define MIDI_CHANNEL_3      2
#define MIDI_CHANNEL_4      3
#define MIDI_CHANNEL_5      4
#define MIDI_CHANNEL_6      5
#define MIDI_CHANNEL_7      6
#define MIDI_CHANNEL_8      7
#define MIDI_CHANNEL_9      8
#define MIDI_CHANNEL_10     9       //drum channel(播放preset num大于128的乐器)
#define MIDI_CHANNEL_11     10
#define MIDI_CHANNEL_12     11
#define MIDI_CHANNEL_13     12
#define MIDI_CHANNEL_14     13
#define MIDI_CHANNEL_15     14
#define MIDI_CHANNEL_16     15

//MIDI播放Track轨道使能选择
#define M_TRACK_0           BIT(0)      //tempo map
#define M_TRACK_1           BIT(1)      //主轨道音符
#define M_TRACK_2           BIT(2)
#define M_TRACK_3           BIT(3)
#define M_TRACK_4           BIT(4)
#define M_TRACK_5           BIT(5)
#define M_TRACK_6           BIT(6)
#define M_TRACK_7           BIT(7)
#define M_TRACK_8           BIT(8)
#define M_TRACK_9           BIT(9)
#define M_TRACK_10          BIT(10)
#define M_TRACK_11          BIT(11)
#define M_TRACK_12          BIT(12)
#define M_TRACK_13          BIT(13)
#define M_TRACK_14          BIT(14)
#define M_TRACK_15          BIT(15)
#define M_TRACK_16          BIT(16)
//配置轨道使能 (MIDI1格式文件track0是tempo map,track1是主轨道音符)
#define OKON_TRACK_EN       (M_TRACK_0 | M_TRACK_1)     //设置okon的轨道

//MIDI播放channel使能选择
#define M_CH_1              BIT(0)      //channel 1
#define M_CH_2              BIT(1)      //channel 2
#define M_CH_3              BIT(2)
#define M_CH_4              BIT(3)
#define M_CH_5              BIT(4)
#define M_CH_6              BIT(5)
#define M_CH_7              BIT(6)
#define M_CH_8              BIT(7)
#define M_CH_9              BIT(8)
#define M_CH_10             BIT(9)      //drum channel(播放preset num大于128的乐器)
#define M_CH_11             BIT(10)
#define M_CH_12             BIT(11)
#define M_CH_13             BIT(12)
#define M_CH_14             BIT(13)
#define M_CH_15             BIT(14)
#define M_CH_16             BIT(15)
#define M_CH_ALL            0xFFFF      //All channel on
//配置通道使能
#define MIDI_CH_EN          (M_CH_ALL)  //设置MIDI播放使能的通道
#define OKON_CH_EN          (M_CH_6 | M_CH_10)    //设置okon的通道(设置的通道发音后经过okon_dtime时间会关音)

extern cbuf_t midi_ptr;

void midi_mode_set(midi_mode_e mode);
void set_midi_okon_next(u8 key_cnt);
void midi_decode_proc(msc_layer_e layer_num);
void midi_init(void);

u8 get_next_note_info(u8 *note, u8 *velocity, u8 *channel);
u8 get_next_note(void);
#endif
