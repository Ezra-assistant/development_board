#ifndef _MIDI_KEYBOARD_H
#define _MIDI_KEYBOARD_H

#define MKEY_25     25      //midi note音符:48~72   
#define MKEY_32     32      //midi note音符:48~79
#define MKEY_49     49      //midi note音符:36~84
#define MKEY_61     61      //midi note音符:36~96
#define MKEY_88     88      //midi note音符:21~108

#if   (MIDI_KEYS_NUM == MKEY_25)
#define MKEY_NOTE_START         48              //midi键盘起始音符
#define MKEY_RANGE_MIN          KEY_MAT_1       //琴键起始
#define MKEY_RANGE_MAX          KEY_MAT_25      //琴键结束
#elif (MIDI_KEYS_NUM == MKEY_32)
#define MKEY_NOTE_START         48
#define MKEY_RANGE_MIN          KEY_MAT_1
#define MKEY_RANGE_MAX          KEY_MAT_32
#elif (MIDI_KEYS_NUM == MKEY_49)
#define MKEY_NOTE_START         36
#define MKEY_RANGE_MIN          KEY_MAT_1
#define MKEY_RANGE_MAX          KEY_MAT_49
#elif (MIDI_KEYS_NUM == MKEY_61)
#define MKEY_NOTE_START         36
#define MKEY_RANGE_MIN          KEY_MAT_1
#define MKEY_RANGE_MAX          KEY_MAT_61
#elif (MIDI_KEYS_NUM == MKEY_88)
#define MKEY_NOTE_START         21
#define MKEY_RANGE_MIN          KEY_MAT_1
#define MKEY_RANGE_MAX          KEY_MAT_88
#endif

#define MKEY_DEF_CH             MIDI_CHANNEL_1  //midi键盘默认使用channel1
#define MKEY_DEF_VELOCITY       100             //midi键盘默认力度



void midi_keyboard_msg_proc(u16 msg);
#endif
