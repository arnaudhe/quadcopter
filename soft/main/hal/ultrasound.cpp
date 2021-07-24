#include <hal/ultrasound.h>
#include <platform.h>

UltrasoundSensor::UltrasoundSensor(void)
{
    _hc = new HcSr04(PLATFORM_HCSR04_ECHO_PIN, PLATFORM_HCSR04_TRIG_CHANNEL, PLATFORM_HCSR04_TRIG_PIN);
}

float UltrasoundSensor::height()
{
    return _hc->height();
}