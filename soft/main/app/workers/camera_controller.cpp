#include <app/workers/camera_controller.h>
#include <hal/log.h>
#include <os/task.h>

CameraController::CameraController(float period, DataRessourcesRegistry * registry, Camera * camera):
    PeriodicTask("camera_controller", Task::Priority::VERY_LOW, 8 * 1024, (int)(period * 1000), false)
{
    _registry = registry;
    _camera   = camera;

    registry->internal_set<string>("camera.status", "off");
    registry->internal_set<bool>("camera.recording", false);
    registry->internal_set<float>("camera.angle", 0.0);

    LOG_INFO("Init done");
}

void CameraController::run(void)
{
    int battery_level;

    if (_camera->is_connected())
    {
        battery_level = _camera->get_battery_level();
        LOG_INFO("Camera battery %d %%", battery_level);

    }
}
