#ifndef _MIDI_UART_H
#define _MIDI_UART_H

#define SYS_CLK                     24000000
#define UART_CMD_MAX_SIZE           64
#define UART_RX_BUF_LEN             128

#define UART_HEAD_ONE               0x55       //帧头
#define UART_HEAD_TWO               0xAA

enum
{
    UART_STEP_HEAD_ONE,       //帧头
    UART_STEP_HEAD_TWO,
    UART_STEP_LENGTH,         //长度
    UART_STEP_CMDID,          //命令处理
    UART_STEP_MIDI_MESSAGE,   //midi消息
};

typedef enum
{
    UART_STATE_INIT = 0,
    UART_STATE_INIT_FINISH,
    UART_STATE_SYNC,
    UART_STATE_CONNECTED,
}UART_STATE_TYPE;

//brief	command packet struct
typedef enum
{
    CMDID_MIDI_TO_BLE            = 0xB0,
    CMDID_MIDI_TO_BR             = 0xB1,
    CMDID_MIDI_TO_USBOUT         = 0xB2,
    CMDID_MIDI_TO_BOTH           = 0xB3,

    CMDID_HANDSHAKE_STATE        = 0x00,
    CMDID_SYS_INIT_CMPLT_STATE   = 0x01,
    CMDID_USB_CONNECT_STATE      = 0x02,

    CMDID_KEYBOARD_PEDAL_CTR     = 0x10,
}UART_CMD_ID;

enum
{
    MIDI_MESSAGE_NOTE_ON        = 0x90,
    MIDI_MESSAGE_NOTE_OFF       = 0x80,
    MIDI_MESSAGE_CTRL_CHANGE    = 0xB0,//Bn Control Change
    MIDI_MESSAGE_PROG_CHANGE    = 0xC0,//Cn Program Change
    MIDI_MESSAGE_PITCH_BEND     = 0xE0,//En Pitch Bend
};

typedef struct
{
    u8 midi_message;           //midi标准事件

    u8 decodeStep;
    bool getHeader1;
    bool getHeader2;

    u8 uLength;
    volatile u8 w_cnt;
    volatile u8 r_cnt;
    u8 DataBuf[UART_CMD_MAX_SIZE];//max is 64byte
}MIDI_UART_DECODE_PACKET;

void midi_uart0_isr(void);
void midi_udet_init(u32 baudrate);
void midi_register_uart0_isr_init(isr_t isr);
void midi_uart_sysinit_finish(void);
void midi_uart_sysn_response(void);
void midi_uart_rx_process(void);
void midi_uart_tx_process(void);

#endif
