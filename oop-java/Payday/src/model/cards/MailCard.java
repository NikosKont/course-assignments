package model.cards;

public abstract class MailCard extends Card {
    String choice;
    /**
     * Constructs a new mail card.
     *
     * @param image the image path of the card
     * @param value the value of the card
     * @param message the message of the card
     * @param choice the choice of the card
     */
    protected MailCard(String image, int value, String message, String choice) {
        super(image, value, message);
        this.choice = choice;
    }

    /**
     * Returns the choice of the card.
     *
     * @return the choice of the card
     */
    public String getChoice() {
        return choice;
    }
}