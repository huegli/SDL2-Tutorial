// Using SDL, SDL_image, standard IO, and strings
#include <SDL.h>
#include <SDL_image.h>
#include <ctime>
#include <stdio.h>
#include <string>

// Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

//Analog joystick dead zone
const int JOYSTICK_DEAD_ZONE = 8000;

//Texture wrapper class
class LTexture
{
  public:
    //Initialize variables
    LTexture();

    //Deallocate memory
    ~LTexture();

    //Loads image at specified path
    bool loadFromFile( std::string path );

    //Deallocate texture
    void free();

    //Set color modulation
    void setColor( Uint8 red, Uint8 green, Uint8 blue );

    //Set blending
    void setBlendMode( SDL_BlendMode blending );

    //Set alpha modulation
    void setAlpha( Uint8 alpha );

    //Renders texture at given point
    void render( int x, int y, SDL_Rect* clip = NULL, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE );

    //Gets image dimensions
    int getWidth();
    int getHeight();

  private:
    //The actual hardware texture
    SDL_Texture* mTexture;

    //Image dimension
    int mWidth;
    int mHeight;
};

// The window we'll be rendering to
SDL_Window *gWindow = NULL;

// The window renderer
SDL_Renderer *gRenderer = NULL;

//Game Controller 1 handle
SDL_GameController* gGameController = NULL;

//Joystick handler with haptic
SDL_Joystick* gJoystick = NULL;
SDL_Haptic* gJoyHaptic = NULL;

//Walking animation
LTexture gArrowTexture;

LTexture::LTexture()
{
  //Initialize
  mTexture = NULL;
  mWidth = 0;
  mHeight = 0;
}

LTexture::~LTexture()
{
  //Deallocate
  free();
}

bool LTexture::loadFromFile( std::string path )
{
  //Get rid of preexisting texture
  free();

  //The final texture
  SDL_Texture* newTexture = NULL;

  //Lead image at specified path
  SDL_Surface* loadedSurface = IMG_Load( path.c_str() );
  if( loadedSurface == NULL )
  {
    printf( "Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError() );
  }
  else
  {
    //Color key image
    SDL_SetColorKey( loadedSurface, SDL_TRUE, SDL_MapRGB( loadedSurface->format, 0, 0xFF, 0xFF  ) );

    //Create texture from surface pixels
    newTexture = SDL_CreateTextureFromSurface( gRenderer, loadedSurface );
    if( newTexture == NULL )
    {
      printf("Unable to create texture from %s!  SDL Error: %s\n", path.c_str(), SDL_GetError() );
    }
    else
    {
      //Get image dimensions
      mWidth = loadedSurface->w;
      mHeight = loadedSurface->h;
    }

    //Ged rid of old loaded surface
    SDL_FreeSurface( loadedSurface );
  }

  //Return success
  mTexture = newTexture;
  return mTexture != NULL;
}

void LTexture::free()
{
  //Free texutre if it exists
  if( mTexture != NULL )
  {
    SDL_DestroyTexture( mTexture );
    mWidth = 0;
    mHeight = 0;
  }
}

void LTexture::setColor( Uint8 red, Uint8 green, Uint8 blue )
{
  //Modulate texture
  SDL_SetTextureColorMod( mTexture, red, green, blue );
}

void LTexture::setBlendMode( SDL_BlendMode blending )
{
  //Set blending function
  SDL_SetTextureBlendMode( mTexture, blending );
}

void LTexture::setAlpha( Uint8 alpha )
{
  //Modulate texture alpha
  SDL_SetTextureAlphaMod( mTexture, alpha );
}

void LTexture::render( int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip )
{
  //Set reendering space and render to screen
  SDL_Rect renderQuad = { x, y, mWidth, mHeight };

  //Set clip rendering dimensions
  if( clip != NULL )
  {
    renderQuad.w = clip->w;
    renderQuad.h = clip->h;
  }
  //Render to screen
  SDL_RenderCopyEx( gRenderer, mTexture, clip, &renderQuad, angle, center, flip );
}

int LTexture::getWidth()
{
  return mWidth;
}

int LTexture::getHeight()
{
  return mHeight;
}

// Starts up SDL and creates window
bool init();

// Loads media
bool loadMedia();

// Frees media and shuts down SDL
void close();


bool init() {
  // Initialization flag
  bool success = true;

  // Initialize SDL
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_HAPTIC | SDL_INIT_GAMECONTROLLER ) < 0) {
    printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
    success = false;
  } else {
    // Set texture filtering to linear
    if ( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ) )
    {
        printf( "Warning: Linear texture filtering not enabled!" );
    }

    //Check for Joysticks
    if(SDL_NumJoysticks() < 1)
    {
      printf("Warning: No joysticks connected!\n");
    }
    else
    {
      //Check if first joystick is game controller interface compatible
      if( !SDL_IsGameController ( 0 ) )
      {
          printf( "Warning: Joystick is not game controller inteface compatible! SDL Error: %s\n", SDL_GetError() );
      }
      else
      {
          //Open gamee controller and check if it supports rumble
          gGameController = SDL_GameControllerOpen ( 0 );
          if( !SDL_GameControllerHasRumble( gGameController ) )
          {
              printf( "Warning: Game controller does not have rumble! SDL Error: %s\n", SDL_GetError() );
          }
      }
      //Load joystick if game controller could not be loaded
      if( gGameController == NULL)
      {
          //Open first joystick
          gJoystick = SDL_JoystickOpen(0);
          if(gJoystick == NULL )
          {
              printf(" Warning: Unable to open game controller! SDL Error: %s\n",SDL_GetError());
          }
          else
          {
              //Check if joystick support haptic
              if( !SDL_JoystickIsHaptic( gJoystick ) )
              {
                  printf( "Warning: Game controller does not support haptics! SD Error: %s\n", SDL_GetError() );
              }
              else
              {
                  //Get joystick haptic device
                  gJoyHaptic = SDL_HapticOpenFromJoystick( gJoystick );
                  if( gJoyHaptic == NULL )
                  {
                      printf( "Warning: Unable to get joystick haptics! SDL Error: %s\n", SDL_GetError() );
                  }
                  else
                  {
                      //Initialize rumble
                      if( SDL_HapticRumbleInit( gJoyHaptic ) < 0 )
                      {
                          printf( "Warning: Unable to initialize haptic rumble! SDL Error: %s\n", SDL_GetError() );
                      }
                  }
              }
          }
      }
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
      gRenderer = SDL_CreateRenderer( gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );
      if( gRenderer == NULL )
      {
        printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
        success = false;
      }
      else {
          //Initialize renderer color
          SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );

          //Initialize PNG loading
          int imgFlags = IMG_INIT_PNG;
          if( !( IMG_Init( imgFlags ) & imgFlags ) )
          {
            printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
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

  //Load sprite sheet texture
  if( !gArrowTexture.loadFromFile( "Lesson_20/arrow.png"))
  {
    printf("Failed to load arrow texture!\n");
    success = false;
  }

  return success;
}

void close() {
  // Free loaded image
  gArrowTexture.free();

  //Close game controller or joystick with haptics
  if( gGameController != NULL )
  {
      SDL_GameControllerClose( gGameController );
  }
  if( gJoyHaptic != NULL )
  {
      SDL_HapticClose( gJoyHaptic );
  }
  if( gJoystick != NULL )
  {
      SDL_JoystickClose(gJoystick);
  }
  gGameController = NULL;
  gJoystick = NULL;
  gJoyHaptic = NULL;

  // Destroy window
  SDL_DestroyRenderer( gRenderer );
  SDL_DestroyWindow(gWindow);
  gWindow = NULL;
  gRenderer = NULL;

  // Quit SDL subsystems
  IMG_Quit();
  SDL_Quit();
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

      //Normalized direction
      int xDir = 0;
      int yDir = 0;

      //Flip type
      SDL_RendererFlip flipType = SDL_FLIP_NONE;

      // While application is running
      while (!quit) {
        // Handle events on queue
        while (SDL_PollEvent(&e) != 0) {
          // User requests quit
          if (e.type == SDL_QUIT) {
            quit = true;
          }
          else if ( e.type == SDL_JOYAXISMOTION )
          {
            //Motion on controller 0
            if(e.jaxis.which == 0)
            {
              //X axis Motion
              if(e.jaxis.axis == 0)
              {
                //Left of dead zone
                if(e.jaxis.value < -JOYSTICK_DEAD_ZONE)
                {
                  xDir = -1;
                }
                //Right of dead zone
                else if(e.jaxis.value > JOYSTICK_DEAD_ZONE)
                {
                  xDir = 1;
                }
                else
                {
                  xDir = 0;
                }
              }
              //Y axis Motion
              if(e.jaxis.axis == 1)
              {
                //Left of dead zone
                if(e.jaxis.value < -JOYSTICK_DEAD_ZONE)
                {
                  yDir = -1;
                }
                //Right of dead zone
                else if(e.jaxis.value > JOYSTICK_DEAD_ZONE)
                {
                  yDir = 1;
                }
                else
                {
                  yDir = 0;
                }
              }
            }
          }
          //Joystick button press
          else if( e.type == SDL_JOYBUTTONDOWN )
          {
              //Use game controller
              if( gGameController != NULL )
              {
                  //Play rumble at 75% for 500 milliseconds
                  if( SDL_GameControllerRumble( gGameController, 0xFFFF * 3 / 4, 0xFFFF * 3 / 4, 500 ) != 0)
                  {
                      printf( "Warning: Unable to play game controller rumble! %s\n", SDL_GetError() );
                  }
                  //Use haptics
                  else if( gJoyHaptic != NULL )
                  {
                      //Play rumble at 75% strength for 500 milliseconds
                      if( SDL_HapticRumblePlay( gJoyHaptic, 0.75, 500 ) != 0 )
                      {
                          printf( "Warning: Unable to play haptic rumble! %s\n", SDL_GetError() );
                      }
                  }
              }
          }
        }

        //Clear screen
        SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
        SDL_RenderClear( gRenderer );

        //Calculate angle
        double joystickAngle = atan2((double)yDir,(double)xDir) * (180.0/M_PI);

        //Correct angle
        if(xDir == 0 && yDir == 0)
        {
          joystickAngle = 0;
        }

        //Render joystick 7 way angle
        gArrowTexture.render( ( SCREEN_WIDTH - gArrowTexture.getWidth() ) / 2, ( SCREEN_HEIGHT - gArrowTexture.getHeight() ) / 2, NULL, joystickAngle );

        // Update screen
        SDL_RenderPresent( gRenderer );

      }
    }
  }

  // Free resources and close SDL
  close();

  return 0;
}
