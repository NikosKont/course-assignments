package controller;

import java.awt.event.ActionEvent;
import java.io.BufferedReader;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Random;
import java.awt.event.*;
import model.board.*;
import model.player.Player;
import model.cards.*;
import view.*;
import view.MainWindow;
import view.MainWindow.*;

/**
 * Controller for the game.
 */
public class Controller {
    private Player player1, player2;
    private Board board;
    private Cards cards;
    private int monthsToPlay;
    private int currentMonth;
    private MainWindow mainWindow;
    private Player turn;

    /**
     * Constructs controller and initializes the game.
     *
     * @post The game is ready to be played.
     */
    public Controller() {
        monthsToPlay = MonthsSelectionDialog.getSelection();

        currentMonth = 1;
        board = new Board(fillBoard());
        cards = initializeCards();
        player1 = new Player("Player 1");
        player2 = new Player("Player 2");
        player1.setOpponent(player2);
        player2.setOpponent(player1);
        turn = player1;
        Jackpot.setImage("resources/images/jackpot.png");
        mainWindow = new MainWindow(this);
    }

    /**
     * Starts the game.
     *
     * @post The game is started.
     */
    public void start() {
        while (player1.getDie() == player2.getDie()) {
            new WaitDiceRoll(player1);
            new WaitDiceRoll(player2);
        }
        turn = player1.getDie() > player2.getDie() ? player1 : player2;
        turn.setHasMoved(false);
        turn.setTurnEnded(false);
        mainWindow.getInfobox().updateValues();
        InfoMessage.showMessage(turn.getName() + " starts");
        mainWindow.getInfobox().setMessage("Roll the dice.");
    }

    public class MailCardListener implements ActionListener {
        @Override
        public void actionPerformed(ActionEvent e) {
            // TODO Auto-generated method stub

        }
    }

    public class DealCardListener implements ActionListener {
        @Override
        public void actionPerformed(ActionEvent e) {
            DealCard card = (DealCard)e.getSource();
            if (card.getOwner() == player1) {
                card.getOwner().addDealCard(card);
                card.discard();
                mainWindow.getInfo(card.getOwner()).updateValues();
            }
            else {
                card.getOwner().sellDealCard(card);
                mainWindow.getInfo(card.getOwner()).updateValues();
            }
        }
    }

    public class PlayerMoved implements ActionListener {
        @Override
        public void actionPerformed(ActionEvent e) {
            if (! (e instanceof PlayerMove)) return;

            Player player = (Player) e.getSource();
            player.setTurnEnded(false);
            Tile tile = getBoard().getTile(player.getPosition());

            if (tile.getDay().equals("Thursday")) {
                int selection = Thursday.getSelection();
                if (selection == 0) {
                    if (turn.readMoney() < 300) {
                        int loan = LoanDialog.getLoanDialog(false, 300 - turn.readMoney());
                        player.getLoan(loan);
                    }
                    player.pay(300);
                }
                int random = new Random().nextInt(2);
                if (random == 1) {
                    turn.addMoney(2400);
                    mainWindow.getInfo(turn).updateValues();
                    Thursday.showRise();
                }
                else
                    Thursday.showFall();
            }
            else if (tile.getDay().equals("Sunday")) {
                int selection = Sunday.getSelection();
                if (selection < 3) {
                    if (turn.readMoney() < 500) {
                        int loan = LoanDialog.getLoanDialog(false, 500 - turn.readMoney());
                        player.getLoan(loan);
                    }
                    player.pay(500);
                }
                if (selection == 0) {
                    player.addMoney((int)2.37 * 500);
                }
                mainWindow.getInfo(player).updateValues();
                Sunday.showResult(selection);
            }

            if (tile instanceof Buyer) {
                if (turn.getDealCards().isEmpty()) {
                    InfoMessage.showMessage("No deal cards to sell.");
                    turn.setTurnEnded(true);
                }
                else
                    mainWindow.getInfobox().setMessage("Open your deal cards and choose one to sell.");
            }
            else if (tile instanceof Deal) {
                mainWindow.getInfobox().setMessage("Draw a deal card.");
                CardButton dealCards = mainWindow.getDealCardButton();
                dealCards.setEnabled(true);
                dealCards.addActionListener(new ActionListener() {
                    @Override
                    public void actionPerformed(ActionEvent e) {
                        if (!turn.turnEnded()) {
                            DealCard card = Cards.getDealCard();
                            card.setOwner(player);
                            mainWindow.new ShowDealCard(card, ShowDealCard.BUY);
                            dealCards.setEnabled(false);
                            turn.setTurnEnded(true);
                            endTurn();
                        }
                        else {
                            ErrorMessage.showMessage("Card already drawn.");
                        }
                    }
                });
            }
            else if (tile instanceof FamilyCasinoNight) {
                if (player.getDie() % 2 == 0) {
                    int value = Jackpot.getValue() > 500 ? 500 : Jackpot.getValue();
                    InfoMessage.showMessage("You take " + value + " euros from Jackpot!");
                }
                else {
                    InfoMessage.showMessage("You give 500 euros to Jackpot.");
                    if (player.readMoney() < 500) {
                        LoanDialog.getLoanDialog(false, 500 - player.readMoney());
                    }
                }
                tile.performAction(player);
                mainWindow.getJackpotValue().updateValue();
                turn.setTurnEnded(true);
            }
            else if (tile instanceof Lottery) {
                int playerSelection = LotteryDialog.lotteryDialog(mainWindow, player);
                int opponentSelection = LotteryDialog.lotteryDialog(mainWindow, player.getOpponent(), playerSelection);

                do {
                    mainWindow.getGUI(player).rollDice();
                } while (player.getDie() != playerSelection && player.getDie() != opponentSelection);

                Player winner = player.getDie() == playerSelection ? player : player.getOpponent();

                InfoMessage.showMessage(winner.getName() + " wins 1000 Euros!");
                winner.addMoney(1000);
                mainWindow.getInfo(winner).updateValues();

                turn.setTurnEnded(true);
            }
            else if (tile instanceof Mail) {
                int numberOfCards = ((Mail)tile).getNumberOfCards();
                if (numberOfCards == 1) {
                    mainWindow.getInfobox().setMessage("Draw a mail card.");
                }
                else {
                    mainWindow.getInfobox().setMessage("Draw two mail cards.");
                }

                CardButton mailCards = mainWindow.getMailCardButton();

                    mailCards.setEnabled(true);
                    mailCards.addActionListener(new ActionListener() {
                        @Override
                        public void actionPerformed(ActionEvent e) {
                            if (!turn.turnEnded()) {
                                MailCard card = Cards.getMailCard();
                                card.setOwner(player);
                                mainWindow.new ShowMailCard(card);
                                card.performAction();
                                mainWindow.getInfo(player).updateValues();
                                mailCards.setEnabled(false);
                                turn.setTurnEnded(true);
                                if (card instanceof MoveToDealBuyer) {
                                    int i;
                                    for (i = player.getPosition(); i < 31 && !(board.getTile(i) instanceof Buyer); i++);

                                    if (i == 31) return;

                                    player.setPosition(i);
                                    mainWindow.getGUI(player).move();
                                }
                            }
                            else {
                                ErrorMessage.showMessage("Card already drawn.");
                            }
                        }
                    });
            }
            else if (tile instanceof PayDay) {
                player.setFinished(true);
                if (player.getOpponent().hasFinished()) {
                    if (getMonthsLeft() == 0) {
                        Endgame.showEndgame(determineWinner());
                        System.exit(0);
                    }
                    else {
                        advanceMonth();
                    }
                }

                turn.setTurnEnded(true);
            }
            else if (tile instanceof RadioContest) {
                do {
                    new WaitDiceRoll(turn);
                    new WaitDiceRoll(turn.getOpponent());
                } while (turn.getDie() == turn.getOpponent().getDie());

                if (turn.getDie() == turn.getOpponent().getDie()) {
                    mainWindow.getInfobox().setMessage("You both rolled the same number. You both get 1000 Euros.");
                    player.addMoney(1000);
                    player.getOpponent().addMoney(1000);
                    mainWindow.getInfo(player).updateValues();
                    mainWindow.getInfo(player.getOpponent()).updateValues();
                }
                else if (turn.getDie() > turn.getOpponent().getDie()) {
                    mainWindow.getInfobox().setMessage("You rolled higher. You get 1000 Euros.");
                    player.addMoney(1000);
                    mainWindow.getInfo(player).updateValues();
                }
                else {
                    mainWindow.getInfobox().setMessage("You rolled lower. You get 1000 Euros.");
                    player.getOpponent().addMoney(1000);
                    mainWindow.getInfo(player.getOpponent()).updateValues();
                }

                turn.setTurnEnded(true);
            }
            else if (tile instanceof Start) {

            }
            else if (tile instanceof Sweepstakes) {
                new WaitDiceRoll(player);
                mainWindow.getInfobox().setMessage("You get " + 1000 * player.getDie() + " euros.");
                tile.performAction(player);
                mainWindow.getInfo(player).updateValues();

                turn.setTurnEnded(true);
            }
            else if (tile instanceof YardSale) {
                mainWindow.getInfobox().setMessage("Roll the dice.");
                new WaitDiceRoll(player);
                mainWindow.getInfobox().setMessage("You pay " + 100 * player.getDie() + " euros. Draw a deal card.");
                mainWindow.getInfo(player).updateValues();

                turn.setTurnEnded(true);
            }

            endTurn();
            mainWindow.getInfobox().updateValues();
        }
    }

    public class DiceRolled implements ActionListener {
        @Override
        public void actionPerformed(ActionEvent e) {
            if (! (e instanceof MainWindow.DiceRoll)) return;

            Player player = (Player)e.getSource();
            int roll = e.getID();

            if (mainWindow.getGUI(player).diceRollPending())
                return;

            if (board.getTile(player.getPosition()) instanceof Lottery)
                return;

            if (!player.hasMoved()) {
                if (roll == 6) {
                    new JackpotDialog(mainWindow);
                    player.addMoney(Jackpot.getValue());
                    Jackpot.reset();
                    mainWindow.getInfo(player).updateValues();
                    mainWindow.getJackpotValue().updateValue();
                }

                int newPosition = player.getPosition() + roll;
                if (newPosition > 31)
                    newPosition = 31;

                if (player.getPosition() == player.getOpponent().getPosition()) {
                    mainWindow.getPawn(player.getOpponent()).setOffset(false);
                    mainWindow.getPawn(player).setOffset(false);
                }

                player.setPosition(newPosition);
                mainWindow.getGUI(player).move();
                player.setHasMoved(true);
            }
        }
    }

    public boolean endTurn() {
        if (turn.turnEnded()) {
            turn = turn.getOpponent();
            turn.setTurnEnded(false);
            turn.setHasMoved(false);
            mainWindow.getInfobox().updateValues();
            mainWindow.getInfobox().setMessage("Roll the dice.");
            mainWindow.getGUI(turn).setDiceRollPending(false);
            return true;
        }
        return false;
    }

    public Board getBoard() {
        return board;
    }

    public Player getPlayer1() {
        return player1;
    }

    public Player getPlayer2() {
        return player2;
    }

    public Player getTurn() {
        return turn;
    }

    public class WaitDiceRoll implements ActionListener {
        private volatile boolean flag = false;
        private Player player;

        public WaitDiceRoll(Player player) {
            Player hasTurn = turn;
            turn = player;

            mainWindow.getInfobox().setMessage("Roll the dice.");
            mainWindow.getInfobox().updateValues();

            this.player = player;
            mainWindow.getGUI(player).addListener(this);
            mainWindow.getGUI(player).setDiceRollPending(true);

            while (!flag) {
                try {
                    Thread.sleep(100);
                }
                catch(InterruptedException e) {
                    System.exit(1);
                }
            }

            turn = hasTurn;
            mainWindow.getInfobox().setMessage("");
            mainWindow.getInfobox().updateValues();
            mainWindow.getGUI(player).removeListener(this);
            mainWindow.getGUI(player).setDiceRollPending(false);
        }

        @Override
        public void actionPerformed(ActionEvent e) {
            if (e instanceof MainWindow.DiceRoll && e.getSource() == player) {
                flag = true;
            }
        }
    }

    public void rollDie(Player player) {
        player.rollDie();
        mainWindow.getInfo(player).updateValues();
    }

    /**
     * Fills the board with tiles.
     *
     * @post The board is filled with tiles according to the rules of the game.
     */
    private Tile[] fillBoard() {
        Random rnd = new Random();
        String path = "resources/images/";

        Tile[] board = new Tile[32];
        board[0] = new Start(path + "start.png");
        board[31] = new PayDay(path + "pay.png");

        int days = 1;
        for (int i = 0; i < 4; i++)
            board[days] = new Mail(days++, "None", path + "mc1.png", 1);

        for (int i = 0; i < 4; i++)
            board[days] = new Mail(days++, "None", path + "mc2.png", 2);

        for (int i = 0; i < 5; i++)
            board[days] = new Deal(days++, "None", path + "deal.png");

        for (int i = 0; i < 2; i++)
            board[days] = new Sweepstakes(days++, "None", path + "sweep.png");

        for (int i = 0; i < 3; i++)
            board[days] = new Lottery(days++, "None", path + "lottery.png");

        for (int i = 0; i < 2; i++)
            board[days] = new RadioContest(days++, "None", path + "radio.png");

        for (int i = 0; i < 6; i++)
            board[days] = new Buyer(days++, "None", path + "buyer.png");

        for (int i = 0; i < 2; i++)
            board[days] = new FamilyCasinoNight(days++, "None", path + "casino.png");

        for (int i = 0; i < 2; i++)
            board[days] = new YardSale(days++, "None", path + "yard.png");

        for (int i = 30; i > 1; i--) {
            int random = rnd.nextInt(i - 1) + 1;    // Start (board[0]) must not change
            Tile temp = board[i];
            board[i] = board[random];
            board[random] = temp;

            board[i].setPosition(i);
            board[i].setDay(Tile.days[i % 7]);
        }

        board[1].setPosition(1);
        board[1].setDay("Monday");

        return board;
    }

    /**
     * Initializes needed cards for the game.
     */
    public Cards initializeCards() {
        String[][] mailCards = readCards("mail");
        String[][] dealCards = readCards("deal");
        ArrayList<MailCard> mailCardList = new ArrayList<>();
        ArrayList<DealCard> dealCardList = new ArrayList<>();
        String imagePath = "resources/cards/";

        for (String[] card: mailCards) {
            MailCard newCard = null;
            switch (card[1]) {
                case "Advertisement":
                    newCard = new Advertisement(imagePath + card[5], Integer.parseInt(card[4]), card[2], card[3]);
                    break;
                case "Bill":
                    newCard = new Bill(imagePath + card[5], Integer.parseInt(card[4]), card[2], card[3]);
                    break;
                case "Charity":
                    newCard = new Charity(imagePath + card[5], Integer.parseInt(card[4]), card[2], card[3]);
                    break;
                case "PayTheNeighbor":
                    newCard = new PayTheNeighbor(imagePath + card[5], Integer.parseInt(card[4]), card[2], card[3]);
                    break;
                case "MadMoney":
                    newCard = new ReceiveFromNeighbor(imagePath + card[5], Integer.parseInt(card[4]), card[2], card[3]);
                    break;
                case "MoveToDealBuyer":
                    newCard = new MoveToDealBuyer(imagePath + card[5], Integer.parseInt(card[4]), card[2], card[3]);
                    break;
                default:
                    System.exit(-1);
            }
            mailCardList.add(newCard);
        }
        mailCardList.trimToSize();

        for (String[] card: dealCards) {
            dealCardList.add(new DealCard(imagePath + card[5], Integer.parseInt(card[4]), card[2], Integer.parseInt(card[3])));
        }
        dealCardList.trimToSize();

        return new Cards(dealCardList, mailCardList);
    }

    /**
     * Reads the cards from the files.
     * @param type
     * @return
     */
    private String[][] readCards(String type) {
        String[][] cards;
        if (type.equals("mail"))
            cards = new String[48][];
        else
            cards = new String[20][];

        BufferedReader br = null;
		String sCurrentLine;
		try {
			String fullPath = System.getProperty("user.dir") + "\\resources\\" + type + "Cards.csv";
			br = new BufferedReader(new FileReader(fullPath));
		}
        catch (FileNotFoundException ex) {
            System.exit(1);
		}
		int count = 0;
		try {
			br.readLine();
			while ((sCurrentLine = br.readLine()) != null) {
                cards[count++] = sCurrentLine.split(",");
			}
			br.close();
		}
        catch (IOException ex) {
			System.exit(1);
		}
        return cards;
    }

    /**
     * Advances the current month.
     *
     * @post current month is incremented.
     */
    public void advanceMonth() {
        currentMonth++;
        board = new Board(fillBoard());
    }

    public int getMonthsLeft() {
        return monthsToPlay - currentMonth;
    }

    /**
     * Determines winner.
     *
     * @pre The game is finished.
     *
     * @return the winner.
     */
    public Player determineWinner() {
        int p1 = player1.readMoney() - player1.readBills() - player1.readLoan();
        int p2 = player1.readMoney() - player1.readBills() - player1.readLoan();

        return (p1 > p2) ? player1 : player2;
    }

    public static void main(String[] args) {
        Controller payday = new Controller();
        payday.start();
    }
}