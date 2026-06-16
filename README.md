# QCOM - 串口与网络调试助手

基于Qt6开发的跨平台串口与网络通信调试工具。

## 功能特性

- **串口通信**
  - 可配置波特率（1200-921600）
  - 数据位、停止位、校验位设置
  - 流控制选项
  - 自动扫描可用端口

- **网络通信**
  - TCP 客户端模式
  - TCP 服务端模式
  - UDP 通信
  - 可配置本地和远程地址

- **数据显示**
  - 时间戳支持
  - 十六进制显示模式
  - UTF-8 中文字符支持
  - 发送/接收彩色标签（发送：黄色，接收：绿色）
  - 可调节字体设置

- **其他功能**
  - 定时自动发送
  - 发送追加换行符（CR+LF）
  - 保存接收数据到文件
  - 可拖动调整接收/发送区域大小

## 编译需求

- Qt 6.x（推荐 Qt 6.11.1）
- CMake 3.16+
- C++17 编译器

### 各平台编译器要求

| 平台 | 编译器 | Qt版本 |
|------|--------|--------|
| Windows | MinGW 13.1.0 或 MSVC 2022 | Qt 6.11.1 |
| macOS | Clang（Xcode） | Qt 6.x |
| Linux | GCC 9+ 或 Clang | Qt 6.x |

## 编译指南

### Windows（MinGW）

```bash
# 设置Qt路径（根据你的安装路径调整）
set PATH=D:\Qt\Tools\mingw1310_64\bin;D:\Qt\6.11.1\mingw_64\bin;%PATH%

# 配置
cmake -B build -G "MinGW Makefiles" -DCMAKE_PREFIX_PATH=D:/Qt/6.11.1/mingw_64

# 编译
cmake --build build

# 运行
build\QCOM.exe
```

### Windows（MSVC）

```bash
# 打开 "x64 Native Tools Command Prompt for VS 2022"

# 配置
cmake -B build -G "Visual Studio 17 2022" -A x64 -DCMAKE_PREFIX_PATH=C:/Qt/6.11.1/msvc2022_64

# 编译
cmake --build build --config Release

# 运行
build\Release\QCOM.exe
```

### macOS

#### 方式一：使用官方Qt安装包

1. **下载安装Qt**
   - 从 [Qt官网](https://www.qt.io/download) 下载Qt安装包
   - 安装Qt 6.x（推荐6.11.1或更高版本）
   - 安装路径通常为：`/Users/你的用户名/Qt/6.x/macos` 或 `/opt/qt6`

2. **安装Xcode命令行工具**
   ```bash
   xcode-select --install
   ```

3. **编译**
   ```bash
   # 设置Qt路径（根据实际安装路径调整）
   export CMAKE_PREFIX_PATH=/Users/你的用户名/Qt/6.11.1/macos
   # 或者使用brew安装的Qt
   # export CMAKE_PREFIX_PATH=/opt/homebrew/opt/qt@6

   # 配置
   cmake -B build -DCMAKE_PREFIX_PATH=$CMAKE_PREFIX_PATH

   # 编译
   cmake --build build

   # 运行
   ./build/QCOM.app/Contents/MacOS/QCOM
   # 或者直接打开app包
   open build/QCOM.app
   ```

#### 方式二：使用Homebrew

```bash
# 安装Qt6
brew install qt@6

# 设置环境变量
export CMAKE_PREFIX_PATH=/opt/homebrew/opt/qt@6

# 配置和编译
cmake -B build -DCMAKE_PREFIX_PATH=/opt/homebrew/opt/qt@6
cmake --build build

# 运行
open build/QCOM.app
```

#### 生成独立可执行的macOS应用包

```bash
# 编译Release版本
cmake -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=/path/to/qt
cmake --build build --config Release

# 使用macdeployqt打包依赖（Qt自带工具）
/path/to/qt/bin/macdeployqt build/QCOM.app -dmg

# 生成的QCOM.dmg可以直接分发给其他mac用户
```

### Linux（Ubuntu/Debian）

```bash
# 安装依赖
sudo apt update
sudo apt install cmake build-essential qt6-base-dev qt6-serialport-dev qt6-tools-dev

# 配置
cmake -B build

# 编译
cmake --build build

# 运行
./build/QCOM
```

### Linux（Arch Linux）

```bash
# 安装依赖
sudo pacman -S cmake qt6-base qt6-serialport qt6-tools

# 配置和编译
cmake -B build
cmake --build build

# 运行
./build/QCOM
```

### Linux ARM（Raspberry Pi等）

```bash
# 确保Qt6支持ARM架构
# 在Raspberry Pi OS上可能需要从源码编译Qt6

# 配置
cmake -B build -DCMAKE_PREFIX_PATH=/usr/lib/qt6

# 编译
cmake --build build

# 运行
./build/QCOM
```

## 项目结构

```
QCOM/
├── CMakeLists.txt          # 编译配置
├── main.cpp                # 应用程序入口
├── mainwindow.h/cpp        # 主窗口实现
├── mainwindow.ui           # UI布局文件
├── icons/                  # SVG图标
│   ├── app_icon.svg        # 应用图标
│   ├── serial.svg          # 串口图标
│   ├── tcp_client.svg      # TCP客户端图标
│   ├── tcp_server.svg      # TCP服务端图标
│   ├── udp.svg             # UDP图标
│   └── ...                 # 其他图标
├── resources.qrc           # Qt资源文件
└── README.md               # 说明文档
```

## 使用说明

1. 选择连接类型（串口/TCP/UDP）
2. 配置连接参数
3. 点击"打开"/"连接"/"绑定"建立连接
4. 在发送区输入数据，点击"发送"
5. 接收的数据显示在接收区

## 许可证

MIT许可证 - 可自由使用、修改和分发。

## 更新日志

### v1.1
- 添加应用图标
- 完善README文档
- 优化接收/发送区初始比例
- 各平台编译指南

### v1.0
- 初始版本发布
- 串口通信功能
- TCP 客户端/服务端模式
- UDP 支持
- 时间戳显示
- 十六进制模式
- UTF-8 中文字符支持
- 可拖动调整UI面板