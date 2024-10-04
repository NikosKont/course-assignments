package model.player;

import model.cards.*;
import java.util.ArrayList;

/**
 * A player in the game.
 */
public class Player {
    private int die;
    private int loan;
    private int bills;
    private int money;
    private String name;
    private int position;
    private Player opponent;
    private boolean finished;
    private boolean hasMoved;
    private boolean turnEnded;
    private ArrayList<DealCard> dealCards;

    /**
     * Constructs a new Player.
     *
     * @pre a valid name is given
     *
     * @post a new Player is created according to the rules of the game
     *
     * @param name the name of the player
     */
    public Player(String name) {
        this.name = name;
        money = 3500;
        position = 0;
        loan = 0;
        bills = 0;
        dealCards = new ArrayList<DealCard>();
        die = 0;
        opponent = null;
        finished = false;
        turnEnded = false;
    }

    public boolean hasMoved() {
        return hasMoved;
    }

    public void setHasMoved(boolean hasMoved) {
        this.hasMoved = hasMoved;
    }

    public boolean turnEnded() {
        return turnEnded;
    }

    public void setTurnEnded(boolean turnEnded) {
        this.turnEnded = turnEnded;
    }

    /**
     * Player pays the given amount of money.
     *
     * @pre the amount of money to pay is valid
     *
     * @post the player's money is reduced by the given amount
     *
     * @param money the amount of money to pay
     */
    public void pay(int money) {
        this.money -= money;
    }

    /**
     * Player receives the given amount of money.
     *
     * @pre the amount of money to receive is valid
     *
     * @post the player's money is increased by the given amount
     *
     * @param money the amount of money to receive
     */
    public void receive(int money) {
        this.money += money;
    }

    public void advance(int positions) {
        position += positions;

    }

    /**
     * Sets player's opponent
     *
     * @pre the given opponent is valid
     *
     * @post the player's opponent is set to the given opponent
     *
     * @param opponent the player's opponent
     */
    public void setOpponent(Player opponent) {
        this.opponent = opponent;
    }

    /**
     * Gets player's opponent; returns null if no opponent is set
     *
     * @return the player's opponent
     */
    public Player getOpponent() {
        return opponent;
    }

    /**
     * Gets player's money.
     *
     * @return the player's money
     */
    public int readMoney() {
        return money;
    }

    /**
     * Sets player's money.
     *
     * @pre the given amount is valid
     *
     * @post the player's money is set to the given amount
     *
     * @param money the player's money
     */
    public void setMoney(int money) {
        this.money = money;
    }

    /**
     * Adds to player's money.
     *
     * @pre the given amount is valid
     *
     * @post the given amount is added to the player's money
     *
     * @param money the player's money
     */
    public void addMoney(int money) {
        this.money += money;
    }

    /**
     * Gets player's position.
     *
     * @return the player's position
     */
    public int getPosition() {
        return position;
    }

    /**
     * Sets player's position.
     *
     * @pre 0 {@literal <}{@literal =} position {@literal <}{@literal =} 31
     *
     * @post the player's position is set to the given position
     *
     * @param position the player's position
     */
    public void setPosition(int position) {
        this.position = position;
    }

    /**
     * Gets player's current loan amount.
     *
     * @return the player's loan
     */
    public int readLoan() {
        return loan;
    }

    /**
     * Adds given amount to player's loan.
     *
     * @pre the given amount is valid
     *
     * @post the player's loan is set to the given amount
     *
     * @param loan the player's loan is increased by the given amount
     */
    public void getLoan(int loan) {
        this.loan += loan;
        this.money += loan;
    }

    /**
     * Removes given amount from player's loan.
     *
     * @pre the given amount is valid; player has enough money to pay off the loan
     *
     * @post the player's loan is reduced by the given amount
     *
     * @param loan amount to pay off
     */
    public void payLoan(int loan) {
        this.loan -= loan;
        this.money -= loan;
    }

    /**
     * Gets player's current bill amount.
     *
     * @return the player's bills
     */
    public int readBills() {
        return bills;
    }

    /**
     * Adds given amount to player's bill amount.
     *
     * @pre the given amount is valid
     *
     * @post the player's bills are increased by the given amount
     *
     * @param bills amount to add to player's bills
     */
    public void addBills(int bills) {
        this.bills += bills;
    }

    /**
     * Removes given amount from player's bill amount.
     *
     * @pre the given amount is valid; player has enough money to pay off the bills
     *
     * @post the player's bills are reduced by the given amount
     *
     * @param bills amount to pay off
     */
    public void payBills(int bills) {
        this.bills -= bills;
        pay(bills);
    }

    /**
     * Gets player's current deal cards.
     *
     * @return arraylist with player's deal cards
     */
    public ArrayList<DealCard> getDealCards() {
        return dealCards;
    }

    /**
     * Sets player's deal cards.
     *
     * @pre the given deal cards are valid
     *
     * @post the player's deal cards are set to the given deal cards
     *
     * @param dealCards valid dealCards arraylist
     */
    public void setDealCards(ArrayList<DealCard> dealCards) {
        this.dealCards = dealCards;
    }

    /**
     * Adds given deal card to player's deal cards.
     *
     * @pre the given deal card is valid
     *
     * @post deal card is added
     *
     * @param dealCard deal card to add to player's deal cards
     */
    public void addDealCard(DealCard dealCard) {
        dealCards.add(dealCard);
        dealCard.setOwner(this);
    }

    /**
     * Removes given deal card from player's deal cards.
     *
     * @pre the given deal card is valid
     *
     * @post deal card is removed
     *
     * @param dealCard deal card to remove from player's deal cards
     */
    public void removeDealCard(DealCard dealCard) {
        dealCards.remove(dealCard);
    }

    /**
     * Gets player's current die value.
     *
     * @return the player's die value
     */
    public int getDie() {
        return die;
    }

    /**
     * Rolls player's die.
     *
     * @return die's value
     */
    public int rollDie() {
        die = (int) (Math.random() * 6 + 1);
        return die;
    }

    /**
     * Gets player's name.
     *
     * @return the player's name
     */
    public String getName() {
        return name;
    }

    /**
     * Checks if player has finished the game.
     *
     * @return true if player has finished the game, false otherwise
     */
    public boolean hasFinished() {
        return finished;
    }

    /**
     * Sets player's finished value.
     *
     * @pre the given value is valid
     *
     * @post the player's finished value is set to the given value
     *
     * @param finished value to set
     */
    public void setFinished(boolean finished) {
        this.finished = finished;
    }

    public void sellDealCard(DealCard card) {
        addMoney(card.getValue());
        card.discard();
    }
}