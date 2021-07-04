#pragma once

#include <stdint.h>

class Filter
{

public:

    Filter() {}
    virtual ~Filter() {}

    virtual float apply(float input) { return input; }

};

class PTFilter : Filter
{

protected:

    float _gain;
    float _period;

public:

    PTFilter(float period, float f_cut);
    virtual ~PTFilter() {}

    virtual void update_fcut(float f_cut) {}
};

class PT1Filter : PTFilter
{

private:

    float _state;

public:

    PT1Filter(float period, float f_cut);

    void update_fcut(float f_cut);

    float apply(float input);
};

class PT2Filter : PTFilter
{

private:

    float _state[2];

public:

    PT2Filter(float period, float f_cut);

    void update_fcut(float f_cut);

    float apply(float input);
};

class PT3Filter : PTFilter
{

private:

    float _state[3];

public:

    PT3Filter(float period, float f_cut);

    void update_fcut(float f_cut);

    float apply(float input);
};

class SlewFilter : Filter
{

private:

    float _slew_limit;
    float _threshold;
    float _state;

public:

    SlewFilter(float slew_limit, float threshold);

    float apply(float input);
};

class BiQuadFilter : Filter
{

protected:

    float _b0, _b1, _b2, _a1, _a2;
    float _x1, _x2, _y1, _y2;

    /* Get notch filter Q given center frequency (f0) and lower cutoff frequency (f1) */
    float _compute_Q(float center_freq, float cutoff_freq);

protected:

    virtual void _update_terms(float filter_freq, uint32_t refresh_rate, float Q);

public:

    BiQuadFilter(float filter_freq, uint32_t refresh_rate, float Q);
    BiQuadFilter(float filter_freq, uint32_t refresh_rate, float center_freq, float cutoff_freq);

    /* Computes a biquad filter on a sample (slightly less precise than df2 but works in dynamic mode) */
    float apply_df1(float input);

    /* Computes a biquad filter in direct form 2 on a sample (higher precision but can't handle changes in coefficients) */
    float apply_df2(float input);
    float apply(float input);
};

class BiQuadLfpFilter : BiQuadFilter
{

private:

    void _update_terms(float filter_freq, uint32_t refresh_rate, float Q);

public:
    BiQuadLfpFilter(float filter_freq, uint32_t refresh_rate);
};

class BiQuadNotchFilter : BiQuadFilter
{

private:

    void _update_terms(float filter_freq, uint32_t refresh_rate, float Q);

public:

    BiQuadNotchFilter(float filter_freq, uint32_t refresh_rate, float center_freq, float cutoff_freq);
};

class BiQuadBfpFilter : BiQuadFilter
{

private:

    void _update_terms(float filter_freq, uint32_t refresh_rate, float Q);

public:

    BiQuadBfpFilter(float filter_freq, uint32_t refresh_rate, float center_freq, float cutoff_freq);
};