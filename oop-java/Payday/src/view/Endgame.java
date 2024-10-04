package view;

import javax.swing.*;
import model.player.Player;

/**
 * Endgame window.
 */
public class Endgame {

    public static void showEndgame(Player player) {
        JOptionPane.showMessageDialog(null, player.getName() + "wins!", "Endgame", JOptionPane.INFORMATION_MESSAGE);
    }
}