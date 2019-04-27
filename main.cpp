#pragma region INCLUDES
#include <iostream>
#include <fstream>
#include <io.h>
#include <windows.h>
#include <unistd.h>
#include <conio.h>
#include <chrono>
#include <ctime>
#pragma endregion

using namespace std;

#pragma region GLOBAL VARIABLES
#define KEY_UP 72
#define KEY_DOWN 80
#define KEY_LEFT 75
#define KEY_RIGHT 77
#define MAX_SCORE 302
#define MAX_TIME 60

struct screen {
    string path;
    ifstream fileR;
    ofstream fileW;
    COORD crd;
} screen_home, screen_game, screen_info[2], screen_exit, screen_postGame;

ifstream ogGameMap;

auto start = chrono::system_clock::now();
unsigned short int score;
unsigned short int playerPos;
unsigned short int prevPlayerPos;
unsigned short int* LAST_KEY_PRESSED;

unsigned short int final_score = 0, final_time = 0;

bool gameIsRunning = false;
#pragma endregion

#pragma region FUNCTIONS
ifstream openR(string);
ofstream openW(string);
void setCursor(COORD);
short userInput();
void draw(string, COORD);
void setup();
void gameLoop();
void updateMap();
void updatePlayer();
string updateScore(string);
string updateTime(string);
void updatePostGame();
void update();
void checkForEndGame();
bool isSolid(int, int);
void moveUP();
void moveDOWN();
void moveLEFT();
void moveRIGHT();
void homeScreen();
void infoScreen_1();
void infoScreen_2();
void postGameScreen();
void exitScreen();
#pragma endregion

int main() {
    setup();
    homeScreen();
    gameLoop();

    return 0;
}

#pragma region SETUP
// Funkcija inicijalizacije
void setup() {
    // Dodeljivanje vrednosti globalnim promenljivama
    score = 1;
    playerPos = 1409;
    prevPlayerPos = 0;
    LAST_KEY_PRESSED = new unsigned short int[2];

    // Inicijalizacija strukture pocetnog ekrana
    screen_home.path = "home.txt";
    screen_home.crd = {0, 2};

    // Inicijalizacija ekrana sa uputstvima i informacijama
    screen_info[0].path = "info-1.txt";
    screen_info[0].crd = {0, 2};
    screen_info[1].path = "info-2.txt";
    screen_info[1].crd = {0, 2};

    // Inicijalizacija ekrana koji se pojavljuje nakon igre
    screen_postGame.path = "post-game.txt";
    screen_postGame.crd = {0, 2};

    // Inicijalizacija ekrana izlaza iz igre
    screen_exit.path = "exit.txt";
    screen_exit.crd = {0,2};

    // Otvaranje originalnog fajla mape
    ogGameMap = openR("level.txt");

    // Inicijalizacija strukture igre
    screen_game.path = "game.txt";
    screen_game.fileW = openW(screen_game.path);
    screen_game.crd = {0, 2};

    // Upisivanje sadrzaja originalne mape u mapu igre
    string line((istreambuf_iterator<char>(ogGameMap)), istreambuf_iterator<char>());
    screen_game.fileW << line;

    // Zatvaranje otvorenih fajlova
    ogGameMap.close();
    screen_game.fileW.close();
}
#pragma endregion

#pragma region UPDATE
// Funkcija za azuriranje mape
void updateMap() {
    // Otvaranje fajla ekrana mape u modu za citanje,
    // upisivanje njegov sadrzaja u string 'line' i
    // zatvaranje fajla ekrana mape
    screen_game.fileR = openR(screen_game.path);
    string line((istreambuf_iterator<char>(screen_game.fileR)), istreambuf_iterator<char>());
    screen_game.fileR.close();

    // Portali na levoj i desnoj strani mape
    if(playerPos == 841) playerPos = 896;
    else if(playerPos == 897) playerPos = 842;

    // Azuriranje skora i vremena, pozicije igraca i prethodnog
    // polja u kome se igrac nalazio u string 'line'
    if(line[playerPos] == '&') score++;
    if(line[prevPlayerPos] != ' ') line[prevPlayerPos] = ' ';
    line[playerPos] = '*';
    line = updateScore(line);
    line = updateTime(line);

    // Otvaranje fajla igre u modu za pisanje, upisivanje
    // sadrzaja stringa 'line' u njega i zatvaranje
    screen_game.fileW = openW(screen_game.path);
    screen_game.fileW << line;
    screen_game.fileW.close();
}

// Funkcija za azuriranje igraca
void updatePlayer() {
    // Funkcija prvo proverava da li je pritisnuto neko dugme na tastaturi
    // Ukoliko jeste, proverava koje je u pitanju i menja pravac igraca
    // Ukoliko nije, nastavlja poslednjim aktuelnim pravcem
    if(_kbhit() && (LAST_KEY_PRESSED[1]=getch()) != LAST_KEY_PRESSED[0]) {
        switch(LAST_KEY_PRESSED[1]) {
            case KEY_UP:
                moveUP();
                break;

            case KEY_DOWN:
                moveDOWN();
                break;

            case KEY_LEFT:
                moveLEFT();
                break;

            case KEY_RIGHT:
                moveRIGHT();
                break;
        }
    } else {
        switch(LAST_KEY_PRESSED[1]) {
            case KEY_UP:
                moveUP();
                break;

            case KEY_DOWN:
                moveDOWN();
                break;

            case KEY_LEFT:
                moveLEFT();
                break;

            case KEY_RIGHT:
                moveRIGHT();
                break;
        }
    }
}

// Funkcija za azuriranje skora
string updateScore(string line) {
    // Funkcija prima string 'line' kao argument od funkcije 'updateMap'
    // Menja sadrzaj stringa tako da upise rec 'SCORE:  ' i trenutni skor
    // Vraca izmenjeni string 'line'
    string scoreText = "SKOR:   " + to_string(score);

    for(int i = 0; i < scoreText.length(); i++) {
        line[804 + i] = scoreText[i];
    }

    final_score = score;

    return line;
}

// Funkcija za azuriranje vremena
string updateTime(string line) {
    // Funkcija uzima trenutno vreme i racuna vreme koje ce se
    // prikazati u igri. Azurira string 'line' novim vremenom
    // i vraca isti
    auto current = chrono::system_clock::now();
    chrono::duration<double> elapsed = current - start;
    unsigned short int elapsedTime = (int)(elapsed.count());
    unsigned short int displayTime = MAX_TIME - elapsedTime;

    string timeText = "VREME:  " + to_string(displayTime);
    int length = timeText.length();

    if(displayTime < 100) {
        line[864 + length] = ' ';
    } else if(displayTime < 10) {
        line[864 + length - 1] = ' ';
    }

    for (int i = 0; i < length; i++) {
        line[864 + i] = timeText[i];
    }

    final_time = MAX_TIME - elapsedTime;

    return line;
}

// Funkcija za azuriranje ekrana nakon igre
void updatePostGame() {
    // Otvaranje fajla nakon igre u modu za citanje,
    // upisivanje njegovog sadrzaja u string 'line' i zatvaranje
    screen_postGame.fileR = openR(screen_postGame.path);
    string line((istreambuf_iterator<char>(screen_postGame.fileR)), istreambuf_iterator<char>());
    screen_postGame.fileR.close();
    
    // Konvertovanje ostvarenog broja poena i vremena u string
    string scoreStr = to_string(final_score);
    string timeStr = to_string(final_time);

    // Upisivanje ostvarenog broja poena i vremena u string 'line'
    for(int i = 0; i < scoreStr.length(); i++) {
        line[512 + i] = scoreStr[i];
    }
    for(int i = 0; i < timeStr.length(); i++) {
        line[572 + i] = timeStr[i];
    }

    screen_postGame.fileW = openW(screen_postGame.path);
    screen_postGame.fileW << line;
    screen_postGame.fileW.close();
}

// Glavna funkcija za azuriranje
void update() {
    // Glavna funkcija za azuriranje koja
    // poziva ostale funkcije azuriranja 
    updateMap();
    updatePlayer();
}
#pragma endregion

#pragma region DRAW
// Funkcija za iscrtavanje mape i fajlova ekrana
void draw(string path, COORD crd) {
    // Funkcija prima kao argumente naziv fajla i koordinatu od
    // koje treba poceti sa iscrtavanjem mape. Otvara proslednjeni
    // fajl, postavlja kursor u komandnom prozoru na proslednjene
    // koordinate i postavlja mod ispisa za UNICODE karaktere
    ifstream file = openR(path);
    setCursor(crd);
    _setmode(_fileno(stdout), 0x00020000);

    // Citanje po jednog karaktera u svakoj iteraciji petlje iz
    // otvorenog fajla i ispisivanje UNICODE znakova (delovi mape,
    // igrac, prazna polja, skor, vreme, ...) 
    char c;
    while(file.get(c)) {
        switch(c) {
            case '!':
                wcout << L'\u2554';
                break;
            case '@':
                wcout << L'\u2557';
                break;
            case '#':
                wcout << L'\u255A';
                break;
            case '$':
                wcout << L'\u255D';
                break;
            case '%':
                wcout << L'\u2550';
                break;
            case '^':
                wcout << L'\u2551';
                break;
            case '&':
                wcout << L'\u2022';
                break;
            case ' ':
                wcout << L'\u00A0';
                break;
            case '-':
                wcout << L'\u002D';
                break;
            case '*':
                wcout << L'\u2588';
                break;
            case '\n':
                setCursor({crd.X, ++crd.Y});
                break;
            default:
                wcout << c;
                break;
        }
    }

    // Zatvaranje fajla
    file.close();
}
#pragma endregion

#pragma region MOVE
// Funkcija za proveru kolizije
bool isSolid(int pos, int udPOS = 1) {
    // Funkcija prihvata kao argumente sledecu poziciju u kojoj
    // igrac treba da se nalazi kao i vrednost promenljive za
    // posebne slucajeve

    // Otvaranje fajla ekrana igre u modu za citanje, upisivanje
    // njegovog sadrzaja u string 'line' i zatvaranje fajla
    screen_game.fileR = openR(screen_game.path);
    string line((istreambuf_iterator<char>(screen_game.fileR)), istreambuf_iterator<char>());
    screen_game.fileR.close();


    // U zavisnosti od promenljive za posebne slucajeve (posto je
    // mapa pravljena tako da u redu svako drugo polje je kruzic
    // koji treba skupiti, a svako prvo ' ', dolazilo je do
    // nezeljenih bugova i kretanja igraca po nemogucim poljima),
    // ispituje se da li je sledece polje zid mape i ukoliko
    // jeste - vraca TRUE, ukoliko nije - FALSE
    if(udPOS) {
        if(line[pos] == '&' || line[pos] == ' ') {
            return false;
        } else {
            return true;
        }
    } else {
        if( (line[pos] == '&' || line[pos] == ' ') &&
            (line[pos - 1] == ' ' && line[pos + 1] == ' ') ) {
                return false;
        } else {
            return true;
        }
    }
}

// Funkcija za pomeranje igraca NAGORE
void moveUP() {
    // Funkcija proverava da li je sledece polje moguce pozivanjem
    // funkcije za proveru kolizije.
    // Ukoliko jeste postavlja igraca na novo mesto, pamti
    // pretposlednji, kao i poslednji, pritisnut taster kako se ne
    // bi desavalo da se igrac pomera vise nego sto bi trebalo
    // ukoliko dodje do vise pritisaka dugmeta.
    // Ukoliko nije, nastavlja sa pomeranjem igraca po poslednjem
    // aktuelnom kursu.
    if(!isSolid((playerPos - 60), 0)) {
        prevPlayerPos = playerPos;
        playerPos -= 60;
        LAST_KEY_PRESSED[0] = LAST_KEY_PRESSED[1];
        LAST_KEY_PRESSED[1] = KEY_UP;
    } else {
        if(LAST_KEY_PRESSED[0] == KEY_LEFT) moveLEFT();
        else if(LAST_KEY_PRESSED[0] == KEY_RIGHT) moveRIGHT();
    }
}

// Funkcija za pomeranje igraca NADOLE
void moveDOWN() {
    // Funkcija proverava da li je sledece polje moguce pozivanjem
    // funkcije za proveru kolizije.
    // Ukoliko jeste postavlja igraca na novo mesto, pamti
    // pretposlednji, kao i poslednji, pritisnut taster kako se ne
    // bi desavalo da se igrac pomera vise nego sto bi trebalo
    // ukoliko dodje do vise pritisaka dugmeta.
    // Ukoliko nije, nastavlja sa pomeranjem igraca po poslednjem
    // aktuelnom kursu.
    if(!isSolid((playerPos + 60), 0)) {
        prevPlayerPos = playerPos;
        playerPos += 60;
        LAST_KEY_PRESSED[0] = LAST_KEY_PRESSED[1];
        LAST_KEY_PRESSED[1] = KEY_DOWN;
    } else {
        if(LAST_KEY_PRESSED[0] == KEY_LEFT) moveLEFT();
        else if(LAST_KEY_PRESSED[0] == KEY_RIGHT) moveRIGHT();
    }
}

// Funkcija za pomeranje igraca NALEVO
void moveLEFT() {
    // Funkcija proverava da li je sledece polje moguce pozivanjem
    // funkcije za proveru kolizije.
    // Ukoliko jeste postavlja igraca na novo mesto, pamti
    // pretposlednji, kao i poslednji, pritisnut taster kako se ne
    // bi desavalo da se igrac pomera vise nego sto bi trebalo
    // ukoliko dodje do vise pritisaka dugmeta.
    // Ukoliko nije, nastavlja sa pomeranjem igraca po poslednjem
    // aktuelnom kursu.
    if(!isSolid((playerPos - 2))) {
        prevPlayerPos = playerPos;
        playerPos -= 1;
        LAST_KEY_PRESSED[0] = LAST_KEY_PRESSED[1];
        LAST_KEY_PRESSED[1] = KEY_LEFT;
    } else {
        if(LAST_KEY_PRESSED[0] == KEY_UP) moveUP();
        else if(LAST_KEY_PRESSED[0] == KEY_DOWN) moveDOWN();
    }
}

// Funkcija za pomeranje igraca NADESNO
void moveRIGHT() {
    // Funkcija proverava da li je sledece polje moguce pozivanjem
    // funkcije za proveru kolizije.
    // Ukoliko jeste postavlja igraca na novo mesto, pamti
    // pretposlednji, kao i poslednji, pritisnut taster kako se ne
    // bi desavalo da se igrac pomera vise nego sto bi trebalo
    // ukoliko dodje do vise pritisaka dugmeta.
    // Ukoliko nije, nastavlja sa pomeranjem igraca po poslednjem
    // aktuelnom kursu.
    if(!isSolid((playerPos + 2))) {
        prevPlayerPos = playerPos;
        playerPos += 1;
        LAST_KEY_PRESSED[0] = LAST_KEY_PRESSED[1];
        LAST_KEY_PRESSED[1] = KEY_RIGHT;
    } else {
        if(LAST_KEY_PRESSED[0] == KEY_UP) moveUP();
        else if(LAST_KEY_PRESSED[0] == KEY_DOWN) moveDOWN();
    }
}
#pragma endregion

#pragma region MISCELLANEOUS
// Funkcija za otvaranje fajlova u modu za CITANJE
ifstream openR(string filename) {
    // Funkcija prima kao argument naziv fajla koji treba otvoriti
    // i otvara ga u modu za CITANJE, obradjuje greske pri
    //otvaranju i ukoliko je sve u redu, vraca otvoreni fajl
    ifstream file;
    file.open(filename);
    if (!file.is_open()) {
        cout << "Greska pri otvaranju! [CITANJE]" << endl;
        exit(1);
    } else {
        return file;
    }
}

// Funkcija za otvaranje fajlova u modu za UPIS
ofstream openW(string filename) {
    // Funkcija prima kao argument naziv fajla koji treba otvoriti
    // i otvara ga u modu za UPIS, obradjuje greske pri otvaranju
    // i ukoliko je sve u redu, vraca otvoreni fajl
    ofstream file;
    file.open(filename);
    if (!file.is_open()) {
        cout << "Greska pri otvaranju! [UPIS]" << endl;
        exit(1);
    } else {
        return file;
    }
}

// Funkcija za postavljanje kursora u konzoli
void setCursor(COORD c) {
    HANDLE consoleHandler = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleCursorPosition(consoleHandler, c);
}

// Funkcija za proveru kraja igre
void checkForEndGame() {
    // Funkcija proverava da li je vreme isteklo ili da li je
    // igrac sakupio maksimalan broj poena, igra se zavrsava
    // ukoliko je uslov zadovoljen
    if(!final_time || score == MAX_SCORE) {
        gameIsRunning = false;
    }
}
#pragma endregion

#pragma region SCREENS
// Funkcija za POCETNI EKRAN
void homeScreen() {
    // Funkcija crta zeljeni ekran i u zavisnosti od unosa
    // korisnika, menja dalji tok programa
    draw(screen_home.path, screen_home.crd);
    char c = getch();
    if(c == '1') {
        gameIsRunning = true;
        gameLoop();
    } else if(c == '2') {
        infoScreen_1();
    } else if(c == '3') {
        exitScreen();
    } else {
        homeScreen();
    }
}

// Funkcija za INFO EKRAN 1/2
void infoScreen_1() {
    // Funkcija crta zeljeni ekran i u zavisnosti od unosa
    // korisnika, menja dalji tok programa
    draw(screen_info[0].path, screen_info[0].crd);
    char c = getch();
    if(c == '1') {
        infoScreen_2();
    } else if(c == '2') {
        homeScreen();
    } else {
        infoScreen_1();
    }
}

// Funkcija za INFO EKRAN 2/2
void infoScreen_2() {
    // Funkcija crta zeljeni ekran i u zavisnosti od unosa
    // korisnika, menja dalji tok programa
    draw(screen_info[1].path, screen_info[1].crd);
    char c = getch();
    if(c == '1') {
        infoScreen_1();
    } else if(c == '2') {
        homeScreen();
    } else {
        infoScreen_2();
    }
}

// Funkcija za EKRAN NAKON ZAVRSETKA IGRE
void postGameScreen() {
    // Funkcija crta zeljeni ekran i u zavisnosti od unosa
    // korisnika, menja dalji tok programa
    draw(screen_postGame.path, screen_postGame.crd);
    char c = getch();
    if(c == '1') {
        setup();
        gameIsRunning = true;
        gameLoop();
    } else if(c == '2') {
        infoScreen_1();
    } else if(c == '3') {
        exitScreen();
    } else {
        postGameScreen();
    }
}

// Funkcija za IZLAZNI EKRAN
void exitScreen() {
    // Funkcija crta zeljeni ekran i u zavisnosti od unosa
    // korisnika, menja dalji tok programa
    draw(screen_exit.path, screen_exit.crd);
    char c = getch();
    if(c == '1') {
        exit(1);
    } else if(c == '2') {
        homeScreen();
    } else {
        exitScreen();
    }
}
#pragma endregion

#pragma region GAMELOOP
// Glavna funkcija u kojoj se odvija igra
void gameLoop() {
    // Ako je vrednost globalne promenljive 'gameIsRunning'
    // TRUE, inicijalizuje se pocetno vreme izvrsavanja programa
    // koje se kasnije koristi u funkciji za azuriranje vreme i
    // ulazi u petlju u kojoj se poziva glavna funkcija za azuriranje,
    // funkcija za iscrtavanje ikrana igre, funkcija koja proverava da
    // li je kraj igre i koja ogranicava prekomerno izvrsavanje petlje
    // pozivanjem funkcije 'sleep()', koja se izvrsava dok je vrednost
    // promenljive 'gameIsRunning' nepromenjena 
    if(gameIsRunning) {
        start = chrono::system_clock::now();
        while(gameIsRunning) {
            update();
            draw(screen_game.path, screen_game.crd);
            checkForEndGame();
            sleep((int)(1/60));
        }
    }

    // Nakon zavrsetka igre, poziva se funkcija za azuriranje i
    // inicijalizaciju ekrana koji se pojavljuje nakon igre kao
    // i funkcija 'postGameScreen()' koja je dalje odgovorna za
    // tok programa
    updatePostGame();
    postGameScreen();
}
#pragma endregion