# BummerGram

BummerGram is a Qt-based desktop application featuring Lua plugin support, customizable settings, and a modern dark-themed UI.

## Features

- **Lua Plugin System**: Extend functionality with Lua scripts
- **Settings Panel**: Configurable appearance and behavior
- **Dark/Light Themes**: Switch between light and dark modes
- **Audio Support**: Startup sound notifications
- **Context Menus**: Copy media links, save avatars
- **Plugin Sandbox**: Security restrictions for untrusted plugins

## Requirements

- **Qt 6.5+** (LTS recommended)
- **CMake 3.20+**
- **Visual Studio 2019+** or MinGW with C++17
- **Lua 5.4** (included via vcpkg)
- **Windows 10/11** (64-bit)

## Quick Start

### Build

```powershell
git clone https://github.com/your-repo/BummerGram.git
cd BummerGram
mkdir build && cd build
cmake .. -G "Visual Studio 16 2019" -A x64
cmake --build . --config Release
```

### Run

```powershell
cd build\Release
.\BummerGram.exe
```

## Project Structure

```
BummerGram/
├── bummergram/              # Main application
│   ├── core/               # Core application classes
│   ├── ui/                 # User interface
│   │   ├── splash/         # Splash screen
│   │   ├── settings/       # Settings panel
│   │   └── widgets/        # Custom widgets
│   ├── audio/             # Audio playback
│   ├── plugins/           # Plugin system
│   │   └── sandbox/       # Lua sandbox
│   └── resources/         # Resources (QSS, icons)
├── cmake/                  # CMake modules
├── scripts/               # Build scripts
├── Doxyfile              # Documentation
└── CMakeLists.txt        # Root CMake file
```

## Lua Plugin API

Plugins are stored in `%AppData%/BummerGram/plugins/`.

Example plugin:

```lua
-- hello_world.lua
bummer.info("Loading hello_world plugin...")

function onStartup()
    bummer.info("Hello from BummerGram!")
end

function onMessage(peer_id, message)
    bummer.debug("Message from " .. peer_id .. ": " .. message)
end
```

### Available Functions

| Function | Description |
|----------|-------------|
| `bummer.debug(msg)` | Log debug message |
| `bummer.info(msg)` | Log info message |
| `bummer.warning(msg)` | Log warning |
| `bummer.error(msg)` | Log error |
| `bummer.format(fmt, ...)` | Format string |
| `bummer.upper(str)` | Convert to uppercase |
| `bummer.lower(str)` | Convert to lowercase |
| `bummer.trim(str)` | Trim whitespace |
| `bummer.split(str, sep)` | Split string |
| `bummer.getTime()` | Get current timestamp |
| `bummer.sleep(ms)` | Sleep for milliseconds |

### Security

The Lua sandbox restricts:
- File access to app data directory only
- OS execute/remove/rename functions blocked
- io.popen and debug functions removed
- Dynamic code loading (loadstring) detected

## Settings

Settings are stored in `%AppData%/BummerGram/settings.ini`.

### Categories

- **General**: Auto-start, minimize to tray, startup sound
- **Layout**: Sidebar width, compact mode, message spacing
- **Lua Plugins**: Manage installed plugins

## Building

See [BUILD_GUIDE.md](BUILD_GUIDE.md) for detailed build instructions.

### Prerequisites

1. Visual Studio 2019+ with C++ workload
2. Qt 6.5+ installed
3. CMake 3.20+
4. vcpkg for dependencies

## License

**GPLv3** - See [LICENSE](LICENSE) file for details.

## Contributing

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Submit a pull request

## Acknowledgments

Built with:
- [Qt Framework](https://www.qt.io/)
- [Lua](https://www.lua.org/)
- [CMake](https://cmake.org/)
