#include "include.h"
#include "func.h"
#include "func_music.h"

#if FUNC_MUSIC_EN
AT(.text.func.idle.msg)
void func_music_message(u16 msg)
{
#if MIDI_KEYBOARD_EN
    u16 key_num = (msg & 0x00FF);
    if(key_num >= KEY_MAT_1 && key_num <= KEY_MAT_88){
        midi_keyboard_msg_proc(msg);
        return;
    }
#endif

    switch (msg) {

        case KU_PLAY:
            wav_speedup_val = 100;
            wav_pitchup_val = 10; //变调则不能和MP3叠加播放
            mp3_song_play(RES_BUF_EN_POWERON_MP3, RES_LEN_EN_POWERON_MP3, 0);
            if (!wav_res_is_play(&wav_layer1)) {
                wav_song_play(MSC_LAYER1, RES_BUF_EN_MAX_VOL_WAV, RES_LEN_EN_MAX_VOL_WAV, 0);
            }
            wav_res_play_loop_back(1);
            break;

        case KU_PREV:
            wav_song_play_end(MSC_LAYER1);
            break;

        case KU_NEXT:
            wav_speedup_val = 100;
            wav_pitchup_val = 20;
            mp3_song_play_end();
            if (!wav_res_is_play(&wav_layer1)) {
                wav_song_play(MSC_LAYER1, RES_BUF_EN_MAX_VOL_WAV, RES_LEN_EN_MAX_VOL_WAV, 0);
            }
            wav_res_play_loop_back(1);
            break;

        default:
            func_message(msg);
            break;
    }
}
#endif // FUNC_MUSIC_EN
