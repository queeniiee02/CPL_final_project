/*This source code copyrighted by Lazy Foo' Productions (2004-2022)
and may not be redistributed without written permission.*/

#include <SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <string>
#include"draw_ellipse.h"
#include "ball.h"
#include "LTexture.h"
#include "dot.h"

const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;
SDL_Window* gWindow = NULL;
SDL_Renderer* gRenderer = NULL;
LTexture gDotTexture;

bool init();
bool loadMedia();
void close();
bool checkCollision( SDL_Rect a, SDL_Rect b );
SDL_Rect createRect(int x, int y, int w, int h);

int main( int argc, char* args[] ) {
	srand(time(0));
	Ball *balls = new Ball[10];
	for (int i=0; i<10; i++) {
		balls[i].init(SCREEN_WIDTH, SCREEN_HEIGHT, 40);
        balls[i].randomGenerator();
        balls[i].id = i;
    }
	if( !init() ) {
		printf( "Failed to initialize!\n" );
	}
	else {
		if( !loadMedia() ) {
			printf( "Failed to load media!\n" );
		}
		else {
			bool quit = false;
			SDL_Event e;
			Dot dot;
			SDL_Rect wall = createRect(0, SCREEN_HEIGHT-40, SCREEN_WIDTH, 40);
			while( !quit ) {
				while( SDL_PollEvent( &e ) != 0 ) {
					if( e.type == SDL_QUIT ) {
						quit = true;
					}
					dot.handleEvent( e );
				}
				dot.move( wall );
				SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
				SDL_RenderClear( gRenderer );
				SDL_SetRenderDrawColor( gRenderer, 0x00, 0x00, 0x00, 0xFF );
				SDL_RenderDrawRect( gRenderer, &wall );
				dot.render();
                sdl_ellipse(gRenderer, 300, 200, 50, 50);
				for (int i=0; i<10; i++) {
					sdl_ellipse(gRenderer, balls[i].x, balls[i].y, balls[i].r, balls[i].r);
					balls[i].motion();
				}
				SDL_RenderPresent( gRenderer );
			}
		}
	}
	close();
	return 0;
}

bool init() {
	bool success = true;
	if( SDL_Init( SDL_INIT_VIDEO ) < 0 ) {
		printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
		success = false;
	}
	else {
		if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ) ) {
			printf( "Warning: Linear texture filtering not enabled!" );
		}
		gWindow = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
		if( gWindow == NULL )
		{
			printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
			success = false;
		}
		else {
			gRenderer = SDL_CreateRenderer( gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );
			if( gRenderer == NULL ) {
				printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
				success = false;
			}
			else {
				SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
				int imgFlags = IMG_INIT_PNG;
				if( !( IMG_Init( imgFlags ) & imgFlags ) ) {
					printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
					success = false;
				}
			}
		}
	}
	return success;
}

SDL_Rect createRect(int x, int y, int w, int h) {
	SDL_Rect rect;
	rect.x = x; rect.y = y; rect.w = w; rect.h = h;
	return rect;
}
bool loadMedia() {
	bool success = true;
	if( !gDotTexture.loadFromFile( "./img/smile.bmp" ) ) {
		printf( "Failed to load dot texture!\n" );
		success = false;
	}
	return success;
}

void close() {
	gDotTexture.free();
	SDL_DestroyRenderer( gRenderer );
	SDL_DestroyWindow( gWindow );
	gWindow = NULL;
	gRenderer = NULL;
	IMG_Quit();
	SDL_Quit();
}

bool checkCollision( SDL_Rect a, SDL_Rect b ) {
    int leftA, leftB, rightA, rightB, topA, topB, bottomA, bottomB;
    leftA = a.x; rightA = a.x + a.w; topA = a.y; 
	bottomA = a.y + a.h; rightB = b.x + b.w;
    topB = b.y;
    bottomB = b.y + b.h;
    if( bottomA <= topB ) {
        return false;
    }
    if( topA >= bottomB ) {
        return false;
    }
    if( rightA <= leftB ) {
        return false;
    }
    if( leftA >= rightB ) {
        return false;
    }
    return true;
}

LTexture::LTexture() {
	mTexture = NULL;
	mWidth = 0;
	mHeight = 0;
}

LTexture::~LTexture() {
	free();
}

bool LTexture::loadFromFile( std::string path ) {
	free();
	SDL_Texture* newTexture = NULL;
	SDL_Surface* loadedSurface = IMG_Load( path.c_str() );
	if( loadedSurface == NULL ) {
		printf( "Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError() );
	}
	else {
		SDL_SetColorKey( loadedSurface, SDL_TRUE, SDL_MapRGB( loadedSurface->format, 0, 0xFF, 0xFF ) );
        newTexture = SDL_CreateTextureFromSurface( gRenderer, loadedSurface );
		if( newTexture == NULL ) {
			printf( "Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError() );
		}
		else {
			mWidth = loadedSurface->w;
			mHeight = loadedSurface->h;
		}
		SDL_FreeSurface( loadedSurface );
	}
	mTexture = newTexture;
	return mTexture != NULL;
}

#if defined(SDL_TTF_MAJOR_VERSION)
bool LTexture::loadFromRenderedText( std::string textureText, SDL_Color textColor ) {
	free();
	SDL_Surface* textSurface = TTF_RenderText_Solid( gFont, textureText.c_str(), textColor );
	if( textSurface != NULL ) {
        mTexture = SDL_CreateTextureFromSurface( gRenderer, textSurface );
		if( mTexture == NULL ) {
			printf( "Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError() );
		}
		else {
			//Get image dimensions
			mWidth = textSurface->w;
			mHeight = textSurface->h;
		}
		SDL_FreeSurface( textSurface );
	}
	else {
		printf( "Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError() );
	}
	return mTexture != NULL;
}
#endif

void LTexture::free() {
	if( mTexture != NULL ) {
		SDL_DestroyTexture( mTexture );
		mTexture = NULL;
		mWidth = 0;
		mHeight = 0;
	}
}
void LTexture::setColor( Uint8 red, Uint8 green, Uint8 blue ) {
	SDL_SetTextureColorMod( mTexture, red, green, blue );
}
void LTexture::setBlendMode( SDL_BlendMode blending ) {
	SDL_SetTextureBlendMode( mTexture, blending );
}
void LTexture::setAlpha( Uint8 alpha ) {
	SDL_SetTextureAlphaMod( mTexture, alpha );
}
void LTexture::render( int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip ) {
	SDL_Rect renderQuad = { x, y, mWidth, mHeight };
	if( clip != NULL ) {
		renderQuad.w = clip->w;
		renderQuad.h = clip->h;
	}
	SDL_RenderCopyEx( gRenderer, mTexture, clip, &renderQuad, angle, center, flip );
}
int LTexture::getWidth() {
	return mWidth;
}
int LTexture::getHeight() {
	return mHeight;
}
Dot::Dot() {
    mPosX = 400; mPosY = 600;
	mCollider.w = DOT_WIDTH;
	mCollider.h = DOT_HEIGHT;
    mVelX = 0; mVelY = 0;
}
void Dot::handleEvent( SDL_Event& e ) {
	if( e.type == SDL_KEYDOWN && e.key.repeat == 0 ) {
        switch( e.key.keysym.sym ) {
            //case SDLK_UP: mVelY -= DOT_VEL; break;
            //case SDLK_DOWN: mVelY += DOT_VEL; break;
            case SDLK_LEFT: mVelX -= DOT_VEL; break;
            case SDLK_RIGHT: mVelX += DOT_VEL; break;
        }
    }
    else if( e.type == SDL_KEYUP && e.key.repeat == 0 ) {
        switch( e.key.keysym.sym ) {
            //case SDLK_UP: mVelY += DOT_VEL; break;
            //case SDLK_DOWN: mVelY -= DOT_VEL; break;
            case SDLK_LEFT: mVelX += DOT_VEL; break;
            case SDLK_RIGHT: mVelX -= DOT_VEL; break;
        }
    }
}
void Dot::move( SDL_Rect& wall ) {
    mPosX += mVelX;
	mCollider.x = mPosX;
    if( ( mPosX < 0 ) || ( mPosX + DOT_WIDTH > SCREEN_WIDTH ) || checkCollision( mCollider, wall ) ) {
        mPosX -= mVelX;
		mCollider.x = mPosX;
    }
    //mPosY += mVelY;
	//mCollider.y = mPosY;
    //if( ( mPosY < 0 ) || ( mPosY + DOT_HEIGHT > SCREEN_HEIGHT ) || checkCollision( mCollider, wall ) ) {
    //    mPosY -= mVelY;
	//	mCollider.y = mPosY;
    //}
}
void Dot::render() {
	gDotTexture.render( mPosX, mPosY );
}
