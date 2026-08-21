--[[
    Logging Demo Plugin for BummerGram
    Demonstrates the logging API with different log levels
--]]

bummer.info("Loading logging_demo plugin...")

-- Plugin metadata
local plugin_name = "Logging Demo"
local plugin_version = "1.0.0"

-- Log level colors (simulated in text)
local logLevels = {
    DEBUG = "[DEBUG]",
    INFO = "[INFO] ",
    WARNING = "[WARN] ",
    ERROR = "[ERROR]"
}

-- Called when the plugin is loaded
function onLoad()
    bummer.info(plugin_name .. " v" .. plugin_version .. " loaded")
    runLogDemo()
end

-- Demonstrate all log levels
function runLogDemo()
    bummer.debug("This is a debug message - detailed information for troubleshooting")
    bummer.info("This is an info message - normal operational information")
    bummer.warning("This is a warning message - something might be wrong")
    bummer.error("This is an error message - something failed")
    
    -- String formatting demo
    local formatted = bummer.format("Formatted: %s %d %.2f", "test", 42, 3.14159)
    bummer.info("Formatted string: " .. formatted)
    
    -- String manipulation demo
    local testString = "  Hello World  "
    bummer.debug("Original: '" .. testString .. "'")
    bummer.debug("Upper:   '" .. bummer.upper(testString) .. "'")
    bummer.debug("Lower:   '" .. bummer.lower(testString) .. "'")
    bummer.debug("Trimmed: '" .. bummer.trim(testString) .. "'")
    
    -- Split demo
    local csvData = "apple,banana,cherry,date"
    bummer.info("Splitting CSV: " .. csvData)
    local fruits = bummer.split(csvData, ",")
    bummer.info("Found " .. #fruits .. " items")
    
    -- Time demo
    local currentTime = bummer.getTime()
    bummer.info("Current timestamp: " .. tostring(currentTime))
end

-- Called when the plugin is unloaded
function onUnload()
    bummer.info(plugin_name .. " unloading...")
end

bummer.info("logging_demo plugin registration complete")
