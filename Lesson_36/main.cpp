// Using  SDL, SDL_image, standard IO, and strings
#include <SDL.h>
#include <SDL2/SDL_hints.h>
#include <SDL_image.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <sstream>

// Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

//Total windows
const int TOTAL_WINDOWS = 3;

// Starts up SDL and creates window
bool init();

// Frees media and shuts down SDL
void close();

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
  void move();

  // Shows the dot on the screen
  void render();

private:
  // The X and Y offsets of the dot
  int mPosX, mPosY;

  // The velocity of the dot
  int mVelX, mVelY;
};

// Window Class
class LWindow
{
  public:
    //Initialize internals
    LWindow();

    //Creates window
    bool init();

    //Creates renderer from internal 
    SDL_Renderer* createRenderer();

    //Handles window events
    void handleEvent( SDL_Event& e);

    //Focuses on window
    void focus();

    //Shows window contents
    void render();
    
    //Deallocates internals
    void free();

    //Window dimensions
    int getWidth();
    int getHeight();
    
    //Window focii
    bool hasMouseFocus();
    bool hasKeyboardFocus();
    bool isMinimized();
    bool isShown();

  private:
    //Window data
    SDL_Window* mWindow;
    SDL_Renderer* mRenderer;
    int mWindowID;
    
    //Window dimensions
    int mWidth;
    int mHeight;

    //Window focus
    bool mMouseFocus;
    bool mKeyboardFocus;
    bool mFullScreen;
    bool mMinimized;
    bool mShown;
};

//Our custom window
LWindow gWindows[ TOTAL_WINDOWS ];

// The window renderer
SDL_Renderer *gRenderer = NULL;

SDL_Color gTextColor = { 0, 0, 0, 0xFF };

// Scene textures
LTexture gSceneTexture;

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

LWindow::LWindow()
{
  //Initialize non-existant window
  mWindow = NULL;
  mMouseFocus = false;
  mKeyboardFocus = false;
  mFullScreen = false;
  mMinimized = false;
  mWidth = 0;
  mHeight = 0;
}

bool LWindow::init()
{
  //Create window
  mWindow = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE );
  if( mWindow != NULL ) {
    mMouseFocus = true;
    mKeyboardFocus = true;
    mWidth = SCREEN_WIDTH;
    mHeight = SCREEN_HEIGHT;

    //Create renderer for window
    mRenderer = SDL_CreateRenderer( mWindow, - 1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );
    if( mRenderer == NULL )
    {
      printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
      SDL_DestroyWindow( mWindow );
      mWindow = NULL;
    }
    else
    {
      //Initialize renderer color
      SDL_SetRenderDrawColor( mRenderer, 0xFF, 0xFF, 0xFF, 0xFF );

      //Grab window identifier
      mWindowID = SDL_GetWindowID( mWindow );

      //Flag as opened
      mShown = true;
    }
  } 
  else
  {
    printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
  }

  return mWindow != NULL && mRenderer != NULL;
}

SDL_Renderer* LWindow::createRenderer()
{
  return SDL_CreateRenderer( mWindow, - 1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );
}

void LWindow::handleEvent( SDL_Event& e )
{
  //If an event was detected for this window
  if( e.type == SDL_WINDOWEVENT && e.window.windowID == mWindowID )
  {
    //Caption update flag
    bool updateCaption = false;
    switch( e.window.event )
    {
      //Window appeared
      case SDL_WINDOWEVENT_SHOWN:
      mShown = true;
      break;

      //Window disappeared
      case SDL_WINDOWEVENT_HIDDEN:
      mShown = false;
      break;
        
      //Get new dimensions and repaint
      case SDL_WINDOWEVENT_SIZE_CHANGED:
      mWidth = e.window.data1;
      mHeight = e.window.data2;
      SDL_RenderPresent( mRenderer );
      break;


      //Repaint on exposure
      case SDL_WINDOWEVENT_EXPOSED:
      SDL_RenderPresent( mRenderer );
      break;

      //Mouse entered window
      case SDL_WINDOWEVENT_ENTER:
      mMouseFocus = true;
      updateCaption =  true;
      break;

      //Mouse left window
      case SDL_WINDOWEVENT_LEAVE:
      mMouseFocus = false;
      updateCaption = true;
      break;

      //Window has keyboard focus
      case SDL_WINDOWEVENT_FOCUS_GAINED:
      mKeyboardFocus = true;
      updateCaption = true;
      break;

      //Window lost keyboard focus
      case SDL_WINDOWEVENT_FOCUS_LOST:
      mKeyboardFocus = false;
      updateCaption = true;

      //Window minimized
      case SDL_WINDOWEVENT_MINIMIZED:
      mMinimized = true;
      break;

      //Window maximized
      case SDL_WINDOWEVENT_MAXIMIZED:
      mMinimized = false;
      break;
      
      //Window restored
      case SDL_WINDOWEVENT_RESTORED:
      mMinimized = false;
      break;

      //Hide on close
      case SDL_WINDOWEVENT_CLOSE:
      SDL_HideWindow( mWindow );
      break;
    }
    //Update window caption with new data
    if( updateCaption )
    {
      std::stringstream caption;

      caption << "SDL Tutorial - MouseFocus:" << ( ( mMouseFocus ) ? "On" : "Off" ) << " KeyboardFocus:" << ( ( mKeyboardFocus ) ? "On" : "Off" );
      SDL_SetWindowTitle( mWindow, caption.str().c_str() );
    }
  }
  //Enter exit full screen on return key
  else if( e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_RETURN )
  {
    if( mFullScreen )
    {
      SDL_SetWindowFullscreen( mWindow, SDL_FALSE );
    }
    else
    {
      SDL_SetWindowFullscreen( mWindow, SDL_TRUE );
      mFullScreen = true;
      mMinimized = false;
    }
  }
}

void LWindow::focus()
{
  //Restore window is needed
  if( !mShown )
  {
    SDL_ShowWindow( mWindow );
  }

  //Move window forward
  SDL_RaiseWindow( mWindow );
}

void LWindow::render()
{
  if( !mMinimized )
  {
    //Clear screen
    SDL_SetRenderDrawColor( mRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
    SDL_RenderClear( mRenderer );

    //Uspdate screen
    SDL_RenderPresent( mRenderer );
  }
}
void LWindow::free()
{
	if( mWindow != NULL )
	{
		SDL_DestroyWindow( mWindow );
	}

	mMouseFocus = false;
	mKeyboardFocus = false;
	mWidth = 0;
	mHeight = 0;
}

int LWindow::getWidth()
{
  return mWidth;
}

int LWindow::getHeight()
{
  return mHeight;
}

bool LWindow::hasMouseFocus()
{
  return mMouseFocus;
}

bool LWindow::hasKeyboardFocus()
{
  return mKeyboardFocus;
}

bool LWindow::isMinimized()
{
  return mMinimized;
}

bool LWindow::isShown()
{
	return mShown;
}

bool init() {
  // Initialization flag
  bool success = true;

  //Initialize SDL
  if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
  {
    printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
    success = false;
  }
  else
  {
    //Set texture filtering to linear
    if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ) )
    { 
      printf( "Warning: Linear texture filtering not enabled!" ); 
    }

    //CReate window
    if( !gWindows[ 0 ].init() )
    {
      printf( "Window 0 could not be created:\n" );
      success = false;
    }
  }

  return success;
}

void close() {

  // Destroy windows
  for( int i = 0; i < TOTAL_WINDOWS; ++i )
  {
    gWindows[ i ].free();
  }
  
  // Quit SDL subsystems
  IMG_Quit();
  SDL_Quit();
}

int main(int argc, char *args[])
{
  // Start up SDL and create window
  if (!init()) {
    printf("Failed to initialize!\n");
  } else {
    //Initialize the rest of the windows
    for( int i = 1; i < TOTAL_WINDOWS; ++i )
    {
      gWindows[ i ].init();
    }
    
    // Main loop flag
    bool quit = false;

    // Event handler
    SDL_Event e;

    // While application is running
    while (!quit) {
      // Handle events on queue
      while (SDL_PollEvent(&e) != 0) {
        // User requests quit
        if (e.type == SDL_QUIT) {
          quit = true;
        }

        //Handle window event
        for( int i = 0; i < TOTAL_WINDOWS; ++ i)
        {
          gWindows[ i ].handleEvent( e );
        }
      
        //Pull up window
        if( e.type == SDL_KEYDOWN )
        {
          switch( e.key.keysym.sym )
          {
            case SDLK_1:
            gWindows[ 0 ].focus();
            break;

            case SDLK_2:
            gWindows[ 1 ].focus();
            break;

            case SDLK_3:
            gWindows[ 2 ].focus();
            break;
          }
        }
      }
      //Update all windows
      for( int i = 0; i < TOTAL_WINDOWS; ++i )
      { 
        gWindows[ i ].render();
      }

      //Check all windows
      bool allWindowsClosed = true;
      for( int i = 0; i < TOTAL_WINDOWS; ++i )
      {
        if ( gWindows[ i ].isShown() )
        {
          allWindowsClosed = false;
          break;
        }
      }

      //Application clossed all windows
      if( allWindowsClosed )
      {
        quit = true;
      }
    }

  }

  // Free resources and close SDL
  close();

  return 0;
}
