#include <app/workers/camera_controller.h>
#include <hal/log.h>
#include <os/task.h>

#define CAMERA_CONTROLLER_BATTERY_UPDATE_PERIOD     5000

CameraController::CameraController(float period, DataRessourcesRegistry * registry, Camera * camera):
    PeriodicTask("camera_controller", Task::Priority::VERY_LOW, 8 * 1024, (int)(period * 1000), false)
{
    _registry             = registry;
    _camera               = camera;
    _recording            = false;
    _battery_measure_time = Tick::now();

    registry->internal_set<bool>("camera.connected", false);
    registry->internal_set<int>("camera.battery", 0);
    registry->internal_set<bool>("camera.recording_request", false);
    registry->internal_set<bool>("camera.recording", false);
    registry->internal_set<float>("camera.tilt", 0.0);

    LOG_INFO("Init done");
}

void CameraController::run(void)
{
    int battery_level;

    if (_camera->is_connected())
    {
        _registry->internal_set<bool>("camera.connected", true);

        /* Battery management */
        if ((Tick::now() - _battery_measure_time).ticks() > CAMERA_CONTROLLER_BATTERY_UPDATE_PERIOD)
        {
            _battery_measure_time = Tick::now();
            battery_level = _camera->get_battery_level();
            if ((battery_level >= 0) && (battery_level <= 100))
            {
                _registry->internal_set<int>("camera.battery", battery_level);
                LOG_INFO("Camera battery %d %%", battery_level);
            }
        }

        /* Tilt management */
        // _gimbal->set_tilt( _registry->internal_get<float>("camera.tilt"));

        /* Recording management */
        if ((_recording == false) && (_registry->internal_get<bool>("camera.recording_request") == true))
        {
            if (_camera->start_recording())
            {
                LOG_INFO("Recording started");
                _recording = true;
                _registry->internal_set<bool>("camera.recording", true);
            }
        }
        else if ((_recording == true) && (_registry->internal_get<bool>("camera.recording_request") == false))
        {
            if (_camera->stop_recording())
            {
                LOG_INFO("Recording stopped");
                _recording = false;
                _registry->internal_set<bool>("camera.recording", false);
            }
        }
    }
    else
    {
        _registry->internal_set<bool>("camera.connected", false);
    }
}
