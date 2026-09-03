// utils.h
#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <cstdint>
#include <random>

// Forward declarations of Luau AST types (actual includes in .cpp)
namespace Luau {
    struct AstStatBlock;
    struct AstStat;
    struct AstExpr;
    // ... other types as needed
}

namespace Utils {

// ----------------------------------------------------------------------
// Random generators
// ----------------------------------------------------------------------

/**
 * Generate a random string of given length using alphanumeric characters.
 * @param length Desired length (must be > 0).
 * @return Random string.
 */
std::string generateRandomString(int length);

/**
 * Generate a random integer in [min, max] (inclusive).
 */
int randomInt(int min, int max);

/**
 * Generate a random byte (0-255).
 */
uint8_t randomByte();

// ----------------------------------------------------------------------
// String escaping for Luau literals
// ----------------------------------------------------------------------

/**
 * Convert a raw string into a Luau string literal (with quotes and escapes).
 * Example: "hello\nworld" -> "\"hello\\nworld\""
 * @param raw The raw string content.
 * @return A valid Luau string literal (including surrounding quotes).
 */
std::string escapeLuauString(const std::string& raw);

/**
 * Unescape a Luau string literal (remove quotes and interpret escape sequences).
 * Example: "\"hello\\nworld\"" -> "hello\nworld"
 * @param escaped A Luau string literal (must start and end with quotes).
 * @return The raw string content.
 * @throws std::runtime_error if the literal is malformed.
 */
std::string unescapeLuauString(const std::string& escaped);

// ----------------------------------------------------------------------
// Unique name generation
// ----------------------------------------------------------------------

/**
 * Generate a unique name with the given prefix.
 * Names are of the form "prefix_N" where N is an incrementing counter.
 * The counter is global within the process.
 */
std::string makeUniqueName(const std::string& prefix = "_tmp");

/**
 * Reset the unique name counter (useful for deterministic runs).
 */
void resetUniqueNameCounter();

// ----------------------------------------------------------------------
// Reserved names (Luau keywords + built‑ins)
// ----------------------------------------------------------------------

/**
 * Check if a name is reserved (keyword or built‑in) and should not be renamed.
 */
bool isReservedName(const std::string& name);

// ----------------------------------------------------------------------
// Logging
// ----------------------------------------------------------------------

/**
 * Log a message if the current verbosity level >= threshold.
 * @param level The verbosity level of this message (lower = more important).
 * @param msg The message to log.
 * @param verbosity The current verbosity threshold (from Obfuscator::Config).
 */
void log(int level, const std::string& msg, int verbosity);

// ----------------------------------------------------------------------
// AST helpers
// ----------------------------------------------------------------------

/**
 * Insert a statement at the beginning of a block.
 * @param block The target block.
 * @param stmt The statement to insert (ownership is transferred to the block's AST).
 */
void insertStatementAtTop(Luau::AstStatBlock* block, Luau::AstStat* stmt);

/**
 * Create a new local variable declaration statement.
 * Example: local varName = initialValue
 * @param varName The name of the local variable.
 * @param initialValue The expression for the initial value (can be nullptr for nil).
 * @return A new AstStatLocal node (must be managed by the AST allocator).
 */
Luau::AstStat* makeLocalDeclaration(const std::string& varName, Luau::AstExpr* initialValue = nullptr);

/**
 * Create an assignment statement: varName = value
 * @param varName The variable name (global or local – caller must ensure it's in scope).
 * @param value The expression to assign.
 * @return A new AstStatAssign node.
 */
Luau::AstStat* makeAssignment(const std::string& varName, Luau::AstExpr* value);

// ----------------------------------------------------------------------
// Scope management (for renaming passes)
// ----------------------------------------------------------------------

/**
 * Simple scope stack for managing variable name mappings.
 * Supports push/pop and lookup in the current scope (innermost first).
 */
class ScopeStack {
public:
    ScopeStack() = default;

    // Enter a new scope.
    void pushScope();

    // Exit the current scope.
    void popScope();

    // Add a mapping from old name to new name in the current scope.
    void addMapping(const std::string& oldName, const std::string& newName);

    // Look up the new name for an old name, searching from innermost scope outward.
    // Returns empty string if not found.
    std::string lookup(const std::string& oldName) const;

    // Check if a name exists in the current (innermost) scope.
    bool hasInCurrentScope(const std::string& oldName) const;

private:
    // Each scope is a map from old name to new name.
    std::vector<std::unordered_map<std::string, std::string>> scopes_;
};

} // namespace Utils
