// obfuscator.h
#pragma once

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <chrono>
#include <stdexcept>

// Forward declare Luau AST types (we don't want to force the include here)
// In a real implementation you would #include <Luau/Ast.h>
namespace Luau {
    struct AstStatBlock;
}

/**
 * Main Luau obfuscation engine.
 */
class Obfuscator {
public:
    // ------------------------------------------------------------------
    // Configuration
    // ------------------------------------------------------------------
    struct Config {
        // Basic passes
        bool renameIdentifiers = true;
        bool encryptStrings    = true;
        bool flattenControlFlow = true;
        bool obfuscateNumbers  = true;     // replace constants with expressions
        bool insertDeadCode    = true;     // add junk instructions
        bool addAntiDebug      = false;    // insert anti-debugging tricks

        // Pass order (can be overridden by registering custom passes)
        std::vector<std::string> passOrder = {
            "RenameIdentifiers",
            "EncryptStrings",
            "ObfuscateNumbers",
            "FlattenControlFlow",
            "InsertDeadCode",
            "AddAntiDebug"
        };

        // Logging level: 0 = silent, 1 = basic, 2 = detailed
        int verbosity = 1;
    };

    // ------------------------------------------------------------------
    // Result of obfuscation
    // ------------------------------------------------------------------
    struct Result {
        std::string obfuscatedCode;
        std::vector<std::string> errors;
        std::vector<std::string> warnings;
        std::chrono::milliseconds elapsed;
        bool success() const { return errors.empty(); }
    };

    // ------------------------------------------------------------------
    // Pass interface
    // ------------------------------------------------------------------
    class Pass {
    public:
        virtual ~Pass() = default;
        virtual std::string name() const = 0;
        virtual void apply(Luau::AstStatBlock* root, const Config& cfg) = 0;
    };

    // ------------------------------------------------------------------
    // Constructors / destructor
    // ------------------------------------------------------------------
    Obfuscator();
    explicit Obfuscator(const Config& cfg);
    ~Obfuscator() = default;

    // Register additional custom passes (inserted at the end of the order)
    void registerPass(std::unique_ptr<Pass> pass);

    // Obfuscate a source string
    Result obfuscate(const std::string& source);
    Result obfuscate(const std::string& source, const Config& cfg);

    // Obfuscate a file (reads source, writes result to another file)
    bool obfuscateFile(const std::string& inputPath, const std::string& outputPath);

private:
    Config config_;
    std::vector<std::unique_ptr<Pass>> extraPasses_;

    // Internal helpers
    void log(int level, const std::string& msg);
    bool parse(const std::string& source, Luau::AstStatBlock*& root);
    std::string generate(Luau::AstStatBlock* root);
    void runPasses(Luau::AstStatBlock* root);
};

// ------------------------------------------------------------------
// Built‑in passes (declarations – implementations in obfuscator.cpp)
// ------------------------------------------------------------------
namespace ObfuscatorPasses {
    std::unique_ptr<Obfuscator::Pass> makeRenameIdentifiersPass();
    std::unique_ptr<Obfuscator::Pass> makeEncryptStringsPass();
    std::unique_ptr<Obfuscator::Pass> makeObfuscateNumbersPass();
    std::unique_ptr<Obfuscator::Pass> makeFlattenControlFlowPass();
    std::unique_ptr<Obfuscator::Pass> makeInsertDeadCodePass();
    std::unique_ptr<Obfuscator::Pass> makeAddAntiDebugPass();
}