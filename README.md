# LenovoOnlineClassTrainer

联想教学系统（Lenovo Teaching System）模块开关工具，通过重命名 `.dll` / `.exe` / `.sys` 模块文件实现对应功能的屏蔽与恢复。

> 仅供教学环境与研究使用，请遵守学校相关规定。

## 项目特性

- 屏蔽/恢复 18 个核心功能模块
- 支持驱动 `inspect.sys` 文件重命名 + Windows 服务控制
- 支持 NTSD 调试器冻结/恢复运行中的学生端进程
- 同时提供 **CLI** 和 **GUI** 两种使用方式
- 实时状态查询、颜色指示、操作日志
- 管理员权限自检
- 配置导入/导出/保存/加载（`trainer_config.txt`）

## 文件结构

```
LenovoOnlineClassTrainer/
├── core.hpp          # 核心功能模块（所有 trainer::* 函数）
├── main.cpp          # CLI 入口
├── main_gui.cpp      # Qt GUI 入口
└── LICENSE           # MIT 协议
```

## 功能模块

| 分类 | 模块 | 文件类型 |
|------|------|----------|
| 屏幕相关 | 屏幕监控、屏幕录制、屏幕广播发送、屏幕广播 | `.dll` |
| 音视频 | 音频渲染、音频发送、音视频广播 | `.dll` / `.exe` |
| 文件传输 | 文件传输（服务端+客户端+核心库） | `.dll` |
| 系统控制 | 系统钩子、键盘锁定、黑屏静音、驱动控制 | `.dll` / `.exe` / `.sys` |
| 进程与考试 | 在线讨论、批量登录、在线考试、进程保护、NTSD | `.dll` / `.exe` |

## 使用方式

### 1. CLI 命令行（main.cpp）

```bash
# 显示帮助
trainer.exe --help

# 查看所有模块状态
trainer.exe --list

# 设置安装路径
trainer.exe -p "D:\Program Files (x86)\Lenovo teaching system" --list

# 屏蔽/恢复单个模块
trainer.exe screen-monitor on
trainer.exe screen-monitor off

# 一键全屏蔽/恢复
trainer.exe all on
trainer.exe all off

# 冻结/恢复学生端进程
trainer.exe ntsd on
trainer.exe ntsd off

# 检查是否以管理员权限运行
trainer.exe --admin

# 保存当前配置到文件（默认 trainer_config.txt）
trainer.exe --save
trainer.exe --save my_config.txt

# 从文件加载配置并应用
trainer.exe --load
trainer.exe --load my_config.txt

# 导出配置（只保存，不修改系统）
trainer.exe --export backup.txt

# 导入配置并应用
trainer.exe --import backup.txt
```

#### 编译（Windows + MinGW）

```bash
g++ -std=c++11 -o trainer.exe main.cpp
```

#### 编译（Windows + MSVC）

```bat
cl /EHsc /std:c++11 main.cpp /Fe:trainer.exe
```

### 2. GUI 图形界面（main_gui.cpp）

基于 Qt5 / Qt6，提供分类选项卡、颜色状态指示、路径设置、实时刷新、操作日志，以及配置保存/加载/导出/导入按钮。

> 安装 Qt 时若使用开源版本，需同意 GNU (L)GPL 开源义务。商用请购买商业许可。

#### 编译（Windows + Qt MinGW）

```cmd
C:\Qt\5.15.2\mingw1310_64\bin\qmake.exe -project "QT+=widgets" "CONFIG+=c++11"
C:\Qt\5.15.2\mingw1310_64\bin\qmake.exe
mingw32-make release
windeployqt trainer_gui.exe
```

#### 编译（Windows + MSVC + Qt）

```bat
qmake -project "QT+=widgets" "CONFIG+=c++11"
qmake
nmake release
windeployqt trainer_gui.exe
```

> 编译时 main.cpp 和 main_gui.cpp 二选一，不要同时链接。

#### 常见编译问题

**`fatal error C1083: Cannot open include file: 'main_gui.moc'`**

`main_gui.cpp` 末尾包含 `#include "main_gui.moc"`，需要先让 Qt 的 **moc（Meta-Object Compiler）** 处理该文件。使用 `qmake` / CMake 的 automoc 会自动生成；若用裸 `cl.exe` 直接编译则找不到该文件。

```cmd
# 推荐：让 qmake 生成 Makefile，自动处理 moc
qmake -project "QT+=widgets" "CONFIG+=c++11"
qmake
nmake release   # 或 mingw32-make
```

## 工作原理

教学软件主程序按文件名加载独立模块。把模块文件重命名为 `.bak` 后缀，主程序找不到对应模块，该功能即被"屏蔽"；改回原文件名即可恢复。

| 状态 | 文件名 |
|------|--------|
| 启用 | `X.dll` / `X.exe` / `X.sys` |
| 屏蔽 | `X.bak.dll` / `X.bak.exe` / `X.bak.sys` |

> ⚠️ 驱动仅重命名文件只对"下次开机"生效，对已加载到内存的驱动无效。要禁用正在运行的驱动需配合 `sc stop` / `sc config` 命令。

## 前置要求

- Windows 7 / 10 / 11
- 联想教学系统已安装（默认 `C:\Program Files (x86)\Lenovo teaching system`）
- 管理员权限（部分操作如驱动控制、进程冻结必需）
- NTSD 调试暂停功能需要 `ntsd.exe`（Win10/11 不再内置，需自行放置到工作目录）

## 安全与免责

- 本工具仅控制本地文件和服务，不会联网
- 重要操作（一键全屏蔽/恢复）已加入确认弹窗
- 使用前请确保已阅读学校相关规定，作者不承担任何违规使用带来的后果

## 协议

MIT License - 详见 [LICENSE](file:///workspace/LenovoOnlineClassTrainer/LICENSE)
