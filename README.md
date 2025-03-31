# STRATEGIC NONSENSE
**STRATEGIC NONSENSE** is a grid-based turn-based strategy game built in Unreal Engine. The game pits two teams (each with a Sniper and a Brawler) against each other on a 25x25 obstacle-filled grid. A human player competes against an AI opponent in a match of tactical positioning and strategic attacks.
## Project Status
The current implementation includes:
- Grid-based map (25x25)
- Paper2D units for Snipers, Brawlers, Trees and Mountains
- Static team selection (one Sniper and one Brawler per team)
- Turn-based match flow between player and AI
- Player-side unit selection widget implemented in C++
- Player chooses unit to place; AI places randomly
- Randomly chosen team colours per match
- Occupancy tracking via 2D matrix for valid placements
- Start message widget logic fully in C++
- Unit placement system with alternating turns
## Gameplay Overview
Each team consists of:

| Unit    | Movement | Attack Type                | Range | Damage | Health |
| ------- | -------- | -------------------------- | ----- | ------ | ------ |
| Sniper  | 3 cells  | Ranged (ignores obstacles) | 10    | 4–8    | 20     |
| Brawler | 6 cells  | Melee                      | 1     | 1–6    | 40     |

### Game Flow
1. Two teams are randomly selected from available colours.
2. A coin toss decides which player starts the placement phase.
3. Players alternate placing one unit at a time.
4. The starting player takes the first turn.
5. On each turn, the player can:
   - Move then attack
   - Attack only
   - Move only
6. The game ends when one team loses both units.
## AI Logic
The AI currently:
- Selects units randomly to place
- Will place units at valid, unoccupied positions
Movement and attack decisions will be added in upcoming iterations.
## Notes
- Each cell is slightly larger than a unit for clear visual separation.
- Obstacles will be configurable by percentage and will not isolate regions.
## Planned Features
- Movement range highlighting
- AI with basic pathfinding (e.g. A*)
- Full match logging
- Counterattack damage system
## Code Structure
The project is implemented in C++, structured around core components like units, the game grid, team logic, and UI widgets. Key systems include game flow management, unit control, grid navigation, and user interaction.
### Core Classes
- **`BattleGameMode`**  
    Coordinates overall game rules, player turns, and win conditions.
- **`BattlePlayerController`**  
    Handles user input and delegates it to relevant systems like unit placement and selection.
- **`Team`**  
    Represents one team’s composition (Sniper + Brawler) and colour. Supports per-match configuration.
- **`UnitActor`** (base class)  
    Abstract base class for all units. Implements shared logic like grid position, health, and basic movement hooks.
- **`SniperUnit` / `BrawlerUnit`**  
    Specialised classes inheriting from `UnitActor`, each with unique movement, damage, and attack range logic.
- **`GridManager`**  
    Tracks cell occupancy and manages the 2D grid using a matrix-based approach. Provides random valid cell generation and placement validation.
- **`Obstacle`**  
    Represents map obstacles like trees or mountains. These are placed randomly based on configuration.
- **`GridCameraActor`**  
    Ensures a proper 2D top-down view of the grid, centring and scaling the camera as needed.
- **`UnitPlacementManager`**  
    Handles the placement phase, alternating between player and AI turns. Ensures that units are placed in unoccupied cells.
- **`CombatManager`**  
    Will coordinate attack and damage logic (structure in place for later implementation).
### Widgets
All UI elements are implemented in C++ (not Blueprints), and appear dynamically at runtime.
- **`StartMessageWidget`**  
    Displays the initial start message with who won the coin toss. Shown before unit placement begins.
- **`UnitSelectionWidget`**  
    Presents the player with a choice between Sniper and Brawler during the placement phase. Uses dynamic multicast delegates to notify the main logic of the selection.
- **`EndTurnWidget`**
    Appears during the players turn, allowing them to end their turn explicitly.
- **`GameOverWidget`** 
    Will appear once a win condition is met, displaying the game outcome. Allows the player to restart the game.
- **`GameStatusWidget`**
    Display real-time info: current turn and unit HP.
