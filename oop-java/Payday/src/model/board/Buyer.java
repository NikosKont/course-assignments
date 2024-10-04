package model.board;

import model.player.Player;

/**
 * A Buyer position.
 */
public class Buyer extends Tile {
    /**
     * Constructor for a new Buyer position.
     * 
     * @pre 1 {@literal <}{@literal =} position {@literal <}{@literal =} 30
     * 
     * @param position The position of the tile on the board.
     * @param day The day of the tile.
     * @param image The path for the image of the tile.
     */
    public Buyer(int position, String day, String image) {
        super(position, day, image);
    }

    @Override
    /**
     * Performs the default action; player sells deal card.
     */
    public void performAction(Player player) {
    }
}