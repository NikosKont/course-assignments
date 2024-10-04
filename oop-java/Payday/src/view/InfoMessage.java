package view;

import javax.swing.JOptionPane;

/**
 * An error message.
 */
public interface InfoMessage {
    /**
     * Shows an error message.
     *
     * @param message The message to show.
     */
    public static void showMessage(String message) {
        JOptionPane.showMessageDialog(null, message, "Message", JOptionPane.INFORMATION_MESSAGE);
    }
}