
#include <background_sim.hpp>
#include <stdlib.h>
#include <time.h>

background_sim::background_sim(int size) {
    falling_count = size;
    rotation = new float[size];
    rot_vel = new float[size];
    pos_y = new float[size];
    vel_y = new float[size];
    pos_x = new int[size];

    // Calculate the structs needed for drawing once
    whole_texture_rect = {0, 0, (float)line_tex.texture.width,
                          (float)line_tex.texture.height};
    texture_middle = {whole_texture_rect.width / 2, whole_texture_rect.height / 2};

    reset();
}

background_sim::~background_sim() {
    delete[] rotation;
    delete[] rot_vel;
    delete[] pos_y;
    delete[] vel_y;
    delete[] pos_x;
}

void background_sim::reset() {
    for (int i = 0; i < falling_count; i++) {
        pos_x[i] = rand() % width;
        pos_y[i] = -(rand() % height);
        vel_y[i] = (rand() % 100) / 100.f + 1.f;
        // rot_vel[i] = ((rand() % 360) - 180.) / 360.;
        rot_vel[i] = ((rand() % 2) * 2 - 1) * ((rand() % 180) + 180) / 360.f;
        rotation[i] = ((rand() % 360) - 180.) / 360.;
    }
    // Now simulate 1000 updates to make it appear more natural
    for (int step = 0; step < 1000; step++)
        update();
}

void background_sim::update() {
    for (int i = 0; i < falling_count; i++) {
        pos_y[i] += vel_y[i];
        // Loop around the screen if it's gone too far down
        if (pos_y[i] > height + size * 4) {
            pos_x[i] = rand() % width;
            pos_y[i] = -(rand() % 100) - 100;
        }
        rotation[i] += rot_vel[i];
    }
}

void background_sim::draw() const {
    // Setup a rectangle to reuse as destination
    Rectangle destination = {0, 0, (float)line_tex.texture.width,
                             (float)line_tex.texture.height};

    for (int i = 0; i < falling_count; i++) {
        Color target_color = getColor(i % color_count + 1);
        target_color = Fade(target_color, 0.1f);
        destination.x = pos_x[i];
        destination.y = pos_y[i];
        DrawTexturePro(line_tex.texture, whole_texture_rect, destination, texture_middle,
                       rotation[i], target_color);
    }
}