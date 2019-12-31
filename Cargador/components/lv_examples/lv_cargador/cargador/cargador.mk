CSRCS += cargador.c
CSRCS += img_bubble_pattern.c

DEPPATH += --dep-path $(LVGL_DIR)/lv_examples/lv_apps/cargador
VPATH += :$(LVGL_DIR)/lv_examples/lv_apps/cargador

CFLAGS += "-I$(LVGL_DIR)/lv_examples/lv_apps/cargador"
