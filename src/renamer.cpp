// renamer.cpp
#include "renamer.h"

// ----------------------------------------------------------------------
// Luau includes (uncomment and adjust paths to your Luau build)
// #include <Luau/Ast.h>
// #include <Luau/Visitor.h>
// ----------------------------------------------------------------------

#include <algorithm>
#include <cctype>

// For demonstration, we define stub types – replace with real Luau types.
// In a real build you would include the Luau headers and remove these stubs.
#ifdef STUB_LUAU
namespace Luau {
    struct AstStatBlock {};
    struct AstLocal { std::string name; };
    struct AstExprGlobal { std::string name; };
}
#endif

// ----------------------------------------------------------------------
// Reserved names (keywords + built‑in globals)
// ----------------------------------------------------------------------
const std::vector<std::string> Renamer::reservedNames = {
    // Luau keywords
    "and", "break", "do", "else", "elseif", "end", "false", "for",
    "function", "goto", "if", "in", "local", "nil", "not", "or",
    "repeat", "return", "then", "true", "until", "while",

    // Global built‑ins (common ones)
    "_G", "_VERSION", "assert", "collectgarbage", "coroutine",
    "debug", "error", "getfenv", "getmetatable", "ipairs", "load",
    "loadstring", "next", "pairs", "pcall", "print", "rawequal",
    "rawget", "rawset", "select", "setfenv", "setmetatable",
    "tonumber", "tostring", "type", "unpack", "xpcall",

    // Roblox/Luau specific
    "script", "game", "workspace", "shared", "plugin", "settings",
    // ... add more as needed
};

// ----------------------------------------------------------------------
// Implementation
// ----------------------------------------------------------------------

bool Renamer::isReserved(const std::string& name) const {
    return std::find(reservedNames.begin(), reservedNames.end(), name) != reservedNames.end();
}

std::string Renamer::generateName() {
    // Generate names like _a, _b, ..., _z, _aa, _ab, ...
    // We'll use a simple base‑26 counter.
    std::string name = "_";
    int n = counter_++;
    do {
        name += char('a' + (n % 26));
        n /= 26;
    } while (n > 0);
    return name;
}

void Renamer::rename(Luau::AstStatBlock* root) {
    if (!root) return;
    // In a full implementation, you'd clear the name map and traverse the AST.
    // Since we are a stub, we print a message.
    // TODO: Implement actual traversal using Luau::AstVisitor.
    // The visitor would call visitLocal(), visitGlobal() for each identifier.
}

// ----------------------------------------------------------------------
// Stub implementations – replace with real AST traversal.
// ----------------------------------------------------------------------

void Renamer::visitBlock(Luau::AstStatBlock* block) {
    // Traverse all statements in the block, recursively visiting nested blocks.
    // For each statement, if it's a local declaration, visit the locals.
    // If it's a function definition, etc.
}

void Renamer::visitLocal(Luau::AstLocal* local) {
    if (!local || local->name.empty()) return;
    if (isReserved(local->name)) return;
    // If the same name appears in a nested scope, we would handle shadowing.
    // For simplicity, we assume no shadowing in our stub.
    auto it = nameMap_.find(local->name);
    if (it == nameMap_.end()) {
        std::string newName = generateName();
        nameMap_[local->name] = newName;
        local->name = newName;   // modify in place
    } else {
        // If already mapped, this is a later reference (or shadow), we replace it.
        local->name = it->second;
    }
}

void Renamer::visitGlobal(Luau::AstExprGlobal* global) {
    if (!global || global->name.empty()) return;
    if (!renameGlobals_) return;
    if (isReserved(global->name)) return;
    // For globals, we usually keep them unchanged, but if renaming is enabled,
    // we map them similarly.
    auto it = nameMap_.find(global->name);
    if (it == nameMap_.end()) {
        std::string newName = generateName();
        nameMap_[global->name] = newName;
        global->name = newName;
    } else {
        global->name = it->second;
    }
}

// ----------------------------------------------------------------------
// To integrate with the Obfuscator::Pass system, wrap this class:
// ----------------------------------------------------------------------
/*
#include "obfuscator.h"

class RenameIdentifiersPass : public Obfuscator::Pass {
public:
    std::string name() const override { return "RenameIdentifiers"; }
    void apply(Luau::AstStatBlock* root, const Obfuscator::Config& cfg) override {
        Renamer renamer;
        renamer.setRenameGlobals(cfg.renameGlobals); // add this flag to Config
        renamer.rename(root);
    }
};
*/