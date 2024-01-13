#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <math.h>
#include <string.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>

#define win_width 800
#define win_height 600
#define snake_width 20

int game_state = 0;
int game_is_running = false;
SDL_Window *window;
SDL_Renderer *renderer;
int last_frame_time = 0;
int last_time;
int appear;
int level=0;

void setup();

struct food_object
{
    float x;
    float y;
    float width;
    float height;
} food;
typedef struct snake_measurments
{
    float x;
    float y;
    float width;
    float height;
    float vel_x;
    float vel_y;
} SM;
typedef struct snake_parts
{
    SM measurements;
    struct snake_parts *next;
} SNAKE;
typedef struct parts_list
{
    SNAKE *head_snake;
    SNAKE *tail;
    int nef;
} list;
list *CreerListe()
{
    list *Li;
    Li = (list *)malloc(sizeof(list));
    Li->tail = NULL;
    Li->nef = 0;
    return (Li);
}
list *li;
SNAKE *createPart()
{
    SNAKE *part;
    part = (SNAKE *)malloc((sizeof(SNAKE)));
    part->next = NULL;
    return (part);
}
SNAKE *RasLef3a, *TailLef3a;

SDL_Surface *ras_surf, *body_surf, *tail_surf;
SDL_Texture *ras_tex, *body_tex, *tail_tex;
int highscore;
FILE *pf;
int high_score(int high){
    
    pf=fopen("HighScore.txt","r+");
    int highscores[2];
    int i=0;
    
    
    while (!feof(pf))
    {   
        fscanf(pf,"%d\n",&highscores[i]);

        i++;
    }
    fclose(pf);
    int highscore;
    pf=fopen("HighScore.txt","w");
    for (int i = 0; i < 2; i++)
    {   //printf("\nsom : %d",level);
        //printf("\nhigh : %d",high);
        if (i==(level-1) && highscores[i]<high)
        {   //printf("\nlinef: %d",high);
            highscores[i]=high;

        }
        fprintf(pf,"%d\n",highscores[i]);
        
    }
    high=highscores[level-1];
    fclose(pf);
    pf=NULL;
    //printf("\nnew: %d",high);
    return high;
}
SDL_Surface *start_surf;
SDL_Texture *start_tex;
void startup(){
    SDL_RenderClear(renderer);
    start_surf = IMG_Load("startup.jpg");
    start_tex = SDL_CreateTextureFromSurface(renderer, start_surf);
    SDL_FreeSurface(start_surf);
    SDL_RenderCopy(renderer, start_tex, NULL, NULL);
    SDL_DestroyTexture(start_tex);
    SDL_RenderPresent(renderer);
   
   
}       
void tail_icon(float velx, float vely)
{
    if (velx > 0)
    {
        tail_surf = IMG_Load("Graphics\\tail_left.png");
    }
    else if (velx < 0)
    {
        tail_surf = IMG_Load("Graphics\\tail_right.png");
    }
    else if (vely > 0)
    {
        tail_surf = IMG_Load("Graphics\\tail_up.png");
    }
    else if (vely < 0)
    {
        tail_surf = IMG_Load("Graphics\\tail_down.png");
    }
    else
    {
        tail_surf = IMG_Load("Graphics\\tail_left.png");
    }
    tail_tex = SDL_CreateTextureFromSurface(renderer, tail_surf);
    SDL_FreeSurface(tail_surf);
}
void head_icon(float velx, float vely)
{
    if (velx < 0)
    {
        ras_surf = IMG_Load("Graphics\\head_left.png");
    }
    else if (velx > 0)
    {
        ras_surf = IMG_Load("Graphics\\head_right.png");
    }
    else if (vely < 0)
    {
        ras_surf = IMG_Load("Graphics\\head_up.png");
    }
    else if (vely > 0)
    {
        ras_surf = IMG_Load("Graphics\\head_down.png");
    }
    else
    {
        ras_surf = IMG_Load("Graphics\\head_right.png");
    }

    ras_tex = SDL_CreateTextureFromSurface(renderer, ras_surf);
    SDL_FreeSurface(ras_surf);
}

void body_icon(SNAKE *courant)
{
    if ((courant->next->measurements.vel_y < 0 && courant->measurements.vel_x < 0) || (courant->next->measurements.vel_x > 0 && courant->measurements.vel_y > 0))
    {
        body_surf = IMG_Load("Graphics\\body_topright.png");
        // printf("\ncourant.next.x: %f  courant.next.y: %f",courant->measurements.vel_x,courant->measurements.vel_y);
    }
    else if ((courant->next->measurements.vel_y < 0 && courant->measurements.vel_x > 0) || (courant->next->measurements.vel_x < 0 && courant->measurements.vel_y > 0))
    {
        body_surf = IMG_Load("Graphics\\body_topleft.png");
        // printf("\ncourant.next.x: %f  courant.next.y: %f",courant->measurements.vel_x,courant->measurements.vel_y);
    }
    else if ((courant->next->measurements.vel_y > 0 && courant->measurements.vel_x < 0) || (courant->next->measurements.vel_x > 0 && courant->measurements.vel_y < 0))
    {
        body_surf = IMG_Load("Graphics\\body_bottomright.png");
        // printf("\ncourant.next.x: %f  courant.next.y: %f",courant->measurements.vel_x,courant->measurements.vel_y);
    }
    else if ((courant->next->measurements.vel_y > 0 && courant->measurements.vel_x > 0) || (courant->next->measurements.vel_x < 0 && courant->measurements.vel_y < 0))
    {
        body_surf = IMG_Load("Graphics\\body_bottomleft.png");
        // printf("\ncourant.next.x: %f  courant.next.y: %f",courant->measurements.vel_x,courant->measurements.vel_y);
    }
    else
    {
        if (courant->measurements.vel_y != 0)
        {
            body_surf = IMG_Load("Graphics\\body_vertical.png");
        }
        else if (courant->measurements.vel_x != 0)
        {
            body_surf = IMG_Load("Graphics\\body_horizontal.png");
            // printf("\ncourant.next.x: %f  courant.next.y: %f",courant->measurements.vel_x,courant->measurements.vel_y);
        }
    }
    body_tex = SDL_CreateTextureFromSurface(renderer, body_surf);
    SDL_FreeSurface(body_surf);
}
SDL_Surface *bg_surf;
SDL_Texture *bg_tex;
void create_bg()
{   if (level==1)
{
    bg_surf = IMG_Load("obstacles.jpg");
    bg_tex = SDL_CreateTextureFromSurface(renderer, bg_surf);
    SDL_free(bg_surf);
}
else if (level==2)
{
    bg_surf = IMG_Load("obstacles2.jpg");
    bg_tex = SDL_CreateTextureFromSurface(renderer, bg_surf);
    SDL_free(bg_surf);
}

setup();
    
}
SDL_Surface *score_surf,*game_surf,*restart_surf;
SDL_Texture *score_tex,*game_tex,*restart_tex;
TTF_Font *font;
void Game_over()
{
    SDL_RenderClear(renderer);
    game_surf = IMG_Load("Game_Over.jpg");
    game_tex = SDL_CreateTextureFromSurface(renderer,game_surf);
    SDL_FreeSurface(game_surf);
    SDL_RenderCopy(renderer, game_tex, NULL, NULL);
    SDL_DestroyTexture(game_tex);
    if (SDL_GetTicks() - last_time > 480)
    {   
        last_time = SDL_GetTicks();

        if (appear == 0)
        {
            
            appear = 1;
            
        }
        else  
        {
            
            appear = 0;
        }
        
    }
    if (appear == 1)
    {
        
        restart_surf = IMG_Load("restart.png");
        restart_tex = SDL_CreateTextureFromSurface(renderer, restart_surf);
        SDL_FreeSurface(restart_surf);
        SDL_RenderCopy(renderer, restart_tex, NULL, NULL);
        SDL_DestroyTexture(restart_tex);
    }
    
    font = TTF_OpenFont("Retro Gaming.ttf", 13);
    
    
    char score[9];
    sprintf(score, "%d", highscore);
    SDL_Color color = {14, 11, 6};
    score_surf = TTF_RenderText_Solid(font, score, color);
    score_tex = SDL_CreateTextureFromSurface(renderer, score_surf);
    SDL_FreeSurface(score_surf);
    SDL_Rect score_rect = {494,
                           172,
                           78,
                           73};
    SDL_RenderCopy(renderer, score_tex, NULL, &score_rect);
    SDL_DestroyTexture(score_tex);
    SDL_RenderPresent(renderer);
    font=NULL;
}


void create_score()
{
    // printf("\n1");
    font = TTF_OpenFont("Retro Gaming.ttf", 50);
    char score[20];

    // printf("\n%d",li->nef);
    sprintf(score, "SCORE: %d", li->nef-2);
    // printf("\n2 %s",score);
    SDL_Color color = {255, 255, 255};
    score_surf = TTF_RenderText_Solid(font, score, color);
    score_tex = SDL_CreateTextureFromSurface(renderer, score_surf);
    SDL_FreeSurface(score_surf);
    SDL_Rect score_rect = {5,
                           0,
                           140,
                           70};
    SDL_RenderCopy(renderer, score_tex, NULL, &score_rect);
    SDL_DestroyTexture(score_tex);
    font=NULL;
}
int initialize_window(void)
{
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        printf("Error initializing SDL.\n");
        return false;
    }
    if (TTF_Init() != 0)
    {
        printf("Error initializing TTF.\n");
        return false;
    }
    window = SDL_CreateWindow(
        "snake",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        win_width,
        win_height,
        0);
    if (!window)
    {
        fprintf(stderr, "Error creating SDL Window.\n");
        return false;
    }
    Uint32 chit = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
    renderer = SDL_CreateRenderer(window, -1, chit);
    if (!renderer)
    {
        printf("Error creating SDL Renderer.\n");
        return false;
    }
    return true;
}
float vel_x = 0;
float vel_y = 0;
float pas = 60;
SDL_Surface *pause_surf;
SDL_Texture *pause_tex;
void pause()
{
    font = TTF_OpenFont("Retro Gaming.ttf", 50);
    char pause[] = "Pause";
    SDL_Color color = {255, 255, 255};
    
    pause_surf = TTF_RenderText_Solid(font, pause, color);
    pause_tex = SDL_CreateTextureFromSurface(renderer, pause_surf);
    SDL_FreeSurface(pause_surf);
    SDL_Rect pause_rect = {340,
                           200,
                           220,
                           150};
    SDL_RenderCopy(renderer, pause_tex, NULL, &pause_rect);
    SDL_DestroyTexture(pause_tex);
}
void process_input(void)
{
    SDL_Event event;
    SDL_PollEvent(&event);
    switch (event.type)
    {
    case SDL_QUIT:
        
        game_is_running = 0;
    case SDL_KEYDOWN:
    //*********when you are playing***************
        if (game_state == 0)
        {
            switch (event.key.keysym.scancode)
            {
            case SDL_SCANCODE_ESCAPE:
                game_state=3;
                startup();
                break;
            case SDL_SCANCODE_UP:
            case SDL_SCANCODE_W:
                if (vel_y <= 0)
                {
                    vel_y = -1;
                    vel_x = 0;
                }
                break;
            case SDL_SCANCODE_DOWN:
            case SDL_SCANCODE_S:
                if (vel_y >= 0)
                {
                    vel_y = 1;
                    vel_x = 0;
                }

                break;
            case SDL_SCANCODE_RIGHT:
            case SDL_SCANCODE_D:
                if (vel_x >= 0)
                {
                    vel_x = 1;
                    vel_y = 0;
                }

                break;
            case SDL_SCANCODE_LEFT:
            case SDL_SCANCODE_A:
                if (vel_x <= 0)
                {
                    vel_x = -1;
                    vel_y = 0;
                }
                break;
            case SDL_SCANCODE_PAGEUP:
                printf("cliked");
                if (pas <= 80)
                {
                    pas += 10;
                }
                break;
            case SDL_SCANCODE_PAGEDOWN:
                printf("cliked");
                if (pas >= 40)
                {
                    pas -= 10;
                }
                break;
            case SDL_SCANCODE_SPACE:
                game_state = 1;
                pause();
                SDL_RenderPresent(renderer);
            }
        }
    //***************When Game is on pause**************
        else if (game_state == 1)
        {
            switch (event.key.keysym.scancode)
            {
            case SDL_SCANCODE_SPACE:
                game_state = 0;
                break;

            case SDL_SCANCODE_ESCAPE:
                game_is_running = 0;
                break;
            }
        }
        //************When you lose******************
        else if (game_state == 2)
        {
            switch (event.key.keysym.scancode)
            {
            case SDL_SCANCODE_ESCAPE:
                game_is_running = 0;
                break;

            case SDL_SCANCODE_R:
                game_state = 0;
                break;
            case SDL_SCANCODE_RETURN:
                game_state=3;
                startup();
                break;
            }
        }
        //***********When you are in the Start Menu************
         else if (game_state == 3)
        {
            switch (event.key.keysym.scancode)
            {
            case SDL_SCANCODE_ESCAPE:
                game_is_running = 0;
                break;

            case SDL_SCANCODE_1:
                level = 1;
                game_state=0;
                create_bg();
            
                break;
            case SDL_SCANCODE_2:
                level = 2;
                game_state=0;
                create_bg();
                break;
            }
        }
    }
}
void obstacl_accident()
{
    float rasx = RasLef3a->measurements.x;
    float rasy = RasLef3a->measurements.y;

    // CADRE-------CADRE--------CADRE
    if (((rasx + snake_width >= 760 || rasx < 40) && (rasy < 320 || rasy > 400)) || rasy <= 40 || rasy + snake_width >= 560)
    {
        printf("\nras.x: %f ras.y: %f", RasLef3a->measurements.x, RasLef3a->measurements.y);
        game_is_running = false;
        printf("\n-----------GAME OVER----------\n");
        SDL_Delay(1000);
    }
    //*************************************
    //----LETTER T --LETTER T ----LETTER T
    else if (((rasx + snake_width >= 360 && rasx <= 480) && (rasy + snake_width >= 240 & rasy <= 280)) || (rasy + snake_width >= 280 & rasy <= 360) & (rasx + snake_width >= 400 & rasx <= 440))
    {
        printf("\nras.x: %f ras.y: %f", RasLef3a->measurements.x, RasLef3a->measurements.y);
        game_is_running = false;
        printf("\n-----------GAME OVER----------\n");
        SDL_Delay(1000);
    }
    //************************************
    else if (((rasy + snake_width > 360 & rasy < 480) & (rasx + snake_width > 520 & rasx < 560)) || (((rasy + snake_width > 440 & rasy < 480)) & (rasx + snake_width > 440 && rasx < 560)))
    {
        printf("\nras.x: %f ras.y: %f", RasLef3a->measurements.x, RasLef3a->measurements.y);
        game_is_running = false;
        printf("\n-----------GAME OVER----------\n");
        SDL_Delay(1000);
    }
}
int check_obstacles(float x, float y,int lev)
{ // printf("\nx: %f y: %f",x,y);
    if (lev==1)
    {
        if (((x + snake_width > 760 || x < 40) && (y < 320 || y + snake_width > 400)) || y < 40 || y + snake_width > 560)
    {
        //printf("\nx: %f y: %f", x, y);
        return 0;
    }
    // TTTT obstacle

    else if ((x + snake_width > 360 && x < 480) && (y >= 240 && y < 280) || ((y >= 280 && y < 360) && (x + snake_width > 400 && x < 440)))
    {
        //printf("\nx: %f y: %f", x, y);
        return 0;
    }
    else if (((y + snake_width > 360 & y < 480) & (x + snake_width > 520 & x < 560)) || (((y + snake_width > 440 & y < 480)) & (x + snake_width > 440 && x < 540)))
        return 0;
    
    }
    //*********this is level 2*******************
    else if (lev==2)
    {
        if (((x >= 760 || x < 40) && (y < 320 || y + snake_width > 400)) || y < 40 || y + snake_width > 560)
    {
        //printf("\nsnakex: %f snakey: %f", x, y);
        return 0;
    }
        else if (((x + snake_width > 120 && x < 280) && (y >= 120 && y < 160)) || ((y >= 120 && y < 240) && (x + snake_width > 120 && x < 160)))
    {
        //printf("\nx: %f y: %f", x, y);
        return 0;
    }
        else if (((x + snake_width > 80 && x < 240) && (y >= 280 && y < 320)) || ((y >= 200 && y < 300) && (x + snake_width > 200 && x < 240)))
    {
        //printf("\nx: %f y: %f", x, y);
        return 0;
    }
        else if (((x + snake_width > 400 && x < 680) && (y >= 240 && y < 280)) || ((y >= 160 && y < 320) && (x + snake_width > 640 && x < 680)))
    {
        //printf("\nx: %f y: %f", x, y);
        return 0;
    }
        else if (((x + snake_width > 360 && x < 600) && (y >= 360 && y < 400)) || ((y >= 320 && y < 520) && (x + snake_width > 400 && x < 440)))
    {
        //printf("\nx: %f y: %f", x, y);
        return 0;
    }
    }
    
    
        return 1;
    
}
void setup()
{
    vel_x = 0;
    vel_y = 0;
    
    
    RasLef3a = createPart();
    TailLef3a = createPart();
    li = CreerListe();

    RasLef3a->measurements.height = snake_width;

    RasLef3a->measurements.width = snake_width;

    TailLef3a->measurements.height = snake_width;

    TailLef3a->measurements.width = snake_width;

    RasLef3a->measurements.x = 160;
    RasLef3a->measurements.y = 100;
    RasLef3a->measurements.vel_x = 0;
    RasLef3a->measurements.vel_y = 0;
    RasLef3a->next = NULL;
    ras_surf = IMG_Load("Graphics\\head_right.png");
    ras_tex = SDL_CreateTextureFromSurface(renderer, ras_surf);
    SDL_FreeSurface(ras_surf);

    TailLef3a->measurements.x = 140;
    TailLef3a->measurements.y = 100;
    TailLef3a->measurements.vel_x = 0;
    TailLef3a->measurements.vel_y = 0;
    TailLef3a->next = RasLef3a;
    tail_surf = IMG_Load("Graphics\\tail_left.png");
    tail_tex = SDL_CreateTextureFromSurface(renderer, tail_surf);
    SDL_FreeSurface(tail_surf);

    li->head_snake = RasLef3a;
    // TailLef3a->next = li->tail;
    li->tail = TailLef3a;
    li->nef = 2;
    food.height = 20;
    food.width = 20;
    do
    {
        food.x = (rand() % win_width / 20) * 20;
        food.y = (rand() % win_height / 20) * 20;
        //printf("\nfood__level:%d",level);
    } while (check_obstacles(food.x, food.y,level) == 0 || (food.x==160 && food.y==100));
    //printf("\nfood.x: %f  food.y: %f",food.x, food.y);
}       

void generate_food()
{   
    if ((abs(food.x - RasLef3a->measurements.x) < food.width) && (abs(food.y - RasLef3a->measurements.y) < food.height))
    {   
        do
        {
            food.x = (rand() % win_width / 20) * 20;
            food.y = (rand() % win_height / 20) * 20;
        } while (check_obstacles(food.x,food.y,level) == 0);

        SNAKE *part;
        part = createPart();
        part->measurements.height = snake_width;
        part->measurements.width = snake_width;
        part->measurements.x = li->tail->measurements.x;
        part->measurements.y = li->tail->measurements.y;
        part->measurements.vel_x = 0;
        part->measurements.vel_y = 0;
        part->next = li->tail->next;
        li->tail->next = part;
        li->nef++;
        //printf("\nnef: %d", li->nef);
        //  assing_measurs_to_part(part);
    }
}
SNAKE *courant;
void update()
{


    
    courant = li->tail;
    if (TailLef3a->next->next == NULL && vel_x == 0 && vel_y == 0)
    {
    }
    else
    {
        TailLef3a->measurements.x = TailLef3a->next->measurements.x;
        TailLef3a->measurements.y = TailLef3a->next->measurements.y;
        TailLef3a->measurements.vel_x = TailLef3a->next->measurements.vel_x;
        TailLef3a->measurements.vel_y = TailLef3a->next->measurements.vel_y;
    }

    while (courant->next != NULL && li->nef > 2)
    { // printf("1111");
        courant->measurements.x = courant->next->measurements.x;
        // printf("courant.x: %f \n", courant->next->measurements.x);
        courant->measurements.y = courant->next->measurements.y;
        // printf("courant.y: %f \n", courant->next->measurements.y);
        courant->measurements.vel_x = courant->next->measurements.vel_x;
        // printf("courant.vel_x: %f \n", courant->next->measurements.vel_x);
        courant->measurements.vel_y = courant->next->measurements.vel_y;
        // printf("courant.ve_y: %f \n", courant->next->measurements.vel_y);
        courant = courant->next;
    }
    RasLef3a->measurements.vel_x = vel_x;
    // printf("\nvel_x: %f", RasLef3a->measurements.vel_x);
    RasLef3a->measurements.vel_y = vel_y;
    // printf("\nvel_y: %f", RasLef3a->measurements.vel_y);
    RasLef3a->measurements.x += RasLef3a->measurements.vel_x * 20;
    RasLef3a->measurements.y += RasLef3a->measurements.vel_y * 20;
    SDL_Delay(pas);
    courant=NULL;
    
    //  printf("\nx: %f", RasLef3a->measurements.x);
    //  printf("\ny: %f", RasLef3a->measurements.y);
}
SDL_Rect part_rect,tail_rect,Ras_rect,food_rect;
void render()
{

    // SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    
    SDL_RenderCopy(renderer, bg_tex, NULL, NULL);
    
    

    int pos = 0;
    if (li->nef > 2)
    {
        
        courant = li->tail->next;

        while (courant->next != NULL)
        { // printf("\npos: %d",pos);
            part_rect.x = (int)courant->measurements.x;
            part_rect.y = (int)courant->measurements.y;
            part_rect.w = (int)courant->measurements.width;
            part_rect.h = (int)courant->measurements.height;
            //SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
            // SDL_RenderFillRect(renderer, &part_rect);
            body_icon(courant);
            SDL_RenderCopy(renderer, body_tex, NULL, &part_rect);
            SDL_DestroyTexture(body_tex);
            courant = courant->next;
            pos++;
        }
        courant=NULL;
    }

    tail_icon(TailLef3a->next->measurements.vel_x, TailLef3a->next->measurements.vel_y);
    tail_rect.x = (int)TailLef3a->measurements.x;
    tail_rect.y = (int)TailLef3a->measurements.y;
    tail_rect.w = (int)TailLef3a->measurements.width;
    tail_rect.h = (int)TailLef3a->measurements.height;
    SDL_RenderCopy(renderer, tail_tex, NULL, &tail_rect);
    SDL_DestroyTexture(tail_tex);

    create_score();
    
    Ras_rect.x = (int)RasLef3a->measurements.x;
    Ras_rect.y = (int)RasLef3a->measurements.y;
    Ras_rect.w = (int)RasLef3a->measurements.width;
    Ras_rect.h = (int)RasLef3a->measurements.height;

    // SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
    // SDL_RenderFillRect(renderer, &Ras_rect);

    head_icon(RasLef3a->measurements.vel_x, RasLef3a->measurements.vel_y);
    SDL_RenderCopy(renderer, ras_tex, NULL, &Ras_rect);
    SDL_DestroyTexture(ras_tex);
    
    food_rect.x = (int)food.x;
            food_rect.y = (int)food.y;
            food_rect.w = (int)food.width;
            food_rect.h = (int)food.height;

    SDL_SetRenderDrawColor(renderer, 255, 100, 0, 255);
    SDL_RenderFillRect(renderer, &food_rect);
    
    SDL_RenderPresent(renderer);
}

void destroy_window(void)
{   SDL_DestroyTexture(ras_tex);
    SDL_DestroyTexture(tail_tex);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
SNAKE *parts;
void self_accident()
{

    int pos = 1;
    if (li->nef >= 5)
    {
        parts = li->tail->next;
        // printf("\n-----------------------begin--------------\n");
        while (parts->next != NULL && li->nef - pos >= 4)
        {
            float dist_x = abs(parts->measurements.x - RasLef3a->measurements.x);
            float dist_y = abs(parts->measurements.y - RasLef3a->measurements.y);

            float pytagor = sqrt(dist_x * dist_x + dist_y * dist_y);
            // printf("\nras.x: %f ras.y: %f\n",RasLef3a->measurements.x,RasLef3a->measurements.y);
            // printf("\nposition : %d courant.x: %f courant.y: %f  pytagor: %f", li->nef - pos,courante->measurements.x,courante->measurements.y ,pytagor);
            // printf("\n");
            if (pytagor < 10)
            {   setup();
                appear = 1;
                game_state = 2;
                printf("\n-----------GAME OVER----------\n");
                Game_over();

                break;
            }

            // printf("\n diff.x: %f", abs(courant->measurements.x - RasLef3a->measurements.x));
            // printf("\n diff.x: %f", abs(courant->measurements.y - RasLef3a->measurements.y));
            pos++;
            parts = parts->next;
        }
        parts=NULL;
        // printf("-----------------------eeeeeennnnndddd---------------");
    }
     
}
void freerun()
{
    if (RasLef3a->measurements.y+snake_width > 600)
        RasLef3a->measurements.y = 0;
    else if (RasLef3a->measurements.y < 0)
        RasLef3a->measurements.y = 600;
    else if (RasLef3a->measurements.x+snake_width > 800)
        RasLef3a->measurements.x = 0;
    else if (RasLef3a->measurements.x < 0)
        RasLef3a->measurements.x = 800;
}
    
int main(int argc, char *args[])
{
    game_state = 3;
    game_is_running = initialize_window();
    
    

    setup();
    startup();
    
    while (game_is_running)
    {
        
        process_input();
        //*****start the game******
        if (game_state==3)
        {  
            continue;
        }
        //****game in pause*****
        if (game_state == 1)
        {
            continue;
        }
        //****Game over******
        if (game_state == 2)
        {   
            Game_over();
            continue;
        }

        //printf("\n2");
        self_accident();
        update();
        //printf("\n3");
        if (check_obstacles(RasLef3a->measurements.x, RasLef3a->measurements.y,level) == 0)
        {
           
            highscore=li->nef-2;
            setup();
            last_time = SDL_GetTicks();
            
            appear = 1;
            game_state = 2;
            
            
            highscore=high_score(highscore);
            
            Game_over();
            // SDL_Delay(2000);
            continue;
            // goto begin;
        }
        
    
        render();
     
        freerun();

        generate_food();
    }

    destroy_window();

    return 0;
}
