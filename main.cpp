
#include <iostream>
#include <raylib.h>
#include <stdlib.h>
#include <time.h>

#include <math.h>

#include <background_sim.hpp>

#define GAMEOVER 2

const int width = 800, height = 600;
const int bwidth = 10, bheight = 21;

const int spacing = 2;
const int size = 20;
const int offset = size + spacing;

const int difficulty_scalar = 1;
const int original_difficulty = 19;

char board[bwidth * bheight];

RenderTexture2D game_buffer;
RenderTexture2D board_tex;
RenderTexture2D line_tex;

Texture cat;

Sound clear;
Sound not_clear;
Sound background_music;

// Color     piece_colors[] = {GRAY, GREEN, BLUE, RED};
Color piece_colors[] = {GRAY,
                        {0x5C, 0xE6, 0x95, 255},
                        {0xE6, 0xCB, 0x45, 255},
                        {0x35, 0x2E, 0xE6, 255},
                        {0xCF, 0x56, 0xE6, 255},
                        {0x5C, 0xC6, 0xE6, 255},
                        {0xE6, 0x45, 0x39, 255}};

const int color_count = 6;
int       last_color;

void intro();

Color getColor(int index) { return piece_colors[index]; }

void draw_board() {
    BeginTextureMode(board_tex);
    ClearBackground({255, 255, 255, 0});
    for (int x = 0; x < bwidth; x++) {
        for (int y = 0; y < bheight; y++) {
            char  piece = board[x + y * bwidth];
            Color c = piece_colors[(size_t)piece];
            DrawRectangle(x * offset, y * offset, size, size, c);
        }
    }
    EndTextureMode();
}

char GetBoardAtPos(int x, int y) { return board[x + y * bwidth]; }

int MovePieceDown(int &piecex, int &piecey, char &placing_piece, bool &cleared_line) {
    cleared_line = false;
    if (piecey == 0 || GetBoardAtPos(piecex, piecey - 1)) {
        if (piecey == bheight - 1) {
            // Game over!
            return GAMEOVER;
        } else {
            // printf("piecey: %d, bheight: %d\n", piecey, bheight);
            // Place the piece
            board[piecex + piecey * bwidth] = placing_piece;
            board[piecex + (piecey + 1) * bwidth] = placing_piece;
            board[piecex + (piecey + 2) * bwidth] = placing_piece;
            board[piecex + (piecey + 3) * bwidth] = placing_piece;
            // Check to see if we can eliminate
            for (int y = 0; y < bheight;) {
                bool flag = true;
                for (int x = 0; x < bwidth; x++) {
                    if (!GetBoardAtPos(x, piecey)) {
                        flag = false;
                        break;
                    }
                }
                if (flag) {
                    cleared_line = true;
                    // We can remove the line.
                    // Move all the above lines down
                    for (int y2 = y + 1; y2 < bheight; y2++) {
                        for (int x2 = 0; x2 < bwidth; x2++) {
                            board[x2 + (y2 - 1) * bwidth] = board[x2 + y2 * bwidth];
                        }
                    }
                    for (int x2 = 0; x2 < bwidth; x2++) {
                        board[x2 + (bheight - 1) * bwidth] = 0;
                    }
                } else
                    y++;
            }
        }
        // Need to update the board too
        draw_board();

        // Reset the piece
        piecex = bwidth / 2;
        piecey = bheight - 1;
        // char old_piece = placing_piece;
        // while (old_piece == placing_piece)
        //     placing_piece = rand() % color_count + 1;
        placing_piece = last_color;
        last_color = (last_color % color_count) + 1;
        return 1;
    } else {
        piecey--;
    }
    return 0;
}

int ShowGameOver() {
    const Rectangle game_buffer_src = {0, 0, width, -height};
    const Rectangle game_buffer_dst = {0, 0, width, height};
    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_DOWN)) {
            printf("Replaying\n");
            return 0;
        }
        BeginDrawing();
        DrawTexturePro(game_buffer.texture, game_buffer_src, game_buffer_dst, {0, 0}, 0,
                       WHITE);
        DrawText("Gameover!\nPress down to play again\nPress escape to quit",
                 width / 2 - 90, height / 2, 16, WHITE);
        EndDrawing();
    }
    return -1;
}

int play_game(bool first_time) {

    int board_width = bwidth * size + (bwidth - 1) * spacing;
    int board_height = bheight * size + (bheight - 1) * spacing;

    // Clear the board
    for (int i = 0; i < bwidth * bheight; i++)
        board[i] = 0;

    // Draw the initial board
    draw_board();

    // Where is the piece we are dropping
    int piecex = bwidth / 2;
    int piecey = bheight - 1;
    last_color = 1;
    char placing_piece = color_count;

    // How fast does it drop
    int current_cooldown_to_piece_drop = original_difficulty;
    int cooldown_to_piece_drop = original_difficulty;

    // Score and multipliers
    int  score = 0;
    bool back_to_back = false;
    bool cleared_line = false;
    int  lines_cleared = 0;

    // Buffer for text messages
    char msg[80];

    // Create our background simulation!
    background_sim background(100);

    const Rectangle game_buffer_src = {0, 0, width, -height};
    const Rectangle game_buffer_dst = {0, 0, width, height};

    int frames = 0;
    if (!first_time)
        frames += 200;

    bool started = false;

    // Main game loop
    while (!WindowShouldClose()) {

        if (!IsSoundPlaying(background_music))
            PlaySound(background_music);

        background.update();

        if (IsKeyPressed(KEY_LEFT)) {
            started = true;
            // Attempt to move the piece left!
            if (piecex > 0 && !GetBoardAtPos(piecex - 1, piecey))
                piecex--;
        } else if (IsKeyPressed(KEY_RIGHT)) {
            started = true;
            // Attempt to move the piece right!
            if (piecex < bwidth - 1 && !GetBoardAtPos(piecex + 1, piecey))
                piecex++;
        } else if (IsKeyPressed(KEY_UP)) {
            started = true;
            // Attempt to drop the piece!
            int res = 0;
            while (!res) {
                if ((res = MovePieceDown(piecex, piecey, placing_piece, cleared_line))) {
                    if (res == GAMEOVER) {
                        return ShowGameOver();
                    }
                    if (cleared_line) {
                        PlaySound(clear);
                        lines_cleared += 4;
                        if (back_to_back) {
                            score += 1200;
                        } else {
                            back_to_back = true;
                            score += 800;
                        }
                        current_cooldown_to_piece_drop =
                            (int)fmax(1., original_difficulty -
                                              (lines_cleared / difficulty_scalar));
                    } else
                        back_to_back = false;
                }
            }
        }

        // he he ha ha
        if (IsKeyPressed(KEY_Y)) {
            OpenURL("https://www.youtube.com/watch?v=dQw4w9WgXcQ");
            return 1;
        }

        if (started && !--cooldown_to_piece_drop) {
            cooldown_to_piece_drop = current_cooldown_to_piece_drop;
            // Attempt to move the piece down!
            int res;
            if ((res = MovePieceDown(piecex, piecey, placing_piece, cleared_line))) {
                if (res == GAMEOVER) {
                    return ShowGameOver();
                }
                if (cleared_line) {
                    current_cooldown_to_piece_drop = (int)fmax(
                        1., original_difficulty - (lines_cleared / difficulty_scalar));
                    int r = rand() % 100;
                    if (!r)
                        PlaySound(clear);
                    else
                        PlaySound(not_clear);
                    lines_cleared += 4;
                    if (back_to_back) {
                        score += 1200;
                    } else {
                        back_to_back = true;
                        score += 800;
                    }
                } else
                    back_to_back = false;
            }
        }

        BeginTextureMode(game_buffer);
        // Fill the screen to setup
        DrawRectangle(0, 0, width, height, RAYWHITE);
        // Add our background effects
        background.draw();

        DrawFPS(10, 10);

        const int border_size = 10;

        int left_edge = (width + board_width + border_size) / 2;
        int top = (height - board_height - border_size) / 2;
        // Draw a box behind the text
        DrawRectangle(left_edge, top, 200, 55, BLACK);
        DrawRectangle(left_edge, top + 5, 195, 45, GRAY);
        sprintf(msg, "Score: %d", score);
        DrawText(msg, left_edge + border_size, top + 10, 20, WHITE);
        sprintf(msg, "Lines cleared: %d", lines_cleared);
        DrawText(msg, left_edge + border_size, top + 28, 20, WHITE);

        // Draw the next box
        DrawRectangle(left_edge, top + 55, 80, 150, BLACK);
        DrawRectangle(left_edge, top + 60, 75, 140, GRAY);
        DrawText("Next:", left_edge + border_size, top + 70, 20, WHITE);
        DrawRectangle(left_edge + 23, top + 94, line_tex.texture.width + 4,
                      line_tex.texture.height + 2, BLACK);
        DrawTexture(line_tex.texture, left_edge + 25, top + 94, piece_colors[last_color]);

        // Draw a border around the grid
        DrawRectangle((width - board_width - border_size) / 2,
                      (height - board_height - border_size) / 2,
                      (board_width + border_size), (board_height + border_size), BLACK);

        // Draw the board
        const int bx = (width - board_width) / 2;
        const int by = (height - board_height) / 2;
        DrawTexture(board_tex.texture, bx, by, WHITE);

        // Draw the current piece!
        Color piece_color = piece_colors[(size_t)placing_piece];
        for (int dy = 0; dy < 4; dy++) {
            int draw_y = piecey + dy;
            if (draw_y < bheight)
                DrawRectangle(bx + piecex * offset,
                              by + ((bheight - 1) - draw_y) * offset, size, size,
                              piece_color);
        }

        EndTextureMode();

        // Draw the game buffer to the window
        BeginDrawing();
        // DrawTexturePro(game_buffer.texture, );
        DrawTexturePro(game_buffer.texture, game_buffer_src, game_buffer_dst, {0, 0}, 0,
                       WHITE);

        float         hue = fmodf((frames / 60.f) * 360. / 10., 360.);
        Color         cat_color = ColorFromHSV(hue, 0.3, 1.);
        unsigned char alpha = (unsigned char)(fmax(sin(frames / 60. / 3.) * 40., 0.));
        cat_color.a = alpha;
        DrawTexture(cat, 0, 0, cat_color);

        // char msg[80];
        // sprintf(msg, "Frames: %d | alpha: %d", frames, alpha);
        // DrawText(msg, 20, 20, 20, BLACK);

        // If we haven't started, add our message!
        if (!started) {
            if (first_time)
                DrawText("Welcome to LINE TETRIS!\n\nMove the piece to begin\nDon't try "
                         "to rotate "
                         "it,\n there's no use\nThe up arrow will drop\nthe piece "
                         "instantly\n\nAlso, don't press Y",
                         width / 2 - 90, height / 2, 16, WHITE);
            else
                DrawText("Move the piece to begin", width / 2 - 90, height / 2, 16,
                         WHITE);
        }

        // We want to add the fade in
        if (frames < 120) {
            unsigned char colo = (unsigned char)(255. * (120. - frames) / 120.);
            DrawRectangle(0, 0, width, height, {0, 0, 0, colo});
        }
        frames++;
        EndDrawing();
    }

    return -1;
}

int main() {
    srand(time(NULL));

    InitWindow(width, height, "shit tetris");
    InitAudioDevice();
    SetTargetFPS(60);

    board[1] = 1;
    board[1 + 10] = 1;
    board[1 + 20] = 1;
    board[1 + 30] = 1;

    int board_width = bwidth * size + (bwidth - 1) * spacing;
    int board_height = bheight * size + (bheight - 1) * spacing;
    board_tex = LoadRenderTexture(board_width, board_height);

    // Need to create the line texture
    line_tex = LoadRenderTexture(size, (size + spacing) * 4);
    BeginTextureMode(line_tex);
    // Clear background
    ClearBackground({255, 255, 255, 0});
    // Draw squares
    DrawRectangle(0, 0, size, size, WHITE);
    DrawRectangle(0, (size + spacing), size, size, WHITE);
    DrawRectangle(0, 2 * (size + spacing), size, size, WHITE);
    DrawRectangle(0, 3 * (size + spacing), size, size, WHITE);
    EndTextureMode();

    game_buffer = LoadRenderTexture(width, height);

    cat = LoadTexture("rsc/cat.png");

    clear = LoadSound("rsc/DOODOO.mp3");
    not_clear = LoadSound("rsc/not_scream.mp3");
    background_music = LoadSound("rsc/GoodTetris.mp3");

    SetSoundVolume(clear, 1.);
    SetSoundVolume(not_clear, 1.);
    SetSoundVolume(background_music, 0.5);

    intro();

    int left = play_game(true);
    while (!left)
        left = play_game(false);

    // Time to unload
    UnloadRenderTexture(board_tex);
    UnloadRenderTexture(line_tex);
    UnloadRenderTexture(game_buffer);
    UnloadTexture(cat);

    UnloadSound(clear);
    UnloadSound(not_clear);

    CloseAudioDevice();
    CloseWindow();
}
