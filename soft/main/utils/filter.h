#pragma once

#include <stdint.h>

class Filter
{

public:

    Filter() {}
    virtual ~Filter() {}

    virtual void init(void) = 0;
    virtual float apply(float input) { return input; }

};

class PTFilter : public Filter
{

protected:

    float _gain;
    float _period;
    float _cutoff_freq;

    virtual void init_state(void) = 0;

public:

    PTFilter(float period, float cutoff_freq) : Filter(), _period(period), _cutoff_freq(cutoff_freq) {}
    virtual ~PTFilter() {}

    void init();
    virtual void update_cutoff_freq(float cutoff_freq) = 0;
};

class PT1Filter : public PTFilter
{

private:

    float _state;

    void init_state(void);

public:

    PT1Filter(float period, float cutoff_freq) : PTFilter(period, cutoff_freq) {}

    void update_cutoff_freq(float cutoff_freq);

    float apply(float input);
};

class PT2Filter : public PTFilter
{

private:

    float _state[2];

    void init_state(void);

public:

    PT2Filter(float period, float cutoff_freq) : PTFilter(period, cutoff_freq) {}

    void update_cutoff_freq(float cutoff_freq);

    float apply(float input);
};

class PT3Filter : public PTFilter
{

private:

    float _state[3];

    void init_state(void);

public:

    PT3Filter(float period, float cutoff_freq) : PTFilter(period, cutoff_freq) {}

    void update_cutoff_freq(float cutoff_freq);

    float apply(float input);
};

class SlewFilter : public Filter
{

private:

    float _slew_limit;
    float _threshold;
    float _state;

public:

    SlewFilter(float slew_limit, float threshold) : Filter(), _slew_limit(slew_limit), _threshold(threshold), _state(0.0f) {}

    void init(void) {}

    float apply(float input);
};

/* Bi-quadratic filters */

class BiQuadraticFilter : public Filter
{

protected:

    float _period;
    float _center_freq, _Q;
    float _b0, _b1, _b2, _a1, _a2;
    float _x1, _x2, _y1, _y2;

protected:

    virtual void _update_terms(void) = 0;

public:

    BiQuadraticFilter(float period, float filter_freq, float Q);

    /* Computes a BiQuadratic filter on a sample (slightly less precise than df2 but works in dynamic mode) */
    float apply_direct_form_1(float input);

    /* Computes a BiQuadratic filter in direct form 2 on a sample (higher precision but can't handle changes in coefficients) */
    float apply_direct_form_2(float input);
    float apply(float input);

    void init(void);
};

/* Bi-quadratic low-pass */

class BiQuadraticLowPassFilter : public BiQuadraticFilter
{

private:

    void _update_terms(void);

    /* Get low-pass filter Q given cutoff frequency */
    float _compute_Q(float cutoff_freq);

public:

    BiQuadraticLowPassFilter(float period, float cutoff_freq);
};

/* Bi-quadratic notch (band-stop) */

class BiQuadraticNotchFilter : public BiQuadraticFilter
{

private:

    void _update_terms(void);

    /* Get notch filter Q given center frequency (f0) and lower cutoff frequency (f1) */
    float _compute_Q(float center_freq, float cutoff_freq);

public:

    BiQuadraticNotchFilter(float period, float center_freq, float cutoff_freq);
};

/* Bi-quadratic band-pass */

class BiQuadraticBandPassFilter : public BiQuadraticFilter
{

private:

    void _update_terms(void);

    /* Get band-pass filter Q given center frequency (f0) and lower cutoff frequency (f1) */
    float _compute_Q(float center_freq, float cutoff_freq);

public:

    BiQuadraticBandPassFilter(float period, float center_freq, float cutoff_freq);
};
