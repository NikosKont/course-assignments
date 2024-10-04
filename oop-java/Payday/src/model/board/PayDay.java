package model.board;

import model.player.Player;

/**
 * Payday position.
 */
public class PayDay extends Tile {
    /**
     * Constructor for a new PayDay position.
     *
     * @param image The path for the image of the tile.
     */
    public PayDay(String image) {
        super(31, "Wednesday", image);
    }

    @Override
    /**
     * Performs the default action; player is paid and pays off his bills.
     */
    public void performAction(Player player) {
        player.addMoney(3500);
        player.payBills(player.readBills());
        player.pay(((int) (0.1 * player.readLoan())));
    }
}