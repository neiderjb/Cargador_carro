CSRCS += demo.c
CSRCS += img_bubble_pattern.c

DEPPATH += --dep-path $(LVGL_DIR)/lv_examples/lv_cargador/cargador
VPATH += :$(LVGL_DIR)/lv_examples/lv_cargador/cargador

CFLAGS += "-I$(LVGL_DIR)/lv_examples/lv_cargador/cargador"
