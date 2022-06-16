#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>

typedef struct IPosition { 
    int x;
    int y;
} Position;

typedef enum IDirection {
    NONE, UP, RIGHT, DOWN, LEFT
} Direction_Snake;

typedef struct ISnake {
    int size;
    Position pos_head;
    Direction_Snake direction;
} Snake;

int random_position(int max_value){
    return 10;
}

void fill_matrix(int height, int width, int matrix[height][width], Position pos_to_fill, int value_to_fill);
void wprint_matrix(WINDOW *win, int height, int width, int matrix[height][width]);
WINDOW *create_newwin(int height, int width, int starty, int startx);
void destroy_win(WINDOW *local_win);
void move_snake(int height, int width, int matrix[height][width], Snake *snake);

Position next_position_by_direction(Position pos, Direction_Snake direction){
    Position next = pos;

    switch(direction){
        case UP:
            next.x--;
            break;
        case RIGHT:
            next.y++;
            break;
        case DOWN:
            next.x++;
            break;
        case LEFT:
            next.y--;
            break;
    }

    return next;
}

int is_next_food(int height, int width, int matrix[height][width], const Snake *snake){
    Position next_head = next_position_by_direction(snake->pos_head, snake->direction);

    if(matrix[next_head.x][next_head.y] < 0 )
        return true;
    else
        return false;
}


int main(int argc, char *argv[]) {
    // Setup Game Variables
    int width = 30, height=30;
    int matrix[height][width];

    // Reset all field of matrix
    for(int i = 0; i < height; i++)
        for(int j = 0; j < height; j++)
            matrix[i][j] = 0;

    int initial_size_snake = 3;

    Snake snake;
    snake.size = initial_size_snake;
    snake.pos_head.x = random_position(height);
    snake.pos_head.y = random_position(width);
    snake.direction = NONE;

    Position first_food;
    first_food.x = random_position(height);
    first_food.y = random_position(width);

    if(first_food.x == snake.pos_head.x && first_food.y == snake.pos_head.y)
        first_food.y--;

    matrix[snake.pos_head.x][snake.pos_head.y] = snake.size;
    matrix[first_food.x][first_food.y] = -1;

    // Setup Ncurses
    initscr();            /* Start curses mode 		*/
    cbreak();             /* Line buffering disabled, Pass on
                           * everty thing to me 		*/
    keypad(stdscr, TRUE); /* I need that nifty F1 	*/

    int starty = (LINES - height) / 2; /* Calculating for a center placement */
    int startx = (COLS - width) / 2;   /* of the window		*/

    printw("Pressiona F1 para sair");
    refresh(); // Por algum motivo isso é obrigatorio

    WINDOW *display;
    display = newwin(height, width, starty, startx);
    box(display, 0, 0);
    wrefresh(display);

    int input;
    while ((input = getch()) != KEY_F(1)) {
        switch(input){
            case KEY_LEFT:
                snake.direction = LEFT;
                break;
            case KEY_RIGHT:
                snake.direction = RIGHT;
                break;
            case KEY_DOWN:
                snake.direction = DOWN;
                break;
            case KEY_UP:
                snake.direction = UP;
                break;
        }

        int is_food = is_next_food(height, width, matrix, &snake);
        if(is_food){
            snake.size++;
            Position food_pos = next_position_by_direction(snake.pos_head, snake.direction);
            matrix[food_pos.x][food_pos.y] = snake.size;
        } else {
            move_snake(height, width, matrix, &snake);
        }

        wprint_matrix(display, height, width, matrix);
        box(display, 0, 0);
        wrefresh(display);
    }

    endwin(); /* End curses mode		  */
    return 0;
}

void fill_matrix(int height, int width, int matrix[height][width], Position pos_to_fill, int value_to_fill){
    matrix[pos_to_fill.x][pos_to_fill.y] = value_to_fill;
}

void wprint_matrix(WINDOW *win, int height, int width, int matrix[height][width]){
    for(int x = 0; x < height; x++){
        for(int y = 0; y < width; y++){
            int value = matrix[x][y];
            if(value < 0) mvwprintw(win, x, y, "X" );
            if(value == 0) mvwprintw(win, x, y, " " );
            if(value > 0) mvwprintw(win, x, y, "O" );
        }
    }
}

void move_snake(int height, int width, int matrix[height][width], Snake *snake){
    Position next_head;
    for(int x = 0; x < height; x++){
        for(int y = 0; y < width; y++){
            if(matrix[x][y] > 0) {
                // Se for a cabeça preenche o campo correspondente à direcao atual
                int size = snake->size;
                if(matrix[x][y] == size){
                    switch(snake->direction){
                        case UP:
                            snake->pos_head.x = x-1;
                            snake->pos_head.y = y;
                            break;
                        case RIGHT:
                            snake->pos_head.x = x;
                            snake->pos_head.y = y+1;
                            break;
                        case DOWN:
                            snake->pos_head.x = x+1;
                            snake->pos_head.y = y;
                            break;
                        case LEFT:
                            snake->pos_head.x = x;
                            snake->pos_head.y = y-1;
                            break;
                    }

                    next_head = snake->pos_head;
                }

                matrix[x][y]--;
            }
        }
    }
    matrix[next_head.x][next_head.y] = snake->size;
}

/* int isEat(Position pos){ */
/*     if(pos < 0 ) */
/*         return 1; */
/*     return 0; */
/* } */

/* void eat_food_snake(int height, int width, int matrix[height][width]){ */
/*     for(int x = 0; x < height; x++){ */
/*         for(int y = 0; y < height; y++){ */
/*             int value = matrix[x][y]; */
/*             if(value < 0) mvwprintw(win, y, x, "X" ); */
/*             if(value == 0) mvwprintw(win, y, x, " " ); */
/*             if(value > 0) mvwprintw(win, y, x, "O" ); */
/*         } */
/*     } */
/* } */









void destroy_win(WINDOW *local_win) {
  /* box(local_win, ' ', ' '); : This won't produce the desired
   * result of erasing the window. It will leave it's four corners
   * and so an ugly remnant of window.
   */
  wborder(local_win, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
  /* The parameters taken are
   * 1. win: the window on which to operate
   * 2. ls: character to be used for the left side of the window
   * 3. rs: character to be used for the right side of the window
   * 4. ts: character to be used for the top side of the window
   * 5. bs: character to be used for the bottom side of the window
   * 6. tl: character to be used for the top left corner of the window
   * 7. tr: character to be used for the top right corner of the window
   * 8. bl: character to be used for the bottom left corner of the window
   * 9. br: character to be used for the bottom right corner of the window
   */
  wrefresh(local_win);
  delwin(local_win);
}
