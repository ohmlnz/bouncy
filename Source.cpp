#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include <stdio.h>  
#include <stdlib.h>   
#include <time.h>      
#include <math.h>     
#include <iostream>

const float PI = 3.141592653589793f;

class Bouncy : public olc::PixelGameEngine
{
public:
	Bouncy()
	{
		sAppName = "Bouncy Ball";
	}

	bool OnUserCreate() override
	{
		// score declaration
		std::string sScore = "Score: " + std::to_string(score);

		// random angle declaration
		initialAngle = float(returnRandomNumber());

		// board declaration
		board.height = 40;
		board.width = 150;
		board.x = (ScreenWidth() / 2) - (board.width / 2);
		board.y = 550.0f;

		// ball declaration
		ball.height = 19;
		ball.width = 19;
		ball.x = ScreenWidth() / 2;
		ball.y = ScreenHeight() / 2;
		ball.sprite = new olc::Sprite("basket-ball.png");

		// bricks declaration
		float yPos = 10.0f;
		float xPos = 0.1f;
		int brickHeight = 30;
		int brickwidth = 78;
		int wLimit = (ScreenWidth() - 5) / brickwidth;

		for (int i = 0; i < MAX_BRICKS; i++)
		{
			// find way to initialize this at the struct level
			bricks[i].width = brickwidth;
			bricks[i].height = brickHeight;

			if ((i+1) % wLimit == 0)
			{
				yPos += float(brickHeight + 10);
				xPos = 0.1f;
				bricks[i].x = xPos * float(brickwidth + 10);
				bricks[i].y = yPos;
				FillRect(int(bricks[i].x), int(bricks[i].y), bricks[i].width, bricks[i].height, olc::RED);
				continue;
			}
			bricks[i].x = xPos * float(brickwidth + 10);
			bricks[i].y = yPos;
			FillRect(int(bricks[i].x), int(bricks[i].y), bricks[i].width, bricks[i].height, olc::RED);
			xPos++;
		}


		// board initialization
		FillRect(int(board.x), int(board.y), board.width, board.height, olc::WHITE);

		// ball initialization	
		SetPixelMode(olc::Pixel::MASK);
		DrawSprite(int(ball.x), int(ball.y), ball.sprite);
		SetPixelMode(olc::Pixel::NORMAL);

		// visual boundaries initialization
		DrawRect(2, 2, ScreenWidth() - 3, ScreenHeight() - 3, olc::WHITE);

		// intro text initialization
		DrawString((ScreenWidth() / 2) - 215, (ScreenHeight() / 2) + 100, "Press the spacebar to start", olc::WHITE, 2);

		// score initialization
		DrawString(ScreenWidth() - 80, ScreenHeight() - 15, sScore, olc::WHITE, 1);

		return true;
	}

	bool OnUserUpdate(float	fElapsedTime) override
	{
		std::string sScore = "Score: " + std::to_string(score);

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
						// reset game status
						lost = false;

						// reset score
						score = 0;

						// reset board
						board.x = (ScreenWidth() / 2) - (board.width / 2);
						board.y = 550;

						// reset ball
						ball.x = ScreenWidth() / 2;
						ball.y = ScreenHeight() / 2;

						// reset bricks
						for (int i = 0; i < MAX_BRICKS; i++)
						{
							bricks[i].isDisplayed = true;
						}
					}
					cosX = 500 * cos(initialAngle * (PI / 180));
					cosY = 500 * sin(initialAngle * (PI / 180));
					started = true;
				}
			}
		}

		// ball movement
		if (started) {
			ball.x += cosX * fElapsedTime;
			ball.y += cosY * fElapsedTime;

			Clear(olc::BLACK);
			SetPixelMode(olc::Pixel::MASK);
			DrawSprite(int(ball.x), int(ball.y), ball.sprite);
			SetPixelMode(olc::Pixel::NORMAL);

			FillRect(int(board.x), int(board.y), board.width, board.height, olc::WHITE);
			DrawRect(2, 2, ScreenWidth() - 5, ScreenHeight() - 5, olc::WHITE);

			// bricks instanciation
			for (int i = 0; i < MAX_BRICKS; i++)
			{
				if (bricks[i].isDisplayed)
				{
					FillRect(int(bricks[i].x), int(bricks[i].y), bricks[i].width, bricks[i].height, olc::RED);
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
				std::string finalScore = "You lost! Your score is: " + std::to_string(score);
				DrawString((ScreenWidth() / 2) - 215, (ScreenHeight() / 2) + 100, finalScore, olc::WHITE, 2);
			}
		}

		// update score
		DrawString(ScreenWidth() - 80, ScreenHeight() - 15, sScore, olc::WHITE, 1);
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
		float l1 = ball.x;
		float t1 = ball.y;
		float r1 = ball.x + ball.width;
		float b1 = ball.y + ball.height;

		float l2 = board.x;
		float t2 = board.y;
		float r2 = board.x + board.width;
		float b2 = board.y + board.height;

		bool boardCollision = true;
		bool wallCollision = true;
		bool brickCollision = true;

		// ball vs bricks
		for (int i = 0; i < MAX_BRICKS; i++)
		{
			float l3 = bricks[i].x;
			float t3 = bricks[i].y;
			float r3 = bricks[i].x + bricks[i].width;
			float b3 = bricks[i].y + bricks[i].height;

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
					score++;
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
	struct object {
		float x;
		float y;
		int height;
		int width;
		bool isDisplayed = true;
		olc::Sprite *sprite = nullptr;
	} ball, board, bricks[49];
	short int MAX_BRICKS = 49;
	float initialAngle = 90.0f;
	float cosX = 0.0f;
	float cosY = 0.0f;
	bool started = false;
	bool lost = false;
	int score = 0;
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