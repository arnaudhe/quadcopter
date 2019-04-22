#include <hal/ultrasound.h>

#define HCSR04_ECHO_PIN         5
#define HCSR04_TRIG_CHANNEL     4
#define HCSR04_TRIG_PIN         4

UltrasoundSensor::UltrasoundSensor(void)
{
    _hc = new HcSr04(HCSR04_ECHO_PIN, HCSR04_TRIG_CHANNEL, HCSR04_TRIG_PIN);
}

float UltrasoundSensor::height()
{
    return _hc->height();
}