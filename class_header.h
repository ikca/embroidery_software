// A Ikic 2020

class PixelData
{
  private:
    uint8_t blue, green, red; // referred to as BGR
    bool goneThru;
    int32_t count;

  public:
  PixelData()
    {
      blue = 0;
      green = 0;
      red = 0;
      goneThru = 0;
      count = 0;
    }

    uint8_t get_blue() {return blue;};
    uint8_t get_green() {return green;};
    uint8_t get_red() {return red;};
    uint8_t get_goneThru() {return goneThru;};
    int32_t get_count(){return count;};

    void set_blueNumber(uint8_t number){ blue = number;};
    void set_greenNumber(uint8_t number){ green = number;};
    void set_redNumber(uint8_t number){ red = number;};
    void set_goneThru(bool t_or_f) {goneThru = t_or_f;};
    void set_countNum(int32_t number){count = number;};

    void set_BGR(char buffer[4])
    {
      blue = (uint8_t) buffer[0];
      green = (uint8_t) buffer[1];
      red = (uint8_t) buffer[2];
    }

    void count_plusOne() {count += 1;};
};

class Pocket
{
  private:
    int32_t startX, endX, lineY;
    bool goneThru;

  public:
  Pocket()
  {
    startX = 0;
    endX = 0;
    lineY = 0;
    goneThru = 0;
  }

  int32_t get_startX() {return startX;};
  int32_t get_endX() {return endX;};
  int32_t get_lineY() {return lineY;};

  bool get_goneThru() {return goneThru;};

  void set_startX(int32_t number) {startX = number;};
  void set_endX(int32_t number) {endX = number;};
  void set_lineY(int32_t number) {lineY = number;};

  void set_goneThru(bool t_or_f) {goneThru = t_or_f;};
};
