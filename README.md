# Pac-Man Game

This is a simplified implementation of the classic Pac-Man game.  
The player controls Pac-Man navigating through a maze while avoiding ghosts.  
Each ghost has a unique targeting algorithm inspired by the original Pac-Man game, creating a dynamic and strategic gameplay experience.

---

## Ghost Movement Algorithm

Each ghost has a different targeting behavior based on Pac-Man’s position and direction. Below is a summary of each ghost’s unique logic:

### Blinky (Red Ghost)
- Directly chases Pac-Man by targeting his **current position**.
- Aggressive and constantly pursues the player.
  
<img width="335" height="424" alt="image" src="https://github.com/user-attachments/assets/e68007ba-cbb2-48c2-bd4a-69763511093a" />

---

### Pinky (Pink Ghost)
- Targets **four tiles ahead** of Pac-Man in the direction he is currently moving.
- Attempts to **ambush Pac-Man** by predicting his future position.
- Does not target Pac-Man directly, but aims to cut him off.
  
<img width="336" height="408" alt="image" src="https://github.com/user-attachments/assets/f9554887-71a0-49b5-a77d-bb71a03b9cb4" />

---

### Inky (Blue Ghost)
- Combines Pac-Man and Blinky’s positions:
  - First calculates a tile **two steps ahead of Pac-Man** (based on Pac-Man’s current direction).
  - Then forms a vector from **Blinky’s position** to that tile, doubles it, and uses the result as the target.
- This makes Inky's movement **unpredictable and reactive**.

<img width="334" height="416" alt="image" src="https://github.com/user-attachments/assets/8fa235dc-e681-487c-921b-1a9fb0414465" />

---

### Clyde (Orange Ghost)
- Behavior switches based on distance to Pac-Man:
  - If **far** from Pac-Man (distance squared ≥ 64 tiles), behaves like Blinky and **chases Pac-Man**.
  - If **close**, retreats to a predefined **scatter corner**.
- This causes Clyde to **randomly alternate** between chasing and fleeing.

<img width="335" height="423" alt="image" src="https://github.com/user-attachments/assets/6319da20-6ee9-4aec-92ac-b26ff0937376" />

---

## Ghost States

Each ghost can be in one of several behavioral states during the game:

- **WAIT**  
  - Ghost remains inside the ghost house and does not move in the maze.  
  - Used at the beginning of the game or after respawning.

- **EXIT**  
  - Ghost exits the ghost house and enters the maze.

- **CHASE**  
  - Ghosts target Pac-Man using their **custom targeting algorithm** (described above).  
  - This mode is **aggressive** and keeps pressure on the player.

- **SCATTER**  
  - Ghosts stop chasing Pac-Man and head toward their **own corner of the maze**.  
  - This mode is **temporary**, allowing players a brief relief and helping to **mix the gameplay flow**.


---

## Ghost Modes Explained

### Scatter Mode
- Each ghost targets a fixed corner of the maze:
  - **Blinky** → Top-right corner  
  - **Pinky** → Top-left corner  
  - **Inky** → Bottom-right corner  
  - **Clyde** → Bottom-left corner  
- Gives players a break from direct chasing.
- Useful for creating rhythm in the gameplay.
<img width="333"  alt="image" src="https://github.com/user-attachments/assets/27a22f78-11a8-4d38-8a74-509052cddcca" />

### Chase Mode
- Each ghost uses its **specific targeting algorithm** to hunt Pac-Man.
- These targeting patterns were designed to **complement each other**, making it difficult to predict or escape all of them at once.
- Chase mode is the core gameplay challenge.

---

Feel free to explore the code to better understand the behavior implementation.  
This game is developed as a learning project using SDL and C++. Screenshots and logic closely follow the original arcade mechanics of Pac-Man.

## Game Rules and Mechanics

### Speeds

Ghosts and Pac-Man have different speeds depending on level and state:

- **Normal Ghost Speed:** `GHOST_SPEED_NORMAL` (1.0f by default)
- **Frightened Ghost Speed:** `GHOST_SPEED_FRIGHTENED` (varies by level, see `getFrightenedSpeed(level)`)
- **Eaten Ghost Speed:** `GHOST_SPEED_EATEN` (2.0f)
- **Pac-Man Speed:** `PACMAN_SPEED` (2)

Ghosts speed increases slightly every few levels as defined in `getGhostSpeed(level)`.

---

### Eating Ghosts and Scoring

When Pac-Man eats a ghost during **Frightened mode**, the score increases for each successive ghost eaten in the same FRIGHTENED period:

| Ghost eaten | Score | Notes |
|-------------|-------|-------|
| 1st ghost   | 200   | First ghost eaten in current FRIGHTENED period |
| 2nd ghost   | 400   | Second ghost eaten in same FRIGHTENED period |
| 3rd ghost   | 800   | Third ghost eaten in same FRIGHTENED period |
| 4th ghost   | 1600  | Fourth ghost eaten in same FRIGHTENED period |

- The scoring **resets** after FRIGHTENED mode ends.
- When a ghost is eaten:
  - `ghostEaten = true` is set.
  - Ghost enters **EATEN state** and returns to the ghost house.
  - `startShowingScore(score)` displays the points above the ghost temporarily.
  - After `scoreDisplayDuration` (1000 ms), the ghost resumes normal behavior.

- Score textures (`scoreTexture200`, `scoreTexture400`, etc.) are loaded and rendered via `Ghost::renderScore()`.



> Note: The points double for each ghost eaten in the same FRIGHTENED period, encouraging Pac-Man to eat multiple ghosts consecutively for higher scores.

---
## Frightened Mode (Vulnerable Ghosts)

When Pac-Man eats a **power pellet**, all ghosts enter **FRIGHTENED mode** for a limited duration:

- **Behavior changes**:
  - Ghosts **move slower** (`GHOST_SPEED_FRIGHTENED`) than normal.
  - Ghosts **flee from Pac-Man** instead of chasing him.
  - Movement becomes **randomized**, making their path less predictable.

- **Interaction with Pac-Man**:
  - If Pac-Man collides with a ghost in this state:
    - The ghost is marked as **`ghostEaten = true`**.
    - It enters the **EATEN state** and returns to the ghost house.
    - Pac-Man earns **increasing points** depending on how many ghosts are eaten consecutively:

    | Ghost eaten | Score | Notes |
        |-------------|-------|-------|
    | 1st ghost   | 200   | First ghost eaten in current FRIGHTENED period |
    | 2nd ghost   | 400   | Second ghost eaten in same FRIGHTENED period |
    | 3rd ghost   | 800   | Third ghost eaten in same FRIGHTENED period |
    | 4th ghost   | 1600  | Fourth ghost eaten in same FRIGHTENED period |

- **Visual cues**:
  - Ghosts **turn blue** to indicate FRIGHTENED mode.
  - Near the end of FRIGHTENED mode, ghosts **flash between blue and white** to warn the player that normal behavior will resume soon.

- **Duration**:
  - Controlled by `FRIGHTENED_TIME` per level.
  - Once time expires, ghosts **return to CHASE or SCATTER mode** depending on the cycle.

### Example (GIF)

<img src="screenshots/ghostFrightened.gif" alt="Frightened Ghost Animation" width="320">

> FRIGHTENED mode encourages strategic play, allowing Pac-Man to **turn the tables** and hunt ghosts temporarily.


### Ghost Behavior Cycles

Ghosts alternate between **CHASE** and **SCATTER** modes based on `GHOST_CYCLES` (milliseconds):

```cpp
constexpr std::array<Uint32, 7> GHOST_CYCLES = { 7000, 20000, 7000, 20000, 5000, 20000, 5000 };

