# A CODE OF ICE AND FIRE
CodinGame's global programming contest for 2019.

## Goal
Build armies to defeat your opponent by destroying their headquarters.

## Rules
The map

The map is a grid of size 12x12, where the top-left corner is the cell (0,0). The map is randomly generated at the start of each game.

Both players start with headquarters (HQ) from opposite edges of the map ((0,0) and (11,11)).

A map cell can be either:

void (#): not a playable cell. <br>
neutral (.): doesn't belong to any player. <br>
captured (O or X): belongs to a player. <br>
inactive (o or x): belongs to a player but inactive. <br>
Territory ownership

Throughout the game, each player will capture cells to enlarge their territory. A player territory is composed of all the cells owned by the player that are active.

A cell is said to be active if and only if the cell is connected to the headquarters. That is, there exists a path of owned cells from the headquarters to this cell.

![image](https://user-images.githubusercontent.com/91319870/215278837-f4a25c2d-caae-42c5-908a-0dd410fca684.png)

The red territory is composed of 6 cells. The 3 red-dark cells are now inactive because the blue player interrupted part of this territory. By capturing, for example, the cell marked by an X, the red player can make these cells active again.

## Income

At the beginning of each turn, a player gains or loses gold based on their income. A player has +1 income for each active cell owned.

Every turn, army units cost some income (upkeep).

Level 1 units reduce income by 1 per unit. <br>
Level 2 units reduce income by 4 per unit. <br>
Level 3 units reduce income by 20 per unit. <br>
Players has +4 income for each mine they control.

If a player has negative income and cannot pay their upkeep using their gold, all of the player's units die and the player's gold is reset to 0.


## Buildings

Players can BUILD buildings to improve their economy or military power. A player can only build on owned active cells that are unoccupied.

Players can build two different buildings: the MINE and the TOWER.

MINE: mines produce gold each turn and can only be built on mine spots. Mines cost 20 + 4 * playerNbOfMines. So the first costs 20, the second one 24... A mine's income is always +4.  <br>
TOWER: towers protect owned cells that are adjacent (up, down, left and right, but not diagonally). Cells protected by a tower can only be reached by enemy units of level 3. Likewise, the tower can only be destroyed by a level 3 unit. A tower costs 15 gold to build and cannot be built on a mine spot. Here is a figure of a tower protection range: <br>

![image](https://user-images.githubusercontent.com/91319870/215278870-0b2a6069-65d4-4818-9ce3-b619bda0bde5.png)

The tower owned by the red player protects the adjacent cells marked by a black cross. Cells are not protected diagonally. Moreover, the blue cell on the right of the tower is not protected as it is not a red cell.

If a building is on an inactive cell, it is not destroyed; it is just inactive.

## Armies

Army units can move to capture cells and to destroy opponent's buildings and units.

Army units of different levels (1, 2 or 3) can be trained using the command TRAIN level x y. The target cell (x,y) must be in the player's territory or its direct neighbourhood (adjacent cells).

Army units can only destroy units of inferior level, except level 3 units which can destroy any unit.

Towers can only be destroyed by level 3 units.

Only level 3 units can be trained or moved on a cell protected by an opponent tower.

Each unit can only move one cell per turn by using the command MOVE id x y.

It's not possible to train or move on a cell with a friendly building or unit on it.

An army unit cannot move on the same turn it's trained.

When using MOVE id x y, if the distance between the unit and the target coordinates (x,y) is greater than 1, the unit moves towards the target.

To train a unit or move it on an enemy unit or building, the attacking unit must be able to destroy the defending unit or building. If so, the attacking unit always survives. Else, the action is invalid; nothing happens.

If a unit is on an inactive cell at the beginning of a turn, the unit is instantly destroyed.

Level	1	2	3 <br>
Recruitment cost	10	20	30 <br>
Upkeep	1	4	20 <br>
Can kill units level	-	1	1, 2, 3 <br>
Can destroy	Mines <br>
HQ	Mines <br>
HQ	Mines <br>
Towers <br>
HQ

## Order of actions

All actions are processed sequentially. Invalid actions are ignored.

![image](https://user-images.githubusercontent.com/91319870/215278951-7e296cb5-9d22-4a02-8682-d68bcb6285d5.png)

## Victory Conditions
Destroy the enemy headquarters. <br>
After 100 turns, you have more military power than your opponent. The military power is computed by the sum of the cost of all of your units + your amount of gold.

## Lose Conditions
You fail to provide a valid command in time. <br>
You provide a unrecognized command.

## Expert Rules
The source code can be found here: https://github.com/Azkellas/a-code-of-ice-and-fire.  <br>
There are between 8 and 20 mine spots.  <br>
A mine spot is always present on the cell directly on the right of the red HQ and on the cell directly on the left of the blue HQ. <br>
Cells surrounding the HQ at distance 1 (including diagonally) are never void cells. <br>
The blue player always owns the (11, 10) cell at the beginning of the game.
 
## Game Input
### Initialization input
Line 1: one integer numberMineSpots: the number of mine spots on the map. <br>
Next numberMineSpots lines: two integers <br>
x and y: coordinates of the mine spot.

### Game turn input
Line 1: an integer gold, the player's amount of gold. <br>
Line 2: an integer income, the player's income. <br>
Line 3: an integer opponentGold, the opponent's amount of gold. <br>
Line 4: an integer opponentIncome: the opponent's income. <br>
Next 12 lines: 12 characters, on for each cell: <br>
#: void <br>
.: neutral <br>
O: owned and active cell <br>
o: owned and inactive <br>
X: active opponent cell <br>
x: inactive opponent cell <br>
Next line: buildingCount: the amount of buildings on the map. <br>
Next buildingCount lines: four integers <br>
owner <br>
0: Owned <br>
1: Enemy <br>
buildingType: the type of building <br>
0: HQ <br>
1: Mine <br>
2: Tower <br>
x and y: the building's coordinates. <br>
Next line:unitCount: the amount of units on the map. <br>
Next unitCount lines: five integers <br>
owner <br>
0: Owned <br>
1: Enemy <br>
unitId: this unit's unique id <br>
level: 1, 2 or 3. <br>
x and y: the unit's coordinates.

### Output
A single line being a combination of these commands separated by ; <br>
MOVE id x y <br>
TRAIN level x y where level is either 1, 2 or 3 <br>
BUILD building-type x y where the building-type is either MINE or TOWER. <br>
WAIT to do nothing. <br>
You can add a message to display in the viewer by appending the command MSG my message.

Example: "MOVE 1 2 3; TRAIN 3 3 3; BUILD MINE 0 1; MSG Team Fire" <br>

### Constraints
Allotted response time to output is ≤ 50ms. <br>
Allotted response time to output on the first turn is ≤ 1000ms.
