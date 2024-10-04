package view;

import javax.swing.JOptionPane;

public interface Sunday {
    public static int getSelection() {
        ScalableImage image = new ScalableImage("resources/images/arsenal_chelsea.jpg", 300, 200);
        String options[] = {"1 (2.37)","X (3.40)","2 (2.10)","Είμαι Χάστης"};
        return JOptionPane.showOptionDialog(null, "Ρίξε 500 ευρώ", "London Derby",
                    JOptionPane.DEFAULT_OPTION, JOptionPane.QUESTION_MESSAGE, image, options, options[0]);
    }

    public static void showResult(int selection) {
        String result;
        if (selection == 0)
            result = "Κέρδισες 1185 ευρώ.";
        else
            result = "Έχασες.";

        ScalableImage image = new ScalableImage("resources/images/arsenal.jpg", 300, 160);
        JOptionPane.showConfirmDialog(null, "Νίκη Arsenal. " + result, "London Derby",
                    JOptionPane.DEFAULT_OPTION, JOptionPane.INFORMATION_MESSAGE, image);
    }
}