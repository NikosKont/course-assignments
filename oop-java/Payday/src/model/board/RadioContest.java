package model.board;

import model.player.Player;

/**
 * A Radio Contest position.
 */
public class RadioContest extends Tile {
    /**
     * Constructor for a new Radio Contest position.
     * 
     * @pre 1 {@literal <}{@literal =} position {@literal <}{@literal =} 30
     * 
     * @param position The position of the tile on the board.
     * @param day The day of the tile.
     * @param image The path for the image of the tile.
     */
    public RadioContest(int position, String day, String image) {
        super(position, day, image);
    }

    @Override
    /**
     * Performs the default action; radio contest takes place.
     */
    public void performAction(Player player) {
        // TODO: implement
    }
}
