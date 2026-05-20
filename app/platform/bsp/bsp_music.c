#include "include.h"

#define FS_CRC_SEED         0xffff

extern u32 __aram_msc_comm_vma, __aram_msc_comm_lma, __aram_msc_comm_size;
extern break_fade_t break_play_fade;

msc_cb_t msc_cb AT(.music_buf);
msc_layer_t msc_layer AT(.music_buf);
music_layer_gain_t msc_gain AT(.music_buf);

#if ((MUISC_WAV_LAYER_EN & MSC_WAV_LAYER1) == (MSC_WAV_LAYER1))
wav_layer_t wav_layer1 AT(.wav_buf);
#endif
#if ((MUISC_WAV_LAYER_EN & MSC_WAV_LAYER2) == (MSC_WAV_LAYER2))
wav_layer_t wav_layer2 AT(.wav_buf);
#endif
#if ((MUISC_WAV_LAYER_EN & MSC_WAV_LAYER3) == (MSC_WAV_LAYER3))
wav_layer_t wav_layer3 AT(.wav_buf);
#endif

wav_layer_t *wav_layer_tbl[3] = {
#if ((MUISC_WAV_LAYER_EN & MSC_WAV_LAYER1) == (MSC_WAV_LAYER1))
&wav_layer1,
#endif
#if ((MUISC_WAV_LAYER_EN & MSC_WAV_LAYER2) == (MSC_WAV_LAYER2))
&wav_layer2,
#endif
#if ((MUISC_WAV_LAYER_EN & MSC_WAV_LAYER3) == (MSC_WAV_LAYER3))
&wav_layer3,
#endif
};

#if (WARNING_ESBC_PLAY || EXSPI_ESBC_EN)
esbc_dec_cb_t esbc_dec_cb AT(.esbc_buf);
#endif

#if WARNING_TONE_EN
u16 spi_mp3_encrypt_key = SPI_MP3_ENCRYPT_KEY;

u8 wav_speedup_val = WAV_SPEEDUP_VAL;
u8 wav_pitchup_val = WAV_PITCHUP_VAL;
#endif //WARNING_TONE_EN

#if MSC_MP3_LINK_EN || MSC_VMP3_LINK_EN
msc_link_sta_t mp3_vmp3_link_sta AT(.music_buf);
#endif

#if MUSIC_MODE_TABLE_EN
const msc_func_tbl_t msc_func = {
    {
        [MSC_MI_MP3] = mp3_song_play,
        [MSC_MI_VMP3] = vmp3_song_play,
        [MSC_MI_WAV] = wav_song_play,
        [MSC_MI_ESBC] = esbc_song_play,
#if MIDI_EN
        [MSC_MI_MIDI] = midi_song_play,
        [MSC_MI_VMIDI] = vmidi_song_play,
#endif
    },
    {
        [MSC_MI_MP3] = mp3_song_play_end,
        [MSC_MI_VMP3] = mp3_song_play_end,
        [MSC_MI_WAV] = wav_song_play_end,
        [MSC_MI_ESBC] = esbc_song_play_end,
#if MIDI_EN
        [MSC_MI_MIDI] = midi_song_play_end,
        [MSC_MI_VMIDI] = midi_song_play_end,
#endif
    },
    {
        [MSC_MI_WAV] = wav_res_play_loop_back, /* v1.0目前只有wav支持无缝循环播放 */
    },
};

const msc_mode_tbl_t msc_mode_tbl[] = {
 /*
  * v1.0
  * 目前只支持MSC_MAX_MERGE_CNT=2，即同时播放两种格式的音频，且只支持[wav/adpcm]和[mp3/vmp3/midi/vmidi/wav_2]的叠加播放
  * 需要先放[mp3/vmp3/midi/vmidi/wav_2]的地址和长度等信息，再放[wav/adpcm]的地址和长度等信息
  * { { { [mp3/vmp3/midi/vmidi/wav_2]地址,  [mp3/vmp3/midi/vmidi/wav_2]长度,音频格式类型,   音频播放选项 },
  *     { [wav/adpcm]地址,                  [wav/adpcm]长度,                音频格式类型,   音频播放选项 }, }, 播放完多少种音频之后切到下一个模式 },
  */
    { { { &RES_BUF_EN_NUM17_WAV,            &RES_LEN_EN_NUM17_WAV,          MSC_MI_WAV_2,   MSC_OPT_PLAY | MSC_OPT_LOOP },
        { &RES_BUF_EN_MAX_VOL_WAV,          &RES_LEN_EN_MAX_VOL_WAV,        MSC_MI_WAV,     MSC_OPT_PLAY }, }, 2 },
    { { { &RES_BUF_EN_POWEROFF_MP3,         &RES_LEN_EN_POWEROFF_MP3,       MSC_MI_MP3,     MSC_OPT_NONE },
        { &RES_BUF_EN_MAX_VOL_WAV,          &RES_LEN_EN_MAX_VOL_WAV,        MSC_MI_WAV,     MSC_OPT_PLAY }, }, 1 },
    { { { &RES_BUF_EN_POWERON_MP3,          &RES_LEN_EN_POWERON_MP3,        MSC_MI_MP3,     MSC_OPT_PLAY },
        { &RES_BUF_EN_MAX_VOL_WAV,          &RES_LEN_EN_MAX_VOL_WAV,        MSC_MI_WAV,     MSC_OPT_NONE }, }, 1 },
    { { { &RES_BUF_EN_POWEROFF_MP3,         &RES_LEN_EN_POWEROFF_MP3,       MSC_MI_MP3,     MSC_OPT_PLAY },
        { &RES_BUF_EN_MAX_VOL_WAV,          &RES_LEN_EN_MAX_VOL_WAV,        MSC_MI_WAV,     MSC_OPT_PLAY }, }, 2 },
#if MIDI_EN
    { { { &RES_BUF_MID_DEMO_MIDI1_VMID,     &RES_LEN_MID_DEMO_MIDI1_VMID,   MSC_MI_VMIDI,   MSC_OPT_PLAY },
        { &RES_BUF_EN_MAX_VOL_WAV,          &RES_LEN_EN_MAX_VOL_WAV,        MSC_MI_WAV,     MSC_OPT_PLAY }, }, 2 },
    { { { &RES_BUF_MID_DEMO_MIDI1_VMID,     &RES_LEN_MID_DEMO_MIDI1_VMID,   MSC_MI_VMIDI,   MSC_OPT_PLAY },
        { &RES_BUF_EN_MAX_VOL_WAV,          &RES_LEN_EN_MAX_VOL_WAV,        MSC_MI_WAV,     MSC_OPT_PLAY | MSC_OPT_LOOP }, }, 1 },
#endif
};

void music_mode_inc(void)
{
    msc_cb.msc_index++;
    if (msc_cb.msc_index >= msc_cb.msc_max) {
        msc_cb.msc_index = 0;
    }
}

void music_mode_auto_en(u8 enable)
{
    msc_cb.msc_auto_en = enable;
}

void music_mode_table_check(void)
{
    msc_mode_tbl_t tbl = msc_mode_tbl[msc_cb.msc_index];
    if (msc_cb.msc_auto_en && msc_cb.msc_index_back != msc_cb.msc_index) { /* 是否已切换到下一个模式 */
        tbl = msc_mode_tbl[msc_cb.msc_index];
        msc_cb.msc_index_back = msc_cb.msc_index;
        msc_cb.msc_playing_cnt = 0;
        msc_cb.msc_played_cnt = 0;
        printf("======================msc_cb.msc_index: %d\n", msc_cb.msc_index);
        for (int i=0; i<MSC_TOTAL_LAYER; i++) {
            msc_opt_e opt = tbl.o[i].opt;
            if (!(opt & MSC_OPT_PLAY)) {
                continue; /* 只有当选项里或上MSC_OPT_PLAY才会去播放 */
            }
            u8 mode = tbl.o[i].mode;
            if (opt & MSC_OPT_LOOP && msc_func.song_loop[mode]) {
                msc_func.song_loop[mode](1); /* 选项满足MSC_OPT_LOOP时才无缝循环播放 */
            }
            if (msc_func.song_play[mode]) {
                msc_func.song_play[mode](*tbl.o[i].ptr, *tbl.o[i].len); /* 播放当前音频 */
                msc_cb.msc_playing_cnt++;
            }
        }
    }
    if (msc_cb.msc_played_cnt >= tbl.played_cnt_next || msc_cb.msc_playing_cnt <= 0) { /* 是否满足切换到下一个模式的条件：播完所有音频，或播完指定数量的音频 */
        for (int i=0; i<MSC_TOTAL_LAYER; i++) { /* 确保退出当前模式所有播放 */
            msc_opt_e opt = tbl.o[i].opt;
            u8 mode = tbl.o[i].mode;
            if (opt & MSC_OPT_LOOP && msc_func.song_loop[mode]) {
                msc_func.song_loop[mode](0);
            }
            if (opt & MSC_OPT_PLAY && msc_func.song_play_end[mode]) {
                msc_func.song_play_end[mode]();
            }
        }
        music_mode_inc(); /* 切换到下一模式 */
    }
}
#endif

//音乐播放公共初始化
void music_res_init(void)
{
    memcpy(&__aram_msc_comm_vma, &__aram_msc_comm_lma, (u32)&__aram_msc_comm_size);
    memset(&msc_cb, 0, sizeof(msc_cb_t));
    memset(&msc_layer, 0, sizeof(msc_layer_t));
    memset(&msc_gain, 0, sizeof(music_layer_gain_t));

    msc_layer.layer_mode[MSC_LAYER0] = 0;
    msc_layer.layer_mode[MSC_LAYER1] = 0;
    msc_gain.layer0_gain = DIG_N0DB;
    msc_gain.layer1_gain = DIG_N0DB;
    msc_gain.layer2_gain = DIG_N0DB;
    msc_gain.layer3_gain = DIG_N0DB;

#if MUSIC_MODE_TABLE_EN
    msc_cb.msc_max = sizeof(msc_mode_tbl) / sizeof(msc_mode_tbl[0]);
    printf("msc_cb.msc_max: %d\n", msc_cb.msc_max);
    msc_cb.msc_index_back = 0xffff;
    msc_cb.msc_auto_en = 1;
#endif
#if MSC_MP3_LINK_EN
    mp3_song_link_disable();
#endif
    dac_gpdma_reset();
}

//进入音乐播放公共
void music_mode_enter(msc_layer_e layer_num, msc_mode_e mode)
{
#if MSC_FADE_INOUT_EN
    dac_fade_in(6);
#endif
    if (!music_layer_play_check_all()) {
#if DAC_ALG_DC_OFF_EN
        #if !TKEY_CLASSD_FIX
        dac_analg_on();             //打开dac模拟
        #endif
#endif
        dac_gpdma_reset();
    }
    switch(mode){
        case MSC_LSBC:
            #if (WARNING_LSBC_PLAY || EXSPI_LSBC_EN)
            if(MSC_LAYER0 == layer_num){
                msc_gain.layer0_gain = DIG_N0DB;
                set_layer0_fade(0, 0, 0, 0);       //lsbc软件淡入淡出
            }
            #endif
            break;
        case MSC_WAV:
            if(MSC_LAYER1 == layer_num){
                #if (WARNING_WAV_PLAY || EXSPI_WAV_EN)
                set_layer1_fade(3, 0, 0, 1);
                #endif
                msc_gain.layer1_gain = DIG_N0DB;
            }else if(MSC_LAYER2 == layer_num){
                msc_gain.layer2_gain = DIG_N0DB;
            }else if(MSC_LAYER3 == layer_num){
                msc_gain.layer3_gain = DIG_N0DB;
            }
            break;
        case MSC_MIDI:
        case MSC_VMIDI:
            #if MIDI_EN
            msc_gain.layer0_gain = DIG_N0DB;
            set_layer0_fade(0, 0, 0, 0);          //midi软件淡入淡出
            if(get_msc_layer_mode(MSC_LAYER0) != MSC_MIDI){
                midi_init();
            }
            #endif
            break;
        case MSC_MP3:
        case MSC_VMP3:
            #if (WARNING_MP3_PLAY || WARNING_VMP3_PLAY || EXSPI_MP3_EN)
            msc_gain.layer0_gain = DIG_N0DB;
            set_layer0_fade(3, 0, 0, 1);
            #endif
            break;
        case MSC_ESBC:
            #if (WARNING_ESBC_PLAY || EXSPI_ESBC_EN)
            msc_gain.layer0_gain = DIG_N0DB;
            #endif
            break;
        default:
            break;
    }
    music_layer_mode_set(layer_num, mode);
    music_layer_sta_set(layer_num, LAYER_PLAYING);
#if MIDI_EN
    set_sys_clk(SYS_160M);
#else
    set_sys_clk(SYS_120M);      //播放音频前抬高主频,避免播放算力不足.
#endif
#if TKEY_CLASSD_FIX
    DI_EN_OUT(1);
#endif
}

//退出音乐播放公共
void music_mode_exit(msc_layer_e layer_num, msc_mode_e mode)
{
    //当需要增加音频播放退出时的处理,在此增加
    switch(mode){
        case MSC_LSBC:
            #if (WARNING_LSBC_PLAY)
            #endif
            break;
        case MSC_WAV:
            #if (WARNING_WAV_PLAY || EXSPI_WAV_EN)
            #endif
            break;
        case MSC_MIDI:
            #if MIDI_EN
                midi_song_play_exit();
            #endif
            break;
        case MSC_VMIDI:
            #if MIDI_EN
                vmidi_song_play_exit();
            #endif
            break;
        case MSC_MP3:
        case MSC_VMP3:
            #if (WARNING_MP3_PLAY || WARNING_VMP3_PLAY || EXSPI_MP3_EN)
            #endif
            break;
        case MSC_ESBC:
            #if (WARNING_ESBC_PLAY || EXSPI_ESBC_EN)
            #endif
            break;
        default:
            break;
    }

    music_layer_mode_clr(layer_num);
    music_layer_sta_set(layer_num, LAYER_STOP);
#if MSC_FADE_INOUT_EN
    dac_fade_out(6);
#endif
    if (!music_layer_play_check_all()) {
#if DAC_ALG_DC_OFF_EN
        dac_dc_exp_off();
        #if !TKEY_CLASSD_FIX
        delay_5ms(2);
        dac_analg_off();            //无音乐时关闭dac模拟
        #endif
#endif
        dac_gpdma_reset();
        music_cb_reset();
        set_sys_clk(SYS_CLK_SEL);
    }
#if TKEY_CLASSD_FIX
    DI_EN_OUT(0);
#endif
}

//设置某layer层当前解码的模式
bool music_layer_mode_set(msc_layer_e layer_num, msc_mode_e mode)
{
    if(layer_num >= MSC_TOTAL_LAYER){
        return false;
    }
    if(mode != msc_layer.layer_mode[layer_num]){
        msc_layer.layer_mode[layer_num] = mode;
    }
    return true;
}

//清空某layer层当前解码的模式
bool music_layer_mode_clr(msc_layer_e layer_num)
{
    if(layer_num >= MSC_TOTAL_LAYER){
        return false;
    }
    msc_layer.layer_mode[layer_num] = 0;
    return true;
}

//获取某layer层当前解码的模式
u16 get_msc_layer_mode(u8 layer_num)
{
    u16 layer_tmp = 0;
    layer_tmp = msc_layer.layer_mode[layer_num];
    return layer_tmp;
}

//检查某layer层某格式音频是否在播
AT(.music_text)
u8 layer_mode_check(msc_layer_e layer_num, msc_mode_e mode)
{
    if(((msc_layer.layer_mode[layer_num] & mode) == mode) && (msc_layer.layer_sta[layer_num] == LAYER_STOP)){
        return 1;
    }
    return 0;
}

//检查所有layer层是否有音频在播
AT(.music_text)
layer_sta_e music_layer_play_check_all(void)
{
    u8 msc_tmp = 0;
    for(u8 i = 0; i < MSC_TOTAL_LAYER; i++){
        if(msc_layer.layer_sta[i] == LAYER_PLAYING || msc_layer.layer_sta[i] == LAYER_PAUSE){
            msc_tmp = 1;
        }
    }
    return msc_tmp;
}

//检查某layer层是否有音频在播
layer_sta_e layer_play_check(msc_layer_e layer_num)
{
    if ((msc_layer.layer_sta[layer_num]) != LAYER_STOP) {
        return 1;
    }
    return 0;
}

//控制song播放某layer层的播放暂停
void music_layer_sta_set(msc_layer_e layer_num, layer_sta_e layer_sta)
{
    if(msc_layer.layer_sta[layer_num] != layer_sta){
        msc_layer.layer_sta[layer_num] = layer_sta;
        if(msc_layer.layer_sta[layer_num] == LAYER_PAUSE || msc_layer.layer_sta[layer_num] == LAYER_STOP){
            if(MSC_LAYER0 == layer_num){
                // set_layer0_fade(0, 10, 1, 0);    //设置1ms后淡出
            }else if(MSC_LAYER1 == layer_num){
                // set_layer1_fade(0, 10, 1, 0);    //设置1ms后淡出
            }
        }else if(msc_layer.layer_sta[layer_num] == LAYER_PLAYING){
            if(MSC_LAYER0 == layer_num){
                // set_layer0_fade(0, 0, 0, 0);     //清空淡入淡出参数
            }else if(MSC_LAYER1 == layer_num){
                // set_layer1_fade(0, 0, 0, 0);     //清空淡入淡出参数
            }
        }
    }
}

//获取某layer层播放状态
layer_sta_e music_layer_sta_get(msc_layer_e layer_num)
{
    return msc_layer.layer_sta[layer_num];
}

#if VMP3_BREAK_POINT_EN
static u32 vmp3_break_point_offset = 0;

u32 get_mp3_bytes_by_second(u16 second);

void vmp3_break_point_save(void)
{
    static u32 write_ticks_addr_pos = 0;
    u32 pos = 0;
    u32 temp;
    if (tick_check_expire(write_ticks_addr_pos, 100 * VMP3_BREAK_POINT_SAVE_PORIOD)) {
        temp = get_mp3_bytes_by_second(VMP3_BREAK_POINT_BACK_SECOND);
        write_ticks_addr_pos = tick_get();
        pos = vmp3_break_point_offset + spires_get_pos();
        if (pos > temp) {
            param_vmp3_break_pos_write(pos - temp);
        } else {
            param_vmp3_break_pos_write(0);
        }
        param_sync();
    }
}

u32 vmp3_break_point_restore(u32 addr)
{
    u32 offset = 0;
    if (addr == param_vmp3_break_addr_read()) {
        offset = param_vmp3_break_pos_read();
    }
    param_vmp3_break_addr_write(addr);
    param_sync();
    return offset;
}
#endif

#if LSBC_BREAK_POINT_EN
static u32 lsbc_break_point_offset = 0;
static u32 break_point_frame = 0;

u32 get_mp3_bytes_by_second(u16 second);

void lsbc_break_point_save(void)
{
    static u32 write_ticks_addr_pos = 0;
    u32 pos = 0;
    if (tick_check_expire(write_ticks_addr_pos, 100 * LSBC_BREAK_POINT_SAVE_PORIOD)) {
        write_ticks_addr_pos = tick_get();
        pos = lsbc_break_point_offset + spires_get_pos();
        break_point_frame = break_point_calculate(pos);             //计算当前帧
        if ((break_point_frame < get_lsbc_total_frame())) {
            param_lsbc_break_pos_write(break_point_frame - 1);      //记录前一帧
        } else {
            param_lsbc_break_pos_write(0);
        }
        param_sync();
    }
}

u32 lsbc_break_point_restore(u32 addr)
{
    u32 offset = 0;
    if (addr == param_lsbc_break_addr_read()) {
        offset = param_lsbc_break_pos_read();
    }
    param_lsbc_break_addr_write(addr);
    param_sync();
    return offset;
}
#endif

#if MIDI_BREAK_POINT_EN
static u32 midi_break_point_offset = 0;
u32 midi_break_point_frame = 0;

u32 get_midi_break_point_frame(void)
{
    return midi_break_point_frame;
}

void midi_break_point_save(u32 v_pos)
{
    static u32 write_ticks_addr_pos = 0;
    u32 pos = 0;
    if (tick_check_expire(write_ticks_addr_pos, 100 * MIDI_BREAK_POINT_SAVE_PORIOD)) {
        write_ticks_addr_pos = tick_get();
        pos = midi_break_point_offset + v_pos;
        param_midi_break_pos_write(pos);      //记录前一帧
        param_sync();
    }
}

u32 midi_break_point_restore(u32 addr)
{
    u32 offset = 0;
    if (addr == param_midi_break_addr_read()) {
        offset = param_midi_break_pos_read();
    }
    param_midi_break_addr_write(addr);
    param_sync();
    return offset;
}
#endif

#if WARNING_TONE_EN

#if WARNING_PAUSE_PLAY
typedef struct {
    bool pause_flag;
    u32 addr;
    u32 len;
    u32 pos;
    void (*music_res_play)(u32 addr, u32 len);
}pause_save_t;
pause_save_t pause_save;

void music_res_pause(u32 addr, u32 len, void *res_play_func)
{
    pause_save_t *p = &pause_save;
    p->addr = addr;
    p->len = len;
    p->pos = spires_get_pos();
    p->music_res_play = res_play_func;
}

void music_res_continue(void)
{
    pause_save_t *p = &pause_save;
    if(p->pos) {
        p->pause_flag = true;
        p->music_res_play(p->addr, p->len);
    }
}

static void music_res_jump(void)
{
    pause_save_t *p = &pause_save;
    if (p->pause_flag) {
        spires_load_pos(p->pos);
        memset(&pause_save, 0, sizeof(pause_save_t));
    }
}
#endif // WARNING_PAUSE_PLAY

void mp3_res_play_do(u32 addr, u32 len, bool sync, u8 loop_cnt)
{
#if WARNING_MP3_PLAY
#if USERBIN_MUSIC_PLAY
    if(!user_music_read_head()){
        printf("user music error!\n");
        return;
    }else{
        if(!get_music_info(&addr, &len, B_MP3, 2)){
            my_printf("error\n");
            return;
        }
    }
#endif
    u16 msg;
    printf("%s: addr: %x, len: %x\n", __func__, addr, len);
    if (len == 0) {
        return;
    }
    if(get_music_dec_sta() != MUSIC_STOP) {
        music_control(MUSIC_STOP);
    }
    bsp_change_volume(WARNING_VOLUME);
#if VMP3_BREAK_POINT_EN
    vmp3_break_point_offset = vmp3_break_point_restore(addr);
    addr += vmp3_break_point_offset;
    len -= vmp3_break_point_offset;
#endif
    mp3_res_play_kick(addr, len, loop_cnt);
#if WARNING_PAUSE_PLAY
    music_res_jump();
#endif
    while (get_music_dec_sta() != MUSIC_STOP) {
        WDT_CLR();
        if (!music_decode()) {
            break;
        }
#if PWM_HW_CLK_AUTO_SHIFT
    pwm_clk_sync();
#endif
#if VMP3_BREAK_POINT_EN
        vmp3_break_point_save();
#endif
        music_pcm_gpdma_kick();
        msg = msg_dequeue();
#if WARNING_PAUSE_PLAY
        if (msg == KU_PLAY_POWER) {     //此处可自定义msg暂停提示音播放并回到主循环，调用music_res_continue恢复提示音播放
            music_res_pause(addr, len, mp3_res_play);
            msg_enqueue(msg);
            break;
        }
#endif
        if (msg != NO_MSG) {
            msg_enqueue(msg);       //还原未处理的消息
        }
    }
    music_control(MUSIC_STOP);
    bsp_change_volume(sys_cb.vol);
    mp3_res_play_exit();
#endif
}

void vmp3_res_play_do(u32 addr, u32 len, bool sync, u8 loop_cnt)
{
#if WARNING_VMP3_PLAY
#if USERBIN_MUSIC_PLAY
    if(!user_music_read_head()){
        printf("user music error!\n");
        return;
    }else{
        if(!get_music_info(&addr, &len, B_VMP3, 1)){
            my_printf("error\n");
            return;
        }
    }
#endif
    u16 msg;
    printf("%s: addr: %x, len: %x\n", __func__, addr, len);
    if (len == 0) {
        return;
    }
    if(get_music_dec_sta() != MUSIC_STOP) {
        music_control(MUSIC_STOP);
    }
    bsp_change_volume(WARNING_VOLUME);
#if VMP3_BREAK_POINT_EN
    vmp3_break_point_offset = vmp3_break_point_restore(addr);
    addr += vmp3_break_point_offset;
    len -= vmp3_break_point_offset;
#endif
    vmp3_res_play_kick(addr, len, loop_cnt);
#if WARNING_PAUSE_PLAY
    music_res_jump();
#endif
    while (get_music_dec_sta() != MUSIC_STOP) {
        WDT_CLR();
        if (!music_decode()) {
            break;
        }
#if PWM_HW_CLK_AUTO_SHIFT
    pwm_clk_sync();
#endif
#if VMP3_BREAK_POINT_EN
        vmp3_break_point_save();
#endif
        music_pcm_gpdma_kick();
        msg = msg_dequeue();
#if WARNING_PAUSE_PLAY
        if (msg == KU_PLAY_POWER) {     //此处可自定义msg暂停提示音播放并回到主循环，调用music_res_continue恢复提示音播放
            music_res_pause(addr, len, vmp3_res_play);
            msg_enqueue(msg);
            break;
        }
#endif
        if (msg != NO_MSG) {
            msg_enqueue(msg);       //还原未处理的消息
        }
    }
    music_control(MUSIC_STOP);
    bsp_change_volume(sys_cb.vol);
    vmp3_res_play_exit();
#endif
}

void esbc_res_play_do(u32 addr, u32 len, bool sync, u8 loop_cnt)
{
#if WARNING_ESBC_PLAY
#if USERBIN_MUSIC_PLAY
    if(!user_music_read_head()){
        printf("user music error!\n");
        return;
    }else{
        if(!get_music_info(&addr, &len, B_ESBC, 1)){
            my_printf("error\n");
            return;
        }
    }
#endif
    u16 msg;
    u8 *ptr = (u8 *)addr;
    printf("%s: addr: %x, len: %x\n", __func__, addr, len);
    if (len == 0 || ptr[0] != 0x8B) {
        printf("esbc format err\n");
        return;
    }
    if(get_music_dec_sta() != MUSIC_STOP) {
        music_control(MUSIC_STOP);
    }
    bsp_change_volume(WARNING_VOLUME);
    esbc_res_play_kick(addr, len, &esbc_dec_cb, loop_cnt);
    while (get_music_dec_sta() != MUSIC_STOP) {
        WDT_CLR();
        if (!music_decode()) {
            break;
        }
#if PWM_HW_CLK_AUTO_SHIFT
    pwm_clk_sync();
#endif
        music_pcm_gpdma_kick();
        msg = msg_dequeue();
        if (msg != NO_MSG) {
            msg_enqueue(msg);       //还原未处理的消息
        }
    }
    music_control(MUSIC_STOP);
    bsp_change_volume(sys_cb.vol);
    esbc_res_play_exit();
#endif
}

#if MSC_ESBC_LINK_EN
void esbc_link_play_do(const msc_link_tbl_t *elink, const u8 e_num, esbc_dec_cb_t *esbc_d_vars)
{
#if WARNING_ESBC_PLAY
    u16 msg;
    u8 i, *ptr;
    u32 *paddr, *plen;
    for(i = 0; i < e_num; i++){
        ptr  = (u8 *)(*elink[i].addr);
        plen = (u32 *)(*elink[i].len);
        if (plen == 0 || ptr[0] != 0x8B) {
            return;
        }
    }
    if(get_music_dec_sta() != MUSIC_STOP) {
        music_control(MUSIC_STOP);
    }
    bsp_change_volume(WARNING_VOLUME);
    esbc_link_play_init();
    for(i = 0; i < e_num; i++){
        paddr = (u32 *)(*elink[i].addr);
        plen  = (u32 *)(*elink[i].len);
        esbc_link_play_kick((u32)paddr, (u32)plen, elink[i].sta_skip_frame, elink[i].end_skip_frame, esbc_d_vars);
        while (get_music_dec_sta() != MUSIC_STOP) {
            WDT_CLR();
            if (!music_decode()) {
                break;
            }
            music_pcm_gpdma_kick();
            msg = msg_dequeue();
            if (msg != NO_MSG) {
                msg_enqueue(msg);       //还原未处理的消息
            }
        }
    }
    music_control(MUSIC_STOP);
    bsp_change_volume(sys_cb.vol);
    esbc_res_play_exit();
#endif
}
#endif

void wav_res_play_do(u32 addr, u32 len, bool sync, u8 loop_cnt)
{
#if WARNING_WAV_PLAY
#if USERBIN_MUSIC_PLAY
    if(!user_music_read_head()){
        printf("user music error!\n");
        return;
    }else{
        if(!get_music_info(&addr, &len, B_WAV, 1)){
            my_printf("error\n");
            return;
        }
    }
#endif
    u16 msg;
    printf("%s: addr: %x, len: %x\n", __func__, addr, len);
    if (len == 0) {
        return;
    }
    if(get_music_dec_sta() != MUSIC_STOP) {
        music_control(MUSIC_STOP);
    }
    bsp_change_volume(WARNING_VOLUME);
    wav_layer1.wav_layer = MSC_LAYER1;
    wav_res_play_kick(addr, len, &wav_layer1, loop_cnt);

#if WARNING_PAUSE_PLAY
    music_res_jump();
#endif
    while (get_music_dec_sta() != MUSIC_STOP) {
        WDT_CLR();
        if (!music_decode()) {
            break;
        }
        music_pcm_gpdma_kick();
        msg = msg_dequeue();
#if PWM_HW_CLK_AUTO_SHIFT
    pwm_clk_sync();
#endif
#if WARNING_PAUSE_PLAY
        if (msg == KU_PLAY_POWER) {     //此处可自定义msg暂停提示音播放并回到主循环，调用mp3_res_continue恢复提示音播放
            music_res_pause(addr, len, wav_res_play);
            msg_enqueue(msg);
            break;
        }
#endif
        if (msg != NO_MSG) {
            msg_enqueue(msg);       //还原未处理的消息
        }
    }
    wav_res_play_exit();
#endif
}

void lsbc_res_play_do(u32 addr, u32 len, u8 loop_cnt)
{
#if WARNING_LSBC_PLAY
#if USERBIN_MUSIC_PLAY
    if(!user_music_read_head()){
        printf("user music error!\n");
        return;
    }else{
        if(!get_music_info(&addr, &len, B_LSBC, 1)){
            my_printf("error\n");
            return;
        }
    }
#endif
    u16 msg;
    u8 *ptr = (u8 *)addr;
    printf("%s: addr: %x, len: %x\n", __func__, addr, len);
    if (len == 0 || ptr[0] != 0x69) {
        printf("lsbc format err\n");
        return;
    }
    if(get_music_dec_sta() != MUSIC_STOP) {
        music_control(MUSIC_STOP);
    }
    bsp_change_volume(WARNING_VOLUME);
#if LSBC_BREAK_POINT_EN
    break_point_frame = lsbc_break_point_restore(addr);
    lsbc_res_loop_play_kick(addr, len, break_point_frame, 0, true);
#else
    lsbc_res_play_kick(addr, len, loop_cnt);
#endif
    while (get_music_dec_sta() != MUSIC_STOP) {
        WDT_CLR();
        if (!music_decode()) {
            break;
        }
#if PWM_HW_CLK_AUTO_SHIFT
    pwm_clk_sync();
#endif
#if LSBC_BREAK_POINT_EN
        lsbc_break_point_save();
#endif
        music_pcm_gpdma_kick();
        msg = msg_dequeue();
        if (msg != NO_MSG) {
            msg_enqueue(msg);       //还原未处理的消息
        }
    }
    music_control(MUSIC_STOP);
    bsp_change_volume(sys_cb.vol);
    lsbc_res_play_exit();
#endif
}
#else
void mp3_res_play_do(u32 addr, u32 len, bool sync, u8 loop_cnt){}
void esbc_res_play_do(u32 addr, u32 len, bool sync, u8 loop_cnt){}
void esbc_link_play_do(const msc_link_tbl_t *elink, const u8 e_num, esbc_dec_cb_t *esbc_d_vars){}
void wav_res_play_do(u32 addr, u32 len, bool sync, u8 loop_cnt){}
void lsbc_res_play_do(u32 addr, u32 len, u8 loop_cnt){}
#endif //WARNING_TONE_EN

//mp3提示音播放接口
void mp3_res_play(u32 addr, u32 len, u8 loop_cnt)
{
    music_mode_enter(MSC_LAYER0, MSC_MP3);
    mp3_res_play_do(addr, len, 0, loop_cnt);
    music_mode_exit(MSC_LAYER0, MSC_MP3);
}

//vmp3提示音播放接口
void vmp3_res_play(u32 addr, u32 len, u8 loop_cnt)
{
    music_mode_enter(MSC_LAYER0, MSC_MP3);
    vmp3_res_play_do(addr, len, 0, loop_cnt);
    music_mode_exit(MSC_LAYER0, MSC_MP3);
}

//esbc播放接口
void esbc_res_play(u32 addr, u32 len, u8 loop_cnt)
{
    music_mode_enter(MSC_LAYER0, MSC_ESBC);
    esbc_res_play_do(addr, len, 0, loop_cnt);
    music_mode_exit(MSC_LAYER0, MSC_ESBC);
}

//wav提示音播放接口
void wav_res_play(u32 addr, u32 len, u8 loop_cnt)
{
    music_mode_enter(MSC_LAYER0, MSC_WAV);
    wav_res_play_do(addr, len, 0, loop_cnt);
    music_mode_exit(MSC_LAYER0, MSC_WAV);
}

//Lsbc提示音播放接口
void lsbc_res_play(u32 addr, u32 len, u8 loop_cnt)
{
    music_mode_enter(MSC_LAYER0, MSC_LSBC);
    lsbc_res_play_do(addr, len, loop_cnt);
    music_mode_exit(MSC_LAYER0, MSC_LSBC);
}

bool music_decode(void)
{
    if (music_cb.pcm_samples) {
        return true;
    }
    if (music_cb.type == WAV_TYPE) {
#if (WARNING_WAV_PLAY || EXSPI_WAV_EN)
        if(wav_res_is_play(&wav_layer1)){
            return wav_res_decode(wav_speedup_val, wav_pitchup_val, &wav_layer1);
        }
#endif
    } else if (music_cb.type == MP3_TYPE) {
#if (WARNING_MP3_PLAY || WARNING_VMP3_PLAY || EXSPI_MP3_EN)
        if(mp3_res_is_play()){
            return mp3_dec_frame();
        }
#endif
    } else if (music_cb.type == ESBC_TYPE) {
#if (WARNING_ESBC_PLAY || EXSPI_ESBC_EN)
        return esbc_decode_frame(&esbc_dec_cb);
#endif
    } else if (music_cb.type == LSBC_TYPE) {
#if (WARNING_LSBC_PLAY || EXSPI_LSBC_EN)
        bool bo_ret = false;
        bo_ret = lsbc_dec_frame();
        lsbc_dec_pcm_out_cb_do();
        return bo_ret;
#endif
    }
    music_cb.sta = MUSIC_STOP;
    return false;
}

#if MUSIC_DECODE_BK_EN
//mp3播放接口
void mp3_song_play(u32 addr, u32 len, u8 loop_cnt)
{
#if MSC_BREAK_P_SOFT_FADE
    if(layer_play_check(MSC_LAYER0) == 1){
        break_play_lay0_fade_start();        //打断播放fade
        break_play_fade.addr = addr;
        break_play_fade.len = len;
        return;
    }
#endif
#if USERBIN_MUSIC_PLAY
    if(!user_music_read_head()){
        printf("user music error!\n");
        return;
    }else{
        if(!get_music_info(&addr, &len, B_MP3, 2)){
            my_printf("error\n");
            return;
        }
    }
#endif
#if MSC_MP3_LINK_EN
    mp3_song_link_disable();
#endif
    printf("%s: addr: %x, len: %x\n", __func__, addr, len);
    music_mode_enter(MSC_LAYER0, MSC_MP3);
    mp3_res_play_kick(addr, len, loop_cnt);
}

//vmp3播放接口
void vmp3_song_play(u32 addr, u32 len, u8 loop_cnt)
{
#if MSC_BREAK_P_SOFT_FADE
    if(layer_play_check(MSC_LAYER0) == 1){
        break_play_lay0_fade_start();        //打断播放fade
        break_play_fade.addr = addr;
        break_play_fade.len = len;
        return;
    }
#endif
#if USERBIN_MUSIC_PLAY
    if(!user_music_read_head()){
        printf("user music error!\n");
        return;
    }else{
        if(!get_music_info(&addr, &len, B_VMP3, 2)){
            my_printf("error\n");
            return;
        }
    }
#endif
    printf("%s: addr: %x, len: %x\n", __func__, addr, len);
    music_mode_enter(MSC_LAYER0, MSC_MP3);
    vmp3_res_play_kick(addr, len, loop_cnt);
}

//mp3/vmp3播放结束回调接口
void mp3_song_play_end(void)
{
#if WAV_PITCHUP_EN
    wav_pitch_up_init(); // 播放MP3之后变调需要重新初始化
#endif
    mp3_dec_stop();
    music_mode_exit(MSC_LAYER0, MSC_MP3);

}

#if MSC_MP3_LINK_EN
//mp3拼接播放接口
void mp3_song_link_play(const msc_link_tbl_t *elink, u8 loop_cnt)
{
    u32 *paddr, *plen;
    u8 mp3_loop_cnt;
    paddr = (u32 *)(*elink[0].addr);//从第1首开始顺序播放
    plen  = (u32 *)(*elink[0].len);
    if(loop_cnt == 0xff){
       mp3_loop_cnt = 0xff;
    }else{
        mp3_loop_cnt = loop_cnt * MSC_MP3_VMP3_LINK1_NUM;
    }
    mp3_link_play_kick((u32)paddr, (u32)plen, mp3_loop_cnt, MSC_MP3_VMP3_LINK1_NUM);
}

void mp3_song_link_init(u8 link_num)
{
    memset(&mp3_vmp3_link_sta, 0, sizeof(msc_link_sta_t));
    music_mode_enter(MSC_LAYER0, MSC_MP3);
    mp3_vmp3_link_sta.link_num = link_num;
    mp3_vmp3_link_sta.cur_num = 0;
    mp3_vmp3_link_sta.kick_start = 1;
    mp3_link_play_init_do();
}

void mp3_song_link_disable(void)
{
    memset(&mp3_link_sta, 0, sizeof(msc_link_sta_t));
}
#endif

#if MSC_VMP3_LINK_EN
//mp3拼接播放接口
void vmp3_song_link_play(const msc_link_tbl_t *elink, u8 loop_cnt)
{
    u32 *paddr, *plen;
    u8 vmp3_loop_cnt;
    paddr = (u32 *)(*elink[0].addr);//从第1首开始顺序播放
    plen  = (u32 *)(*elink[0].len);
    if(loop_cnt == 0xff){
       vmp3_loop_cnt = 0xff;
    }else{
        vmp3_loop_cnt = loop_cnt *MSC_MP3_VMP3_LINK1_NUM;
    }
    mp3_link_play_kick((u32)paddr, (u32)plen, vmp3_loop_cnt, MSC_MP3_VMP3_LINK1_NUM);
}

void vmp3_song_link_init(u8 link_num)
{
    memset(&mp3_vmp3_link_sta, 0, sizeof(msc_link_sta_t));
    music_mode_enter(MSC_LAYER0, MSC_VMP3);
    mp3_vmp3_link_sta.link_num = link_num;
    mp3_vmp3_link_sta.cur_num = 0;
    mp3_vmp3_link_sta.kick_start = 1;
    vmp3_link_play_init_do();
}

void vmp3_song_link_disable(void)
{
    memset(&mp3_vmp3_link_sta, 0, sizeof(msc_link_sta_t));
}
#endif

//esbc播放接口
void esbc_song_play(u32 addr, u32 len, u8 loop_cnt)
{
#if MSC_BREAK_P_SOFT_FADE
    if(layer_play_check(MSC_LAYER0) == 1){
        break_play_lay0_fade_start();        //打断播放fade
        break_play_fade.addr = addr;
        break_play_fade.len = len;
        return;
    }
#endif
#if (WARNING_ESBC_PLAY || EXSPI_ESBC_EN)
#if USERBIN_MUSIC_PLAY
    if(!user_music_read_head()){
        printf("user music error!\n");
        return;
    }else{
        if(!get_music_info(&addr, &len, B_ESBC, 2)){
            my_printf("error\n");
            return;
        }
    }
#endif
    printf("%s: addr: %x, len: %x\n", __func__, addr, len);
    music_mode_enter(MSC_LAYER0, MSC_ESBC);
    esbc_res_start(&esbc_dec_cb);
    esbc_res_play_kick(addr, len, &esbc_dec_cb, loop_cnt);
#endif
}

//esbc播放结束回调接口
void esbc_song_play_end(void)
{
#if (WARNING_ESBC_PLAY || EXSPI_ESBC_EN)
    esbc_res_stop(&esbc_dec_cb);
    music_mode_exit(MSC_LAYER0, MSC_ESBC);
#endif
}

//lsbc播放接口
void lsbc_song_play(u32 addr, u32 len, u8 loop_cnt)
{
#if MSC_BREAK_P_SOFT_FADE
    if(layer_play_check(MSC_LAYER0) == 1){
        break_play_lay0_fade_start();        //打断播放fade
        break_play_fade.addr = addr;
        break_play_fade.len = len;
        return;
    }
#endif
#if (WARNING_LSBC_PLAY)
#if USERBIN_MUSIC_PLAY
    if(!user_music_read_head()){
        printf("user music error!\n");
        return;
    }else{
        if(!get_music_info(&addr, &len, B_LSBC, 2)){
            my_printf("error\n");
            return;
        }
    }
#endif
    printf("%s: addr: %x, len: %x\n", __func__, addr, len);
    music_mode_enter(MSC_LAYER0, MSC_LSBC);
    lsbc_res_play_kick(addr, len, loop_cnt);
#endif
}

//Lsbc播放结束回调接口
void lsbc_song_play_end(void)
{
    printf("lsbc_song_play_end\n");
#if (WARNING_LSBC_PLAY)
    music_mode_exit(MSC_LAYER0, MSC_LSBC);
#endif
}

//wav/adpcm播放接口
void wav_song_play(msc_layer_e layer_num, u32 addr, u32 len, u8 loop_cnt)
{
#if MSC_BREAK_P_SOFT_FADE
    if(layer_play_check(MSC_LAYER1) == 1){
        break_play_lay1_fade_start();        //打断播放fade
        break_play_fade.addr = addr;
        break_play_fade.len = len;
        return;
    }
#endif
#if USERBIN_MUSIC_PLAY
    if(!user_music_read_head()){
        printf("user music error!\n");
        return;
    }else{
        if(!get_music_info(&addr, &len, B_WAV, 2)){
            my_printf("error\n");
            return;
        }
    }
#endif
#if (WARNING_WAV_PLAY)
    printf("%s: addr: %x, len: %x, layer: %d\n", __func__, addr, len, layer_num);
    music_mode_enter(layer_num, MSC_WAV);
    if(MSC_LAYER1 == layer_num){
#if ((MUISC_WAV_LAYER_EN & MSC_WAV_LAYER1) == (MSC_WAV_LAYER1))
        wav_layer1.wav_layer = MSC_LAYER1;
        wav_res_play_kick(addr, len, &wav_layer1, loop_cnt);
#endif
    }else if(MSC_LAYER2 == layer_num){
#if ((MUISC_WAV_LAYER_EN & MSC_WAV_LAYER2) == (MSC_WAV_LAYER2))
        wav_layer2.wav_layer = MSC_LAYER2;
        wav_res_play_kick(addr, len, &wav_layer2, loop_cnt);
#endif
    }else if(MSC_LAYER3 == layer_num){
#if ((MUISC_WAV_LAYER_EN & MSC_WAV_LAYER3) == (MSC_WAV_LAYER3))
        wav_layer3.wav_layer = MSC_LAYER3;
        wav_res_play_kick(addr, len, &wav_layer3, loop_cnt);
#endif
    }
#endif
}

//wav/adpcm播放结束回调接口
void wav_song_play_end(msc_layer_e layer_num)
{
    printf("wav_song_play_end, layer: %d\n", layer_num);
#if (WARNING_WAV_PLAY)
    if(MSC_LAYER1 == layer_num){
#if ((MUISC_WAV_LAYER_EN & MSC_WAV_LAYER1) == (MSC_WAV_LAYER1))
        wav_dec_stop(&wav_layer1);
        music_mode_exit(MSC_LAYER1, MSC_WAV);
#endif
    }else if(MSC_LAYER2 == layer_num){
#if ((MUISC_WAV_LAYER_EN & MSC_WAV_LAYER2) == (MSC_WAV_LAYER2))
        wav_dec_stop(&wav_layer2);
        music_mode_exit(MSC_LAYER2, MSC_WAV);
#endif
    }else if(MSC_LAYER3 == layer_num){
#if ((MUISC_WAV_LAYER_EN & MSC_WAV_LAYER3) == (MSC_WAV_LAYER3))
        wav_dec_stop(&wav_layer3);
        music_mode_exit(MSC_LAYER3, MSC_WAV);
#endif
    }
    music_mode_exit(layer_num, MSC_WAV);
#endif
}

void wav_dec_proc(msc_layer_e layer_num, wav_layer_t *wav_layer)
{
    if(wav_res_is_play(wav_layer) && (MSC_LAYER0 != layer_num)){
        if(music_layer_sta_get(layer_num) == LAYER_PLAYING){
            if(!wav_res_decode(wav_speedup_val, wav_pitchup_val, wav_layer)){
                wav_song_play_end(layer_num);
            }
        }else{
            wav_dec_pause_do(wav_layer);
        }
    }
}

void lsbc_song_dec_proc(msc_layer_e layer_num)
{
    if((music_layer_sta_get(layer_num) == LAYER_PLAYING)){
        if(!lsbc_dec_frame()){
            lsbc_song_play_end();
        }else{
            lsbc_dec_pcm_out_cb_do();
        }
    }else{
        lsbc_dec_pause_do();
    }
}

void mp3_song_dec_proc(msc_layer_e layer_num)
{
    if(!(music_layer_sta_get(layer_num) == LAYER_PLAYING)){
        mp3_dec_pause_do();
        return;
    }
    if(mp3_res_is_play()){
        if(!mp3_dec_frame()){
            mp3_song_play_end();
        }
    }
}

void esbc_song_dec_proc(msc_layer_e layer_num)
{
    if(!(music_layer_sta_get(layer_num) == LAYER_PLAYING)){
        esbc_dec_pause_do();
        return;
    }
    if(esbc_res_is_play(&esbc_dec_cb)){
        if(!esbc_decode_frame(&esbc_dec_cb)){
            esbc_song_play_end();
        }
    }
}

//AT(.com_rodata.bat)
//const char msc_pow_str[] = "msc_max_pow: %d\n";

#define MUSIC_PCM_PWM_CTRL_EN          (MUSIC_PCM_PWM_DUTY_EN && (PWM_HW_EN || PWM_TMR2_EN))

#if MUSIC_PCM_PWM_CTRL_EN
u32 music_pwm_get_pr(void)
{
#if PWM_TMR2_EN
    return TMR2PR;
#else
    switch (MUSIC_PCM_PWM_CH) {
        case 0: return PTMR0PR;
        case 1: return PTMR1PR;
        case 2: return PTMR2PR;
        case 3: return PTMR3PR;
        default: return PTMR0PR;
    }
#endif
}

void music_pwm_set_duty(u32 duty)
{
#if PWM_TMR2_EN
    u16 duty16 = (duty > 0xffff) ? 0xffff : (u16)duty;
    if (MUSIC_PCM_PWM_CH == 1) {
        TMR2DUTY1 = duty16;
    } else {
        TMR2DUTY0 = duty16;
    }
#else
    switch (MUSIC_PCM_PWM_CH) {
        case 0:
            PTMR0DUTY = duty;
            break;
        case 1:
            PTMR1DUTY = duty;
            break;
        case 2:
            PTMR2DUTY = duty;
            break;
        case 3:
            PTMR3DUTY = duty;
            break;
        default:
            PTMR0DUTY = duty;
            break;
    }
#endif
}

u32 music_pcm_pow_to_pwm_duty(u16 pcm_pow)
{
    u32 pwm_pr = music_pwm_get_pr();
    u32 duty_min = (pwm_pr * MUSIC_PWM_DUTY_MIN_PERMILLE) / 1000;
    u32 duty_max = (pwm_pr * MUSIC_PWM_DUTY_MAX_PERMILLE) / 1000;

    if (pwm_pr > 1 && duty_max >= pwm_pr) {
        duty_max = pwm_pr - 1;
    }
    if (duty_min > duty_max) {
        duty_min = duty_max;
    }

    if (MUSIC_PCM_POW_FULL_SCALE <= MUSIC_PCM_POW_NOISE_FLOOR) {
        return duty_min;
    }

    u32 pow = ((u32)pcm_pow) << MUSIC_PCM_POW_GAIN_SHIFT;
    if (pow <= MUSIC_PCM_POW_NOISE_FLOOR) {
        return duty_min;
    }
    if (pow >= MUSIC_PCM_POW_FULL_SCALE) {
        return duty_max;
    }

    u32 duty_range = duty_max - duty_min;
    u32 pow_range = MUSIC_PCM_POW_FULL_SCALE - MUSIC_PCM_POW_NOISE_FLOOR;
    return duty_min + (((pow - MUSIC_PCM_POW_NOISE_FLOOR) * duty_range) / pow_range);
}

void music_pwm_update_by_pcm_pow(u16 pcm_pow)
{
    static u32 duty_smooth = 0;
    static u8 duty_init = 0;

    u32 duty_target = music_pcm_pow_to_pwm_duty(pcm_pow);
    if (!duty_init) {
        duty_smooth = duty_target;
        duty_init = 1;
    } else if (duty_target > duty_smooth) {
        u32 delta = duty_target - duty_smooth;
        duty_smooth += (delta >> MUSIC_PWM_ATTACK_SHIFT) + 1;
        if (duty_smooth > duty_target) {
            duty_smooth = duty_target;
        }
    } else if (duty_target < duty_smooth) {
        u32 delta = duty_smooth - duty_target;
        duty_smooth -= (delta >> MUSIC_PWM_RELEASE_SHIFT) + 1;
        if (duty_smooth < duty_target) {
            duty_smooth = duty_target;
        }
    }

    music_pwm_set_duty(duty_smooth);
}
#endif

#if DAC_SOFT_DNR
#define MUTE_POW_VAR   1
#define MUTE_POW_CNT   3

static u16 dac_dnr_max_pow;
dac_dnr_t dac_dnr;

AT(.com_text.dac.dnr) WEAK
u8 get_dac_dnr_sta(void)
{
    return dac_dnr.dnr_flag;
}

AT(.com_text.dac.dnr) WEAK
void set_dac_dnr_sta(u8 sta)
{
    dac_dnr.dnr_flag = sta;
}

AT(.com_text.dac.dnr) WEAK
u16 dac_dnr_get_max_pow(void)
{
    return dac_dnr_max_pow;
}

AT(.com_text.dac.dnr) WEAK
void dac_dnr_detect(void)
{
    if(get_dac_dnr_sta()) {
        u16 pow = dac_dnr_get_max_pow();
        if(pow == MUTE_POW_VAR){
            dac_dnr.dnr_cnt++;
            if(dac_dnr.dnr_cnt == MUTE_POW_CNT){
                dac_dnr.dnr_cnt = 0;
                set_dac_dnr_sta(0);
                dac_dnr_out();
            }
        } else {
            dac_dnr.dnr_cnt = 0;
        }
    }
}

void dac_dnr_init(void)
{
    memset(&dac_dnr, 0, sizeof(dac_dnr));
}
#endif // DAC_SOFT_DNR

//音乐解码后pcm回调函数
//ptr:指向的数据为layer1~n合并后的pcm, samples:pcm数据样点数

AT(.music_text)
void music_pcm_out_callback(s16 *ptr, u32 samples)
{
#if DAC_SOFT_DNR
    u16 msc_max_pow = dnr_buf_maxpow(ptr, samples);
#endif

#if DAC_SOFT_DNR
    dac_dnr_max_pow = msc_max_pow;
    my_printf(msc_pow_str, dac_dnr_max_pow);
#endif

#if MUSIC_PCM_PWM_CTRL_EN
    music_pwm_update_by_pcm_pow(msc_max_pow);
#endif


    if(SPI1_AUDIO_EN && SPI1_MASTER_EN){
        spi1_pcm_send_proc((u8 *)music_cb.pcm_buf, music_cb.pcm_samples*2);
    }
}

ALIGNED(512)
void music_decode_process(void)
{
#if MUSIC_MODE_TABLE_EN
    music_mode_table_check();
#endif
    if(!music_layer_play_check_all() || (get_music_dec_sta() == MUSIC_STOP)){
        return;
    }
    for(u8 i = 0; i < MSC_TOTAL_LAYER; i++){
        switch(msc_layer.layer_mode[i]){
            case MSC_LSBC:
#if (WARNING_LSBC_PLAY || EXSPI_LSBC_EN)
                lsbc_song_dec_proc(i);
#endif
                break;
            case MSC_WAV:
#if (WARNING_WAV_PLAY || EXSPI_WAV_EN)
                wav_dec_proc(i, wav_layer_tbl[i-1]);
#endif
                break;
            case MSC_MIDI:
            case MSC_VMIDI:
#if MIDI_EN
                midi_decode_proc(i);
#endif
                break;
            case MSC_MP3:
            case MSC_VMP3:
#if (WARNING_MP3_PLAY || WARNING_VMP3_PLAY || EXSPI_MP3_EN)
                mp3_song_dec_proc(i);
#endif
                break;
            case MSC_ESBC:
#if (WARNING_ESBC_PLAY || EXSPI_ESBC_EN)
                esbc_song_dec_proc(i);
#endif
                break;
            default:
                break;
        }
    }
    if(music_cb.pcm_samples){
        music_pcm_gpdma_kick();
    }
}
#endif

#if MSC_ESBC_LINK_EN   //esbc拼接播放测试示例
#define MSC_ESBC_LINK1_NUM  3
msc_link_tbl_t msc_esbc_link1[MSC_ESBC_LINK1_NUM] = {
    {&RES_BUF_SBC_K003_SBC, &RES_LEN_SBC_K003_SBC, 5, 50},
    {&RES_BUF_SBC_K004_SBC, &RES_LEN_SBC_K004_SBC, 5, 50},
    {&RES_BUF_SBC_K005_SBC, &RES_LEN_SBC_K005_SBC, 5, 50},
};
void esbc_link_test(void)
{
    esbc_link_play_do(msc_esbc_link1, MSC_ESBC_LINK1_NUM, &esbc_dec_cb);
}
#endif

#if MSC_MP3_LINK_EN || MSC_VMP3_LINK_EN//mp3拼接播放测试示例
#define MSC_MP3_VMP3_LINK1_NUM       3       //拼接播放音频个数
msc_link_tbl_t msc_mp3_vmp3_link1[MSC_MP3_VMP3_LINK1_NUM] = {
     {&RES_BUF_EN_POWERON_VMP3, &RES_LEN_EN_POWERON_VMP3, 0, 0},
     {&RES_BUF_EN_POWERON_VMP3, &RES_LEN_EN_POWERON_VMP3, 0, 0},
     {&RES_BUF_EN_POWERON_VMP3, &RES_LEN_EN_POWERON_VMP3, 0, 0},
    //{&RES_BUF_EN_POWERON_MP3, &RES_LEN_EN_POWERON_MP3, 0x11, 0x22},
	//{&RES_BUF_EN_POWEROFF_MP3, &RES_LEN_EN_POWEROFF_MP3, 0x33, 0x44},
    //{&RES_BUF_EN_NUM127_MP3, &RES_LEN_EN_NUM127_MP3, 0x12, 0x34},
};
#endif

#if MSC_BREAK_P_SOFT_FADE

break_fade_t break_play_fade;

void break_play_lay0_fade_start(void)
{
    set_layer0_fade(0, 10, 0, 0);    //设置淡出帧长
    set_lay0_fade_out_start(1);
    break_play_fade.lay0_tick = tick_get();
    break_play_fade.lay0_s_flag = 1;
}

void break_play_lay1_fade_start(void)
{
    set_layer1_fade(0, 10, 0, 0);    //设置淡出帧长
    set_lay1_fade_out_start(1);
    break_play_fade.lay1_tick = tick_get();
    break_play_fade.lay1_s_flag = 1;
}

void break_play_fade_process(void)
{
    if (tick_check_expire(break_play_fade.lay0_tick, 150) && (break_play_fade.lay0_s_flag == 1)) {       //打断播放时，淡出150ms后再播放
        break_play_fade.lay0_s_flag = 0;
        music_mode_exit(MSC_LAYER0, MSC_MP3);
        mp3_song_play(break_play_fade.addr, break_play_fade.len, 0);
    }

    if (tick_check_expire(break_play_fade.lay1_tick, 150) && (break_play_fade.lay1_s_flag == 1)) {       //打断播放时，淡出150ms后再播放
        break_play_fade.lay1_s_flag = 0;
        music_mode_exit(MSC_LAYER1, MSC_WAV);
        wav_song_play(MSC_LAYER1, break_play_fade.addr, break_play_fade.len, 0);
    }
}
#endif
