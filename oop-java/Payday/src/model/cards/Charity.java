package model.cards;

import model.board.Jackpot;

/**
 * Charity mail card.
 */
public class Charity extends MailCard {
    /**
     * Constructs a new charity mail card.
     *
     * @param image the image path of the card
     * @param value the value of the card
     * @param message the message of the card
     * @param choice the choice of the card
     */
    public Charity(String image, int value, String message, String choice) {
        super(image, value, message, choice);
    }

    @Override
    /**
     * Executes the card's action; player adds the card's value to jackpot.
     */
    public void performAction() {
        owner.pay(value);
        Jackpot.add(value);
    }
}