/*
 * Marmalade - Lightweight Game Engine
 * Copyright (C) 2025 Tayler Parsons
 * Copyright (C) 2025 Ryan Bester
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

const fs = require("fs");
const {exec} = require("child_process");
const http = require("http");
const path = require("path");

const WATCH_DIRS = [path.resolve(__dirname, "../src"), path.resolve(__dirname, "../include"), path.resolve(__dirname, "../misc/docs")];
const DOCS_DIR = path.resolve(__dirname, "../doxydocs/html");
const PORT = 8000;
const DELAY = 5000;

let timeout;

let clients = new Set();

function runDoxygen() {
    console.log("Regenerating documentation...");
    exec("doxygen Doxyfile", {
        'cwd': '../'
    }, (error, stdout, stderr) => {
        if (error) console.error(`Error: ${error.message}`);
        if (stderr) console.error(`stderr: ${stderr}`);
        console.log(stdout || "Documentation updated!");
        notifyClients();
    });
}

function notifyClients() {
    console.log("reloading clients");
    for (const res of clients) {
        res.write("data: reload\n\n");
    }
}

function getContentType(filePath) {
    const ext = path.extname(filePath).toLowerCase();
    const mimeTypes = {
        ".html": "text/html",
        ".css": "text/css",
        ".js": "application/javascript",
        ".png": "image/png",
        ".jpg": "image/jpeg",
        ".gif": "image/gif",
        ".svg": "image/svg+xml",
        ".json": "application/json",
    };
    return mimeTypes[ext] || "application/octet-stream";
}

// ----- MAIN

runDoxygen();

WATCH_DIRS.forEach(watch_dir => {
    fs.watch(watch_dir, {recursive: true}, (eventType, filename) => {
        if (filename && (filename.endsWith(".cpp") || filename.endsWith(".h") || filename.endsWith(".dox"))) {
            console.log(`File changed: ${filename}`);
            clearTimeout(timeout);
            timeout = setTimeout(runDoxygen, DELAY);
        }
    });
});

console.log(`Watching for changes in: ${WATCH_DIRS}`);

const server = http.createServer((req, res) => {
    let filePath = path.join(DOCS_DIR, req.url === "/" ? "index.html" : req.url);


    if (req.url === "/subscribe") {
        res.writeHead(200, {
            "Content-Type": "text/event-stream",
            "Cache-Control": "no-cache",
            "Connection": "keep-alive",
            "Access-Control-Allow-Origin": "*",
        });

        res.write(": connected\n\n");
        console.log("client subscribed");
        clients.add(res);

        req.on("close", () => {
            clients.delete(res);
        });

        return;
    }

    fs.readFile(filePath, "utf8", (err, data) => {
        if (err) {
            res.writeHead(404, {"Content-Type": "text/plain"});
            res.end("404 Not Found");
        } else {
            let contentType = getContentType(filePath);
            res.writeHead(200, {"Content-Type": contentType});

            // Inject auto-reload script
            if (contentType === "text/html") {
                const injectedScript = `
<script>
  let evtSource = new EventSource("/subscribe");
  evtSource.onmessage = function () { location.reload(); };
</script>
</body>`;
                const modified = data.replace("</body>", injectedScript);
                res.end(modified);
            } else {
                res.end(data);
            }
        }
    });
});

server.listen(PORT, () => {
    console.log(`Serving documentation at http://localhost:${PORT}/`);
});
