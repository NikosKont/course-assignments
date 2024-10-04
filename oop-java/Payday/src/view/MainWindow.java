package view;

import java.awt.Color;

import java.awt.Font;
import java.awt.Point;
import java.awt.event.*;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;

import javax.swing.*;
import controller.Controller;
import model.board.*;
import model.player.Player;
import java.awt.Dimension;

import model.cards.*;

/**
 * The main window of the game.
 */
public class MainWindow extends JFrame {
    private Controller controller;
    private JLayeredPane layeredPane;
    private JLabel jackpot;
    private JackpotValue jackpotValue;
    private Infobox infobox;
    private CardButton mailCards, dealCards;
    private TileGUI[] days;
    private Map<Player, PlayerGUI> playerGUIs;

    /**
     * Constructs Main Window.
     *
     * @param controller the controller of the game.
     */
    public MainWindow(Controller controller) {
        super("Payday");
        setIconImage(new ScalableImage("resources/images/logo.png").getImage());
        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        setResizable(false);
        this.controller = controller;
        initComponents();
        pack();
        setLocationRelativeTo(null);

        playerGUIs = new HashMap<>(2);
        playerGUIs.put(controller.getPlayer1(), new PlayerGUI(controller.getPlayer1(), Color.BLUE));
        playerGUIs.get(controller.getPlayer1()).info.setLocation(TileGUI.TILE_WIDTH * 7 + 19, 20);
        playerGUIs.put(controller.getPlayer2(), new PlayerGUI(controller.getPlayer2(), Color.YELLOW));
        playerGUIs.get(controller.getPlayer2()).info.setLocation(TileGUI.TILE_WIDTH * 7 + 19, 768 - 20 - 250);

        setVisible(true);
    }

    abstract class ShowCard extends JFrame {
        JLabel image;
        JLabel message;

        protected ShowCard(Card card, String type) {
            super(type);
            getContentPane().setPreferredSize(new Dimension(400, 200));
            pack();
            setLayout(null);
            setLocationRelativeTo(null);
            setDefaultCloseOperation(JFrame.DISPOSE_ON_CLOSE);

            image = new JLabel(new ScalableImage(card.getImage(), 150, 125));
            image.setBounds(10, 10, 150, 125);
            add(image);
        }
    }

    public class ShowMailCard extends ShowCard {
        JButton button;

        public ShowMailCard(MailCard card) {
            super(card, card.getClass().getSimpleName() + " Mail Card");
            message = new JLabel(getMessage(card), JLabel.CENTER);
            message.setBounds(170, 10, 220, 125);
            add(message);

            setIconImage(new ImageIcon("resources/images/mailCard.png").getImage());

            button = new JButton(card.getChoice());
            button.setSize(button.getPreferredSize());
            button.setLocation(200 - button.getWidth() / 2, 150);
            button.addActionListener(e -> {
                card.performAction();
                dispose();
            });
            add(button);

            setVisible(true);
        }

        String getMessage(MailCard card) {
            // HTML so that text breaks over lines as needed
            return "<html>" + card.getMessage() + "</html>";
        }
    }

    public class ShowDealCard extends ShowCard {
        public static final int BUY = 0;
        public static final int SELL = 1;
        public static final int SEE = 2;
        JButton option1;
        JButton option2;
        DealCard card;

        public ShowDealCard(DealCard card, int action) {
            super(card, "Deal Card");

            this.card = card;

            message = new JLabel(getMessage(card), JLabel.CENTER);
            message.setBounds(170, 10, 220, 125);
            this.add(message);

            setIconImage(new ImageIcon("resources/images/dealCard.png").getImage());

            option1 = new JButton("Accept");
            option1.setBounds(90, 150, 100, 30);

            option2 = new JButton("Reject");
            option2.setBounds(210, 150, 100, 30);

            addActionListener(action);

            add(option1);
            add(option2);

            setVisible(true);
        }

        private void addActionListener(int action) {
            Player player = controller.getTurn();
            if (action == BUY) {
                option1.addActionListener(e -> {
                    if (player.readMoney() < card.getCost()) {
                        int difference = card.getCost() - player.readMoney();
                        int loan = LoanDialog.getLoanDialog(true, difference);
                        if (loan == 0) {
                            dispose();
                            return;
                        }
                        else {
                            player.getLoan(loan);
                        }
                    }
                    player.addDealCard(card);
                    player.pay(card.getCost());
                    getGUI(player).updateAll();
                    dispose();
                });

                option2.addActionListener(e -> {
                    card.discard();
                    dispose();
                });
            }
            else if (action == SELL) {
                option1.addActionListener(e -> {
                    if (!player.turnEnded()) {
                        getInfo(player).updateValues();
                        player.sellDealCard(card);
                        getGUI(player).updateAll();
                        player.setTurnEnded(true);
                        controller.endTurn();
                    }
                    else {
                        ErrorMessage.showMessage("Already sold a card.");
                    }
                    dispose();
                });
                option1.setText("Sell");

                option2.addActionListener(e -> {
                    showNextDealCard(1);
                    dispose();
                });
                option2.setText("Next");
            }
            else if (action == SEE) {
                option1.addActionListener(e -> dispose());
                option1.setText("Exit");

                option2.addActionListener(e -> {
                    showNextDealCard(2);
                    dispose();
                });
                option2.setText("Next");
            }
        }

        void showNextDealCard(int action) {
            int index = card.getOwner().getDealCards().indexOf(card);
            if (index == card.getOwner().getDealCards().size() - 1) {
                new ShowDealCard(card.getOwner().getDealCards().get(0), action);
            }
            else {
                new ShowDealCard(card.getOwner().getDealCards().get(index + 1), action);
            }
        }

        String getMessage(DealCard card) {
            // HTML so that text breaks over lines as needed
            return "<html>" + card.getMessage() + "<br><br>Cost: " + card.getCost() + "<br>Value: " + card.getValue() + "</html>";
        }
    }

    public PlayerGUI getGUI (Player player) {
        return playerGUIs.get(player);
    }

    public void updateAllValues() {

    }

    public class PlayerGUI implements Subject {
        private PlayerInfo info;
        private Player player;
        private Pawn pawn;
        private ArrayList<ActionListener> listeners;
        private boolean diceRollPending;

        PlayerGUI(Player player, Color color) {
            this.player = player;
            info = new PlayerInfo(player, color);
            pawn = new Pawn(player, color);
            layeredPane.add(pawn, Integer.valueOf(2));
            layeredPane.add(info, Integer.valueOf(1));

            listeners = new ArrayList<>(2);

            addListener(controller.new PlayerMoved());
            addListener(controller.new DiceRolled());
            listeners.trimToSize();
        }

        public void updateAll() {
            info.updateValues();
            pawn.updateLocation();
        }

        public void move() {
            pawn.updateLocation();
            notifyListeners(new PlayerMove(player, player.getDie(), "Player Moved"));
        }

        public void notifyListeners(ActionEvent e) {
            for (ActionListener l : listeners) {
                l.actionPerformed(e);
            }
        }

        public void addListener(ActionListener l) {
            listeners.add(l);
        }

        public void removeListener(ActionListener l) {
            listeners.remove(l);
        }

        public void diceRolled(){
            notifyListeners(new DiceRoll(player, player.getDie(), "Dice Rolled"));
        }

        public void rollDice() {
            info.rollDice();
        }

        public boolean diceRollPending() {
            return diceRollPending;
        }

        public void setDiceRollPending(boolean diceRollPending) {
            this.diceRollPending = diceRollPending;
        }
    }

    public class Pawn extends JLabel {
        Player player;
        boolean isOffset;

        public Pawn(Player player, String image) {
            super(new ScalableImage(image, 70, 70), JLabel.CENTER);
            this.setPreferredSize(new Dimension(70, 70));
            this.setSize(70, 70);
            this.player = player;
            isOffset = false;
            this.updateLocation();
        }

        public Pawn(Player player, Color color) {
            this(player, getPawnImage(color));
        }

        public boolean isOffset() {
            return isOffset;
        }

        public void setOffset(boolean offset) {
            if (!offset) {
                this.setLocation(getNormalLocation());
            }
            isOffset = offset;
        }

        public Point getNormalLocation() {
            Point location = days[player.getPosition()].getLocation();
            location.setLocation(location.x + 20, location.y + 42);
            return location;
        }

        public void updateLocation() {
            Point location = getNormalLocation();
            try {
                if (player.getPosition() == player.getOpponent().getPosition()) {
                    if (!getPawn(player.getOpponent()).isOffset()) {
                        setLocation(location.x - 20, location.y - 15);
                        isOffset = true;
                        getPawn(player.getOpponent()).setLocation(location.x + 20, location.y + 15);
                    }
                }
                else
                    setLocation(location);
            }
            catch (NullPointerException e) {
                setLocation(location);
            }
        }
    }

    public class DiceRoll extends ActionEvent {
        public DiceRoll(Player player, int id, String command) {
            super(player, id, command);
        }
    }

    public class PlayerMove extends ActionEvent {
        public PlayerMove(Player player, int id, String command) {
            super(player, id, command);
        }
    }

    public class CardDraw extends ActionEvent {
        public CardDraw(Card card, int id, String command) {
            super(card, id, command);
        }
    }

    public class PlayerInfo extends JPanel {
        private JLabel playerName;
        private JLabel playerMoney;
        private JLabel playerLoan;
        private JLabel playerBills;
        private JButton dealCards;
        private JButton getLoan;
        private JButton endTurn;
        private JButton dice;
        private Player player;

        public PlayerInfo(Player player) {
            super();
            this.player = player;
            setLayout(null);
            setPreferredSize(new Dimension(300, 250));
            setSize(300, 250);
            setOpaque(true);

            playerName = new JLabel(player.getName(), JLabel.CENTER);
            playerName.setFont(new Font("Arial", Font.BOLD, 20));
            playerName.setBounds(0, 10, 300, 30);

            playerMoney = new JLabel("Money: " + player.readMoney() + " Euros");
            playerMoney.setBounds(10, 40, 280, 20);

            playerLoan = new JLabel("Loan: " + player.readLoan() + " Euros");
            playerLoan.setBounds(10, 65, 280, 20);

            playerBills = new JLabel("Bills: " + player.readBills() + " Euros");
            playerBills.setBounds(10, 90, 280, 20);

            dealCards = new JButton("My Deal Cards");
            dealCards.setBounds(10, 130, 150, 30);
            dealCards.addActionListener(new ActionListener() {
                @Override
                public void actionPerformed(ActionEvent e) {
                    if (!player.getDealCards().isEmpty()){
                        if (controller.getBoard().getTile(player.getPosition()) instanceof Buyer && !player.turnEnded())
                            new ShowDealCard(player.getDealCards().get(0), ShowDealCard.SELL);
                        else
                            new ShowDealCard(player.getDealCards().get(0), ShowDealCard.SEE);
                    }
                    else
                        InfoMessage.showMessage("You don't have any deal cards!");
                }
            });

            getLoan = new JButton("Get Loan");
            getLoan.setBounds(10, 170, 150, 30);
            getLoan.addActionListener(new ActionListener() {
                @Override
                public void actionPerformed(ActionEvent e) {
                    if (controller.getTurn() == player) {
                        player.getLoan(LoanDialog.getLoanDialog(true));
                        updateValues();
                    }
                }
            });

            endTurn = new JButton("End Turn");
            endTurn.addActionListener(new ActionListener() {
                @Override
                public void actionPerformed(ActionEvent e) {
                    if (! (controller.getTurn() == player && controller.getTurn().turnEnded()) ) {
                        ErrorMessage.showMessage("Cannot end turn yet.");
                    }
                    else
                        controller.endTurn();
                }
            });
            endTurn.setBounds(10, 210, 150, 30);

            dice = new JButton(getDiceImage(player.getDie()));
            dice.setBounds(175, 130, 110, 110);
            dice.addActionListener(new ActionListener() {
                @Override
                public void actionPerformed(ActionEvent e) {
                    if (controller.getTurn() == player && !player.turnEnded()) {
                        dice.setIcon(getDiceImage(player.rollDie()));
                        getGUI(player).diceRolled();
                    }
                }
            });

            add(playerName);
            add(playerMoney);
            add(playerLoan);
            add(playerBills);
            add(dealCards);
            add(getLoan);
            add(endTurn);
            add(dice);
        }

        public PlayerInfo(Player player, Color color) {
            this(player);
            this.setBorder(BorderFactory.createLineBorder(color, 3));
        }

        public void setBorder(Color color) {
            this.setBorder(BorderFactory.createLineBorder(color, 3));
        }

        public void updateValues() {
            playerMoney.setText("Money: " + player.readMoney() + " Euros");
            playerLoan.setText("Loan: " + player.readLoan() + " Euros");
            playerBills.setText("Bills: " + player.readBills() + " Euros");
            dice.setIcon(getDiceImage(player.getDie()));
        }

        private ScalableImage getDiceImage(int die) {
            if (die == 0)
                die = 1;

            return new ScalableImage("resources/images/dice-" + die + ".jpg", 110, 110);
        }

        public JButton getDiceButton() {
            return dice;
        }

        public void rollDice() {
            dice.setIcon(getDiceImage(player.rollDie()));
        }
    }

    void initComponents() {
        layeredPane = new JLayeredPane();
        layeredPane.setPreferredSize(new Dimension(1024, 768));
        setContentPane(layeredPane);

        ScalableImage logo, background;
        logo = new ScalableImage("resources/images/logo.png");
        JLabel logoLabel = new JLabel(logo.getScaled(TileGUI.TILE_WIDTH * 7, TileGUI.TILE_HEIGHT, 4));
        logoLabel.setBounds(0, 0, TileGUI.TILE_WIDTH * 7, TileGUI.TILE_HEIGHT);
        background = new ScalableImage("resources/images/bg_green.png");
        JLabel bg = new JLabel(background, JLabel.CENTER);
        bg.setSize(1024, 768);

        layeredPane.add(logoLabel, Integer.valueOf(1));
        layeredPane.add(bg, Integer.valueOf(0));

        initTiles();

        for (int i = 0; i < days.length; i++) {
            layeredPane.add(days[i], Integer.valueOf(1));
        }

        jackpot = new JLabel(new ScalableImage("resources/images/jackpot.png", TileGUI.TILE_WIDTH * 2, TileGUI.IMAGE_HEIGHT), JLabel.CENTER);
        jackpot.setBounds(TileGUI.TILE_WIDTH * 4, TileGUI.TILE_HEIGHT * 5 + 10, TileGUI.TILE_WIDTH * 3, TileGUI.IMAGE_HEIGHT);
        layeredPane.add(jackpot, Integer.valueOf(1));

        jackpotValue = new JackpotValue();
        layeredPane.add(jackpotValue, Integer.valueOf(1));

        infobox = new Infobox();
        layeredPane.add(infobox, Integer.valueOf(1));

        dealCards = new CardButton("resources/images/dealCard.png", TileGUI.TILE_WIDTH * 7 + 19, 389);
        layeredPane.add(dealCards, Integer.valueOf(1));
        mailCards = new CardButton("resources/images/mailCard.png", TileGUI.TILE_WIDTH * 7 + 19 + dealCards.getWidth() + 10, 389);
        layeredPane.add(mailCards, Integer.valueOf(1));
    }

    public class Infobox extends JPanel {
        private String message = "";
        private JLabel monthsLeft;
        private JLabel turn;
        private JLabel action;
        private JLabel title;

        Infobox() {
            super();
            setLayout(null);
            setPreferredSize(new Dimension (300, 99));
            setBounds(TileGUI.TILE_WIDTH * 7 + 19, 280, 300, 99);
            title = new JLabel("Info Box", JLabel.CENTER);
            title.setFont(new Font("Arial", Font.BOLD, 20));
            title.setBounds(0, 5, 300, 25);
            add(title);

            monthsLeft = new JLabel("Months Left: " + controller.getMonthsLeft());
            monthsLeft.setBounds(10, 40, 135, 15);
            add(monthsLeft);
            turn = new JLabel("Turn: " + controller.getTurn().getName(), JLabel.RIGHT);
            turn.setBounds(155, 40, 135, 15);
            add(turn);
            action = new JLabel("--> " + message);
            action.setBounds(10, 70, 280, 15);
            add(action);
            setOpaque(true);
            setBorder(BorderFactory.createLineBorder(Color.BLACK, 3));
        }

        Infobox(String message) {
            this();
            setMessage(message);
        }

        public void updateValues() {
            monthsLeft.setText("Months Left: " + controller.getMonthsLeft());
            turn.setText("Turn: " + controller.getTurn().getName());
        }

        public void setMessage(String message) {
            this.message = message;
            action.setText("--> " + message);
        }
    }

    public JButton getDice(Player player) {
        return getInfo(player).dice;
    }

    public CardButton getDealCardButton() {
        return dealCards;
    }

    public CardButton getMailCardButton() {
        return mailCards;
    }

    public Pawn getPawn (Player player) {
        return playerGUIs.get(player).pawn;
    }

    public PlayerInfo getInfo (Player player) {
        return playerGUIs.get(player).info;
    }

    public void updateAll (Player player) {
        playerGUIs.get(player).updateAll();
    }

    public Infobox getInfobox() {
        return infobox;
    }

    public JackpotValue getJackpotValue() {
        return jackpotValue;
    }

    private static String getPawnImage(Color color) {
        if (color == Color.BLUE)
            return "resources/images/pawn_blue.png";
        else
            return "resources/images/pawn_yellow.png";
    }

    public class TileGUI extends JPanel {
        // Board width == 2/3 of window width, 7 tiles per row
        private static final int TILE_WIDTH = (int) ((2.0 / 3.0) * (1.0 / 7.0) * 1024);
        // 6 tiles per column
        private static final int TILE_HEIGHT = (int) ((1.0 / 6.0) * 768);
        private static final int LABEL_HEIGHT = (int) ((1.0 / 4.0) * TILE_HEIGHT);
        private static final int IMAGE_HEIGHT = (int) ((3.0 / 4.0) * TILE_HEIGHT);

        private JLabel label;
        private JLabel image;

        public TileGUI(int x, int y) {
            super();
            setSize(TILE_WIDTH, TILE_HEIGHT);
            setLocation(x, y);
            setOpaque(true);
            setLayout(null);
        }

        public TileGUI(int x, int y, String imagePath, String label) {
            this(x, y);
            setLabel(label);
            setImage(imagePath);
        }

        public TileGUI(String imagePath, String label) {
            this(0, 0, imagePath, label);
        }

        public void setImage(String path) {
            image = new JLabel(new ScalableImage(path, TILE_WIDTH, IMAGE_HEIGHT));
            image.setBounds(0, LABEL_HEIGHT, TILE_WIDTH, IMAGE_HEIGHT);
            add(image);
        }

        public void setLabel(String text) {
            label = new JLabel(text, JLabel.CENTER);
            label.setBounds(0, 0, TILE_WIDTH, LABEL_HEIGHT);
            label.setPreferredSize(new Dimension(TILE_WIDTH, LABEL_HEIGHT));
            label.setLocation(0, 0);
            label.setBackground(Color.YELLOW);
            label.setOpaque(true);
            label.setBorder(BorderFactory.createMatteBorder(1, 1, 1, 1, Color.LIGHT_GRAY));
            add(label);
        }

        public JLabel getLabel() {
            return label;
        }

        public JLabel getImage() {
            return image;
        }
    }

    public class JackpotValue extends JLabel {
        private int value;

        public JackpotValue() {
            super("Jackpot: " + Jackpot.getValue() + " Euros", JLabel.CENTER);
            this.value = Jackpot.getValue();
            setFont(new Font("Arial", Font.BOLD, 20));
            setForeground(Color.WHITE);
            setPreferredSize(new Dimension(TileGUI.TILE_WIDTH * 3, 20));
            setBounds(TileGUI.TILE_WIDTH * 4, TileGUI.TILE_HEIGHT * 6 - 22, TileGUI.TILE_WIDTH * 3, 20);
        }

        public void updateValue(int value) {
            this.value = value;
            setText("Jackpot: " + value + " Euros");
        }

        public void updateValue() {
            this.value = model.board.Jackpot.getValue();
            setText("Jackpot: " + value + " Euros");
        }
    }

    public class CardButton extends JButton {

        public CardButton(String image) {
            super(new ScalableImage(image, 145, 99));
            setPreferredSize(new Dimension(145, 99));
            setSize(145, 99);
            setEnabled(false);
        }

        public CardButton(String image, int x, int y) {
            this(image);
            setLocation(x, y);
        }
    }

    private TileGUI[] initTiles() {
        days = new TileGUI[32];

        days[0] = new TileGUI(controller.getBoard().getTile(0).getImage(), "Start");
        days[0].setLocation(0, TileGUI.TILE_HEIGHT);

        for (int i = 1; i < 32; i++)
            days[i] = new TileGUI(TileGUI.TILE_WIDTH * (i % 7), TileGUI.TILE_HEIGHT * ((i / 7) + 1),
                            controller.getBoard().getTile(i).getImage(),
                            controller.getBoard().getTile(i).getDay() + " " + i);

        return days;
    }
}