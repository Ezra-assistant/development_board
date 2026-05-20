#ifndef _BSP_MUSIC_H
#define _BSP_MUSIC_H

#define MSC_MI_NONE     0
#define MSC_MI_MP3      BIT(1)
#define MSC_MI_VMP3     BIT(2)
#define MSC_MI_LSBC     BIT(3)
#define MSC_MI_ESBC     BIT(4)
#define MSC_MI_MIDI     BIT(5)
#define MSC_MI_VMIDI    BIT(6)
#define MSC_MI_WAV      BIT(7)
#define MSC_MI_WAV_2    BIT(8)

#define MSC_WAV_LAYER1      BIT(0)      //占用data空间约为896byte
#define MSC_WAV_LAYER2      BIT(1)      //占用data空间约为896byte
#define MSC_WAV_LAYER3      BIT(2)      //占用data空间约为896byte
#define MSC_WAV_SEL         (MSC_WAV_LAYER1)


typedef struct{
    u8 link_num;            //拼接总数
    u8 cur_num;             //当前播放
    u8 kick_start;          //下一曲使能
}msc_link_sta_t;

#if MSC_MP3_LINK_EN || MSC_VMP3_LINK_EN//mp3拼接播放测试示例
#define MSC_MP3_VMP3_LINK1_NUM   3   //拼接播放音频个数
extern msc_link_tbl_t msc_mp3_vmp3_link1[MSC_MP3_VMP3_LINK1_NUM];
#endif

typedef enum{
    B_MP3,
    B_VMP3,
    B_LSBC,
    B_ESBC,
    B_MIDI,
    B_VMIDI,
    B_WAV,
    MSC_TOTAL_TYPE,
}msc_type_e;

typedef enum {
    MSC_NONE,
    MSC_MP3     = BIT(B_MP3),
    MSC_VMP3    = BIT(B_VMP3),
    MSC_LSBC    = BIT(B_LSBC),
    MSC_ESBC    = BIT(B_ESBC),
    MSC_MIDI    = BIT(B_MIDI),
    MSC_VMIDI   = BIT(B_VMIDI),
    MSC_WAV     = BIT(B_WAV),
    MSC_TOTAL,
} msc_mode_e;

typedef enum{
    LAYER_STOP,
    LAYER_PAUSE,
    LAYER_PLAYING,
}layer_sta_e;

typedef struct{
    layer_sta_e layer_sta[MSC_TOTAL_LAYER];
    msc_mode_e layer_mode[MSC_TOTAL_LAYER];
    // void (*song_play[MSC_TOTAL_TYPE])(u32 addr, u32 len);
    // void (*song_play_end[MSC_TOTAL_TYPE])(void);
}msc_layer_t;

typedef struct {
    msc_mode_e msc_mode;
    u8 wav_speed_val;
    u8 wav_pitch_val;
#if MUSIC_MODE_TABLE_EN
    u8 msc_auto_en;                 /* 是否自动切换下一播放模式 */
    s8 msc_playing_cnt;             /* 当前正在播的音频数量 */
    s8 msc_played_cnt;              /* 已经播完的音频数量 */
    u16 msc_index;                  /* 当前播放模式的索引 */
    u16 msc_index_back;             /* 当前播放模式的备份索引 */
    u16 msc_max;                    /* 一共多少种播放模式 */
#endif
} msc_cb_t;

typedef enum {
    MSC_OPT_NONE = 0,               /* 仅用作占位 */
    MSC_OPT_PLAY = BIT(0),          /* 是否播放 */
    MSC_OPT_LOOP = BIT(1),          /* 是否无缝循环播放 */
} msc_opt_e;                        /* 播放模式针对每一种格式的音频的选项 */

typedef struct {
    u32 *ptr;                       /* 音频地址指针 */
    u32 *len;                       /* 音频长度指针 */
    u8 mode;                        /* 音频格式类型，见MSC_MI_NONE */
    msc_opt_e  opt;                 /* 当前音频播放选项 */
} msc_opt_t;

typedef struct {
    msc_opt_t o[MSC_TOTAL_LAYER];   /* 当前模式播放的多个音频的信息 */
    u8 played_cnt_next;             /* 当播完多少音频后切换到下一模式 */
} msc_mode_tbl_t;

typedef struct {
    void (*song_play[MSC_TOTAL_TYPE])(u32 addr, u32 len);
    void (*song_play_end[MSC_TOTAL_TYPE])(void);
    void (*song_loop[MSC_TOTAL_TYPE])(u8 enable);
} msc_func_tbl_t;

typedef struct {
    u32 lay0_tick;
    u32 lay1_tick;
    u8 lay0_s_flag;
    u8 lay1_s_flag;
    u32 addr;
    u32 len;
} break_fade_t;

typedef struct {
    u8 dnr_flag;
    u8 dnr_cnt;
    u8 fade_sta;
} dac_dnr_t;
extern dac_dnr_t dac_dnr;

extern u8 wav_speedup_val;
extern u8 wav_pitchup_val;

extern wav_layer_t wav_layer1;

uint calc_crc(void *buf, uint len, uint seed);
bool mp3_dec_frame(void);
bool wav_dec_frame(void);
bool wav_res_decode(u16 speed, u16 pitch, wav_layer_t *wav_layer);
bool esbc_decode_frame(esbc_dec_cb_t *esbc_d_vars);
bool drc_v3_init(const void *bin, int bin_size);

void music_mode_table_check(void);

void music_res_init(void);                                          //音乐播放公共初始化
void music_mode_enter(msc_layer_e layer_num, msc_mode_e mode);      //进入音乐播放公共
void music_mode_exit(msc_layer_e layer_num, msc_mode_e mode);       //退出音乐播放公共
bool music_layer_mode_set(msc_layer_e layer_num, msc_mode_e mode);
bool music_layer_mode_clr(msc_layer_e layer_num);
u16 get_msc_layer_mode(u8 layer_num);
layer_sta_e music_layer_play_check_all(void);                       //检查所有layer层是否有音频在播
u8 layer_mode_check(msc_layer_e layer_num, msc_mode_e mode);        //检查某layer层某格式音频是否在播
layer_sta_e layer_play_check(msc_layer_e layer_num);                //检查某layer层是否有音频在播
void music_layer_sta_set(msc_layer_e layer_num, layer_sta_e layer_sta);  //控制song播放某layer层的播放暂停
layer_sta_e music_layer_sta_get(msc_layer_e layer_num);             //获取某layer层播放状态

void mp3_res_play_do(u32 addr, u32 len, bool sync, u8 loop_cnt);
void vmp3_res_play_do(u32 addr, u32 len, bool sync, u8 loop_cnt);
void wav_res_play_do(u32 addr, u32 len, bool sync, u8 loop_cnt);
void esbc_res_play_do(u32 addr, u32 len, bool sync, u8 loop_cnt);

void mp3_res_play(u32 addr, u32 len, u8 loop_cnt);                               //提示音独占播放:mp3播放接口
void vmp3_res_play(u32 addr, u32 len, u8 loop_cnt);                              //提示音独占播放:vmp3播放接口
void wav_res_play(u32 addr, u32 len, u8 loop_cnt);                               //提示音独占播放:wav播放接口
void esbc_res_play(u32 addr, u32 len, u8 loop_cnt);                              //提示音独占播放:esbc播放接口
void lsbc_res_play(u32 addr, u32 len, u8 loop_cnt);                              //提示音独占播放:lsbc播放接口

void mp3_song_play(u32 addr, u32 len, u8 loop_cnt);                              //后台解码播放:mp3播放接口
void vmp3_song_play(u32 addr, u32 len, u8 loop_cnt);                             //后台解码播放:vmp3播放接口
void wav_song_play(msc_layer_e layer_num, u32 addr, u32 len, u8 loop_cnt);       //后台解码播放:wav播放接口(MSC_LAYER0/MSC_LAYER1均可播放)
void esbc_song_play(u32 addr, u32 len, u8 loop_cnt);                             //后台解码播放:esbc播放接口
void lsbc_song_play(u32 addr, u32 len, u8 loop_cnt);                             //提示音独占播放:lsbc播放接口

void esbc_link_play_do(const msc_link_tbl_t *elink, const u8 e_num, esbc_dec_cb_t *esbc_d_vars);

void mp3_song_play_end(void);                                       //mp3/vmp3播放结束回调接口
void wav_song_play_end(msc_layer_e layer_num);                      //wav/adpcm播放结束回调接口
void esbc_song_play_end(void);                                      //esbc播放结束回调接口
void midi_song_play_end(void);                                      //midi播放结束回调接口
void lsbc_song_play_end(void);                                      //lsbc播放结束回调接口

void music_res_pause(u32 addr, u32 len, void *res_play_func);
void music_res_continue(void);
void music_decode_process(void);

u32 midi_break_point_restore(u32 addr);

void mp3_song_link_init(u8 link_num);
void mp3_song_link_disable(void);
void mp3_song_link_play(const msc_link_tbl_t *elink, u8 loop_cnt);

void set_lay0_fade_out_start(u8 start_en);
void set_lay1_fade_out_start(u8 start_en);
void break_play_lay0_fade_start(void);
void break_play_lay1_fade_start(void);
void break_play_fade_process(void);

void music_cb_reset(void);

void dac_dnr_detect();
void dac_dnr_init();
void set_dac_dnr_sta(u8 sta);
u8 get_dac_dnr_sta(void);
void dac_dnr_detect(void);
#endif // _BSP_MUSIC_H
