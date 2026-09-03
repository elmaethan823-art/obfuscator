// utils.cpp
#include "utils.h"

// ----------------------------------------------------------------------
// Luau includes (uncomment and adjust paths)
// #include <Luau/Ast.h>
// #include <Luau/Builder.h>  // if available for AST construction
// ----------------------------------------------------------------------

#include <random>
#include <chrono>
#include <sstream>
#include <stdexcept>
#include <cctype>
#include <algorithm>
#include <iostream>

namespace Utils {

// ----------------------------------------------------------------------
// Random generators
// ----------------------------------------------------------------------

std::string generateRandomString(int length) {
    static const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
    static std::mt19937 rng(static_cast<unsigned>(std::chrono::steady_clock::now().time_since_epoch().count()));
    std::uniform_int_distribution<> dist(0, sizeof(alphanum) - 2);
    std::string result;
    result.reserve(length);
    for (int i = 0; i < length; ++i) {
        result += alphanum[dist(rng)];
    }
    return result;
}

int randomInt(int min, int max) {
    static std::mt19937 rng(static_cast<unsigned>(std::chrono::steady_clock::now().time_since_epoch().count()));
    std::uniform_int_distribution<> dist(min, max);
    return dist(rng);
}

uint8_t randomByte() {
    static std::mt19937 rng(static_cast<unsigned>(std::chrono::steady_clock::now().time_since_epoch().count()));
    std::uniform_int_distribution<uint16_t> dist(0, 255);
    return static_cast<uint8_t>(dist(rng));
}

// ----------------------------------------------------------------------
// String escaping for Luau literals
// ----------------------------------------------------------------------

std::string escapeLuauString(const std::string& raw) {
    std::ostringstream oss;
    oss << '"';
    for (char c : raw) {
        switch (c) {
            case '"':  oss << "\\\""; break;
            case '\\': oss << "\\\\"; break;
            case '\n': oss << "\\n";  break;
            case '\r': oss << "\\r";  break;
            case '\t': oss << "\\t";  break;
            case '\f': oss << "\\f";  break;
            case '\b': oss << "\\b";  break;
            default:
                if (static_cast<unsigned char>(c) < 32 || c == 127) {
                    // Non‑printable: use decimal escape \ddd
                    oss << '\\' << std::oct << static_cast<int>(static_cast<unsigned char>(c));
                } else {
                    oss << c;
                }
                break;
        }
    }
    oss << '"';
    return oss.str();
}

std::string unescapeLuauString(const std::string& escaped) {
    if (escaped.size() < 2 || escaped.front() != '"' || escaped.back() != '"') {
        throw std::runtime_error("Invalid Luau string literal: missing quotes");
    }
    std::string raw;
    raw.reserve(escaped.size() - 2);
    for (size_t i = 1; i < escaped.size() - 1; ++i) {
        char c = escaped[i];
        if (c == '\\') {
            if (i + 1 >= escaped.size() - 1) {
                throw std::runtime_error("Incomplete escape sequence");
            }
            char next = escaped[++i];
            switch (next) {
                case '"':  raw.push_back('"'); break;
                case '\\': raw.push_back('\\'); break;
                case 'n':  raw.push_back('\n'); break;
                case 'r':  raw.push_back('\r'); break;
                case 't':  raw.push_back('\t'); break;
                case 'f':  raw.push_back('\f'); break;
                case 'b':  raw.push_back('\b'); break;
                default:
                    // Decimal escape: \ddd (three digits)
                    if (std::isdigit(static_cast<unsigned char>(next))) {
                        std::string octal;
                        octal.push_back(next);
                        for (int j = 0; j < 2 && i + 1 < escaped.size() - 1; ++j) {
                            char d = escaped[i + 1];
                            if (std::isdigit(static_cast<unsigned char>(d))) {
                                octal.push_back(d);
                                ++i;
                            } else break;
                        }
                        int val = std::stoi(octal, nullptr, 8);
                        if (val < 0 || val > 255) {
                            throw std::runtime_error("Invalid octal escape: " + octal);
                        }
                        raw.push_back(static_cast<char>(val));
                    } else {
                        raw.push_back(next); // unknown escape, keep as is
                    }
                    break;
            }
        } else {
            raw.push_back(c);
        }
    }
    return raw;
}

// ----------------------------------------------------------------------
// Unique name generation
// ----------------------------------------------------------------------

static int uniqueCounter = 0;

std::string makeUniqueName(const std::string& prefix) {
    return prefix + "_" + std::to_string(uniqueCounter++);
}

void resetUniqueNameCounter() {
    uniqueCounter = 0;
}

// ----------------------------------------------------------------------
// Reserved names
// ----------------------------------------------------------------------

bool isReservedName(const std::string& name) {
    static const std::vector<std::string> reserved = {
        // Luau keywords
        "and", "break", "do", "else", "elseif", "end", "false", "for",
        "function", "goto", "if", "in", "local", "nil", "not", "or",
        "repeat", "return", "then", "true", "until", "while",
        // Global built‑ins (Lua 5.1)
        "_G", "_VERSION", "assert", "collectgarbage", "coroutine",
        "debug", "error", "getfenv", "getmetatable", "ipairs", "load",
        "loadstring", "next", "pairs", "pcall", "print", "rawequal",
        "rawget", "rawset", "select", "setfenv", "setmetatable",
        "tonumber", "tostring", "type", "unpack", "xpcall",
        // Roblox/Luau specifics
        "script", "game", "workspace", "shared", "plugin", "settings",
        "Instance", "typeof", "rawset", "rawget", "printidentity", "warn",
        "tick", "time", "delay", "spawn", "wait", "syn",
        // ... add more as needed
    };
    return std::find(reserved.begin(), reserved.end(), name) != reserved.end();
}

// ----------------------------------------------------------------------
// Logging
// ----------------------------------------------------------------------

void log(int level, const std::string& msg, int verbosity) {
    if (level <= verbosity) {
        std::cerr << "[Utils] " << msg << std::endl;
    }
}

// ----------------------------------------------------------------------
// AST helpers (stubs – replace with real Luau API)
// ----------------------------------------------------------------------

void insertStatementAtTop(Luau::AstStatBlock* block, Luau::AstStat* stmt) {
    // TODO: In a real implementation, insert stmt at the beginning of block->body.
    // For now, this is a placeholder.
    // You would do: block->body.insert(block->body.begin(), stmt);
}

Luau::AstStat* makeLocalDeclaration(const std::string& varName, Luau::AstExpr* initialValue) {
    // TODO: Use Luau::AstBuilder to create a local variable declaration.
    // Return a new AstStatLocal node.
    return nullptr; // stub
}

Luau::AstStat* makeAssignment(const std::string& varName, Luau::AstExpr* value) {
    // TODO: Create an assignment statement.
    return nullptr; // stub
}

// ----------------------------------------------------------------------
// ScopeStack implementation
// ----------------------------------------------------------------------

void ScopeStack::pushScope() {
    scopes_.emplace_back();
}

void ScopeStack::popScope() {
    if (!scopes_.empty()) {
        scopes_.pop_back();
    }
}

void ScopeStack::addMapping(const std::string& oldName, const std::string& newName) {
    if (scopes_.empty()) {
        pushScope(); // ensure at least one scope
    }
    scopes_.back()[oldName] = newName;
}

std::string ScopeStack::lookup(const std::string& oldName) const {
    // Search from innermost to outermost.
    for (auto it = scopes_.rbegin(); it != scopes_.rend(); ++it) {
        auto found = it->find(oldName);
        if (found != it->end()) {
            return found->second;
        }
    }
    return {};
}

bool ScopeStack::hasInCurrentScope(const std::string& oldName) const {
    if (scopes_.empty()) return false;
    return scopes_.back().find(oldName) != scopes_.back().end();
}

} // namespace Utils
