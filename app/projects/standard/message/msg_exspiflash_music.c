#include "include.h"
#include "func.h"
//#include "func_music.h"
#include "func_exspiflash_music.h"

#if (EX_SPIFLASH_SUPPORT)
AT(.text.func.music.msg)
void func_exspiflash_music_message(u16 msg)
{
#if SPIFLASH_MUSIC_PLAY_REC
    u32 rec_total = 0;
#endif
    switch (msg) {
        case KU_PREV_VOL_DOWN:
        case KL_VOL_DOWN_PREV:
        case KU_PREV:
        #if SPIFLASH_MUSIC_PLAY_REC
            exspiflash_rec_switch_file(0);
        #else
            exspiflash_music_switch_file(0);
        #endif
            break;

        case KU_NEXT_VOL_UP:
        case KL_VOL_UP_NEXT:
        case KU_NEXT:
        #if SPIFLASH_MUSIC_PLAY_REC
            exspiflash_rec_switch_file(1);
        #else
            exspiflash_music_switch_file(1);
        #endif
            break;

#if SPIFLASH_MUSIC_PLAY_REC
        case KL_REC:
            printf("delete cur file\n");
            music_control(MUSIC_STOP);
            spiflash_rec_delete_curplay_file();
            rec_total = spiflash_rec_get_total_file();
            printf("rec_total = %d\n",rec_total);
            if (0 == rec_total) {
                func_cb.sta = FUNC_NULL;
            }
#endif
            break;

        case KU_PLAY:
        case KU_PLAY_POWER:
        case KU_PLAY_MODE:
              if (exspi_msc.pause) {
                exspi_msc.pause = false;
                music_layer_sta_set(MSC_LAYER0, LAYER_PLAYING);
                printf("music continue playing\n");
            } else {
                exspi_msc.pause = true;
                bsp_clr_mute_sta();
                music_layer_sta_set(MSC_LAYER0, LAYER_PAUSE);
                printf("music pause\n");
            }
            break;

        default:
            non_volatile_memory();
            func_message(msg);
            break;
    }
}
#endif //(EX_SPIFLASH_SUPPORT & EXSPI_MUSIC)

