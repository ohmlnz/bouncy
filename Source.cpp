#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include <stdio.h>      /* NULL */
#include <stdlib.h>     /* srand, rand */
#include <time.h>      /* time */
#include <math.h>      /* cos, sin */
#include <iostream>

// TODO: restructure in a more object oriented approach
// TODO: refactor variables to optimize RAM usage
// TODO: use fElapsedTime to avoid CPU spikes / drops
// TODO: display text prior to starting the game
// TODO: update score
// TODO: add time

class Bouncy : public olc::PixelGameEngine
{
public:
	Bouncy()
	{
		sAppName = "Bouncy Ball";
	}

	bool OnUserCreate() override
	{
		// random angle declaration
		initialAngle = returnRandomNumber();

		// board declaration
		board.height = 40;
		board.width = 150;
		board.x = (ScreenWidth() / 2) - (board.width / 2);
		board.y = 550;

		// ball declaration
		ball.height = 20;
		ball.width = 20;
		ball.x = ScreenWidth() / 2;
		ball.y = ScreenHeight() / 2;

		// bricks declaration
		int yPos = 10;
		int xPos = 0;
		int brickHeight = 30;
		int brickwidth = 80;
		int wLimit = ScreenWidth() / brickwidth;

		for (int i = 0; i < MAX_BRICKS; i++)
		{
			// find way to initialize this at the struct level
			bricks[i].width = brickwidth;
			bricks[i].height = brickHeight;

			if (i != 0 && i % wLimit == 0)
			{
				yPos += (brickHeight + 10);
				xPos = 0;
			}
			bricks[i].x = xPos * (brickwidth + 10);
			bricks[i].y = yPos;
			FillRect(bricks[i].x, bricks[i].y, bricks[i].width, bricks[i].height, olc::RED);
			xPos++;
		}


		// visual boundaries initialization
		DrawRect(2, 2, ScreenWidth() - 5, ScreenHeight() - 5, olc::WHITE);

		// board instanciation
		FillRect(board.x, board.y, board.width, board.height, olc::WHITE);

		// ball instanciation	
		FillRect(ball.x, ball.y, ball.width, ball.height, olc::WHITE);

		return true;
	}

	bool OnUserUpdate(float	fElapsedTime) override
	{
		// board left and right controls
		if (IsFocused())
		{	
			if (started) {
				if (GetKey(olc::Key::RIGHT).bHeld && board.x < ScreenWidth() - (board.width + 5))
				{
					board.x += 2;
				}

				if (GetKey(olc::Key::LEFT).bHeld && board.x > 5)
				{
					board.x -= 2;
				}
			}

			if (GetKey(olc::Key::SPACE).bPressed)
			{
				// ball bouncing in random direction
				if (!started) {
					if (lost) {
						lost = false;

						// reset board
						board.x = (ScreenWidth() / 2) - (board.width / 2);
						board.y = 550;

						// reset ball
						ball.x = ScreenWidth() / 2;
						ball.y = ScreenHeight() / 2;
					}
					cosX = 5 * cos(initialAngle * (PI / 180));
					cosY = 5 * sin(initialAngle * (PI / 180));
					started = true;
				}
			}
		}

		// ball movement
		if (started) {
			ball.x += cosX / 2;
			ball.y += cosY / 2;
			Clear(olc::BLACK);
			FillRect(ball.x, ball.y, ball.width, ball.height, olc::WHITE);
			FillRect(board.x, board.y, board.width, board.height, olc::WHITE);
			DrawRect(2, 2, ScreenWidth() - 5, ScreenHeight() - 5, olc::WHITE);
			// bricks instanciation
			for (int i = 0; i < MAX_BRICKS; i++)
			{
				if (bricks[i].isDisplayed)
				{
					FillRect(bricks[i].x, bricks[i].y, bricks[i].width, bricks[i].height, olc::RED);
				}
			}
		}

		// collision handler
		if (isColliding())
		{
			if ((ball.x + ball.width) > ScreenWidth() || ball.x < 0)
			{
				cosX *= -1;
			}
			else {
				cosY *= -1;
			}

			// handle loss
			if ((ball.y + ball.height) >= ScreenHeight())
			{
				lost = true;
				started = false;
				// DrawString(10, 10, "Score: " + std::to_string(40), olc::WHITE);
			}
		}

		// Render score
		// DrawString(10, 10, "Score: " + std::to_string(40), olc::WHITE);

		return true;
	}

	int returnRandomNumber()
	{
		// TODO: find a better way to randomize than this crap
		/* initialize random seed: */
		srand(time(NULL));

		/* generate secret number between 0 and 360: */
		int randomNumber = rand() % 360 + 0;
		return randomNumber;
	}

	bool OnUserDestroy() override
	{	
		// called when the window is closed
		// unload resources
		// disconnect hardware
		return true;
	}

	bool isColliding()
	{
		int l1 = ball.x;
		int t1 = ball.y;
		int r1 = ball.x + ball.width;
		int b1 = ball.y + ball.height;

		int l2 = board.x;
		int t2 = board.y;
		int r2 = board.x + board.width;
		int b2 = board.y + board.height;

		bool boardCollision = true;
		bool wallCollision = true;
		bool brickCollision = true;

		// ball vs bricks
		for (int i = 0; i < MAX_BRICKS; i++)
		{
			int l3 = bricks[i].x;
			int t3 = bricks[i].y;
			int r3 = bricks[i].x + bricks[i].width;
			int b3 = bricks[i].y + bricks[i].height;

			if (b1 < t3 || t1 > b3 || r1 < l3 || l1 > r3)
			{
				brickCollision = false;
			}
			else
			{
				if (bricks[i].isDisplayed)
				{
					bricks[i].isDisplayed = false;
					brickCollision = true;
					break;
				}
			}
		}

		// ball vs board
		if (b1 < t2 || t1 > b2 || r1 < l2 || l1 > r2)
		{
			boardCollision = false;
		}

		// ball vs boundaries
		if (b1 < ScreenHeight() && t1 > 0 && r1 < ScreenWidth() && l1 > 0)
		{
			wallCollision = false;
		}

		if (boardCollision || wallCollision || brickCollision)
		{
			return true;
		}

		return false;
	 }

private:
	short int MAX_BRICKS = 50;
	struct object {
		double x;
		double y;
		double height;
		double width;
		bool isDisplayed = true;
	} ball, board, bricks[50];

	const double PI = 3.141592653589793;
	int initialAngle = 90;
	bool started = false;
	bool lost = false;
	double cosX = 0;
	double cosY = 0;
};

int main()
{	
	Bouncy demo;
	if (demo.Construct(800, 600, 1, 1))
	{
		demo.Start();
	}
	return 0;
}