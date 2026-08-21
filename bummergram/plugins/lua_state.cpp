#include "lua_state.h"
#include "lua_safe_wrappers.h"
#include "../core/localstorage.h"

#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QDateTime>
#include <lua.hpp>

namespace BummerGram {

// ============================================================================
// Private Implementation
// ============================================================================

class LuaState::Impl {
public:
    QString pluginName;
    lua_State* L = nullptr;
    bool initialized = false;
    QString lastError;
};

} // namespace

// ============================================================================
// Construction
// ============================================================================

LuaState::LuaState(const QString& pluginName, QObject* parent)
    : QObject(parent)
    , d(std::make_unique<Impl>())
{
    d->pluginName = pluginName;
    initialize();
}

LuaState::~LuaState() {
    if (d->L) {
        lua_close(d->L);
        d->L = nullptr;
    }
}

// ============================================================================
// State Management
// ============================================================================

void LuaState::initialize() {
    if (d->initialized) {
        return;
    }
    
    // Create new Lua state
    d->L = luaL_newstate();
    
    if (!d->L) {
        d->lastError = "Failed to create Lua state";
        emit error(d->lastError);
        return;
    }
    
    // Open standard libraries
    luaL_openlibs(d->L);
    
    // Register C functions
    registerWrappers();
    
    d->initialized = true;
    
    qDebug() << "[LuaState] Initialized for plugin:" << d->pluginName;
}

void LuaState::registerWrappers() {
    if (!d->L) return;
    
    // Create 'bummer' table
    lua_newtable(d->L);
    int tableIndex = lua_gettop(d->L);
    
    // Register print function
    lua_pushcfunction(d->L, &LuaState::luaPrint);
    lua_setfield(d->L, tableIndex, "print");
    
    // Register log function
    lua_pushcfunction(d->L, &LuaState::luaLog);
    lua_setfield(d->L, tableIndex, "log");
    
    // Register getVersion
    lua_pushcfunction(d->L, &LuaState::luaGetVersion);
    lua_setfield(d->L, tableIndex, "getVersion");
    
    // Register getPlatform
    lua_pushcfunction(d->L, &LuaState::luaGetPlatform);
    lua_setfield(d->L, tableIndex, "getPlatform");
    
    // Register additional safe wrappers
    LuaSafeWrappers::registerGlobals(d->L);
    
    // Set 'bummer' as global
    lua_setglobal(d->L, "bummer");
}

bool LuaState::loadFile(const QString& filePath) {
    if (!d->L) {
        d->lastError = "Lua state not initialized";
        return false;
    }
    
    // Load file content
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        d->lastError = QString("Failed to open file: %1").arg(filePath);
        emit error(d->lastError);
        return false;
    }
    
    QTextStream stream(&file);
    QString content = stream.readAll();
    file.close();
    
    return loadString(content);
}

bool LuaState::loadString(const QString& script) {
    if (!d->L) {
        d->lastError = "Lua state not initialized";
        return false;
    }
    
    // Compile the script
    int status = luaL_loadstring(d->L, script.toUtf8().constData());
    
    if (status != LUA_OK) {
        d->lastError = QString(lua_tostring(d->L, -1));
        lua_pop(d->L, 1);
        emit error(d->lastError);
        return false;
    }
    
    return true;
}

bool LuaState::execute(const QString& script) {
    if (!d->L) {
        d->lastError = "Lua state not initialized";
        return false;
    }
    
    return executeChunk(script);
}

bool LuaState::executeChunk(const QString& chunk, const QString& name) {
    if (!d->L) {
        return false;
    }
    
    // Compile chunk
    int status = luaL_loadstring(d->L, chunk.toUtf8().constData());
    
    if (status != LUA_OK) {
        d->lastError = QString(lua_tostring(d->L, -1));
        lua_pop(d->L, 1);
        return false;
    }
    
    // Execute chunk
    status = lua_pcall(d->L, 0, LUA_MULTRET, 0);
    
    if (status != LUA_OK) {
        d->lastError = QString(lua_tostring(d->L, -1));
        lua_pop(d->L, 1);
        emit error(d->lastError);
        return false;
    }
    
    return true;
}

bool LuaState::callFunction(const QString& functionName, const QStringList& args) {
    if (!d->L) {
        return false;
    }
    
    // Push function onto stack
    lua_getglobal(d->L, functionName.toUtf8().constData());
    
    if (!lua_isfunction(d->L, -1)) {
        lua_pop(d->L, 1);
        return false;
    }
    
    // Push arguments
    for (const QString& arg : args) {
        pushString(arg);
    }
    
    // Call function
    int status = lua_pcall(d->L, args.size(), 0, 0);
    
    if (status != LUA_OK) {
        d->lastError = QString(lua_tostring(d->L, -1));
        lua_pop(d->L, 1);
        emit error(d->lastError);
        return false;
    }
    
    return true;
}

bool LuaState::functionExists(const QString& functionName) {
    if (!d->L) {
        return false;
    }
    
    lua_getglobal(d->L, functionName.toUtf8().constData());
    bool exists = lua_isfunction(d->L, -1);
    lua_pop(d->L, 1);
    
    return exists;
}

bool LuaState::getMetadata(QString& version, QString& author, QString& description) {
    version = "1.0.0";
    author = "Unknown";
    description = "No description";
    
    if (!d->L) {
        return false;
    }
    
    // Try to get metadata from global variables
    struct {
        const char* name;
        QString* target;
    } fields[] = {
        {"PLUGIN_VERSION", &version},
        {"PLUGIN_AUTHOR", &author},
        {"PLUGIN_DESCRIPTION", &description}
    };
    
    for (const auto& field : fields) {
        lua_getglobal(d->L, field.name);
        if (lua_isstring(d->L, -1)) {
            *field.target = QString(lua_tostring(d->L, -1));
        }
        lua_pop(d->L, 1);
    }
    
    return true;
}

// ============================================================================
// Variable Access
// ============================================================================

void LuaState::setGlobal(const QString& name, const QString& value) {
    if (!d->L) return;
    
    pushString(value);
    lua_setglobal(d->L, name.toUtf8().constData());
}

QString LuaState::getGlobal(const QString& name) {
    if (!d->L) return QString();
    
    lua_getglobal(d->L, name.toUtf8().constData());
    QString value = toString(-1);
    lua_pop(d->L, 1);
    
    return value;
}

void LuaState::clearGlobals() {
    if (!d->L) return;
    
    lua_newtable(d->L);
    lua_setglobal(d->L, "_G");
}

// ============================================================================
// Helper Methods
// ============================================================================

void LuaState::pushString(const QString& str) {
    lua_pushstring(d->L, str.toUtf8().constData());
}

QString LuaState::toString(int index) {
    if (lua_isstring(d->L, index)) {
        return QString(lua_tostring(d->L, index));
    }
    return QString();
}

// ============================================================================
// Lua C Functions
// ============================================================================

int LuaState::luaPrint(lua_State* L) {
    int n = lua_gettop(L);
    QString output;
    
    for (int i = 1; i <= n; ++i) {
        if (i > 1) output += "\t";
        
        switch (lua_type(L, i)) {
            case LUA_TSTRING:
                output += lua_tostring(L, i);
                break;
            case LUA_TNUMBER:
                output += QString::number(lua_tonumber(L, i));
                break;
            case LUA_TBOOLEAN:
                output += lua_toboolean(L, i) ? "true" : "false";
                break;
            case LUA_TNIL:
                output += "nil";
                break;
            default:
                output += lua_typename(L, lua_type(L, i));
                break;
        }
    }
    
    qDebug().noquote() << "[Lua]" << output;
    
    return 0;
}

int LuaState::luaLog(lua_State* L) {
    int n = lua_gettop(L);
    QString output;
    
    for (int i = 1; i <= n; ++i) {
        if (i > 1) output += " ";
        
        if (lua_isstring(L, i)) {
            output += lua_tostring(L, i);
        } else if (lua_isnumber(L, i)) {
            output += QString::number(lua_tonumber(L, i));
        }
    }
    
    qDebug().noquote() << "[Plugin]" << output;
    
    return 0;
}

int LuaState::luaGetVersion(lua_State* L) {
    lua_pushstring(L, LocalStorage::getVersionString().toUtf8().constData());
    return 1;
}

int LuaState::luaGetPlatform(lua_State* L) {
#ifdef _WIN32
    lua_pushstring(L, "Windows");
#elif __APPLE__
    lua_pushstring(L, "macOS");
#elif __linux__
    lua_pushstring(L, "Linux");
#else
    lua_pushstring(L, "Unknown");
#endif
    return 1;
}

} // namespace BummerGram
