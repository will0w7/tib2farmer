EXECUTABLE = tib2farmer

BUILD_DIR = build

BUILD_TYPE = Release

CMAKE = cmake

DEL = del /f /q

RMDIR = rmdir /s /q

MKDIR = mkdir

all: clean config build

config:
	@if not exist "$(BUILD_DIR)" ( \
		echo Making directory "$(BUILD_DIR)" && \
		$(MKDIR) "$(BUILD_DIR)" \
	)
	$(CMAKE) -B "$(BUILD_DIR)" -DCMAKE_BUILD_TYPE=$(BUILD_TYPE)

build: config
	$(CMAKE) --build "$(BUILD_DIR)"

clean:
	@if exist "$(BUILD_DIR)" $(RMDIR) "$(BUILD_DIR)"

run: build
	@if exist "$(BUILD_DIR)/$(EXECUTABLE).exe" ( \
		"$(BUILD_DIR)/$(EXECUTABLE).exe" \
	) else ( \
		echo "Error: $(EXECUTABLE).exe not found. Run 'make build' first." \
	)

.PHONY: all clean config build run