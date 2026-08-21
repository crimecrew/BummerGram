#pragma once
/**
 * @file lua_safe_wrappers.h
 * @brief Safe Qt wrappers exposed to Lua scripts
 */

#include <QString>
#include <QStringList>

struct lua_State;

namespace BummerGram {

/**
 * @class LuaSafeWrappers
 * @brief Registers safe C++ functions for Lua scripts
 * 
 * These wrappers provide controlled access to Qt functionality
 * without allowing potentially harmful operations.
 */
class LuaSafeWrappers {
public:
    // ========================================================================
    // Registration
    // ========================================================================
    
    /**
     * @brief Register all safe wrappers in a Lua state
     */
    static void registerGlobals(lua_State* L);
    
    // ========================================================================
    // Logging Functions
    // ========================================================================
    
    /**
     * @brief Log a debug message
     */
    static int luaLogDebug(lua_State* L);
    
    /**
     * @brief Log an info message
     */
    static int luaLogInfo(lua_State* L);
    
    /**
     * @brief Log a warning message
     */
    static int luaLogWarning(lua_State* L);
    
    /**
     * @brief Log an error message
     */
    static int luaLogError(lua_State* L);
    
    // ========================================================================
    // String Functions
    // ========================================================================
    
    /**
     * @brief String formatting
     */
    static int luaStringFormat(lua_State* L);
    
    /**
     * @brief String upper case
     */
    static int luaStringUpper(lua_State* L);
    
    /**
     * @brief String lower case
     */
    static int luaStringLower(lua_State* L);
    
    /**
     * @brief String trim
     */
    static int luaStringTrim(lua_State* L);
    
    /**
     * @brief String split
     */
    static int luaStringSplit(lua_State* L);
    
    // ========================================================================
    // System Functions
    // ========================================================================
    
    /**
     * @brief Get current timestamp
     */
    static int luaGetTime(lua_State* L);
    
    /**
     * @brief Sleep for milliseconds
     */
    static int luaSleep(lua_State* L);
    
    /**
     * @brief Get environment variable
     */
    static int luaGetEnv(lua_State* L);
    
    // ========================================================================
    // File Functions (Limited)
    // ========================================================================
    
    /**
     * @brief Read text file (sandboxed)
     */
    static int luaReadFile(lua_State* L);
    
    /**
     * @brief Write text file (sandboxed)
     */
    static int luaWriteFile(lua_State* L);
    
    /**
     * @brief Check if file exists
     */
    static int luaFileExists(lua_State* L);
    
    // ========================================================================
    // Chat/Message Functions
    // ========================================================================
    
    /**
     * @brief Get chat list
     */
    static int luaGetChatList(lua_State* L);
    
    /**
     * @brief Get message by ID
     */
    static int luaGetMessage(lua_State* L);
    
    /**
     * @brief Send message
     */
    static int luaSendMessage(lua_State* L);
    
private:
    // Helper to push string onto stack
    static void pushString(lua_State* L, const QString& str);
    
    // Helper to get string from stack
    static QString getString(lua_State* L, int index);
};

} // namespace BummerGram
