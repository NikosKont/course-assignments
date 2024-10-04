package model.board;

/**
 * Jackpot position.
 */
public class Jackpot {
    private static int value;
    private static String image;

    /**
     * Constructs a new Jackpot position.
     */
    public Jackpot() {
        value = 0;
    }

    /**
     * Gets the value of the Jackpot.
     * 
     * @return The value of the Jackpot.
     */
    public static int getValue() {
        return value;
    }

    /**
     * Adds to value of the Jackpot.
     * 
     * @param value The value to add to the Jackpot.
     */
    public static void add(int value) {
        Jackpot.value += value;
    }

    /**
     * Removes from value of the Jackpot.
     * 
     * @param value The value to remove from the Jackpot.
     */
    public static void remove(int value) {
        Jackpot.value -= value;
    }

    /**
     * Resets the value of the Jackpot.
     */
    public static void reset() {
        value = 0;
    }

    /**
     * Gets the image of the Jackpot.
     * 
     * @return The image of the Jackpot.
     */
    public static String getImage() {
        return image;
    }

    /**
     * Sets the image of the Jackpot.
     * 
     * @param image The image of the Jackpot.
     */
    public static void setImage(String image) {
        Jackpot.image = image;
    }
}
