#include <math.h>
#include <utils/filter.h>
#include <esp_attr.h>

#define M_PIf 3.14159265359

void PTFilter::init()
{
    this->init_state();
    this->update_cutoff_freq(this->_cutoff_freq);
}

/* PT1 Low Pass filter */

void PT1Filter::init_state(void)
{
    this->_state = 0.0f;
}

void IRAM_ATTR PT1Filter::update_cutoff_freq(float cutoff_freq)
{
    this->_cutoff_freq = cutoff_freq;
    float rc = 1 / (2 * M_PIf * cutoff_freq);
    this->_gain = this->_period / (rc + this->_period);
}

float IRAM_ATTR PT1Filter::apply(float input)
{
    this->_state = this->_state + this->_gain * (input - this->_state);
    return this->_state;
}

/* PT2 Low Pass filter */

void PT2Filter::init_state(void)
{
    this->_state[0] = 0.0f;
    this->_state[1] = 0.0f;
}

void IRAM_ATTR PT2Filter::update_cutoff_freq(float cutoff_freq)
{
    const float order = 2.0f;
    const float order_cutoff_correction = 1 / sqrtf(powf(2, 1.0f / order) - 1);
    float rc = 1 / (2 * order_cutoff_correction * M_PIf * cutoff_freq);
    this->_gain = this->_period / (rc + this->_period);
}

float IRAM_ATTR PT2Filter::apply(float input)
{
    this->_state[1] = this->_state[1] + this->_gain * (input - this->_state[1]);
    this->_state[0] = this->_state[0] + this->_gain * (this->_state[1] - this->_state[0]);

    return this->_state[0];
}

/* PT3 Low Pass filter */

void PT3Filter::init_state(void)
{
    this->_state[0] = 0.0f;
    this->_state[1] = 0.0f;
    this->_state[2] = 0.0f;
}

void IRAM_ATTR PT3Filter::update_cutoff_freq(float cutoff_freq)
{
    const float order = 3.0f;
    const float order_cutoff_correction = 1 / sqrtf(powf(2, 1.0f / order) - 1);
    float rc = 1 / (2 * order_cutoff_correction * M_PIf * cutoff_freq);
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

float IRAM_ATTR SlewFilter::apply(float input)
{
    if (((input - this->_state) / this->_period) > this->_slew_limit)
    {
        this->_state += this->_slew_limit * this->_period;
    }
    else if (((input - this->_state) / this->_period) < -this->_slew_limit)
    {
        this->_state -= this->_slew_limit * this->_period;
    }
    else
    {
        this->_state = input;
    }

    return this->_state;
}

/* BiQuadratic filters */

BiQuadraticFilter::BiQuadraticFilter(float period, float center_freq, float Q) :
    Filter()
{
    this->_center_freq = center_freq;
    this->_period      = period;
    this->_Q           = Q;
}

void BiQuadraticFilter::update_internal(float center_freq, float Q)
{
    this->_center_freq = center_freq;
    this->_Q           = Q;

    this->_update_terms();
}

void BiQuadraticFilter::init(void)
{
    this->_update_terms();

    // zero initial samples
    this->_x1 = this->_x2 = 0;
    this->_y1 = this->_y2 = 0;
}

float IRAM_ATTR BiQuadraticFilter::apply_direct_form_1(float input)
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

float IRAM_ATTR BiQuadraticFilter::apply_direct_form_2(float input)
{
    const float result = this->_b0 * input + this->_x1;
    this->_x1 = this->_b1 * input - this->_a1 * result + this->_x2;
    this->_x2 = this->_b2 * input - this->_a2 * result;
    return result;
}

float IRAM_ATTR BiQuadraticFilter::apply(float input)
{
    return this->apply_direct_form_1(input);
}

BiQuadraticLowPassFilter::BiQuadraticLowPassFilter(float period, float cutoff_freq)
    : BiQuadraticFilter(period, cutoff_freq, this->_compute_Q(cutoff_freq))
{
}

float BiQuadraticLowPassFilter::_compute_Q(float cutoff_freq)
{
    return 1.0f / sqrtf(2.0f);    /* quality factor - 2nd order butterworth */
}

void IRAM_ATTR BiQuadraticLowPassFilter::_update_terms(void)
{
    const float omega = 2.0f * M_PIf * _center_freq * _period;
    const float sn = sinf(omega);
    const float cs = cosf(omega);
    const float alpha = sn / (2.0f * _Q);
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

void IRAM_ATTR BiQuadraticLowPassFilter::update(float cutoff_freq)
{
    this->update_internal(cutoff_freq, this->_compute_Q(cutoff_freq));
}

BiQuadraticNotchFilter::BiQuadraticNotchFilter(float period, float center_freq, float cutoff_freq)
    : BiQuadraticFilter(period, center_freq, _compute_Q(center_freq, cutoff_freq))
{
}

float IRAM_ATTR BiQuadraticNotchFilter::_compute_Q(float center_freq, float cutoff_freq)
{
    return center_freq * cutoff_freq / (center_freq * center_freq - cutoff_freq * cutoff_freq);
}

void IRAM_ATTR BiQuadraticNotchFilter::_update_terms(void)
{
    const float omega = 2.0f * M_PIf * _center_freq * _period;
    const float sn = sinf(omega);
    const float cs = cosf(omega);
    const float alpha = sn / (2.0f * _Q);
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

void IRAM_ATTR BiQuadraticNotchFilter::update(float center_freq, float cutoff_freq)
{
    this->update_internal(center_freq, this->_compute_Q(center_freq, cutoff_freq));
}

float IRAM_ATTR BiQuadraticBandPassFilter::_compute_Q(float center_freq, float cutoff_freq)
{
    return center_freq * cutoff_freq / (center_freq * center_freq - cutoff_freq * cutoff_freq);
}

BiQuadraticBandPassFilter::BiQuadraticBandPassFilter(float period, float center_freq, float cutoff_freq)
    : BiQuadraticFilter(period, center_freq, _compute_Q(center_freq, cutoff_freq))
{
}

void IRAM_ATTR BiQuadraticBandPassFilter::_update_terms(void)
{
    const float omega = 2.0f * M_PIf * _center_freq * _period;
    const float sn = sinf(omega);
    const float cs = cosf(omega);
    const float alpha = sn / (2.0f * _Q);
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

void IRAM_ATTR BiQuadraticBandPassFilter::update(float center_freq, float cutoff_freq)
{
    this->update_internal(center_freq, this->_compute_Q(center_freq, cutoff_freq));
}
