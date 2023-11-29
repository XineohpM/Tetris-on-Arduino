#include <gamma.h>
#include <RGBmatrixPanel.h>
#include <Adafruit_GFX.h>

// define the wiring of the LED screen
const uint8_t CLK  = 8;
const uint8_t LAT = A3;
const uint8_t OE = 9;
const uint8_t A = A0;
const uint8_t B = A1;
const uint8_t C = A2;

// define the wiring of the inputs
const int POTENTIOMETER_PIN_NUMBER = 5;
const int BUTTON_PIN_NUMBER = 10;

// Matrix Dimensions
const uint8_t MAT_WIDTH = 32;
const uint8_t MAT_HEIGHT = 16;

// a global variable that represents the LED screen
RGBmatrixPanel matrix(A, B, C, CLK, LAT, OE, false);

// the following functions are for printing messages
void game_over();
void game_start();
void print_restart();
void choose_restart();
void print_quit();
void choose_quit();

class Color {
  public:
    int red;
    int green;
    int blue;
    Color() {
      red = 0;
      green = 0;
      blue = 0;
    }
    Color(int r, int g, int b) {
      red = r;
      green = g;
      blue = b;
    }
    uint16_t to_333() const {
      return matrix.Color333(red, green, blue);
    }
};

const Color BLACK(0, 0, 0);
const Color RED(4, 0, 0);
const Color ORANGE(6, 1, 0);
const Color YELLOW(4, 4, 0);
const Color GREEN(0, 4, 0);
const Color BLUE(0, 0, 4);
const Color PURPLE(1, 0, 2);
const Color WHITE(4, 4, 4);
const Color LIME(2, 4, 0);
const Color AQUA(0, 4, 4);

const Color colors[9] = {BLACK, RED, ORANGE, YELLOW, LIME, GREEN, AQUA, BLUE, PURPLE};

/*  (31, 0)--------(31, 15)
 *     |               |
 *     |               |
 *     |               |
 *     |               |
 *     |               |
 *     |               |
 *     |               |
 *     |               |
 *  (0, 0)----------(0, 15)
 */


class Block {
  public:
  // Default constructor
    Block() {
      x = 31;
      y = 0;
      for (int i = 0; i < 4; i++) {
        x_arr[i] = 0;
        y_arr[i] = 0;
      }
      type = 0;
      color_index = 0;
      rot_index = 0;
      falling = false;
    }

    // Setters
    // Set the y-coordinate of the block
    void set_y(int y_arg) {
      if ((y_arg >= 0) && (y_arg < MAT_HEIGHT)) {
        y = y_arg;
      }
      else {
        Serial.print("Illegal value for y!");
      }
    }

    // Set the type of the block
    void set_type(int type_arg) {
      type = type_arg;
      switch (type) {
        // O_Block
        case 1:
          x_arr[0] = 0;
          x_arr[1] = 0;
          x_arr[2] = -1;
          x_arr[3] = -1;
          y_arr[0] = 0;
          y_arr[1] = 1;
          y_arr[2] = 0;
          y_arr[3] = 1;
          break;
        // I_Block
        case 2:
          x_arr[0] = 0;
          x_arr[1] = -1;
          x_arr[2] = -2;
          x_arr[3] = -3;
          y_arr[0] = 0;
          y_arr[1] = 0;
          y_arr[2] = 0;
          y_arr[3] = 0;
          break;
        // L_Block
        case 3:
          x_arr[0] = 0;
          x_arr[1] = -1;
          x_arr[2] = -2;
          x_arr[3] = -2;
          y_arr[0] = 0;
          y_arr[1] = 0;
          y_arr[2] = 0;
          y_arr[3] = -1;
          break;
        // T_Block
        case 4:
          x_arr[0] = 0;
          x_arr[1] = -1;
          x_arr[2] = -2;
          x_arr[3] = -1;
          y_arr[0] = 0;
          y_arr[1] = 0;
          y_arr[2] = 0;
          y_arr[3] = -1;
          break;
        // Z_Block
        case 5:
          x_arr[0] = 0;
          x_arr[1] = 0;
          x_arr[2] = -1;
          x_arr[3] = -1;
          y_arr[0] = 0;
          y_arr[1] = -1;
          y_arr[2] = -1;
          y_arr[3] = -2;
          break;
        // Other cases
        default:
          for (int i = 0; i < 4; i++) {
            x_arr[i] = 0;
            y_arr[i] = 0;
          }
          break;
      }
    }

    // Set the color of the block
    void set_color(int color_index_arg) {
      if ((color_index_arg >= 0) && (color_index_arg < 9)) {
        color_index = color_index_arg;
      }
    }

    // Set the rotation of the block
    void set_rot(int rot_index_arg) {
      if ((rot_index_arg >= 0) && (rot_index_arg < 4)) {
        while (rot_index <= rot_index_arg) {
          rotate();
        }
      }
    }

    // Stop the block falling
    void stop() {
      falling = false;
    }

    // Getters
    // Get the y-coordinate of the block
    int get_y() const{
      return y;
    }

    // Get the x-coordinate of the block
    int get_x() const{
      return x;
    }

    // Get the status of the block, check if any block is falling
    bool is_falling() const{
      return falling;
    }

    // Get the color index of the block
    int get_color() const{
      return color_index;
    }

    // Get the rotation index of the block
    int get_rot() const{
      return rot_index;
    }

    // Get x_arr of the block
    void get_x_arr(int x_arr_arg[4]) {
      for (int i = 0; i < 4; i++) {
        x_arr_arg[i] = x_arr[i];
      }
    }

    // Get y_arr of the block
    void get_y_arr(int y_arr_arg[4]) {
      for (int i = 0; i < 4; i++) {
        y_arr_arg[i] = y_arr[i];
      }
    }

    // Reset the block
    void reset() {
      x = MAT_WIDTH - 1;
      y = 0;
      type = 0;
      color_index = 0;
      rot_index = 0;
      falling = false;
      
      // 1-O, 2-I, 3-L, 4-T, 5-Z
      set_type(random(1, 6));
      // colors[9] = {BLACK, RED, ORANGE, YELLOW, LIME, GREEN, AQUA, BLUE, PURPLE}
      // 1, 2, 3, 4, 5, 6, 7, 8
      set_color(random(1, 9));
      // 0, 1, 2, 3
      set_rot(random(0, 4));
      set_y(random(0, MAT_HEIGHT));
      falling = true;
    }

    // Draw the block
    void draw() {
      if (falling) {
        draw_with_rgb(colors[color_index]);
      }
    }

    // Erase the block
    void erase() {
      set_color(0);
      draw_with_rgb(colors[color_index]);
    }

    // Move the block
    void fall() {
      if (x_in_range(x_arr) && y_in_range(y_arr)) {
        erase();
        x--;
        draw();
      }
      else {
        reset();
      }
    }

    // Rotate the block
    void rotate() {
      if (falling) {
        int temp_x_arr[4];
        int temp_y_arr[4];
        for (int i = 0; i < 4; i++) {
          temp_x_arr[i] = 0 - y_arr[i];
          temp_y_arr[i] = x_arr[i];
        }
        // Rotate only if the block after rotation is in the screen
        if (x_in_range(temp_x_arr) && y_in_range(temp_y_arr)) {
          for (int i = 0; i < 4; i++) {
            x_arr[i] = temp_x_arr[i];
            y_arr[i] = temp_y_arr[i];
          }
          rot_index++;
          rot_index %= 4;
        }
      }
    }


  private:
    int x;
    int y;
    // Relative coordinates of all 4 pixels of the block
    int x_arr[4];
    int y_arr[4];
    // Mark the type of the block
    int type;
    // Mark the color of the block
    int color_index;
    // Mark the rotation of the block, 0 <= rot_index < 3
    int rot_index;
    // Check if the block is falling
    bool falling;

    // Check if the block is in the screen
    bool x_in_range(int x_arr_arg[4]) {
      for (int i = 0; i < 4; i++) {
        if ((x + x_arr_arg[i] >= MAT_WIDTH) || (x + x_arr_arg[i] < 0)) {
          return false;
        }
      }
      return true;
    }

    bool y_in_range(int y_arr_arg[4]) {
      for (int i = 0; i < 4; i++) {
        if ((y + y_arr_arg[i] >= MAT_HEIGHT) || (y + y_arr_arg[i] < 0)) {
          return false;
        }
      }
      return true;
    }

    // Draw the pixel with given color
    void draw_with_rgb(Color color) {
      for (int i = 0; i < 4; i++) {
        matrix.drawPixel(x + x_arr[i], y + y_arr[i], color.to_333());
      }
    }
};




class Game {
  public:
    Game() {
      time = 0;
    }
    
    // sets up a new game of Tetris
    // Modifies: global variable matrix
    void setupGame() {
      // Initialize time
      time = 0;

      // Generate a random number seed
      randomSeed(millis());

      // Clear picture
      clear_picture();

      // Clear the blocks on the screen
      matrix.fillScreen(BLACK.to_333());

      // Display "game start"
      game_start();

      delay(2000);

      // Create the first block
      block.reset();
    }
    
    // Modifies: global variable matrix
    void update(int potentiometer_value, bool button_pressed) {
      // Draw picture
      draw_picture();

      // Check for filled columns, if there is any column filled, then game over
      for (int i = 0; i < MAT_HEIGHT; i++) {
        // Game over when touching the top of the screen
        if (is_touching_top()) {
          // Print "game over"
          game_over();
          // Wait 3 seconds
          delay(3000);
          // Fill Screen with black
          matrix.fillScreen(BLACK.to_333());
          // Set seletion menu to be active
          menu_active = true;
        }
      }

      // Display selection menu if menu is active
      if (menu_active == true){
          draw_cursor(potentiometer_value);
          if (button_pressed) {
            // Restart selected
            if (potentiometer_value < 512) {
                setupGame(); 
                menu_active = false;
                return;
            } 
            else {
              // Quit selected, enter an empty loop and pause
              matrix.fillScreen(BLACK.to_333());
              while (true) {}
            }
          }
        // Slightly delay to prevent button jitter
        delay(100); 
      }
      // Play the game as normal if menu is not active
      else {
        // Operate block when there is block falling
        if (block.is_falling()) {
          // Move block using value of the potentiometer
          block.erase();
          block.set_y(((MAT_HEIGHT) * potentiometer_value) / 1024);
          Serial.print(block.get_y());
          block.draw();

          // Rotate block using button
          if (button_pressed) {
            block.rotate();
          }

          // Block falls every 20 loops, with an initial delay of 100 loops
          if ((time % 20 == 0) && (time > 100)) {
            block.fall();
          }

          // Check if block touching the existing fallen blocks
          if (is_touching(block)) {
            // Stop the block falling
            block.stop();

            // Add the block to picture
            pic_add_block(block);

            // Reset a new block
            block.reset();
          }
        }
        // No block is falling
        else {
          // Reset a new block
          block.reset();
        }

        // Check and delete filled rows 
        for (int i = 0; i < MAT_WIDTH; i++) {
          if (is_row_filled(i)) {
            pic_del_row(i);
          }
        }
      }
      time++;
    }

  private:
    unsigned long time;
    // The picture formed by stack of fallen blocks, this array stores the color index of each pixel
    int picture[MAT_WIDTH][MAT_HEIGHT];

    Block block;

    bool menu_active = false;

    // Draw the cursor for selecting Restart or Quit
    void draw_cursor(int potentiometer_value_arg) {
      if (potentiometer_value_arg < 512) {
        // Cursor selected Restart
        choose_restart();
        print_quit();
      }
      else {
        // Cursor selected Quit
        choose_quit();
        print_restart();
      }
    }

    // Draw the picture formed by stack of fallen blocks
    void draw_picture() {
      for (int i = 0; i < MAT_WIDTH; i++) {
        for (int j = 0; j < MAT_HEIGHT; j++) {
          // Draw the pixel only when the color is not black
          if (picture[i][j] != 0) {
            Color pixel_color = colors[picture[i][j]];
            matrix.drawPixel(i, j, pixel_color.to_333());
          }
        }
      }
    }

    // Clear the picture formed by stack of fallen blocks
    void clear_picture() {
      for (int i = 0; i < MAT_WIDTH; i++) {
        for (int j = 0; j < MAT_HEIGHT; j++) {
          picture[i][j] = 0;
        }
      }
    }

    // Add a block to picture
    void pic_add_block(Block block_arg) {
      // Get the (x, y) coordinate of the block
      int x = block_arg.get_x();
      int y = block_arg.get_y();
      // Get the relative coordinates of all other pixels of the block
      int x_arr_arg[4] = {};
      int y_arr_arg[4] = {};
      block_arg.get_x_arr(x_arr_arg);
      block_arg.get_y_arr(y_arr_arg);
      // Add the block coordinates and the corresponding color into picture
      for (int i = 0; i < 4; i++) {
        picture[x + x_arr_arg[i]][y + y_arr_arg[i]] = block_arg.get_color();
      }
    }

    // Check if a row is filled
    bool is_row_filled(int row) {
      for (int i = 0; i < MAT_HEIGHT; i++) {
        if (picture[row][i] == 0) {
          return false;
        }
      }
      return true;
    }

    // Check if any block reach the top of the screen
    bool is_touching_top() {
      for (int i = 0; i < MAT_HEIGHT; i++) {
        if (picture[31][i] != 0) {
          return true;
        }
      }
      return false;
    }

    // Delete a filled row from picture and update
    void pic_del_row(int row) {
      if (is_row_filled(row)) {
        for (int j = row; j < MAT_WIDTH - 1; j++) {
          for (int i = 0; i < MAT_HEIGHT; i++) {
            // Move contents of the row above to the current row
            picture[j][i] = picture[j + 1][i];
          }
        }
        // Clear the top row
        for (int i = 0; i < MAT_HEIGHT; i++) {
          picture[MAT_WIDTH - 1][i] = 0;
        }
      }
    }
    
    // Check if any block touches existing fallen blocks or the bottom of the screen
    bool is_touching(Block block_arg) {
      // Get the (x, y) coordinate of the block
      int x = block_arg.get_x();
      int y = block_arg.get_y();
      // Get the relative coordinates of all other pixels of the block
      int x_arr_arg[4] = {};
      int y_arr_arg[4] = {};
      block_arg.get_x_arr(x_arr_arg);
      block_arg.get_y_arr(y_arr_arg);
      for (int i = 0; i < 4; i++) {
        int check_x = x + x_arr_arg[i] - 1;
        // Touch the bottom
        if (check_x < 0) {
          return true;
        }
        int check_y = y + y_arr_arg[i];
        if ((check_x >= 0) && (check_x < MAT_WIDTH) && (check_y >= 0) && (check_y < MAT_HEIGHT)) {
          // Check if the space directly beneath each block pixel is occupied
          if (picture[check_x][check_y]) {
            return true;
          }
        }
      }
      return false;
    }
};

// a global variable that represents the game Tetris
Game game;

// see https://www.arduino.cc/reference/en/language/structure/sketch/setup/
void setup() {
  Serial.begin(9600);
  pinMode(BUTTON_PIN_NUMBER, INPUT);
  matrix.begin();
  game.setupGame();
}

// see https://www.arduino.cc/reference/en/language/structure/sketch/loop/
void loop() {
  // Smooth the readings from an analog input
  int total = 0;
  int num_readings = 10;
  for (int i = 0; i < num_readings; i++) {
    total += analogRead(POTENTIOMETER_PIN_NUMBER);
  }  
  int potentiometer_value = total / num_readings;

  bool button_pressed = (digitalRead(BUTTON_PIN_NUMBER) == HIGH);

  game.update(potentiometer_value, button_pressed);
  
  delay(25);
}

// displays "game over"
void game_over() {
  matrix.fillScreen(BLACK.to_333());
  matrix.setTextSize(1);
  matrix.setTextColor(RED.to_333());
  matrix.setCursor(0, 0); 
  matrix.print('G');
  matrix.print('A');
  matrix.print('M');
  matrix.print('E');
  matrix.print(' ');
  matrix.print('O');
  matrix.print('V');
  matrix.print('E');
  matrix.print('R');
  matrix.print('!');
}

// displays "game start"
void game_start() {
  matrix.fillScreen(BLACK.to_333());
  matrix.setTextSize(1);
  matrix.setTextColor(GREEN.to_333());
  matrix.setCursor(0, 0); 
  matrix.print('G');
  matrix.print('A');
  matrix.print('M');
  matrix.print('E');
  matrix.print(' ');
  matrix.print('S');
  matrix.print('T');
  matrix.print('A');
  matrix.print('R');
  matrix.print('T');
}

// Display "R" in white for Restart
void print_restart() {
  matrix.setTextColor(WHITE.to_333());
  matrix.setCursor(0, 0);
  matrix.print("R");
}

// Display "R" in yellow for user to select Restart
void choose_restart() {
  matrix.setTextColor(YELLOW.to_333());
  matrix.setCursor(0, 0);
  matrix.print("R");
}

// Display "Q" in white for Quit
void print_quit() {
  matrix.setTextColor(WHITE.to_333());
  matrix.setCursor(0, 8);
  matrix.print("Q");
}

// Display "Q" in yellow for user to select Quit
void choose_quit() {
  matrix.setTextColor(YELLOW.to_333());
  matrix.setCursor(0, 8);
  matrix.print("Q");
}

