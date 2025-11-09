
# Bot API Documentation

## Overview
The Bot API provides an interface for communication between the main chess engine and external bot executables.  
It allows connecting, exchanging moves, and maintaining the internal state of the bot through a lightweight process-based protocol.

---

## Data Structures

### `BotConnector`
Holds all necessary information for managing the bot process.

| Field | Type | Description |
|-------|------|--------------|
| `FILE *bot_in` | Input stream used to send data to the bot (stdin). |
| `FILE *bot_out` | Output stream used to read responses from the bot (stdout). |
| `pid_t pid` | Process ID of the running bot instance. |

---

## Functions

### `int bot_connect(BotConnector *bot, const char *bot_path)`
Initializes a connection to the bot executable.

**Parameters:**
- `bot`: Pointer to a `BotConnector` structure to initialize.
- `bot_path`: Path to the bot executable file.

**Returns:**
- `0` on success.
- `-1` if the bot process could not be started or the pipes failed.

---

### `int is_bot_connected(BotConnector *bot)`
Checks if the bot is currently connected.

**Parameters:**
- `bot`: Pointer to a `BotConnector` structure.

**Returns:**
- `1` if the bot is connected and the file streams are valid.
- `0` otherwise.

---

### `int bot_set_fen(BotConnector *bot, const char *fen)`
Sets the board state (FEN position) in the bot’s internal memory.

**Parameters:**
- `bot`: Active bot connection.
- `fen`: FEN string representing the current board state.

**Returns:**
- `0` on success.
- `-1` if the bot is not connected.

---

### `int bot_get_best_move(BotConnector *bot, char *move_str)`
Requests the bot’s best move for the current position.

**Parameters:**
- `bot`: Active bot connection.
- `move_str`: Output buffer to store the move (e.g., `"e2e4"`).

**Returns:**
- `0` if a valid move was received.
- `-1` if there was an error or timeout.

---

### `int bot_play_move(BotConnector *bot, const char *move_str)`
Informs the bot that a move was played so it can update its internal state.

**Parameters:**
- `bot`: Active bot connection.
- `move_str`: Move string (e.g., `"e2e4"`).

**Returns:**
- `0` on success.
- `-1` if the bot is not connected.

---

### `void bot_disconnect(BotConnector *bot)`
Terminates the connection and cleans up the associated resources.

**Parameters:**
- `bot`: Pointer to the bot connection to close.

**Behavior:**
- Closes input/output streams.
- Waits for the bot process to terminate.
- Resets the `BotConnector` fields to safe defaults.

---

## Communication Protocol

Commands sent to the bot:
- `"fen <FEN_STRING>"` — sets the current board position. No response
- `"get move"` — requests the best move from the bot. Responds with the best move
- `"move <MOVE>"` — applies a move to the internal state. No response
- `"quit"` — terminates the bot process. No response

The bot responds on its stdout

---

## Error Handling
All functions return `-1` in case of communication or process errors.  
Timeouts are handled internally in `bot_get_best_move` to prevent infinite blocking.

---
