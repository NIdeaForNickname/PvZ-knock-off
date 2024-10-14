#include <cstdlib> // рандом
#include <ctime> // семя для рандома
#include <vector>
#include "DrawBoard.h"
#include "myDynam.h"

void ncursesInitialize(){
    initscr(); // запуск ncurses 
    nodelay(stdscr, FALSE); // не ждать символ в getch
    cbreak(); // плавнее по ощущениям делает (во всех гайдах пишут что нужно)
    noecho(); // отключить видимость введёных симолов
    move(0, 0); // курсор на 0, 0
    start_color(); // 256 - цветов
    curs_set(0); // невидимый курсор
    setlocale(LC_ALL, ""); // UTF-8
    // цветовые пары (к примеру красный текст, чёрный фон, только тут повсюду чёрный цвет)
    for (int i = 0; i < 256; ++i) {
        init_pair(i, i, COLOR_BLACK);
    }
}


void menuSection(const int &SCREEN_HEIGHT, const int &SCREEN_WIDTH);
void myExit();
void game(const int &SCREEN_HEIGHT, const int &SCREEN_WIDTH);

void updateSuns(int money);
bool chekPresence(Entity *vect, int &size, int x, int y, entityType typr);
bool checkExistence(Entity *vect, int &size, entityType typr);
int findThisEntity(Entity *vect, int &size, int x, int y, entityType typr);
void zombiePreset(Entity &ent, zombies subt = NORMAL, int y = 0, int x = 40);
void plantPreset(Entity &ent, plants subt = SUNFLOWER, int y = 0, int x = 2);
void projPreset(Entity &ent, projectiles subt = PEA, int y = 0, int x = 0);
void killEmAll(Entity* &vect, int &size, int x, int y, entityType typr);
void updatePlant(Entity plant, int x, int y, bool active);
void updateZombie(Entity plant, int x, int y);
void endOfGame(bool won);
int readWaves(int** &waveList);
int randomEnemy(int* arr, int size);
void drawAllEnteties(Entity* ent1, int size, int RELATIVE_X, int RELATIVE_Y);

using namespace std;
int main()
{
    ncursesInitialize();
    while(true){
        // размеры экрана
        const int SCREEN_WIDTH = 64;
        const int SCREEN_HEIGHT = 20;

        // отсюда и -
        makeBorder(true, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0);

        const string logo[] = {
            "██████╗ ██╗   ██╗███████╗",
            "██╔══██╗██║   ██║╚══███╔╝",
            "██████╔╝██║   ██║  ███╔╝ ",
            "██╔═══╝ ╚██╗ ██╔╝ ███╔╝  ",
            "██║      ╚████╔╝ ███████╗",
            "╚═╝       ╚═══╝  ╚══════╝"};

        showMultiElement(logo, WHITE, (SCREEN_WIDTH - 25) / 2, 2);
        showElement(multiplyStr(" ", SCREEN_WIDTH), BLACK, 0, SCREEN_HEIGHT);

        showElement("Play", WHITE, (SCREEN_WIDTH - 4) / 2, 11);
        makeBorder(false, 8, 3, 28, 10);

        showElement("Exit", WHITE, (SCREEN_WIDTH - 4) / 2, 14);

        showMultiElement({"Controls:", "W/A/S/D - Move", "SPACE - Enter", "ESC - Back"}, GREY, 2, SCREEN_HEIGHT - 5);

        showElement("", WHITE, 0, SCREEN_HEIGHT);
        // - до сюда отрисовка меню

        menuSection(SCREEN_HEIGHT, SCREEN_WIDTH);
    }
}

void menuSection(const int &SCREEN_HEIGHT, const int &SCREEN_WIDTH)
{
    // нажатая клавиша и текущий выбор
    int code, state = 0;
    while (true)
    { 
        code = getchar();
        switch (code) // обновление состояния
        {
        case UP:
            state = 0;
            removeBorder(8, 3, (SCREEN_WIDTH - 8) / 2, 13);
            makeBorder(false, 8, 3, (SCREEN_WIDTH - 8) / 2, 10);
            break;
        case DOWN:
            state = 1;
            removeBorder(8, 3, (SCREEN_WIDTH - 8) / 2, 10);
            makeBorder(false, 8, 3, (SCREEN_WIDTH - 8) / 2, 13);
            break;
        case SPACEBAR:
            switch (state)
            {
            case 1:
                myExit();
                break;
            case 0:
                game(SCREEN_HEIGHT, SCREEN_WIDTH);
                return;
                break;
            default:
                break;
            }
        case ESCAPE:
            myExit();
        }
    }
}

void game(const int &SCREEN_HEIGHT, const int &SCREEN_WIDTH)
{
    srand(time(0));

    int money = 100;
    makeBorder(true, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0);
    drawLine(2, SCREEN_HEIGHT, 18, 0);


    drawLine(1, 19, 0, 4);
    showMultiElement(SUNS, YELLOW, 2, 1);
    showElement(to_string(money), YELLOW, 6, 2);


    string plantsL[] = {"Sunflower", "Peashooter", "Landmine"};
    drawMenu(15, 3, 2, 5, plantsL);


    const int RELATIVE_X = 20;
    const int RELATIVE_Y = 4;

    const int PLAYFIELD_WIDTH = 41;
    const int PLAYFIELD_HEIGHT = 11;
    const int PLAYFIELD_HEIGHT2 = PLAYFIELD_HEIGHT / 2;


    // список Существ
    int boardSize = 0;
    Entity *board = new Entity[boardSize];

    // дисплей поля
    string **boardDisplay = new string *[PLAYFIELD_HEIGHT];
    for (int i = 0; i < PLAYFIELD_WIDTH; i++)
    {
        boardDisplay[i] = new string[PLAYFIELD_WIDTH];
    }

    // игровое поле
    drawGrid(PLAYFIELD_WIDTH, PLAYFIELD_HEIGHT, RELATIVE_X, RELATIVE_Y, boardDisplay);
    updateArrZone(PLAYFIELD_WIDTH, PLAYFIELD_HEIGHT, RELATIVE_X, RELATIVE_Y, boardDisplay, WHITE);
    makeBorder(true, 15, 3, 45, 16, DARKGREEN);


    nodelay(stdscr, TRUE); // getch не останавливает програму
    Entity tempEnt; // временая сущность, что бы потом добавлять в список обновления


    // структура волн
    int **waveStructure = nullptr;
    int wSSize = readWaves(waveStructure);


    // переменные используемые в игре
    int state = 0, substate[] = {0, 0}, alive = 0, timer = 50, counter = -1, wave = 0, temp;
    bool allDead = false, canwalk, loop = true, won = false;
    int ch;
    while (loop)
    {
        // проверка на нажатие
        ch = getch();
        if (ch != ERR)
        {
            // отрисовка меню, и обработка нажатия 
            updateMenu(15, 3, 2, 5, plantsL, ch, state);
            if (ch == SPACEBAR)
            {
                // переход в режим выбора клетки
                nodelay(stdscr, FALSE); // ждать нажатие
                do
                {
                    // отрисовка выбраной клетки, и выбор клетки
                    if (ch == SPACEBAR)
                    {
                        updateZoneSection(5, 3, RELATIVE_X + (substate[0] * 4), RELATIVE_Y + (substate[1] * 2), (substate[0] * 4), (substate[1] * 2), boardDisplay, DARKGREEN);
                        drawAllEnteties(board, boardSize, RELATIVE_X, RELATIVE_Y);
                    }
                    ch = getch();
                    updateZoneSection(5, 3, RELATIVE_X + (substate[0] * 4), RELATIVE_Y + (substate[1] * 2), (substate[0] * 4), (substate[1] * 2), boardDisplay, WHITE);
                    switch (ch)
                    {
                    case UP:
                        if (substate[1] > 0)
                        {
                            substate[1]--;
                        }
                        break;
                    case DOWN:
                        if (substate[1] < 4)
                        {
                            substate[1]++;
                        }
                        break;
                    case LEFT:
                        if (substate[0] > 0)
                        {
                            substate[0]--;
                        }
                        break;
                    case RIGHT:
                        if (substate[0] < 9)
                        {
                            substate[0]++;
                        }
                        break;
                    case SPACEBAR:
                        // проверка на присутствие растения на выбраной клеткке
                        if(!chekPresence(board, boardSize, substate[0]*4+2, substate[1], PLANT)){
                            // проверка хватает ли солнышкек и установка растения если хватает
                            // также отнятие солнышек если растение было посажено 
                            switch (state)
                            {
                            case SUNFLOWER:
                                if (money >= 50){
                                    plantPreset(tempEnt, plants(state), substate[1], (substate[0]*4)+2);
                                    myAppend(board, boardSize, tempEnt);
                                    money -= 50;
                                }
                                break;
                            case PEASHOOTER:
                                if (money >= 100){
                                    plantPreset(tempEnt, plants(state), substate[1], (substate[0]*4)+2);
                                    myAppend(board, boardSize, tempEnt);
                                    money -= 100;
                                }
                                break;
                            case LANDMINE:
                                if (money >= 25){
                                    plantPreset(tempEnt, plants(state), substate[1], (substate[0]*4)+2); 
                                    myAppend(board, boardSize, tempEnt);
                                    money -= 25;
                                }   
                                break;
                            default:
                                break;
                            }
                            updateSuns(money);

                        } else {
                            // если нажато на клетку с растением вернуть к выбору клетки (при ch == SPACEBAR вы покидаете цикл выбора клетки и посадки)
                            ch = ERR;
                        }
                    default:
                        break;
                    }
                    updateZoneSection(5, 3, RELATIVE_X + (substate[0] * 4), RELATIVE_Y + (substate[1] * 2), (substate[0] * 4), (substate[1] * 2), boardDisplay, DARKGREEN);
                    drawAllEnteties(board, boardSize, RELATIVE_X, RELATIVE_Y);
                } while (ch != SPACEBAR && ch != ESCAPE);
                // очистить выбор
                updateZoneSection(5, 3, RELATIVE_X + (substate[0] * 4), RELATIVE_Y + (substate[1] * 2), (substate[0] * 4), (substate[1] * 2), boardDisplay, WHITE);
                nodelay(stdscr, TRUE); // перестать ждать напжатия
            }
        }

        for (int i = 0; i < boardSize; i++) // цикл обновления состояния всего живого (и не совсем)
        {
            Entity &ent = board[i]; // текущая сущность
            switch (ent.type){ // проверка сущности по типу (PLANT = растение, ZOMBIE = зомби, PROJECTILE = горошина)
            case ZOMBIE:
                if(ent.health <= 0){ // проверка живой ли он, и если нет то удаление его из списка живих, а так же выход из данного case'а
                    myPop(board, boardSize, i);
                    updateZoneSection(1, 1, RELATIVE_X + ent.x, RELATIVE_Y + (ent.y * 2) + 1, ent.x, (ent.y * 2) + 1, boardDisplay, WHITE);
                    i--;
                    break;
                }

                // обновить сколько осталось до следующего хода
                ent.turnMeter += ent.speed;

                // проверка есть ли растение впереди
                canwalk = !chekPresence(board, boardSize, ent.x - 1 , ent.y, PLANT);
                
                // если есть растение впереди нанести урон
                if(!canwalk){
                    board[findThisEntity(board, boardSize, ent.x-1, ent.y, PLANT)].health -= ent.damage;
                } else if (ent.turnMeter >= 6){ //в ином случае если щётчик движения достаточно высок сдвинуться на клетку 
                    updateZoneSection(1, 1, RELATIVE_X + ent.x, RELATIVE_Y + (ent.y * 2) + 1, ent.x, (ent.y * 2) + 1, boardDisplay, WHITE);

                    ent.turnMeter %= 6; // обновление Щётчика
                    ent.x -= 1;

                    updateZombie(ent, RELATIVE_X + ent.x, RELATIVE_Y + (ent.y * 2 + 1));
                    // если зоби прошёл до конца, game over и вернуться в menuSection(), а от туда через return в мейн для отрисовки меню
                    if(ent.x <= 0){
                        sleep(1);
                        loop = false;
                    }
                }
                break;
            case PLANT:  // проверка живой ли он, и если нет то удаление его из списка живих, а так же выход из данного case'а
                if(ent.health <= 0){
                    myPop(board, boardSize, i);
                    updateZoneSection(1, 1, RELATIVE_X + ent.x - 1, RELATIVE_Y + (ent.y * 2) + 1, ent.x - 1, (ent.y * 2) + 1, boardDisplay, WHITE);
                    i--;
                    break;
                }

                // обновить сколько осталось до следующего хода
                ent.turnMeter += ent.speed;
                if (ent.subType == LANDMINE && ent.abilityUsed == true && ent.health < 100){ // проверка мины
                    killEmAll(board, boardSize, ent.x+1, ent.y, ZOMBIE);
                    updateZoneSection(3, 1, RELATIVE_X + ent.x - 1, RELATIVE_Y + (ent.y * 2) + 1, ent.x - 1, (ent.y * 2) + 1, boardDisplay, WHITE);
                    myPop(board, boardSize, i);
                    i--;
                    break;
                } else if (ent.turnMeter >= 10){
                    switch (ent.subType){
                    case PEASHOOTER: // в случае горохострела создать проджектайл
                        ent.turnMeter -= 6; // обновление щётчика
                        projPreset(tempEnt, PEA, ent.y, ent.x+1);
                        myAppend(board, boardSize, tempEnt);
                        break;
                    case SUNFLOWER: // добавление солнышек
                        ent.turnMeter = -65;
                        money += 25;
                        updateSuns(money);
                        break;
                    case LANDMINE: // взвод мины
                        ent.health = 100;
                        ent.abilityUsed = true;
                        break;
                    default:
                        break;
                    } // отрисовка растения в активном виде (подсолнух дал солнце, горох стрельнул, мина взвелась)
                    updatePlant(ent, RELATIVE_X+ent.x, RELATIVE_Y + (ent.y*2)+1, true);
                } else{
                    updatePlant(ent, RELATIVE_X+ent.x, RELATIVE_Y + (ent.y*2)+1, ent.abilityUsed); // отрисовка нейтрального растения(ничего не сделало за ход)
                }
                break;
            case PROJECTILE:
                // проверка присутствия зомби на той же клетке что и горошина
                canwalk = !chekPresence(board, boardSize, ent.x, ent.y, ZOMBIE);
                if (!canwalk) {// если есть зомби нанести урон, и самоликвидироваться
                    board[findThisEntity(board, boardSize, ent.x, ent.y, ZOMBIE)].health -= ent.damage;
                    myPop(board, boardSize, i);
                    i--;
                    break;
                }
                else {
                    canwalk = chekPresence(board, boardSize, ent.x + 1, ent.y, ZOMBIE); // проверка зомби спереди
                    if (canwalk){// если есть зомби нанести урон, и самоликвидироваться
                        board[findThisEntity(board, boardSize, ent.x + 1, ent.y, ZOMBIE)].health -= ent.damage;
                        myPop(board, boardSize, i);
                        i--;
                        updateZoneSection(1, 1, RELATIVE_X + ent.x, RELATIVE_Y + (ent.y * 2) + 1, ent.x, (ent.y * 2) + 1, boardDisplay, WHITE);
                    } else if (ent.x < PLAYFIELD_WIDTH-1){ // если нет проверить может ли продвинуться вперёд(что бы не было "out of bound" или иными словами "Segmentation fault")
                        updateZoneSection(1, 1, RELATIVE_X + ent.x, RELATIVE_Y + (ent.y * 2) + 1, ent.x, (ent.y * 2) + 1, boardDisplay, WHITE);
                        ent.x += 1;
                        showElement(PEA_ICON, GREEN, RELATIVE_X + ent.x, RELATIVE_Y + (ent.y * 2 + 1));
                    } else{ // двигаться вперёд
                        updateZoneSection(1, 1, RELATIVE_X + ent.x, RELATIVE_Y + (ent.y * 2) + 1, ent.x, (ent.y * 2) + 1, boardDisplay, WHITE);
                        myPop(board, boardSize, i);
                        i--;
                        break;
                    }
                }
                break;
            default:
                break;
            }
        }

        timer--; //таймер на минимальную дистанцию между 1 волной + время между спавном зомби
        allDead = checkExistence(board, boardSize, ZOMBIE); // проверка на присутствие зомби на поле
        if(timer <= 0 && !allDead){ // если все зомби мертвы и минимальное время между волн прошло послать следущкю волну
             counter++; // переход на след. волну

            // обновить прогресс по волнам
            makeBorder(true, 15, 3, 45, 16, DARKGREEN);
            showElement(LINE2_I, YELLOW, 58-(state*2), 17);
            
            // если волны закончились "ю вин" и на тайтл скрин
            if (wSSize <= counter){
                won = true;
                loop = false;
                break; // выход из while -> menuSection(return to main)-> main(draw menu) -> menuSection(Exit or play)
            }
        }

        if(counter >= 0 && timer <= 0){ // если минимальное время между спавном прошло заспанить нового зомби(так же проверка что бы не пробывало достучатся в список по -1 элементу)
            temp = randomEnemy(waveStructure[counter], 3); // проверка есть ли доступный враг, если нет то -1, если да выбрать случайного
            if (temp >= 0){
                // добавить случ. врага
                zombiePreset(tempEnt, zombies(temp), rand() % 5);
                myAppend(board, boardSize, tempEnt);

                // установить таймер перед следующей проверкой на доступность врага
                timer = 6;
            } else { // если закончились враги запустить таймер перед волной
                timer = 40;
            }
        }

        // задержка между "ходами"
        usleep(50000); // microseconds (1 S/1,000,000 mcS)
    }

    // отрисовка победы/проиграша
    makeBorder(true, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0);
    endOfGame(won);

    // очистка динам памяти от grid'а
    for (int i = 0; i < PLAYFIELD_HEIGHT; i++)  // Deallocating strings
    {
        delete[] boardDisplay[i];
    }
    delete[] boardDisplay;  // Deallocating array of pointers
    delete[] board; // очитка динам памяти от сущностей
    return;
}

void endOfGame(bool won){// отрисовка проиграша/победы соответствено
    string wonScreen[6]= {"██╗   ██╗ ██████╗ ██╗   ██╗     ██╗    ██╗██╗███╗   ██╗██╗",
                          "╚██╗ ██╔╝██╔═══██╗██║   ██║     ██║    ██║██║████╗  ██║██║",
                          " ╚████╔╝ ██║   ██║██║   ██║     ██║ █╗ ██║██║██╔██╗ ██║██║",
                          "  ╚██╔╝  ██║   ██║██║   ██║     ██║███╗██║██║██║╚██╗██║╚═╝",
                          "   ██║   ╚██████╔╝╚██████╔╝     ╚███╔███╔╝██║██║ ╚████║██╗",
                          "   ╚═╝    ╚═════╝  ╚═════╝       ╚══╝╚══╝ ╚═╝╚═╝  ╚═══╝╚═╝"
    };
    string lostScreen[13]= {"  ██╗   ██╗ ██████╗ ██╗   ██╗", 
                          "  ╚██╗ ██╔╝██╔═══██╗██║   ██║", 
                          "   ╚████╔╝ ██║   ██║██║   ██║",    
                          "    ╚██╔╝  ██║   ██║██║   ██║",    
                          "     ██║   ╚██████╔╝╚██████╔╝",    
                          "     ╚═╝    ╚═════╝  ╚═════╝ ", 
                          " ",
                          "██╗      ██████╗ ███████╗████████╗",
                          "██║     ██╔═══██╗██╔════╝╚══██╔══╝",
                          "██║     ██║   ██║███████╗   ██║",
                          "██║     ██║   ██║╚════██║   ██║",
                          "███████╗╚██████╔╝███████║   ██║",
                          "╚══════╝ ╚═════╝ ╚══════╝   ╚═╝"
    };
    if(won){
        showMultiElement(wonScreen, WHITE, 3, 5);
    } else {
        showMultiElement(lostScreen, WHITE, 15, 3);
    }
    sleep(3);
    return;
}


void myExit() // для линукса нужно
{
    showElement("", WHITE, 0, 20);
#ifdef __linux__
    system("/bin/stty cooked");
#endif
    exit(0);
}

void zombiePreset(Entity &ent, zombies subt, int y, int x) // установить значения зомби в переменную указаную в game()
{
    ent.type = ZOMBIE;
    ent.abilityUsed = false;
    ent.subType = subt;
    ent.damage = 1;
    ent.health = 10 + (subt * 15);
    if (subt != SPRINTER)
    {
        ent.speed = 1;
    }
    else
    {
        ent.speed = 2;
    }
    ent.damage = 1;
    ent.turnMeter = 0;
    ent.x = x;
    ent.y = y;
}

void plantPreset(Entity &ent, plants subt, int y, int x) // установить значения соответствующего растения в переменную указаную в game()
{
    ent.type = PLANT;
    ent.subType = subt;
    ent.abilityUsed = false;
    ent.speed = 1;
    switch (subt)
    {
    case SUNFLOWER:
        ent.turnMeter = -40;
        ent.damage = 0;
        ent.health = 10;
        break;
    case PEASHOOTER:
        ent.turnMeter = 0;
        ent.damage = 0;
        ent.health = 15;
        break;
    case LANDMINE:
        ent.turnMeter = -75;
        ent.damage = 50;
        ent.health = 10;
        break;
    default:
        break;
    }
    ent.x = x;
    ent.y = y;
}

void projPreset(Entity &ent, projectiles subt, int y, int x)  // установить значения горошини в переменную указаную в game()
{
    ent.type = PROJECTILE;
    ent.subType = PEA;
    ent.damage = 1;
    ent.speed = 1;
    ent.x = x;
    ent.y = y;
    ent.turnMeter = 0;
}

bool chekPresence(Entity *vect, int &size, int x, int y, entityType typr) // проверка на присутствие указаного типа в указаном месте
{
    Entity ent;
    for (int i = 0; i < size; i++)
    {
        ent = vect[i];
        if (ent.x == x && ent.y == y && ent.type == typr)
        {
            return true;
        }
    }
    return false;
}

bool checkExistence(Entity *vect, int &size, entityType typr){ // проверка на присутствие указаного типа в цеелом
    Entity ent;
    for (int i = 0; i < size; i++)
    {
        ent = vect[i];
        if (ent.type == typr)
        {
            return true;
        }
    }
    return false;
}

int findThisEntity(Entity *vect, int &size, int x, int y, entityType typr) // найти индекс существа с указаным типом в указаном месте
{
    int temp = size;
    for (int i = 0; i < temp; i++)
    {
        if (vect[i].x == x && vect[i].y == y && vect[i].type == typr)
        {
            return i;
        }
    }
    return -1;
}

void killEmAll(Entity* &vect, int &size, int x, int y, entityType typr) // мина бабах всех в указаной клетке
{
    Entity ent;
    for (int i = 0; i < size; i++)
    {
        ent = vect[i];
        if (ent.x == x && ent.y == y && ent.type == typr)
        {
            myPop(vect, size, i);
            i--;
            size--;
        }
    }
}

void updatePlant(Entity plant, int x, int y, bool active){ // отрисовать указаное растение в соответсвующем месте
    switch (plant.subType)
    {
    case SUNFLOWER:
        if(!active){
            showElement(SICON, ORANGE, x, y);
        } else {
            showElement(SICON, YELLOW, x, y);
        }
        break;
    case PEASHOOTER:
        if(!active){
            showElement(PICON, GREEN, x, y);
        } else{
            showElement(PICON, WHITE, x, y);
        }
        break;
    case LANDMINE:
        if(!active){
            showElement(LSICON, LANDMINE_COLOR, x, y);
        } else {
            showElement(LAICON, LANDMINE_COLOR, x, y);
        }
        break;

    default:
        break;
    }
}

void updateZombie(Entity plant, int x, int y){ // отрисовать указаное зомби в соответсвующем месте
    switch (plant.subType)
    {
    case NORMAL:
        showElement(ZICON, DARKGREEN, x, y);
        break;
    case CONE:
        showElement(ZICON, ORANGE, x, y);
        break;
    case BUCKET:
        showElement(ZICON, GREY, x, y);
        break;
    default:
        break;
    }
}


int readWaves(int** &waveList){ // считать структуру волн из файла, записать их в указаный аргумент, а также вернуть количество волн
    FILE* f = fopen("wave.txt", "r");  
    if (f == nullptr) {
        cout << "Something's wrong with file";
        myExit();
        return -1;
    }

    vector<vector<int>> tempWaves;  
    char line[256]; 

    while (fgets(line, sizeof(line), f)) {
        vector<int> waveRow;
        int num1, num2, num3;

        if (sscanf(line, "%d,%d,%d", &num1, &num2, &num3) == 3) {
            waveRow.push_back(num1);
            waveRow.push_back(num2);
            waveRow.push_back(num3);
            tempWaves.push_back(waveRow);  
        }
    }

    fclose(f);  

    int ySize = tempWaves.size();  
    if (ySize == 0) {
        cerr << "Error: File is empty or incorrectly formatted!" << endl;
        return 0;
    }

    waveList = new int*[ySize];
    for (int i = 0; i < ySize; ++i) {
        waveList[i] = new int[3];  
    }

    for (int i = 0; i < ySize; ++i) {
        for (int j = 0; j < 3; ++j) {
            waveList[i][j] = tempWaves[i][j];
        }
    }

    return ySize;  
}

void updateSuns(int money){ // отрисовать солнышки
    showElement("    ", YELLOW, 6, 2);
    showElement(to_string(money), YELLOW, 6, 2);
}

int randomEnemy(int* arr, int size){ // выбрать случайного врага
    vector<int> availableEnemies;

    for (int i = 0; i < size; i++){ // индекс в availableEnemies как допустимого врага
        if (arr[i] > 0){
            availableEnemies.push_back(i);
        }
    }

    if (availableEnemies.size() == 0){ // если допустимых врагов нет вернуть -1
        return -1;
    }
    
    int enemy = availableEnemies[rand() % availableEnemies.size()]; // случайно выбрать врага
    arr[enemy]--; // уменьшить количество оставшиъся врагов по индексу i

    return enemy; //вернуть индекс врага
}

void drawAllEnteties(Entity* ent1, int size, int RELATIVE_X, int RELATIVE_Y){
    for (int i = 0; i < size; i++)
    {
        Entity ent = ent1[i];
        switch (ent.type){
        case PROJECTILE:
            showElement(PEA_ICON, GREEN, RELATIVE_X + ent.x, RELATIVE_Y + (ent.y * 2 + 1));
            break;
        case PLANT:
            updatePlant(ent, RELATIVE_X+ent.x, RELATIVE_Y + (ent.y*2)+1, ent.abilityUsed);
            break;
        case ZOMBIE:
            updateZombie(ent, RELATIVE_X + ent.x, RELATIVE_Y + (ent.y * 2 + 1));
            break;
        default:
            break;
        }
    }
    
}