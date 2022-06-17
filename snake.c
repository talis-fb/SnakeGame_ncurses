#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

/* #include <time.h> */
#include <unistd.h>

#include <ncurses.h>

// Types
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

// Utils
int random_position(int max_value){
    return rand()%max_value;
}

// Methods to handle Matrix
void fill_matrix(int height, int width, int matrix[height][width], Position pos_to_fill, int value_to_fill){
    matrix[pos_to_fill.x][pos_to_fill.y] = value_to_fill;
}


// Methods to handle Windows
void wprint_matrix(WINDOW *win, int height, int width, int matrix[height][width]);
void destroy_win(WINDOW *local_win);

void game_over_screen(){
    clear();
    /* printw(" ▄▄▄▄▄▄▄ ▄▄▄▄▄▄▄ ▄▄   ▄▄ ▄▄▄▄▄▄▄    ▄▄▄▄▄▄▄ ▄▄   ▄▄ ▄▄▄▄▄▄▄ ▄▄▄▄▄▄"); */
    /* printw("█       █       █  █▄█  █       █  █       █  █ █  █       █   ▄  █"); */
    /* printw("█   ▄▄▄▄█   ▄   █       █    ▄▄▄█  █   ▄   █  █▄█  █    ▄▄▄█  █ █ █"); */
    /* printw("█  █  ▄▄█  █▄█  █       █   █▄▄▄   █  █ █  █       █   █▄▄▄█   █▄▄█▄"); */
    /* printw("█  █ █  █       █       █    ▄▄▄█  █  █▄█  █       █    ▄▄▄█    ▄▄  █"); */
    /* printw("█  █▄▄█ █   ▄   █ ██▄██ █   █▄▄▄   █       ██     ██   █▄▄▄█   █  █ █"); */
    /* printw("█▄▄▄▄▄▄▄█▄▄█ █▄▄█▄█   █▄█▄▄▄▄▄▄▄█  █▄▄▄▄▄▄▄█ █▄▄▄█ █▄▄▄▄▄▄▄█▄▄▄█  █▄█"); */

    int y = (COLS - 60) / 2;
    int x = (LINES - 6) / 2;

    mvprintw(x,y,  "################################################################");
    mvprintw(x+1,y,"################################################################");
    mvprintw(x+2,y,"###############          GAME OVER          ####################");
    mvprintw(x+3,y,"################################################################");
    mvprintw(x+4,y,"################################################################");
    mvprintw(x+5,y,"");
    mvprintw(x+6,y,"          < Pressiona qualquer tecla para sair >");

    refresh();
}

// Methods to handle Snake
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


Position next_position_by_direction(Position pos, Direction_Snake direction){
    Position next = pos;

    if(direction == UP) next.x--;
    if(direction == DOWN) next.x++;
    if(direction == RIGHT) next.y++;
    if(direction == LEFT) next.y--;

    return next;
}

int is_food(Position pos, int height, int width, int matrix[height][width]){
    // TRUE: É comida
    if(matrix[pos.x][pos.y] == -1 )
        return 1;

    // Se for um pedaço da cobra
    if(matrix[pos.x][pos.y] > 0 )
        return -2;

    if(pos.x < 0 || pos.x >= height) return -1;
    if(pos.y < 0 || pos.y >= width) return -1;

    // Espaço vazio
    return 0;
}

void create_new_fruit(int height, int width, int matrix[height][width]){
    int food_in_safe_place = 0;

    while(!food_in_safe_place){
        int x = random_position(height);
        int y = random_position(width);

        if( (0 <= x) && (x < height) && (0 <= y) && (y < width) ){
            if(matrix[x][y] == 0){
                matrix[x][y] = -1;
                food_in_safe_place = 1;
            }
        }
    }
}

static int input;
static Snake snake;

static int game_is_over = 0;
static int score = 0;

void* read_input(void *args){
    while((input = getch()) && !game_is_over){
        switch(input){
            case KEY_LEFT:
            case 'a':
                snake.direction = LEFT;
                break;
            case KEY_RIGHT:
            case 'd':
                snake.direction = RIGHT;
                break;
            case KEY_DOWN:
            case 's':
                snake.direction = DOWN;
                break;
            case KEY_UP:
            case 'w':
                snake.direction = UP;
                break;
        }
    }

    return NULL;
}

int main(int argc, char *argv[]) {
    srand(time(NULL));

    // Setup Ncurses
    initscr();            /* Start curses mode 		*/
    cbreak();             /* Line buffering disabled, Pass on everty thing to me 		*/
    keypad(stdscr, TRUE); /* I need that nifty F1 	*/

    // Print difilculdade choice
    mvprintw(10, 10, "Bem-vindo ao SNAKE GAME");
    mvprintw(11, 10, "(0) \"Mamae, posso jogar?\"");
    mvprintw(12, 10, "(1) Facil");
    mvprintw(13, 10, "(2) Medio");
    mvprintw(14, 10, "(3) Dificil");
    mvprintw(15, 10, "(4) Manda a braba!");
    mvprintw(16, 10, " > ");
    refresh();

    // get choice e 
    char d[5];
    getstr(d);
    int dificuldade_selecionada = atoi(d);

    // Agr nao printa mais oq o cara escreve
    noecho();

    // Setup Game Variables
    int width = (5 * dificuldade_selecionada) + 10;
    int height = (5 * dificuldade_selecionada) + 10;
    int initial_size_snake = ( dificuldade_selecionada < 2 ) ? 5 : 3;
    int speed = 100000 - ( dificuldade_selecionada * 10000 ) ;

    /* int y = (COLS - 60) / 2; */
    /* int x = (LINES - 6) / 2; */

    int matrix[height][width];

    // Setup GRID
    int starty = (LINES - height) / 2; /* Calculating for a center placement */
    int startx = (COLS - width) / 2;   /* of the window		*/

    WINDOW *display;
    display = newwin(height, width, starty, startx);
    wborder(display, '|', '|', '-', '-', '+', '+', '+', '+');
    wrefresh(display);

    // Reset all field of matrix
    for(int i = 0; i < height; i++)
        for(int j = 0; j < height; j++)
            matrix[i][j] = 0;

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

    // Start thread just to 
    pthread_t input_thread;
    pthread_create(&input_thread, NULL, read_input, NULL);

    while (input != KEY_F(1)) {
        Position next_position = next_position_by_direction(snake.pos_head, snake.direction);
        int is_new_step_food = is_food(next_position, height, width, matrix);

        // END GAME
        if(is_new_step_food < 0 && snake.direction != NONE){
            // Destroi o grid do jogo e imprime a tela de game over
            destroy_win(display);
            clear();
            game_over_screen();
            refresh();

            // Seta a variavel para encerrar o loop da outra thread e espera ela encerrar
            // como ela tá travada para receber o input do usuario, ela já serve como o ultimo input
            // mencionado na tela de game over
            game_is_over = 1;
            pthread_join(input_thread, NULL);

            break;
        }

        // IF NEXT STEP IS A FRUIT
        if(is_new_step_food > 0){
            snake.size++;
            Position food_pos = next_position_by_direction(snake.pos_head, snake.direction);
            matrix[food_pos.x][food_pos.y] = snake.size;
            snake.pos_head = food_pos;
            create_new_fruit(height, width, matrix);
        } else {
            move_snake(height, width, matrix, &snake);
        }

        wprint_matrix(display, height, width, matrix);

        // TODO: a box está pegando as bordas
        wborder(display, '|', '|', '-', '-', '+', '+', '+', '+');
        wrefresh(display);
        usleep(speed);
    }

    endwin(); /* End curses mode		  */
    return 0;
}

void wprint_matrix(WINDOW *win, int height, int width, int matrix[height][width]){
    for(int x = 0; x < height; x++){
        for(int y = 0; y < width; y++){
            int value = matrix[x][y];
            if(value < 0){
                mvwaddch(win, x, y, 'X' | COLOR_PAIR(2) | A_UNDERLINE);
            }
            if(value == 0){
                mvwprintw(win, x, y, " " );
            }
            if(value > 0) {
                mvwprintw(win, x, y, "O" );
            }
        }
    }
}


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
