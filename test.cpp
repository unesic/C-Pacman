#include <iostream>
#include <ncurses.h>

int main() {
  int c;

  initscr();                 /* Start curses mode     */
  // printw("Hello World !!!"); /* Print Hello World    */
  // refresh();                 /* Print it on to the real screen */
  // getch();                   /* Wait for user input */
  // endwin();                  /* End curses mode    */

  // std::cout << c;

  // do {
    // intrflush(stdscr, FALSE);
    // keypad(stdscr, TRUE);
  // } while(c != KEY_LEFT);
  timeout(-1);
    c = getch();
    // refresh();

  std::cout << c;
  if (c == 1) {
    printf("%o\n", c);
  } else {
    printf("%c\n", c);
  }
  // endwin();
  
  return 0;
}