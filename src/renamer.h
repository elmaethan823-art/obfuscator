// renamer.h
#pragma once

#include <string>
#include <unordered_map>
#include <vector>

// Forward declarations (actual includes are in the .cpp)
namespace Luau {
    struct AstStatBlock;
    struct AstLocal;
    struct AstExprGlobal;
}

/**
 * Renames identifiers in a Luau AST.
 * By default, renames all local variables (and optionally globals) to
 * short, meaningless names (e.g., "_a", "_b", ...).
 *
 * Usage:
 *   Renamer renamer;
 *   renamer.setRenameGlobals(false);   // keep global names intact
 *   renamer.rename(root);              // root is Luau::AstStatBlock*
 */
class Renamer {
public:
    Renamer() = default;

    // Control whether global variables are also renamed (default: false)
    void setRenameGlobals(bool enable) { renameGlobals_ = enable; }

    // Main entry: traverse the AST and rename all applicable identifiers.
    void rename(Luau::AstStatBlock* root);

    // Optional: get the mapping from original name to new name (for debugging)
    const std::unordered_map<std::string, std::string>& getNameMap() const {
        return nameMap_;
    }

private:
    // Internal visitor implementation
    void visitBlock(Luau::AstStatBlock* block);
    void visitLocal(Luau::AstLocal* local);
    void visitGlobal(Luau::AstExprGlobal* global);

    // Generate a unique short name (e.g., "_a", "_b", ...)
    std::string generateName();

    // Check if a name is a Luau keyword or built-in that should not be renamed.
    bool isReserved(const std::string& name) const;

    // Mapping from original name to new name for the current scope.
    // In a full implementation, you'd keep a stack of scopes, but for simplicity
    // we use a single map and handle shadowing by keeping only the innermost.
    // A robust implementation would use a scope stack.
    std::unordered_map<std::string, std::string> nameMap_;

    // Counter for generating unique short names.
    int counter_ = 0;

    // Flag to rename globals (default: false).
    bool renameGlobals_ = false;

    // List of Luau keywords and built‑ins that must not be renamed.
    static const std::vector<std::string> reservedNames;
};