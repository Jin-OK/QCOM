# QCOM - 串口与网络调试助手

基于Qt6开发的跨平台串口与网络通信调试工具。

## 下载预编译版本

推荐使用预编译版本，无需自行编译：

前往 [Releases](https://github.com/Jin-OK/QCOM/releases) 页面下载最新版本。

| 平台 | 文件 | 说明 |
|------|------|------|
| Windows x64 | QCOM-windows-x64.zip | Windows 64位，解压后运行 QCOM.exe |
| macOS ARM64 | QCOM-macos-arm64.dmg | M1/M2/M3芯片 Mac |
| Linux x64 | QCOM-linux-x64.tar.gz | Linux 64位 |
| Linux ARM64 | QCOM-linux-arm64.tar.gz | 树莓派等ARM设备 |

### macOS 使用说明

双击 dmg 文件安装。首次运行如提示"已损坏"或"无法验证开发者"，请在终端执行：

```bash
sudo xattr -cr /Applications/QCOM.app
```

然后即可正常打开应用。

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
  - 深色主题界面

- **其他功能**
  - 定时自动发送
  - 发送追加换行符（CR+LF）
  - 保存接收数据到文件
  - 可拖动调整接收/发送区域大小

## 编译需求

如需自行编译，请满足以下条件：

- Qt 6.x（推荐 Qt 6.5.3+）
- CMake 3.16+
- C++17 编译器

### 各平台编译器要求

| 平台 | 编译器 | Qt版本 |
|------|--------|--------|
| Windows | MSVC 2022 | Qt 6.5.3+ |
| macOS | Clang（Xcode） | Qt 6.5.3+ |
| Linux | GCC 9+ 或 Clang | Qt 6.4+ |

## 编译指南

### Windows（MSVC）

```bash
# 打开 "x64 Native Tools Command Prompt for VS 2022"

# 配置
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=C:/Qt/6.5.3/msvc2019_64

# 编译
cmake --build build

# 部署依赖
cd build
windeployqt --release QCOM.exe

# 运行
QCOM.exe
```

### macOS

#### 方式一：使用官方Qt安装包

1. **下载安装Qt**
   - 从 [Qt官网](https://www.qt.io/download) 下载Qt安装包
   - 安装Qt 6.x（推荐6.5.3或更高版本）

2. **安装Xcode命令行工具**
   ```bash
   xcode-select --install
   ```

3. **编译**
   ```bash
   # 设置Qt路径（根据实际安装路径调整）
   export CMAKE_PREFIX_PATH=/Users/你的用户名/Qt/6.5.3/macos

   # 配置
   cmake -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=$CMAKE_PREFIX_PATH

   # 编译
   cmake --build build

   # 运行
   open build/QCOM.app
   ```

#### 方式二：使用Homebrew

```bash
# 安装Qt6
brew install qt@6

# 配置和编译
cmake -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=/opt/homebrew/opt/qt@6
cmake --build build

# 运行
open build/QCOM.app
```

### Linux（Ubuntu/Debian）

```bash
# 安装依赖
sudo apt update
sudo apt install cmake build-essential qt6-base-dev qt6-serialport-dev

# 配置
cmake -B build -DCMAKE_BUILD_TYPE=Release

# 编译
cmake --build build

# 运行
./build/QCOM
```

### Linux ARM（Raspberry Pi等）

```bash
# 安装依赖
sudo apt update
sudo apt install cmake build-essential qt6-base-dev qt6-serialport-dev

# 配置
cmake -B build -DCMAKE_BUILD_TYPE=Release

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
│   ├── dropdown.svg        # 下拉箭头图标
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

### v2.0
- 添加深色主题（暗蓝色风格）
- 添加下拉箭头图标
- GitHub Actions 自动编译
- 支持 5 个平台预编译版本
- macOS 最低支持版本 10.15

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