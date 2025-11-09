# Legal Move Generation

## Overview

This chess engine implements a highly optimized legal move generation system, achieving performance of up to **80 million moves per second** during perft testing. The implementation leverages modern optimization techniques including bitboards, magic bitboards, efficient data structures, and multi-threading.

## Core Techniques

### Bitboards

The engine uses a bitboard representation where each piece type and color is represented as a 64-bit integer, with each bit corresponding to a square on the board. This approach offers several advantages:

- **Parallel operations**: Multiple squares can be evaluated simultaneously using bitwise operations
- **Memory efficiency**: The entire board state fits in CPU cache
- **Fast move generation**: Bitwise operations (AND, OR, XOR, shifts) are extremely fast at the hardware level

### Magic Bitboards

Magic bitboards are used for sliding piece move generation (bishops, rooks, queens). This technique uses pre-computed lookup tables with magic numbers to instantly retrieve possible moves:

1. **Occupancy masking**: Relevant occupied squares are identified
2. **Magic multiplication**: A magic number transforms the occupancy into a unique hash
3. **Table lookup**: The hash indexes into a pre-computed move table

This eliminates the need for iterative ray-casting and provides O(1) move generation for sliding pieces.

### Compact Move Structures

Moves are represented using minimal memory to maximize cache efficiency:

- Compact bit-packed structures store source square, destination square, move type, and flags
- Smaller move structures mean more moves fit in cache, reducing memory latency
- Move lists can be processed more efficiently due to better spatial locality

### Multi-threading

The move generation system leverages parallel processing for performance-critical operations:

- Perft calculations are parallelized across multiple threads
- Work is distributed efficiently to maximize CPU utilization
- Thread-safe data structures ensure correctness in concurrent scenarios

## Performance Testing with Perft

### What is Perft?

Perft (performance test) is a standard debugging and benchmarking function in chess programming. It counts all leaf nodes at a given depth in the game tree, serving as both a correctness test and a performance benchmark.

### Testing Methodology

The engine's move generation was validated and benchmarked using perft tests:

1. **Correctness verification**: Results were compared against known perft values from standard positions
2. **Performance measurement**: Nodes per second (NPS) was measured across various depths
3. **Regression testing**: Perft suites ensure modifications don't introduce bugs

### Performance Results

The implementation achieves **80 million moves per second** during perft testing, demonstrating:

- Efficient move generation algorithms
- Effective use of CPU cache through compact data structures
- Successful parallelization across multiple cores
- Minimal branching and optimal instruction pipelining

### Standard Test Positions

Common perft positions used for validation include:

- **Starting position**: `rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1`
- **Kiwipete**: A complex middlegame position testing en passant, castling, and promotions
- **Position 3-6**: Additional positions from the perft suite covering edge cases

👉 [Perft results reference](https://www.chessprogramming.org/Perft_Results)

## Implementation Highlights

### Move Generation Pipeline

1. **Bitboard initialization**: Board state is maintained as bitboards
2. **Attack generation**: Magic bitboards generate sliding piece attacks
3. **Legal move filtering**: Moves are validated against check and pin constraints
4. **Move encoding**: Legal moves are packed into compact structures
5. **Move ordering**: Moves are sorted for optimal alpha-beta pruning (when used in search)

### Optimization Techniques Applied

- **Pre-computed lookup tables**: Attack patterns, magic numbers, and ray tables
- **Bit manipulation intrinsics**: Use of hardware instructions (POPCNT, TZCNT, etc.)
- **Branch prediction optimization**: Code structured to minimize mispredictions
- **Memory alignment**: Data structures aligned for optimal CPU access
- **Copy-make approach**: Efficient board state copying for recursive search

## Future Improvements

Potential areas for further optimization:

- SIMD instructions for parallel bitboard operations
- GPU acceleration for bulk perft calculations
- Advanced prefetching strategies
- Further tuning of magic bitboard tables

## Conclusion

The combination of bitboards, magic bitboards, compact data structures, and multi-threading results in a move generation system that is both fast and correct. The 80 million NPS achieved during perft testing demonstrates that the implementation is competitive with modern chess engines and provides a solid foundation for the complete chess engine.
