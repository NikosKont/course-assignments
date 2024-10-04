package model.board;

import model.player.Player;

/**
 * A Deal position.
 */
public class Deal extends Tile {
    /**
     * Constructor for a new Deal position.
     *
     * @pre 1 {@literal <}{@literal =} position {@literal <}{@literal =} 30
     *
     * @param position The position of the tile on the board.
     * @param day The day of the tile.
     * @param image The path for the image of the tile.
     */
    public Deal(int position, String day, String image) {
        super(position, day, image);
    }

    @Override
    /**
     * Performs the default action; player draws deal card.
     */
    public void performAction(Player player) {
    }
}