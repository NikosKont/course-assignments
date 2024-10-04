package model.cards;

public class DealCard extends Card {
    private int cost;

    /**
     * Constructs a new mail card.
     *
     * @param image the image path of the card
     * @param value the value of the card
     * @param message the message of the card
     * @param cost the cost of the card
     */
    public DealCard(String image, int value, String message, int cost) {
        super(image, value, message);
        this.cost = cost;
    }

    @Override
    /**
     * Executes the card's default action; player buys the card.
     */
    public void performAction() {
        owner.pay(cost);
    }

    /**
     * Sells card.
     */
    public void sell() {
        owner.receive(value);
        setOwner(null);
        discard();
    }

    /**
     * Discards the card
     *
     * @post the card is discarded
     */
    @Override
    public void discard() {
        if (owner != null) {
            owner.removeDealCard(this);
            setOwner(null);
        }
        else
            Cards.discardCard(this);
    }

    /**
     * Gets the cost of the card.
     *
     * @return the cost of the card
     */
    public int getCost() {
        return cost;
    }

    /**
     * Sets the cost of the card.
     *
     * @param cost the cost of the card
     */
    public void setCost(int cost) {
        this.cost = cost;
    }
}