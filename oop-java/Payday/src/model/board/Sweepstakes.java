package model.board;

import model.player.Player;

/**
 * A Sweepstakes position.
 */
public class Sweepstakes extends Tile {
    /**
     * Constructor for a new Sweepstakes position.
     * 
     * @pre 1 {@literal <}{@literal =} position {@literal <}{@literal =} 30
     * 
     * @param position The position of the tile on the board.
     * @param day The day of the tile.
     * @param image The path for the image of the tile.
     */
    public Sweepstakes(int position, String day, String image) {
        super(position, day, image);
    }

    @Override
    /**
     * Performs the default action; player receives 1000* their roll.
     */
    public void performAction(Player player) {
    }
}
