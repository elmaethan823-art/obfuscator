// main.cpp – Luau Obfuscator with built‑in HTTP server and HTML UI
// Compile: g++ -std=c++17 main.cpp -o luau-obfuscator -pthread
//          (requires httplib.h in your include path)

#include <httplib.h>
#include <string>
#include <sstream>
#include <regex>
#include <unordered_map>
#include <cctype>
#include <stdexcept>

// ----------------------------------------------------------------------
// Obfuscation engine (stub – replace with real Luau AST passes)
// ----------------------------------------------------------------------

std::string simple_obfuscate(const std::string& source, bool rename, bool encrypt, bool flatten) {
    std::string result = source;

    if (rename) {
        // Very naive: find identifiers after 'local' and replace them with _a, _b, ...
        std::regex local_var(R"(\blocal\s+([a-zA-Z_][a-zA-Z0-9_]*))");
        std::smatch match;
        std::string temp = result;
        int counter = 0;
        std::unordered_map<std::string, std::string> rename_map;

        // Collect all local variable names (first pass)
        std::string::const_iterator start = temp.cbegin();
        while (std::regex_search(start, temp.cend(), match, local_var)) {
            std::string old_name = match[1];
            if (rename_map.find(old_name) == rename_map.end()) {
                rename_map[old_name] = "_" + std::to_string(counter++);
            }
            start = match.suffix().first;
        }

        // Replace all occurrences of those variables (not perfect, just demo)
        for (const auto& [old, neu] : rename_map) {
            std::regex word_regex("\\b" + old + "\\b");
            temp = std::regex_replace(temp, word_regex, neu);
        }
        result = temp;
    }

    if (encrypt) {
        result += "\n-- String encryption would be applied here.\n";
    }

    if (flatten) {
        result += "\n-- Control flow flattening would be applied here.\n";
    }

    return result;
}

// ----------------------------------------------------------------------
// HTTP server
// ----------------------------------------------------------------------

int main() {
    using namespace httplib;

    Server svr;

    // HTML page (embedded as raw string)
    const std::string html_page = R"(
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <title>Luau Obfuscator</title>
    <style>
        body { font-family: Arial, sans-serif; margin: 30px; background: #f4f4f9; }
        h1 { color: #333; }
        textarea { width: 100%; max-width: 800px; height: 200px; font-family: monospace; }
        .options { margin: 15px 0; }
        .options label { margin-right: 20px; }
        input[type=submit] { padding: 10px 20px; background: #2a7; color: white; border: none; border-radius: 5px; cursor: pointer; }
        input[type=submit]:hover { background: #1a6; }
        #result { margin-top: 20px; background: #fff; padding: 15px; border: 1px solid #ccc; border-radius: 5px; white-space: pre-wrap; word-wrap: break-word; }
    </style>
</head>
<body>
    <h1>🔒 Luau Obfuscator</h1>
    <p>Paste your Luau code below and choose obfuscation options.</p>
    <form action="/obfuscate" method="post">
        <textarea name="code" placeholder="-- Your Luau code here ..."></textarea>
        <div class="options">
            <label><input type="checkbox" name="rename" checked> Rename variables</label>
            <label><input type="checkbox" name="encrypt" checked> Encrypt strings</label>
            <label><input type="checkbox" name="flatten" checked> Flatten control flow</label>
        </div>
        <input type="submit" value="Obfuscate">
    </form>
    <div id="result"></div>
    <script>
        // Optional: display result without page reload
        document.querySelector('form').addEventListener('submit', async (e) => {
            e.preventDefault();
            const form = e.target;
            const formData = new FormData(form);
            const response = await fetch('/obfuscate', { method: 'POST', body: formData });
            const text = await response.text();
            document.getElementById('result').textContent = text;
        });
    </script>
</body>
</html>
)";

    // Serve the HTML form at GET /
    svr.Get("/", [&](const Request&, Response& res) {
        res.set_content(html_page, "text/html");
    });

    // Handle obfuscation POST
    svr.Post("/obfuscate", [&](const Request& req, Response& res) {
        auto code = req.get_param_value("code");
        bool rename = req.get_param_value("rename") == "on";
        bool encrypt = req.get_param_value("encrypt") == "on";
        bool flatten = req.get_param_value("flatten") == "on";

        if (code.empty()) {
            res.set_content("Error: No code provided.", "text/plain");
            return;
        }

        try {
            std::string obfuscated = simple_obfuscate(code, rename, encrypt, flatten);
            res.set_content(obfuscated, "text/plain");
        } catch (const std::exception& e) {
            res.set_content(std::string("Error: ") + e.what(), "text/plain");
        }
    });

    std::cout << "Luau Obfuscator server running on http://localhost:8080" << std::endl;
    svr.listen("0.0.0.0", 8080);

    return 0;
}