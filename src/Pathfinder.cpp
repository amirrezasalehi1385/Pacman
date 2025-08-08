#include "../include/PathFinder.h"
#include "../include/Map.h"

Direction bfsFindDirection(const Map& map, int startX, int startY, int targetX, int targetY) {
    if (startX == targetX && startY == targetY) return STOP;

    int rows = 36;
    int cols = 28;

    std::vector<std::vector<SDL_Point>> parent(rows, std::vector<SDL_Point>(cols, {-1, -1}));
    std::vector<std::vector<bool>> visited(rows, std::vector<bool>(cols, false));

    std::queue<SDL_Point> q;
    q.push({startX, startY});
    visited[startY][startX] = true;

    const SDL_Point directions[4] = { {0,-1}, {0,1}, {-1,0}, {1,0} };

    while (!q.empty()) {
        SDL_Point current = q.front(); q.pop();

        if (current.x == targetX && current.y == targetY) {
            SDL_Point cur = current;
            SDL_Point prev = parent[cur.y][cur.x];

            while (!(prev.x == startX && prev.y == startY)) {
                cur = prev;
                prev = parent[cur.y][cur.x];
            }

            if (cur.x == startX && cur.y == startY - 1) return UP;
            if (cur.x == startX && cur.y == startY + 1) return DOWN;
            if (cur.x == startX - 1 && cur.y == startY) return LEFT;
            if (cur.x == startX + 1 && cur.y == startY) return RIGHT;
            return STOP;
        }

        for (auto dir : directions) {
            int nx = current.x + dir.x;
            int ny = current.y + dir.y;

            if (nx >= 0 && ny >= 0 && nx < cols && ny < rows) {
                if (!visited[ny][nx] && map.isWalkable(nx, ny)) {
                    visited[ny][nx] = true;
                    parent[ny][nx] = current;
                    q.push({nx, ny});
                }
            }
        }
    }
    return STOP;
}
