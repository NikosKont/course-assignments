package view;

import javax.swing.JOptionPane;

public interface Thursday {
    public static int getSelection() {
        ScalableImage image = new ScalableImage("resources/images/litecoin.png", 300, 200);
        String options[] = {"Αγόρασε", "Αγνόησε"};
        return JOptionPane.showOptionDialog(null, "Αγόρασε 3.12 LTC (300 ευρώ)", "Buy Litecoin",
                    JOptionPane.DEFAULT_OPTION, JOptionPane.QUESTION_MESSAGE, image, options, options[0]);
    }

    static void showRise() {
        ScalableImage image = new ScalableImage("resources/images/rise.png", 68, 250);
        JOptionPane.showConfirmDialog(null, "Η αξία ανέβηκε. Κέρδισες 2400 ευρώ.", "Litecoin",
                    JOptionPane.DEFAULT_OPTION, JOptionPane.INFORMATION_MESSAGE, image);
    }

    static void showFall() {
        ScalableImage image = new ScalableImage("resources/images/fall.png", 68, 250);
        JOptionPane.showConfirmDialog(null, "Η αξία έπεσε. Έχασες τα λεφτά σου.", "Litecoin",
                    JOptionPane.DEFAULT_OPTION, JOptionPane.INFORMATION_MESSAGE, image);
    }
}