#include "Player.h"

Player::Player()
{
	init();
}

void Player::init()
{
	audioManager.setlistener(VECTOR_ZERO, VECTOR_FORWARD);
	landSound = audioManager.loadSound("..\\res\\audio\\Land.wav");
	jumpSound = audioManager.loadSound("..\\res\\audio\\Jump.wav");
}

void Player::correctPosition()
{
	float x = cam.getPosition().x;
	float z = cam.getPosition().z;

	if (x > 36.5f)
		cam.setPosition(glm::vec3(36.49f, cam.getPosition().y, cam.getPosition().z));
	else if (x < -36.5f)
		cam.setPosition(glm::vec3(-36.49f, cam.getPosition().y, cam.getPosition().z));
	if(z > 33.5f)
		cam.setPosition(glm::vec3(cam.getPosition().x, cam.getPosition().y, 33.49f));
	else if (z < -36.0f)
		cam.setPosition(glm::vec3(cam.getPosition().x, cam.getPosition().y, -35.99f));
}

void Player::jump(float pwr) //pwr is the amount in vertical units that we want to jump
{
	if (grounded)
	{
		float j = glm::sqrt(pwr * -2 * GRAVITY.y); //standard jump formula
		velocity.y += j;
		
		grounded = false;
		audioManager.playSound(jumpSound);
	}
}

void Player::updatePlayer()
{	
	velocityUpdate(); //calculates velocity for the current frame
	cam.translate(velocity); //moves camera accordingly

	if (cam.getPosition().y < 2.0f) //we stop the player after it gets to a certain minimum height
	{
		velocity.y = 0.0f;
		cam.setPosition(glm::vec3(cam.getPosition().x, 2.0f, cam.getPosition().z)); //reset on the Y in case it compenetrates a little
		grounded = true;
		audioManager.playSound(landSound);
	}

	correctPosition();
}

void Player::velocityUpdate()
{
	if (grounded == true)
		return;

	float subjectiveGravity; //different gravity strength depending on the sign of the vertical speed

	subjectiveGravity = velocity.y > 0 ? -4.65f : -6.8f;
	velocity.y += subjectiveGravity * (fixedTime * fixedTime);
}
