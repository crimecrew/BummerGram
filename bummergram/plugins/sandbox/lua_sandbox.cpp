#include "lua_sandbox.h"
#include "../../core/localstorage.h"

#include <QSet>
#include <QRegularExpression>
#include <QDebug>
#include <lua.hpp>

namespace BummerGram {

// ============================================================================
// Private Implementation
// ============================================================================

class LuaSandbox::Impl {
public:
    QSet<QString> allowedPaths;
    int timeoutMs = 5000; // 5 second default timeout
    bool allowDangerousFunctions = false;
    
    // Dangerous patterns to detect
    QList<QRegularExpression> dangerousPatterns = {
        // Load bytecode / loadstring with dynamic code
        QRegularExpression(R"(loadstring\s*\()", QRegularExpression::CaseInsensitiveOption),
        QRegularExpression(R"(load\s*\(\s*[^']+['\"])", QRegularExpression::CaseInsensitiveOption),
        
        // OS execute / os.exit / os.remove
        QRegularExpression(R"(\bos\.execute\b)", QRegularExpression::CaseInsensitiveOption),
        QRegularExpression(R"(\bos\.remove\b)", QRegularExpression::CaseInsensitiveOption),
        QRegularExpression(R"(\bos\.rename\b)", QRegularExpression::CaseInsensitiveOption),
        QRegularExpression(R"(\bos\.tmpname\b)", QRegularExpression::CaseInsensitiveOption),
        
        // io.open / io.popen
        QRegularExpression(R"(\bio\.open\b)", QRegularExpression::CaseInsensitiveOption),
        QRegularExpression(R"(\bio\.popen\b)", QRegularExpression::CaseInsensitiveOption),
        QRegularExpression(R"(\bio\.input\b)", QRegularExpression::CaseInsensitiveOption),
        QRegularExpression(R"(\bio\.output\b)", QRegularExpression::CaseInsensitiveOption),
        
        // require with filesystem
        QRegularExpression(R"(\brequire\s*\(\s*['\"]os\b)", QRegularExpression::CaseInsensitiveOption),
        QRegularExpression(R"(\brequire\s*\(\s*['\"]io\b)", QRegularExpression::CaseInsensitiveOption),
        QRegularExpression(R"(\brequire\s*\(\s*['\"]socket\b)", QRegularExpression::CaseInsensitiveOption),
        
        // Metatable manipulation tricks
        QRegularExpression(R"(\bsetmetatable\s*\([^,]+,\s*\{[^}]*__[^}]*\})", QRegularExpression::CaseInsensitiveOption),
        
        // Debug access
        QRegularExpression(R"(\bdebug\.)", QRegularExpression::CaseInsensitiveOption),
        
        // Dofile
        QRegularExpression(R"(\bdofile\b)", QRegularExpression::CaseInsensitiveOption),
    };
    
    QStringList validationErrors;
};

void LuaSandbox::ImplDeleter::operator()(LuaSandbox::Impl* p) const {
    delete p;
}

} // namespace

// ============================================================================
// Construction
// ============================================================================

LuaSandbox::LuaSandbox()
    : d(std::make_unique<Impl>())
{
    // Set default allowed paths
    d->allowedPaths.insert(LocalStorage::getAppDataPath());
    d->allowedPaths.insert(LocalStorage::getPluginsPath());
    d->allowedPaths.insert(LocalStorage::getConfigPath());
    d->allowedPaths.insert(LocalStorage::getCachePath());
}

LuaSandbox::~LuaSandbox() = default;

// ============================================================================
// Configuration
// ============================================================================

void LuaSandbox::setAllowedPaths(const QStringList& paths) {
    d->allowedPaths.clear();
    for (const QString& path : paths) {
        d->allowedPaths.insert(path);
    }
}

void LuaSandbox::addAllowedPath(const QString& path) {
    d->allowedPaths.insert(path);
}

void LuaSandbox::setTimeout(int milliseconds) {
    d->timeoutMs = milliseconds;
}

void LuaSandbox::setAllowDangerousFunctions(bool allow) {
    d->allowDangerousFunctions = allow;
}

// ============================================================================
// Validation
// ============================================================================

bool LuaSandbox::isPathAllowed(const QString& path) const {
    for (const QString& allowed : d->allowedPaths) {
        if (path.startsWith(allowed)) {
            return true;
        }
    }
    return false;
}

bool LuaSandbox::validateScript(const QString& script) const {
    d->validationErrors.clear();
    
    if (d->allowDangerousFunctions) {
        return true;
    }
    
    // Check for dangerous patterns
    if (containsDangerousPattern(script)) {
        return false;
    }
    
    return true;
}

QStringList LuaSandbox::getValidationErrors() const {
    return d->validationErrors;
}

bool LuaSandbox::containsDangerousPattern(const QString& script) const {
    for (const QRegularExpression& pattern : d->dangerousPatterns) {
        QRegularExpressionMatch match = pattern.match(script);
        if (match.hasMatch()) {
            d->validationErrors.append(
                QString("Dangerous pattern detected: %1 (matched: %2)")
                    .arg(pattern.pattern())
                    .arg(match.captured(0))
            );
            return true;
        }
    }
    
    return false;
}

// ============================================================================
// Static Pattern Detection
// ============================================================================

bool LuaSandbox::isDangerousLoadstring(const QString& script) {
    return script.contains("loadstring", Qt::CaseInsensitive) ||
           script.contains("load(", Qt::CaseInsensitive);
}

bool LuaSandbox::isDangerousOsExecute(const QString& script) {
    QRegularExpression re(R"(\bos\.execute\b)", QRegularExpression::CaseInsensitiveOption);
    return re.match(script).hasMatch();
}

bool LuaSandbox::isDangerousIoAccess(const QString& script) {
    QRegularExpression re(R"(\bio\.(open|popen|input|output)\b)", 
                         QRegularExpression::CaseInsensitiveOption);
    return re.match(script).hasMatch();
}

bool LuaSandbox::isDangerousRequireFs(const QString& script) {
    return script.contains("require(\"os\"", Qt::CaseInsensitive) ||
           script.contains("require('os'", Qt::CaseInsensitive) ||
           script.contains("require(\"io\"", Qt::CaseInsensitive) ||
           script.contains("require('io'", Qt::CaseInsensitive);
}

bool LuaSandbox::isDangerousMetatableTrick(const QString& script) {
    // Basic check for setmetatable with __index that accesses globals
    return script.contains("setmetatable", Qt::CaseInsensitive) &&
           script.contains("__index", Qt::CaseInsensitive);
}

// ============================================================================
// Application
// ============================================================================

void LuaSandbox::applyToState(lua_State* L) const {
    if (!L) return;
    
    // Remove dangerous functions from globals
    // Instead of completely removing, we nil them
    
    // Remove os.execute, os.remove, os.rename
    lua_getglobal(L, "os");
    if (lua_istable(L, -1)) {
        lua_pushnil(L);
        lua_setfield(L, -2, "execute");
        lua_pushnil(L);
        lua_setfield(L, -2, "remove");
        lua_pushnil(L);
        lua_setfield(L, -2, "rename");
        lua_pushnil(L);
        lua_setfield(L, -2, "tmpname");
        lua_pushnil(L);
        lua_setfield(L, -2, "exit");
    }
    lua_pop(L, 1);
    
    // Remove io.open, io.popen
    lua_getglobal(L, "io");
    if (lua_istable(L, -1)) {
        lua_pushnil(L);
        lua_setfield(L, -2, "popen");
    }
    lua_pop(L, 1);
    
    // Remove dofile
    lua_pushnil(L);
    lua_setglobal(L, "dofile");
    
    // Remove loadfile (encourage using require instead)
    lua_pushnil(L);
    lua_setglobal(L, "loadfile");
    
    qDebug() << "[LuaSandbox] Restrictions applied";
}

} // namespace BummerGram
