
#
# This is a project Makefile. It is assumed the directory this Makefile resides in is a
# project subdirectory.
#

PROJECT_NAME := quadcopter

COPY_FILES = $(SRC_DIR)../data_model/data_model.json

all: $(COPY_FILES)

$(SRC_DIR)../data_model/data_model.json: storgage_img/data_model.json

$(SRC_DIR)/%:
    cp -f $< $@

include $(IDF_PATH)/make/project.mk


SPIFFS_IMAGE_FLASH_IN_PROJECT := 1
$(eval $(call spiffs_create_partition_image,storage,storgage_img))
