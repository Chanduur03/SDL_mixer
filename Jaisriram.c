#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define PLAYER_SIZE 50
#define OBSTACLE_WIDTH 50
#define OBSTACLE_HEIGHT 50
#define OBSTACLE_SPEED 5

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
SDL_Rect player;
SDL_Rect obstacle;
int score = 0;
bool running = true;

// Audio
Mix_Music *bgMusic = NULL;
Mix_Chunk *jumpSound = NULL;
Mix_Chunk *gameOverSound = NULL;

void init() {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);

    window = SDL_CreateWindow("Infinite Runner", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    // Load audio files
    bgMusic = Mix_LoadMUS("background.mp3");
    jumpSound = Mix_LoadWAV("jump.wav");
    gameOverSound = Mix_LoadWAV("gameover.wav");

    if (!bgMusic || !jumpSound || !gameOverSound) {
        printf("Failed to load sound! Error: %s\n", Mix_GetError());
        exit(1);
    }

    // Start background music
    Mix_PlayMusic(bgMusic, -1);

    // Initialize player position
    player.x = 100;
    player.y = SCREEN_HEIGHT - PLAYER_SIZE - 10;
    player.w = PLAYER_SIZE;
    player.h = PLAYER_SIZE;

    srand(time(NULL));
    obstacle.x = SCREEN_WIDTH;
    obstacle.y = SCREEN_HEIGHT - OBSTACLE_HEIGHT - 10;
    obstacle.w = OBSTACLE_WIDTH;
    obstacle.h = OBSTACLE_HEIGHT;
}

void handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            running = false;
        }
        if (event.type == SDL_KEYDOWN) {
            if (event.key.keysym.sym == SDLK_SPACE && player.y == SCREEN_HEIGHT - PLAYER_SIZE - 10) {
                player.y -= 150; // Jump height
                Mix_PlayChannel(-1, jumpSound, 0);
            }
        }
    }
}

void update() {
    // Gravity effect
    if (player.y < SCREEN_HEIGHT - PLAYER_SIZE - 10) {
        player.y += 5; // Fall speed
    }

    // Move obstacle
    obstacle.x -= OBSTACLE_SPEED;
    if (obstacle.x + OBSTACLE_WIDTH < 0) {
        obstacle.x = SCREEN_WIDTH;
        score++;
    }

    // Collision detection
    if (SDL_HasIntersection(&player, &obstacle)) {
        printf("Game Over! Score: %d\n", score);
        Mix_PlayChannel(-1, gameOverSound, 0);
        SDL_Delay(1000);
        running = false;
    }
}

void render() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // Draw player
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    SDL_RenderFillRect(renderer, &player);

    // Draw obstacle
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderFillRect(renderer, &obstacle);

    SDL_RenderPresent(renderer);
}

void cleanup() {
    Mix_FreeMusic(bgMusic);
    Mix_FreeChunk(jumpSound);
    Mix_FreeChunk(gameOverSound);

    Mix_CloseAudio();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

int main() {
    init();

    while (running) {
        handleEvents();
        update();
        render();
        SDL_Delay(16); // ~60 FPS
    }

    cleanup();
    return 0;
}
