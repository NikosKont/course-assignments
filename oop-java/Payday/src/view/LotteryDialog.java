package view;

import javax.swing.JOptionPane;
import model.player.Player;
import java.util.Random;

public interface LotteryDialog {
    public static int lotteryDialog(MainWindow parent, Player player, String[] options) {
        ScalableImage icon = new ScalableImage("resources/images/lottery.png", 100, 100);
        String input = (String)JOptionPane.showInputDialog(null, player.getName() + " select a dice value.", "Lottery", JOptionPane.NO_OPTION, icon, options, 1);
        int selection;
        try {
            selection = Integer.parseInt(input);
        }
        catch (NumberFormatException e) {
            selection = Integer.parseInt(options[new Random().nextInt(options.length)]);
        }
        return selection;
    }

    public static int lotteryDialog(MainWindow parent, Player player) {
        String[] options = new String[] {"1", "2", "3", "4", "5", "6"};
        return lotteryDialog(parent, player, options);
    }

    public static int lotteryDialog(MainWindow parent, Player player, int removeValue) {
        String[] options = new String[5];
        for (int i = 0, value = 1; i < options.length; i++, value++) {
            if (value == removeValue)
                value++;

            options[i] = Integer.toString(value);
        }
        return lotteryDialog(parent, player, options);
    }
}