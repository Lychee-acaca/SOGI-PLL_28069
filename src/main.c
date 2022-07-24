#include "DSP28x_Project.h"
#include "sogi_pll.h"

#define Epwm_Frequency 4500
#define ZeroVoltage 2022 //0V基准

void initialization(void);
interrupt void adc_isr(void);

int main(void)
{
    initialization();

    EALLOW;
    GpioCtrlRegs.GPAPUD.bit.GPIO8 = 0;
    GpioCtrlRegs.GPAMUX1.bit.GPIO8 = 0;
    GpioCtrlRegs.GPADIR.bit.GPIO8 = 1;
    GpioDataRegs.GPASET.bit.GPIO8 = 1;
    EDIS;

    while(1)
    {

    }
}

void initialization(void)
{
    InitSysCtrl();
    DINT;
    InitPieCtrl();
    IER = 0x0000;
    IFR = 0x0000;
    PieCtrlRegs.PIECTRL.bit.ENPIE = 1;

    // flash download
/*
    memcpy(&RamfuncsRunStart, &RamfuncsLoadStart, (Uint32)&RamfuncsLoadSize);
    InitFlash();
*/

    //INT1.1, 这是ADC的中断
    EALLOW;
    PieVectTable.ADCINT1 = &adc_isr; // 中断向量表绑定
    EDIS;
    PieCtrlRegs.PIEIER1.bit.INTx1 = 1;
    IER |= M_INT1;

    InitAdc();
    AdcOffsetSelfCal();

    EALLOW;
    AdcRegs.ADCCTL1.bit.INTPULSEPOS = 1;
    AdcRegs.INTSEL1N2.bit.INT1E = 1;
    AdcRegs.INTSEL1N2.bit.INT1CONT = 0;
    AdcRegs.INTSEL1N2.bit.INT1SEL = 0;

    AdcRegs.ADCSOC0CTL.bit.CHSEL = 0; // A0
    AdcRegs.ADCSOC0CTL.bit.TRIGSEL = 9;
    AdcRegs.ADCSOC0CTL.bit.ACQPS = 6;
    EDIS;

    EPwm3Regs.ETSEL.bit.SOCAEN = 1;
    EPwm3Regs.ETSEL.bit.SOCASEL = 3;
    EPwm3Regs.ETPS.bit.SOCAPRD = 1;

    EPwm3Regs.TBPRD = Epwm_Frequency;
    EPwm3Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN;

    EPwm3Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1;
    EPwm3Regs.TBCTL.bit.CLKDIV = TB_DIV1;

    EPwm3Regs.TBPHS.half.TBPHS = 0x0000;
    EPwm3Regs.TBCTL.bit.PHSEN = TB_ENABLE;

    EPwm3Regs.TBCTL.bit.SYNCOSEL = TB_CTR_ZERO;

    EINT; // 使能全局中断
    ERTM; // 使能全局中断DBGM
}

interrupt void adc_isr(void)
{
    Uint16 VacSample; //采样值

    //采样
    VacSample = AdcResult.ADCRESULT0; //A0

    //sogi_pll计算
    sogi_pll_Cal(VacSample - ZeroVoltage);

    //锁相环测试代码
    if (theta >=6.283185307179586476925286766559 * 180 / 360)
    {
        GpioDataRegs.GPADAT.bit.GPIO8 = 0;
    } else
    {
        GpioDataRegs.GPADAT.bit.GPIO8 = 1;
    }

    AdcRegs.ADCINTFLGCLR.bit.ADCINT1 = 1;
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}

