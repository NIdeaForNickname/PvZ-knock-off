#define _XOPEN_SOURCE 700
#include <iostream>
#include <stdio.h>
#include <locale>
#include <wchar.h>
#include <unistd.h>
#include <ncursesw/curses.h>
using namespace std;
#define ENTER_CHAR "\r\n"
void showMessageBox(string message = " ") {
    string command = "zenity --info --text=\"" + message + "\"";
    system(command.c_str());
}
enum KeyCodes { ENTER = 13, ESCAPE = 27, LEFT = 97, RIGHT = 100, UP = 119, DOWN = 115, SPACEBAR = 32 }; // wasd only 
enum myColors{ DARKGREEN = 22, RED = 160, YELLOW = 226, BLUE = 33, BLACK = 0, GREY = 8, GREEN = 40, WHITE = 255, ORANGE = 202, LANDMINE_COLOR = 174};

// https://www.compart.com/en/unicode/block/U+2500

#define PEA_ICON "\u25cf"

#define LSICON "\u06d5"
#define LAICON "\u06fe"
#define PICON "\u0393"
#define ZICON "\u2639"
#define SICON "\u2605"

#define LINE_K "\u2560"
#define LINE_BK "\u2563" // B-Backwards
#define LINE_T "\u2566"
#define LINE_BT "\u2569" // B-Backwards
#define LINE__ "\u2550"
#define LINE_I "\u2551"
#define LINE_L "\u255a" 
#define LINE_r "\u2554"
#define LINE_Br "\u2557" // B-Backwards
#define LINE_J "\u255d"
#define LINE_X "\u2735"


#define LINE2_K "\u251c"
#define LINE2_BK "\u2524" // B-Backwards
#define LINE2_T "\u252c"
#define LINE2_BT "\u2534" // B-Backwards
#define LINE2__ "\u2500"
#define LINE2_I "\u2502"
#define LINE2_L "\u2570" 
#define LINE2_r "\u256d"
#define LINE2_Br "\u256e" // B-Backwards
#define LINE2_J "\u256f"
#define LINE2_X "\u253c"


#define SUNS {" ∧ ", "<O>", " v"}

wstring stringToWstring(const string& str) { // превратить стринг в встринг(для отрисовки в ncurses(ncurses UTF-8))
    size_t size = mbstowcs(nullptr, str.c_str(), 0);
    
    if (size == (size_t)-1) {
        return L"";  // в случае ошибки вернуть пустой встринг
    }

    wstring wstr(size, L'\0'); // создать встринг
    mbstowcs(&wstr[0], str.c_str(), size); // присвоить созданому встрингу згачение
    return wstr;
}

void showElement(const wstring& str, int color, int x, int y) { // тут точно не могу сказать, писал по гайдам как через UTF-8 печатать UTF-16 (ncurses поддерживает максимум UTF-8 как мне известно)
    move(y, x);
    attron(COLOR_PAIR(color));

    cchar_t cchar; // Declare cchar_t structure for wide characters
    for (wchar_t wc : str) {
        setcchar(&cchar, &wc, 0, color, nullptr);  // Convert wchar_t to cchar_t
        add_wch(&cchar);  // Use add_wch to print the wide character
    }

    attroff(COLOR_PAIR(color));
    refresh();
}

void showElement(const string str, int color, int x, int y) {// тоже самое, тут точно не могу сказать, писал по гайдам как через UTF-8 печатать UTF-16
    move(y, x);
    attron(COLOR_PAIR(color));
    wstring str1 = stringToWstring(str);
    cchar_t cchar;  // Declare cchar_t structure for wide characters
    for (wchar_t wc : str1) {
        setcchar(&cchar, &wc, 0, color, nullptr);  // Convert wchar_t to cchar_t
        add_wch(&cchar);  // Use add_wch to print the wide character
    }

    attroff(COLOR_PAIR(color));
    refresh();
}

template<size_t N> // автоматически находит размер не динам списка
void showMultiElement(const string (&str)[N], myColors color, int x, int y){ // отображение списка(не динам)
	for(int i = 0; i < N; i++){
		showElement(str[i], color, x, y+i);
	}
}

void makeBorder(bool inner, int size_x, int size_y, int x, int y, int color = WHITE){ // отрисовка границы указаного размера и позиции (inner отвечает очищать ли внутрености ганицы)
	string temp = "";
	if (inner){
		temp = " ";
	}
	for(int i = 0; i < size_y; i++){
        for (int j = 0; j < size_x; j++){
            showElement(temp, color, j+x, i+y);
            if (j == 0 || j == size_x-1){
                showElement(LINE_I, color, j+x, i+y);
            } else if (i == 0 || i == size_y-1){
                showElement(LINE__, color, j+x, i+y);
            }
        }
        showElement("", color, x, i+1);
    }

	showElement(LINE_r, color, x, y);
    showElement(LINE_L, color, x, size_y+y-1);
    showElement(LINE_Br, color, size_x+x-1, y);
    showElement(LINE_J, color, size_x+x-1, size_y+y-1);
}

void removeBorder(int size_x, int size_y, int x, int y){ // удаление границы указаного размера и позиции
	for(int i = 0; i < size_y; i++){
        for (int j = 0; j < size_x; j++){
            showElement("", WHITE, j+x, i+y);
            if (j == 0 || j == size_x-1){
                showElement(" ", WHITE, j+x, i+y);
            } else if (i == 0 || i == size_y-1){
                showElement(" ", WHITE, j+x, i+y);
            }
        }
    }
}

string multiplyStr(string str, int multiplication){ // функция как в пайтоне что бы было "_"*20
	string ans = "";
	for (int i = 0; i < multiplication; i++){
		ans += str;
	}
	return ans;
}

void drawLine(int way, int length, int x, int y){ // нарисовать линию в указаном направлении указаной длины
    switch (way)
    {
    case 2: // up-down
        showElement(LINE_T, WHITE, x, y);
        
        for(int i = y+1; i < y+length-1; i++){
            showElement(LINE_I, WHITE, x, i);
        }
        showElement(LINE_BT, WHITE, x, y + length-1);
        break;
    case 1: // right-left
        showElement(LINE_K, WHITE, x, y);
        
        for(int i = x+1; i < x+length-1; i++){
            showElement(LINE__, WHITE, i, y);
        }

        showElement(LINE_BK, WHITE, x + length-1, y);
        break;
    default:
        break;
    }
}


void updateZoneSection(int size_x, int size_y, int x, int y, int rel_x, int rel_y, string** arr, myColors color){ // обновить зону грида
    for (int i = 0; i < size_y; i++)
    {
        for (int j = 0; j < size_x; j++)
        {
            showElement(arr[i+rel_y][j+rel_x], color, x+j, y+i);
        }
    }
}


void updateArrZone(int size_x, int size_y, int x, int y, string** arr, myColors color){ // обновить грид
    for (int i = 0; i < size_y; i++)
    {
        for (int j = 0; j < size_x; j++)
        {
            showElement(arr[i][j], color, x+j, y+i);
        }
    }
}


void drawGrid(int size_x, int size_y, int x, int y, string** arr){ // гарисовать и записать вид грида
    for(int i = 0; i < size_y; i++){
        for (int j = 0; j < size_x; j++)
        {
            if(i % 2 == 0){
                if (j % 4 != 0){
                    arr[i][j] = LINE2__;
                } else {
                    if (i == 0){
                        if(j == 0){
                            arr[i][j] = LINE2_r;
                        } else if (j == size_x-1){
                            arr[i][j] = LINE2_Br;
                        } else {
                            arr[i][j] = LINE2_T;
                        }
                    } else if (i == size_y-1){
                        if(j == 0){
                            arr[i][j] = LINE2_L;
                        } else if (j == size_x-1){
                            arr[i][j] = LINE2_J;
                        } else {
                            arr[i][j] = LINE2_BT;
                        }
                    } else{
                        if(j == 0){
                            arr[i][j] = LINE2_K;
                        } else if (j == size_x-1){
                            arr[i][j] = LINE2_BK;
                        } else {
                            arr[i][j] = LINE2_X;
                        }
                    }
                }
            }
            else {
                if (j % 4 == 0){
                    arr[i][j] = LINE2_I;
                } else {
                    arr[i][j] = " ";
                }
            }
        }
    }
}

template<size_t N>
void drawMenu(int size_x, int size_y, int x, int y, const string (&str)[N], const int (&colors)[N] = {YELLOW, GREEN, LANDMINE_COLOR}){ // нариовать меню 
    for(int i = 0; i < N; i++){
        showElement(str[i], colors[i], x + 2, y+1+(3*i));
    }
    makeBorder(false, size_x, size_y, x, y, colors[0]);
}

template<size_t N>
void updateMenu(int size_x, int size_y, int x, int y, const string (&str)[N], int key, int &state, int maxstate = 2, const int (&colors)[N] = {YELLOW, GREEN, LANDMINE_COLOR}){ // посчитать выбор из меню
    removeBorder(size_x, size_y, x, y+(state*size_y));
    switch (key)
    {
    case UP:
        if (state > 0){state--;}
        break;
    case DOWN:
        if (state < N-1){state++;}
    default:
        break;
    }
    makeBorder(false, size_x, size_y, x, y+(state*size_y), colors[state]);
}
