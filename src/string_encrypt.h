// string_encrypt.h
#pragma once

#include <string>
#include <memory>

// Forward declarations
class Obfuscator; // from obfuscator.h

/**
 * Pass that replaces all string literals with encrypted versions.
 * A global decryption function is inserted at the top of the script.
 * Each string literal becomes a call to that function with an encrypted table.
 */
class EncryptStringsPass : public Obfuscator::Pass {
public:
    std::string name() const override { return "EncryptStrings"; }

    // Apply the pass to the AST root.
    void apply(Luau::AstStatBlock* root, const Obfuscator::Config& cfg) override;

private:
    // Helper to insert the decryption function at the beginning of the block.
    void insertDecryptFunction(Luau::AstStatBlock* block);

    // Helper to encrypt a single string and produce a table literal.
    // Returns the new expression node (a call to the decrypt function).
    Luau::AstExpr* encryptString(Luau::AstExprString* original);
};