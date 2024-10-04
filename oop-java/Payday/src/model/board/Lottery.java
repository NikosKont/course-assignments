package model.board;

import model.player.Player;

/**
 * A Lottery position.
 */
public class Lottery extends Tile {
    /**
     * Constructor for a new Lottery position.
     *
     * @pre 1 {@literal <}{@literal =} position {@literal <}{@literal =} 30
     *
     * @param position The position of the tile on the board.
     * @param day The day of the tile.
     * @param image The path for the image of the tile.
     */
    public Lottery(int position, String day, String image) {
        super(position, day, image);
    }

    @Override
    /**
     * Performs the default action; lottery takes place.
     */
    public void performAction(Player player) {
        if (player.getDie() > player.getOpponent().getDie()) {
            player.addMoney(player.getOpponent().getDie() * 10);
            player.getOpponent().pay(player.getOpponent().getDie() * 10);
        }
        else {
            player.getOpponent().addMoney(player.getDie() * 10);
            player.pay(player.getDie() * 10);
        }
    }
}