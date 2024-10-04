package view;

import javax.swing.JOptionPane;

public class JackpotDialog {
    public JackpotDialog(MainWindow parent) {
        JOptionPane.showMessageDialog(null, "You won the Jackpot!", "Jackpot", JOptionPane.INFORMATION_MESSAGE, new ScalableImage("resources/images/jackpot.png"));
    }
}