--[[
    Hello World Plugin for BummerGram
    A simple example demonstrating the plugin API
--]]

bummer.info("Loading hello_world plugin...")

-- Plugin metadata
local plugin_name = "Hello World"
local plugin_version = "1.0.0"
local plugin_author = "BummerGram"
local plugin_description = "A simple hello world plugin demonstrating the plugin API"

-- Configuration
local config = {
    enabled = true,
    greeting = "Hello from BummerGram!"
}

-- Called when the plugin is loaded
function onLoad()
    bummer.info(plugin_name .. " v" .. plugin_version .. " loaded successfully")
    bummer.debug("Plugin description: " .. plugin_description)
end

-- Called when the application starts
function onStartup()
    bummer.info(config.greeting)
end

-- Called when a message is received
-- Parameters: peer_id (string), message (string), timestamp (number)
function onMessage(peer_id, message, timestamp)
    bummer.debug("Message from " .. tostring(peer_id) .. " at " .. tostring(timestamp))
    bummer.debug("Message content: " .. tostring(message))
    
    -- Echo detection
    if message:lower():find("hello") then
        bummer.info("Hello keyword detected from peer " .. tostring(peer_id))
    end
end

-- Called when the plugin is unloaded
function onUnload()
    bummer.info(plugin_name .. " unloading...")
end

-- Utility function example
function formatTimestamp(ts)
    -- Simple timestamp formatter
    return bummer.format("Timestamp: %d", ts)
end

-- Example: String manipulation
function processText(text)
    -- Demonstrate string functions
    local upper = bummer.upper(text)
    local lower = bummer.lower(text)
    local trimmed = bummer.trim("  " .. text .. "  ")
    
    return {
        original = text,
        upper = upper,
        lower = lower,
        trimmed = trimmed,
        formatted = bummer.format("'%s' in upper is '%s'", text, upper)
    }
end

bummer.info("hello_world plugin registration complete")
