#include <criterion/criterion.h>
#include <criterion/redirect.h>
#include "chessboard/hashtables/hash_table.h"
#include "chessboard/chessboard.h"

void setup_hash(void)
{
    init_zobrist();
}

TestSuite(hash, .init = setup_hash);

// ==================== Tests Zobrist Initialization ====================

Test(hash, zobrist_initialization)
{
    printf("TEST HASHTABLES ...\n");
    fflush(stdout);

    cr_assert_neq(zobrist_side_to_move, 0, "zobrist_side_to_move should not be 0");
    cr_assert_neq(zobrist_pieces[0][0], zobrist_pieces[1][0],
                  "Different pieces should have different hashes");

    cr_assert_neq(zobrist_pieces[0][0], zobrist_pieces[0][1],
                  "Same piece on different squares should have different hashes");
}

Test(hash, zobrist_uniqueness)
{
    // Vérifier qu'il n'y a pas trop de collisions (test probabiliste)
    int collision_count = 0;
    for (int i = 0; i < 12; i++)
    {
        for (int j = 0; j < 64; j++)
        {
            for (int k = i; k < 12; k++)
            {
                int start = (k == i) ? j + 1 : 0;
                for (int l = start; l < 64; l++)
                {
                    if (zobrist_pieces[i][j] == zobrist_pieces[k][l])
                    {
                        collision_count++;
                    }
                }
            }
        }
    }
    cr_assert_eq(collision_count, 0, "Should have no collisions in zobrist tables");
}

// ==================== Tests Hash Computation ====================

Test(hash, starting_position_hash)
{
    Chessboard board;
    init_chessboard(&board);

    ZobristKey hash = compute_hash(&board);
    cr_assert_neq(hash, 0, "Starting position hash should not be 0");
}

Test(hash, same_position_same_hash)
{
    Chessboard board1, board2;
    init_chessboard(&board1);
    init_chessboard(&board2);

    ZobristKey hash1 = compute_hash(&board1);
    ZobristKey hash2 = compute_hash(&board2);

    cr_assert_eq(hash1, hash2, "Same positions should have the same hash");
}

Test(hash, different_positions_different_hash)
{
    Chessboard board1, board2;
    init_chessboard(&board1);

    init_chessboard_from_fen(&board2, "rnbqkbnr/ppp1pppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

    ZobristKey hash1 = compute_hash(&board1);
    ZobristKey hash2 = compute_hash(&board2);

    cr_assert_neq(hash1, hash2, "Different positions should have different hashes");
}

Test(hash, side_to_move_affects_hash)
{
    Chessboard board1, board2;
    init_chessboard_from_fen(&board1, "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    init_chessboard_from_fen(&board2, "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR b KQkq - 0 1");

    ZobristKey hash1 = compute_hash(&board1);
    ZobristKey hash2 = compute_hash(&board2);

    cr_assert_neq(hash1, hash2, "Side to move should affect the hash");
}

Test(hash, castling_rights_affect_hash)
{
    Chessboard board1, board2;
    init_chessboard_from_fen(&board1, "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    init_chessboard_from_fen(&board2, "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQ - 0 1");

    ZobristKey hash1 = compute_hash(&board1);
    ZobristKey hash2 = compute_hash(&board2);

    cr_assert_neq(hash1, hash2, "Castling rights should affect the hash");
}

Test(hash, en_passant_affects_hash)
{
    Chessboard board1, board2;
    init_chessboard_from_fen(&board1, "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1");
    init_chessboard_from_fen(&board2, "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq - 0 1");

    ZobristKey hash1 = compute_hash(&board1);
    ZobristKey hash2 = compute_hash(&board2);

    cr_assert_neq(hash1, hash2, "En passant square should affect the hash");
}

// ==================== Tests Repetition Table ====================

Test(hash, repetition_table_initialization)
{
    RepetitionTable table;
    init_repetition_table(&table);

    cr_assert_eq(table.history_count, 0, "History count should be 0 after init");

    // Vérifier que toutes les entrées sont à 0
    for (int i = 0; i < HASH_TABLE_SIZE; i++)
    {
        cr_assert_eq(table.entries[i].count, 0, "All counts should be 0 after init");
    }
}

Test(hash, add_position_once)
{
    RepetitionTable table;
    init_repetition_table(&table);

    Chessboard board;
    init_chessboard(&board);
    ZobristKey hash = compute_hash(&board);

    add_position(&table, hash);

    cr_assert_eq(table.history_count, 1, "History count should be 1");
    cr_assert(!is_threefold_repetition(&table, hash), "Should not be threefold after 1 occurrence");
}

Test(hash, add_position_twice)
{
    RepetitionTable table;
    init_repetition_table(&table);

    Chessboard board;
    init_chessboard(&board);
    ZobristKey hash = compute_hash(&board);

    add_position(&table, hash);
    add_position(&table, hash);

    cr_assert_eq(table.history_count, 2, "History count should be 2");
    cr_assert(!is_threefold_repetition(&table, hash), "Should not be threefold after 2 occurrences");
}

Test(hash, threefold_repetition_detected)
{
    RepetitionTable table;
    init_repetition_table(&table);

    Chessboard board;
    init_chessboard(&board);
    ZobristKey hash = compute_hash(&board);

    add_position(&table, hash);
    add_position(&table, hash);
    add_position(&table, hash);

    cr_assert_eq(table.history_count, 3, "History count should be 3");
    cr_assert(is_threefold_repetition(&table, hash), "Should detect threefold repetition");
}

Test(hash, pop_position_works)
{
    RepetitionTable table;
    init_repetition_table(&table);

    Chessboard board;
    init_chessboard(&board);
    ZobristKey hash = compute_hash(&board);

    add_position(&table, hash);
    add_position(&table, hash);
    add_position(&table, hash);

    cr_assert(is_threefold_repetition(&table, hash), "Should have threefold");

    pop_position(&table);

    cr_assert_eq(table.history_count, 2, "History count should be 2 after pop");
    cr_assert(!is_threefold_repetition(&table, hash), "Should not have threefold after pop");
}

Test(hash, multiple_positions_tracking)
{
    RepetitionTable table;
    init_repetition_table(&table);

    Chessboard board1, board2;
    init_chessboard(&board1);
    init_chessboard_from_fen(&board2, "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1");

    ZobristKey hash1 = compute_hash(&board1);
    ZobristKey hash2 = compute_hash(&board2);

    // Ajouter alternativement les deux positions
    add_position(&table, hash1);
    add_position(&table, hash2);
    add_position(&table, hash1);
    add_position(&table, hash2);
    add_position(&table, hash1);

    cr_assert(is_threefold_repetition(&table, hash1), "hash1 should have threefold");
    cr_assert(!is_threefold_repetition(&table, hash2), "hash2 should not have threefold");
}

Test(hash, clear_repetition_table)
{
    RepetitionTable table;
    init_repetition_table(&table);

    Chessboard board;
    init_chessboard(&board);
    ZobristKey hash = compute_hash(&board);

    add_position(&table, hash);
    add_position(&table, hash);
    add_position(&table, hash);

    clear_repetition_table(&table);

    cr_assert_eq(table.history_count, 0, "History count should be 0 after clear");
    cr_assert(!is_threefold_repetition(&table, hash), "Should not detect repetition after clear");
}

Test(hash, remove_position_works)
{
    RepetitionTable table;
    init_repetition_table(&table);

    Chessboard board;
    init_chessboard(&board);
    ZobristKey hash = compute_hash(&board);

    add_position(&table, hash);
    add_position(&table, hash);

    remove_position(&table, hash);

    cr_assert(!is_threefold_repetition(&table, hash), "Count should decrease after remove");
}

Test(hash, large_history_test)
{
    RepetitionTable table;
    init_repetition_table(&table);

    Chessboard board;
    init_chessboard(&board);
    ZobristKey hash = compute_hash(&board);

    // Ajouter beaucoup de positions
    for (int i = 0; i < 100; i++)
    {
        add_position(&table, hash);
    }

    cr_assert(is_threefold_repetition(&table, hash), "Should still detect threefold with many positions");
    cr_assert_leq(table.history_count, 512, "Should not exceed history limit");
}

// ==================== Tests de collision ====================

Test(hash, linear_probing_collision_handling)
{
    RepetitionTable table;
    init_repetition_table(&table);

    // Créer deux hashs qui auront le même index initial
    ZobristKey hash1 = HASH_TABLE_SIZE * 2;
    ZobristKey hash2 = HASH_TABLE_SIZE * 3;

    add_position(&table, hash1);
    add_position(&table, hash2);
    add_position(&table, hash1);
    add_position(&table, hash2);
    add_position(&table, hash1);

    cr_assert(is_threefold_repetition(&table, hash1), "Should handle collisions correctly for hash1");
    cr_assert(!is_threefold_repetition(&table, hash2), "Should handle collisions correctly for hash2");
}