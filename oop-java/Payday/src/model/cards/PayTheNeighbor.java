package model.cards;

/**
 * Pay the Neighbor mail card.
 */
public class PayTheNeighbor extends MailCard {
    /**
     * Constructs a new pay the Neighbor mail card.
     *
     * @param image the image path of the card
     * @param value the value of the card
     * @param message the message of the card
     * @param choice the choice of the card
     */
    public PayTheNeighbor(String image, int value, String message, String choice) {
        super(image, value, message, choice);
    }

    @Override
    /**
     * Executes the card's action; player pays the card's value to the neighbor.
     */
    public void performAction() {
        owner.pay(value);
        owner.getOpponent().receive(value);
    }
}