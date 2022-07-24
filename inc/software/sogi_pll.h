#ifndef SOGI_PLL_H
#define SOGI_PLL_H

extern float theta;

#define PLL_kp 0.004
#define PLL_ki 0.002

#define pi2 6.283185307179586476925286766559
#define pi20 62.83185307179586476925286766559
#define pi100 314.15926535897932384626433832795

typedef struct filterTag
{
    float x_k0;
    float y_k0, y_k1;
}filter;

filter SOGI_BandPassFilter50hz(filter LastfilterIn, float samp);
filter SOGI_LowPassFilter50hz(filter LastfilterIn, float samp);
void sogi_pll_Cal(int16 samp);

#endif
