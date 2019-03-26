#include <app/observers/euler_observer.h>
#include <app/observers/euler_observer_conf.h>

EulerObserver::EulerObserver(float period) :
   Madgwick(period, EULER_OBSERVER_GAIN)
{
}