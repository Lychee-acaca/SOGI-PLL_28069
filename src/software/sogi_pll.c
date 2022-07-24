#include "DSP28x_Project.h"
#include "sogi_pll.h"
#include "math.h"

float theta = 0;

filter SOGI_BandPassFilter50hz(filter LastfilterIn, float samp)
{
    filter result;
    result.y_k1 = LastfilterIn.y_k0;
    result.x_k0 = samp;
    result.y_k0 = 1.9777942 * LastfilterIn.y_k0 - 0.97803547 * LastfilterIn.y_k1 + 0.021723205 * samp - 0.021723205 * LastfilterIn.x_k0;
    return result;
}

filter SOGI_LowPassFilter50hz(filter LastfilterIn, float samp)
{
    filter result;
    result.y_k1 = LastfilterIn.y_k0;
    //result.x_k0 = samp; //实际上没有用
    result.y_k0 = 1.9777942 * LastfilterIn.y_k0 - 0.97803547 * LastfilterIn.y_k1 + 0.00034123658 * samp;
    return result;
}

void sogi_pll_Cal(int16 samp)
{
    static filter Vac_a = {0}, Vac_b = {0};
    static float PLL_pi = 0, Vac_d = 0, Vac_d1 = 0; //pi值和坐标变换后的值

    //PLL滤波+坐标变换
    Vac_a = SOGI_BandPassFilter50hz(Vac_a, samp);
    Vac_b = SOGI_LowPassFilter50hz(Vac_b, samp);
    Vac_d = cos(theta) * Vac_a.y_k0 + sin(theta) * Vac_b.y_k0;

    //PLL控制
    PLL_pi += (Vac_d - Vac_d1) * PLL_kp + Vac_d * PLL_ki;
    Vac_d1 = Vac_d;

    if (PLL_pi > pi20)
        PLL_pi = pi20;
    else if (PLL_pi < -pi20)
        PLL_pi = -pi20;

    theta += 0.00005 * (pi100 + PLL_pi);
    if (theta > pi2)
    {
        theta -= pi2;
    } else if (theta < 0)
    {
        theta += pi2;
    }
}
