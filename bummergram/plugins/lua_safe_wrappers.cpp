#include "lua_safe_wrappers.h"
#include "../core/localstorage.h"

#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QThread>
#include <QDebug>
#include <lua.hpp>

namespace BummerGram {

// ============================================================================
// Registration
// ============================================================================

void LuaSafeWrappers::registerGlobals(lua_State* L) {
    if (!L) return;
    
    // Create 'bummer' table if it doesn't exist
    lua_getglobal(L, "bummer");
    if (!lua_istable(L, -1)) {
        lua_pop(L, 1);
        lua_newtable(L);
    }
    int bummerIndex = lua_gettop(L);
    
    // Register logging functions
    lua_pushcfunction(L, &LuaSafeWrappers::luaLogDebug);
    lua_setfield(L, bummerIndex, "debug");
    
    lua_pushcfunction(L, &LuaSafeWrappers::luaLogInfo);
    lua_setfield(L, bummerIndex, "info");
    
    lua_pushcfunction(L, &LuaSafeWrappers::luaLogWarning);
    lua_setfield(L, bummerIndex, "warning");
    
    lua_pushcfunction(L, &LuaSafeWrappers::luaLogError);
    lua_setfield(L, bummerIndex, "error");
    
    // Register string functions
    lua_pushcfunction(L, &LuaSafeWrappers::luaStringFormat);
    lua_setfield(L, bummerIndex, "format");
    
    lua_pushcfunction(L, &LuaSafeWrappers::luaStringUpper);
    lua_setfield(L, bummerIndex, "upper");
    
    lua_pushcfunction(L, &LuaSafeWrappers::luaStringLower);
    lua_setfield(L, bummerIndex, "lower");
    
    lua_pushcfunction(L, &LuaSafeWrappers::luaStringTrim);
    lua_setfield(L, bummerIndex, "trim");
    
    lua_pushcfunction(L, &LuaSafeWrappers::luaStringSplit);
    lua_setfield(L, bummerIndex, "split");
    
    // Register system functions
    lua_pushcfunction(L, &LuaSafeWrappers::luaGetTime);
    lua_setfield(L, bummerIndex, "getTime");
    
    lua_pushcfunction(L, &LuaSafeWrappers::luaSleep);
    lua_setfield(L, bummerIndex, "sleep");
    
    lua_pushcfunction(L, &LuaSafeWrappers::luaGetEnv);
    lua_setfield(L, bummerIndex, "getEnv");
    
    // Register file functions (sandboxed)
    lua_pushcfunction(L, &LuaSafeWrappers::luaReadFile);
    lua_setfield(L, bummerIndex, "readFile");
    
    lua_pushcfunction(L, &LuaSafeWrappers::luaWriteFile);
    lua_setfield(L, bummerIndex, "writeFile");
    
    lua_pushcfunction(L, &LuaSafeWrappers::luaFileExists);
    lua_setfield(L, bummerIndex, "fileExists");
    
    // Update global 'bummer' table
    lua_setglobal(L, "bummer");
}

// ============================================================================
// Logging Functions
// ============================================================================

int LuaSafeWrappers::luaLogDebug(lua_State* L) {
    QString msg = getString(L, 1);
    qDebug().noquote() << "[Lua/Debug]" << msg;
    return 0;
}

int LuaSafeWrappers::luaLogInfo(lua_State* L) {
    QString msg = getString(L, 1);
    qDebug().noquote() << "[Lua/Info]" << msg;
    return 0;
}

int LuaSafeWrappers::luaLogWarning(lua_State* L) {
    QString msg = getString(L, 1);
    qWarning().noquote() << "[Lua/Warning]" << msg;
    return 0;
}

int LuaSafeWrappers::luaLogError(lua_State* L) {
    QString msg = getString(L, 1);
    qCritical().noquote() << "[Lua/Error]" << msg;
    return 0;
}

// ============================================================================
// String Functions
// ============================================================================

int LuaSafeWrappers::luaStringFormat(lua_State* L) {
    // Simple format: %s, %d placeholders
    QString format = getString(L, 1);
    int n = lua_gettop(L);
    
    int argIndex = 2;
    int placeholderIndex = 0;
    QString result;
    
    for (int i = 0; i < format.length(); ++i) {
        if (format[i] == '%' && i + 1 < format.length()) {
            QChar spec = format[i + 1];
            
            if (spec == 's') {
                if (argIndex <= n) {
                    result += getString(L, argIndex++);
                    ++i; // Skip spec char
                    continue;
                }
            } else if (spec == 'd' || spec == 'i') {
                if (argIndex <= n && lua_isnumber(L, argIndex)) {
                    result += QString::number(lua_tointeger(L, argIndex++));
                    ++i;
                    continue;
                }
            } else if (spec == 'f') {
                if (argIndex <= n && lua_isnumber(L, argIndex)) {
                    result += QString::number(lua_tonumber(L, argIndex++), 'f', 6);
                    ++i;
                    continue;
                }
            }
        }
        result += format[i];
    }
    
    pushString(L, result);
    return 1;
}

int LuaSafeWrappers::luaStringUpper(lua_State* L) {
    QString str = getString(L, 1);
    pushString(L, str.toUpper());
    return 1;
}

int LuaSafeWrappers::luaStringLower(lua_State* L) {
    QString str = getString(L, 1);
    pushString(L, str.toLower());
    return 1;
}

int LuaSafeWrappers::luaStringTrim(lua_State* L) {
    QString str = getString(L, 1);
    pushString(L, str.trimmed());
    return 1;
}

int LuaSafeWrappers::luaStringSplit(lua_State* L) {
    QString str = getString(L, 1);
    QString sep = getString(L, 2);
    
    // Create result table
    lua_newtable(L);
    int tableIndex = lua_gettop(L);
    
    QStringList parts = str.split(sep);
    for (int i = 0; i < parts.size(); ++i) {
        lua_pushinteger(L, i + 1);
        pushString(L, parts[i]);
        lua_settable(L, tableIndex);
    }
    
    return 1;
}

// ============================================================================
// System Functions
// ============================================================================

int LuaSafeWrappers::luaGetTime(lua_State* L) {
    qint64 timestamp = QDateTime::currentMSecsSinceEpoch();
    lua_pushinteger(L, timestamp);
    return 1;
}

int LuaSafeWrappers::luaSleep(lua_State* L) {
    int ms = lua_tointeger(L, 1);
    QThread::msleep(ms);
    return 0;
}

int LuaSafeWrappers::luaGetEnv(lua_State* L) {
    QString name = getString(L, 1);
    QString value = QString::fromUtf8(qgetenv(name.toUtf8().constData()));
    pushString(L, value);
    return 1;
}

// ============================================================================
// File Functions (Sandboxed)
// ============================================================================

int LuaSafeWrappers::luaReadFile(lua_State* L) {
    QString filePath = getString(L, 1);
    
    // Security: Only allow access to app directories
    QString appData = LocalStorage::getAppDataPath();
    if (!filePath.startsWith(appData)) {
        lua_pushnil(L);
        pushString(L, "Access denied: can only read files in app data directory");
        return 2;
    }
    
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        lua_pushnil(L);
        pushString(L, "Failed to open file");
        return 2;
    }
    
    QTextStream stream(&file);
    QString content = stream.readAll();
    file.close();
    
    pushString(L, content);
    return 1;
}

int LuaSafeWrappers::luaWriteFile(lua_State* L) {
    QString filePath = getString(L, 1);
    QString content = getString(L, 2);
    
    // Security: Only allow write to app directories
    QString appData = LocalStorage::getAppDataPath();
    if (!filePath.startsWith(appData)) {
        lua_pushboolean(L, 0);
        pushString(L, "Access denied: can only write files in app data directory");
        return 2;
    }
    
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        lua_pushboolean(L, 0);
        pushString(L, "Failed to open file for writing");
        return 2;
    }
    
    QTextStream stream(&file);
    stream << content;
    file.close();
    
    lua_pushboolean(L, 1);
    return 1;
}

int LuaSafeWrappers::luaFileExists(lua_State* L) {
    QString filePath = getString(L, 1);
    
    // Security: Only check files in app directories
    QString appData = LocalStorage::getAppDataPath();
    if (!filePath.startsWith(appData)) {
        lua_pushboolean(L, 0);
        return 1;
    }
    
    lua_pushboolean(L, QFile::exists(filePath));
    return 1;
}

// ============================================================================
// Chat Functions (Stubs)
// ============================================================================

int LuaSafeWrappers::luaGetChatList(lua_State* L) {
    // Return empty table as placeholder
    lua_newtable(L);
    return 1;
}

int LuaSafeWrappers::luaGetMessage(lua_State* L) {
    // Return nil as placeholder
    lua_pushnil(L);
    return 1;
}

int LuaSafeWrappers::luaSendMessage(lua_State* L) {
    // Return false as placeholder (not implemented)
    lua_pushboolean(L, 0);
    pushString(L, "Sending messages not implemented in this version");
    return 2;
}

// ============================================================================
// Helper Methods
// ============================================================================

void LuaSafeWrappers::pushString(lua_State* L, const QString& str) {
    lua_pushstring(L, str.toUtf8().constData());
}

QString LuaSafeWrappers::getString(lua_State* L, int index) {
    if (lua_isstring(L, index)) {
        return QString(lua_tostring(L, index));
    }
    return QString();
}

} // namespace BummerGram
