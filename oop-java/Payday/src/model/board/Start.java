package model.board;

import model.player.Player;

/**
 * Start position.
 */
public class Start extends Tile {
    /**
     * Constructor for a new Start position.
     * 
     * @param image The path for the image of the tile.
     */
    public Start (String image) {
        super(0, "None", image);
    }

    @Override
    /**
     * Performs the default action; player starts.
     */
    public void performAction(Player player) {
        // TODO: implement
    }
}
