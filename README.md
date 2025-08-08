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

<img width="561" height="645" alt="image" src="https://github.com/user-attachments/assets/a813db8f-82e7-47c4-96bc-9ef71219c70b" />

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

### Chase Mode
- Each ghost uses its **specific targeting algorithm** to hunt Pac-Man.
- These targeting patterns were designed to **complement each other**, making it difficult to predict or escape all of them at once.
- Chase mode is the core gameplay challenge.

---

Feel free to explore the code to better understand the behavior implementation.  
This game is developed as a learning project using SDL and C++. Screenshots and logic closely follow the original arcade mechanics of Pac-Man.
