package model.cards;

/**
 * Advertisement mail card.
 */
public class Advertisement extends MailCard {
    /**
     * Constructs a new Advertisement mail card.
     *
     * @param image the image path of the card
     * @param value the value of the card
     * @param message the message of the card
     * @param choice the choice of the card
     */
    public Advertisement(String image, int value, String message, String choice) {
        super(image, value, message, choice);
    }

    @Override
    /**
     * Executes the card's action; player receives the card's value.
     */
    public void performAction() {
        owner.receive(value);
    }
}