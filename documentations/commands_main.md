# 🧩 Chess Engine Command List

This document describes the main commands available to interact with the chess engine via the command line.

---

## ⚙️ Available Commands

### `position fen 'FEN_CODE'`
Loads a specific position from a **FEN** string.  
Use this command to initialize the board with a custom position.

**Example:**
```bash
position fen rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1
```

---

### `position startpos`
Loads the **standard starting position** of a chess game.
This position is loaded by default.

**Example:**
```bash
position startpos
```

---

### `go perft N`
Performs a **performance test (perft)** up to depth `N`.  
This is used to validate and benchmark move generation accuracy and speed.

**Example:**
```bash
go perft 5
```

---

### `go play`
Starts an **automatic game** between two players (human or AI depending on configuration).  
The engine alternates turns automatically.

**Example:**
```bash
go play
```

---

### `go play ui -c`
Starts a **game against the AI with a user interface**.  
The `-c` flag specifies which color the AI will play:  
- `-c w` → AI plays **white**  
- `-c b` → AI plays **black**

**Examples:**
```bash
go play ui -c w   # AI plays white
go play ui -c b   # AI plays black
```

---

## 💡 Notes
- Commands must be entered in lowercase.  
- The engine communicates through standard input/output (`stdin`/`stdout`).  
- `position` commands must always precede `go` commands.
