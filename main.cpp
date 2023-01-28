#include <cmath>
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>

using namespace std;

struct Position {
    Position() {}
        
    int x, y;
    
    Position& operator=(const Position &position) {   
        if (this == &position)
            return *this;
     
        x = position.x;
        y = position.y;
        return *this;
    }
    
    Position(int x, int y): x(x), y(y) {}
};

const Position WEST(-1, 0);
const Position EAST(1, 0);
const Position NORTH(0, -1);
const Position SOUTH(0, 1);
const Position NORTH_WEST(-1, -1);
const Position NORTH_EAST(1, -1);
const Position SOUTH_WEST(-1, 1);
const Position SOUTH_EAST(1, 1);

const string WAIT            = "WAIT;";
const string MOVE            = "MOVE ";
const string BUILD_MINE      = "BUILD MINE ";
const string BUILD_TOWER     = "BUILD TOWER ";
const string TRAIN_SCOUT     = "TRAIN 1 ";
const string TRAIN_SWORDSMAN = "TRAIN 2 ";
const string TRAIN_KNIGHT    = "TRAIN 3 ";

bool isBattlefield(Position square) {
    return (square.x >= 0) && (square.x < 12)
        && (square.y >= 0) && (square.y < 12);
}

int distance(Position one, Position two) { // Distance between two squares.
    return abs(one.x - two.x) + abs(one.y - two.y);
}

struct Battlefield {
    vector<string> map;
    /*
    * void (#): not a playable cell.
    * neutral (.): doesn't belong to any player.
    * captured (O or X): belongs to a player.
    * inactive (o or x): belongs to a player but inactive.
    * 1: my first level soldiers.
    * 2: my second level soldiers.
    * 3: my third level soldiers.
    * 7: opponent first level soldiers.
    * 8: opponent second level soldiers.
    * 9: opponent third level soldiers.
    * m: my active mines.
    * t: my active towers.
    * M: opponent active mines.
    * T: opponent active towers.
    */
    
    vector<string> towersMap;
    /*
    * t: my active towers.
    * T: opponent active towers.
    * g: my savezones.
    * G: opponent savezones.
    */
    
    vector<Position> borderLine;
    vector<Position> myActiveTerritory;
    vector<Position> myTerritory;
    vector<Position> opponentBorderLine;
    vector<Position> opponentBorderScouts;
    vector<Position> opponentBorderSwordsmans;
    vector<Position> opponentBorderKnights;
    
    Battlefield() {
        for (int i = 0; i < 12; i++) {
            map.push_back("############");
            towersMap.push_back("############");
        }
    }
    
    int spawnSize() {
        return borderLine.size() +
               myActiveTerritory.size() +
               opponentBorderLine.size() +
               opponentBorderScouts.size()+
               opponentBorderSwordsmans.size() +
               opponentBorderKnights.size();
    }
               
    vector<Position> inactiveMines;
    
    void replace(vector<Position> squares, char symbol) {
        for (auto square: squares) {
            if (isBattlefield(square))
                map[square.y][square.x] = symbol;
        }
    }
    
    void towerUpdate(Position square, int owner) {
        if (isBattlefield(square)) {
            if (owner) {    
                map[square.y][square.x] = 'T';
                towersMap[square.y][square.x] = 'T';
                
                if (isBattlefield(Position(square.x + EAST.x, square.y + EAST.y))) {
                    if (towersMap[square.y + EAST.y][square.x + EAST.x] != '#')
                        towersMap[square.y + EAST.y][square.x + EAST.x] = 'G';
                }
                if (isBattlefield(Position(square.x + WEST.x, square.y + WEST.y))) {
                    if (towersMap[square.y + WEST.y][square.x + WEST.x] != '#')
                        towersMap[square.y + WEST.y][square.x + WEST.x] = 'G';
                }
                if (isBattlefield(Position(square.x + NORTH.x, square.y + NORTH.y))) {
                    if (towersMap[square.y + NORTH.y][square.x + NORTH.x] != '#')
                        towersMap[square.y + NORTH.y][square.x + NORTH.x] = 'G';
                }
                if (isBattlefield(Position(square.x + SOUTH.x, square.y + SOUTH.y))) {
                    if (towersMap[square.y + SOUTH.y][square.x + SOUTH.x] != '#')
                        towersMap[square.y + SOUTH.y][square.x + SOUTH.x] = 'G';
                }
            } else {    
                map[square.y][square.x] = 't';
                towersMap[square.y][square.x] = 't';
                
                if (isBattlefield(Position(square.x + EAST.x, square.y + EAST.y))) {
                    if (towersMap[square.y + EAST.y][square.x + EAST.x] != '#')
                        towersMap[square.y + EAST.y][square.x + EAST.x] = 'g';
                }
                if (isBattlefield(Position(square.x + WEST.x, square.y + WEST.y))) {
                    if (towersMap[square.y + WEST.y][square.x + WEST.x] != '#')
                        towersMap[square.y + WEST.y][square.x + WEST.x] = 'g';
                }
                if (isBattlefield(Position(square.x + NORTH.x, square.y + NORTH.y))) {
                    if (towersMap[square.y + NORTH.y][square.x + NORTH.x] != '#')
                        towersMap[square.y + NORTH.y][square.x + NORTH.x] = 'g';
                }
                if (isBattlefield(Position(square.x + SOUTH.x, square.y + SOUTH.y))) {
                    if (towersMap[square.y + SOUTH.y][square.x + SOUTH.x] != '#')
                        towersMap[square.y + SOUTH.y][square.x + SOUTH.x] = 'g';
                }
            }
        }
    }
    
    bool look(Position current, Position swift, char target) {
        return isBattlefield(Position(current.x + swift.x, current.y + swift.y)) &&
               (map[current.y + swift.y][current.x + swift.x] == target) /* &&
               (towersMap[current.y + swift.y][current.x + swift.x] == target)*/;
    }
    
    vector<Position> scan(Position current, char target) {
        vector<Position> result;
            
        if (look(current, WEST, target))
            result.push_back(Position(current.x + WEST.x, current.y + WEST.y));
        if (look(current, EAST, target))
            result.push_back(Position(current.x + EAST.x, current.y + EAST.y));
        if (look(current, NORTH, target))
            result.push_back(Position(current.x + NORTH.x, current.y + NORTH.y));
        if (look(current, SOUTH, target))
            result.push_back(Position(current.x + SOUTH.x, current.y + SOUTH.y));
              
        return result;
    }
    
    void updateBorderLine() {
        borderLine.clear();
        opponentBorderLine.clear();
        opponentBorderScouts.clear();
        opponentBorderSwordsmans.clear();
        opponentBorderKnights.clear();
        
        vector<Position> border;
        
        for (int i = 0; i < 12; i++) {
            for (int j = 0; j < 12; j++) {
                if (map[i][j] == 'O' ||
                    map[i][j] == '1' ||
                    map[i][j] == '2' ||
                    map[i][j] == '3' ||
                    map[i][j] == 't' ||
                    map[i][j] == 'm') {
                    border = scan(Position(j, i), '.');
                    
                    for (int i = 0; i < border.size(); i++)
                        borderLine.push_back(border[i]);
                        
                    border = scan(Position(j, i), 'X');
                    
                    for (int i = 0; i < border.size(); i++)
                        opponentBorderLine.push_back(border[i]);
                    
                    border = scan(Position(j, i), 'x');
                    
                    for (int i = 0; i < border.size(); i++)
                        opponentBorderLine.push_back(border[i]);
                    
                    border = scan(Position(j, i), 'M');
                    
                    for (int i = 0; i < border.size(); i++)
                        opponentBorderLine.push_back(border[i]);
                    
                    border = scan(Position(j, i), '7');
                    
                    for (int i = 0; i < border.size(); i++)
                        opponentBorderScouts.push_back(border[i]);
                        
                    border = scan(Position(j, i), '8');
                    
                    for (int i = 0; i < border.size(); i++)
                        opponentBorderSwordsmans.push_back(border[i]);
                        
                    border = scan(Position(j, i), '9');
                    
                    for (int i = 0; i < border.size(); i++)
                       opponentBorderKnights.push_back(border[i]);
                }
            }
        }
    }
    
    void updateMyActiveTerritory() {
        myActiveTerritory.clear();
        myTerritory.clear();
            
        for (int i = 0; i < 12; i++) {
            for (int j = 0; j < 12; j++) {
                if (map[i][j] == 'O') {
                    if ((i == 0) && (j == 0))
                        continue;
                    if ((i == 11) && (j == 11))
                        continue; 
                        
                    myActiveTerritory.push_back(Position(j, i));
                    myTerritory.push_back(Position(j, i));
                }
                
                if (map[i][j] == '1' ||
                    map[i][j] == '2' ||
                    map[i][j] == '3')
                    myTerritory.push_back(Position(j, i));      
            }
        }
    }
};

struct Unit {
    int id;    
    Position position;

    Unit(int x, int y, int id): position(x, y), id(id) {}
};

struct Army {
    Army() {}
    
    vector<Unit> scouts;
    vector<Unit> swordsmans;
    vector<Unit> knights;
    
    int size()   { return scouts.size() + swordsmans.size() + knights.size(); }
    int salary() { return scouts.size() + swordsmans.size() * 4 + knights.size() * 20; }
    void clear() { scouts.clear(); swordsmans.clear(); knights.clear(); }
};   

struct Buildings {
    Buildings() {}
    
    Position HQ;
    vector<Position> mines;
    vector<Position> towers;
    
    void clear() { mines.clear(); towers.clear(); }
};

class Game {   
public:
    Game() {}
    
    int turn = 0;
    int myGold, myIncome, opponentGold, opponentIncome;
    
    Army myArmy, opponentArmy;
    Buildings myBuildings, opponentBuildings;
    
    string output = "";
    Battlefield battlefield;

    void debug() {
        cerr << endl;
        
        for (int i = 0; i < 12; i++) {
            for (int j = 0; j < 12; j++) {
                cerr << battlefield.map[i][j] << " ";
            }
            
            cerr << endl;
        }
        
        cerr << endl;
        
        for (int i = 0; i < 12; i++) {
            for (int j = 0; j < 12; j++) {
                cerr << battlefield.towersMap[i][j] << " ";
            }
            
            cerr << endl;
        }
        
        cerr << endl;
    }

    void initialization() {
        int numberMineSpots;
        cin >> numberMineSpots; cin.ignore();
        
        for (int i = 0; i < numberMineSpots; i++) {
            int x, y;
            cin >> x >> y; cin.ignore();
            battlefield.inactiveMines.push_back(Position(x, y));
        }
    }

    void update() {
        turn++;
        clear();
        
        // Read turn input
        cin >> myGold >> myIncome >> opponentGold >> opponentIncome; cin.ignore();
        
        // Read battlefield
        for (int i = 0; i < 12; i++) {
            string line;
            cin >> line; cin.ignore();  
            battlefield.map[i] = line;
            battlefield.towersMap[i] = line;
        }
           
        // Read buildings.
        int buildingCount;
        cin >> buildingCount; cin.ignore();
        for (int i = 0; i < buildingCount; i++) {
            int owner, buildingType, x, y;
            cin >> owner >> buildingType >> x >> y; cin.ignore();
            
            if (owner) {
                if (buildingType == 0) {
                    opponentBuildings.HQ = Position(x, y);
                } else if (buildingType == 1) {
                    opponentBuildings.mines.push_back(Position(x, y));
                    if (battlefield.map[y][x] == 'X')
                        battlefield.map[y][x] = 'M';
                } else {
                    opponentBuildings.towers.push_back(Position(x, y));
                    if (battlefield.map[y][x] == 'X')
                        battlefield.towerUpdate(Position(x, y), owner);
                }
            } else {
                if (buildingType == 0) {
                    myBuildings.HQ = Position(x, y);
                    
                    if (turn > 2)
                        battlefield.map[y][x] = '@';
                } else if (buildingType == 1) {
                    myBuildings.mines.push_back(Position(x, y));
                    
                    if (battlefield.map[y][x] == 'O')
                        battlefield.map[y][x] = 'm';
                } else {
                    myBuildings.towers.push_back(Position(x, y));
                    if (battlefield.map[y][x] == 'O')
                        battlefield.towerUpdate(Position(x, y), owner);
                }
            }
        }
                
        // Read units.
        int unitCount;
        cin >> unitCount; cin.ignore();
        for (int i = 0; i < unitCount; i++) {
            int owner, id, level, x, y;
            cin >> owner >> id >> level >> x >> y; cin.ignore();
            battlefield.map[y][x] = level + owner * 6 + '0';
            
            if (owner) {
                if (level == 1)
                    opponentArmy.scouts.push_back(Unit(x, y, id));
                else if (level == 2)
                    opponentArmy.swordsmans.push_back(Unit(x, y, id));
                else
                    opponentArmy.knights.push_back(Unit(x, y, id));
            } else {
                if (level == 1)
                    myArmy.scouts.push_back(Unit(x, y, id));
                else if (level == 2)
                    myArmy.swordsmans.push_back(Unit(x, y, id));
                else
                    myArmy.knights.push_back(Unit(x, y, id));
            }
        }
        
        battlefield.updateBorderLine();
        battlefield.updateMyActiveTerritory();
    }
    
    // Artificial intelligence engine
    void engine() {
        moveScouts();
        moveSwordsmans();
        moveKnights();
        
        battlefield.updateMyActiveTerritory();
        checkAttack();
        
       if ((myGold >= 20 + 4 * myBuildings.mines.size()) && (myBuildings.mines.size() < 1) && (turn > 3))
            buildMine();

        /*if ((myGold >= 20 + 4 * myBuildings.mines.size()) && (myBuildings.mines.size() < 4) && (turn > 17))
            buildMine();*/
        
        if ((myGold >= 15) && (myBuildings.towers.size() < 1) && (turn > 7)) {
            buildTower();
        }
        
        if ((myGold >= 15) && (myBuildings.towers.size() < 6) && (turn > 12)) {
            buildTower();
        }
        
        bool check = true;
                
        if (battlefield.spawnSize() > 0) {
            cerr << endl << battlefield.borderLine.size() << " " << 
            battlefield.myActiveTerritory.size() << " " << 
            battlefield.opponentBorderLine.size() << " " << 
            battlefield.opponentBorderScouts.size() << " " << 
            battlefield.opponentBorderSwordsmans.size() << " " << 
            battlefield.opponentBorderKnights.size() << " " << endl;//*/
            
            if ((turn < 9) || (turn > 10)) {
                while ((myGold >= 10) && (check == true)) {
                    check = false;
                    
                    if ((myArmy.scouts.size() < 15) || (myArmy.scouts.size() < opponentArmy.scouts.size())) {
                        if ((battlefield.borderLine.size() > 0) ||
                            (battlefield.myActiveTerritory.size() > 0) ||
                            (battlefield.opponentBorderLine.size() > 0)) {
                            if (myGold >= 10) {
                                trainScout();
                                check = true;
                            }
                        }
                    }
                }
            }
        }
        
        out();
    }
    
private:
    void checkAttack() {
        if (myBuildings.HQ.x == 0) {
            string attack = "";
            int bestPoint = 200, point;
            Position best, last(1000, 1000);
            bool check = true;
            
            for (auto current: battlefield.myTerritory) {
                point = distance(opponentBuildings.HQ, current);
                
                if (point <= bestPoint) {
                    bestPoint = point;
                    best = current;
                }
            }
            
            cerr << "Distance: " << bestPoint << ":" << best.x << ":" << best.y << endl;
            
            if (bestPoint * 10 > myGold)
                return;
                
            int web[(12 - best.y) * (12 - best.x)][(12 - best.y) * (12 - best.x)];
            
            for (int i = 0; i < (12 - best.y) * (12 - best.x); i++) {            
                for (int j = 0; j < (12 - best.y) * (12 - best.x); j++) {
                    web[i][j] = 0;
                }
            }
            
            vector<Position> grid;
            for (int i = 0; i < 12 - best.y; i++) {
                for (int j = 0; j < 12 - best.x; j++) {
                    int weight = 0;
                    
                    grid.push_back(Position(best.x + j, best.y + i));
                    
                    if (battlefield.map[best.y + i][best.x + j] == '#')
                        continue;
                    
                    if (((j + 1) < (12 - best.x))) {
                        switch (battlefield.towersMap[best.y + i][best.x + j + 1]) {
                            case 'G': weight = 30; break;
                            default:
                                switch (battlefield.map[best.y + i][best.x + j + 1]) {
                                    case '7': weight = 20;  break;
                                    case '8': case '9': case 'T': weight = 30; break;
                                    case '#': weight = 0; break;
                                    default: weight = 10; break;
                                }
                                
                                break;
                        }
                            
                        web[i * (12 - best.x) + j][i * (12 - best.x) + j + 1] = weight;
                    }
                    
                    if ((j - 1) >= 0) {
                        switch (battlefield.towersMap[best.y + i][best.x + j - 1]) {
                            case 'G': weight = 30; break;
                            default:
                                switch (battlefield.map[best.y + i][best.x + j - 1]) {
                                    case '7': weight = 20;  break;
                                    case '8': case '9': case 'T': weight = 30; break;
                                    case '#': weight = 0; break;
                                    default: weight = 10; break;
                                }
                                
                                break;
                        }
                            
                        web[i * (12 - best.x) + j][i * (12 - best.x) + j - 1] = weight;
                    }
                    
                    if ((i + 1) < (12 - best.y)) {
                        switch (battlefield.towersMap[best.y + i + 1][best.x + j]) {
                            case 'G': weight = 30; break;
                            default:
                                switch (battlefield.map[best.y + i + 1][best.x + j]) {
                                    case '7': weight = 20;  break;
                                    case '8': case '9': case 'T': weight = 30; break;
                                    case '#': weight = 0; break;
                                    default: weight = 10; break;
                                }
                                
                                break;
                        }
                            
                        web[i * (12 - best.x) + j][(i + 1) * (12 - best.x) + j] = weight;
                    }
                    
                    if ((i - 1) >= 0) {
                        switch(battlefield.towersMap[best.y + i - 1][best.x + j]) {
                            case 'G': weight = 30; break;
                            default:
                                switch (battlefield.map[best.y + i - 1][best.x + j]) {
                                    case '7': weight = 20;  break;
                                    case '8': case '9': case 'T': weight = 30; break;
                                    case '#': weight = 0; break;
                                    default: weight = 10; break;
                                }
                                
                                break;
                        }
                            
                        web[i * (12 - best.x) + j][(i - 1) * (12 - best.x) + j] = weight;
                    }
                }
            }
            
            for (int i = 0; i < (12 - best.y) * (12 - best.x); i++) {            
                for (int j = 0; j < (12 - best.y) * (12 - best.x); j++) {
                    cerr << web[i][j] << " ";
                }
                
                cerr << endl;
            }
            
            int distance[(12 - best.y) * (12 - best.x)]; // minimal distance
            int bitmask[(12 - best.y) * (12 - best.x)];
            int temp;
            int minIndex, min;
          
            for (int i = 0; i < (12 - best.y) * (12 - best.x); i++) {
                distance[i] = 10000;
                bitmask[i] = 1;
            }
             
            distance[0] = 0;
         
            do {
                minIndex = 10000;
                min = 10000;
                
                for (int i = 0; i < (12 - best.y) * (12 - best.x); i++) {
                    if ((bitmask[i] == 1) && (distance[i] < min)) {
                        min = distance[i];
                        minIndex = i;
                    }
                }
                
                if (minIndex != 10000) {
                    for (int i = 0; i < (12 - best.y) * (12 - best.x); i++) {
                        if (web[minIndex][i] > 0) {
                            temp = min + web[minIndex][i];
                            
                            if (temp < distance[i]) {
                                distance[i] = temp;
                            }
                        }
                    }
                    
                    bitmask[minIndex] = 0;
                }
            } while (minIndex < 10000);
    
            vector<Position> way;
            int picks[(12 - best.y) * (12 - best.x)];
            int end = (12 - best.y) * (12 - best.x) - 1;
            picks[0] = end + 1;
            
            int k = 1;
            int weight = distance[end];
            
            if ((distance[end] != 10000) && (distance[end] <= myGold)) {   
                way.push_back(Position(11, 11));
                while (end > 0) {
                    for (int i = 0; i < (12 - best.y) * (12 - best.x); i++) {
                        if (web[end][i] != 0) {
                            int temp = weight - web[i][end];
                            
                            if (temp == distance[i]) {                 
                                weight = temp;
                                end = i;      
                                picks[k] = i;
                                way.push_back(grid[i]);
                                k++;
                            }
                        }
                    }
                }
                
                for (int i = k - 2; i >= 0; i--) {
                    cerr << picks[i] << " (" << way[i].x << ", " << way[i].y << ");" << endl;
                    switch (battlefield.towersMap[way[i].y][way[i].x]) {
                        case 'G': attack += TRAIN_KNIGHT + to_string(way[i].x) + " " + to_string(way[i].y) + ";"; break;
                        default:
                            switch (battlefield.map[way[i].y][way[i].x]) {
                                case '7': attack += TRAIN_SWORDSMAN + to_string(way[i].x) + " " + to_string(way[i].y) + ";";  break;
                                case '8': case '9': case 'T': attack += TRAIN_KNIGHT + to_string(way[i].x) + " " + to_string(way[i].y) + ";"; break;
                                default: attack += TRAIN_SCOUT + to_string(way[i].x) + " " + to_string(way[i].y) + ";"; break;
                            }
                            
                            break;
                    }
                }
                
                output += attack;
            }
        } else {
            string attack = "";
            int bestPoint = 200, point;
            Position best, last(1000, 1000);
            bool check = true;
            
            for (auto current: battlefield.myTerritory) {
                point = distance(opponentBuildings.HQ, current);
                
                if (point < bestPoint) {
                    bestPoint = point;
                    best = current;
                }
            }
            
            cerr << "Distance: " << bestPoint << ":" << best.x << ":" << best.y << endl;
            
            if (bestPoint * 10 > myGold)
                return;
                
            int web[(best.y + 1) * (best.x + 1)][(best.y + 1) * (best.x + 1)];
            
            for (int i = 0; i < (best.y + 1) * (best.x + 1); i++) {            
                for (int j = 0; j < (best.y + 1) * (best.x + 1); j++) {
                    web[i][j] = 0;
                }
            }
            
            vector<Position> grid;
            
            for (int i = best.y; i >= 0; i--) {
                for (int j = best.x; j >= 0; j--) {
                    int weight = 0;
                    
                    grid.push_back(Position(j, i));
                    
                    if (battlefield.map[i][j] == '#')
                        continue;
                    
                    if ((j + 1) <= best.x) {
                        switch (battlefield.towersMap[i][j + 1]) {
                            case 'G': weight = 30; break;
                            default:
                                switch (battlefield.map[i][j + 1]) {
                                    case '7': weight = 20;  break;
                                    case '8': case '9': case 'T': weight = 30; break;
                                    case '#': weight = 0; break;
                                    default: weight = 10; break;
                                }
                                
                                break;
                        }
                            
                        web[(best.x + 1) * (best.y + 1 - i) - (j + 1)][(best.x + 1) * (best.y + 1 - i) - (j + 2)] = weight;
                    }
                    
                    if ((j - 1) >= 0) {
                        switch (battlefield.towersMap[i][j - 1]) {
                            case 'G': weight = 30; break;
                            default:
                                switch (battlefield.map[i][j - 1]) {
                                    case '7': weight = 20;  break;
                                    case '8': case '9': case 'T': weight = 30; break;
                                    case '#': weight = 0; break;
                                    default: weight = 10; break;
                                }
                                
                                break;
                        }
                            
                        web[(best.x + 1) * (best.y + 1 - i) - (j + 1)][(best.x + 1) * (best.y + 1 - i) - j] = weight;
                    }
                    
                    if ((i + 1) <= best.y) {
                        switch (battlefield.towersMap[i + 1][j]) {
                            case 'G': weight = 30; break;
                            default:
                                switch (battlefield.map[i + 1][j]) {
                                    case '7': weight = 20;  break;
                                    case '8': case '9': case 'T': weight = 30; break;
                                    case '#': weight = 0; break;
                                    default: weight = 10; break;
                                }
                                
                                break;
                        }
                            
                        web[(best.x + 1) * (best.y + 1 - i) - (j + 1)][(best.x + 1) * (best.y - i) - (j + 1)] = weight;
                    }
                    
                    if ((i - 1) >= 0) {
                        switch(battlefield.towersMap[i - 1][j]) {
                            case 'G': weight = 30; break;
                            default:
                                switch (battlefield.map[i - 1][j]) {
                                    case '7': weight = 20;  break;
                                    case '8': case '9': case 'T': weight = 30; break;
                                    case '#': weight = 0; break;
                                    default: weight = 10; break;
                                }
                                
                                break;
                        }
                            
                        web[(best.x + 1) * (best.y + 1 - i) - (j + 1)][(best.x + 1) * (best.y + 2 - i) - (j + 1)] = weight;
                    }
                }
            }
            
            for (int i = 0; i < (best.y + 1) * (best.x + 1); i++) {            
                for (int j = 0; j < (best.y + 1) * (best.x + 1); j++) {
                    cerr << web[i][j] << " ";
                }
                
                cerr << endl;
            }
            
            int distance[(best.y + 1) * (best.x + 1)]; // minimal distance
            int bitmask[(best.y + 1) * (best.x + 1)];
            int temp;
            int minIndex, min;
          
            for (int i = 0; i < (best.y + 1) * (best.x + 1); i++) {
                distance[i] = 10000;
                bitmask[i] = 1;
            }
             
            distance[0] = 0;
         
            do {
                minIndex = 10000;
                min = 10000;
                
                for (int i = 0; i < (best.y + 1) * (best.x + 1); i++) {
                    if ((bitmask[i] == 1) && (distance[i] < min)) {
                        min = distance[i];
                        minIndex = i;
                    }
                }
                
                if (minIndex != 10000) {
                    for (int i = 0; i < (best.y + 1) * (best.x + 1); i++) {
                        if (web[minIndex][i] > 0) {
                            temp = min + web[minIndex][i];
                            
                            if (temp < distance[i]) {
                                distance[i] = temp;
                            }
                        }
                    }
                    
                    bitmask[minIndex] = 0;
                }
            } while (minIndex < 10000);
    
            vector<Position> way;
            int picks[(best.y + 1) * (best.x + 1)];
            int end = (best.y + 1) * (best.x + 1) - 1;
            picks[0] = end + 1;
            
            int k = 1;
            int weight = distance[end];
            
            if ((distance[end] != 10000) && (distance[end] <= myGold)) {   
                way.push_back(Position(0, 0));
                while (end > 0) {
                    for (int i = 0; i < (best.y + 1) * (best.x + 1); i++) {
                        if (web[end][i] != 0) {
                            int temp = weight - web[i][end];
                            
                            if (temp == distance[i]) {                 
                                weight = temp;
                                end = i;      
                                picks[k] = i;
                                way.push_back(grid[i]);
                                k++;
                            }
                        }
                    }
                }
                
                for (int i = k - 2; i >= 0; i--) {
                    cerr << picks[i] << " (" << way[i].x << ", " << way[i].y << ");" << endl;
                    switch (battlefield.towersMap[way[i].y][way[i].x]) {
                        case 'G': attack += TRAIN_KNIGHT + to_string(way[i].x) + " " + to_string(way[i].y) + ";"; break;
                        default:
                            switch (battlefield.map[way[i].y][way[i].x]) {
                                case '7': attack += TRAIN_SWORDSMAN + to_string(way[i].x) + " " + to_string(way[i].y) + ";";  break;
                                case '8': case '9': case 'T': attack += TRAIN_KNIGHT + to_string(way[i].x) + " " + to_string(way[i].y) + ";"; break;
                                default: attack += TRAIN_SCOUT + to_string(way[i].x) + " " + to_string(way[i].y) + ";"; break;
                            }
                            
                            break;
                    }
                }
                
                output += attack;
            }
        }
    }
    
    void clear() {
        myArmy.clear(); opponentArmy.clear();
        myBuildings.clear(); opponentBuildings.clear();
        output.clear();
    }
    
    void out() {
        cout << output << WAIT << endl;
    }
    
    void buildTower() {
        Position target = findBuildingPlaceTower();
        output += BUILD_TOWER + to_string(target.x) + " " + to_string(target.y) + ";";
        battlefield.map[target.x][target.y] = 't';
        battlefield.updateBorderLine();
        battlefield.towerUpdate(target, 0);
        battlefield.updateMyActiveTerritory();
        myBuildings.towers.push_back(target);
                
        myGold -= 15;
    }
    
    Position findBuildingPlaceTower() {
        int point, bestPoint = 0;
        Position best;
        char target = 'O';
                
        for (auto current: battlefield.myActiveTerritory) {
            if (battlefield.map[current.y][current.x] == 'O') {
                point = distance(myBuildings.HQ, current);
                    
                if (look(current, WEST, target))
                    point += 2;                
                if (look(current, EAST, target))
                    point += 2;
                if (look(current, NORTH, target))
                    point += 2;
                if (look(current, SOUTH, target))
                    point += 2;
                    
                if (point > bestPoint) {
                    bestPoint = point;
                    best = current;
                }
            }
        }
        
        return best;
    }   
                
    void buildMine() {
        for (int i = 0; i < battlefield.inactiveMines.size(); i++) {
            if (battlefield.map[battlefield.inactiveMines[i].y][battlefield.inactiveMines[i].x] == 'O') {
                myGold -= 20 + 4 * myBuildings.mines.size();
                output += BUILD_MINE + to_string(battlefield.inactiveMines[i].x) + " " + to_string(battlefield.inactiveMines[i].y) + ";";
                myBuildings.mines.push_back(battlefield.inactiveMines[i]);
                battlefield.map[battlefield.inactiveMines[i].y][battlefield.inactiveMines[i].x] = 'm';
                battlefield.updateBorderLine();
                battlefield.updateMyActiveTerritory();
                battlefield.inactiveMines.erase(battlefield.inactiveMines.begin() + i);
                
                return;
            }
        }
    }
                
    Position scan(Position current, int level) {
        int top = 10, right = 10, bottom = 10, left = 10, temp;
        bool checkTop = true, checkRight = true, checkBottom = true, checkLeft = true;
        
        if (level == 3) { 
            // Distance == 1 // top
            if (isBattlefield(Position(current.x + NORTH.x, current.y + NORTH.y))) {
                switch(battlefield.map[current.y + NORTH.y][current.x + NORTH.x]) {
                case '#': case '@': case '1': case '2': case '3': case 'm': case 't': checkTop = false; top -= 10000; break;
                case '.': top += 2; break;
                case 'o': top += 2; break;
                case 'O': top -= 2; break;
                case 'x': top += 2; break;
                case 'X': top += 3; break;
                case 'M': top += 7; break;
                case '7': top += 5; break;
                case '8': top += 8; break;
                case '9': return Position(current.y + NORTH.y, current.x + NORTH.x); break;
                case 'T': top += 6; break;
                }
                
                if (checkTop) {
                    // Distance == 2 // top
                    for (int i = -1; i < 2; i++) {
                        int j = -2;
                        
                        if (abs(i) == 1)
                            j = -1;
                            
                        if (!isBattlefield(Position(current.x + i, current.y + j)))
                            continue;
                        
                        switch(battlefield.map[current.y + j][current.x + i]) {
                        case '#': top -= 1; break;
                        case '.': top += 1; break;
                        case 'o': top += 1; break;
                        case 'O': top -= 1; break;
                        case 'x': top += 1; break;
                        case 'X': top += 2; break;
                        case 'M': top += 4; break;
                        case '7': top += 3; break;
                        case '8': top += 5; break;
                        case '9': return current; break;
                        case 'T': top += 2; break;
                        }
                    }
                    
                    // Distance == 3 // top
                    for (int i = -2; i < 3; i++) {
                        int j = -3;
                        
                        if (abs(i) == 2)
                            j = -1;
                        else if (abs(i) == 1)
                            j = -2;
                            
                        if (!isBattlefield(Position(current.x + i, current.y + j)))
                            continue;
                        
                        switch(battlefield.map[current.y + j][current.x + i]) {
                        case '#': top -= 1; break;
                        case 'O': top -= 1; break;
                        case 'X': top += 1; break;
                        case 'M': top += 3; break;
                        case '7': top += 2; break;
                        case '8': top += 3; break;
                        case '9': top += 5; break;
                        case 'T': top += 1; break;
                        }
                    }
                }
            } else {
                top -= 10000;
            }
            
            // Distance == 1 // right
            if (isBattlefield(Position(current.x + EAST.x, current.y + EAST.y))) {
                switch(battlefield.map[current.y + EAST.y][current.x + EAST.x]) {
                case '#': case '@': case '1': case '2': case '3': case 'm': case 't': checkRight = false; right -= 10000; break;
                case '.': right += 2; break;
                case 'o': right += 2; break;
                case 'O': right -= 2; break;
                case 'x': right += 2; break;
                case 'X': right += 3; break;
                case 'M': right += 7; break;
                case '7': right += 5; break;
                case '8': right += 8; break;
                case '9': return Position(current.y + EAST.y, current.x + EAST.x); break;
                case 'T': right += 6; break;
            }
                
                if (checkRight) {
                    // Distance == 2 // right
                    for (int j = -1; j < 2; j++) {
                        int i = 2;
                        
                        if (abs(j) == 1)
                            i = 1;
                            
                        if (!isBattlefield(Position(current.x + i, current.y + j)))
                            continue;
                        
                        switch(battlefield.map[current.y + j][current.x + i]) {
                        case '#': right -= 1; break;
                        case '.': right += 1; break;
                        case 'o': right += 1; break;
                        case 'O': right -= 1; break;
                        case 'x': right += 1; break;
                        case 'X': right += 2; break;
                        case 'M': right += 4; break;
                        case '7': right += 3; break;
                        case '8': right += 5; break;
                        case '9': return current; break;
                        case 'T': right += 2; break;
                        }
                    }
                    
                    // Distance == 3 // right
                    for (int j = -2; j < 3; j++) {
                        int i = 3;
                        
                        if (abs(j) == 2)
                            i = 1;
                        else if (abs(j) == 1)
                            i = 2;
                        
                        if (!isBattlefield(Position(current.x + i, current.y + j)))
                            continue;
                        
                        switch(battlefield.map[current.y + j][current.x + i]) {
                        case '#': right -= 1; break;
                        case 'O': right -= 1; break;
                        case 'X': right += 1; break;
                        case 'M': right += 3; break;
                        case '7': right += 2; break;
                        case '8': right += 3; break;
                        case '9': right += 5; break;
                        case 'T': right += 1; break;
                        }
                    }
                }
            } else {
                right -= 10000;
            }
                
            // Distance == 1 // bottom
            if (isBattlefield(Position(current.x + SOUTH.x, current.y + SOUTH.y))) {
                switch(battlefield.map[current.y + SOUTH.y][current.x + SOUTH.x]) {
                case '#': case '@': case '1': case '2': case '3': case 'm': case 't': checkBottom = false; bottom = -10000; break;
                case '.': bottom += 2; break;
                case 'o': bottom += 2; break;
                case 'O': bottom -= 2; break;
                case 'x': bottom += 2; break;
                case 'X': bottom += 3; break;
                case 'M': bottom += 7; break;
                case '7': bottom += 5; break;
                case '8': bottom += 8; break;
                case '9': return Position(current.y + SOUTH.y, current.x + SOUTH.x); break;
                case 'T': bottom += 6; break;
                }
                
                if (checkBottom) {
                    // Distance == 2 // bottom
                    for (int i = -1; i < 2; i++) {
                        int j = 2;
                        
                        if (abs(i) == 1)
                            j = 1;
                            
                        if (!isBattlefield(Position(current.x + i, current.y + j)))
                            continue;
                        
                        switch(battlefield.map[current.y + j][current.x + i]) {
                        case '#': bottom -= 1; break;
                        case '.': bottom += 1; break;
                        case 'o': bottom += 1; break;
                        case 'O': bottom -= 1; break;
                        case 'x': bottom += 1; break;
                        case 'X': bottom += 2; break;
                        case 'M': bottom += 4; break;
                        case '7': bottom += 3; break;
                        case '8': bottom += 5; break;
                        case '9': return current; break;
                        case 'T': bottom += 2; break;
                        }
                    }
                    
                    // Distance == 3 // bottom
                    for (int i = -2; i < 3; i++) {
                        int j = 3;
                        
                        if (abs(i) == 2)
                            j = 1;
                        else if (abs(i) == 1)
                            j = 2;
                        
                        if (!isBattlefield(Position(current.x + i, current.y + j)))
                            continue;
                        
                        switch(battlefield.map[current.y + j][current.x + i]) {
                        case '#': bottom -= 1; break;
                        case 'O': bottom -= 1; break;
                        case 'X': bottom += 1; break;
                        case 'M': bottom += 3; break;
                        case '7': bottom += 2; break;
                        case '8': bottom += 3; break;
                        case '9': bottom += 5; break;
                        case 'T': bottom += 1; break;
                        }
                    }
                }
            } else {
                bottom -= 10000;
            }
            
            // Distance == 1 // left
            if (isBattlefield(Position(current.x + WEST.x, current.y + WEST.y))) {
                switch(battlefield.map[current.y + WEST.y][current.x + WEST.x]) {
                case '#': case '@': case '1': case '2': case '3': case 'm': case 't': checkLeft = false; left = -10000; break;
                case '.': left += 2; break;
                case 'o': left += 2; break;
                case 'O': left -= 2; break;
                case 'x': left += 2; break;
                case 'X': left += 3; break;
                case 'M': left += 7; break;
                case '7': left += 5; break;
                case '8': left += 8; break;
                case '9': return Position(current.y + WEST.y, current.x + WEST.x); break;
                case 'T': left += 6; break;
                }
                
                if (checkLeft) {
                    // Distance == 2 // left
                    for (int j = -1; j < 2; j++) {
                        int i = -2;
                        
                        if (abs(j) == 1)
                            i = -1;
                        
                        if (!isBattlefield(Position(current.x + i, current.y + j)))
                            continue;
                        
                        switch(battlefield.map[current.y + j][current.x + i]) {
                        case '#': left -= 1; break;
                        case '.': left += 1; break;
                        case 'o': left += 1; break;
                        case 'O': left -= 1; break;
                        case 'x': left += 1; break;
                        case 'X': left += 2; break;
                        case 'M': left += 4; break;
                        case '7': left += 3; break;
                        case '8': left += 5; break;
                        case '9': return current; break;
                        case 'T': left += 2; break;
                        }
                    }
                    
                    // Distance == 3 // left
                    for (int j = -2; j < 3; j++) {
                        int i = -3;
                        
                        if (abs(j) == 2)
                            i = -1;
                        else if (abs(j) == 1)
                            i = -2;
                        
                        if (!isBattlefield(Position(current.x + i, current.y + j)))
                            continue;
                        
                        switch(battlefield.map[current.y + j][current.x + i]) {
                        case '#': left -= 1; break;
                        case 'O': left -= 1; break;
                        case 'X': left += 1; break;
                        case 'M': left += 3; break;
                        case '7': left += 2; break;
                        case '8': left += 3; break;
                        case '9': left += 5; break;
                        case 'T': left += 1; break;
                        }
                    }
                }
            } else {
                left -= 10000;
            }
                
            temp = max(max(top, bottom), max(right, left));
            
            if (temp < 0)
                return current;
                
            if (myBuildings.HQ.x == 0) {
                if (right == temp)
                    return Position(current.x + EAST.x, current.y + EAST.y);
                if (bottom == temp)
                    return Position(current.x + SOUTH.x, current.y + SOUTH.y);
                if (top == temp)
                    return Position(current.x + NORTH.x, current.y + NORTH.y);
                if (left == temp)
                    return Position(current.x + WEST.x, current.y + WEST.y);
            } else {   
                if (left == temp)
                    return Position(current.x + WEST.x, current.y + WEST.y);
                if (top == temp)
                    return Position(current.x + NORTH.x, current.y + NORTH.y);
                if (bottom == temp)
                    return Position(current.x + SOUTH.x, current.y + SOUTH.y);
                if (right == temp)
                    return Position(current.x + EAST.x, current.y + EAST.y);
            }       
        } else if (level == 2) {    
            // Distance == 1 // top
            if (isBattlefield(Position(current.x + NORTH.x, current.y + NORTH.y))) {
                if (battlefield.towersMap[current.y + NORTH.y][current.x + NORTH.x] == 'G') {
                    checkTop = false; top -= 10000;
                }
                
                if (checkTop) {
                    switch(battlefield.map[current.y + NORTH.y][current.x + NORTH.x]) {
                    case '8': case '#': case '@': case '1': case '2': case '3': case 'm': case 't': case '9': case 'T': checkTop = false; top -= 10000; break;
                    case '.': top += 2; break;
                    case 'o': top += 2; break;
                    case 'O': top -= 2; break;
                    case 'x': top += 2; break;
                    case 'X': top += 3; break;
                    case 'M': top += 4; break;
                    case '7': top += 8; break;
                    //case '8': return Position(current.y + NORTH.y, current.x + NORTH.x);; break;
                    }
                    
                    if (checkTop) {
                        // Distance == 2 // top
                        for (int i = -1; i < 2; i++) {
                            int j = -2;
                            
                            if (abs(i) == 1)
                                j = -1;
                            
                            if (!isBattlefield(Position(current.x + i, current.y + j)))
                                continue;
                            
                            switch(battlefield.map[current.y + j][current.x + i]) {
                            case '#': top -= 1; break;
                            case '.': top += 1; break;
                            case 'o': top += 1; break;
                            case 'O': top -= 1; break;
                            case 'x': top += 1; break;
                            case 'X': top += 2; break;
                            case 'M': top += 3; break;
                            case '7': top += 4; break;
                            //case '8': return current; break;
                            case '9': top -= 10000; break;
                            }
                        }
                        
                        // Distance == 3 // top
                        for (int i = -2; i < 3; i++) {
                            int j = -3;
                            
                            if (abs(i) == 2)
                                j = -1;
                            else if (abs(i) == 1)
                                j = -2;
                            
                            if (!isBattlefield(Position(current.x + i, current.y + j)))
                                continue;
                            
                            switch(battlefield.map[current.y + j][current.x + i]) {
                            case '#': top -= 1; break;
                            case 'O': top -= 1; break;
                            case 'X': top += 1; break;
                            case 'M': top += 1; break;
                            case '7': top += 2; break;
                            case '8': top += 5; break;
                            case '9': top -= 3; break;
                            }
                        }
                    }
                }
            } else {
                top -= 10000;
            }
            
            // Distance == 1 // right
            if (isBattlefield(Position(current.x + EAST.x, current.y + EAST.y))) {
                if (battlefield.towersMap[current.y + EAST.y][current.x + EAST.x]  == 'G') {
                    checkRight = false; right -= 10000;
                }
                    
                if (checkRight) {
                    switch(battlefield.map[current.y + EAST.y][current.x + EAST.x]) {
                    case '8': case '#': case '@': case '1': case '2': case '3': case 'm': case 't': case '9': case 'T': checkRight = false; right -= 10000; break;
                    case '.': right += 2; break;
                    case 'o': right += 2; break;
                    case 'O': right -= 2; break;
                    case 'x': right += 2; break;
                    case 'X': right += 3; break;
                    case 'M': right += 4; break;
                    case '7': right += 8; break;
                    //case '8': return Position(current.y + EAST.y, current.x + EAST.x); break;
                    }
                    
                    if (checkRight) {
                        // Distance == 2 // right
                        for (int j = -1; j < 2; j++) {
                            int i = 2;
                            
                            if (abs(j) == 1)
                                i = 1;
                            
                            if (!isBattlefield(Position(current.x + i, current.y + j)))
                                continue;
                            
                            switch(battlefield.map[current.y + j][current.x + i]) {
                            case '#': right -= 1; break;
                            case '.': right += 1; break;
                            case 'o': right += 1; break;
                            case 'O': right -= 1; break;
                            case 'x': right += 1; break;
                            case 'X': right += 2; break;
                            case 'M': right += 3; break;
                            case '7': right += 4; break;
                            //case '8': return current; break;
                            case '9': right -= 10000; break;
                            }
                        }
                        
                        // Distance == 3 // right
                        for (int j = -2; j < 3; j++) {
                            int i = 3;
                            
                            if (abs(j) == 2)
                                i = 1;
                            else if (abs(j) == 1)
                                i = 2;
                            
                            if (!isBattlefield(Position(current.x + i, current.y + j)))
                                continue;
                            
                            switch(battlefield.map[current.y + j][current.x + i]) {
                            case '#': right -= 1; break;
                            case 'O': right -= 1; break;
                            case 'X': right += 1; break;
                            case 'M': right += 1; break;
                            case '7': right += 2; break;
                            case '8': right += 5; break;
                            case '9': right -= 3; break;
                            }
                        }
                    }
                }
            } else {
                right -= 10000;
            }
            
            // Distance == 1 // bottom
            if (isBattlefield(Position(current.x + SOUTH.x, current.y + SOUTH.y))) {
                if (battlefield.towersMap[current.y + SOUTH.y][current.x + SOUTH.x]  == 'G') {
                    checkBottom = false; bottom -= 10000;
                }
                       
                if (checkBottom) {
                    switch(battlefield.map[current.y + SOUTH.y][current.x + SOUTH.x]) {
                    case '8': case '#': case '@': case '1': case '2': case '3': case 'm': case 't': case '9': case 'T': checkBottom = false; bottom -= 10000; break;
                    case '.': bottom += 2; break;
                    case 'o': bottom += 2; break;
                    case 'O': bottom -= 2; break;
                    case 'x': bottom += 2; break;
                    case 'X': bottom += 3; break;
                    case 'M': bottom += 4; break;
                    case '7': bottom += 8; break;
                    //case '8': return Position(current.y + SOUTH.y, current.x + SOUTH.x); break;
                    }
                    
                    if (checkBottom) {
                        // Distance == 2 // bottom
                        for (int i = -1; i < 2; i++) {
                            int j = 2;
                            
                            if (abs(i) == 1)
                                j = 1;
                            
                            if (!isBattlefield(Position(current.x + i, current.y + j)))
                                continue;
                            
                            switch(battlefield.map[current.y + j][current.x + i]) {
                            case '#': bottom -= 1; break;
                            case '.': bottom += 1; break;
                            case 'o': bottom += 1; break;
                            case 'O': bottom -= 1; break;
                            case 'x': bottom += 1; break;
                            case 'X': bottom += 2; break;
                            case 'M': bottom += 3; break;
                            case '7': bottom += 4; break;
                            //case '8': return current; break;
                            case '9': bottom -= 10000; break;
                            }
                        }
                        
                        // Distance == 3 // bottom
                        for (int i = -2; i < 3; i++) {
                            int j =-3;
                            
                            if (abs(i) == 2)
                                j = 1;
                            else if (abs(i) == 1)
                                j = 2;
                            
                            if (!isBattlefield(Position(current.x + i, current.y + j)))
                                continue;
                            
                            switch(battlefield.map[current.y + j][current.x + i]) {
                            case '#': bottom -= 1; break;
                            case 'O': bottom -= 1; break;
                            case 'X': bottom += 1; break;
                            case 'M': bottom += 1; break;
                            case '7': bottom += 2; break;
                            case '8': bottom += 5; break;
                            case '9': bottom -= 3; break;
                            }
                        }
                    }
                }
            } else {
                bottom -= 10000;
            }
            
            // Distance == 1 // left
            if (isBattlefield(Position(current.x + WEST.x, current.y + WEST.y))) {
                if (battlefield.towersMap[current.y + WEST.y][current.x + WEST.x]  == 'G') {
                    checkLeft = false; left -= 10000;
                }
                       
                if (checkLeft) {
                    switch(battlefield.map[current.y + WEST.y][current.x + WEST.x]) {
                    case '8': case '#': case '@': case '1': case '2': case '3': case 'm': case 't': case '9': case 'T': checkLeft = false; left -= 10000; break;
                    case '.': left += 2; break;
                    case 'o': left += 2; break;
                    case 'O': left -= 2; break;
                    case 'x': left += 2; break;
                    case 'X': left += 3; break;
                    case 'M': left += 4; break;
                    case '7': left += 8; break;
                    //case '8': return Position(current.y + WEST.y, current.x + WEST.x); break;
                    }
                    
                    if (checkLeft) {
                        // Distance == 2 // left
                        for (int j = -1; j < 2; j++) {
                            int i = -2;
                            
                            if (abs(j) == 1)
                                i = -1;
                                
                            if (!isBattlefield(Position(current.x + i, current.y + j)))
                                continue;
                            
                            switch(battlefield.map[current.y + j][current.x + i]) {
                            case '#': left -= 1; break;
                            case '.': left += 1; break;
                            case 'o': left += 1; break;
                            case 'O': left -= 1; break;
                            case 'x': left += 1; break;
                            case 'X': left += 2; break;
                            case 'M': left += 3; break;
                            case '7': left += 4; break;
                            //case '8': return current; break;
                            case '9': left -= 10000; break;
                            }
                        }
                        
                        // Distance == 3 // left
                        for (int j = -2; j < 3; j++) {
                            int i = -3;
                            
                            if (abs(j) == 2)
                                i = -1;
                            else if (abs(j) == 1)
                                i = -2;
                            
                            if (!isBattlefield(Position(current.x + i, current.y + j)))
                                continue;
                            
                            switch(battlefield.map[current.y + j][current.x + i]) {
                            case '#': left -= 1; break;
                            case 'O': left -= 1; break;
                            case 'X': left += 1; break;
                            case 'M': left += 1; break;
                            case '7': left += 2; break;
                            case '8': left += 5; break;
                            case '9': left -= 3; break;
                            }
                        }
                    }
                }
            } else {
                left -= 10000;
            }
            
            cerr << bottom;
            
            temp = max(max(top, bottom), max(right, left));
            
            if (temp < 0)
                return current;
                
            if (myBuildings.HQ.x == 0) {
                if (right == temp)
                    return Position(current.x + EAST.x, current.y + EAST.y);
                if (bottom == temp)
                    return Position(current.x + SOUTH.x, current.y + SOUTH.y);
                if (top == temp)
                    return Position(current.x + NORTH.x, current.y + NORTH.y);
                if (left == temp)
                    return Position(current.x + WEST.x, current.y + WEST.y);
            } else {   
                if (left == temp)
                    return Position(current.x + WEST.x, current.y + WEST.y);
                if (top == temp)
                    return Position(current.x + NORTH.x, current.y + NORTH.y);
                if (bottom == temp)
                    return Position(current.x + SOUTH.x, current.y + SOUTH.y);
                if (right == temp)
                    return Position(current.x + EAST.x, current.y + EAST.y);
            }     
        } else {
            // Distance == 1 // top
            if (isBattlefield(Position(current.x + NORTH.x, current.y + NORTH.y))) {
                if (battlefield.towersMap[current.y + NORTH.y][current.x + NORTH.x] == 'G') {
                    checkTop = false; top -= 10000;
                }
                
                if (checkTop) {
                    switch(battlefield.map[current.y + NORTH.y][current.x + NORTH.x]) {
                    case '#': case '@': case '1': case '2': case '3': case 'm': case 't': case '7': case '8': case '9': case 'T': checkTop = false; top -= 10000; break;
                    case '.': top += 3; break;
                    case 'o': top += 3; break;
                    case 'O': top -= 5; break;
                    case 'x': top += 3; break;
                    case 'X': top += 5; break;
                    case 'M': top += 6; break;
                    }
                    
                    if (checkTop) {
                        // Distance == 2 // top
                        for (int i = -1; i < 2; i++) {
                            int j = -2;
                            
                            if (abs(i) == 1)
                                j = -1;
                                
                            if (!isBattlefield(Position(current.x + i, current.y + j)))
                                continue;    
                            
                            switch(battlefield.map[current.y + j][current.x + i]) {
                            case '#': top -= 1; break;
                            case '.': top += 3; break;
                            case 'o': top += 1; break;
                            case 'O': top -= 3; break;
                            case 'x': top += 2; break;
                            case 'X': top += 4; break;
                            case 'M': top += 5; break;
                            case '8': top -= 4; break;
                            case '9': top -= 4; break;
                            }
                        }
                        
                        // Distance == 3 // top
                        for (int i = -2; i < 3; i++) {
                            int j = -3;
                            
                            if (abs(i) == 2)
                                j = -1;
                            else if (abs(i) == 1)
                                j = -2;
                                
                            if (!isBattlefield(Position(current.x + i, current.y + j)))
                                continue;
                                
                            switch(battlefield.map[current.y + j][current.x + i]) {
                            case '#': top -= 1; break;
                            case 'O': top -= 1; break;
                            case '.': top += 1; break;
                            case 'X': top += 1; break;
                            case 'M': top += 2; break;
                            case '8': top -= 1; break;
                            case '9': top -= 1; break;
                            }
                        }
                    }
                }
            } else {
                top -= 10000;
            }
            
            // Distance == 1 // right
            if (isBattlefield(Position(current.x + EAST.x, current.y + EAST.y))) {
                if (battlefield.towersMap[current.y + EAST.y][current.x + EAST.x]  == 'G') {
                    checkRight = false; right -= 10000;
                }
                    
                if (checkRight) {
                    switch(battlefield.map[current.y + EAST.y][current.x + EAST.x]) {
                    case '#': case '@': case '1': case '2': case '3': case 'm': case 't': case '7': case '8': case '9': case 'T': checkRight = false; right -= 10000; break;
                    case '.': right += 3; break;
                    case 'o': right += 3; break;
                    case 'O': right -= 5; break;
                    case 'x': right += 3; break;
                    case 'X': right += 5; break;
                    case 'M': right += 6; break;
                    }
                    
                    if (checkRight) {
                        // Distance == 2 // right
                        for (int j = -1; j < 2; j++) {
                            int i = 2;
                            
                            if (abs(j) == 1)
                                i = 1;
                                
                            if (!isBattlefield(Position(current.x + i, current.y + j)))
                                continue;
                            
                            switch(battlefield.map[current.y + j][current.x + i]) {
                            case '#': right -= 1; break;
                            case '.': right += 3; break;
                            case 'o': right += 1; break;
                            case 'O': right -= 3; break;
                            case 'x': right += 2; break;
                            case 'X': right += 4; break;
                            case 'M': right += 5; break;
                            case '8': right -= 4; break;
                            case '9': right -= 4; break;
                            }
                        }
                        
                        // Distance == 3 // right
                        for (int j = -2; j < 3; j++) {
                            int i = 3;
                            
                            if (abs(j) == 2)
                                i = 1;
                            else if (abs(j) == 1)
                                i = 2;
                                
                            if (!isBattlefield(Position(current.x + i, current.y + j)))
                                continue;
                            
                            switch(battlefield.map[current.y + j][current.x + i]) {
                            case '#': right -= 1; break;
                            case 'O': right -= 1; break;
                            case '.': right += 1; break;
                            case 'X': right += 1; break;
                            case 'M': right += 2; break;
                            case '8': right -= 1; break;
                            case '9': right -= 1; break;
                            }
                        }
                    }
                }
            } else {
                right -= 10000;
            }
            
            // Distance == 1 // bottom
            if (isBattlefield(Position(current.x + SOUTH.x, current.y + SOUTH.y))) {
                if (battlefield.towersMap[current.y + SOUTH.y][current.x + SOUTH.x]  == 'G') {
                    checkBottom = false; bottom -= 10000;
                }
                       
                if (checkBottom) {
                    switch(battlefield.map[current.y + SOUTH.y][current.x + SOUTH.x]) {
                    case '#': case '@': case '1': case '2': case '3': case 'm': case 't': case '7': case '8': case '9': case 'T': checkBottom = false; bottom = -10000; break;
                    case '.': bottom += 3; break;
                    case 'o': bottom += 3; break;
                    case 'O': bottom -= 5; break;
                    case 'x': bottom += 3; break;
                    case 'X': bottom += 5; break;
                    case 'M': bottom += 6; break;
                    }
                    
                    if (checkBottom) {
                        // Distance == 2 // bottom
                        for (int i = -1; i < 2; i++) {
                            int j = 2;
                            
                            if (abs(i) == 1)
                                j = 1;
                            
                            if (!isBattlefield(Position(current.x + i, current.y + j)))
                                continue;
                            
                            switch(battlefield.map[current.y + j][current.x + i]) {
                            case '#': bottom -= 1; break;
                            case '.': bottom += 3; break;
                            case 'o': bottom += 1; break;
                            case 'O': bottom -= 3; break;
                            case 'x': bottom += 2; break;
                            case 'X': bottom += 4; break;
                            case 'M': bottom += 5; break;
                            case '8': bottom -= 4; break;
                            case '9': bottom -= 4; break;
                            }
                        }
                        
                        // Distance == 3 // bottom
                        for (int i = -2; i < 3; i++) {
                            int j = 3;
                            
                            if (abs(i) == 2)
                                j = 1;
                            else if (abs(i) == 1)
                                j = 2;
                            
                            if (!isBattlefield(Position(current.x + i, current.y + j)))
                                continue;
                            
                            switch(battlefield.map[current.y + j][current.x + i]) {
                            case '#': bottom -= 1; break;
                            case 'O': bottom -= 1; break;
                            case '.': bottom += 1; break;
                            case 'X': bottom += 1; break;
                            case 'M': bottom += 2; break;
                            case '8': bottom -= 1; break;
                            case '9': bottom -= 1; break;
                            }
                        }
                    }
                }
            } else {
                bottom -= 10000;
            }
            
            // Distance == 1 // left
            if (isBattlefield(Position(current.x + WEST.x, current.y + WEST.y))) {
                if (battlefield.towersMap[current.y + WEST.y][current.x + WEST.x]  == 'G') {
                    checkLeft = false; left -= 10000;
                }
                       
                if (checkLeft) {
                    switch(battlefield.map[current.y + WEST.y][current.x + WEST.x]) {
                    case '#': case '@': case '1': case '2': case '3': case 'm': case 't': case '7': case '8': case '9': case 'T': checkLeft = false; left = -10000; break;
                    case '.': left += 3; break;
                    case 'o': left += 3; break;
                    case 'O': left -= 5; break;
                    case 'x': left += 3; break;
                    case 'X': left += 5; break;
                    case 'M': left += 6; break;
                    }
                    
                    if (checkLeft) {
                        // Distance == 2 // left
                        for (int j = -1; j < 2; j++) {
                            int i = -2;
                            
                            if (abs(j) == 1)
                                i = -1;
                                
                            if (!isBattlefield(Position(current.x + i, current.y + j)))
                                continue;
                            
                            switch(battlefield.map[current.y + j][current.x + i]) {
                            case '#': left -= 1; break;
                            case '.': left += 3; break;
                            case 'o': left += 1; break;
                            case 'O': left -= 3; break;
                            case 'x': left += 2; break;
                            case 'X': left += 4; break;
                            case 'M': left += 5; break;
                            case '8': left -= 4; break;
                            case '9': left -= 4; break;
                            }
                        }
                        
                        // Distance == 3 // left
                        for (int j = -2; j < 3; j++) {
                            int i = -3;
                            
                            if (abs(j) == 2)
                                i = -1;
                            else if (abs(j) == 1)
                                i = -2;
                                
                            if (!isBattlefield(Position(current.x + i, current.y + j)))
                                continue;
                            
                            switch(battlefield.map[current.y + j][current.x + i]) {
                            case '#': left -= 1; break;
                            case 'O': left -= 1; break;
                            case '.': left += 1; break;
                            case 'X': left += 1; break;
                            case 'M': left += 2; break;
                            case '8': left -= 1; break;
                            case '9': left -= 1; break;
                            }
                        }
                    }
                }
            } else {
                left -= 10000;
            }
            
            temp = max(max(top, bottom), max(right, left));
            
            if (temp < 0)
                return current;
            
            if (myBuildings.HQ.x == 0) {
                if (right == temp)
                    return Position(current.x + EAST.x, current.y + EAST.y);
                if (bottom == temp)
                    return Position(current.x + SOUTH.x, current.y + SOUTH.y);
                if (top == temp)
                    return Position(current.x + NORTH.x, current.y + NORTH.y);
                if (left == temp)
                    return Position(current.x + WEST.x, current.y + WEST.y);
            } else {   
                if (left == temp)
                    return Position(current.x + WEST.x, current.y + WEST.y);
                if (top == temp)
                    return Position(current.x + NORTH.x, current.y + NORTH.y);
                if (bottom == temp)
                    return Position(current.x + SOUTH.x, current.y + SOUTH.y);
                if (right == temp)
                    return Position(current.x + EAST.x, current.y + EAST.y);
            }        
        }
    }
    
    bool look(Position current, Position swift, char target) {
            return isBattlefield(Position(current.x + swift.x, current.y + swift.y)) &&
               battlefield.map[current.y + swift.y][current.x + swift.x] == target;
    }
        
    Position findTrainPosition(int level) {
        int bestPoint = 0, bestPointMax = 200, point;
        char target = '.';
        Position best;
        
        if (level == 1) {   
            if (battlefield.opponentBorderLine.size() > 0) {
                for (auto current: battlefield.opponentBorderLine) {
                    point = distance(opponentBuildings.HQ, current);
                    
                    if (point < bestPointMax) {
                        bestPointMax = point;
                        best = current;
                    }
                }
                
                if (bestPointMax != 0)
                    return best;
            }
                        
            if (battlefield.borderLine.size() > 0) {
                for (auto current: battlefield.borderLine) {
                    point = distance(myBuildings.HQ, current);
                    
                    if (look(current, WEST, target))
                        point += 2;                
                    if (look(current, EAST, target))
                        point += 2;
                    if (look(current, NORTH, target))
                        point += 2;
                    if (look(current, SOUTH, target))
                        point += 2;
                        
                    if (point > bestPoint) {
                        bestPoint = point;
                        best = current;
                    }
                }
                
                if (bestPoint != 0)
                    return best;
            }
            
            for (auto current: battlefield.myActiveTerritory) {
                point = distance(myBuildings.HQ, current);
                        
                if (point > bestPoint) {
                    bestPoint = point;
                    best = current;
                }
            }
            
            if (bestPoint != 0)
                return best;
        } else if (level == 2) {
            if (battlefield.opponentBorderScouts.size() > 0) {
                for (auto current: battlefield.opponentBorderScouts) {
                    point = distance(myBuildings.HQ, current);
                    
                    if (point <= bestPointMax) {
                        bestPointMax = point;
                        best = current;
                    }
                }
                
                if (bestPointMax != 200)
                    return best;
            }
                
            if (battlefield.opponentBorderLine.size() > 0) {
                for (auto current: battlefield.opponentBorderLine) {
                    point = distance(myBuildings.HQ, current);
                    
                    if (point > bestPoint) {
                        bestPoint = point;
                        best = current;
                    }
                }
                
                if (bestPoint != 0)
                    return best;
            }
                        
            if (battlefield.borderLine.size() > 0) {
                for (auto current: battlefield.borderLine) {
                    point = distance(myBuildings.HQ, current);
                    
                    if (look(current, WEST, target))
                        point += 2;                
                    if (look(current, EAST, target))
                        point += 2;
                    if (look(current, NORTH, target))
                        point += 2;
                    if (look(current, SOUTH, target))
                        point += 2;
                        
                    if (point > bestPoint) {
                        bestPoint = point;
                        best = current;
                    }
                }
                
                if (bestPoint != 0)
                    return best;
            }
            
            for (auto current: battlefield.myActiveTerritory) {
                point = distance(myBuildings.HQ, current);
                        
                if (point > bestPoint) {
                    bestPoint = point;
                    best = current;
                }
            }
            
            if (bestPoint != 0)
                return best;
        } else if (level == 3) {
            if (battlefield.opponentBorderKnights.size() > 0) {
                for (auto current: battlefield.opponentBorderKnights) {
                    point = distance(myBuildings.HQ, current);
                    
                    if (point <= bestPointMax) {
                        bestPointMax = point;
                        best = current;
                    }
                }
                
                if (bestPointMax != 200)
                    return best;
            }
            
            if (battlefield.opponentBorderSwordsmans.size() > 0) {
                for (auto current: battlefield.opponentBorderSwordsmans) {
                    point = distance(myBuildings.HQ, current);
                    
                    if (point <= bestPointMax) {
                        bestPointMax = point;
                        best = current;
                    }
                }
                
                if (bestPointMax != 200)
                    return best;
            }
            
            if (battlefield.opponentBorderScouts.size() > 0) {
                for (auto current: battlefield.opponentBorderScouts) {
                    point = distance(myBuildings.HQ, current);
                    
                    if (point <= bestPointMax) {
                        bestPointMax = point;
                        best = current;
                    }
                }
                
                if (bestPointMax != 200)
                    return best;
            }
                
            if (battlefield.opponentBorderLine.size() > 0) {
                for (auto current: battlefield.opponentBorderLine) {
                    point = distance(myBuildings.HQ, current);
                    
                    if (point > bestPoint) {
                        bestPoint = point;
                        best = current;
                    }
                }
                
                if (bestPoint != 0)
                    return best;
            }
                        
            if (battlefield.borderLine.size() > 0) {
                for (auto current: battlefield.borderLine) {
                    point = distance(myBuildings.HQ, current);
                    
                    if (look(current, WEST, target))
                        point += 2;                
                    if (look(current, EAST, target))
                        point += 2;
                    if (look(current, NORTH, target))
                        point += 2;
                    if (look(current, SOUTH, target))
                        point += 2;
                        
                    if (point > bestPoint) {
                        bestPoint = point;
                        best = current;
                    }
                }
                
                if (bestPoint != 0)
                    return best;
            }
            
            for (auto current: battlefield.myActiveTerritory) {
                point = distance(myBuildings.HQ, current);
                        
                if (point > bestPoint) {
                    bestPoint = point;
                    best = current;
                }
            }
            
            if (bestPoint != 0)
                return best;
        }
    }
    
    void moveScouts() {
        for (auto scout: myArmy.scouts) {
            Position target;
            
            if ((turn < 9) || (turn > 8)) {
                target = scan(scout.position, 1);
            } else {
                target = opponentBuildings.HQ;
            }
            
            if ((target.x == scout.position.x) && (target.y == scout.position.y))
                continue; 
            
            battlefield.replace({scout.position}, 'O');
            battlefield.replace({target}, '1');
            battlefield.updateBorderLine();
            battlefield.updateMyActiveTerritory();
            output += MOVE + to_string(scout.id) + " " + to_string(target.x) + " " + to_string(target.y) + ";";
        }
    }
        
    void moveSwordsmans() {
        for (auto swordsman: myArmy.swordsmans) {
            Position target;
            
            if ((turn < 9) || (turn > 8)) {
                target = scan(swordsman.position, 2);
            } else {
                target = opponentBuildings.HQ;
            }
            
            if ((target.x == swordsman.position.x) && (target.y == swordsman.position.y))
                continue; 
            
            battlefield.replace({swordsman.position}, 'O');
            battlefield.replace({target}, '2');
            battlefield.updateBorderLine();
            battlefield.updateMyActiveTerritory();
            output += MOVE + to_string(swordsman.id) + " " + to_string(target.x) + " " + to_string(target.y) + ";";
        }
    }
    
    void moveKnights() {
        for (auto knight: myArmy.knights) {
            Position target;
            
            target = scan(knight.position, 3);
            
            if ((target.x == knight.position.x) && (target.y == knight.position.y))
                continue; 
            
            battlefield.replace({knight.position}, 'O');
            battlefield.replace({target}, '3');
            battlefield.updateBorderLine();
            battlefield.updateMyActiveTerritory();
            output += MOVE + to_string(knight.id) + " " + to_string(target.x) + " " + to_string(target.y) + ";";
        }
    }
    
    void trainScout() {       
        Position target;
        
        target = findTrainPosition(1);
        
        battlefield.replace({target}, '1');
        battlefield.updateBorderLine();
        battlefield.updateMyActiveTerritory();
        output += TRAIN_SCOUT + to_string(target.x) + " " + to_string(target.y) + ";";
        myIncome--;
        myGold -= 10;
    }
            
    void trainSwordsman() {            
        Position target;
        
        target = findTrainPosition(2);
            
        battlefield.replace({target}, '2');
        battlefield.updateBorderLine();
        battlefield.updateMyActiveTerritory();
        output += TRAIN_SWORDSMAN + to_string(target.x) + " " + to_string(target.y) + ";";
        myIncome -= -4;
        myGold -= 20;
    }
        
    void trainKnight() {           
        Position target;
        
        target = findTrainPosition(3);
        
        battlefield.replace({target}, '3');
        battlefield.updateBorderLine();
        battlefield.updateMyActiveTerritory();
        output += TRAIN_KNIGHT + to_string(target.x) + " " + to_string(target.y) + ";";
        myIncome -= -20;
        myGold -= 30;
    }
};

int main() {    
    Game game;
    game.initialization();

    // Game loop
    while (true) {
        game.update();
        game.debug();
        game.engine();
    }
}