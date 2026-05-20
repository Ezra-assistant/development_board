#ifndef _FFT_PROC_H
#define _FFT_PROC_H

//FFT处理长度配置
#define FFT_512_EN              0       //处理512 byte(AB15X上处理不了,空间不足)
#define FFT_256_EN              1       //处理128 byte
#define FFT_128_EN              0       //处理64  byte

#if FFT_512_EN
#define PART_LEN                (256)
#elif FFT_256_EN
#define PART_LEN                (128)
#elif FFT_128_EN
#define PART_LEN                (64)
#endif

#define PART_LEN2               (PART_LEN << 1)

#define AEC_SPL_SAT(A, B, C)    (B > A ? A : B < C ? C : B)
#define AEC_SPL_WORD16_MAX      (32767)
#define AEC_SPL_WORD16_MIN      (-32768)
#define HANN_SHIFT              (16)

typedef struct{
	int fft[PART_LEN2];
	int xbuf[PART_LEN2];
	int outBuf[PART_LEN];
}nr_buf_t;

#if FFT_512_EN
    extern const unsigned short sqrtHanning[257];
#elif FFT_256_EN
    extern const unsigned short sqrtHanning[129];
#elif FFT_128_EN
    extern const unsigned short sqrtHanning[65];
#endif

#endif
