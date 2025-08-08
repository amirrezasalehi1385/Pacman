# Pac-Man Game

This is a simplified implementation of the classic Pac-Man game.  
The player controls Pac-Man navigating through a maze while avoiding ghosts.

## Ghost Movement Algorithm

Each ghost in the game has a unique behavior inspired by the original Pac-Man:

### Blinky (Red Ghost)
- Directly chases Pac-Man by targeting his current position.
- Aggressive and constantly pursues the player.

### Clyde (Orange Ghost)
- Switches behavior based on distance to Pac-Man:
  - If far (distance squared ≥ 64 tiles), chases Pac-Man directly.
  - If too close, targets a fixed scatter corner to move away.

### Inky (Blue Ghost)
- Targets a position based on both Pac-Man and Blinky:
  - Calculates a point two tiles ahead of Pac-Man in his moving direction.
  - Forms a vector from Blinky’s position to this point and doubles it.
  - Targets the resulting position, making his behavior unpredictable.

## Ghost States

Ghosts have several states that change their behavior:

- **WAIT:** Stay inside the ghost house waiting to exit.
- **EXIT:** Move out of the ghost house into the maze.
- **CHASE:** Actively pursue Pac-Man based on their targeting algorithm.
- **SCATTER:** Retreat to their assigned corners temporarily.

---

_Add screenshots here to show gameplay and ghost behaviors._
