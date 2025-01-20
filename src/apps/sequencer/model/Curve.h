#pragma once

class Curve {
    
public:
    typedef float (*Function)(float x);

    enum Type {
        Low,
        High,
        RampUp,
        RampDown,
        rampUpHalf,
        rampDownHalf,
        doubleRampUpHalf,
        doubleRampDownHalf,
        ExpUp,
        ExpDown,
        expUpHalf,
        expDownHalf,
        doubleExpUpHalf,
        doubleExpDownHalf,
        LogUp,
        LogDown,
        logUpHalf,
        logDownHalf,
        doubleLogUpHalf,
        doubleLogDownHalf,
        SmoothUp,
        SmoothDown,
        smoothUpHalf,
        smoothDownHalf,
        doubleSmoothUpHalf,
        doubleSmoothDownHalf,
        Triangle,
        RevTriangle,
        Bell,
        RevBell,
        StepUp,
        StepDown,
        ExpUp2x,
        ExpDown2x,
        ExpUp3x,
        ExpDown3x,
        ExpUp4x,
        ExpDown4x,
        Trigger,
        Last,
    };

 

    static Function function(Type type);

    static float eval(Type type, float x);

    static Type invAt(int i);
    static Type revAt(int i);
};
