# Chess Engine

A high-performance chess engine written in C, featuring an advanced move generation system, graphical interface, and bot API for automated gameplay.

## Features

### 🚀 High-Performance Move Generation
- **Bitboard representation** for efficient board state management
- **Magic bitboards** for O(1) sliding piece move generation
- **Multi-threaded perft testing** achieving up to 80 million moves per second
- Compact move structures optimized for cache efficiency

### 🎮 Graphical Interface
- Interactive chess board with visual piece movement
- Human vs Bot gameplay
- Bot vs Bot gameplay

### 🤖 Bot API
- bot communication
- Support for bot vs bot matches
- Standardized move format and game state representation
- Easy integration for custom chess engines

### 🧠 Chess Engine
- Legal move generation with full rule support
- Evaluation function for position assessment
- Search algorithms for move selection
- Support for standard chess notation

## Getting Started

### Prerequisites
- GCC or compatible C compiler
- Make build system
- POSIX threads support (for multi-threading)
- SDL2 for ui

### Building the Project

```bash
# Clone the repository
git clone [https://github.com/TomBaran501/chessboardEngine.git]
cd [chessboardEngine]

# Build and run the project
make run
```

## Documentation

Comprehensive documentation is available in the `documentation/` folder:

- **[Move Generation](documentations/legal_move_generation.md)** - Technical details on the legal move generation system, optimization techniques, and performance benchmarks
- **[Bot API](documentations/bot_api.md)** - API specification for developing and connecting chess bots
- **[Main Commands](documentations/commands_main.md)** - Command-line interface reference and usage guide

## Usage

### Playing Against the Bot

Launch the graphical interface and play against the built-in chess engine. Use the documented commands to control the game flow.

### Running Bot vs Bot Matches

Start the API server and connect your bots to compete against each other. See the [Bot API documentation](documentation/bot_api.md) for integration details.

### Performance Testing

Run perft tests to verify move generation correctness and measure performance:

```bash
./chess 
go perft [depth]
```

## Project Structure

```
.
├── src/                  # Source code files
├── lib/                  # Header files
├── documentation/        # Project documentation
├── tests/                # Test suites
├── bots/                 # Test suites
├── Makefile              # Build configuration
└── README.md             # This file


## Performance Benchmarks

- **Move generation**: Up to 80 million moves/second (perft)
- **Evaluation speed**: Between 5 millions and 40 millions positions evaluated per second

## Technical Highlights

- Efficient bitboard-based board representation
- Magic bitboard move generation for sliding pieces
- Multi-threaded perft calculations
- Cache-optimized data structures
- Minimal memory footprint per move


## License

MIT License

Copyright (c) 2025 Baran Tom

## Acknowledgments

- Magic bitboard implementation inspired by [Chess Programming Wiki](https://www.chessprogramming.org/)
- Perft positions from standard test suites


**Note**: This is an educational/hobby project demonstrating advanced techniques in chess programming and game engine development.