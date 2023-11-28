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
          y_arr[2] = -2;
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
      color_index = color_index_arg;
    }

    // Set the rotation of the block
    void set_rot(int rot_index_arg) {
      rot_index = rot_index_arg;
      for (int i = 0; i < rot_index; i++) {
        rotate();
      }
    }

    // Getters
    // Get the y-coordinate of the block
    int get_y() const{
      return y;
    }

    // Get the status of the block, check if any block is falling
    bool is_falling() {
      return falling;
    }

    // Reset the block
    void reset() {
      type = random(1, 5);
      color_index = random(1, 10);
      rot_index = random(1, 4);
      y = random(0, MAT_HEIGHT);
      x = MAT_WIDTH - 1;
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
      // MAT_WIDTH = 32, MAT_HEIGHT = 16
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
    // Mark the rotation of the block
    int rot_index;
    // Check if the block is falling
    bool falling;

    // Check if the block is in the screen
    bool x_in_range(int x_arr_arg[4]) {
      for (int i = 0; i < 4; i++) {
        if ((x_arr_arg[i] >= MAT_WIDTH) || (x_arr_arg[i] < 0)) {
          return false;
        }
      }
      return true;
    }

    bool y_in_range(int y_arr_arg[4]) {
      for (int i = 0; i < 4; i++) {
        if ((y_arr_arg[i] >= MAT_HEIGHT) || (y_arr_arg[i] < 0)) {
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

      // Clear the blocks on the screen
      matrix.fillScreen(BLACK.to_333());

      // Reset max_height recording the maximum height of each column
      for (int i = 0; i < MAT_HEIGHT; i++) {
        max_height[MAT_HEIGHT] = 0;
      }
    }
    
    // Modifies: global variable matrix
    void update(int potentiometer_value, bool button_pressed) {
      

    }

  private:
    unsigned long time;
    Block block;
    int max_height[MAT_HEIGHT];
};

// a global variable that represents the game Tetris
Game game;

// see https://www.arduino.cc/reference/en/language/structure/sketch/setup/
void setup() {
  Serial.begin(9600);
  pinMode(BUTTON_PIN_NUMBER, INPUT);
  matrix.begin();
}

// see https://www.arduino.cc/reference/en/language/structure/sketch/loop/
void loop() {
  int potentiometer_value = analogRead(POTENTIOMETER_PIN_NUMBER);
  bool button_pressed = (digitalRead(BUTTON_PIN_NUMBER) == HIGH);

  game.update(potentiometer_value, button_pressed);
}


// displays "game over"
void game_over() {
}

