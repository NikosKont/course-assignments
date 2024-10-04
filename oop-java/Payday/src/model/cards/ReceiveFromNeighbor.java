package model.cards;

/**
 * Receive from the Neighbor mail card.
 */
public class ReceiveFromNeighbor extends MailCard {
    /**
     * Constructs a new "Receive" from the Neighbor mail card.
     *
     * @param image the image path of the card
     * @param value the value of the card
     * @param message the message of the card
     * @param choice the choice of the card
     */
    public ReceiveFromNeighbor(String image, int value, String message, String choice) {
        super(image, value, message, choice);
    }

    @Override
    /**
     * Executes the card's action; player gets paid the card's value by the neighbor.
     */
    public void performAction() {
        owner.receive(value);
        owner.getOpponent().pay(value);
    }
}