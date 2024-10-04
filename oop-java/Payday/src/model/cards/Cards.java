package model.cards;

import java.util.ArrayList;
import java.util.Collections;

/**
 * Deck of cards; contains all deal and mail cards, including those discarded.
 */
public class Cards {
    private static ArrayList<MailCard> mailCards;
    private static ArrayList<DealCard> dealCards;
    private static ArrayList<MailCard> discardedMailCards;
    private static ArrayList<DealCard> discardedDealCards;

    /**
     * Constructs mail and deal decks of cards from given mail and deal cards.
     * 
     * @pre valid deal and mail cards ArrayLists are given
     * 
     * @param mail mail cards
     * 
     * @param deal deal cards
     */
    public Cards(ArrayList<DealCard> deal, ArrayList<MailCard> mail) {
        dealCards = deal;
        mailCards = mail;
        discardedMailCards = new ArrayList<>();
        discardedDealCards = new ArrayList<>();
        shuffle();
    }

    /**
     * Shuffles the mail and deal card stacks.
     */
    public static void shuffle() {
        Collections.shuffle(mailCards);
        Collections.shuffle(dealCards);
    }

    /**
     * Returns first mail card on the deck.
     * 
     * @pre there are mail cards left
     * 
     * @post mail card is removed from deck
     * 
     * @return the first mail card
     */
    public static MailCard getMailCard() {
        if (mailCards.isEmpty()) {
            mailCards = (ArrayList<MailCard>)discardedMailCards.clone();
            shuffle();
        }
        return mailCards.remove(0);
    }

    /**
     * Returns first deal card on the deck.
     * 
     * @pre there are deal cards left
     * 
     * @post deal card is removed from deck
     * 
     * @return the first deal card
     */
    public static DealCard getDealCard() {
        if (dealCards.isEmpty()) {
            dealCards = (ArrayList<DealCard>)discardedDealCards.clone();
            shuffle();
        }
        return dealCards.remove(0);
    }

    /**
     * Discards mail card.
     * 
     * @pre valid card is given
     * 
     * @param card card to be discarded
     */
    public static void discardCard(Card card) {
        if (card instanceof MailCard) {
            discardedMailCards.add((MailCard) card);
        } else {
            discardedDealCards.add((DealCard) card);
        }
    }

    /**
     * Discards given mail card.
     * 
     * @pre valid mail card is given
     * 
     * @param card mail card to be discarded
     */
    public static void discardMailCard(MailCard card) {
        card.setOwner(null);

        discardedMailCards.add(card);

        if (mailCards.isEmpty()) {
            mailCards.addAll(discardedMailCards);
            Collections.shuffle(mailCards);
            discardedMailCards.clear();
        }
    }

    /**
     * Discards given deal card.
     * 
     * @pre valid deal card is given
     * 
     * @param card deal card to be discarded
     */
    public static void discardDealCard(DealCard card) {
        if (card.owner != null) {
            card.getOwner().removeDealCard(card);
            card.setOwner(null);
        }

        discardedDealCards.add(card);

        if (dealCards.isEmpty()) {
            dealCards.addAll(discardedDealCards);
            Collections.shuffle(dealCards);
            discardedDealCards.clear();
        }
    }
}