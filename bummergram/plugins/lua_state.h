#pragma once
/**
 * @file lua_state.h
 * @brief Lua VM state wrapper for BummerGram
 */

#include <QObject>
#include <QString>
#include <QStringList>
#include <memory>

struct lua_State;

namespace BummerGram {

/**
 * @class LuaState
 * @brief Wrapper for Lua virtual machine state
 */
class LuaState : public QObject {
    Q_OBJECT

public:
    /**
     * @brief Construct a new Lua state
     * @param pluginName Name of the plugin owning this state
     */
    explicit LuaState(const QString& pluginName, QObject* parent = nullptr);
    
    /**
     * @brief Destructor - closes Lua state
     */
    ~LuaState() override;

    // ========================================================================
    // State Management
    // ========================================================================
    
    /**
     * @brief Load a Lua script from file
     * @param filePath Path to the .lua file
     * @return true if loaded successfully
     */
    bool loadFile(const QString& filePath);
    
    /**
     * @brief Load a Lua script from string
     * @param script Lua script content
     * @return true if loaded successfully
     */
    bool loadString(const QString& script);
    
    /**
     * @brief Execute a Lua script
     * @param script Script to execute
     * @return true if executed successfully
     */
    bool execute(const QString& script);
    
    /**
     * @brief Call a Lua function
     * @param functionName Name of function to call
     * @param args Arguments to pass
     * @return true if called successfully
     */
    bool callFunction(const QString& functionName, const QStringList& args = {});
    
    /**
     * @brief Check if a function exists
     */
    bool functionExists(const QString& functionName);
    
    /**
     * @brief Get plugin metadata from script
     */
    bool getMetadata(QString& version, QString& author, QString& description);

    // ========================================================================
    // Variable Access
    // ========================================================================
    
    /**
     * @brief Set a global variable
     */
    void setGlobal(const QString& name, const QString& value);
    
    /**
     * @brief Get a global variable
     */
    QString getGlobal(const QString& name);
    
    /**
     * @brief Clear all globals
     */
    void clearGlobals();

signals:
    /**
     * @brief Emitted when an error occurs
     */
    void error(const QString& message);
    
    /**
     * @brief Emitted when output is generated
     */
    void output(const QString& message);

private:
    // ========================================================================
    // Private Methods
    // ========================================================================
    
    void initialize();
    void registerWrappers();
    void pushString(const QString& str);
    QString toString(int index);
    bool executeChunk(const QString& chunk, const QString& name = "=?");
    
    // Lua C function bindings
    static int luaPrint(lua_State* L);
    static int luaLog(lua_State* L);
    static int luaGetVersion(lua_State* L);
    static int luaGetPlatform(lua_State* L);
    
    // ========================================================================
    // Member Variables
    // ========================================================================
    
    class Impl;
    std::unique_ptr<Impl> d;
};

} // namespace BummerGram
