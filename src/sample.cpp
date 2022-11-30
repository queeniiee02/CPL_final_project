#include "graphics.h"
#include <iostream>
#include <time.h>
#include <random>

std::mt19937 seeded_engine() {
    std::random_device r;
    std::seed_seq seed{r(), r(), r(), r(), r(), r(), r(), r()};
    return std::mt19937(seed);
}
struct C {
    // Hold RNG state as a member variable
    std::mt19937 eng = seeded_engine();
    
    int foo() {
        // use the member variable to generate random numbers in a member function.
        return std::uniform_int_distribution<>(1,10)(eng);
    }
};
class Ball {
public:
    Ball() {
        r = 50, dx = 5, dy= 1, x = 0, y = 0, v = 5;
        color = GREEN;
        ddy = 1;
    }
    Ball (int _r, int _x, int _y, int _v, int _color) {
       r = _r, x = _x, y = _y, v = _v, color = _color;
       dy = 1;
       ddy = 1;
    }
    void randomGenerator () {
        r = std::uniform_int_distribution<>(10,100)(eng);
        x = std::uniform_int_distribution<>(0, getmaxx())(eng);
        y = std::uniform_int_distribution<>(0, getmaxy())(eng);
        v = std::uniform_int_distribution<>(1, 30)(eng);
        dy = std::uniform_int_distribution<>(1, 3)(eng);
        color = std::uniform_int_distribution<>(0, 15)(eng);
        ddy = 1;
    }
    void motion () {
		dy += ddy;
		x += dx; 
        if (x < r) dx = v;
        else if (x >= getmaxx() - r) dx = -v;
        if (y < r && dy < 0) dy = -dy;
        else if (y >= getmaxy() - r && dy > 0) dy = - dy;
        else y += dy;
    }
    std::mt19937 eng = seeded_engine();

    int r, dx, dy, ddy, x, y, color, v;

};

int main(int argc, char*argv[]){
	//int x = 120, y = 120, dx = 5, dy = 1, ddy = 2, r = 100;
    srand(time(NULL));
    Ball b; // must be outside the while function
    Ball c(100, 0, 100, 3, YELLOW);
    Ball *balls = new Ball[10];
    for (int i=0; i<10; i++) {
        balls[i].randomGenerator();
    }
	initwindow(1440, 800);	//window size
	while (1){
		if(kbhit()) break;
		cleardevice();
		//setcolor(GREEN);
        for (int i=0; i<10; i++) {
            setfillstyle(SOLID_FILL, balls[i].color);
            fillellipse(balls[i].x, balls[i].y, balls[i].r, balls[i].r);
            balls[i].motion();
        }
		setfillstyle(SOLID_FILL, b.color);
		fillellipse(b.x, b.y, b.r, b.r);
        b.motion();
		setfillstyle(SOLID_FILL, c.color);
		fillellipse(c.x, c.y, c.r, c.r);
        c.motion();
		refresh();
		delay(25);
	}
    delete balls;
	closegraph();
}

