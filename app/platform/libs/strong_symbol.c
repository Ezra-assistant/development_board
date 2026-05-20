/**********************************************************************
*
*   strong_symbol.c
*   定义库里面部分WEAK函数的Strong函数，动态关闭库代码
***********************************************************************/
#include "include.h"

#if (SPI1_AUDIO_EN)

#elif (!DAC_EN) || DAC_GPIO_PIN_EN
AT(.com_text.dac)
void dac_gpdma_kick(s16 *ptr, u32 samples){}
void spi1_send_buf_do(u8 *txbuf, u8 *rxbuf, u32 len){}
#else
void spi1_send_buf_do(u8 *txbuf, u8 *rxbuf, u32 len){}
#endif

#if UART_S_UPDATE
WEAK u8 get_uart_upd (u8 *ch){
    return 0;
}
AT(.com_text.uart_isr)
WEAK void put_upd_uart_msg(void)
{
}
#endif // UART_S_UPDATE

#if !WAV_SPEEDUP_EN
void magic_voice_init(mav_init_t *p, u8 idx) {}
void magic_voice_process(s16 *data, u32 samples, u8 idx) {}
#endif

#if (!WARNING_WAV_PLAY)
void wav_res_play_loop_back(u8 enable){}
void wav_res_play_init(u32 addr, u32 len, wav_layer_t *wav_layer, u8 loop_cnt){}
bool wav_res_play_kick(u32 addr, u32 len, wav_layer_t *wav_layer, u8 loop_cnt){}
#endif
#if ((MUISC_WAV_LAYER_EN & MSC_WAV_LAYER1) != (MSC_WAV_LAYER1))
int wav1_spi_stream_read(void *buf, unsigned int size){return 0;}
bool wav1_spi_stream_seek(unsigned int ofs, int whence){return false;}
spires_cb_t *get_wav1_res_ptr(void){return NULL;}
#endif
#if ((MUISC_WAV_LAYER_EN & MSC_WAV_LAYER2) != (MSC_WAV_LAYER2))
int wav2_spi_stream_read(void *buf, unsigned int size){return 0;}
bool wav2_spi_stream_seek(unsigned int ofs, int whence){return false;}
spires_cb_t *get_wav2_res_ptr(void){return NULL;}
#endif
#if ((MUISC_WAV_LAYER_EN & MSC_WAV_LAYER3) != (MSC_WAV_LAYER3))
int wav3_spi_stream_read(void *buf, unsigned int size){return 0;}
bool wav3_spi_stream_seek(unsigned int ofs, int whence){return false;}
spires_cb_t *get_wav3_res_ptr(void){return NULL;}
#endif

#if (EX_SPIFLASH_SUPPORT & EXSPI_MUSIC)

#else
void exspiflash_wav_num_kick(msc_layer_e layer_num, u32 num) {}
#endif

#if !DAC_GPIO_PIN_EN
void dac_pin_gpio_init(bool bo_en) {}
void dac_voutp_set(u8 io_sta) {}
void dac_voutn_set(u8 io_sta) {}
#endif

#if !DAC_MAP_EN
void dac_io_map(u8 ch0_map_sel,u8 ch1_map_sel) {}
#endif

#if (!WARNING_VSBC_PLAY) && (!EX_SPIFLASH_SUPPORT)
u16 spiflash_vsbc_loop_skip_start_size(void) {return 0;}
u16 spiflash_vsbc_loop_skip_end_size(void) {return 0;}
u16 spiflash_vsbc_skip_start_size(void) {return 0;}
u16 spiflash_vsbc_skip_end_size(void) {return 0;}
#endif

#if !UFLASH_BIN_EN
bool spiflash_read_kick(void *buf, u32 addr, uint len){return false;}
bool spiflash_read_wait(void){return false;}
#endif

#if !(MIDI_EN || UFLASH_BIN_EN)
void vmidi_song_play(u32 addr, u32 len, midi_mode_e midi_mode, u8 loop_cnt){}
void midi_song_play(u32 addr, u32 len, midi_mode_e midi_mode, u8 loop_cnt){}
u8 get_midi_dec_sta(void){return 0;}
void midi_control(u8 sta){}
void dcache_init(void){}
AT(.pdncom_text.lp.sleep)
void dcache_init_l4m(void){}
void dcache_init_4addr(void){}
#endif

#if (!USER_ADKEY) && (!USER_ADKEY2) && (!USER_IOKEY)
u16 key_multi_press_process(u16 key_val){return 0;}
#endif

#if FFT_PROC_EN
    #if !FFT_512_EN
    AT(.fft_rodata.windown)
    const s32 rdft_w_512[1] = {};
    #endif
    #if !FFT_256_EN
    AT(.fft_rodata.windown)
    const s32 rdft_w_256[1] = {};
    #endif
    #if !FFT_128_EN
    AT(.fft_rodata.windown)
    const s32 rdft_w_128[1] = {};
    #endif
#endif

#if !DAC_SOFT_EQ_EN
s16 mic_eq_proc(s16 input){return 0;}
bool dac_set_eq_by_res(u32 *addr, u32 *len){return false;}
#endif

#if !MUSIC_DECODE_BK_EN
void mp3_song_play(u32 addr, u32 len, u8 loop_cnt){}
void vmp3_song_play(u32 addr, u32 len, u8 loop_cnt){}
void wav_song_play(msc_layer_e layer_num, u32 addr, u32 len, u8 loop_cnt){}
void wav_song_play_end(msc_layer_e layer_num){}
void mp3_song_play_end(void){}
#endif

#if !MIDI_REC_EN
bool get_midi_rec_en(void){return 0;}
#endif

#if !MIDI_OKON_EN
u8 get_okon_key_cnt(void){return 0;}
void reset_okon_key_cnt(void){}
#endif

#if !WARNING_ESBC_PLAY
void esbc_link_play_kick(u32 addr, u32 len, u16 sta_skip_frame, u16 end_skip_frame, esbc_dec_cb_t *esbc_d_vars){}
void esbc_link_play_init(void){}
int spi_esbc_link_decode_init(esbc_dec_cb_t *esbc_d_vars){return 0;}

void esbc_res_play_kick(u32 addr, u32 len, esbc_dec_cb_t *esbc_d_vars, u8 loop_cnt){}
void esbc_res_play_exit(void){}
int spi_esbc_decode_init(esbc_dec_cb_t *esbc_d_vars){return 0;}
bool esbc_decode_frame(esbc_dec_cb_t *esbc_d_vars){return false;}
int esbc_init(esbc_dec_cb_t *esbc_d_vars){return 0;}
#endif

#if !WARNING_LSBC_PLAY
void lsbc_res_play_exit(void){};
void lsbc_res_play_kick(u32 addr, u32 len, u8 loop_cnt){};
bool lsbc_dec_frame(void){return 0;};
void lsbc_dec_pcm_out_cb_do(void){};
#endif

#if !LSBC_BREAK_POINT_EN
bool lsbc_res_loop_play_kick(u32 addr, u32 len, u16 sta_skip_frame, u16 end_skip_frame, bool loop_en){return 0;};
u32 get_lsbc_total_frame(void){return 0;};
u32 break_point_calculate(u32 pos){return 0;};
#endif

#if !DAC_DRC_EN
bool drc_v3_init(const void *bin, int bin_size){return false;}
AT(.drc_text.drc_proc)
s16 drc_v3_calc(s32 sample){return 0;}
#endif

#if !DAC_SOFT_DNR
void dac_dnr_out(void){};
void dac_dnr_in(void){};
#endif
