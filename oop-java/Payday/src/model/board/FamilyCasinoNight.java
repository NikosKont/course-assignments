package model.board;

import model.player.Player;

/**
 * A Family Casino Night position.
 */
public class FamilyCasinoNight extends Tile {
    /**
     * Constructor for a new Family Casino Night position.
     *
     * @pre 1 {@literal <}{@literal =} position {@literal <}{@literal =} 30
     *
     * @param position The position of the tile on the board.
     * @param day The day of the tile.
     * @param image The path for the image of the tile.
     */
    public FamilyCasinoNight(int position, String day, String image) {
        super(position, day, image);
    }

    @Override
    /**
     * Performs the default action; Family Casino Night takes place.
     */
    public void performAction(Player player) {
        if (player.getDie() % 2 == 0) {
            if (Jackpot.getValue() >= 500) {
                player.addMoney(500);
                Jackpot.remove(500);
            }
            else {
                player.addMoney(Jackpot.getValue());
                Jackpot.reset();
            }

        }
        else {
            player.pay(500);
            Jackpot.add(500);
        }
    }
}