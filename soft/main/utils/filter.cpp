#include <math.h>
#include <utils/filter.h>
#include <esp_attr.h>

#define M_PIf 3.14159265359

#define BIQUAD_Q 1.0f / sqrtf(2.0f)     /* quality factor - 2nd order butterworth*/

PTFilter::PTFilter(float period, float f_cut):
    Filter(),
    _period(period)
{
  this->update_fcut(f_cut);
}

/* PT1 Low Pass filter */

PT1Filter::PT1Filter(float period, float f_cut):
    PTFilter(period, f_cut)
{
    this->_state = 0.0f;
}

void IRAM_ATTR PT1Filter::update_fcut(float f_cut)
{
    float rc = 1 / (2 * M_PIf * f_cut);
    this->_gain = this->_period / (rc + this->_period);
}

float IRAM_ATTR PT1Filter::apply(float input)
{
    this->_state = this->_state + this->_gain * (input - this->_state);
    return this->_state;
}

/* PT2 Low Pass filter */

PT2Filter::PT2Filter(float period, float f_cut)
    : PTFilter(period, f_cut)
{
    this->_state[0] = 0.0f;
    this->_state[1] = 0.0f;
}

void IRAM_ATTR PT2Filter::update_fcut(float f_cut)
{
    const float order = 2.0f;
    const float order_cutoff_correction = 1 / sqrtf(powf(2, 1.0f / order) - 1);
    float rc = 1 / (2 * order_cutoff_correction * M_PIf * f_cut);
    this->_gain = this->_period / (rc + this->_period);
}

float IRAM_ATTR PT2Filter::apply(float input)
{
    this->_state[1] = this->_state[1] + this->_gain * (input - this->_state[1]);
    this->_state[0] = this->_state[0] + this->_gain * (this->_state[1] - this->_state[0]);

    return this->_state[0];
}

/* PT3 Low Pass filter */

PT3Filter::PT3Filter(float period, float f_cut):
    PTFilter(period, f_cut)
{
    this->_state[0] = 0.0f;
    this->_state[1] = 0.0f;
    this->_state[2] = 0.0f;
}

void IRAM_ATTR PT3Filter::update_fcut(float f_cut)
{
    const float order = 3.0f;
    const float order_cutoff_correction = 1 / sqrtf(powf(2, 1.0f / order) - 1);
    float rc = 1 / (2 * order_cutoff_correction * M_PIf * f_cut);
    this->_gain = this->_period / (rc + this->_period);
}

float IRAM_ATTR PT3Filter::apply(float input)
{
    this->_state[2] = this->_state[2] + this->_gain * (input - this->_state[2]);
    this->_state[1] = this->_state[1] + this->_gain * (this->_state[2] - this->_state[1]);
    this->_state[0] = this->_state[0] + this->_gain * (this->_state[1] - this->_state[0]);

    return this->_state[0];
}

// Slew filter with limit

SlewFilter::SlewFilter(float slew_limit, float threshold):
    _slew_limit(slew_limit),
    _threshold(threshold),
    _state(0.0f)
{
}

float IRAM_ATTR SlewFilter::apply(float input)
{
    if (this->_state >= this->_threshold)
    {
        if (input >= this->_state - this->_slew_limit)
        {
            this->_state = input;
        }
    }
    else if (this->_state <= -this->_threshold)
    {
        if (input <= this->_state + this->_slew_limit)
        {
            this->_state = input;
        }
    }
    else
    {
        this->_state = input;
    }

    return this->_state;
}

/* BiQuad filter */

BiQuadFilter::BiQuadFilter(float filter_freq, uint32_t refresh_rate, float center_freq, float cutoff_freq)
{
    this->_update_terms(filter_freq, refresh_rate, this->_compute_Q(center_freq, cutoff_freq));

    // zero initial samples
    this->_x1 = this->_x2 = 0;
    this->_y1 = this->_y2 = 0;
}

BiQuadFilter::BiQuadFilter(float filter_freq, uint32_t refresh_rate, float Q)
{
    this->_update_terms(filter_freq, refresh_rate, Q);

    // zero initial samples
    this->_x1 = this->_x2 = 0;
    this->_y1 = this->_y2 = 0;
}

float IRAM_ATTR BiQuadFilter::_compute_Q(float center_freq, float cutoff_freq)
{
    return center_freq * cutoff_freq / (center_freq * center_freq - cutoff_freq * cutoff_freq);
}

float IRAM_ATTR BiQuadFilter::apply_df1(float input)
{
    /* compute result */
    const float result = this->_b0 * input + this->_b1 * this->_x1 + this->_b2 * this->_x2 - this->_a1 * this->_y1 - this->_a2 * this->_y2;

    /* shift x1 to x2, input to x1 */
    this->_x2 = this->_x1;
    this->_x1 = input;

    /* shift y1 to y2, result to y1 */
    this->_y2 = this->_y1;
    this->_y1 = result;

    return result;
}

float IRAM_ATTR BiQuadFilter::apply_df2(float input)
{
    const float result = this->_b0 * input + this->_x1;
    this->_x1 = this->_b1 * input - this->_a1 * result + this->_x2;
    this->_x2 = this->_b2 * input - this->_a2 * result;
    return result;
}

float IRAM_ATTR BiQuadFilter::apply(float input)
{
    return this->apply_df2(input);
}

BiQuadLfpFilter::BiQuadLfpFilter(float filter_freq, uint32_t refresh_rate)
    : BiQuadFilter(filter_freq, refresh_rate, BIQUAD_Q)
{
}

void IRAM_ATTR BiQuadLfpFilter::_update_terms(float filter_freq, uint32_t refresh_rate, float Q)
{
    const float omega = 2.0f * M_PIf * filter_freq * refresh_rate * 0.000001f;
    const float sn = sinf(omega);
    const float cs = cosf(omega);
    const float alpha = sn / (2.0f * Q);
    const float a0 = 1 + alpha;

    this->_b1 = 1 - cs;
    this->_b0 = this->_b1 * 0.5f;
    this->_b2 = this->_b0;
    this->_a1 = -2 * cs;
    this->_a2 = 1 - alpha;

    // precompute the coefficients
    this->_b0 /= a0;
    this->_b1 /= a0;
    this->_b2 /= a0;
    this->_a1 /= a0;
    this->_a2 /= a0;
}

void IRAM_ATTR BiQuadNotchFilter::_update_terms(float filter_freq, uint32_t refresh_rate, float Q)
{
    const float omega = 2.0f * M_PIf * filter_freq * refresh_rate * 0.000001f;
    const float sn = sinf(omega);
    const float cs = cosf(omega);
    const float alpha = sn / (2.0f * Q);
    const float a0 = 1 + alpha;

    this->_b0 = 1;
    this->_b1 = -2 * cs;
    this->_b2 = 1;
    this->_a1 = this->_b1;
    this->_a2 = 1 - alpha;

    // precompute the coefficients
    this->_b0 /= a0;
    this->_b1 /= a0;
    this->_b2 /= a0;
    this->_a1 /= a0;
    this->_a2 /= a0;
}

void IRAM_ATTR BiQuadBfpFilter::_update_terms(float filter_freq, uint32_t refresh_rate, float Q)
{
    const float omega = 2.0f * M_PIf * filter_freq * refresh_rate * 0.000001f;
    const float sn = sinf(omega);
    const float cs = cosf(omega);
    const float alpha = sn / (2.0f * Q);
    const float a0 = 1 + alpha;

    this->_b0 = alpha;
    this->_b1 = 0;
    this->_b2 = -alpha;
    this->_a1 = -2 * cs;
    this->_a2 = 1 - alpha;

    // precompute the coefficients
    this->_b0 /= a0;
    this->_b1 /= a0;
    this->_b2 /= a0;
    this->_a1 /= a0;
    this->_a2 /= a0;
}
