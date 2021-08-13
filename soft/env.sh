export IDF_TOOLS_PATH=${HOME}/.espressif
export IDF_PATH=$(pwd)/esp-idf

PYTHON_ENV=.venv

echo "IDF_TOOLS_PATH : ${IDF_TOOLS_PATH}"
echo "    PYTHON_ENV : ${PYTHON_ENV}"
echo "      IDF_PATH : ${IDF_PATH}"

source ${PYTHON_ENV}/bin/activate
source ${IDF_PATH}/export.sh