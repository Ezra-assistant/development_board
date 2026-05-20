#ifndef _API_FFT_H
#define _API_FFT_H

void fft_forward(s32 *buf);
void fft_inverse(s32* buf);
void fft_512_init(void);
void fft_256_init(void);
void fft_128_init(void);
#endif
