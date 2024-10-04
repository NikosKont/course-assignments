package view;

import javax.swing.JOptionPane;

public interface MonthsSelectionDialog {
    static final Integer[] monthsOptions = {1, 2, 3};

    /**
     * Gets the selection of months to play.
     *
     * @return user's selection
     */
    public static int getSelection() {
        try {
            return (int) JOptionPane.showInputDialog(null, "Select number of months to play:",
                                                            "Months Selection", JOptionPane.QUESTION_MESSAGE, null,
                                                            monthsOptions, 1);
        }
        catch (Exception e) {
            System.exit(1);
        }
        return 1;
    }
}