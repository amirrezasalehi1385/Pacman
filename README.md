# Pac-Man Game

This is a simplified implementation of the classic Pac-Man game.  
The player controls Pac-Man navigating through a maze while avoiding ghosts.

## Ghost Movement Algorithm

Each ghost in the game has a unique behavior inspired by the original Pac-Man:

### Blinky (Red Ghost)
- Directly chases Pac-Man by targeting his current position.
- Aggressive and constantly pursues the player.
- 
<img width="335" height="424" alt="image" src="https://github.com/user-attachments/assets/e68007ba-cbb2-48c2-bd4a-69763511093a" />

### Pinky (Pink Ghost)
- Targets four tiles ahead of Pac-Man in the direction Pac-Man is currently moving.
- Attempts to ambush Pac-Man by predicting his future position.
- Moves toward this target tile, making Pinky’s behavior more strategic and tricky.
<img width="336" height="408" alt="image" src="https://github.com/user-attachments/assets/f9554887-71a0-49b5-a77d-bb71a03b9cb4" />

### Inky (Blue Ghost)
- Targets a position based on both Pac-Man and Blinky:
  - Calculates a point two tiles ahead of Pac-Man in his moving direction.
  - Forms a vector from Blinky’s position to this point and doubles it.
  - Targets the resulting position, making his behavior unpredictable.
<img width="334" height="416" alt="image" src="https://github.com/user-attachments/assets/8fa235dc-e681-487c-921b-1a9fb0414465" />

### Clyde (Orange Ghost)
- Switches behavior based on distance to Pac-Man:
  - If far (distance squared ≥ 64 tiles), chases Pac-Man directly.
  - If too close, targets a fixed scatter corner to move away.
<img width="335" height="423" alt="image" src="https://github.com/user-attachments/assets/6319da20-6ee9-4aec-92ac-b26ff0937376" />

## Ghost States

Ghosts have several states that change their behavior:

- **WAIT:** Stay inside the ghost house waiting to exit.
- **EXIT:** Move out of the ghost house into the maze.
- **CHASE:** Actively pursue Pac-Man based on their targeting algorithm.
- **SCATTER:** Retreat to their assigned corners temporarily.

---

<img width="561" height="645" alt="image" src="https://github.com/user-attachments/assets/a813db8f-82e7-47c4-96bc-9ef71219c70b" />
