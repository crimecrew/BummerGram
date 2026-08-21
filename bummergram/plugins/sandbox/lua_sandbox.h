#pragma once
/**
 * @file lua_sandbox.h
 * @brief Security sandbox for Lua execution
 */

#include <QString>
#include <QSet>

struct lua_State;

namespace BummerGram {

/**
 * @class LuaSandbox
 * @brief Security sandbox for untrusted Lua scripts
 * 
 * Restricts access to:
 * - File system (only app directories)
 * - System commands
 * - Network access
 * - Dangerous Lua functions
 */
class LuaSandbox {
public:
    /**
     * @brief Construct sandbox
     */
    LuaSandbox();
    
    /**
     * @brief Destructor
     */
    ~LuaSandbox();

    // ========================================================================
    // Configuration
    // ========================================================================
    
    /**
     * @brief Set allowed file paths
     */
    void setAllowedPaths(const QStringList& paths);
    
    /**
     * @brief Add allowed path
     */
    void addAllowedPath(const QString& path);
    
    /**
     * @brief Set timeout for script execution
     */
    void setTimeout(int milliseconds);
    
    /**
     * @brief Enable/disable dangerous functions
     */
    void setAllowDangerousFunctions(bool allow);

    // ========================================================================
    // Validation
    // ========================================================================
    
    /**
     * @brief Check if a path is allowed
     */
    bool isPathAllowed(const QString& path) const;
    
    /**
     * @brief Validate a Lua script
     * @param script Script to validate
     * @return true if safe
     */
    bool validateScript(const QString& script) const;
    
    /**
     * @brief Get validation errors
     */
    QStringList getValidationErrors() const;

    // ========================================================================
    // Application
    // ========================================================================
    
    /**
     * @brief Apply sandbox restrictions to a Lua state
     */
    void applyToState(lua_State* L) const;

private:
    // ========================================================================
    // Private Methods
    // ========================================================================
    
    void initializeRestrictedFunctions();
    bool containsDangerousPattern(const QString& script) const;

    // ========================================================================
    // Dangerous Pattern Detection
    // ========================================================================
    
    static bool isDangerousLoadstring(const QString& script);
    static bool isDangerousOsExecute(const QString& script);
    static bool isDangerousIoAccess(const QString& script);
    static bool isDangerousRequireFs(const QString& script);
    static bool isDangerousMetatableTrick(const QString& script);

    // ========================================================================
    // Member Variables
    // ========================================================================
    
    class Impl;
    struct ImplDeleter {
        void operator()(LuaSandbox::Impl* p) const;
    };
    
    std::unique_ptr<Impl, ImplDeleter> d;
};

} // namespace BummerGram
