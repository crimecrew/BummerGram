# BummerGram Installation Guide

## System Requirements

| Component | Minimum | Recommended |
|-----------|---------|-------------|
| OS | Windows 10 (64-bit) | Windows 11 |
| RAM | 4 GB | 8 GB |
| Disk | 500 MB | 1 GB |
| Display | 1280x720 | 1920x1080 |

## Installation Methods

### Method 1: Pre-built Binary

1. Download the latest release from the releases page
2. Extract the ZIP file to a folder of your choice
3. Run `BummerGram.exe`

### Method 2: Build from Source

See [BUILD_GUIDE.md](BUILD_GUIDE.md) for detailed instructions.

## Directory Structure

After installation, BummerGram creates the following directories:

```
%AppData%/BummerGram/
├── cache/              # Cache files
├── config/             # Configuration files
│   └── settings.ini    # User settings
├── plugins/            # Lua plugins
│   └── (plugin files)
├── logs/               # Application logs
└── avatar_cache/       # Downloaded avatars
```

## First Run

1. Launch BummerGram
2. The splash screen will appear briefly
3. The main window will open
4. Plugins in the plugins directory will be loaded automatically

## Configuration

Settings are stored in `%AppData%/BummerGram\config\settings.ini`.

### General Settings

| Setting | Default | Description |
|---------|---------|-------------|
| AutoStart | false | Start with Windows |
| StartMinimized | false | Start minimized to tray |
| MinimizeToTray | true | Minimize to system tray |
| PlayStartupSound | true | Play sound on startup |
| Theme | dark | UI theme (dark/light/system) |
| CacheSizeMB | 512 | Cache size in megabytes |

### Layout Settings

| Setting | Default | Description |
|---------|---------|-------------|
| SidebarWidth | 280 | Sidebar width in pixels |
| CompactMode | false | Use compact layout |
| ShowStatusIcons | true | Show online/offline icons |
| ShowTimestamps | true | Show message timestamps |

## Lua Plugins

Place `.lua` files in `%AppData%/BummerGram/plugins/`.

### Plugin API

See [README.md](README.md#Lua-Plugin-API) for the full plugin API documentation.

## Uninstallation

1. Close BummerGram
2. Delete the installation folder
3. Delete `%AppData%/BummerGram` (optional, removes all settings)

## Troubleshooting

### Application Won't Start

1. Check if all required DLLs are present
2. Ensure Visual C++ Redistributable is installed
3. Check logs in `%AppData%/BummerGram/logs/`

### Plugins Not Loading

1. Verify plugins are in the correct directory
2. Check plugin syntax (Lua errors)
3. Enable debug logging to see errors

### Audio Not Playing

1. Ensure sound device is working
2. Check Windows volume settings
3. Verify startup.wav exists in resources

## Support

For issues and feature requests, please open an issue on the GitHub repository.
