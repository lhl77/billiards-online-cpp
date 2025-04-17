# 编译器设置
CXX := g++
CXXFLAGS := -g -mwindows -std=c++17 -Wall -Wextra
LDFLAGS := -static-libgcc -static-libstdc++

# 包含路径
INCLUDE_DIRS := -I./thirdparty-lib/easyX/include \
                -I./thirdparty-lib/curl/include \
                -I./thirdparty-lib/openssl/include \
                -I./include

# 库路径
LIB_DIRS := -L./thirdparty-lib/easyX/lib \
            -L./thirdparty-lib/curl/lib \
            -L./thirdparty-lib/openssl/lib \
            -L./lib

# 链接库
LIBS := -leasyx \
        -lcurl \
        -lssl \
        -lcrypto \
        -lgdi32 \
        -lmsimg32 \
        -lwinmm \
        -lws2_32

# 文件路径
SRC := EchoPool.cpp
OBJ := $(SRC:.cpp=.o)
TARGET := EchoPool.exe
BUILD_DIR := build

# 修改DLL路径配置（适配常见安装情况）
CURL_DLL := thirdparty-lib\\curl\\lib\\libcurl*.dll
OPENSSL_DLLS := thirdparty-lib\\openssl\\lib\\libssl-*.dll \
               thirdparty-lib\\openssl\\lib\\libcrypto-*.dll

# 构建规则
all: prepare_build $(BUILD_DIR)/$(TARGET) copy_resources copy_dlls

prepare_build:
	@if not exist "$(BUILD_DIR)" mkdir "$(BUILD_DIR)"

$(BUILD_DIR)/$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $^ -o $@ $(LIB_DIRS) $(LIBS)
	@echo Executable built to $(BUILD_DIR)/

copy_resources:
	@if not exist "$(BUILD_DIR)\images" mkdir "$(BUILD_DIR)\images"
	@xcopy /Y /E /I /Q "images\*" "$(BUILD_DIR)\images\" > nul
	@if not exist "$(BUILD_DIR)\packs" mkdir "$(BUILD_DIR)\packs"
	@xcopy /Y /E /I /Q "packs\*" "$(BUILD_DIR)\packs\" > nul
	@echo Resources copied to $(BUILD_DIR)

copy_dlls: prepare_build
	@echo [INFO] Copying DLL files...
	@if exist "$(CURL_DLL)" ( \
		copy /Y "$(CURL_DLL)" "$(BUILD_DIR)\\" > nul && \
		echo   - Copied $(notdir $(CURL_DLL)) \
	) else ( \
		echo [ERROR] libcurl.dll not found! && exit 1 \
	)
	@for %%f in ($(OPENSSL_DLLS)) do ( \
		if exist "%%f" ( \
			copy /Y "%%f" "$(BUILD_DIR)\\" > nul && \
			echo   - Copied %%~nxf \
		) else ( \
			echo [WARNING] Missing OpenSSL DLL: %%~nxf \
		) \
	)
	@echo [DONE] DLL copy completed

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDE_DIRS) -c $< -o $@

clean:
	@if exist "$(BUILD_DIR)" rmdir /S /Q "$(BUILD_DIR)"
	@del /Q $(OBJ) 2> nul
	@echo Cleaned build files

.PHONY: all prepare_build copy_resources copy_dlls clean
