#include "include.h"
#include "bsp_user_music.h"

#if USERBIN_MUSIC_PLAY

#define TRACE_EN                0
#if TRACE_EN
#define TRACE(...)              my_printf(__VA_ARGS__)
#define TRACE_R(...)            my_print_r(__VA_ARGS__)
#else
#define TRACE(...)
#define TRACE_R(...)
#endif // TRACE_EN
user_msc_t user_msc AT(.bss.user_msc);

bool user_music_read_head(void)
{
    /* 暂时只支持文件数为1 */
    u8 head_buf[8];
    u8 init_num = 0;
    u32 user_laddr;

    memcpy(head_buf, (u8* )USER_READ_ADDR, 8);
    if ((head_buf[0] != 'B') || (head_buf[1] != 'l')|| (head_buf[2] != 'u')|| (head_buf[3] != 'e')
        || (head_buf[4] != 'T')|| (head_buf[5] != 'r')|| (head_buf[6] != 'u')|| (head_buf[7] != 'm')){
        TRACE("err Tag ERR\n");
        return false;
    }

    memcpy(&user_msc.music_type, (u8*)USER_MUSIC_HEAD_ADDR, 1);
    TRACE("music_type %x\n",user_msc.music_type);

    if(user_msc.music_type & USE_WAV){
        TRACE("USE_WAV\n");
        user_laddr = USER_MUSIC_ADDR + (init_num*0x10) + 4;
        memcpy(&user_msc.wav_chunk_addr, (u8*)user_laddr, 4);       //wav chunck begin
        memcpy(&user_msc.wav_file_num, (u8*)(user_laddr + 8), 4);   //music num
    }
    init_num++;
    if(user_msc.music_type & USE_MP3){
        TRACE("USE_MP3\n");
        user_laddr = USER_MUSIC_ADDR + (init_num*0x10) + 4;
        memcpy(&user_msc.mp3_chunk_addr, (u8*)user_laddr, 4);       //mp3 chunck begin
        memcpy(&user_msc.mp3_file_num, (u8*)(user_laddr + 8), 4);   //music num
    }
    init_num += 3;
    if(user_msc.music_type & USE_VMP3){
        TRACE("USE_VMP3\n");
        user_laddr = USER_MUSIC_ADDR + (init_num*0x10) + 4;
        memcpy(&user_msc.vmp3_chunk_addr, (u8*)user_laddr, 4);      //vmp3 chunck begin
        memcpy(&user_msc.vmp3_file_num, (u8*)(user_laddr + 8), 4);  //music num
    }
    init_num++;
    if(user_msc.music_type & USE_ESBC){
        TRACE("USE_ESBC\n");
        user_laddr = USER_MUSIC_ADDR + (init_num*0x10) + 4;
        memcpy(&user_msc.esbc_chunk_addr, (u8*)user_laddr, 4);      //esbc chunck begin
        memcpy(&user_msc.esbc_file_num, (u8*)(user_laddr + 8), 4);  //music num
    }
    init_num++;
    if(user_msc.music_type & USE_LSBC){
        TRACE("USE_LCBS\n");
        user_laddr = USER_MUSIC_ADDR + (init_num*0x10) + 4;
        memcpy(&user_msc.lsbc_chunk_addr, (u8*)user_laddr, 4);      //lsbc chunck begin
        memcpy(&user_msc.lsbc_file_num, (u8*)(user_laddr + 8), 4);  //music num
    }
    return true;
}

bool get_music_info(u32* addr, u32* len, u8 type, u8 num)
{
    u32 user_laddr;
    switch(type){
        case B_MP3:
            if(num > user_msc.mp3_file_num){
                return false;
            }else{
                user_laddr = (USER_READ_ADDR + user_msc.mp3_chunk_addr + (num-1)*0x10);
            }
            break;
        case B_VMP3:
            if(num > user_msc.vmp3_file_num){
                return false;
            }else{
                user_laddr = (USER_READ_ADDR + user_msc.vmp3_chunk_addr + (num-1)*0x10);
            }
            break;

        case B_LSBC:
            if(num > user_msc.lsbc_file_num){
                return false;
            }else{
                user_laddr = (USER_READ_ADDR + user_msc.lsbc_chunk_addr + (num-1)*0x10);
            }
            break;

        case B_ESBC:
            if(num > user_msc.esbc_file_num){
                return false;
            }else{
                user_laddr = (USER_READ_ADDR + user_msc.esbc_chunk_addr + (num-1)*0x10);
            }
            break;

        case B_WAV:
            if(num > user_msc.wav_file_num){
                return false;
            }else{
                user_laddr = (USER_READ_ADDR + user_msc.wav_chunk_addr + (num-1)*0x10);
            }
            break;

        default:
            return false;
    }
    memcpy(addr, (u8*)user_laddr, 4);
    *addr += USER_READ_ADDR;
    memcpy(len, (u8*)(user_laddr + 4), 4);
    return true;
}

#endif