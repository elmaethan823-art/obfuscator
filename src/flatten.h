// flatten.h
#pragma once

#include <memory>
#include <string>

// Forward declarations
class Obfuscator; // from obfuscator.h

/**
 * Pass that flattens control flow.
 * All if-statements, loops (while, for, repeat), and break/continue
 * are replaced by a state machine:
 *
 *   local state = 0
 *   while true do
 *       if state == 0 then ... state = 1
 *       elseif state == 1 then ... state = 2
 *       ... end
 *   end
 *
 * Each original block becomes a case in the main switch (if-elseif chain).
 * Conditionals are transformed into computed state jumps.
 */
class FlattenControlFlowPass : public Obfuscator::Pass {
public:
    std::string name() const override { return "FlattenControlFlow"; }

    void apply(Luau::AstStatBlock* root, const Obfuscator::Config& cfg) override;

private:
    // Main transformation routine: processes a block and returns a new block.
    Luau::AstStatBlock* flattenBlock(Luau::AstStatBlock* block);

    // Helper to generate a new unique state name (e.g., "_state").
    std::string generateStateName();

    // Helper to build the dispatch loop.
    // Returns a statement that is a while-true loop containing the switch.
    Luau::AstStat* buildDispatchLoop(Luau::AstStatBlock* flattenedBody);

    // Recursively flatten statements inside a block.
    // Returns a list of statements with state assignments instead of control flow.
    std::vector<Luau::AstStat*> flattenStatements(
        const std::vector<Luau::AstStat*>& stmts,
        int& nextState
    );
};
