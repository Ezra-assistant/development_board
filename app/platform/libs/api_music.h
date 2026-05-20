#ifndef _API_MUSIC_H
#define _API_MUSIC_H

#define SEEK_SET    0
#define SEEK_CUR    1

#define AVFMT_INVALID              0                        //音频格式无效
#define AVFMT_CORRECT              BIT(0)                   //音频格式有效
#define AVFMT_NOT_SUPPORT         (BIT(0) | BIT(1))         //音频格式有效，但超出解码能力

#define WAV_BUF_LEN                 192     //wav decode samples
#define WAV_PIT_LEN                 64      //变速变调固定输出样点数
#define ADPCM_READ_LEN              64
#define PEEK_SIZE                   44

#define WAV_BUF_SIZE                (WAV_BUF_LEN*3)

typedef enum{
    MSC_LAYER0,
    MSC_LAYER1,
    MSC_LAYER2,
    MSC_LAYER3,
    MSC_TOTAL_LAYER,
}msc_layer_e;

typedef enum {
    NONE_TYPE = 0,
    WAV_TYPE,
    MP3_TYPE,
    VSBC_TYPE,
    MIDI_TYPE,
    ESBC_TYPE,
    LSBC_TYPE,
} enum_msc_type_t;

enum {
    MUSIC_STOP = 0,
    MUSIC_PAUSE,
    MUSIC_PLAYING,
};

enum {
    //decode msg
    MUSIC_MSG_STOP = 0,
    MUSIC_MSG_PAUSE,
    MUSIC_MSG_PLAY,
    MUSIC_MSG_FRAME,

    //encode msg
    ENC_MSG_MP3 = 32,
    ENC_MSG_AEC,
    ENC_MSG_ALC,
    ENC_MSG_PLC,
    ENC_MSG_SBC,
    ENC_MSG_WAV,            //pcm wave
    ENC_MSG_ADPCM,          //adpcm-ima wave
    ENC_MSG_SPDIF,
    ENC_MSG_I2S,
};

typedef struct{
    u16 layer0_gain;
    u16 layer1_gain;
    u16 layer2_gain;
    u16 layer3_gain;
}music_layer_gain_t;

typedef struct {
    u8  sta;
    u8  type;
    u16 total_time;
    u16 cur_time_ms;
    u16 pcm_samples;
    u32 *pcm_buf;

    u8  layer0_busy  :1,
        layer1_busy  :1,
        layer2_busy  :1,
        layer3_busy  :1;
} music_cb_t;
extern music_cb_t music_cb;

//codec common info
typedef struct  {
    u16 start_skip_frame;
    u16 end_skip_frame;
    u32 total_frame;
    u32 frame_len;
    u32 frame_count;                //current frame count
    u32 file_ptr;                   //file ptr of current frame
    u16 samples;                    //sample pionts of frame
    u8  spr;                        //sample rate of enum number
    u32 sample_rate;                //sample rate
    u32 bitrate;                    //bit rate
} codec_info_t;
extern codec_info_t *codec_info;

typedef struct {
    u8 encrypt;
    u8 loop_cnt;                //0-不循环,1~0xFE循环次数,0xFF一直循环
    u8 lsbc_loop_reset;
    u16 crc_key;
    u8 *addr;
    u32 pos;
    u32 len;
    u8 sysclk;
    u32 start_skip_len;         //循环播放开头跳过长度
    u32 end_skip_len;           //循环播放结尾跳过长度
} spires_cb_t;

typedef struct{
    u32 *addr;
    u32 *len;
    u16 sta_skip_frame;     //开头跳过的帧数
    u16 end_skip_frame;     //末尾跳过的帧数
}msc_link_tbl_t;

#define  L_TOTAL      240
#define  L_WINDOW     240
#define  L_NEXT       40
#define  L_FRAME      80
#define  L_SUBFR      40
#define  M            10
#define  MP1          (M+1)
#define  PIT_MIN      20
#define  PIT_MAX      143
#define  L_INTERPOL   (10+1)
#define  GAMMA1       24576
#define  PRM_SIZE     11
#define  SERIAL_SIZE  (10)
#define  SHARPMAX     13017
#define  SHARPMIN     3277

#define   NC            5
#define   MA_NP         4
#define   MODE          2
#define   NC0_B         7
#define   NC1_B         5
#define   NC0           (1<<NC0_B)
#define   NC1           (1<<NC1_B)

#define   L_LIMIT          40
#define   M_LIMIT       25681

#define   GAP1          10
#define   GAP2          5
#define   GAP3          321
#define GRID_POINTS     50

#define PI04      ((s16)1029) 
#define PI92      ((s16)23677)
#define CONST10   ((s16)10*(1<<11))
#define CONST12   ((s16)19661)

#define L_H         22
#define GAMMAP      16384
#define INV_GAMMAP  21845
#define GAMMAP_2    10923
#define GAMMA2_PST  18022
#define GAMMA1_PST  22938
#define MU          26214
#define AGC_FAC     29491
#define AGC_FAC1 (s16)(32767 - AGC_FAC)

typedef struct {
    s16 y2_hi;
    s16 y2_lo;
    s16 y1_hi;
    s16 y1_lo;
    s16 x0;
    s16 x1;
} post_hpf_cb_t;

typedef struct
{
    u8  sta;
    s16 bad_lsf;
    s16 synth_buf[L_FRAME+M];
    s16 *synth;
    u8  serial[L_FRAME+3];
    s16 Az_dec[MP1*2];

    s16 *exc;
    s16 sharp;
    s16 old_T0;
    s16 gain_code;
    s16 gain_pitch;
    s16 lsp_old[M];
    s16 lsp_new[M];
    s16 mem_syn[M];
    s16 old_exc[L_FRAME+PIT_MAX+L_INTERPOL];
    s16 acelp_code[L_SUBFR];

    s16 past_qua_en_dec[4];

    s16 freq_prev[MA_NP][M];
    s16 prev_lsp[M];
    s16 prev_ma;

    s16 res2_buf[PIT_MAX+L_SUBFR];
    s16 scal_res2_buf[PIT_MAX+L_SUBFR];
    s16 *res2;
    s16 *scal_res2;

    s16 mem_syn_pst[M];

    s16 res2_pst[L_SUBFR];
    s16 syn_pst[L_FRAME];
    s16 Ap3[MP1];
    s16 Ap4[MP1];
    s16 pf_h[L_H];
    s16 mem_pre;
    s16 past_gain;

    post_hpf_cb_t post_hpf_cb;
}esbc_dec_cb_t;

typedef struct {
    u32 input_len;
    u32 output_len;
    u32 quality;
} mav_init_t;

typedef struct AVIOContext {
    unsigned char *buffer;
    int buffer_size;
    unsigned char *buf_ptr;
    unsigned char *buf_end;
    unsigned int peek_size;
    int (*read_packet)(void *buf, unsigned int buf_size);
    bool (*seek)(unsigned int offset, int whence);
    unsigned int pos;
    int eof_reached;
    int error;
} AVIOContext;

typedef struct _WAVContext {
    AVIOContext pb;
    codec_info_t info;

    ///fmt Chunk
    u16 fmt_tag;
    u16 nchannel;
    u32 sample_rate;
    u32 bit_rate;
    u16 block_align;
    u16 bits_per_sample;
    u8  extensible_flag;

    ///data Chunk
    u32 data_size;
    u32 data_saddr;                 //data start address
    u32 data_pos;
    u32 skip;                       //sample rate > 48K, skip samples count

    //ima_adpcm
    struct {
        u8      adpcm_end;
        u8      loop_flag;
        short   step_index[2];
        short   predictor[2];
        uint    nb_samples;           //samples from one block,  nb_samples=(block_align-4*channels)*2/channels
        short   *pcm_buf;             //Temporary saved 8 sample piont
        uint8_t bit_len[2];           //residue length
        uint    bit_buf[2];           //bit buf
    } adpcm;

} WAVContext;

typedef struct {
    u8  wav_start;
    u16 bdec_num;                   //block decode samples number
    u16 samples_cnt;                //decode sample count
    
    u8 *ptr;
    u8 sta;
    u16 ret;                        //记录保存的数据量
    u16 spr_fix_val;

    u8 data_rbuf[ADPCM_READ_LEN];
} wav_dec_cb_t;

typedef struct{
    msc_layer_e wav_layer;
    WAVContext wav_ctx;
    wav_dec_cb_t wav_dec;
    u8  wav_avio_buf[WAV_BUF_SIZE + PEEK_SIZE];
    u16 adpcm_buf[16];
    u8  adpcm_obuf[384];
}wav_layer_t;

int wav_decode_init(void);
int mp3_decode_init(void);
int spi_mp3_decode_init(void);
int music_decode_init(void);
void music_control(u8 sta);
bool music_decode(void);
u8 get_music_dec_sta(void);
void set_music_dec_sta(u8 sta);
void music_pcm_gpdma_kick(void);

u16 music_get_total_time(void);                 //获取音乐播放总时长
u16 music_get_cur_time(void);                   //获取当前音乐播放的时间(单位ms)
void music_set_cur_time(u32 cur_time);          //设置当前音乐播放的时间
void music_set_jump(void *brkpt);               //跳转到音乐播放断点
void music_get_breakpiont(void *brkpt);         //获取音乐播放断点
void music_qskip(bool direct, u8 second);       //快进快退控制 direct = 1为快退，0为快进
void music_qskip_end(void);                     //结束快进快退，继续播放

u8 get_layer0_busy(void);                       //获取music layer0层busy标志
u8 get_layer1_busy(void);                       //获取music layer1层busy标志

bool mp3_res_is_play(void);
bool mp3_dec_frame(void);
bool mp3_res_play_kick(u32 addr, u32 len, u8 loop_cnt);
bool mp3_res_loop_play_kick(u32 addr, u32 len, u32 skip_start_bytes, u32 skip_end_bytes, bool loop_en);     //支持循环播放，头尾跳帧播放
void mp3_res_play_exit(void);
void mp3_dec_stop(void);

bool vmp3_res_play_kick(u32 addr, u32 len, u8 loop_cnt);
bool vmp3_res_loop_play_kick(u32 addr, u32 len, u32 skip_start_bytes, u32 skip_end_bytes, bool loop_en);    //支持循环播放，头尾跳帧播放
void vmp3_res_play_exit(void);

bool wav_res_is_play(wav_layer_t *wav_layer);
bool wav_res_decode(u16 speed, u16 pitch, wav_layer_t *wav_layer);
bool wav_res_play_kick(u32 addr, u32 len, wav_layer_t *wav_layer, u8 loop_cnt);
void wav_res_play_exit(void);
void wav_dec_stop(wav_layer_t *wav_layer);
void exspiflash_wav_num_kick(msc_layer_e layer_num, u32 num);

void wav_res_play_loop_back(u8 enable);             //wav循环播放使能

bool wav_2_res_is_play(void);
bool wav_2_res_decode(u16 speed, u8 pitch);
bool wav_2_res_play_kick(u32 addr, u32 len);
void wav_2_dec_stop(void);

void esbc_res_play_kick(u32 addr, u32 len, esbc_dec_cb_t *esbc_d_vars, u8 loop_cnt);
void esbc_res_play_exit(void);
void esbc_link_play_init(void);
void esbc_link_play_kick(u32 addr, u32 len, u16 sta_skip_frame, u16 end_skip_frame, esbc_dec_cb_t *esbc_d_vars);
bool esbc_res_is_play(esbc_dec_cb_t *esbc_d_vars);
void esbc_res_start(esbc_dec_cb_t *esbc_d_vars);
void esbc_res_stop(esbc_dec_cb_t *esbc_d_vars);

void lsbc_res_play_exit(void);
void lsbc_res_play_kick(u32 addr, u32 len, u8 loop_cnt);
bool lsbc_dec_frame(void);
void lsbc_dec_pcm_out_cb_do(void);
bool lsbc_res_loop_play_kick(u32 addr, u32 len, u16 sta_skip_frame, u16 end_skip_frame, bool loop_en);
u32 get_lsbc_total_frame(void);
u32 break_point_calculate(u32 pos);

void lsbc_dec_pause_do(void);
void mp3_dec_pause_do(void);
void wav_dec_pause_do(wav_layer_t *wav_layer);
void wav_2_dec_pause_do(void);
void esbc_dec_pause_do(void);
void midi_dec_pause_do(void);

void mp3_link_play_init_do(void);
void vmp3_link_play_init_do(void);

bool mp3_link_play_kick(u32 addr, u32 len, u8 loop_cnt, u8 link_num);

/*--------------------------------------
@brief:     layer0淡入淡出参数设置
@fade_in_frame_num:     layer0淡入帧长设置
@fade_out_frame_num:    layer0淡出帧长设置
@fade_out_tick:         layer0淡出tick时间设置(设置此参数,控制音频播放多少ms后启动淡出,并以fade_out_frame_num为淡出长度计算基准)
@fade_out_type:         layer0淡出方式,0-从音频开始计算fade_out_tick时间后开始淡出,1-从音频剩余fade_out_frame_num帧长时开始淡出
@fade_out_type:         layer0淡出方式为1时仅支持wav和mp3音频
--------------------------------------*/
void set_layer0_fade(u16 fade_in_frame_num, u16 fade_out_frame_num, u32 fade_out_tick, u8 fade_out_type);
/*--------------------------------------
@brief:     layer1淡入淡出参数设置
@fade_in_frame_num:     layer1淡入帧长设置
@fade_out_frame_num:    layer1淡出帧长设置
@fade_out_tick:         layer1淡出tick时间设置(设置此参数,控制音频播放多少ms后启动淡出,并以fade_out_frame_num为淡出长度计算基准)
@fade_out_type:         layer1淡出方式,0-从音频开始计算fade_out_tick时间后开始淡出,1-从音频剩余fade_out_frame_num帧长时开始淡出
@fade_out_type:         layer1淡出方式为1时仅支持wav和mp3音频
--------------------------------------*/
void set_layer1_fade(u16 fade_in_frame_num, u16 fade_out_frame_num, u32 fade_out_tick, u8 fade_out_type);

void wav_pitch_up_init(void);

void res_play_loop_back(u8 loop_cnt);                 //spiflash循环播放接口
void res_play_loop_back_skip_size(u16 skip_star_size, u16 skip_end_size); //外接FLASH音乐循环播放mp3开关,设置循环时候跳过前后多少字节音频数据，用于部分应用避开静音区数
void spiflash_rec_mp3_filelen_reduce(u16 ms);       //剪掉REC尾部 ms(spiflash)

u32 spires_get_pos(void);
void spires_load_pos(u32 pos);

bool dac_set_eq_by_res(u32 *addr, u32 *len);
void set_dac_eq_spr(u8 spr);
u8 get_dac_eq_spr(void);
#endif // _API_MUSIC_H
