CC = g++
CFLAGS = -g -I include -fPIE -I stack/include -Wcast-qual -Wconversion -Wctor-dtor-privacy    \
		 -Wempty-body -Wformat-security -Wformat=2 -Wignored-qualifiers -Wlogical-op \
         -Wno-missing-field-initializers -Wnon-virtual-dtor -Woverloaded-virtual     \
		 -Wpointer-arith -Wsign-promo -Wstack-usage=8192 -Wstrict-aliasing           \
		 -Wstrict-null-sentinel -Wtype-limits -Wwrite-strings -fexceptions -pipe     \

LDFLAGS = -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio -lstdc++

SOURCES_DIR = src
OBJECTS_DIR = bin
LOGS_DIR    = logs
BUILD_DIR   = build

EXECUTABLE = proc
EXECUTABLE_PATH = $(BUILD_DIR)/$(EXECUTABLE)

SOURCE_FILES = $(wildcard $(SOURCES_DIR)/*.cpp)
OBJECT_FILES = $(subst $(SOURCES_DIR), $(OBJECTS_DIR), $(SOURCE_FILES:.cpp=.o))

all: $(EXECUTABLE_PATH)
	echo "cd asm; build/asm; cd ..; build/proc"

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)
	mkdir -p $(LOGS_DIR)

$(OBJECTS_DIR):
	mkdir -p $(OBJECTS_DIR)

$(EXECUTABLE_PATH): $(OBJECT_FILES) $(BUILD_DIR)
	$(CC) $(OBJECT_FILES) stack/bin/stack.o stack/bin/allocation.o -o $@ $(LDFLAGS)

$(OBJECTS_DIR)/%.o: $(SOURCES_DIR)/%.cpp $(OBJECTS_DIR)
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -fr $(OBJECTS_DIR) $(BUILD_DIR) $(LOGS_DIR)
