# KV Store — Project Notes

Goal of this project: learn C++, low-level systems programming, and system design. Not aiming for production-grade — aiming to understand every piece.

## Decisions made so far

- **Protocol:** raw TCP, custom line-based text protocol (not HTTP, not a binary/length-prefixed format — that's a possible future direction, not v1).
- **Concurrency (v1):** single-threaded, sequential — one client at a time. `accept()` a client, serve them fully, `close()`, loop back to `accept()` for the next one. No threads, no mutex yet.
- **Build tool:** CMake (not plain g++, not Makefile) — not wired up yet, still compiling by hand with `g++ ... -I./include ...`.
- **Layout:** `include/` for headers (interfaces), `src/` for `.cpp` implementation/entry points, `builds/` for compiled output (gitignored).

## v1 scope

**In:**
- Commands: `PUT key value`, `GET key`, `DEL key`, `EXISTS key`, `QUIT`
- Server serves clients one at a time, sequentially
- In-memory only (no persistence — data lost on restart)
- Hardcoded port (9090), no auth, no config

**Explicitly out (future versions):**
- Persistence (WAL / snapshotting)
- TTL / eviction
- Concurrency beyond sequential (thread-per-connection + mutex was the plan when we get there)
- Real client library (using `nc` as the client for now)
- Config, logging, metrics, auth

## Current state of the code

- **`include/KVStore.hpp`** — thread-unsafe (no mutex yet, not needed until concurrency work) in-memory KV class: `put`, `get` (throws `std::runtime_error` if missing), `delete_key`, `exists`. Done and working.
- **`src/main.cpp`** — one-shot demo of `KVStore`, no networking. Done and working.
- **`src/server.cpp`** — TCP server. Currently:
  - `socket()` → `setsockopt(SO_REUSEADDR)` → `bind()` → `listen()` — all done once, correctly, **before** the loop (this was a real bug we hit and fixed: originally `bind`/`listen` were mistakenly inside the loop, and `close(serverFd)` was being called every iteration instead of `close(clientFd)` — killed the listening socket after the first client).
  - `while(true) { accept(); recv() ONCE; close(clientFd); }` — loops correctly across multiple clients now.
  - **Not yet done:** the `recv()` inside the loop only reads once per client instead of looping until disconnect, so a client can currently only send one message before getting disconnected. No line buffering yet. Not wired to `KVStore` at all yet — just prints raw bytes.

## Plan for next session, in order

1. **Inner `recv()` loop + line buffering** in `server.cpp`:
   - Loop `recv()` for a client until it returns `<= 0` (disconnect/error), instead of just once.
   - Accumulate received bytes into a `std::string` (declared per-client, before the inner loop).
   - After each `recv()`, pull out every complete line currently in the buffer (`.find('\n')`, `.substr()`, `.erase()`) — a single `recv()` can contain 0, 1, or multiple lines.
   - Test: `nc localhost 9090`, send several lines in one session, confirm the server prints each one separately.

2. **Wire in `KVStore`:**
   - `#include "KVStore.hpp"` in `server.cpp`.
   - One `KVStore` instance shared across the whole server’s lifetime (declared before the `accept` loop).
   - Still just printing lines at this point — just prove it compiles and is reachable.

3. **Command parsing + dispatch:**
   - For each line extracted in step 1, split into command + args (`std::istringstream`).
   - Dispatch `PUT`/`GET`/`DEL`/`EXISTS`/`QUIT` to the matching `KVStore` method.
   - `send()` a response back over `clientFd` instead of printing.
   - Test each command manually via `nc`.

4. **CMake:**
   - Minimal `CMakeLists.txt`: project + C++ standard + two executables (`main`, `server`) + `include/` on the header search path (`target_include_directories`).
   - Replace manual `g++ -I./include ...` with `cmake -B build && cmake --build build`.

## Known gotchas hit so far (don't re-learn these the hard way)

- `std::cout << std::cout << x` doesn't compile — can't insert an `ostream` into itself.
- `#include "X.hpp"` (quotes) searches relative to the including file's dir first, then falls back to `-I` paths — need `-I./include` when headers live outside `src/`.
- Header files need `#pragma once` to avoid redefinition errors once more than one `.cpp` includes them.
- `bind()` failing right after a restart is usually `TIME_WAIT` on the port from the previous run — fixed with `setsockopt(SO_REUSEADDR)` *before* `bind()`.
- `bind()`/`listen()` must run once, outside the accept loop — only `accept()`/per-client handling repeats. Closing `serverFd` instead of `clientFd` inside the loop kills the listening socket after one client.
- Ctrl+C in a terminal running `nc` doesn't send the byte `0x03` over the socket — it's intercepted locally as `SIGINT` and just kills `nc`, which the server sees as a normal disconnect (`recv()` returns `0`).
