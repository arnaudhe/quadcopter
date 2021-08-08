export IDF_TOOLS_PATH=${HOME}/.espressif
export IDF_PATH=$(pwd)/esp-idf

PYTHON_ENV=$(ls ${IDF_TOOLS_PATH}/python_env)

echo "IDF_TOOLS_PATH : ${IDF_TOOLS_PATH}"
echo "    PYTHON_ENV : ${PYTHON_ENV}"
echo "      IDF_PATH : ${IDF_PATH}"

source ${IDF_TOOLS_PATH}/python_env/idf4.2_py3.9_env/bin/activate
source ${IDF_PATH}/export.sh