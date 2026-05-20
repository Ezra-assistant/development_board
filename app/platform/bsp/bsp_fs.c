#include "include.h"

#if FUNC_MUSIC_EN
#if MUSIC_MP3_LOOPBACK_EN
u32 loopback_frame;
u32 music_loopback_end = 0;

u32 music_get_mp3_frame_length(void);
void music_qskip_frame(bool direct, u32 frame);
u32 fs_get_file_pos(void);

//SD音乐循环，字节转换为帧
u32 music_byte_to_frame(u32 byte)
{
    u32 frame = 0, frame_len;
    frame_len = music_get_mp3_frame_length();
    frame = byte / frame_len;
    return frame;
}

//SD音乐循环,首尾地址偏移
AT(.com_text.music_loopback)
void music_mp3_loopback_size(u32 start_byte_offset, u32 end_byte_offset)
{
    u32 music_size = fs_get_file_size();
    if (music_size > (start_byte_offset + end_byte_offset)) {
        loopback_frame = music_byte_to_frame(music_size - start_byte_offset - end_byte_offset); //循环的帧数
        music_loopback_end = music_size - end_byte_offset;
        u32 start_frame = music_byte_to_frame(start_byte_offset);                               //开头偏移的帧数
        music_qskip_frame(0, start_frame);                                                      //往前进start_frame帧
    }
}

//SD音乐循环，回到循环播放的起点
AT(.text.music_loopback)
void music_mp3_loopback_replay(void)
{
    if (fs_get_file_pos() >= music_loopback_end && music_loopback_end != 0 ) {
        music_qskip_frame(1, loopback_frame);                                                   //往后退loopback_frame帧
    }
}
#endif

extern u8 fname_buf[100];
#define IsUpper(c)  (((c)>='A')&&((c)<='Z'))
#define IsLower(c)  (((c)>='a')&&((c)<='z'))

//#define MUSIC_EXT_NUM 6                         //一共搜索3个扩展名
//const char tbl_music_ext[MUSIC_EXT_NUM][4] = {"mp3", "wav", "wma", "ape", "fla", "sbc"};

//转换为小写字符
AT(.text.fs.scan)
unsigned char char_tolower(unsigned char c)
{
	if (c >= 'A' && c <= 'Z') {
		c -= 'A'-'a';
	}
	return c;
}

//将字符串转换为小写
AT(.text.fs.scan)
void str_tolower(char *str, uint len)
{
    uint i;
    for (i = 0; i < len; i++) {
        str[i] = char_tolower(str[i]);
    }
}

AT(.text.fs.scan)
static bool file_is_music(char *ext_name)
{
//    u8 i;
//    for (i=0; i != MUSIC_EXT_NUM; i++) {
//        if (0 == memcmp(ext_name, (char *)(tbl_music_ext[i]), 3)) {
//            return true;
//        }
//    }
//    return false;

    if (0 == memcmp(ext_name, "mp3", 3)) {
        return true;
    }
#if MUSIC_WAV_SUPPORT
    if (0 == memcmp(ext_name, "wav", 3)) {
        return true;
    }
#endif // MUSIC_WAV_SUPPORT

//#if MUSIC_SBC_SUPPORT
//    if (0 == memcmp(ext_name, "sbc", 3)) {
//        return true;
//    }
//#endif // MUSIC_SBC_SUPPORT
    return false;
}

///返回值： 0->不匹配的文件类型， 1->匹配的文件类型
AT(.text.fs.scan)
u8 music_file_filter(void)
{
    char extension[3];
    fs_get_fname_extension(extension);
    str_tolower(extension, 3);
    if (file_is_music(extension)) {
#if REC_FAST_PLAY
        bsp_update_final_rec_file_num();
#endif // REC_FAST_PLAY
        return 1;
    }
    return 0;
}

///返回值： 0->不匹配的目录类型， 1->匹配的目录类型
AT(.text.fs.scan)
u8 music_dir_filter(void)
{
#if REC_FAST_PLAY
    char sfn[13];
    fs_get_short_fname(sfn, 1);

    //录音目录
    if (is_record_dir(sfn)) {
        if (f_msc.rec_scan & BIT(0)) {
            f_msc.rec_scan |= BIT(1);
        }
    } else {
        f_msc.rec_scan &= ~BIT(1);
    }
#endif // REC_FAST_PLAY

    return 1;
}

#if MUSIC_REC_FILE_FILTER
///只播放录音文件歌曲
u8 music_only_record_dir_filter(void)
{
    char sfn[13];
    fs_get_short_fname(sfn, 1);

    //录音目录
    if (is_record_dir(sfn)) {
#if REC_FAST_PLAY
        if (f_msc.rec_scan & BIT(0)) {
            f_msc.rec_scan |= BIT(1);
        }
#endif // REC_FAST_PLAY
        return 1;
    }
    return 0;
}

///只播放录音文件歌曲
u8 music_only_record_file_filter(void)
{
    char extension[3];
    fs_get_fname_extension(extension);
    str_tolower(extension, 3);
    if (file_is_music(extension) && fs_get_dir_depth()) {   //去掉根目录正常歌曲
#if REC_FAST_PLAY
        bsp_update_final_rec_file_num();
#endif // REC_FAST_PLAY
        return 1;
    }
    return 0;
}

///去掉录音文件
u8 music_rm_record_dir_filter(void)
{
    char sfn[13];
    fs_get_short_fname(sfn, 1);

    //录音目录
    if (is_record_dir(sfn)) {
        return 0;
    }

    return 1;
}
#endif // MUSIC_REC_FILE_FILTER

#if REC_FAST_PLAY
void bsp_update_final_rec_file_num(void)
{
    if (fs_get_dir_depth() && (f_msc.rec_scan & BIT(1))) {
        if (fs_get_ftime() > sys_cb.ftime) {
            sys_cb.ftime = fs_get_ftime();
            sys_cb.rec_num = fs_get_file_count();
        }
    }
}
#endif // REC_FAST_PLAY

AT(.text.stream)
int stream_read(void *buf, unsigned int size)
{
    UINT len;
    u8 res = fs_read(buf, size, &len);
#if MUSIC_MP3_LOOPBACK_EN
    music_mp3_loopback_replay();
#endif
    if (res == FR_OK) {
#if MUSIC_ENCRYPT_EN
        if (f_msc.encrypt) {
            music_stream_decrypt(buf, len);
        }
#endif
        return len;
    } else {
        return -1;
    }
}

AT(.text.stream)
bool stream_seek(unsigned int ofs, int whence)
{
#if MUSIC_ENCRYPT_EN
    if (f_msc.encrypt) {
        if (whence == SEEK_SET) {
            ofs += 1;
        }
    }
#endif
    u8 res = fs_lseek(ofs, whence);
    if (res == FR_OK) {
        return true;
    }
    return false;
}

#if MUSIC_FILE_SORT_CHAR_EN
static bool bo_sort_en = false;
static bool bo_sys_en = false;
file_sort_var_t file_sort_var AT(.pff_sort.buf);
file_sort_t file_sort_buf_ONE[MUSIC_FILE_SORT_NUM] AT(.pff_sort.buf);
file_sort_t file_sort_buf_TWO[MUSIC_FILE_SORT_NUM] AT(.pff_sort.buf);
AT(.text.file_sort)
bool file_sort_en(void)
{
    return bo_sort_en;
}
AT(.text.file_sort)
void set_sort_en(bool boset)
{
    bo_sort_en = boset;
}
AT(.text.file_sort)
u16 file_sort_num(void)
{
    return MUSIC_FILE_SORT_NUM;
}

AT(.text.file_sort)
void file_sort_init(void)
{
    memset(file_sort_buf_ONE, 0, sizeof(file_sort_buf_ONE));
    memset(file_sort_buf_TWO, 0, sizeof(file_sort_buf_TWO));
    memset(&file_sort_var, 0, sizeof(file_sort_var));
    file_sort_var.dir_01_save_num = 1;
    file_sort_var.dir_02_save_num = 1;
    set_sort_en(true);
    bo_sys_en = false;
}

AT(.text.file_sort)
void dir_num_deal(char *p_dir_fname)
{
    if(!bo_sys_en){
        bo_sys_en = true;
        if(!memcmp(p_dir_fname, "SYS", 3)){
            printf("system_insert\n");
        }else{
            file_sort_var.dir_cur_num++;
        }
    }
    if(0 == memcmp(p_dir_fname, "01", 2)){
        file_sort_var.file_01_offset = file_sort_var.dir_cur_num;
    }else if(0 == memcmp(p_dir_fname, "02", 2)){
        file_sort_var.file_02_offset = file_sort_var.dir_cur_num;
    }else if(0 == memcmp(p_dir_fname, "03", 2)){
        file_sort_var.file_03_offset = file_sort_var.dir_cur_num;
    }
    file_sort_var.dir_cur_num++;
}

AT(.text.file_sort)
void file_sort_char_deal(u16 dir_count, char *p_file_fname, u16 file_count)
{
    file_sort_t temp_file_sort_1;
    file_sort_t temp_file_sort_2;
    file_sort_t *p_sort1 = file_sort_buf_ONE;
    file_sort_t *p_sort2 = file_sort_buf_TWO;
    // printf("offset:%d, %d, %d\n", file_sort_var.file_01_offset, file_sort_var.file_02_offset, dir_count);
    if(file_sort_var.file_01_offset == dir_count){
        temp_file_sort_1.file_count = file_count;
        memcpy(&temp_file_sort_1.fname, p_file_fname, 5);
        // printf("temp_file_sort_1.fname:[%s]\n", temp_file_sort_1.fname);

        if(1 == file_sort_var.dir_01_save_num){//第一首直接放数组1
            p_sort1[file_sort_var.dir_01_save_num] = temp_file_sort_1;
            file_sort_var.dir_01_save_num++;
        }else if((file_sort_var.dir_01_save_num > 1) && (file_sort_var.dir_01_save_num < MUSIC_FILE_SORT_NUM)){
            for(u8 i = 1; i <= file_sort_var.dir_01_save_num; i++){//从数组1开始比较到数组n
                temp_file_sort_1 = p_sort1[i];

                int re_sta = memcmp(p_file_fname ,&temp_file_sort_1.fname, 3);
                if(!re_sta){
                    printf("dir1 file name error\n");
                    break;
                }else if(re_sta < 0){//s1 < s2
                    for(u8 j = file_sort_var.dir_01_save_num; j > i; j--){
                        p_sort1[j] = p_sort1[j-1];
                    }
                    temp_file_sort_1.file_count = file_count;
                    memcpy(&temp_file_sort_1.fname, p_file_fname, 3);
                    p_sort1[i] = temp_file_sort_1;//插值
                    break;
                }

                if(i == file_sort_var.dir_01_save_num){//如果遍历到数组n还未找到比之大的，则直接放最后也就是数组n位置
                    temp_file_sort_1.file_count = file_count;
                    memcpy(&temp_file_sort_1.fname, p_file_fname, 3);
                    p_sort1[file_sort_var.dir_01_save_num] = temp_file_sort_1;
                }
            }
            file_sort_var.dir_01_save_num++;
        }else{
            printf("dir_01_save_num error\n");
        }
    }else if(file_sort_var.file_02_offset == dir_count){
        temp_file_sort_2.file_count = file_count;
        memcpy(&temp_file_sort_2.fname, p_file_fname, 5);
        // printf("temp_file_sort_2.fname:[%s]\n", temp_file_sort_2.fname);

        if(1 == file_sort_var.dir_02_save_num){//第一首直接放数组1
            p_sort2[file_sort_var.dir_02_save_num] = temp_file_sort_2;
            file_sort_var.dir_02_save_num++;
        }else if((file_sort_var.dir_02_save_num > 1) && (file_sort_var.dir_02_save_num < MUSIC_FILE_SORT_NUM)){
            for(u8 i = 1; i <= file_sort_var.dir_02_save_num; i++){//从数组1开始比较到数组n
                temp_file_sort_2 = p_sort2[i];

                int re_sta = memcmp(p_file_fname ,&temp_file_sort_2.fname, 3);
                if(!re_sta){
                    printf("dir2 file name error\n");
                    break;
                }else if(re_sta < 0){//s1 < s2
                    for(u8 j = file_sort_var.dir_02_save_num; j > i; j--){
                        p_sort2[j] = p_sort2[j-1];
                    }
                    temp_file_sort_2.file_count = file_count;
                    memcpy(&temp_file_sort_2.fname, p_file_fname, 3);
                    p_sort2[i] = temp_file_sort_2;//插值
                    break;
                }

                if(i == file_sort_var.dir_02_save_num){//如果遍历到数组n还未找到比之大的，则直接放最后也就是数组n位置
                    temp_file_sort_2.file_count = file_count;
                    memcpy(&temp_file_sort_2.fname, p_file_fname, 3);
                    p_sort2[file_sort_var.dir_02_save_num] = temp_file_sort_2;
                }
            }
            file_sort_var.dir_02_save_num++;
        }else{
            printf("dir_01_save_num error\n");
        }
    }else{

    }
    return;
}
#endif //MUSIC_FILE_SORT_CHAR_EN

AT(.text.fs.rename)
void unicode_to_ascii(char *dest, char *source, u8 CpySize)
{
    for(u32 i = 0; i < CpySize; i++){
        dest[i] = source[i*2];
    }
}

AT(.text.fs.rename)
void ascii_to_unicode(char *uni_ptr, char *sfn_ptr, u8 CpySize)
{
    for (u8 i = 0; i < CpySize; i++) {
        uni_ptr[i*2] = sfn_ptr[i];
    }
}

AT(.text.fs.rename)
void filename_convert_upper(char *inbuf,  u16 buf_len)
{//短文件名判断需要全部都是大写字母,包括后缀名
    char tempBuf[100];
    memset(tempBuf, 0, sizeof(tempBuf));
    memcpy(tempBuf, inbuf, buf_len);
    for(u8 i = 0; i < buf_len; i++){
        if((tempBuf[i] != 0) && (tempBuf[i] != ' ')){
            if(IsLower(tempBuf[i])){
                *inbuf++ = tempBuf[i] - 0x20;
            }else{
                *inbuf++ = tempBuf[i];
            }
        }
    }
}

AT(.text.fs.rename)
void filename_dirname_link(
    char *infilename,
    u8 infilename_len,
    u8 file_LSFN_sta,   /*0-infilename is unicode, else is ascii*/

    char *indirname,
    u8 indirname_len,
    u8 dir_LSFN_sta,    /*0-indirname is unicode, else is ascii*/

    char *outfullname,  /*return ascii*/
    u8 outfullname_len
)
{
    u8 name_len = 0;
    char *outfullname_ptr = outfullname;
    memset(outfullname, 0, outfullname_len);

    if(*indirname != 0){//sub
        if(dir_LSFN_sta == 0){//获取到长文件夹名
        unicode_to_ascii(outfullname, indirname, indirname_len);
        while(*outfullname_ptr != 0xFF){//指向第1个ff
            outfullname_ptr++;
            name_len++;
        }
        *(outfullname_ptr - 1) = '/';
        memset(outfullname_ptr, 0, (outfullname_len- name_len));
        }else{//获取到短文件夹名
            memcpy(outfullname, indirname, indirname_len);
            outfullname_ptr += indirname_len;
            *outfullname_ptr = '/';
            outfullname_ptr++;
            name_len += (indirname_len + 1);
        }
    }else{//root

    }

    if(file_LSFN_sta == 0){//获取到长文件名
        unicode_to_ascii(outfullname_ptr, infilename, infilename_len);
    }else{//获取到短文件夹名
        memcpy(outfullname_ptr, infilename, infilename_len);
    }
    name_len += infilename_len;
    outfullname[outfullname_len - 2] = name_len;
}

//测试pf_rename代码
#define RENAME_ROOT_OR_SUB  1
AT(.text.fs.rename)
void pf_rename_test_proc()
{
//     static u8 file_LSFN_sta = 0;//0-获取到长文件名,1-获取到短文件名
//     static u8 name_num = 0x31;
//
//     char dir_path_buf[100];
//     static u8 dir_LSFN_sta = 0;
//
//     char oldname_path[100];
//     u16 oldname_path_len;
//     char oldname_buf[102];
//
//     char frename_buf[102];
//     // u16 frename_path_len;
//     u16 frename_file_len;
//
// #if RENAME_ROOT_OR_SUB
//     char frename_path[] = "F1/1aB.mp3";
//     //char frename_path[] = "F1ABCDEFGHMN12/3aB.mp3";
//     //char frename_dir[] = "F1ABCDEFGHMN12/";
//     char frename_file[] = "1aB.mp3";
//
//     //char frename_path2[]= "F1/1zxc.mp3";
// #else
//     char frename_path[] = "1aB.mp3";
//     char frename_file[] = "1aB.mp3";
// #endif
//     memset(f_msc.fname, 0, sizeof(fname_buf));
//     memset(dir_path_buf, 0, sizeof(dir_path_buf));
//     fs_get_dirname_for_number(2, 1, dir_path_buf, sizeof(dir_path_buf), &dir_LSFN_sta);
//     printf("dir_path_buf :\n");
//     print_r(dir_path_buf, 100);
//
//     fs_get_filename_for_number(2, 1, f_msc.fname, sizeof(fname_buf), &file_LSFN_sta);//修改当前的文件名字
//     printf("music_lfn_name :\n");
//     print_r(f_msc.fname, 100);
//
//     if(file_LSFN_sta == 0){
//         memset(oldname_buf, 0, sizeof(oldname_buf));
//         oldname_path_len = (f_msc.fname[98]);
//         oldname_buf[0] = oldname_path_len & 0x0F;
//         oldname_buf[1] = oldname_path_len >> 8;
//         memcpy(&oldname_buf[2], f_msc.fname, 100);
//     }else{
//         oldname_path_len = (f_msc.fname[98]);
//     }
//
//     printf("file_LSFN_sta: %d\n", file_LSFN_sta);
//     printf("dir_LSFN_sta: %d\n", dir_LSFN_sta);
//     filename_dirname_link(
//         f_msc.fname,
//         f_msc.fname[98],
//         file_LSFN_sta,
//         dir_path_buf,
//         dir_path_buf[98],
//         dir_LSFN_sta,
//         oldname_path,
//         100);
//     printf("Name Link_oldname_path:\n");
//     print_r(oldname_path, 100);
//
//     if(file_LSFN_sta == 0){//长文件名
//         printf("!!! long file name !!!\n");
//     }else{//短文件名
//         printf("!!! short file name !!!\n");
//         // filename_convert_upper(f_msc.fname, f_msc.fname[98]);
//         // printf("music_lfn_name upper :\n");
//         // print_r(f_msc.fname, 100);
//
//         memset(oldname_buf, 0, sizeof(oldname_buf));
//         ascii_to_unicode(oldname_buf, f_msc.fname, oldname_path_len);
//     }
//
//     printf("oldname_buf :\n");
//     print_r(oldname_buf, 100);
//
//     #if RENAME_ROOT_OR_SUB
//         frename_path[3] = name_num;
//         frename_file[0] = name_num++;
//         if(name_num >= 0x39){
//             name_num = 0x31;
//         }
//     #else
//         frename_path[0] = name_num;
//         frename_file[0] = name_num++;
//         if(name_num >= 0x39){
//             name_num = 0x31;
//         }
//     #endif
//
//     memset(frename_buf, 0, sizeof(frename_buf));
//     // frename_path_len = (sizeof(frename_path)/(sizeof(char))) - 1;//减去'\0'
//     frename_file_len = (sizeof(frename_file)/(sizeof(char))) - 1;
//
//     printf("frename_path 8 :\n");
//     // print_r(frename_path, frename_path_len);
//
//     frename_buf[0] = frename_file_len  & 0x0F;
//     frename_buf[1] = frename_file_len>> 8;
//     ascii_to_unicode(&frename_buf[2], frename_file, frename_file_len);
//     printf("frename_buf:\n");
//     print_r(frename_buf, frename_file_len * 2 + 2);
//
//     fs_rename(oldname_path, frename_path, oldname_buf, frename_buf);
//     f_msc.file_total = fs_get_total_files();//改名字后，刷新下文件的序号
//     f_msc.dir_total = fs_get_dirs_count();
}

#endif // FUNC_MUSIC_EN
