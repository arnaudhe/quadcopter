# Software

## Ressources

#### `base_station`

Python-based base station able to run an a laptop or embedded on the radio-command remote

Base Station has several front-ends which can be enabled via command-line:
* Remote (XBox or Quadcopter remote)
* Graphical User Interface for data-model operations
* Logging received from quadcopter
* Scope plotting from real-time telemetry sent by quadcopter
* Video display received by USB video receiver (VRX)

Base Station can communicate to quadcopter via:
* UDP (over wifi)
* RF (custom FSK protocol)

#### `data_model`

* Definition of communication data_model, shared between flight controller and base station
* Specification of the json and binary protocols which supports this data model

#### `flight_controller`

Espressif esp-idf-based flight controller firmware

#### `tests`

Jupyter notebook for tests (filters, algorithms, ...)

#### `tools`

Python utilities (plotting, camera testing, ...)

## Setup the environment

Software environment is based on:
* Espressif ESP-IDF sdk for embedded flight controller
* Python for base_station (remote or laptop), tooling and tests

### Requirements

Make sure to have python3.x and pip3 in your system path when running command
```
# python3 --version
Python 3.9.4
```

Make sure to have cmake installed on your system and in your PATH
```
cmake --version
```

Make sure to have ninja installed on your system and in your PATH
```
ninja --version
```

### Setup the environment

```
./setup.sh
```

This will:
* Fetch espressif SDK as a git submodule
* Create a folder `.espressif` in your home folder, and download the espressif toolchain, bin utils and debug utils
* Setup a python virtual environement in local folder `.venv`, and install all the python dependencies

### Use the environment

```
. ./env.sh
```

You might be able to run python3 with dependencies installed, and access to espressif SDK utils, for instance `idf.py --version`

## Flight controller development

Refer to flight_controller folder README

## Visual Studio Code integration

### Extensions

- ["C/C++"](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools)
- ["Python"](https://marketplace.visualstudio.com/items?itemName=ms-python.python)

### Features

Open the workspace `quadcopter.code-workspace` in Visual Studio Code. You may already have:
* C/C++ indexing and completion for flight controller
* Integrated build and flash for flight controller
* Python indexing and completion for base station and tools
* Python debugging

Don't forget to run Visual Studio in python virtual environment

### Flight Controller : Build, Flash, Monitor

Hit `cmd + shift + B` to build the project. The errors and warnings should be highlighted in the "problems" tab.

