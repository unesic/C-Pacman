#pragma region INCLUDES

#include <iostream>
#include <fstream>
#include <sys/uio.h>
#include <unistd.h>
#include <ncurses.h>
#include <chrono>
#include <ctime>

#pragma endregion

using namespace std;

#pragma region GLOBAL VARIABLES

#define KEY_UP 119
#define KEY_DOWN 115
#define KEY_LEFT 97
#define KEY_RIGHT 100
#define KEY_ONE 49
#define KEY_TWO 50
#define KEY_THR 51
#define MAX_SCORE 302
#define MAX_TIME 10

typedef struct _COORD
{
	short int X;
	short int Y;
} COORD, *PCOORD;

struct screen
{
	string path;
	ifstream fileR;
	ofstream fileW;
	COORD crd;
} screen_home, screen_game, SCREEN_INFO[2], SCREEN_EXIT, SCREEN_POST;

ifstream OG_GAME_MAP;

chrono::duration<double> start = chrono::system_clock::now();
unsigned short int SCORE;
unsigned short int curr_player_pos;
unsigned short int prev_player_pos;
unsigned short int *last_key_pressed;

unsigned short int final_score = 0, final_time = 0;

bool game_is_running = false;

#pragma endregion

#pragma region FUNCTIONS
int kbhit();
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

int main()
{

	initscr();

	cbreak();
	noecho();
	nodelay(stdscr, TRUE);

	scrollok(stdscr, TRUE);
	// timeout(1);
	setup();
	homeScreen();
	endwin();

	return 0;
}

#pragma region SETUP
// Funkcija inicijalizacije
void setup()
{
	// Dodeljivanje vrednosti globalnim promenljivama
	SCORE = 1;
	curr_player_pos = 1409;
	prev_player_pos = 0;
	last_key_pressed = new unsigned short int[2];

	// Inicijalizacija strukture pocetnog ekrana
	screen_home.path = "./home.txt";
	screen_home.crd.X = 0;
	screen_home.crd.Y = 2;

	// Inicijalizacija ekrana sa uputstvima i informacijama
	SCREEN_INFO[0].path = "./info-1.txt";
	screen_home.crd.X = 0;
	screen_home.crd.Y = 2;
	SCREEN_INFO[1].path = "./info-2.txt";
	screen_home.crd.X = 0;
	screen_home.crd.Y = 2;

	// Inicijalizacija ekrana koji se pojavljuje nakon igre
	SCREEN_POST.path = "./post-game.txt";
	screen_home.crd.X = 0;
	screen_home.crd.Y = 2;

	// Inicijalizacija ekrana izlaza iz igre
	SCREEN_EXIT.path = "./exit.txt";
	screen_home.crd.X = 0;
	screen_home.crd.Y = 2;

	// Otvaranje originalnog fajla mape
	OG_GAME_MAP = openR("./level.txt");

	// Inicijalizacija strukture igre
	screen_game.path = "./game.txt";
	screen_game.fileW = openW(screen_game.path);
	screen_home.crd.X = 0;
	screen_home.crd.Y = 2;

	// Upisivanje sadrzaja originalne mape u mapu igre
	string line((istreambuf_iterator<char>(OG_GAME_MAP)), istreambuf_iterator<char>());
	screen_game.fileW << line;

	// Zatvaranje otvorenih fajlova
	OG_GAME_MAP.close();
	screen_game.fileW.close();
}
#pragma endregion

#pragma region UPDATE
// Funkcija za azuriranje mape
void updateMap()
{
	// Otvaranje fajla ekrana mape u modu za citanje,
	// upisivanje njegov sadrzaja u string 'line' i
	// zatvaranje fajla ekrana mape
	screen_game.fileR = openR(screen_game.path);
	string line((istreambuf_iterator<char>(screen_game.fileR)), istreambuf_iterator<char>());
	screen_game.fileR.close();

	// Portali na levoj i desnoj strani mape
	if (curr_player_pos == 841)
		curr_player_pos = 896;
	else if (curr_player_pos == 897)
		curr_player_pos = 842;

	// Azuriranje skora i vremena, pozicije igraca i prethodnog
	// polja u kome se igrac nalazio u string 'line'
	if (line[curr_player_pos] == '&')
		SCORE++;
	if (line[prev_player_pos] != ' ')
		line[prev_player_pos] = ' ';
	line[curr_player_pos] = '*';
	line = updateScore(line);
	line = updateTime(line);

	// Otvaranje fajla igre u modu za pisanje, upisivanje
	// sadrzaja stringa 'line' u njega i zatvaranje
	screen_game.fileW = openW(screen_game.path);
	screen_game.fileW << line;
	screen_game.fileW.close();
}

// Funkcija za azuriranje igraca
void updatePlayer()
{
	// Funkcija prvo proverava da li je pritisnuto neko dugme na tastaturi
	// Ukoliko jeste, proverava koje je u pitanju i menja pravac igraca
	// Ukoliko nije, nastavlja poslednjim aktuelnim pravcem
	if (kbhit() && (last_key_pressed[1] = getch()) != last_key_pressed[0])
	{
		switch (last_key_pressed[1])
		{
		case ARR_UP:
			moveUP();
			break;

		case ARR_DOWN:
			moveDOWN();
			break;

		case KEY_LEFT:
			moveLEFT();
			break;

		case ARR_RIGHT:
			moveRIGHT();
			break;
		}
		refresh();
	}
	else
	{
		switch (last_key_pressed[1])
		{
		case ARR_UP:
			moveUP();
			break;

		case ARR_DOWN:
			moveDOWN();
			break;

		case KEY_LEFT:
			moveLEFT();
			break;

		case ARR_RIGHT:
			moveRIGHT();
			break;
		}
		refresh();
	}
}

// Funkcija za azuriranje skora
string updateScore(string line)
{
	// Funkcija prima string 'line' kao argument od funkcije 'updateMap'
	// Menja sadrzaj stringa tako da upise rec 'SCORE:  ' i trenutni skor
	// Vraca izmenjeni string 'line'
	string scoreText = "SKOR:   " + to_string(SCORE);

	for (int i = 0; i < scoreText.length(); i++)
	{
		line[804 + i] = scoreText[i];
	}

	final_score = SCORE;

	return line;
}

// Funkcija za azuriranje vremena
string updateTime(string line)
{
	// Funkcija uzima trenutno vreme i racuna vreme koje ce se
	// prikazati u igri. Azurira string 'line' novim vremenom
	// i vraca isti
	chrono::duration<double> current = chrono::system_clock::now();
	chrono::duration<double> elapsed = current - start;
	unsigned short int elapsedTime = (int)(elapsed.count());
	unsigned short int displayTime = MAX_TIME - elapsedTime;

	string timeText = "VREME:  " + to_string(displayTime);
	int length = timeText.length();

	if (displayTime < 100)
	{
		line[864 + length] = ' ';
	}
	else if (displayTime < 10)
	{
		line[864 + length - 1] = ' ';
	}

	for (int i = 0; i < length; i++)
	{
		line[864 + i] = timeText[i];
	}

	final_time = MAX_TIME - elapsedTime;

	return line;
}

// Funkcija za azuriranje ekrana nakon igre
void updatePostGame()
{
	// Otvaranje fajla nakon igre u modu za citanje,
	// upisivanje njegovog sadrzaja u string 'line' i zatvaranje
	SCREEN_POST.fileR = openR(SCREEN_POST.path);
	string line((istreambuf_iterator<char>(SCREEN_POST.fileR)), istreambuf_iterator<char>());
	SCREEN_POST.fileR.close();

	// Konvertovanje ostvarenog broja poena i vremena u string
	string scoreStr = to_string(final_score);
	string timeStr = to_string(final_time);

	// Upisivanje ostvarenog broja poena i vremena u string 'line'
	for (int i = 0; i < scoreStr.length(); i++)
	{
		line[512 + i] = scoreStr[i];
	}
	for (int i = 0; i < timeStr.length(); i++)
	{
		line[572 + i] = timeStr[i];
	}

	SCREEN_POST.fileW = openW(SCREEN_POST.path);
	SCREEN_POST.fileW << line;
	SCREEN_POST.fileW.close();
}

// Glavna funkcija za azuriranje
void update()
{
	// Glavna funkcija za azuriranje koja
	// poziva ostale funkcije azuriranja
	updateMap();
	updatePlayer();
}
#pragma endregion

#pragma region DRAW
// Funkcija za iscrtavanje mape i fajlova ekrana
void draw(string path, COORD crd)
{
	// Funkcija prima kao argumente naziv fajla i koordinatu od
	// koje treba poceti sa iscrtavanjem mape. Otvara proslednjeni
	// fajl, postavlja kursor u komandnom prozoru na proslednjene
	// koordinate i postavlja mod ispisa za UNICODE karaktere
	ifstream file = openR(path);
	setCursor(crd);
	std::wcout.imbue(std::locale("en_US.UTF-8"));

	// Citanje po jednog karaktera u svakoj iteraciji petlje iz
	// otvorenog fajla i ispisivanje UNICODE znakova (delovi mape,
	// igrac, prazna polja, skor, vreme, ...)
	char c;
	while (file.get(c))
	{
		switch (c)
		{
		case '!':
			std::wcout << L'\u2554';
			break;
		case '@':
			std::wcout << L'\u2557';
			break;
		case '#':
			std::wcout << L'\u255A';
			break;
		case '$':
			std::wcout << L'\u255D';
			break;
		case '%':
			std::wcout << L'\u2550';
			break;
		case '^':
			std::wcout << L'\u2551';
			break;
		case '&':
			std::wcout << L'\u2022';
			break;
		case ' ':
			std::wcout << L'\u00A0';
			break;
		case '-':
			std::wcout << '-';
			break;
		case '*':
			std::wcout << L'\u2588';
			break;
		case '\n':
			crd.Y++;
			setCursor(crd);
			break;
		default:
			std::wcout << c;
			break;
		}
	}

	// Zatvaranje fajla
	file.close();
}
#pragma endregion

#pragma region MOVE
// Funkcija za proveru kolizije
bool isSolid(int pos, int udPOS = 1)
{
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
	if (udPOS)
	{
		if (line[pos] == '&' || line[pos] == ' ')
		{
			return false;
		}
		else
		{
			return true;
		}
	}
	else
	{
		if ((line[pos] == '&' || line[pos] == ' ') &&
				(line[pos - 1] == ' ' && line[pos + 1] == ' '))
		{
			return false;
		}
		else
		{
			return true;
		}
	}
}

// Funkcija za pomeranje igraca NAGORE
void moveUP()
{
	// Funkcija proverava da li je sledece polje moguce pozivanjem
	// funkcije za proveru kolizije.
	// Ukoliko jeste postavlja igraca na novo mesto, pamti
	// pretposlednji, kao i poslednji, pritisnut taster kako se ne
	// bi desavalo da se igrac pomera vise nego sto bi trebalo
	// ukoliko dodje do vise pritisaka dugmeta.
	// Ukoliko nije, nastavlja sa pomeranjem igraca po poslednjem
	// aktuelnom kursu.
	if (!isSolid((curr_player_pos - 60), 0))
	{
		prev_player_pos = curr_player_pos;
		curr_player_pos -= 60;
		last_key_pressed[0] = last_key_pressed[1];
		last_key_pressed[1] = ARR_UP;
	}
	else
	{
		if (last_key_pressed[0] == KEY_LEFT)
			moveLEFT();
		else if (last_key_pressed[0] == ARR_RIGHT)
			moveRIGHT();
	}
}

// Funkcija za pomeranje igraca NADOLE
void moveDOWN()
{
	// Funkcija proverava da li je sledece polje moguce pozivanjem
	// funkcije za proveru kolizije.
	// Ukoliko jeste postavlja igraca na novo mesto, pamti
	// pretposlednji, kao i poslednji, pritisnut taster kako se ne
	// bi desavalo da se igrac pomera vise nego sto bi trebalo
	// ukoliko dodje do vise pritisaka dugmeta.
	// Ukoliko nije, nastavlja sa pomeranjem igraca po poslednjem
	// aktuelnom kursu.
	if (!isSolid((curr_player_pos + 60), 0))
	{
		prev_player_pos = curr_player_pos;
		curr_player_pos += 60;
		last_key_pressed[0] = last_key_pressed[1];
		last_key_pressed[1] = ARR_DOWN;
	}
	else
	{
		if (last_key_pressed[0] == KEY_LEFT)
			moveLEFT();
		else if (last_key_pressed[0] == ARR_RIGHT)
			moveRIGHT();
	}
}

// Funkcija za pomeranje igraca NALEVO
void moveLEFT()
{
	// Funkcija proverava da li je sledece polje moguce pozivanjem
	// funkcije za proveru kolizije.
	// Ukoliko jeste postavlja igraca na novo mesto, pamti
	// pretposlednji, kao i poslednji, pritisnut taster kako se ne
	// bi desavalo da se igrac pomera vise nego sto bi trebalo
	// ukoliko dodje do vise pritisaka dugmeta.
	// Ukoliko nije, nastavlja sa pomeranjem igraca po poslednjem
	// aktuelnom kursu.
	if (!isSolid((curr_player_pos - 2)))
	{
		prev_player_pos = curr_player_pos;
		curr_player_pos -= 1;
		last_key_pressed[0] = last_key_pressed[1];
		last_key_pressed[1] = KEY_LEFT;
	}
	else
	{
		if (last_key_pressed[0] == ARR_UP)
			moveUP();
		else if (last_key_pressed[0] == ARR_DOWN)
			moveDOWN();
	}
}

// Funkcija za pomeranje igraca NADESNO
void moveRIGHT()
{
	// Funkcija proverava da li je sledece polje moguce pozivanjem
	// funkcije za proveru kolizije.
	// Ukoliko jeste postavlja igraca na novo mesto, pamti
	// pretposlednji, kao i poslednji, pritisnut taster kako se ne
	// bi desavalo da se igrac pomera vise nego sto bi trebalo
	// ukoliko dodje do vise pritisaka dugmeta.
	// Ukoliko nije, nastavlja sa pomeranjem igraca po poslednjem
	// aktuelnom kursu.
	if (!isSolid((curr_player_pos + 2)))
	{
		prev_player_pos = curr_player_pos;
		curr_player_pos += 1;
		last_key_pressed[0] = last_key_pressed[1];
		last_key_pressed[1] = ARR_RIGHT;
	}
	else
	{
		if (last_key_pressed[0] == ARR_UP)
			moveUP();
		else if (last_key_pressed[0] == ARR_DOWN)
			moveDOWN();
	}
}
#pragma endregion

#pragma region MISCELLANEOUS
// Funkcija za otvaranje fajlova u modu za CITANJE
ifstream openR(string filename)
{
	// Funkcija prima kao argument naziv fajla koji treba otvoriti
	// i otvara ga u modu za CITANJE, obradjuje greske pri
	//otvaranju i ukoliko je sve u redu, vraca otvoreni fajl
	ifstream file;
	file.open(filename);
	if (!file.is_open())
	{
		cout << "Greska pri otvaranju! [CITANJE]" << endl;
		exit(1);
	}
	else
	{
		return file;
	}
}

// Funkcija za otvaranje fajlova u modu za UPIS
ofstream openW(string filename)
{
	// Funkcija prima kao argument naziv fajla koji treba otvoriti
	// i otvara ga u modu za UPIS, obradjuje greske pri otvaranju
	// i ukoliko je sve u redu, vraca otvoreni fajl
	ofstream file;
	file.open(filename);
	if (!file.is_open())
	{
		cout << "Greska pri otvaranju! [UPIS]" << endl;
		exit(1);
	}
	else
	{
		return file;
	}
}

// Funkcija za postavljanje kursora u konzoli
void setCursor(COORD c)
{
	// HANDLE consoleHandler = GetStdHandle(STD_OUTPUT_HANDLE);
	// SetConsoleCursorPosition(consoleHandler, c);
	// std::cout << "\033[%d;%dm" << c.X << c.Y;
	printf("%c[%d;%df", 0x1B, c.Y, c.X);
	// move(0, 0);
}

// Funkcija za proveru kraja igre
void checkForEndGame()
{
	// Funkcija proverava da li je vreme isteklo ili da li je
	// igrac sakupio maksimalan broj poena, igra se zavrsava
	// ukoliko je uslov zadovoljen
	if (!final_time || SCORE == MAX_SCORE)
	{
		game_is_running = false;
	}
}
#pragma endregion

#pragma region SCREENS
// Funkcija za POCETNI EKRAN
void homeScreen()
{
	// Funkcija crta zeljeni ekran i u zavisnosti od unosa
	// korisnika, menja dalji tok programa
	draw(screen_home.path, screen_home.crd);
	char c;
	// std::cin >> c;

	// intrflush(stdscr, FALSE);
	// keypad(stdscr, TRUE);
	// refresh();
	// c = getch();
	std::cin >> c;
	// cout << c;
	if (c == '1')
	{
		game_is_running = true;
		gameLoop();
	}
	else if (c == '2')
	{
		infoScreen_1();
	}
	else if (c == '3')
	{
		exitScreen();
	}
	else
	{
		homeScreen();
	}
}

// Funkcija za INFO EKRAN 1/2
void infoScreen_1()
{
	// Funkcija crta zeljeni ekran i u zavisnosti od unosa
	// korisnika, menja dalji tok programa
	draw(SCREEN_INFO[0].path, SCREEN_INFO[0].crd);
	char c;
	std::cin >> c;
	if (c == '1')
	{
		infoScreen_2();
	}
	else if (c == '2')
	{
		homeScreen();
	}
	else
	{
		infoScreen_1();
	}
}

// Funkcija za INFO EKRAN 2/2
void infoScreen_2()
{
	// Funkcija crta zeljeni ekran i u zavisnosti od unosa
	// korisnika, menja dalji tok programa
	draw(SCREEN_INFO[1].path, SCREEN_INFO[1].crd);
	char c;
	std::cin >> c;
	if (c == '1')
	{
		infoScreen_1();
	}
	else if (c == '2')
	{
		homeScreen();
	}
	else
	{
		infoScreen_2();
	}
}

// Funkcija za EKRAN NAKON ZAVRSETKA IGRE
void postGameScreen()
{
	// Funkcija crta zeljeni ekran i u zavisnosti od unosa
	// korisnika, menja dalji tok programa
	draw(SCREEN_POST.path, SCREEN_POST.crd);
	char c;
	std::cin >> c;
	if (c == '1')
	{
		setup();
		game_is_running = true;
		gameLoop();
	}
	else if (c == '2')
	{
		infoScreen_1();
	}
	else if (c == '3')
	{
		exitScreen();
	}
	else
	{
		postGameScreen();
	}
}

// Funkcija za IZLAZNI EKRAN
void exitScreen()
{
	// Funkcija crta zeljeni ekran i u zavisnosti od unosa
	// korisnika, menja dalji tok programa
	draw(SCREEN_EXIT.path, SCREEN_EXIT.crd);
	char c;
	std::cin >> c;
	if (c == '1')
	{
		exit(1);
	}
	else if (c == '2')
	{
		homeScreen();
	}
	else
	{
		exitScreen();
	}
}
#pragma endregion

#pragma region GAMELOOP
// Glavna funkcija u kojoj se odvija igra
void gameLoop()
{
	// Ako je vrednost globalne promenljive 'gameIsRunning'
	// TRUE, inicijalizuje se pocetno vreme izvrsavanja programa
	// koje se kasnije koristi u funkciji za azuriranje vreme i
	// ulazi u petlju u kojoj se poziva glavna funkcija za azuriranje,
	// funkcija za iscrtavanje ikrana igre, funkcija koja proverava da
	// li je kraj igre i koja ogranicava prekomerno izvrsavanje petlje
	// pozivanjem funkcije 'sleep()', koja se izvrsava dok je vrednost
	// promenljive 'gameIsRunning' nepromenjena
	if (game_is_running)
	{
		start = chrono::system_clock::now();
		while (game_is_running)
		{
			update();
			draw(screen_game.path, screen_game.crd);
			checkForEndGame();
			usleep(1000000 / 60);
			// sleep((int)(10/60));
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

int kbhit(void)
{
	int ch = getch();

	if (ch != ERR)
	{
		ungetch(ch);
		return 1;
	}
	else
	{
		return 0;
	}
}
