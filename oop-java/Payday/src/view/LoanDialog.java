package view;

import javax.swing.JOptionPane;

/**
 * Loan dialog window.
 */
public interface LoanDialog {
    public static int loanDialog(String message, String errorMessage, boolean cancelable, int min) {
        try {
            int loan = Integer.parseInt(JOptionPane.showInputDialog(message));
            if (loan < 1000 || loan % 1000 != 0 || loan < min) {
                throw new IllegalArgumentException("Invalid loan amount.");
            }
            return loan;
        }
        catch (IllegalArgumentException e) {
            ErrorMessage.showMessage(errorMessage);
            if (cancelable) {
                return 0;
            }
            else {
                return loanDialog(message, errorMessage, cancelable, min);
            }
        }
    }

    public static int getLoanDialog(boolean cancelable) {
        return loanDialog("Enter loan amount to get (multiple of 1000):", "Could not get loan", cancelable, 0);
    }

    public static int getLoanDialog(boolean cancelable, int min) {
        return loanDialog("Enter loan amount to get (multiple of 1000, at least " + min + "):", "Could not get loan", cancelable, min);
    }

    public static int payLoanDialog(boolean cancelable) {
        return loanDialog("Enter loan amount to pay off (multiple of 1000):", "Could not pay off loan", cancelable, 0);
    }
}