#ifndef _API_MIDI_H
#define _API_MIDI_H

// GEN_FLOAT_LIMIT12K5K msf_timecents2Secsf 0.0f ~ 18.0f
#define T12_5           uint16_t
#define T12_5_SHF       11
#define T12_5_MUL       (1<<T12_5_SHF)
// GEN_FLOAT_LIMIT12K8K msf_timecents2Secsf 0.0f ~ 101.6f
#define T12_8           uint16_t
#define T12_8_SHF       9
#define T12_8_MUL       (1<<T12_8_SHF)
// MAX1000、MAX1440 sustain 1.0f ~ 0.0f
#define T_SUS           uint16_t
#define T_SUS_SHF       15
#define T_SUS_MUL       (1<<T_SUS_SHF)

typedef enum{
    SMPL_8000,
    SMPL_12000,
    SMPL_16000,
    SMPL_22050,
    SMPL_24000,
    SMPL_32000,
    SMPL_44100,
    SMPL_48000,
}midi_smpl_rate_e;

typedef enum{
    MIDI_STOP,
    MIDI_PAUSE,
    MIDI_PLAYING,
}midi_sta_e;

typedef enum{
    MIDI_MODE_NORM,     //正常解码模式
    MIDI_MODE_OKON,     //one key one note模式
    MIDI_MODE_MUTE,     //静音当前设置的音轨的声音
}midi_mode_e;

typedef enum{
    OKON_MODE_A,        //okon A:按下播放下一次delta time内的音符(已使能的track/channel okon,其余旋律不播放)
    OKON_MODE_B,        //okon B:按下播放下一个音符(已使能的track/channel okon,其余旋律不播放)
    OKON_MODE_C,        //okon C:按下播放下一个音符(已使能的track/channel okon,其余旋律正常播放)
}okon_mode_e;

// 下一个音符信息结构体
typedef struct {
    u8 note;        // 音符值 (0-127)
    u8 velocity;    // 力度 (0-127)
    u8 channel;     // 通道 (0-15)
    u8 valid;       // 是否有效 (1=有效, 0=无效/已播完)
} next_note_info_t;

typedef struct{
    okon_mode_e okon_mode;
    u32 okon_outspl_cnt;            //统计okon轨道的当前输出采样点
    u8  okon_dtime;                 //note发音时长设置参数
    u8  okon_key_cnt;               //按键次数
    u8  sub_start;                  //副曲启动使能
    next_note_info_t next_note;     //下一个音符信息
    u32 okon_tick;                  //当前note key已播放时间
}midi_okon_t;

typedef struct{
    u8 voice_num;
    u8 track_num;
    u8 channel_num;
    u8 smpl_rate;       //midi sample rate select,midi_smpl_rate_e
    u8 midi_mode;       //midi_mode_e
    u8 loop_cnt;
}midi_cfg_t;

/*--------------------------------------
@brief:     发送一个开/关音码(9n kk vv/8n kk vv)
@bynote:    开音码/关音码(n为midi通道0~16)
@bynum:     音符号9n/8n(kk为音高0~127)
@byvel:     力度(vv为音量0~127)
--------------------------------------*/
void mkey_note_onoff(u8 bynote, u8 bynum, u8 byvel);
/*--------------------------------------
@brief:     发送一个控制器码Control change(An ss cc/Bn ss cc)
@bynote:    控制器码An/Bn(n为midi通道0~16)
@bynum:     控制器号(ss为0~127)
@byvel:     控制值(cc为0~127)
--------------------------------------*/
void mkey_ctrl_change(u8 bynote, u8 bynum, u8 byvel);
/*--------------------------------------
@brief:     发送一个音色码Program change(Cn pp)
@bynote:    音色码Cn(n为midi通道0~16)
@byvel:     音色编号(pp为0~127)
--------------------------------------*/
void mkey_prog_change(u8 bynote, u8 byvel);
/*--------------------------------------
@brief:     发送一个通道压力码Channel After Touch(Dn vv)
@bynote:    通道码Dn(n为midi通道0~16)
@byvel:     压力值(pp为0~127)
--------------------------------------*/
void mkey_chan_at(u8 bynote, u8 byvel);
/*--------------------------------------
@brief:     发送一个弯音码Pitch bend(En bL bH)
@bynote:    弯音码En(n为midi通道0~16)
@vel_l:     弯音值低位(bL为0~127)
@vel_h:     弯音值高位(bH为0~127)
--------------------------------------*/
void mkey_pitch_bend(u8 bynote, u8 vel_l, u8 vel_h);

u32 get_midi_tick(void);                                        //获取当前播放midi/vmidi的midi tick
void set_midi_tick(u32 midi_tick_val);                          //设置当前播放midi/vmidi的midi tick

u8 midi_decode(void);                                           //midi解码接口
u32 midi_fifo_decode(cbuf_t *mbuf);                             //midi fifo解码

u8 get_midi_dec_sta(void);                                      //获取当前midi解码状态(状态:midi_sta_e)
void midi_control(u8 sta);                                      //控制midi状态接口
void midi_voice_kill(void);                                     //清除当前正在发音的midi

void vmidi_res_play_kick(u32 midi_addr, u32 midi_len);          //vmidi提示音kick接口
void vmidi_res_play_exit(void);                                 //vmidi提示音exit接口

void vmidi_song_play_kick(u32 midi_addr, u32 midi_len, u8 loop_cnt); //vmidi歌曲kick接口(非独占播放)
void vmidi_song_play_exit(void);                                //vmidi歌曲exit接口(非独占播放)

void midi_song_play_kick(u32 midi_addr, u32 midi_len, u8 loop_cnt);  //标准midi kick接口
void midi_song_play_exit(void);                                 //标准midi exit接口
u32 get_midi_track_stat(void);                                  //获取标准midi轨道Track播放使能位
void set_midi_track_stat(u32 track_stat);                       //设置标准midi轨道Track播放使能位
u32 get_midi_channel_stat(void);                                //获取标准midi通道channel播放使能位
void set_midi_channel_stat(u32 ch_stat);                        //设置标准midi通道Channel播放使能位

void midi_cfg_init(midi_cfg_t *m_cfg);                          //MIDI参数配置设置
void midi_okon_init(midi_okon_t *o_cfg);                        //MIDI OKON参数初始化
void midi_synth_init(u32 voice_addr, u32 voice_len);            //发音合成器初始化
#endif
