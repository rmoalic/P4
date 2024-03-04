LOCAL_PATH := $(call my-dir)
include $(call all-subdir-makefiles)
$(call import-module,SDL2-2.30.0)
$(call import-module,SDL2_ttf-2.22.0)
