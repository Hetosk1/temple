# Wire Protocol (v1)

Raw TCP, line-based text protocol. A "packet" is just one line terminated by `\n`. There is no header and no length prefix. TCP is a byte stream, not a message stream, so a single `recv()` can contain zero, one, or many lines, or half a line — the server must buffer bytes per-client and split on `\n` before parsing anything.

## Requests (client → server)

| Command | Format | Example | Notes |
|---|---|---|---|
| PUT | `PUT <key> <value>\n` | `PUT het backend\n` | `<value>` is everything after the second space, up to `\n` — it **can contain spaces**. Split on the first two spaces only. |
| GET | `GET <key>\n` | `GET het\n` | `<key>` is a single token — split on the first space only. |
| DEL | `DEL <key>\n` | `DEL het\n` | Same as GET. |
| EXISTS | `EXISTS <key>\n` | `EXISTS het\n` | Same as GET. |
| QUIT | `QUIT\n` | `QUIT\n` | No arguments. Handled before dispatch — closes the connection, never touches `KVStore`. |

## Responses (server → client)

| Command | Success | Failure |
|---|---|---|
| PUT | `OK\n` | — (cannot fail) |
| GET | `VALUE <value>\n` | `ERR not found\n` (catch `KVStore::get`'s `std::runtime_error`) |
| DEL | `OK\n` | `ERR not found\n` (`delete_key` returned `false`) |
| EXISTS | `TRUE\n` | `FALSE\n` (never an error — always one or the other) |
| QUIT | *(socket closed, no response line sent)* | — |

## Parsing rules

1. Split each line on **whitespace**, but only as many times as the command needs: `PUT` takes 2 splits (command, key, rest-of-line-as-value); `GET`/`DEL`/`EXISTS` take 1 split (command, key).
2. `GET` on a missing key must be caught with `try/catch` around `KVStore::get` and turned into `ERR not found\n` — never let it propagate and crash the server.
3. Unknown command word → `ERR unknown command\n` (not yet decided if this is in scope for v1, but the parser needs *some* default case).
