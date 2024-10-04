package view;

import javax.swing.ImageIcon;

public class ScalableImage extends ImageIcon {
    public ScalableImage(String path) {
        super(path);
    }

    public ScalableImage(ImageIcon icon) {
        super(icon.getImage());
    }

    public ScalableImage(ImageIcon icon, int width, int height) {
        super(icon.getImage());
        scale(width, height);
    }

    public ScalableImage(String path, int width, int height) {
        super(path);
        scale(width, height);
    }

    public ImageIcon getScaled(int width, int height) {
        return new ImageIcon(getImage().getScaledInstance(width, height, 4));
    }

    public ImageIcon getScaled(int width, int height, int hints) {
        return new ImageIcon(getImage().getScaledInstance(width, height, hints));
    }

    public void scale(int width, int height, int hints) {
        this.setImage(this.getImage().getScaledInstance(width, height, hints));
    }

    public void scale(int width, int height) {
        this.setImage(this.getImage().getScaledInstance(width, height, 4));
    }
}