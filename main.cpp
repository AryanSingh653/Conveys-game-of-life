#include<raylib.h>

const int CELL = 20;
const int screen_width = 1800;
const int screen_hieght = 1000;
const int columns = screen_width / CELL;  // 45
const int rows = screen_hieght / CELL;    // 35

const int SETUP_FPS = 60;  // responsive mouse while setting up
const int SIM_FPS = 12;    // simulation speed (try 6 for slower)

// Convention used everywhere: mat[col][row]
int mat[columns][rows] = {0};   // current generation
int next[columns][rows] = {0};  // next generation (scratch array) 

enum GameState { SETUP, RUNNING };
GameState state = SETUP;

Color GREY = {29, 29, 29, 255};

void DrawCells()
{
    for (int c = 0; c < columns; c++)
    {
        for (int r = 0; r < rows; r++)
        {
            if (mat[c][r] == 1)
            {
                DrawRectangle(c * CELL, r * CELL, CELL, CELL, GREEN);
            }
        }
    }
}

void DrawGrid()
{
    for (int x = 0; x <= screen_width; x += CELL)
    {
        DrawLine(x, 0, x, screen_hieght, DARKGRAY);
    }
    for (int y = 0; y <= screen_hieght; y += CELL)
    {
        DrawLine(0, y, screen_width, y, DARKGRAY);
    }
}

void SetupSim()
{
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        Vector2 mouse = GetMousePosition();
        int col = (int)mouse.x / CELL;
        int row = (int)mouse.y / CELL;

        // make sure we never index outside the array
        if (col >= 0 && col < columns && row >= 0 && row < rows)
        {
            mat[col][row] = 1 - mat[col][row];  // toggle alive/dead
        }
    }
}

// Counts live neighbours of (c, r). Cells outside the grid count as dead.
int CountNeighbors(int c, int r)
{
    int count = 0;
    for (int dx = -1; dx <= 1; dx++)
    {
        for (int dy = -1; dy <= 1; dy++)
        {
            if (dx == 0 && dy == 0) continue;  // skip the cell itself

            int nc = c + dx;
            int nr = r + dy;

            if (nc >= 0 && nc < columns && nr >= 0 && nr < rows)
            {
                count += mat[nc][nr];
            }
        }
    }
    return count;
}

void RunSim()
{
    // 1) Work out the next generation, reading only from mat
    for (int c = 0; c < columns; c++)
    {
        for (int r = 0; r < rows; r++)
        {
            int n = CountNeighbors(c, r);

            if (mat[c][r] == 1)
            {
                // alive: survives with 2 or 3 neighbours, otherwise dies
                next[c][r] = (n == 2 || n == 3) ? 1 : 0;
            }
            else
            {
                // dead: becomes alive with exactly 3 neighbours
                next[c][r] = (n == 3) ? 1 : 0;
            }
        }
    }

    // 2) Only now copy next back into mat
    for (int c = 0; c < columns; c++)
    {
        for (int r = 0; r < rows; r++)
        {
            mat[c][r] = next[c][r];
        }
    }
}

void UpdateBoard()
{
    switch (state)
    {
        case SETUP:
            SetupSim();
            if (IsKeyPressed(KEY_ENTER))
            {
                state = RUNNING;
                SetTargetFPS(SIM_FPS);  // slow the loop down for the simulation
            }
            break;

        case RUNNING:
            RunSim();
            break;
    }
}

int main()
{
    InitWindow(screen_width, screen_hieght, "Conway's Game Of Life Simulation");
    SetTargetFPS(SETUP_FPS);

    while (!WindowShouldClose())
    {
        // 1] Event handling + 2] Updating state
        UpdateBoard();

        // 3] Drawing
        BeginDrawing();
        ClearBackground(GREY);
        DrawCells();  // cells first, so grid lines sit on top
        DrawGrid();
        if (state == SETUP)
        {
            DrawText("Click cells to toggle. Press ENTER to start.", 10, 10, 20, RED);
        }
        EndDrawing();
    }

    CloseWindow();
    return 0;
}