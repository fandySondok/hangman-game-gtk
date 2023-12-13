
PROJECT_DIRECTORY = $(shell pwd)

UNSUSED_IFLAGS = -I/usr/include/at-spi2-atk/2.0 \
        -I/usr/include/at-spi-2.0 \
        -I/usr/include/atk-1.0 \
        -I/usr/include/cairo \
        -I/usr/include/dbus-1.0 \
        -I/usr/include/gtk-3.0 \
        -I/usr/include/gio-unix-2.0/ \
        -I/usr/include/gdk-pixbuf-2.0 \
        -I/usr/include/glib-2.0 \
        -I/usr/include/harfbuzz \
        -I/usr/include/libpng16 \
        -I/usr/include/pango-1.0 \
        -I/usr/include/pixman-1

IFLAGS += -I$(PROJECT_DIRECTORY)/include/ \
          -I$(PROJECT_DIRECTORY)/lib/ \
          -I$(PROJECT_DIRECTORY)/src/

LIB_PKG_CONFIG = `pkg-config --cflags --libs gtk+-3.0 glib-2.0`

LIB_DBEPAYMENT = -lbcadllARM -ldbepayment

LIB_GTK = -latk-1.0 \
          -lcairo \
          -lcairo-gobject \
          -lcrypt \
          -lgtk-3 \
          -lgdk-3 \
          -lgdk_pixbuf-2.0 \
          -lgio-2.0 \
          -lgobject-2.0 \
          -lglib-2.0 \
          -lpango-1.0 \
          -lpangocairo-1.0 \
          -rdynamic \
          -lrt

LIB_SSL = -lcrypto -lssl

# Install dependency libpq5
# sudo apt install libpq5=12.8-0ubuntu0.20.04.1
# lalu install libpq-dev

INCLUDE = $(LIB_PKG_CONFIG) \
          -lm \
          -lpq \
          -lpthread \
          -lsqlite3 \
          -lX11 $(LIB_SSL)

ENCO_GUI = src/hm-gui/hm-gui.c \
           src/hm-gui/hm-gui-main.c \
					 src/hm-gui/hm-gui-menu.c \
					 src/hm-gui/hm-gui-popup.c

SHIKI_TOOLS = lib/shiki-config-tools/shiki-config-tools.c

SOURCE   = src/main.c \
           src/hm-debug/hm-debug.c \
					 src/hm-tools/hm-tools.c \
					 src/hm-tools/hm-linked-list.c \
					 src/hm-conf/hm-conf.c \
           $(ENCO_GUI)


TARGET             = hangman
CC                 = gcc
ACU_CROSS_PLATFORM =
WARN_OPTION        = -Wall
BUILD_DIRECTORY    = build
OBJ_DIRECTORY      = obj
OBJECTS            = $(patsubst %.c,$(OBJ_DIRECTORY)/%.o,$(SOURCE))

TIME_CREATED   = `date +%y.%m.%d_%H.%M.%S`
GIT_IGNORE_CMD = `cat .gitignore | grep -v $(OBJ_DIRECTORY) | grep -v $(BUILD_DIRECTORY)`

vpath $(TARGET) $(BUILD_DIRECTORY)
vpath %.o $(OBJ_DIRECTORY)

$(TARGET):$(OBJECTS)
	@echo
	@echo "  \033[1;33mCreating executable file : $@\033[0m"
	$(CC) $(WARN_OPTION) $(OBJECTS) -o $(BUILD_DIRECTORY)/$@ $(INCLUDE) $(IFLAGS)
	@cp $(BUILD_DIRECTORY)/$@ .
	@echo "\033[1;33m-------------------------FINISH-------------------------- \033[0m"

$(OBJ_DIRECTORY)/%.o:%.c
	@echo
	@echo "  \033[1;32mCompiling: $<\033[0m"
	$(call init_proc);
	$(CC) $(WARN_OPTION) -c $< -o $@ $(INCLUDE) $(IFLAGS)

debug:
	$(CC) $(WARN_OPTION) $(INCLUDE) $(IFLAGS) $(SOURCE) -g

init:
	$(call init_proc);
	@echo "$(GIT_IGNORE_CMD)" > .gitignore
	@echo "$(OBJ_DIRECTORY)/" >> .gitignore
	@echo "$(BUILD_DIRECTORY)/" >> .gitignore

glade_copy:
	@sudo cp ui_parkir.glade /var/encoreader/ui_parkir.glade
	@echo "   \033[1;33mcp\033[0m ui_parkir.glade /var/encoreader/ui_parkir.glade"
	@md5sum ui_parkir.glade /var/encoreader/ui_parkir.glade

check:
	@echo "      \033[1;33mMD5SUM Compiled file: $<\033[0m"
	md5sum build/$(TARGET)
	@echo "      \033[1;33mMD5SUM Running file: $<\033[0m"
	md5sum /var/pkr/s_pkr

clean:
	@rm -fv `find . -type f -name '*.o'`
	@rm -fv ./$(BUILD_DIRECTORY)/$(TARGET)

define init_proc
	@mkdir -p $(OBJ_DIRECTORY)
	@mkdir -p $(BUILD_DIRECTORY)
	@find . -type f -name '*.c' -printf '%h\n' |sort -u | grep -v '$(BUILD_DIRECTORY)' | grep -v '$(OBJ_DIRECTORY)' > dir.struct
	@cd $(OBJ_DIRECTORY) && xargs mkdir -p < ../dir.struct
endef
