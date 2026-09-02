// string_encrypt.cpp
#include "string_encrypt.h"
#include "obfuscator.h" // for Obfuscator::Config and Pass base class

// ----------------------------------------------------------------------
// Luau includes (uncomment and adjust paths)
// #include <Luau/Ast.h>
// #include <Luau/Visitor.h>
// #include <Luau/Builder.h>   // for constructing new AST nodes
// ----------------------------------------------------------------------

#include <vector>
#include <cstdint>
#include <cstdlib>
#include <ctime>
#include <algorithm>

// For demonstration, we define stub types.
// In a real build, these would come from Luau headers.
#ifdef STUB_LUAU
namespace Luau {
    struct AstStatBlock {};
    struct AstExprString { std::string value; };
    struct AstExpr {};
    struct AstExprCall : AstExpr {};
    struct AstExprTable : AstExpr {};
}
#endif

// ----------------------------------------------------------------------
// Encryption utility: simple XOR cipher with random key per string.
// The key is a single byte (0-255) chosen randomly for each string.
// The encrypted data is stored as a sequence of byte values in a table.
// ----------------------------------------------------------------------
namespace StringEncryption {

    // Encrypt a string and return a vector of encrypted bytes and the key used.
    std::pair<std::vector<uint8_t>, uint8_t> encrypt(const std::string& plaintext) {
        static bool seeded = false;
        if (!seeded) {
            std::srand(static_cast<unsigned>(std::time(nullptr)));
            seeded = true;
        }

        uint8_t key = static_cast<uint8_t>(std::rand() % 256);
        std::vector<uint8_t> encrypted;
        encrypted.reserve(plaintext.size());
        for (unsigned char c : plaintext) {
            encrypted.push_back(c ^ key);
        }
        return {encrypted, key};
    }

    // Build a Luau table literal expression from a vector of bytes.
    // Returns a pointer to a new AstExprTable (ownership is managed by the AST allocator).
    Luau::AstExprTable* buildTableFromBytes(const std::vector<uint8_t>& bytes) {
        // TODO: In a real implementation, use Luau::AstBuilder to create a table node.
        // For stub, return nullptr.
        return nullptr;
    }

    // Build a call to the decrypt function.
    // The decrypt function is named "__decrypt" (or a random name).
    Luau::AstExprCall* buildDecryptCall(Luau::AstExprTable* encryptedTable) {
        // TODO: Construct an AstExprCall that calls a global/local function "__decrypt"
        // with the encryptedTable as argument.
        return nullptr;
    }

} // namespace StringEncryption

// ----------------------------------------------------------------------
// Visitor that replaces all AstExprString with encrypted calls.
// ----------------------------------------------------------------------
class StringReplacer : public Luau::AstVisitor {
public:
    StringReplacer() = default;

    // Main entry point: visit the entire AST and perform replacements.
    void replaceAll(Luau::AstStatBlock* root) {
        // In a real implementation, we would traverse the AST and override visitExprString.
        // Using a visitor, we collect all string literal nodes and then replace them.
        // Since we cannot modify the AST while visiting, we first collect them,
        // then replace after the visit.
        traverse(root);
        for (auto* node : toReplace_) {
            // Replace the node in its parent. This requires knowing the parent context.
            // For simplicity, we use a separate rewrite pass.
        }
    }

    // Override the visit method for string expressions.
    // The Luau visitor pattern: visit(AstExprString*) returns bool.
    bool visit(Luau::AstExprString* expr) override {
        // Collect the node; we'll handle replacement later.
        toReplace_.push_back(expr);
        return true; // continue traversal
    }

private:
    std::vector<Luau::AstExprString*> toReplace_;
};

// ----------------------------------------------------------------------
// Implementation of the pass
// ----------------------------------------------------------------------

void EncryptStringsPass::apply(Luau::AstStatBlock* root, const Obfuscator::Config& cfg) {
    if (!root) return;

    // Step 1: Insert the decryption function at the top of the block.
    insertDecryptFunction(root);

    // Step 2: Traverse the AST, find all string literals, and replace them.
    // We'll use a visitor that collects them and then we rewrite.
    // In practice, the Luau AST is immutable; we need to rebuild the containing
    // expressions. This is complex; for a complete solution, you would use
    // Luau's AST cloning/rebuilding utilities.
    // For the purpose of this skeleton, we print a message.
    // Real implementation would do:
    //   StringReplacer replacer;
    //   replacer.replaceAll(root);
    //   // Then for each collected node, replace with new encrypted call.
}

void EncryptStringsPass::insertDecryptFunction(Luau::AstStatBlock* block) {
    // Build the decryption function:
    //   local function __decrypt(t)
    //       local s = ""
    //       for i = 1, #t do
    //           s = s .. string.char(t[i] ^ key)
    //       end
    //       return s
    //   end
    // 
    // The key is actually per-string, so each encryption uses its own key.
    // Therefore we need to store the key inside the encrypted table (e.g., as the first element)
    // or pass it as a second argument.
    // Let's store the key as the first element of the table, and the encrypted bytes follow.
    // Then the decryption function reads the key from t[1], and the data from t[2..end].

    // Actually for simplicity, we can pass the key as a second argument to the decrypt function,
    // but that would require each call to include the key. Instead, we can embed the key in the table.
    // Let's store the key as t[1], and the data starting at t[2].

    // Revised decryption function:
    //   local function __decrypt(t)
    //       local key = t[1]
    //       local s = ""
    //       for i = 2, #t do
    //           s = s .. string.char(t[i] ^ key)
    //       end
    //       return s
    //   end

    // TODO: Use Luau::AstBuilder to construct these nodes.
    // Insert the function statement at the beginning of the block.
}
