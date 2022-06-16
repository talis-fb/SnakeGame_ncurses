#include <ncurses.h>

int main() {
    initscr();                 /* Start curses mode 		  */
    raw();                /* Line buffering disabled	*/
    keypad(stdscr, TRUE); /* We get F1, F2 etc..		*/
    noecho();     /* Don't echo() while we do getch */
    start_color();        /* Start the color functionality */


    char input;
    int game_over = 0;
    while(!game_over){

    }

        printw("Hello World !!!"); /* Print Hello World		  */
    refresh();                 /* Print it on to the real screen */
    getch();                   /* Wait for user input */
    endwin();                  /* End curses mode		  */


    return 0;
}
