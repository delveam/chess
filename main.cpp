#include "raylib-cpp.hpp"

int main() {
    int screenWidth = 320 * 2;
    int screenHeight = 180 * 2;

    raylib::Color background(RAYWHITE);
    raylib::Color textColor(LIGHTGRAY);
    raylib::Window window(screenWidth, screenHeight, "chess");

    SetTargetFPS(60);

    while (!window.ShouldClose())
    {
        BeginDrawing();
				{
					background.ClearBackground();
					textColor.DrawText("Congrats! You created your first window!", 190, 200, 20);
				}
        EndDrawing();
    }

    return 0;
}
