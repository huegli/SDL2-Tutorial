// Using SDL, SDL_image, standard IO, and strings
#include <SDL.h>
#include <SDL2/SDL_render.h>
#include <SDL_image.h>
#include <stdio.h>
#include <string>

// Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

// Starts up SDL and creates window
bool init();

// Loads media
bool loadMedia();

// Frees media and shuts down SDL
void close();

// Box collision detector
bool checkCollision(SDL_Rect a, SDL_Rect b);

// Texture wrapper class
class LTexture {
public:
  // Initialize variables
  LTexture();

  // Deallocate memory
  ~LTexture();

  // Loads image at specified path
  bool loadFromFile(std::string path);

  // Creates image from font string
#if defined(SDL_TTF_MAJOR_VERSION)
  bool loadFromRenderedText(std::string textureText, SDL_Color textColor);
#endif

  // Deallocate texture
  void free();

  // Set color modulation
  void setColor(Uint8 red, Uint8 green, Uint8 blue);

  // Set blending
  void setBlendMode(SDL_BlendMode blending);

  // Set alpha modulation
  void setAlpha(Uint8 alpha);

  // Renders texture at given point
  void render(int x, int y, SDL_Rect *clip = NULL, double angle = 0.0,
              SDL_Point *center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE);

  // Gets image dimensions
  int getWidth();
  int getHeight();

private:
  // The actual hardware texture
  SDL_Texture *mTexture;

  // Image dimension
  int mWidth;
  int mHeight;
};

// The application time based timer
class LTimer {
public:
  // Initializes variables
  LTimer();

  // The various clock actions
  void start();
  void stop();
  void pause();
  void unpause();

  // Gets the timer's time
  Uint32 getTicks();

  // Checks the status of the timer
  bool isStarted();
  bool isPaused();

private:
  // The clock time when the timer started
  Uint32 mStartTicks;

  // The ticks stored when the timer was paused
  Uint32 mPausedTicks;

  // The timer status
  bool mPaused;
  bool mStarted;
};

// The dot that will move around on the screen
class Dot {
public:
  // The dimension of the dot
  static const int DOT_WIDTH = 20;
  static const int DOT_HEIGHT = 20;

  // Maximum axis velocity of the dot
  static const int DOT_VEL = 10;

  // Initializes the variables
  Dot();

  // Takes key presses and adjusts the dot's velocity
  void handleEvent(SDL_Event &e);

  // Moves the dot
  void move(SDL_Rect &wall);

  // Shows the dot on the screen
  void render();

private:
  // The X and Y offsets of the dot
  int mPosX, mPosY;

  // The velocity of the dot
  int mVelX, mVelY;

  // Dot's collision  box
  SDL_Rect mCollider;
};

// The window we'll be rendering to
SDL_Window *gWindow = NULL;

// The window renderer
SDL_Renderer *gRenderer = NULL;

// Rendered texture
LTexture gDotTexture;

LTexture::LTexture() {
  // Initialize
  mTexture = NULL;
  mWidth = 0;
  mHeight = 0;
}

LTexture::~LTexture() {
  // Deallocate
  free();
}

bool LTexture::loadFromFile(std::string path) {
  // Get rid of preexisting texture
  free();

  // The final texture
  SDL_Texture *newTexture = NULL;

  // Lead image at specified path
  SDL_Surface *loadedSurface = IMG_Load(path.c_str());
  if (loadedSurface == NULL) {
    printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(),
           IMG_GetError());
  } else {
    // Color key image
    SDL_SetColorKey(loadedSurface, SDL_TRUE,
                    SDL_MapRGB(loadedSurface->format, 0, 0xFF, 0xFF));

    // Create texture from surface pixels
    newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
    if (newTexture == NULL) {
      printf("Unable to create texture from %s!  SDL Error: %s\n", path.c_str(),
             SDL_GetError());
    } else {
      // Get image dimensions
      mWidth = loadedSurface->w;
      mHeight = loadedSurface->h;
    }

    // Ged rid of old loaded surface
    SDL_FreeSurface(loadedSurface);
  }

  // Return success
  mTexture = newTexture;
  return mTexture != NULL;
}

#if defined(SDL_TTF_MAJOR_VERSION)
bool LTexture::loadFromRenderedText(std::string textureText,
                                    SDL_Color textColor) {
  // Get rid of preexisting texture
  free();

  // Render text surface
  SDL_Surface *textSurface =
      TTF_RenderText_Solid(gFont, textureText.c_str(), textColor);
  if (textSurface == NULL) {
    printf("Unable to render text surface! SDL_ttf Error: %s\n",
           TTF_GetError());
  } else {
    // Create texture from surface pixels
    mTexture = SDL_CreateTextureFromSurface(gRenderer, textSurface);
    if (mTexture == NULL) {
      printf("Unable to create texture from rendered text! SDL Error: %s\n",
             SDL_GetError());
    } else {
      // Get image dimensions
      mWidth = textSurface->w;
      mHeight = textSurface->h;
    }

    // Get rid of old surface
    SDL_FreeSurface(textSurface);
  }

  // Return success
  return mTexture != NULL;
}
#endif

void LTexture::free() {
  // Free texutre if it exists
  if (mTexture != NULL) {
    SDL_DestroyTexture(mTexture);
    mWidth = 0;
    mHeight = 0;
  }
}

void LTexture::setColor(Uint8 red, Uint8 green, Uint8 blue) {
  // Modulate texture
  SDL_SetTextureColorMod(mTexture, red, green, blue);
}

void LTexture::setBlendMode(SDL_BlendMode blending) {
  // Set blending function
  SDL_SetTextureBlendMode(mTexture, blending);
}

void LTexture::setAlpha(Uint8 alpha) {
  // Modulate texture alpha
  SDL_SetTextureAlphaMod(mTexture, alpha);
}

void LTexture::render(int x, int y, SDL_Rect *clip, double angle,
                      SDL_Point *center, SDL_RendererFlip flip) {
  // Set reendering space and render to screen
  SDL_Rect renderQuad = {x, y, mWidth, mHeight};

  // Set clip rendering dimensions
  if (clip != NULL) {
    renderQuad.w = clip->w;
    renderQuad.h = clip->h;
  }
  // Render to screen
  SDL_RenderCopyEx(gRenderer, mTexture, clip, &renderQuad, angle, center, flip);
}

int LTexture::getWidth() { return mWidth; }

int LTexture::getHeight() { return mHeight; }

LTimer::LTimer() {
  // Initialize the variables
  mStartTicks = 0;
  mPausedTicks = 0;

  mPaused = false;
  mStarted = false;
}

void LTimer::start() {
  // Start the timer
  mStarted = true;

  // Unpause the timer
  mPaused = false;

  // Get the current clock time
  mStartTicks = SDL_GetTicks();
  mPausedTicks = 0;
}

void LTimer::stop() {
  // Stop the timer
  mStarted = false;

  // Unpause the timer
  mPaused = false;

  // Clear tick variables
  mStartTicks = 0;
  mPausedTicks = 0;
}

void LTimer::pause() {
  // If the timer is running and isn't already paused
  if (mStarted && !mPaused) {
    // Unpause the timer
    mPaused = true;

    // Reset the starting ticks
    mPausedTicks = SDL_GetTicks() - mStartTicks;
    mStartTicks = 0;
  }
}

void LTimer::unpause() {
  // If the timer is running and paused
  if (mStarted && mPaused) {
    // Unpause the timer
    mPaused = false;

    // Reset the starting ticks
    mStartTicks = SDL_GetTicks() - mPausedTicks;

    // Reset the paused ticks
    mPausedTicks = 0;
  }
}

Uint32 LTimer::getTicks() {
  // The actual timer time
  Uint32 time = 0;

  // If the timer is running
  if (mStarted) {
    // If the timer is paused
    if (mPaused) {
      // Return the number of ticks when the timer was paused
      time = mPausedTicks;
    } else {
      // Return the current time minus the start time
      time = SDL_GetTicks() - mStartTicks;
    }
  }

  return time;
}

bool LTimer::isStarted() {
  // Timer is running and paused or unpaused
  return mStarted;
}

bool LTimer::isPaused() {
  // Timer is running and paused
  return mPaused && mStarted;
}

Dot::Dot() {
  // Initialize the offsets
  mPosX = 0;
  mPosY = 0;

  // Set collision box dimenension
  mCollider.w = DOT_WIDTH;
  mCollider.h = DOT_HEIGHT;

  // Initialize the velocity
  mVelX = 0;
  mVelY = 0;
}

void Dot::handleEvent(SDL_Event &e) {
  // If a key was pressed
  if (e.type == SDL_KEYDOWN && e.key.repeat == 0) {
    // Adjust the velocity
    switch (e.key.keysym.sym) {
    case SDLK_UP:
      mVelY -= DOT_VEL;
      break;
    case SDLK_DOWN:
      mVelY += DOT_VEL;
      break;
    case SDLK_LEFT:
      mVelX -= DOT_VEL;
      break;
    case SDLK_RIGHT:
      mVelX += DOT_VEL;
      break;
    }
  }
  // If a key was released
  else if (e.type == SDL_KEYUP && e.key.repeat == 0) {
    // Adjust the velocity
    switch (e.key.keysym.sym) {
    case SDLK_UP:
      mVelY += DOT_VEL;
      break;
    case SDLK_DOWN:
      mVelY -= DOT_VEL;
      break;
    case SDLK_LEFT:
      mVelX += DOT_VEL;
      break;
    case SDLK_RIGHT:
      mVelX -= DOT_VEL;
      break;
    }
  }
}

void Dot::move(SDL_Rect &wall) {
  // Move the dot left or right
  mPosX += mVelX;
  mCollider.x = mPosX;

  // If the dot collided or went too far to the left or right
  if ((mPosX < 0) || (mPosX + DOT_WIDTH > SCREEN_WIDTH) || checkCollision(mCollider, wall))
  {
    // Move back
    mPosX -= mVelX;
    mCollider.x = mPosX;
  }

  // Move the dot up or down
  mPosY += mVelY;
  mCollider.y = mPosY;

  // If the dot collided or went too far up or down
  if ( ( mPosY < 0) || (mPosY + DOT_HEIGHT > SCREEN_HEIGHT) || checkCollision(mCollider, wall))
  {
    // Move back
    mPosY -= mVelY;
    mCollider.y = mPosY;
  }
}

void Dot::render() {
  // Show the dot
  gDotTexture.render(mPosX, mPosY);
}

bool init() {
  // Initialization flag
  bool success = true;

  // Initialize SDL
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
    success = false;
  } else {
    // Set texture filtering to linear
    if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1")) {
      printf("Warning: Linear texture filtering not enabled!");
    }
    // Create window
    gWindow = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED,
                               SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH,
                               SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (gWindow == NULL) {
      printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
      success = false;
    } else {
      // create renderer for window
      gRenderer = SDL_CreateRenderer(
          gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
      if (gRenderer == NULL) {
        printf("Renderer could not be created! SDL Error: %s\n",
               SDL_GetError());
        success = false;
      } else {
        // Initialize renderer color
        SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

        // Initialize PNG loading
        int imgFlags = IMG_INIT_PNG;
        if (!(IMG_Init(imgFlags) & imgFlags)) {
          printf("SDL_image could not initialize! SDL_image Error: %s\n",
                 IMG_GetError());
          success = false;
        }
      }
    }
  }

  return success;
}

bool loadMedia() {
  // Loading success flag
  bool success = true;

  // Load dot texture
  if (!gDotTexture.loadFromFile("Lesson_26/dot.bmp")) {
    printf("Failed to load dot texture!\n");
    success = false;
  }

  return success;
}

void close() {
  // Free loaded image
  gDotTexture.free();

  // Destroy window
  SDL_DestroyRenderer(gRenderer);
  SDL_DestroyWindow(gWindow);
  gWindow = NULL;
  gRenderer = NULL;

  // Quit SDL subsystems
  IMG_Quit();
  SDL_Quit();
}

bool checkCollision( SDL_Rect a, SDL_Rect b )
{
  //The sides of the reectangles
  int leftA, leftB;
  int rightA, rightB;
  int topA, topB;
  int bottomA, bottomB;

  //Calculate the sides of rect A 
  leftA = a.x;
  rightA = a.x + a.w;
  topA = a.y;
  bottomA = a.y + a.h;

  //Calculate the sides of rect B 
  leftB = b.x;
  rightB = b.x + b.w;
  topB = b.y;
  bottomB = b.y + b.h;

  //If any of the sides from A are outside of B 
  if( bottomA < topB )
  {
    return false;
  }

  if( topA >= bottomB )
  {
    return false;
  }

  if( rightA <= leftB )
  {
    return false;
  }

  if( leftA >= rightB )
  {
    return false;
  }

  //If none of the sides from A are outside B 
  return true;
}

int main(int argc, char *args[]) {
  // Start up SDL and create window
  if (!init()) {
    printf("Failed to initialize!\n");
  } else {
    // Load media
    if (!loadMedia()) {
      printf("Failed to load media!\n");
    } else {
      // Main loop flag
      bool quit = false;

      // Event handler
      SDL_Event e;

      // The dot that will be moving around on the screen
      Dot dot;

      //Set the wall
      SDL_Rect wall;
      wall.x =  300;
      wall.y = 40;
      wall.w = 40;
      wall.h = 400;

      // While application is running
      while (!quit) {
        // Handle events on queue
        while (SDL_PollEvent(&e) != 0) {
          // User requests quit
          if (e.type == SDL_QUIT) {
            quit = true;
          }

          // Handle input for the dot
          dot.handleEvent(e);
        }

        // Move the dot
        dot.move( wall );

        // Clear screen
        SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
        SDL_RenderClear(gRenderer);

        // Render wall
        SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0xFF);
        SDL_RenderDrawRect( gRenderer, &wall );

        // Render objects
        dot.render();

        // Update screen
        SDL_RenderPresent(gRenderer);
      }
    }
  }

  // Free resources and close SDL
  close();

  return 0;
}
