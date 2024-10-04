package model.board;

import model.cards.Cards;
import model.cards.MailCard;
import model.cards.MoveToDealBuyer;
import model.player.Player;

/**
 * A Mail position.
 */
public class Mail extends Tile {
    int numberOfCards;

    /**
     * Constructor for a new Mail position.
     *
     * @pre 1 {@literal <}{@literal =} position {@literal <}{@literal =} 30 {@literal &}{@literal &}  1 {@literal <}{@literal =} numberOfCards {@literal <}{@literal =} 2
     *
     * @param position The position of the tile on the board.
     * @param day The day of the tile.
     * @param image The path for the image of the tile.
     * @param numberOfCards number of cards to draw
     */
    public Mail(int position, String day, String image, int numberOfCards) {
        super(position, day, image);
        this.numberOfCards = numberOfCards;
    }

    public int getNumberOfCards() {
        return numberOfCards;
    }

    @Override
    /**
     * Performs the default action; player draws mail card(s).
     */
    public void performAction(Player player) {
        MailCard card = Cards.getMailCard();
        card.setOwner(player);
        if (numberOfCards == 2) {
            MailCard card2 = Cards.getMailCard();

            if (card instanceof MoveToDealBuyer) {
                card2.performAction();
                card2.discard();
                card.performAction();
                card.discard();
                return;
            }

            card2.performAction();
            card2.discard();

        }
        card.performAction();
        card.discard();
    }
}