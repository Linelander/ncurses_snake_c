#include <stdio.h>
#include <curses.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>


#define SPEED 50


typedef struct coord {
    int y;
    int x;
} coord;


typedef struct segment {
    coord ncoord;
    struct segment *next;
    struct segment *previous;
} segment;




struct segment* create_segment(coord pos);
bool check_failure(void);
bool coord_equals(struct coord a, struct coord b);
void headcrawl(void);
void bodycrawl(void);
void grow(coord start);
void drawall(void);
void get_direction(void);
void place_fruit(void);
void screenwrap(void);
void free_snake(void);

int SCORE = 0;
int GROW_FACTOR = 20;

int SIZE = 32;
int WIN_HEIGHT = -1;
int WIN_WIDTH = -1;

struct coord RIGHT = {0, 1};
struct coord LEFT = {0, -1};
struct coord UP = {-1, 0};
struct coord DOWN = {1, -0};
struct coord CURRENTDIRECTION = {0, 0};

coord START = {1, 1};

struct segment* HEAD = NULL;
struct segment* TAIL = NULL;

struct coord FRUIT = {-1, -1};




int main(int argc, char **argv)
{
    WIN_HEIGHT = SIZE;
    WIN_WIDTH = SIZE * 2;
    
    initscr();
    noecho();
    cbreak(); 
    curs_set(0);
    timeout(SPEED);
    refresh();

    WINDOW *snakewin = newwin(WIN_HEIGHT, WIN_WIDTH, 0, 0);
    box(snakewin, 0, 0);
    nodelay(snakewin, TRUE);

    coord start = {1,1};
    grow(start);                // does this have anything to do with why the snake doesn't grow after eating its first fruit?
    
    wrefresh(snakewin);
    place_fruit();

    bool loop = TRUE;
    while(loop)
    {                
        // Game logic order
        get_direction();
        headcrawl();
        loop = check_failure();
        if (HEAD->ncoord.x == FRUIT.x
        && HEAD->ncoord.y == FRUIT.y)
        {
            for (int i = 0; i < GROW_FACTOR; i++)
            {
                grow(TAIL->ncoord);
            }
            place_fruit();
        }
        bodycrawl();
        drawall();
    }
    void place_fruit(void);
    endwin();

    printf("SCORE: %i\n", SCORE);
    napms(1000);

    free_snake();
    return 0;
}
    
void get_direction(void)
{
    char dpad = getch();
    flushinp();
    switch(dpad)
    {
        case 'a':
            if (!coord_equals(CURRENTDIRECTION, RIGHT)) 
            {
                CURRENTDIRECTION = LEFT;
            }
            napms(SPEED);
            break;

        case 'd':
            if (!coord_equals(CURRENTDIRECTION, LEFT))
            {
                CURRENTDIRECTION = RIGHT;
            }
            napms(SPEED);
            break;

        case 'w':
            if (!coord_equals(CURRENTDIRECTION, DOWN))
            {
                CURRENTDIRECTION = UP;

            }
            napms(SPEED);
            break;

        case 's':
            if (!coord_equals(CURRENTDIRECTION, UP))
            {
                CURRENTDIRECTION = DOWN;
            }
            napms(SPEED);
            break;
    }
}

void headcrawl(void)
{   
    //1. Update position of the head
    HEAD->ncoord.y += CURRENTDIRECTION.y;
    HEAD->ncoord.x += CURRENTDIRECTION.x;
    screenwrap();
}

void bodycrawl(void)
{
    // 2. MAKE OTHER SEGMENTS FOLLOW
    segment *current = TAIL;
    while (current != NULL)
    {
        if (current->previous != NULL)
        {
            current->ncoord.x = current->previous->ncoord.x;
            current->ncoord.y = current->previous->ncoord.y;

        }
        current = current->previous;
    }
    free(current);
}

struct segment* create_segment(coord pos)
{
    struct segment* new_seg
        = (struct segment*)malloc(sizeof(struct segment));
    new_seg->ncoord = pos;
    new_seg->next = NULL;
    new_seg->previous = NULL;
    return new_seg;
}

void grow(coord start)
{
    struct segment* new_seg = create_segment(start);
    if (HEAD == NULL && TAIL == NULL)
    {
        HEAD = new_seg;
        TAIL = new_seg;
    }
    else if (HEAD == TAIL)
    {
        new_seg->previous = HEAD;               // is this even necessary?
        HEAD->next = new_seg;
        new_seg->ncoord.x = HEAD->ncoord.x;
        new_seg->ncoord.y = HEAD->ncoord.y;
        TAIL = new_seg;
        SCORE++;
    }
    else
    {
        new_seg->previous = TAIL;
        TAIL->next = new_seg;
        new_seg->ncoord.x = TAIL->ncoord.x;
        new_seg->ncoord.y = TAIL->ncoord.y;
        TAIL = new_seg;
        SCORE++;
    }
    //free(new_seg);
}

void drawall(void)
{
    // Clear board
    for (int i = 1; i < WIN_HEIGHT-1; i++)
    {
        for (int j = 1; j < WIN_WIDTH-1; j++)
        {
            mvprintw(i, j, " ");
        }
    }
    
    // Print body
    segment *current = HEAD->next;
    while (current != NULL)
    {
        mvprintw(current->ncoord.y, current->ncoord.x, "o");
        current = current->next;
    }
    free(current);
    
    // Print fruit
    mvprintw(FRUIT.y, FRUIT.x, "Q");

    // Print head
    mvprintw(HEAD->ncoord.y, HEAD->ncoord.x, "@");

    free(current);
}



bool check_failure(void)
{
    // Don't touch the wall
    // if (HEAD->ncoord.y == 0 || HEAD->ncoord.y == WIN_HEIGHT-1 || HEAD->ncoord.x == 0 || HEAD->ncoord.x == WIN_WIDTH-1)
    // {
    //     endwin();
    //     return FALSE;
    // }

    // Stop hitting yourself!
    segment *current = HEAD->next;
    while (current != NULL)
    {
        if (current->ncoord.x == HEAD->ncoord.x && current->ncoord.y == HEAD->ncoord.y)
        {
            return FALSE;
        }
        current = current->next;
    }
    free(current);
    return TRUE;
}


void place_fruit(void)
{
    FRUIT.x = 1 + (rand() % (WIN_WIDTH-2));
    FRUIT.y = 1 + (rand() % (WIN_HEIGHT-2));
}

bool coord_equals(struct coord a, struct coord b)
{
    if (a.x == b.x && a.y == b.y)
    {
        return TRUE;
    }
    return FALSE;
}

void screenwrap(void)
{
    if (HEAD->ncoord.x == WIN_WIDTH-1)
    {
        HEAD->ncoord.x = 1;
    }
    else if (HEAD->ncoord.x == 0)
    {
        HEAD->ncoord.x = WIN_WIDTH-2;
    }
    else if (HEAD->ncoord.y == 0)
    {
        HEAD->ncoord.y = WIN_HEIGHT-2;
    }
    else if (HEAD->ncoord.y == WIN_HEIGHT-1)
    {
        HEAD->ncoord.y = 1;
    }
}

void free_snake(void) {
    segment *current = HEAD;
    while (current != NULL) {
        segment *next = current->next;
        free(current);
        current = next;
    }
    HEAD = NULL;
    TAIL = NULL;
}
