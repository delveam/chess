#include "Chess.hpp"
#include "raylib-cpp.hpp"

void Chess::draw()
{
	BeginDrawing();
	{
		ClearBackground(WHITE);
		DrawText("my name is chess", 8, 8, 20, BLACK);
	}
	EndDrawing();
}
