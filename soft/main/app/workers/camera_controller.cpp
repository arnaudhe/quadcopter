#include <app/workers/camera_controller.h>
#include <hal/log.h>
#include <os/task.h>

CameraController::CameraController(float period, DataRessourcesRegistry * registry):
    PeriodicTask("camera_controller", Task::Priority::VERY_LOW, (int)(period * 1000), false)
{
    _registry = registry;

    registry->internal_set<string>("camera.status", "off");
    registry->internal_set<bool>("camera.angle", false);
    registry->internal_set<float>("camera.angle", 0.0);

    LOG_INFO("Init done");
}

void CameraController::run(void)
{
    return;
}
