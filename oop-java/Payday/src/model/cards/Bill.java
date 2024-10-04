package model.cards;

/**
 * Bill mail card.
 */
public class Bill extends MailCard {
    /**
     * Constructs a new Bill mail card.
     *
     * @param image the image path of the card
     * @param value the value of the card
     * @param message the message of the card
     * @param choice the choice of the card
     */
    public Bill (String image, int value, String message, String choice) {
        super(image, value, message, choice);
    }

    @Override
    /**
     * Executes the card's action; the card's value is added to player's bills.
     */
    public void performAction() {
        owner.addBills(value);
    }
}