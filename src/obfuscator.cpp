// obfuscator.cpp
#include "obfuscator.h"

// ----------------------------------------------------------------------
// Luau includes (uncomment when building with Luau)
// #include <Luau/Parser.h>
// #include <Luau/Compiler.h>
// #include <Luau/ToString.h>
// #include <Luau/Ast.h>
// ----------------------------------------------------------------------

#include <iostream>
#include <sstream>
#include <fstream>
#include <unordered_map>
#include <chrono>
#include <algorithm>

// ----------------------------------------------------------------------
// Built‑in passes – each derived from Obfuscator::Pass
// ----------------------------------------------------------------------

namespace {

    // Base pass with common logging
    class BasePass : public Obfuscator::Pass {
    protected:
        void log(int level, const std::string& msg) {
            // In a real implementation, we'd forward to the Obfuscator's logger.
            if (level <= 1) {
                std::cout << "[Pass] " << msg << std::endl;
            }
        }
    };

    // Rename identifiers pass (stub)
    class RenameIdentifiersPass : public BasePass {
    public:
        std::string name() const override { return "RenameIdentifiers"; }
        void apply(Luau::AstStatBlock* root, const Obfuscator::Config& cfg) override {
            // TODO: Traverse AST and rename all local/global variables.
            // This is where you'd use a Luau::AstVisitor.
            log(cfg.verbosity, "Renaming identifiers... (stub)");
        }
    };

    // Encrypt strings pass (stub)
    class EncryptStringsPass : public BasePass {
    public:
        std::string name() const override { return "EncryptStrings"; }
        void apply(Luau::AstStatBlock* root, const Obfuscator::Config& cfg) override {
            log(cfg.verbosity, "Encrypting string literals... (stub)");
        }
    };

    // Obfuscate numbers pass (stub)
    class ObfuscateNumbersPass : public BasePass {
    public:
        std::string name() const override { return "ObfuscateNumbers"; }
        void apply(Luau::AstStatBlock* root, const Obfuscator::Config& cfg) override {
            log(cfg.verbosity, "Obfuscating numeric constants... (stub)");
        }
    };

    // Flatten control flow pass (stub)
    class FlattenControlFlowPass : public BasePass {
    public:
        std::string name() const override { return "FlattenControlFlow"; }
        void apply(Luau::AstStatBlock* root, const Obfuscator::Config& cfg) override {
            log(cfg.verbosity, "Flattening control flow... (stub)");
        }
    };

    // Insert dead code pass (stub)
    class InsertDeadCodePass : public BasePass {
    public:
        std::string name() const override { return "InsertDeadCode"; }
        void apply(Luau::AstStatBlock* root, const Obfuscator::Config& cfg) override {
            log(cfg.verbosity, "Inserting dead code... (stub)");
        }
    };

    // Add anti‑debug stubs (stub)
    class AddAntiDebugPass : public BasePass {
    public:
        std::string name() const override { return "AddAntiDebug"; }
        void apply(Luau::AstStatBlock* root, const Obfuscator::Config& cfg) override {
            log(cfg.verbosity, "Adding anti‑debugging stubs... (stub)");
        }
    };

} // anonymous namespace

// ----------------------------------------------------------------------
// Factory functions for built‑in passes
// ----------------------------------------------------------------------
namespace ObfuscatorPasses {
    std::unique_ptr<Obfuscator::Pass> makeRenameIdentifiersPass() {
        return std::make_unique<RenameIdentifiersPass>();
    }
    std::unique_ptr<Obfuscator::Pass> makeEncryptStringsPass() {
        return std::make_unique<EncryptStringsPass>();
    }
    std::unique_ptr<Obfuscator::Pass> makeObfuscateNumbersPass() {
        return std::make_unique<ObfuscateNumbersPass>();
    }
    std::unique_ptr<Obfuscator::Pass> makeFlattenControlFlowPass() {
        return std::make_unique<FlattenControlFlowPass>();
    }
    std::unique_ptr<Obfuscator::Pass> makeInsertDeadCodePass() {
        return std::make_unique<InsertDeadCodePass>();
    }
    std::unique_ptr<Obfuscator::Pass> makeAddAntiDebugPass() {
        return std::make_unique<AddAntiDebugPass>();
    }
}

// ----------------------------------------------------------------------
// Obfuscator implementation
// ----------------------------------------------------------------------

Obfuscator::Obfuscator() : Obfuscator(Config{}) {}

Obfuscator::Obfuscator(const Config& cfg) : config_(cfg) {
    // Register built‑in passes (if they are enabled)
    // We'll decide which to actually run based on the config in runPasses().
}

void Obfuscator::registerPass(std::unique_ptr<Pass> pass) {
    extraPasses_.push_back(std::move(pass));
}

Obfuscator::Result Obfuscator::obfuscate(const std::string& source) {
    return obfuscate(source, config_);
}

Obfuscator::Result Obfuscator::obfuscate(const std::string& source, const Config& cfg) {
    config_ = cfg; // use the passed config
    Result result;
    auto start = std::chrono::steady_clock::now();

    try {
        // 1. Parse
        Luau::AstStatBlock* root = nullptr;
        if (!parse(source, root)) {
            throw std::runtime_error("Failed to parse source");
        }

        // 2. Run passes
        runPasses(root);

        // 3. Generate code
        result.obfuscatedCode = generate(root);

        // (Optional) delete root when done – but we rely on Luau's memory management
    } catch (const std::exception& e) {
        result.errors.push_back(e.what());
    }

    result.elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - start
    );
    return result;
}

bool Obfuscator::obfuscateFile(const std::string& inputPath, const std::string& outputPath) {
    std::ifstream in(inputPath);
    if (!in.is_open()) {
        log(0, "Cannot open input file: " + inputPath);
        return false;
    }
    std::string source((std::istreambuf_iterator<char>(in)),
                       std::istreambuf_iterator<char>());
    in.close();

    auto result = obfuscate(source);
    if (!result.success()) {
        for (const auto& err : result.errors)
            log(0, err);
        return false;
    }

    std::ofstream out(outputPath);
    if (!out.is_open()) {
        log(0, "Cannot open output file: " + outputPath);
        return false;
    }
    out << result.obfuscatedCode;
    out.close();
    return true;
}

// ----------------------------------------------------------------------
// Private helpers
// ----------------------------------------------------------------------

void Obfuscator::log(int level, const std::string& msg) {
    if (level <= config_.verbosity) {
        std::cerr << "[Obfuscator] " << msg << std::endl;
    }
}

bool Obfuscator::parse(const std::string& source, Luau::AstStatBlock*& root) {
    // Placeholder: in reality call Luau::Parser::parse
    // Luau::ParseResult result = Luau::Parser::parse(source.c_str(), source.size());
    // if (!result.root) return false;
    // root = result.root;
    // return true;

    // Stub: pretend it worked
    log(2, "Parsing source (stub)");
    root = nullptr; // You must set to a valid pointer in real implementation
    return true;
}

std::string Obfuscator::generate(Luau::AstStatBlock* root) {
    // Placeholder: Luau::ToStringOptions opts; return Luau::toString(root, opts);
    log(2, "Generating code (stub)");
    return "-- Obfuscated code (stub)\n-- Replace with real generation.\n";
}

void Obfuscator::runPasses(Luau::AstStatBlock* root) {
    // Build the full list of passes to run, respecting the enabled flags.
    std::vector<std::unique_ptr<Pass>> passes;

    // Helper to add a pass if the corresponding config flag is true.
    auto addIf = [&](bool enabled, std::unique_ptr<Pass> pass) {
        if (enabled) passes.push_back(std::move(pass));
    };

    addIf(config_.renameIdentifiers, ObfuscatorPasses::makeRenameIdentifiersPass());
    addIf(config_.encryptStrings,    ObfuscatorPasses::makeEncryptStringsPass());
    addIf(config_.obfuscateNumbers,  ObfuscatorPasses::makeObfuscateNumbersPass());
    addIf(config_.flattenControlFlow,ObfuscatorPasses::makeFlattenControlFlowPass());
    addIf(config_.insertDeadCode,    ObfuscatorPasses::makeInsertDeadCodePass());
    addIf(config_.addAntiDebug,      ObfuscatorPasses::makeAddAntiDebugPass());

    // Add any extra passes registered by the user
    for (auto& extra : extraPasses_) {
        // We clone? For simplicity, we just move them (they will be destroyed after this)
        // But if we want to keep them, we need a clone interface. Not needed for now.
        passes.push_back(std::move(extra));
    }

    // For determinism, we could sort by name or by a priority order,
    // but we rely on the order they were added (config flags first, then extras).
    // We can also respect the config.passOrder if provided, but that's advanced.

    // Apply each pass
    for (auto& pass : passes) {
        log(1, "Applying pass: " + pass->name());
        pass->apply(root, config_);
    }
}