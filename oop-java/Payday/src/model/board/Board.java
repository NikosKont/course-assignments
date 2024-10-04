package model.board;
/**
 * The main board of the game.
 */
public class Board {
    private Tile[] board;

    /**
     * Constructs the board with the given tiles.
     *
     * @pre The given tiles must be valid according to the rules of the game.
     *
     * @param board an array of tiles
     */
    public Board(Tile[] board) {
        this.board = board;
    }

    /**
     * Gets the tile at the given position.
     *
     * @pre 0 {@literal <}{@literal =} position {@literal <} 32
     *
     * @param position the position of desired tile
     *
     * @return the tile at the given position
     */
    public Tile getTile(int position) {
        return board[position];
    }
}