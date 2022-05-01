
which cmake &>/dev/null
if [ $? -ne 0 ]
then
    echo "cmake: command not found."
    exit 1
fi

which python3 &>/dev/null
if [ $? -ne 0 ]
then
    echo "python3: command not found."
    exit 1
fi

which pip3 &>/dev/null
if [ $? -ne 0 ]
then
    echo "pip3: command not found."
    exit 1
fi

echo "Fetching ESP-IDF"
git submodule update --init --recursive

echo "Adding SDK binaries to PATH"
export IDF_PATH=$(pwd)/flight_controller/esp-idf

echo "Installing ESP-IDF tools"
python3 ${IDF_PATH}/tools/idf_tools.py install

echo "Installing Python environment and packages"
python3 -m venv .venv
. .venv/bin/activate
pip install --upgrade pip
pip install -r flight_controller/esp-idf/requirements.txt
pip install -r base_station/requirements.txt
pip install -r tests/requirements.txt
pip install -r tools/requirements.txt

echo ""
echo "All done! You can now activate the environment:"
echo ""
echo "  . env.sh"
echo ""
