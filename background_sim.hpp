#pragma once
#ifndef BACKGROUND_SIM_HPP
#define BACKGROUND_SIM_HPP

#include <raylib.h>

extern const int       width, height, size, color_count;
extern RenderTexture2D line_tex;

Color getColor(int index);

class background_sim {
  public:
    background_sim(int size = 100);
    ~background_sim();

    void reset();
    void update();
    void draw() const;

  private:
    int    falling_count;
    float *rotation;
    float *rot_vel;
    float *pos_y;
    float *vel_y;
    int   *pos_x;

    Rectangle whole_texture_rect;
    Vector2   texture_middle;
};

#endif