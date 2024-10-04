package model.cards;

import model.board.Board;
import model.board.Tile;

/**
 * Move to Deal/Buyer mail card.
 */
public class MoveToDealBuyer extends MailCard {
    /**
     * Constructs a new Move to Deal/Buyer mail card.
     *
     * @param image the image path of the card
     * @param value the value of the card
     * @param message the message of the card
     * @param choice the choice of the card
     */
    public MoveToDealBuyer (String image, int value, String message, String choice) {
        super(image, value, message, choice);
    }

    @Override
    /**
     * Executes the card's action; player moves to the nearest Deal or Buyer position, if there is one.
     */
    public void performAction() {
    }
}