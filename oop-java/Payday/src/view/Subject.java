package view;

import java.awt.event.ActionListener;
import java.awt.event.ActionEvent;

public interface Subject {
    public void addListener (ActionListener l);
    public void removeListener (ActionListener l);
    public void notifyListeners(ActionEvent e);
}