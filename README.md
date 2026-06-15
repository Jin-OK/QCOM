# QCOM - Serial & Network Assistant

A cross-platform serial port and network communication assistant built with Qt6.

## Features

- **Serial Port Communication**
  - Configurable baud rate (1200-921600)
  - Data bits, stop bits, parity settings
  - Flow control options
  - Auto-scan available ports

- **Network Communication**
  - TCP Client mode
  - TCP Server mode
  - UDP communication
  - Configurable local and remote addresses

- **Data Display**
  - Timestamp support
  - Hexadecimal display mode
  - UTF-8 Chinese character support
  - Color-coded send/receive labels (Send: yellow, Recv: green)
  - Adjustable font settings

- **Additional Features**
  - Auto-send with configurable interval
  - Append newline option (CR+LF)
  - Save received data to file
  - Resizable receive/send areas

## Screenshots

![QCOM Screenshot](docs/screenshot.png)

## Build Requirements

- Qt 6.x (Qt6.11.1 recommended)
- CMake 3.16+
- C++17 compiler

### Platform-specific Requirements

| Platform | Compiler | Qt Version |
|----------|----------|------------|
| Windows  | MinGW 13.1.0 or MSVC 2022 | Qt 6.11.1 (MinGW/MSVC) |
| macOS    | Clang (Xcode) | Qt 6.x |
| Linux    | GCC 9+ or Clang | Qt 6.x |

## Build Instructions

### Windows (MinGW)

```bash
# Set Qt path (adjust to your installation)
set PATH=D:\Qt\Tools\mingw1310_64\bin;D:\Qt\6.11.1\mingw_64\bin;%PATH%

# Configure
cmake -B build -G "MinGW Makefiles" -DCMAKE_PREFIX_PATH=D:/Qt/6.11.1/mingw_64

# Build
cmake --build build

# Run
build\QCOM.exe
```

### Windows (MSVC)

```bash
# Open "x64 Native Tools Command Prompt for VS 2022"

# Configure
cmake -B build -G "Visual Studio 17 2022" -A x64 -DCMAKE_PREFIX_PATH=C:/Qt/6.11.1/msvc2022_64

# Build
cmake --build build --config Release

# Run
build\Release\QCOM.exe
```

### macOS

```bash
# Install Qt via Homebrew or official installer
# Assuming Qt is installed at /opt/qt6 or ~/Qt/6.x/macos

# Configure
cmake -B build -DCMAKE_PREFIX_PATH=/path/to/qt6

# Build
cmake --build build

# Run
./build/QCOM.app/Contents/MacOS/QCOM
# Or simply open the app bundle
open build/QCOM.app
```

### Linux (Ubuntu/Debian)

```bash
# Install dependencies
sudo apt update
sudo apt install cmake build-essential qt6-base-dev qt6-serialport-dev qt6-tools-dev

# Configure
cmake -B build

# Build
cmake --build build

# Run
./build/QCOM
```

### Linux (Arch Linux)

```bash
# Install dependencies
sudo pacman -S cmake qt6-base qt6-serialport qt6-tools

# Configure and build
cmake -B build
cmake --build build

# Run
./build/QCOM
```

### Linux ARM (Raspberry Pi etc.)

```bash
# Ensure Qt6 is available for ARM architecture
# On Raspberry Pi OS, you may need to build Qt6 from source

# Configure with ARM-specific settings
cmake -B build -DCMAKE_PREFIX_PATH=/usr/lib/qt6

# Build
cmake --build build

# Run
./build/QCOM
```

## Application Icon

For Windows executable icon, you need to create an `.ico` file:

1. Convert `icons/app_icon.svg` to `.ico` format using tools like:
   - ImageMagick: `convert app_icon.svg app_icon.ico`
   - Online converters
   - GIMP

2. Add to CMakeLists.txt:
```cmake
if(WIN32)
    set_target_properties(QCOM PROPERTIES WIN32_EXECUTABLE TRUE)
    # Add Windows resource file for icon
endif()
```

## Project Structure

```
QCOM/
├── CMakeLists.txt          # Build configuration
├── main.cpp                # Application entry point
├── mainwindow.h/cpp        # Main window implementation
├── mainwindow.ui           # UI layout (Qt Designer)
├── icons/                  # SVG icons
│   ├── app_icon.svg        # Application icon
│   ├── serial.svg          # Serial port icon
│   ├── tcp_client.svg      # TCP client icon
│   ├── tcp_server.svg      # TCP server icon
│   ├── udp.svg             # UDP icon
│   └── ...                 # Other icons
├── resources.qrc           # Qt resource file
└── README.md               # Documentation
```

## Usage

1. Select connection type (Serial/TCP/UDP)
2. Configure connection parameters
3. Click "Open"/"Connect"/"Bind" to establish connection
4. Type data in Send area and click "Send"
5. Received data appears in Receive area

## License

MIT License - Feel free to use, modify, and distribute.

## Contributing

Contributions welcome! Please submit pull requests or open issues for:
- Bug fixes
- Feature enhancements
- Platform-specific improvements
- Documentation improvements

## Author

QCOM Project

## Changelog

### v1.0.0
- Initial release
- Serial port communication
- TCP Client/Server modes
- UDP support
- Timestamp display
- Hex mode
- UTF-8 Chinese character support
- Resizable UI panels