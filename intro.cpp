
#include <math.h>
#include <raylib.h>
#include <stdio.h>

extern const int width, height;

float lerp(float start, float end, float progress) {
    progress = fmin(fmax(progress, 0.), 1.);
    return start * (1. - progress) + end * progress;
}

void intro() {
    // Load the sound to play
    Sound s = LoadSound("rsc/Startup.mp3");

    Texture t = LoadTexture("rsc/shitari.png");

    // Start off with a white screen
    BeginDrawing();
    ClearBackground(RAYWHITE);
    EndDrawing();
    BeginDrawing();
    ClearBackground(RAYWHITE);
    EndDrawing();

    // Wait a few frames
    for (int i = 0; i < 30; i++) {
        BeginDrawing();
        EndDrawing();
    }

    // Start the sound
    PlaySound(s);

    // Wait for the sound to finish playing
    const int sz = 20;
    for (int frame = 0; frame < 480; frame++) {
        BeginDrawing();

        DrawRectangle(0, 0, width, height, {255, 255, 255, 10});

        unsigned char fade_in = (unsigned char)lerp(0, 255, (frame - 240) / 60.f);
        DrawTexture(t, (width - t.width) / 2, height / 2 - 200, {255, 255, 255, fade_in});

        DrawRectangle(lerp(-sz, 200, (frame - 60) / 60.), height / 2, sz, sz, BLUE);
        DrawRectangle(lerp(-sz, 400, (frame - 90) / 90.), height / 2, sz, sz, YELLOW);
        DrawRectangle(lerp(-sz, 500, (frame - 120) / 120.), height / 2, sz, sz, GREEN);
        DrawRectangle(lerp(-sz, 600, (frame - 150) / 150.), height / 2, sz, sz, RED);

        EndDrawing();
    }

    // Do a fade to black
    for (int i = 0; i < 90; i++) {
        unsigned char colo = (unsigned char)(255. * (90. - i) / 90.);
        BeginDrawing();
        ClearBackground({colo, colo, colo, 255});
        EndDrawing();
    }

    // Wait a few more frames
    for (int i = 0; i < 10; i++) {
        BeginDrawing();
        EndDrawing();
    }

    UnloadSound(s);
    UnloadTexture(t);
}
