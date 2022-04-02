#pragma once

#include <string>

#include <os/periodic_task.h>

#include <data_model/data_ressources_registry.h>

#include <hal/camera.h>

class CameraController : public PeriodicTask
{

  private:

    DataRessourcesRegistry  * _registry;
    Camera                  * _camera;

    void run();

  public:

    CameraController(float period, DataRessourcesRegistry * registry, Camera * camera);
};