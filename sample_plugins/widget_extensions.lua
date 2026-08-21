--[[
    Widget Extensions Plugin for BummerGram
    Demonstrates integration with UI elements
--]]

bummer.info("Loading widget_extensions plugin...")

local plugin_name = "Widget Extensions"
local plugin_version = "1.0.0"

-- Track plugin state
local state = {
    enabled = true,
    showNotifications = true
}

-- Called when the plugin is loaded
function onLoad()
    bummer.info(plugin_name .. " v" .. plugin_version .. " loaded")
    bummer.info("Widget extensions available:")
    bummer.info("  - Avatar context menu integration")
    bummer.info("  - Media preview context menu integration")
end

-- Example: Format peer information
function formatPeerInfo(peer_id, name, avatar_path)
    local info = {}
    
    table.insert(info, "=== Peer Information ===")
    table.insert(info, "ID: " .. tostring(peer_id))
    table.insert(info, "Name: " .. (name or "Unknown"))
    
    if avatar_path then
        local exists = bummer.fileExists(avatar_path)
        table.insert(info, "Avatar: " .. (exists and "Available" or "Not found"))
    else
        table.insert(info, "Avatar: Not set")
    end
    
    return table.concat(info, "\n")
end

-- Example: Validate file path
function isValidFilePath(path)
    if not path then
        return false, "Path is nil"
    end
    
    if path:find("..") then
        return false, "Path contains parent directory reference"
    end
    
    if not bummer.fileExists(path) then
        return false, "File does not exist"
    end
    
    return true, "Valid"
end

-- Example: Read configuration file
function loadPluginConfig()
    local configPath = "config/plugin_settings.txt"
    
    if not bummer.fileExists(configPath) then
        bummer.warning("Config file not found: " .. configPath)
        return nil
    end
    
    local content = bummer.readFile(configPath)
    if content then
        bummer.info("Loaded config: " .. content)
        return content
    end
    
    return nil
end

-- Example: Save plugin state
function savePluginState()
    local statePath = "config/plugin_state.txt"
    local content = "enabled=" .. tostring(state.enabled) .. "\n"
    content = content .. "showNotifications=" .. tostring(state.showNotifications)
    
    local success = bummer.writeFile(statePath, content)
    if success then
        bummer.info("Plugin state saved")
    else
        bummer.error("Failed to save plugin state")
    end
end

-- Called when the plugin is unloaded
function onUnload()
    savePluginState()
    bummer.info(plugin_name .. " unloading...")
end

bummer.info("widget_extensions plugin registration complete")
