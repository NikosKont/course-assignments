package model.board;

import model.player.Player;

/**
 * A Yard Sale position.
 */
public class YardSale extends Tile {
    /**
     * Constructor for a new Yard Sale position.
     *
     * @pre 1 {@literal <}{@literal =} position {@literal <}{@literal =} 30
     *
     * @param position The position of the tile on the board.
     * @param day The day of the tile.
     * @param image The path for the image of the tile.
     */
    public YardSale(int position, String day, String image) {
        super(position, day, image);
    }

    @Override
    /**
     * Performs the default action; player pays 100* roll and draws deal card.
     */
    public void performAction(Player player) {
        player.pay(player.getDie());
    }
}