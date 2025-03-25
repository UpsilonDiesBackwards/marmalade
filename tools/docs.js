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
const { exec } = require("child_process");
const http = require("http");
const path = require("path");

const WATCH_DIR = path.resolve(__dirname, "../src");
const DOCS_DIR = path.resolve(__dirname, "../doxydocs/html");
const PORT = 8000;
const DELAY = 5000;

let timeout;

function runDoxygen() {
    console.log("Regenerating documentation...");
    exec("doxygen Doxyfile", (error, stdout, stderr) => {
        if (error) console.error(`Error: ${error.message}`);
        if (stderr) console.error(`stderr: ${stderr}`);
        console.log(stdout || "Documentation updated!");
    });
}

fs.watch(WATCH_DIR, { recursive: true }, (eventType, filename) => {
    if (filename && (filename.endsWith(".cpp") || filename.endsWith(".h"))) {
        console.log(`File changed: ${filename}`);
        clearTimeout(timeout);
        timeout = setTimeout(runDoxygen, DELAY);
    }
});

console.log(`Watching for changes in: ${WATCH_DIR}`);

const server = http.createServer((req, res) => {
    let filePath = path.join(DOCS_DIR, req.url === "/" ? "index.html" : req.url);

    fs.readFile(filePath, (err, data) => {
        if (err) {
            res.writeHead(404, { "Content-Type": "text/plain" });
            res.end("404 Not Found");
        } else {
            res.writeHead(200, { "Content-Type": getContentType(filePath) });
            res.end(data);
        }
    });
});

server.listen(PORT, () => {
    console.log(`Serving documentation at http://localhost:${PORT}/`);
});

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
