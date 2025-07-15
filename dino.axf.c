#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "address_map_arm.h"

#define STANDARD_X     320
#define STANDARD_Y     240
#define TREX_X          20
#define TREX_WIDTH      20
#define TREX_HEIGHT     20
#define CACTUS_WIDTH     6
#define MAX_CACTI        3
#define GROUND_Y        (STANDARD_Y - 10)
#define INTEL_BLUE     0x0071C5
#define BUFFER0         0xC8000000
#define BUFFER1         0xC9000000

int screen_x, screen_y;
int res_offset, col_offset;

/* Protótipos */
int get_data_bits(int mode);
int resample_rgb(int num_bits, int color);
void video_text(int x, int y, char *p);
void video_box(int x1, int y1, int x2, int y2, short color);
void draw_background(int db);
void swap_buffers();

short int dino[20][20] = {
    {0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1},
    {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0},
    {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0},
    {1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {1, 1, 1, 1, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0},
    {1, 1, 1, 1, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {1, 1, 1, 1, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
};

short int cactus[14][20] = {
    {0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0},
    {0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0},
    {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0},
    {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
};

/* Double buffering */
void swap_buffers() {
    volatile int *pixel_ctrl_ptr = (int *)PIXEL_BUF_CTRL_BASE;
    if (*(pixel_ctrl_ptr + 1) == BUFFER0)
        *(pixel_ctrl_ptr + 1) = BUFFER1;
    else
        *(pixel_ctrl_ptr + 1) = BUFFER0;
    while ((*(pixel_ctrl_ptr + 3) & 0x01) != 0);  // espera troca do buffer
}

int get_data_bits(int mode) {
    switch (mode) {
        case 0x0:   return 1;
        case 0x7:   return 8;
        case 0x11:  return 8;
        case 0x12:  return 9;
        case 0x14:  return 16;
        case 0x17:  return 24;
        case 0x19:  return 30;
        case 0x31:  return 8;
        case 0x32:  return 12;
        case 0x33:  return 16;
        case 0x37:  return 32;
        case 0x39:  return 40;
        default:    return 16;
    }
}

int resample_rgb(int num_bits, int color) {
    if (num_bits == 8) {
        color = (((color >> 16) & 0xE0)
               | ((color >> 11) & 0x1C)
               | ((color >> 6 ) & 0x03));
        color = (color << 8) | color;
    } else {
        color = (((color >> 8) & 0xF800)
               | ((color >> 5) & 0x07E0)
               | ((color >> 3) & 0x001F));
    }
    return color;
}

void video_text(int x, int y, char *p) {
    int offset = (y << 7) + x;
    volatile char *buf = (char *)FPGA_CHAR_BASE;
    char c;
    while ((c = *p++) != '\0') {
        buf[offset++] = c;
    }
}

void video_box(int x1, int y1, int x2, int y2, short color) {
    volatile int *pixel_buf_ptr = (int *)(*(int *)(PIXEL_BUF_CTRL_BASE)); // BACK buffer
    int row, col;
    int x_factor = 1 << (res_offset + col_offset);
    int y_factor = 1 << res_offset;
    x1 /= x_factor;  x2 /= x_factor;
    y1 /= y_factor;  y2 /= y_factor;
    for (row = y1; row <= y2; row++) {
        for (col = x1; col <= x2; col++) {
            int ptr = (int)pixel_buf_ptr
                    + (row << (10 - res_offset - col_offset))
                    + (col << 1);
            *(short *)ptr = color;
        }
    }
}

/* Fundo azul, sol, nuvens e grama */
void draw_background(int db) {
    short sky = resample_rgb(db, 0x87CEEB);    // azul céu
    short sun = resample_rgb(db, 0xFFFF00);    // amarelo sol
    short cloud = resample_rgb(db, 0xFFFFFF);  // branco nuvem
    short grass = resample_rgb(db, 0x228B22);  // verde grama

    video_box(0, 0, STANDARD_X, GROUND_Y, sky);

    // Sol (blocos simples)
    video_box(20, 20, 40, 40, sun);
    video_box(25, 15, 35, 20, sun);
    video_box(15, 25, 20, 35, sun);
    video_box(40, 25, 45, 35, sun);

    // Nuvem 1
    video_box(100, 30, 120, 40, cloud);
    video_box(110, 25, 130, 35, cloud);
    video_box(120, 30, 140, 40, cloud);

    // Nuvem 2
    video_box(200, 50, 220, 60, cloud);
    video_box(210, 45, 230, 55, cloud);
    video_box(220, 50, 240, 60, cloud);

    // Grama (chão verde)
    video_box(0, GROUND_Y, STANDARD_X, GROUND_Y + 5, grass);
}

/* Sprites do T-rex */
void draw_trex(int y, int db) {
    short col = resample_rgb(db, 0xFFA500);  // cor laranja
    short bg_sky = resample_rgb(db, 0x87CEEB);
    int i, j;
   
    for (i = 0; i < 20; i++) {
        for (j = 0; j < 20; j++) {
            if (dino[i][j]) {
                video_box(TREX_X + i, j + y, TREX_X + i, j + y, col);
            }
            else {
            	video_box(TREX_X + i, j + y, TREX_X + i, j + y, bg_sky);
            }
        }        
    }    
}


void clear_trex(int y, int db) {
    short bg_sky = resample_rgb(db, 0x87CEEB);
    int i, j;
   
    for (i = 0; i < 20; i++) {
        for (j = 0; j < 20; j++) {
            	video_box(TREX_X + i, j + y, TREX_X + i, j + y, bg_sky);
           
        }        
    }
}
/* Sprites de cacto */
void draw_cactus(int x, int height, int db) {
    short green = resample_rgb(db, 0xFF00);
    short bg_sky = resample_rgb(db, 0x87CEEB);
    int i,j;
   
    for(i = 0; i<14; i++){
        for(j = 0; j<20; j++){
        if(cactus[i][j]){
          	video_box(x+i,j+GROUND_Y-height,x+i,j+GROUND_Y-height, green);
          }
          else{
	          video_box(x+i,j+GROUND_Y-height,x+i,j+GROUND_Y-height, bg_sky);
              }
        }
    }
}

void clear_cactus(int x, int height, int db) {
    short bg_sky = resample_rgb(db, 0x87CEEB);
    int i,j;
    for(i = 0; i<14; i++){
        for(j = 0; j<20; j++){
	          video_box(x+i,j+GROUND_Y-height,x+i,j+GROUND_Y-height, bg_sky);
        }
    }
}

/* Estado do jogo */
int trex_y;
bool jumping;
int jump_velocity;
int gravity = 1;
int cactus_x[MAX_CACTI];
int cactus_heights[MAX_CACTI];
int score;
int frame_count;
bool game_over;

void reset_game() {
    int i;
    trex_y        = GROUND_Y - TREX_HEIGHT;
    jumping       = false;
    jump_velocity = 0;
    frame_count   = 0;
    score         = 0;
    game_over     = false;
    for (i = 0; i < MAX_CACTI; i++) {
        cactus_x[i]       = screen_x + i * 80;
        cactus_heights[i] = 20;
    }
    // Limpar tela no back buffer antes do próximo frame
    int db = get_data_bits(*(int *)(RGB_RESAMPLER_BASE) & 0x3F);
    video_box(0, 0, STANDARD_X, STANDARD_Y, 0);
    for (i = 0; i < 60; i++) {
        video_text(0, i, "                                                                                ");
    }
}

bool check_collision(int cx, int ch) {
    return cx < TREX_X + TREX_WIDTH
        && cx + CACTUS_WIDTH > TREX_X
        && trex_y + TREX_HEIGHT >= GROUND_Y - ch;
}

int main(void) {
    int i, keys, speed, d;
    volatile int *video_resolution = (int *)(PIXEL_BUF_CTRL_BASE + 0x8);
    volatile int *rgb_status       = (int *)(RGB_RESAMPLER_BASE);
    volatile int *key_ptr          = (int *)(KEY_BASE);
    volatile int *pixel_ctrl_ptr   = (int *)PIXEL_BUF_CTRL_BASE;

    // Inicializa vídeo
    {
        int vr = *video_resolution;
        screen_x =  vr        & 0xFFFF;
        screen_y = (vr >> 16) & 0xFFFF;
        int db = get_data_bits(*rgb_status & 0x3F);
        res_offset = (screen_x == 160) ? 1 : 0;
        col_offset = (db == 8)        ? 1 : 0;

        // Inicializa buffer de back para BUFFER1
        *(pixel_ctrl_ptr + 1) = BUFFER1;
    }

    // Tela de boot
    {
        int db = get_data_bits(*rgb_status & 0x3F);
        short bg = resample_rgb(db, INTEL_BLUE);
        video_box(0, 0, STANDARD_X, STANDARD_Y, 0);
        video_text(35, 29, "Intel FPGA");
        video_text(32, 30, "Computer Systems");
        video_box(31*4, 28*4, 49*4-1, 32*4-1, bg);
        swap_buffers(); // mostra tela de boot
    }

    reset_game();

    {
        int db = get_data_bits(*rgb_status & 0x3F);
        draw_background(db);
        swap_buffers();  // mostra fundo
    }

    while (1) {
        frame_count++;
        speed = 4 + frame_count / 500;
        if (speed > 10) speed = 10;

        keys = *key_ptr & 0x1;  // só Key0

        if (game_over) {
            if (keys != 0) {
                reset_game();
                int db = get_data_bits(*rgb_status & 0x3F);
                draw_background(db);
            }
            swap_buffers();
            continue;
        }

        // Pulo
        if (keys != 0 && !jumping && trex_y == GROUND_Y - TREX_HEIGHT) {
            jumping = true;
            jump_velocity = -10;
        }

        // Limpa chão só (não limpa fundo)
        {
            int db = get_data_bits(*rgb_status & 0x3F);
            short grass = resample_rgb(db, 0x228B22);
            video_box(0, GROUND_Y, STANDARD_X, GROUND_Y + 5, grass);
        }

        // Atualiza T-Rex
        if (jumping) {
            clear_trex(trex_y, get_data_bits(*rgb_status & 0x3F));
            jump_velocity += gravity;
            trex_y += jump_velocity;
            if (trex_y >= GROUND_Y - TREX_HEIGHT) {
                trex_y = GROUND_Y - TREX_HEIGHT;
                jumping = false;
            }
        }
        draw_trex(trex_y, get_data_bits(*rgb_status & 0x3F));
        
        // Cactos
        for (i = 0; i < MAX_CACTI; i++) {
            int db = get_data_bits(*rgb_status & 0x3F);
            clear_cactus(cactus_x[i], cactus_heights[i], db);

            cactus_x[i] -= speed;
            if (cactus_x[i] < -CACTUS_WIDTH - 2) {
                cactus_x[i] = screen_x + i * 80;
                if (cactus_heights[i] > TREX_HEIGHT)
                    cactus_heights[i] = TREX_HEIGHT;
            }

            draw_cactus(cactus_x[i], cactus_heights[i], db);

            if (check_collision(cactus_x[i], cactus_heights[i])) {
                video_text(30, 10, "GAME OVER!");
                game_over = true;
                break;
            }
        }

        // Score
        {
            char buf[20];
            sprintf(buf, "Score: %2d", score/10);
            score++;
            video_text(2, 0, buf);
        }

        swap_buffers(); // troca buffer pra exibir frame desenhado
		swap_buffers(); // troca buffer pra exibir frame desenhado
        // Delay
        for (d = 0; d < 17000000; d++);
    }

    return 0;
}
