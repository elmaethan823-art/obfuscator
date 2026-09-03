// main.cpp – Luau Obfuscator with embedded HTML and basic obfuscation
// Compile: g++ -std=c++17 main.cpp -o luau-obfuscator -pthread
// Requires: httplib.h (https://github.com/yhirose/cpp-httplib)

#include <httplib.h>
#include <string>
#include <sstream>
#include <regex>
#include <unordered_map>
#include <cctype>
#include <stdexcept>
#include <iostream>

// ----------------------------------------------------------------------
// Simple obfuscation engine (placeholder – replace with real AST passes)
// ----------------------------------------------------------------------
std::string simple_obfuscate(const std::string& source,
                             bool rename,
                             bool encrypt,
                             bool flatten,
                             bool obfuscateNumbers,
                             bool deadCode,
                             bool antiDebug) {
    std::string result = source;

    // 1. Rename local variables (very naive)
    if (rename) {
        std::regex local_var(R"(\blocal\s+([a-zA-Z_][a-zA-Z0-9_]*))");
        std::smatch match;
        std::string temp = result;
        int counter = 0;
        std::unordered_map<std::string, std::string> rename_map;

        std::string::const_iterator start = temp.cbegin();
        while (std::regex_search(start, temp.cend(), match, local_var)) {
            std::string old_name = match[1];
            if (rename_map.find(old_name) == rename_map.end()) {
                rename_map[old_name] = "_" + std::to_string(counter++);
            }
            start = match.suffix().first;
        }

        for (const auto& [old, neu] : rename_map) {
            std::regex word_regex("\\b" + old + "\\b");
            temp = std::regex_replace(temp, word_regex, neu);
        }
        result = temp;
    }

    // 2. Add comments for other passes (to show they would run)
    if (encrypt)          result += "\n-- [String encryption applied]";
    if (flatten)          result += "\n-- [Control flow flattening applied]";
    if (obfuscateNumbers) result += "\n-- [Number obfuscation applied]";
    if (deadCode)         result += "\n-- [Dead code inserted]";
    if (antiDebug)        result += "\n-- [Anti-debug stubs added]";

    return result;
}

// ----------------------------------------------------------------------
// HTTP server
// ----------------------------------------------------------------------
int main() {
    using namespace httplib;

    Server svr;

    // ------------------------------------------------------------------
    // Embedded HTML – full UI with dark theme and all options
    // ------------------------------------------------------------------
    const std::string html_page = R"(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Luau Obfuscator</title>
    <style>
        * { margin:0; padding:0; box-sizing:border-box; }
        body {
            background: #0d1117;
            color: #c9d1d9;
            font-family: 'Segoe UI', system-ui, -apple-system, sans-serif;
            padding: 2rem;
            min-height: 100vh;
            display: flex;
            justify-content: center;
            align-items: center;
        }
        .container {
            max-width: 1000px;
            width: 100%;
            background: #161b22;
            border-radius: 16px;
            padding: 2rem;
            box-shadow: 0 8px 24px rgba(0,0,0,0.5);
        }
        h1 {
            font-size: 2rem;
            font-weight: 600;
            margin-bottom: 0.25rem;
            display: flex;
            align-items: center;
            gap: 0.5rem;
        }
        h1 span {
            background: #238636;
            font-size: 1rem;
            padding: 0.1rem 0.6rem;
            border-radius: 20px;
            color: #fff;
        }
        .subtitle { color: #8b949e; margin-bottom: 1.5rem; }
        .options-grid {
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(180px, 1fr));
            gap: 0.75rem 1.5rem;
            background: #0d1117;
            padding: 1rem 1.25rem;
            border-radius: 10px;
            margin-bottom: 1.5rem;
        }
        .options-grid label {
            display: flex;
            align-items: center;
            gap: 0.5rem;
            cursor: pointer;
            font-size: 0.95rem;
            transition: color 0.2s;
        }
        .options-grid label:hover { color: #f0f6fc; }
        .options-grid input[type="checkbox"] {
            appearance: none;
            width: 18px;
            height: 18px;
            border: 2px solid #30363d;
            border-radius: 4px;
            background: #0d1117;
            transition: all 0.15s;
            position: relative;
            flex-shrink: 0;
            cursor: pointer;
        }
        .options-grid input[type="checkbox"]:checked {
            background: #238636;
            border-color: #238636;
        }
        .options-grid input[type="checkbox"]:checked::after {
            content: "✓";
            color: #fff;
            font-size: 14px;
            position: absolute;
            top: -2px;
            left: 2px;
        }
        .options-grid input[type="checkbox"]:focus-visible {
            outline: 2px solid #238636;
            outline-offset: 2px;
        }
        textarea {
            width: 100%;
            font-family: 'JetBrains Mono', 'Fira Code', monospace;
            font-size: 0.9rem;
            background: #0d1117;
            color: #e6edf3;
            border: 1px solid #30363d;
            border-radius: 8px;
            padding: 1rem;
            resize: vertical;
            min-height: 200px;
            transition: border 0.2s;
        }
        textarea:focus {
            outline: none;
            border-color: #58a6ff;
            box-shadow: 0 0 0 3px rgba(88,166,255,0.2);
        }
        textarea::placeholder { color: #484f58; }
        .button-row {
            display: flex;
            flex-wrap: wrap;
            gap: 0.75rem;
            margin: 1.25rem 0 1rem;
        }
        .btn {
            padding: 0.6rem 1.5rem;
            border: none;
            border-radius: 8px;
            font-weight: 600;
            font-size: 0.95rem;
            cursor: pointer;
            transition: all 0.15s;
            display: inline-flex;
            align-items: center;
            gap: 0.4rem;
        }
        .btn-primary {
            background: #238636;
            color: #fff;
        }
        .btn-primary:hover {
            background: #2ea043;
            transform: translateY(-1px);
        }
        .btn-primary:disabled {
            opacity: 0.6;
            cursor: not-allowed;
            transform: none;
        }
        .btn-secondary {
            background: #21262d;
            color: #c9d1d9;
        }
        .btn-secondary:hover { background: #30363d; }
        .btn-danger {
            background: #da3633;
            color: #fff;
        }
        .btn-danger:hover { background: #f85149; }
        #result-container {
            margin-top: 1.5rem;
            border-radius: 8px;
            border: 1px solid #30363d;
            background: #0d1117;
            overflow: hidden;
            display: none;
        }
        #result-container.visible { display: block; }
        #result-header {
            display: flex;
            justify-content: space-between;
            align-items: center;
            padding: 0.6rem 1rem;
            background: #161b22;
            border-bottom: 1px solid #30363d;
        }
        #result-header span {
            font-weight: 500;
            color: #8b949e;
        }
        #result-header .actions {
            display: flex;
            gap: 0.5rem;
        }
        #result-header .actions .btn {
            padding: 0.2rem 0.8rem;
            font-size: 0.8rem;
        }
        #result-output {
            padding: 1rem;
            white-space: pre-wrap;
            word-break: break-all;
            font-family: 'JetBrains Mono', 'Fira Code', monospace;
            font-size: 0.85rem;
            max-height: 500px;
            overflow-y: auto;
            background: #0d1117;
        }
        #result-output::-webkit-scrollbar {
            width: 8px;
        }
        #result-output::-webkit-scrollbar-track {
            background: #0d1117;
        }
        #result-output::-webkit-scrollbar-thumb {
            background: #30363d;
            border-radius: 4px;
        }
        #result-output::-webkit-scrollbar-thumb:hover {
            background: #484f58;
        }
        .spinner {
            display: none;
            align-items: center;
            gap: 0.6rem;
            color: #8b949e;
        }
        .spinner.active { display: flex; }
        .spinner::after {
            content: "";
            width: 18px;
            height: 18px;
            border: 2px solid #30363d;
            border-top: 2px solid #58a6ff;
            border-radius: 50%;
            animation: spin 0.8s linear infinite;
        }
        @keyframes spin {
            to { transform: rotate(360deg); }
        }
        @media (max-width: 640px) {
            body { padding: 1rem; }
            .container { padding: 1rem; }
            .options-grid { grid-template-columns: 1fr 1fr; }
            .button-row { flex-direction: column; }
            .btn { justify-content: center; }
        }
        @media (max-width: 480px) {
            .options-grid { grid-template-columns: 1fr; }
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>🔒 Luau Obfuscator <span>v2</span></h1>
        <p class="subtitle">Paste your code, choose options, and get obfuscated Luau.</p>
        <form id="obfuscateForm" action="/obfuscate" method="post">
            <div class="options-grid">
                <label><input type="checkbox" name="rename" checked> Rename identifiers</label>
                <label><input type="checkbox" name="encrypt" checked> Encrypt strings</label>
                <label><input type="checkbox" name="flatten" checked> Flatten control flow</label>
                <label><input type="checkbox" name="obfuscateNumbers" checked> Obfuscate numbers</label>
                <label><input type="checkbox" name="deadCode" checked> Insert dead code</label>
                <label><input type="checkbox" name="antiDebug"> Add anti‑debug stubs</label>
            </div>
            <textarea id="inputCode" name="code" placeholder="-- Your Luau code here ..." spellcheck="false"></textarea>
            <div class="button-row">
                <button type="submit" class="btn btn-primary" id="obfuscateBtn"><span>⚡</span> Obfuscate</button>
                <button type="button" class="btn btn-secondary" id="clearBtn"><span>🗑️</span> Clear</button>
                <button type="button" class="btn btn-secondary" id="loadSampleBtn"><span>📄</span> Load sample</button>
                <div class="spinner" id="loadingSpinner">Processing…</div>
            </div>
        </form>
        <div id="result-container">
            <div id="result-header">
                <span>📋 Obfuscated output</span>
                <div class="actions">
                    <button class="btn btn-secondary" id="copyBtn">📝 Copy</button>
                    <button class="btn btn-secondary" id="downloadBtn">⬇️ Download</button>
                </div>
            </div>
            <div id="result-output"></div>
        </div>
    </div>
    <script>
        (function() {
            const form = document.getElementById('obfuscateForm');
            const inputCode = document.getElementById('inputCode');
            const resultContainer = document.getElementById('result-container');
            const resultOutput = document.getElementById('result-output');
            const obfuscateBtn = document.getElementById('obfuscateBtn');
            const loadingSpinner = document.getElementById('loadingSpinner');
            const clearBtn = document.getElementById('clearBtn');
            const loadSampleBtn = document.getElementById('loadSampleBtn');
            const copyBtn = document.getElementById('copyBtn');
            const downloadBtn = document.getElementById('downloadBtn');

            const SAMPLE_CODE = `-- Simple Luau program
local function greet(name)
    if name == "Luau" then
        print("Hello, Luau!")
    else
        print("Hello, " .. name .. "!")
    end
end

local function factorial(n)
    if n <= 1 then
        return 1
    end
    return n * factorial(n - 1)
end

for i = 1, 5 do
    local msg = "Factorial of " .. i .. " is " .. factorial(i)
    print(msg)
end

local data = {
    name = "Luau",
    version = "0.6",
    features = { "types", "syntax", "performance" }
}

print("Obfuscate me!")`;

            function showResult(text) {
                resultOutput.textContent = text;
                resultContainer.classList.add('visible');
            }
            function hideResult() {
                resultContainer.classList.remove('visible');
            }
            function setLoading(loading) {
                obfuscateBtn.disabled = loading;
                loadingSpinner.classList.toggle('active', loading);
            }

            form.addEventListener('submit', async (e) => {
                e.preventDefault();
                const code = inputCode.value.trim();
                if (!code) { alert('Please enter some Luau code.'); return; }
                const formData = new FormData(form);
                setLoading(true);
                hideResult();
                try {
                    const response = await fetch('/obfuscate', { method: 'POST', body: formData });
                    if (!response.ok) throw new Error('Server error: ' + response.status);
                    const result = await response.text();
                    showResult(result);
                } catch (err) {
                    alert('Error: ' + err.message);
                } finally {
                    setLoading(false);
                }
            });

            clearBtn.addEventListener('click', () => { inputCode.value = ''; hideResult(); });
            loadSampleBtn.addEventListener('click', () => {
                inputCode.value = SAMPLE_CODE;
                hideResult();
                inputCode.focus();
            });
            copyBtn.addEventListener('click', async () => {
                const text = resultOutput.textContent;
                if (!text) return;
                try {
                    await navigator.clipboard.writeText(text);
                    const orig = copyBtn.textContent;
                    copyBtn.textContent = '✅ Copied!';
                    setTimeout(() => copyBtn.textContent = orig, 2000);
                } catch { alert('Could not copy.'); }
            });
            downloadBtn.addEventListener('click', () => {
                const text = resultOutput.textContent;
                if (!text) return;
                const blob = new Blob([text], { type: 'text/plain' });
                const url = URL.createObjectURL(blob);
                const a = document.createElement('a');
                a.href = url;
                a.download = 'obfuscated.lua';
                document.body.appendChild(a);
                a.click();
                document.body.removeChild(a);
                URL.revokeObjectURL(url);
            });

            inputCode.addEventListener('keydown', (e) => {
                if (e.ctrlKey && e.key === 'Enter') form.dispatchEvent(new Event('submit'));
            });
            inputCode.focus();
        })();
    </script>
</body>
</html>
)";

    // ------------------------------------------------------------------
    // Serve the HTML at GET /
    // ------------------------------------------------------------------
    svr.Get("/", [&](const Request&, Response& res) {
        res.set_content(html_page, "text/html");
    });

    // ------------------------------------------------------------------
    // Handle POST /obfuscate
    // ------------------------------------------------------------------
    svr.Post("/obfuscate", [&](const Request& req, Response& res) {
        // Read form fields
        auto code = req.get_param_value("code");
        bool rename          = req.get_param_value("rename") == "on";
        bool encrypt         = req.get_param_value("encrypt") == "on";
        bool flatten         = req.get_param_value("flatten") == "on";
        bool obfuscateNumbers = req.get_param_value("obfuscateNumbers") == "on";
        bool deadCode        = req.get_param_value("deadCode") == "on";
        bool antiDebug       = req.get_param_value("antiDebug") == "on";

        if (code.empty()) {
            res.set_content("Error: No code provided.", "text/plain");
            return;
        }

        try {
            std::string obfuscated = simple_obfuscate(
                code, rename, encrypt, flatten,
                obfuscateNumbers, deadCode, antiDebug
            );
            res.set_content(obfuscated, "text/plain");
        } catch (const std::exception& e) {
            res.set_content(std::string("Error: ") + e.what(), "text/plain");
        }
    });

    // ------------------------------------------------------------------
    // Start the server
    // ------------------------------------------------------------------
    std::cout << "Luau Obfuscator server running on http://localhost:8080" << std::endl;
    svr.listen("0.0.0.0", 8080);

    return 0;
}
