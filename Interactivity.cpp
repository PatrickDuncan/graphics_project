#include "Interactivity.h"
#include <stdio.h>
#include <stdlib.h>

#ifdef __APPLE__
#  include <GLUT/glut.h>
#else
#  include <GL/freeglut.h>
#endif

#include <iostream>
#include <cmath>
using namespace std;

// Include project files
#include "Player.h"
#include "UserInterface.h"

bool enemyCollision = false;

int level = 1;						// the level the game is on
int lives = 3;
int playerBeenLength = 0;			// the level the game is on
int blocks[3] = {3, 5, 7};			// Number of blocks in a row for each level
int space = 0;

float theta = 40;					// The angle of rotation
// float array instead of point3D because you can't initialize a struct up here
// The first 3 paramters of gluLookAt
float eye[] = {Interactivity::getLength(), Interactivity::getLength() * 3, -12};
// The 4-6 paramters of gluLookAt	
float center[] = {Interactivity::getLength(), 0, Interactivity::getLength() * 2 + 8};

Structure::point3D playerBeen[50];

Enemy Interactivity::enemy[2];

int Interactivity::getLives() {		 // Get the game level
	return lives;
}

void Interactivity::setLives(int n) {	 // Set the number of lives
	lives += n;
}

// 0 - cannot move, 1 - can move
int Interactivity::getSpace() {		 // Get the space state (can move or restart)
	return space;
}

void Interactivity::setSpace(int n) {	 // Set the space state (can move or restart)
	space = n;
}

int Interactivity::getLevel() {	 	// Get the game level
	return level;
}

void Interactivity::incrLevel() {	 	// Get the game level
	if(level + 1 <= 3)
		level++;
	else{
		UserInterface::setGameOverState();
		UserInterface::setWinGameState();
	}
}

void Interactivity::setLevel(int n) {	// Set the game level
	level = n;
}

int Interactivity::getLength() {	// Get the number of blocks in a row
	return blocks[level - 1];
}

int Interactivity::getBeenTo() {	// Get the number of blocks the player has been to
	return playerBeenLength;
}

int Interactivity::getAmountOfBlocks() {	// Get the number of blocks
	int len = Interactivity::getLength();
	return (len * (len + 1)) / 2;
}

float Interactivity::getTheta() { 	// Get the horizontal angle of rotation
	return theta;
}

// Check to see if the player is on the same block as an enemy
void Interactivity::checkIntersections() {
	Structure::point3D enemy0 = Interactivity::enemy[0].getCoor();
	Structure::point3D enemy1 = Interactivity::enemy[1].getCoor();
	Structure::point3D player = Player::getCoor();
	// if the player and the enemy are on the same block
	// level 2 has 2 enemies and level 3 has 3 enemies
	if (level == 2 &&
	   ((player.x == enemy0.x && player.z == enemy0.z) && !enemyCollision)) {
	   		
	   		enemyCollision = true;
	   		Interactivity::setLives(-1);
			Player::setStopped(true);

			if(Interactivity::getLives() == 0)
				UserInterface::setGameOverState();
			else
				UserInterface::setLevelState();
			
	}
	else if (level == 3 &&
	    ((player.x == enemy0.x && player.z == enemy0.z) ||
		(player.x == enemy1.x && player.z == enemy1.z) && !enemyCollision)) {
		
		enemyCollision = true;
		Interactivity::setLives(-1);
		Player::setStopped(true);
		
		if(Interactivity::getLives() == 0)
			UserInterface::setGameOverState();
		else
			UserInterface::setLevelState();
			
	}
}

Structure::point3D Interactivity::getEye() { // Get first 3 paramters of gluLookAt
	Structure::point3D point;
	point.x = eye[0];
	point.y = eye[1];
	point.z = eye[2];
	return point;
}

Structure::point3D Interactivity::getCenter() { // Get 4-6 paramters of gluLookAt
	Structure::point3D point;
	point.x = center[0];
	point.y = center[1];
	point.z = center[2];
	return point;
}

Structure::point3D* Interactivity::getPlayerBeen() {
	return playerBeen;
}

// Move back the camera when the game progress through the levels
void cameraAdjust() {
	eye[0] = Interactivity::getLength();
	eye[1] = Interactivity::getLength() * 3;
	center[0] = Interactivity::getLength();
	center[2] = Interactivity::getLength() * 2 + 8;
}

void resetPlayerBeen() {	// Rest the playerBeen array
	for (int i = 0; i < playerBeenLength; i++) {
		playerBeen[i].x = 0;
		playerBeen[i].z = 0;
	}
}

void Interactivity::pushPosition(int x, int z) {
	Structure::point3D point;
	point.x = x;
	point.z = z;
	bool duplicate = false;
	// Check for duplicate spot
	for (int i = 0; i < playerBeenLength; ++i) {
		if (playerBeen[i].x == x
			&& playerBeen[i].z == z) {
			duplicate = true;
			UserInterface::decrScore(1);
			break;
		}
	}
	
	int len = Interactivity::getLength();
	// Check if its a not a duplicate spot and doesn't go past bounds
	if (!duplicate &&	// not a duplicate
		x/2 > -1 &&	// bounds
		x/2 > -1 &&	// bounds
		x/2 < len &&	// further edge
		z/2 < len &&	// further edge
		x != z + 2) {	// dont count inside jump out of bounds
		playerBeen[playerBeenLength] = point;
		++playerBeenLength;
		
		UserInterface::incrScore();
		int numOfBlocks = Interactivity::getAmountOfBlocks();
		if ((playerBeenLength == numOfBlocks)) {				//finished level

			if(Interactivity::getLevel() == 3){
				UserInterface::setWinGameState();
				UserInterface::setGameOverState();
			}

			else
				UserInterface::setFinishedLevelState();

		}
	}
}

void Interactivity::keyboard(unsigned char key, int x, int y) {
	
	switch (key) {
		case 'q':	// Quit the program
		case 'Q':
		case 27:
			exit(0);
			break;
		case 'w':	// Forward
		case 'W':
			Player::setRotation(270);
			break;
		case 'a':	// Left
		case 'A':
			Player::setRotation(0);
			break;
		case 's':	// Backwards
		case 'S':
			Player::setRotation(90);
			break;
		case 'd':	// Right
		case 'D':
			Player::setRotation(180);
			break;
		case 32:	// Space
			if(!UserInterface::getFinishedLevelState() &&
				!UserInterface::getLevelState() &&
				!Player::currentlyOffBlock() &&
				!UserInterface::getIntroState())
				space = 1;
			break;
		case 'R':
		case 'r':
			if (UserInterface::getGameOverState()) {	// If the game is in Game Over state
				if(UserInterface::getWinGameState()){	// If the player won only
					UserInterface::setWinGameState();	// Turns off Game Won state
					UserInterface::writeHighScore();	// Wrties the high score to the text file (if there is one)
					UserInterface::decrScore(UserInterface::getScore());	// Reset score to zero
					Interactivity::setLives(-Interactivity::getLives());	// Reset lives to 3
				}
				// If the player won or lost
				UserInterface::setGameOverState();		// Disable game over (to start game again)
				Interactivity::setLevel(1);				// Set level to 1
				Player::reset();						// Reset player position
				Interactivity::enemy[0].init(0);		// Reset Enemy 1
				Interactivity::enemy[1].init(1);		// Reset Enemy 2
				resetPlayerBeen();						// Reset player position
				playerBeenLength = 0;					// Reset block parameters
				Player::setStopped(false);				
				UserInterface::decrScore(UserInterface::getScore());	//Decrease the score to zero
				enemyCollision = false;			// Resets enemy interactions
				cameraAdjust();					// Reset camera
				Interactivity::setLives(3);		// Reset lives
				UserInterface::setTime();		// Reset in-game time
			}
			else if (UserInterface::getLevelState()) {	// If the game is in Level Lost state (but not in Game Over)
				UserInterface::setLevelState();			// Restarts the level
				Interactivity::setLevel(Interactivity::getLevel()); // Keeps game on same level (until Finished Level or Game Over) 
				UserInterface::decrScore(15);		//Decrease score by 15
				Player::reset();					// Reset player position
				Interactivity::enemy[0].init(0);	// Reset Enemy 1
				Interactivity::enemy[1].init(1);	// Reset Enemy 2
				resetPlayerBeen();					// Turns off Intro state
				playerBeenLength = 0;				// Reset block parameters
				Player::setStopped(false);			// Resets environment interactions
				enemyCollision = false;				// Resets enemy interactions
				UserInterface::setTime();	// Reset in-game time
			}
			else if (UserInterface::getFinishedLevelState()) {  // If the game is in Level Finished state
				Interactivity::incrLevel();						// Increase level by 1
				resetPlayerBeen();								// Reset player position
				playerBeenLength = 0;							// Reset block parameters
				Player::reset();								// Reset player position
				cameraAdjust();									// Adjust camera angle according to next level
				UserInterface::setFinishedLevelState();			// Turns off Finished Level state
				Interactivity::enemy[0].init(0);				// Reset Enemy 1
				Interactivity::enemy[1].init(1);				// Reset Enemy 2
				UserInterface::setTime();						// Reset in-game time
				UserInterface::setCalculatingScore(false);		// Stop calculating the end-of-level score
			}
			else if (UserInterface::getIntroState()) { // If the game is in Intro state 
				UserInterface::setIntroState();	//Turns off Intro state
			}
			break;
		case 't':	// Reset the camera
		case 'T':
			eye[0] = Interactivity::getLength();
			eye[1] = Interactivity::getLength() * 3;
			eye[2] = -12;
			center[0] = Interactivity::getLength();
			center[1] = 0;
			center[2] = Interactivity::getLength() * 2 + 8;
			theta = 40;
			break;
	}
}

void Interactivity::special(int key, int x, int y) {
	float change;
	switch (key) {
		case GLUT_KEY_LEFT:		// Rotate left
			if (theta < 75)
				theta += 5;
			break;
 		case GLUT_KEY_RIGHT:	// Rotate right
			if (theta > -55)
				theta -= 5;
 			break;
 		case GLUT_KEY_UP:		// Move camera upwards
			if (eye[1] < 30)
				eye[1] += 2;
			break;
 		case GLUT_KEY_DOWN:		// Move camera downwards
			if (eye[1] > 6)
				eye[1] -= 2;
 			break;
	}
}

void Interactivity::printInstructions() {
	cout << "-----------------------------------"  << endl;
	cout << "INSTRUCITONS: " << endl;
	cout << "KEY             ACTION" << endl;
	cout << "Q               Quit"   << endl;
	cout << "ESC"            << endl;
	cout << "LEFT            Rotate camera left" << endl;
	cout << "RIGHT           Rotate camera right" << endl;
	cout << "UP              Move camera upwards" << endl;
	cout << "DOWN            Move camera downwards" << endl;
	cout << "W               Turn the character forwards" << endl;
	cout << "A               Turn the character left" << endl;
	cout << "S               Turn the character backwards" << endl;
	cout << "D               Turn the character right" << endl;
	cout << "SPACE           Jump in current direction" << endl;
	cout << "T               Reset the camera" << endl;
	cout << "R               Press when prompted" << endl;
	cout << "MOUSE MOVEMENT  Move the camera around" << endl;
	cout << "-----------------------------------"  << endl;
}

GLubyte* Interactivity::loadPPM(char* file, int* width, int* height, int* maxi) {
    GLubyte* img;
    FILE *fd;
    int n, m;
    int  k, nm;
    char c;
    int i;
    char b[100];
    float s;
    int red, green, blue;
    
    /* first open file and check if it's an ASCII PPM (indicated by P3 at the start) */
    fd = fopen(file, "r");
    fscanf(fd,"%[^\n] ", b);
    if (b[0] != 'P' || b[1] != '3') {
        printf("%s is not a PPM file!\n",file);
        exit(0);
    }
    fscanf(fd, "%c", &c);
    
    /* next, skip past the comments - any line starting with #*/
    while (c == '#') {
        fscanf(fd, "%[^\n] ", b);
        fscanf(fd, "%c", &c);
    }
    ungetc(c, fd);
    
    /* now get the dimensions and maxi colour value from the image */
    fscanf(fd, "%d %d %d", &n, &m, &k);
        
    /* calculate number of pixels and allocate storage for this */
    nm = n * m;
    img = (GLubyte*)malloc(3 * sizeof(GLuint) * nm);
    s = 255.0 / k;
    
    /* for every pixel, grab the read green and blue values, storing them in the image data array */
    for(i=0; i<nm; i++) {
        fscanf(fd, "%d %d %d", &red, &green, &blue);
        img[3 * nm - 3 * i - 3] = red * s;
        img[3 * nm - 3 * i - 2] = green * s;
        img[3 * nm - 3 * i - 1] = blue * s;
    }
    
    /* finally, set the "return parameters" (width, height, maxi) and return the image array */
    *width = n;
    *height = m;
    *maxi = k;
    
    return img;
}