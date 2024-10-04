package model.cards;

import model.player.Player;

/**
 * A card; used as a base class for all cards
 */
public abstract class Card {
    protected int value;
    protected String image;
    protected Player owner;
    protected String message;

    /**
     * Constructs a new card
     *
     * @pre image is a valid image file; value is valid; message is a valid String
     *
     * @post a new card is constructed with the given parameters
     *
     * @param image the path of the image for the card
     * @param value the value of the card
     * @param message the message of the card
     */
    protected Card(String image, int value, String message) {
        this.image = image;
        this.value = value;
        this.message = message;
    }

    /**
     * Performs the dedicated action of the card
     *
     * @post the action of the card is performed
     */
    public abstract void performAction();

    /**
     * Discards the card
     *
     * @post the card is discarded
     */
    public void discard() {
        if (owner != null) {
            setOwner(null);
        }
        else
            Cards.discardCard(this);
    }

        /**
     * Sets the value of the card
     *
     * @pre value is valid
     *
     * @post the value of the card is set
     *
     * @param value the value of the card
     */
    public void setValue(int value) {
        this.value = value;
    }

    /**
     * Gets the value of the card
     *
     * @post the value of the card is returned
     *
     * @return the value of the card
     */
    public int getValue() {
        return this.value;
    }

    /**
     * Sets the message of the card
     *
     * @pre message is a valid String
     *
     * @post the message of the card is set
     *
     * @param message the message of the card
     */
    public void setMessage(String message) {
        this.message = message;
    }

    /**
     * Gets the message of the card
     *
     * @post the message of the card is returned
     *
     * @return the message of the card
     */
    public String getMessage() {
        return this.message;
    }

    /**
     * Sets the image file of the card
     *
     * @pre image is a valid image file
     *
     * @post the image of the card is set
     *
     * @param image the image for the card
     */
    public void setImage(String image) {
        this.image = image;
    }

    /**
     * Gets the image of the card
     *
     * @post the image of the card is returned
     *
     * @return the image of the card
     */
    public String getImage() {
        return this.image;
    }

    /**
     * Sets the owner of the card
     *
     * @pre owner is a valid Player
     *
     * @post the owner of the card is set
     *
     * @param owner the owner of the card
     */
    public void setOwner(Player owner) {
        this.owner = owner;
    }

    /**
     * Gets the owner of the card
     *
     * @post the owner of the card is returned
     *
     * @return the owner of the card; null if the card is not owned
     */
    public Player getOwner() {
        return this.owner;
    }
}