package model.board;

import model.player.Player;

/**
 * A tile (position/date) on the board.
 */
public abstract class Tile {
    private int position;
    // private Days day;
    private String day;
    private String image;
    private boolean isThursday;
    private boolean isSunday;
    public static final String[] days = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "None"};

    /**
     * Constructor for a new tile.
     * @pre 1 {@literal <}{@literal =} position {@literal <}{@literal =} 31
     * @param position The position of the tile on the board.
     * @param day The day of the tile.
     * @param image The path for the image of the tile.
     */
    protected Tile(int position, String day, String image) {
        this.position = position;
        this.day = day;
        this.image = image;
        this.isThursday = (day.equals("Thursday"));
        this.isSunday = (day.equals("Sunday"));
    }

    protected Tile(int position, int day, String image) {
        this(position, days[day], image);
    }

    /**
     * Gets the position of the tile on the board.
     * 
     * @return The position of the tile on the board.
     */
    public int getPosition() {
        return position;
    }

    /**
     * Sets the position of the tile on the board.
     * @pre 1 {@literal <}{@literal =} position {@literal <}{@literal =} 31
     * @param position a valid position on the board.
     */
    public void setPosition(int position) {
        this.position = position;
    }

    /**
     * Gets the day of the tile.
     * 
     * @return The day of the tile.
     */
    public String getDay() {
        return day;
    }

    /**
     * Sets the day of the tile.
     * 
     * @param day valid day or "None".
     */
    public void setDay(String day) {
        this.day = day;
    }

    /**
     * Sets the day of the tile.
     * 
     * @pre 0 {@literal <}{@literal =} day {@literal <}{@literal =} 7
     * 
     * @param day number corresponding to desired day in days[].
     */
    public void setDay(int day) {
        this.day = days[day];
    }

    /**
     * Gets the image of the tile.
     * 
     * @return The image of the tile.
     */
    public String getImage() {
        return image;
    }

    /**
     * Performs the default action of the tile.
     * 
     * @param player The player to perform the action on.
     */
    public abstract void performAction(Player player);

    /**
     * Gets whether the tile is on Thursday.
     * 
     * @return True if the tile is on Thursday.
     */
    public boolean isThursday() {
        return isThursday;
    }

    /**
     * Gets whether the tile is on Sunday.
     * 
     * @return True if the tile is on Sunday.
     */
    public boolean isSunday() {
        return isSunday;
    }

    /**
     * Sets whether the tile is on Thursday.
     * 
     * @param isThursday True if the tile is on Thursday.
     */
    public void setThursday(boolean isThursday) {
        this.isThursday = isThursday;
    }

    /**
     * Sets whether the tile is on Sunday.
     * 
     * @param isSunday True if the tile is on Sunday.
     */
    public void setSunday(boolean isSunday) {
        this.isSunday = isSunday;
    }
}