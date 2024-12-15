#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <glut.h>
#include <iostream>
#include <irrKlang.h>

#define GLUT_KEY_ESCAPE 27
#define DEG2RAD(a) (a * 0.0174532925)

class Vector3f {
public:
	float x, y, z;

	Vector3f(float _x = 0.0f, float _y = 0.0f, float _z = 0.0f) {
		x = _x;
		y = _y;
		z = _z;
	}

	Vector3f operator+(Vector3f& v) {
		return Vector3f(x + v.x, y + v.y, z + v.z);
	}

	Vector3f operator-(Vector3f& v) {
		return Vector3f(x - v.x, y - v.y, z - v.z);
	}

	Vector3f operator*(float n) {
		return Vector3f(x * n, y * n, z * n);
	}

	Vector3f operator/(float n) {
		return Vector3f(x / n, y / n, z / n);
	}

	Vector3f unit() {
		return *this / sqrt(x * x + y * y + z * z);
	}

	Vector3f cross(Vector3f v) {
		return Vector3f(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
	}
};

class Camera {
public:
	Vector3f eye, center, up;

	Camera(float eyeX = 1.0f, float eyeY = 1.0f, float eyeZ = 1.0f, float centerX = 0.0f, float centerY = 0.0f, float centerZ = 0.0f, float upX = 0.0f, float upY = 1.0f, float upZ = 0.0f) {
		eye = Vector3f(eyeX, eyeY, eyeZ);
		center = Vector3f(centerX, centerY, centerZ);
		up = Vector3f(upX, upY, upZ);
	}

	void moveX(float d) {
		Vector3f right = up.cross(center - eye).unit();
		eye = eye + right * d;
		center = center + right * d;
	}

	void moveY(float d) {
		eye = eye + up.unit() * d;
		center = center + up.unit() * d;
	}

	void moveZ(float d) {
		Vector3f view = (center - eye).unit();
		eye = eye + view * d;
		center = center + view * d;
	}

	void rotateX(float a) {
		Vector3f view = (center - eye).unit();
		Vector3f right = up.cross(view).unit();
		view = view * cos(DEG2RAD(a)) + up * sin(DEG2RAD(a));
		up = view.cross(right);
		center = eye + view;
	}

	void rotateY(float a) {
		Vector3f view = (center - eye).unit();
		Vector3f right = up.cross(view).unit();
		view = view * cos(DEG2RAD(a)) + right * sin(DEG2RAD(a));
		right = view.cross(up);
		center = eye + view;
	}

	void look() {
		gluLookAt(
			eye.x, eye.y, eye.z,
			center.x, center.y, center.z,
			up.x, up.y, up.z
		);
	}
};

Camera camera;

const Vector3f TOP_VIEW_EYE(0.0f, 30.0f, 0.0f);   // Position above the scene
const Vector3f TOP_VIEW_CENTER(0.0f, 0.0f, 10.0f); // Looking down at the center
const Vector3f TOP_VIEW_UP(0.0f, 0.0f, -1.0f);    // Up is negative Z-axis

const Vector3f SIDE_VIEW_EYE(30.0f, 0.0f, 0.0f);   // Position to the side of the scene
const Vector3f SIDE_VIEW_CENTER(0.0f, 0.0f, 10.0f); // Looking at the center
const Vector3f SIDE_VIEW_UP(0.0f, 1.0f, 0.0f);     // Up is Y-axis

const Vector3f FRONT_VIEW_EYE(0.0f, 0.0f, 25.0f);  // Position in front of the scene
const Vector3f FRONT_VIEW_CENTER(0.0f, 0.0f, 0.0f); // Looking at the center
const Vector3f FRONT_VIEW_UP(0.0f, 1.0f, 0.0f);    // Up is Y-axis

irrklang::ISoundEngine* engine2 = irrklang::createIrrKlangDevice();

float TableRotation = 0.0;
float colorR = 1.0f, colorG = 0.0f, colorB = 0.0f; // Initial color

void drawWall(double thickness) {
	glPushMatrix();
	glColor3f(colorR, colorG, colorB);
	glTranslated(0.5, 0.5 * thickness, 0.5);
	glScaled(1.0, thickness, 1.0);
	glutSolidCube(1);
	glPopMatrix();
	glPushMatrix();
	glColor3f(colorR, colorB, colorG);
	glTranslated(0.5, thickness, 0.5);
	glutSolidCube(1);
	glPopMatrix();
}
void drawTableLeg(double thick, double len) {
	glPushMatrix();
	glTranslated(0, len / 2, 0);
	glScaled(thick, len, thick);
	glutSolidCube(1.0);
	glPopMatrix();
}
void drawJackPart() {
	glPushMatrix();
	glScaled(0.2, 0.2, 1.0);
	glutSolidSphere(1, 15, 15);
	glPopMatrix();
	glPushMatrix();
	glTranslated(0, 0, 1.2);
	glutSolidSphere(0.2, 15, 15);
	glTranslated(0, 0, -2.4);
	glutSolidSphere(0.2, 15, 15);
	glPopMatrix();
}
void drawJack() {
	glPushMatrix();
	drawJackPart();
	glRotated(90.0, 0, 1, 0);
	drawJackPart();
	glRotated(90.0, 1, 0, 0);
	drawJackPart();
	glPopMatrix();
}
void drawTable(double topWid, double topThick, double legThick, double legLen) {
	glPushAttrib(GL_LIGHTING_BIT | GL_CURRENT_BIT);
	glPushMatrix();
	glTranslatef(10, -1.2, 10);
	glScalef(4.0, 4.0, 4.0);
	glRotated(TableRotation, 0.0, 1.0, 0.0);
	glColor3f(1.0, 1.0, 0.0);
	glPushMatrix();
	glTranslated(0, legLen, 0);
	glScaled(topWid, topThick, topWid);
	glutSolidCube(1.0);
	glPopMatrix();

	double dist = 0.95 * topWid / 2.0 - legThick / 2.0;
	glPushMatrix();
	glTranslated(dist, 0, dist);
	drawTableLeg(legThick, legLen);
	glTranslated(0, 0, -2 * dist);
	drawTableLeg(legThick, legLen);
	glTranslated(-2 * dist, 0, 2 * dist);
	drawTableLeg(legThick, legLen);
	glTranslated(0, 0, -2 * dist);
	drawTableLeg(legThick, legLen);
	glPopMatrix();
	glPopMatrix();
	glPopAttrib();
}

void setupLights() {
	GLfloat ambient[] = { 0.7f, 0.7f, 0.7, 1.0f };
	GLfloat diffuse[] = { 0.6f, 0.6f, 0.6, 1.0f };
	GLfloat specular[] = { 1.0f, 1.0f, 1.0, 1.0f };
	GLfloat shininess[] = { 50 };
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, shininess);

	GLfloat lightIntensity[] = { 0.7f, 0.7f, 1, 1.0f };
	GLfloat lightPosition[] = { -7.0f, 6.0f, 3.0f, 0.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, lightIntensity);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightIntensity);
}
void setupCamera() {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, 640 / 480, 0.001, 100);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	camera.look();
}

float playerX = 0.0f; // Initial x-position of the player
float playerZ = 0.0f;
float rotationAngle = 0.0f;
float walkAngle = 0.0f; // Walking cycle for animation
bool isWalking = false; // Tracks if the player is currently walking
const float moveSpeed = 0.05f;
const float rotateSpeed = 0.3f; // Rotation sensitivity
int lastMouseX = 0;
float leftLegAngle = 0.0f;
float rightLegAngle = 0.0f;
bool legForward = true; // To alternate the swing direction
float legSwingSpeed = 1.0f; // Speed of the leg swing
bool isShoot = false;
float arrowSpeed = 0.1;
float arrowX = playerX;
float arrowZ = playerZ;


void updateLegs() {
	if (isWalking) {
		if (legForward) {
			leftLegAngle += legSwingSpeed;
			rightLegAngle -= legSwingSpeed;
			if (leftLegAngle >= 15.0f) { // Max swing angle
				legForward = false;
			}
		}
		else {
			leftLegAngle -= legSwingSpeed;
			rightLegAngle += legSwingSpeed;
			if (leftLegAngle <= -15.0f) { // Min swing angle
				legForward = true;
			}
		}
	}
	else {
		// Reset leg angles when not moving
		leftLegAngle = 0.0f;
		rightLegAngle = 0.0f;
	}
}



// 1. Draw the Head (1 primitive)
void drawHead() {
	glPushMatrix();
	glTranslatef(0.0f, 0.8f, 0.0f); // Position above the torso
	glutSolidSphere(0.3, 20, 20);   // Sphere for the head
	glPopMatrix();
}

// 2. Draw the Torso (1 primitive)
void drawTorso() {
	glPushMatrix();
	glScalef(0.5f, 1.0f, 0.3f); // Scale a cube to create a rectangular torso
	glutSolidCube(1.0);          // Cube for the torso
	glPopMatrix();
}

// 3. Draw the Left Arm (1 primitive)
void drawLeftArm() {
	glPushMatrix();
	glTranslatef(-0.3f, 0.1f, 0.2f); // Position to the left of the torso
	glRotatef(30.0, 0.0, 1.0, 0.0);
	glRotatef(-90.0, 1.0, 0.0, 0.0);
	glScalef(0.2f, 0.8f, 0.2f);      // Scale to make it look like an arm
	glutSolidCube(1.0);              // Cube for the left arm
	glPopMatrix();
}

// 4. Draw the Right Arm (1 primitive)
void drawRightArm() {
	glPushMatrix();
	glTranslatef(0.27f, 0.3f, 0.4f); // Position the right arm to the right of the torso
	glRotatef(-15, 0.0, 1.0, 0.0);
	glRotatef(-90.0f, 1.0f, 0.0f, 0.0f); // Rotate the arm to face forward
	glScalef(0.2f, 0.8f, 0.2f); // Scale to make it look like an arm
	glutSolidCube(1.0); // Cube for the right arm
	glPopMatrix();
}


// 5. Draw the Left Leg (1 primitive)
void drawLeftLeg() {
	glPushMatrix();
	glTranslatef(-0.2f, -0.75f, 0.0f); // Position below the torso on the left

	glScalef(0.2f, 0.8f, 0.2f);       // Scale to make it look like a leg
	glutSolidCube(1.0);               // Cube for the left leg
	glPopMatrix();
}

// 6. Draw the Right Leg (1 primitive)
void drawRightLeg() {
	glPushMatrix();
	glTranslatef(0.2f, -0.75f, 0.0f); // Position below the torso on the right
	glScalef(0.2f, 0.8f, 0.2f);       // Scale to make it look like a leg
	glutSolidCube(1.0);               // Cube for the right leg
	glPopMatrix();
}

// Draw Eyes
void drawEyes() {
	// Left Eye
	glPushMatrix();
	glTranslatef(-0.1f, 0.9f, 0.25f);  // Position the left eye
	glColor3f(0.0f, 0.0f, 0.0f);  // Black color for the eye
	glutSolidSphere(0.05, 20, 20);  // Draw a small sphere for the eye
	glPopMatrix();

	// Right Eye
	glPushMatrix();
	glTranslatef(0.1f, 0.9f, 0.25f);  // Position the right eye
	glColor3f(0.0f, 0.0f, 0.0f);  // Black color for the eye
	glutSolidSphere(0.05, 20, 20);  // Draw a small sphere for the eye
	glPopMatrix();


}

// Draw Mouth
void drawMouth() {
	glPushMatrix();
	glTranslatef(0.0f, 0.8f, 0.3f); // Position the mouth slightly below the nose

	glColor3f(0.0f, 0.0f, 0.0f); // Black color for the mouth

	// Draw a simple closed mouth as a line
	glBegin(GL_LINES);
	glVertex3f(-0.1f, 0.0f, 0.0f); // Left corner of the mouth
	glVertex3f(0.1f, 0.0f, 0.0f);  // Right corner of the mouth
	glEnd();

	glPopMatrix();


}

void drawRightHand() {
	glPushMatrix();
	glTranslatef(0.1f, 0.3f, 0.85f); // Position the hand at the end of the rotated arm
	glColor3f(0.9f, 0.7f, 0.5f); // Same skin color as the head
	glutSolidSphere(0.1, 20, 20); // Small sphere for the hand
	glPopMatrix();
}

void drawLeftHand() {
	glPushMatrix();
	glTranslatef(-0.05, 0.1, 0.6);
	glColor3f(0.9f, 0.7f, 0.5f); // Same skin color as the head
	glutSolidSphere(0.1, 20, 20); // Small sphere for the hand
	glPopMatrix();
}

// Function to draw the semi-circle part of the bow
void drawBowSemiCircle(float x) {
	if (x == 0.0) {
		glPushMatrix();
		glTranslatef(0.0f, -0.5f, 0.0f);

		glColor3f(0.5f, 0.3f, 0.1f); // Color for the semi-circle (wooden color)

		glBegin(GL_LINE_STRIP);
		int num_segments = 100; // Number of segments to approximate the semi-circle
		for (int i = 0; i <= num_segments; i++) {
			float theta = 3.14159f * float(i) / float(num_segments); // Half-circle (pi radians)
			float x = 1.0f * cos(theta); // X coordinate
			float y = 1.0f * sin(theta); // Y coordinate
			glVertex3f(x, y, 0.0f);      // Add a point to the semi-circle
		}
		glEnd();

		glPopMatrix();
	}
	else {
		float radius = 0.0f;
		if (x == 1.0) {
			radius = 0.85 * x;
		}
		else {
			radius = 1.9 * x;
		}
		glPushMatrix();
		glTranslatef(0.0f, -0.5f, 0.0f);

		glColor3f(0.5f, 0.3f, 0.1f); // Color for the semi-circle (wooden color)

		glBegin(GL_LINE_STRIP);
		int num_segments = 100; // Number of segments to approximate the semi-circle
		for (int i = 0; i <= num_segments; i++) {
			float theta = 3.14159f * float(i) / float(num_segments); // Half-circle (pi radians)
			float x = radius * cos(theta); // X coordinate scaled by the radius
			float y = radius * sin(theta); // Y coordinate scaled by the radius
			glVertex3f(x, y, 0.0f);        // Add a point to the semi-circle
		}
		glEnd();

		glPopMatrix();

	}
}

// Function to draw the bowstring as two lines (to allow interaction with the arrow)
void drawBowString() {
	glPushMatrix();
	glTranslatef(0.0f, -0.5f, 0.0f); // Position the string (align with the center of the bow)

	glColor3f(0.1f, 0.1f, 0.1f); // Color for the string (black or dark color)
	glLineWidth(2.0f); // Thicker line for the string

	// Left side of the bowstring
	glBegin(GL_LINES);
	glVertex3f(-1.0f, 0.0f, 0.0f); // Left side of the string
	glVertex3f(0.0f, 0.0f, 0.0f);  // Middle point (where the arrow attaches)
	glEnd();

	// Right side of the bowstring
	glBegin(GL_LINES);
	glVertex3f(1.0f, 0.0f, 0.0f);  // Right side of the string
	glVertex3f(0.0f, 0.0f, 0.0f);  // Middle point (where the arrow attaches)
	glEnd();

	glPopMatrix();
}



void drawCurvedString(float x) {
	glPushMatrix();
	glTranslatef(0.0f, -0.5f, 0.0f); // Position the string (align with the center of the bow)

	glColor3f(0.1f, 0.1f, 0.1f); // Color for the string (black or dark color)
	glLineWidth(2.0f); // Thicker line for the string

	// Left side of the bowstring
	glPushMatrix();
	glTranslatef(0.0f, 0.5 * x, 0.0f);
	glRotatef(30 * x, 0.0f, 0.0f, 1.0f);
	glBegin(GL_LINES);
	glVertex3f(-1.0f, 0.0f, 0.0f); // Left side of the string
	glVertex3f(0.0f, 0.0f, 0.0f);  // Middle point (where the arrow attaches)
	glEnd();
	glPopMatrix();

	// Right side of the bowstring
	glPushMatrix();
	glTranslatef(0.0f, 0.5 * x, 0.0f);
	glRotatef(-30 * x, 0.0f, 0.0f, 1.0f);
	glBegin(GL_LINES);
	glVertex3f(1.0f, 0.0f, 0.0f);  // Right side of the string
	glVertex3f(0.0f, 0.0f, 0.0f);  // Middle point (where the arrow attaches)
	glEnd();
	glPopMatrix();

	glPopMatrix();

}

// Function to draw the bow
void drawBow(float x) {
	glPushAttrib(GL_LIGHTING_BIT | GL_CURRENT_BIT);
	glPushMatrix();
	glTranslated(0.1, 1.4, 1.4);
	glRotated(90, 0.0, 1.0, 0.0);
	glRotated(90, 0.0, 0.0, 1.0);
	drawBowSemiCircle(x); // Draw the semi-circle of the bow
	if (x == 0.0) {
		drawBowString();  // Draw the string of the bow (two lines)
	}
	else {
		drawCurvedString(x);
	}
	glPopAttrib();
	glPopMatrix();
}

void drawArrow() {
	glPushMatrix();
	glTranslatef(0.01f, 1.5f, 1.0f); // Position the arrow
	glScaled(0.7, 0.7, 0.5);
	glRotatef(-90, 0, 1, 0);
	glRotated(90, 0, 0, 1);
	glRotated(90, 1.0, 0, 0);

	// Shaft of the arrow - Cylinder
	glColor3f(0.8f, 0.8f, 0.8f); // Light gray color
	GLUquadric* quad = gluNewQuadric();
	gluCylinder(quad, 0.05, 0.05, 2.0, 20, 5); // Arrow shaft

	// Arrowhead - Cone
	glPushMatrix();
	glTranslatef(0.0f, 0.0f, 2.0f); // Position at end of shaft
	glColor3f(1.0f, 0.0f, 0.0f); // Red color for the arrowhead
	glutSolidCone(0.1, 0.3, 20, 10); // Arrowhead
	glPopMatrix();

	// Fletchings (feathers) at the back of the arrow
	glColor3f(0.7f, 0.7f, 0.7f); // Gray color for fletchings

	// Right fletching
	glPushMatrix();
	glTranslatef(0.1f, 0.0f, -0.2f); // Position at back of the shaft
	glRotatef(30, 0.0f, 1.0f, 0.0f); // Rotate fletching
	glutSolidCone(0.05, 0.2, 10, 5); // Right fletching
	glPopMatrix();

	// Left fletching
	glPushMatrix();
	glTranslatef(-0.1f, 0.0f, -0.2f); // Position at back of the shaft
	glRotatef(-30, 0.0f, 1.0f, 0.0f); // Rotate fletching
	glutSolidCone(0.05, 0.2, 10, 5); // Left fletching
	glPopMatrix();

	// Top fletching
	glPushMatrix();
	glTranslatef(0.0f, 0.1f, -0.2f); // Position at back of the shaft
	glRotatef(90, 1.0f, 0.0f, 0.0f); // Rotate top fletching
	glutSolidCone(0.05, 0.2, 10, 5); // Top fletching
	glPopMatrix();

	gluDeleteQuadric(quad);
	glPopMatrix();
}


float tempAngle = 0.0;

void drawPlayer(float x, float y, float z) {
	// Save the current lighting and color states
	glPushAttrib(GL_LIGHTING_BIT | GL_CURRENT_BIT);
	glPushMatrix();
	glTranslatef(playerX, 0.0f, playerZ);
	glRotatef(rotationAngle, 0, 1, 0);

	// Enable GL_COLOR_MATERIAL to use glColor calls for each part
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	glPushMatrix();
	glTranslatef(x, y, z); // Position the player at (x, y, z)
	glScaled(2.0, 2.0, 2.0);
	// Set specific colors for each part to avoid unintended color issues

	// Draw Head
	glColor3f(0.9f, 0.7f, 0.5f); // Skin color for the head
	drawHead();

	// Draw Torso
	glColor3f(0.2f, 0.6f, 1.0f); // Shirt color for the torso
	drawTorso();

	// Draw Left Arm
	glColor3f(0.2f, 0.6f, 1.0f); // Same shirt color for the left arm
	drawLeftArm();
	glColor3f(0.9f, 0.7f, 0.5f); // Skin color for the hand
	drawLeftHand();

	// Draw Right Arm
	glColor3f(0.2f, 0.6f, 1.0f); // Same shirt color for the right arm
	drawRightArm();
	glColor3f(0.9f, 0.7f, 0.5f); // Skin color for the hand
	drawRightHand();

	// Draw Left Leg
	glColor3f(0.5f, 0.35f, 0.05f); // Pants color for the left leg
	glPushMatrix();
	glRotatef(leftLegAngle, 1, 0, 0); // Apply the swing rotation
	drawLeftLeg(); // Your function to draw a leg
	glPopMatrix();


	// Draw Right Leg
	glColor3f(0.5f, 0.35f, 0.05f); // Pants color for the right leg
	glPushMatrix();
	glRotatef(rightLegAngle, 1, 0, 0); // Apply the swing rotation
	drawRightLeg(); // Your function to draw a leg
	glPopMatrix();

	// Draw Eyes
	glColor3f(0.0f, 0.0f, 0.0f); // Black color for eyes
	drawEyes();

	// Draw Mouth
	glColor3f(0.0f, 0.0f, 0.0f); // Black color for mouth
	drawMouth();

	glPopMatrix();
	drawBow(0.5f);
	glPopMatrix();
	if (!isShoot) {
		glPushMatrix();
		glTranslatef(playerX, 0.0f, playerZ);
		glRotatef(rotationAngle, 0, 1, 0);
		drawArrow();
		glPopMatrix();
		tempAngle = rotationAngle;
		arrowX = playerX;
		arrowZ = playerZ;
	}
	else {
		glPushMatrix();

		glTranslatef(arrowX, 0.0, arrowZ);
		glRotatef(tempAngle, 0, 1, 0);
		drawArrow();
		glPopMatrix();
	}

	// Restore previous lighting and color states
	glPopAttrib();
}

// Function to draw a ring using a cylinder
void drawRing(float radius, float innerRadius, float r, float g, float b) {
	glPushMatrix();
	glColor3f(r, g, b);  // Set color for the ring

	// Create the outer cylinder (ring)
	GLUquadric* quad = gluNewQuadric();
	gluCylinder(quad, innerRadius, radius, 0.02f, 50, 1);  // Thin cylinder with small height
	gluDeleteQuadric(quad);

	glPopMatrix();
}

float TargetScale = 1.0;

// Function to draw the entire archery target using cylinders for the rings
void drawArcheryTarget() {
	glPushAttrib(GL_LIGHTING_BIT | GL_CURRENT_BIT);

	glPushMatrix();
	glScaled(TargetScale, TargetScale, TargetScale);  // Scale the target

	// Draw the rings as cylinders
	// 1. Bullseye (red)
	drawRing(0.1f, 0.0f, 1.0f, 0.0f, 0.0f);  // Red center

	// 2. Inner ring (blue)
	drawRing(0.2f, 0.1f, 0.0f, 0.0f, 1.0f);  // Blue ring

	// 3. Second ring (black)
	drawRing(0.3f, 0.2f, 0.0f, 0.0f, 0.0f);  // Black ring

	// 4. Third ring (white)
	drawRing(0.4f, 0.3f, 1.0f, 1.0f, 1.0f);  // White ring

	// 5. Outer ring (yellow)
	drawRing(0.5f, 0.4f, 1.0f, 1.0f, 0.0f);  // Yellow ring

	glPopMatrix();
	glPopAttrib();
}


void drawCircle(float outerRadius) {
	int numSegments = 100;
	float innerRadius = outerRadius - 0.1f; // Small offset for the inner radius to define the ring's thickness
	float depth = 0.05f; // Depth for the 3D effect

	glBegin(GL_QUAD_STRIP);
	for (int i = 0; i <= numSegments; i++) {
		float angle = 2.0f * 3.14159f * i / numSegments;
		float xOuter = outerRadius * cos(angle);
		float yOuter = outerRadius * sin(angle);
		float xInner = innerRadius * cos(angle);
		float yInner = innerRadius * sin(angle);

		// Front face of the ring
		glVertex3f(xOuter, yOuter, depth); // Outer point on front face
		glVertex3f(xInner, yInner, depth); // Inner point on front face

		// Back face of the ring
		glVertex3f(xOuter, yOuter, -depth); // Outer point on back face
		glVertex3f(xInner, yInner, -depth); // Inner point on back face
	}
	glEnd();
}


void drawOlympicFlag() {
	glPushAttrib(GL_LIGHTING_BIT | GL_CURRENT_BIT);
	float ringRadius = 0.6;  // Radius of each ring
	float yOffset = 0.0;     // Vertical offset for the rings




	glPushMatrix();
	glTranslated(-1.5, yOffset, 0.0); // Position of the blue ring
	glColor3f(0.0, 0.0, 1.0);          // Blue color
	drawCircle(ringRadius);
	glPopMatrix();

	// Black Ring
	glPushMatrix();
	glTranslated(0.0, yOffset, 0.0);
	glColor3f(0.0, 0.0, 0.0);
	drawCircle(ringRadius);
	glPopMatrix();

	// Red Ring
	glPushMatrix();
	glTranslated(1.5, yOffset, 0.0);
	glColor3f(1.0, 0.0, 0.0);
	drawCircle(ringRadius);
	glPopMatrix();

	// Yellow Ring
	glPushMatrix();
	glTranslated(-0.75, -0.5, 0.0);
	glColor3f(1.0, 1.0, 0.0);
	drawCircle(ringRadius);
	glPopMatrix();

	// Green Ring
	glPushMatrix();
	glTranslated(0.75, -0.5, 0.0);
	glColor3f(0.0, 1.0, 0.0);
	drawCircle(ringRadius);
	glPopMatrix();
	glPopAttrib();
}


float timeElapsed = 0.0f; // Keeps track of time
float PodR = 1.0;
float PodG = 0.0;
float PodB = 0.0;
bool changePodColor = false;

// Single function for updating color and rendering
void updateWallColor(int val) {
	// Update color over time using a sine wave pattern
	colorR = (sin(timeElapsed) + 1.0f) / 2.0f;      // Red oscillates between 0 and 1
	colorG = (sin(timeElapsed + 2.0f) + 1.0f) / 2.0f; // Green with a phase shift
	colorB = (sin(timeElapsed + 4.0f) + 1.0f) / 2.0f; // Blue with a phase shift
	TargetScale = (sin(timeElapsed) + 1.0f) * (5.0f - 0.5f) / 2.0f + 0.5f;
	if (changePodColor) {
		PodG = (sin(timeElapsed) + 1.0f) / 2.0f;      // Red oscillates between 0 and 1
		PodB = (sin(timeElapsed + 2.0f) + 1.0f) / 2.0f; // Green with a phase shift
		PodR = (sin(timeElapsed + 4.0f) + 1.0f) / 2.0f; // Blue with a phase shift
	}
	// Update the time for the next frame
	timeElapsed += 0.005f;
	glutPostRedisplay();
	glutTimerFunc(50, updateWallColor, 0);
}

void drawRoom() {
	glPushMatrix();

	glTranslated(0.0, 8.85, 0.0);
	// Front Wall (Z-axis)
	glPushMatrix();
	glTranslated(-15.0, -10.0, -5.0); // Position the front wall
	glScaled(30.0, 2.0, 1.0);      // Scale it to make it wide
	drawWall(5.0);                 // Thickness of the wall
	glPopMatrix();
	// Back Wall (Z-axis)
	glPushMatrix();
	glTranslated(-15.0, -10.0, 25.0);  // Position the back wall
	glScaled(30.0, 2.0, 1.0);     // Scale it to make it wide
	drawWall(5.0);                // Thickness of the wall
	glPopMatrix();

	// Left Wall (X-axis)
	glPushMatrix();
	glTranslated(-15.0, -10.0, 25.0); // Position the left wall
	glRotated(90.0, 0.0, 1.0, 0.0); // Rotate the wall 90 degrees
	glScaled(30.0, 2.0, 1.0);      // Scale it to make it tall
	drawWall(5.0);                 // Thickness of the wall
	glPopMatrix();

	//// Right Wall (X-axis)
	//glPushMatrix();
	//glTranslated(15.0, -10.0, 25.0);  // Position the right wall
	//glRotated(90.0, 0.0, 1.0, 0.0); // Rotate the wall 90 degrees
	//glScaled(30.0, 2.0, 1.0);     // Scale it to make it tall
	//drawWall(5.0);                // Thickness of the wall
	//glPopMatrix();

	glPushMatrix();
	glTranslatef(-15.0, -10.0, -10.0);
	glRotated(90.0, 1.0, 0.0, 0.0);
	glScaled(30.0, 8.0, 1.0);
	drawWall(5.0);
	glPopMatrix();
	glPopMatrix();
	glPushMatrix();
	glTranslated(0.0, 7.0, -3.95);
	drawOlympicFlag();
	glPopMatrix();
	glPushMatrix();
	glTranslated(0.0, 1.5, -3.95);
	drawArcheryTarget();
	glPopMatrix();

}
int timer = 60;
bool isOver = false;
int score = 0;
float shootingAngle = 0.0;
bool firstMov = true;
int numberHit = 0;
void ShootArrow() {
	if (firstMov) {
		shootingAngle = rotationAngle * 3.14 / 180.0f;
		firstMov = false;
	}
	if (isShoot) {

		float newX, newZ;
		newX = arrowX + sin(shootingAngle) * arrowSpeed;
		newZ = arrowZ + cos(shootingAngle) * arrowSpeed;

		// Apply bounds check before updating
		if (newX < 14.0 && newX > -12.0 && newZ < 25.0 && newZ > -2) {
			arrowX = newX;
			arrowZ = newZ;
			if (newZ < -1.7 && newZ > -2 && newX <0.2 && newX > -0.2) {
				score += 1;
				numberHit += 1;
				if (numberHit == 1 || numberHit == 4 || numberHit == 7) {
					engine2->play2D("media/HitSound.mp3", false);
				}
			}
			if (score >= 9) {
				isOver = true;
			}
		}
		else {
			arrowX = playerX;
			arrowZ = playerZ;
			isShoot = false;
			firstMov = true;
		}
	}
	else {
		firstMov = true;
	}
}


float sphereY = 2.0f;
bool bounceUp = false;
float bounceSpeed = 0.1f;
bool bounce = false;

void animateLamp() {
	if (bounce) {
		if (bounceUp) {
			sphereY += bounceSpeed;
			if (sphereY > 4.0f) bounceUp = false;
		}
		else {
			sphereY -= bounceSpeed;
			if (sphereY < 2.0f) bounceUp = true;
		}
	}
}

void drawLamp() {
	glPushAttrib(GL_LIGHTING_BIT | GL_CURRENT_BIT);
	glPushMatrix();

	// Translate the object to a desired position
	glTranslatef(-5.0f, sphereY, 20.0f); // Position the lamp

	glPushMatrix();
	// Draw the light bulb (Sphere)
	glTranslatef(0.0, 0.5, 0.7);
	glColor3f(1.0f, 1.0f, 0.0f); // Yellow color for the light bulb
	glutSolidSphere(1.0f, 50, 50); // Draw the light bulb as a sphere
	glPopMatrix();

	// Draw the lamp stand (Cylinder)
	glTranslatef(0.0f, -1.5f, 0.0f); // Move the stand below the bulb
	glColor3f(0.6f, 0.6f, 0.6f); // Gray color for the stand
	glPushMatrix();
	glTranslatef(0.0, -2, 2.0);
	glRotated(-90, 1.0, 0.0, 0.0);
	GLUquadric* quadric = gluNewQuadric();
	gluCylinder(quadric, 0.2f, 0.2f, 4.0f, 32, 32); // Draw the stand as a cylinder
	glPopMatrix();

	// Draw the lampshade (Cone)
	glTranslatef(0.0f, 2.0f, 0.0f); // Move the cone above the stand
	glColor3f(0.5f, 0.5f, 0.5f); // Gray color for the lampshade
	glutSolidCone(1.5f, 3.0f, 50, 50);  // Draw the lampshade as a cone

	glPopMatrix();
	glPopAttrib();
}





void drawOlympicPodium() {
	glPushAttrib(GL_LIGHTING_BIT | GL_CURRENT_BIT);
	glPushMatrix();
	// Translate to position the podium
	glTranslatef(-5.0f, -0.5, 15.0f);
	glColor3f(PodR, PodG, PodB);

	// Draw the base (Rectangular Block)

	glPushMatrix();
	glScalef(3.0f, 0.5f, 1.0f);  // Scale the rectangular block
	glutSolidCube(2.0f); // Draw the rectangular base
	glPopMatrix();

	// Draw the first step (Cylinder)
	glTranslatef(0.0f, 1.5f, 0.0f); // Move up for the first step
	glutSolidCube(2.0f);

	// Draw the second step (Cylinder)
	glPushMatrix();
	glTranslatef(1.0f, -1.0f, 0.0f); // Move up for the second step
	glScalef(2.0, 0.5, 1.0);
	glutSolidCube(2.0f);
	glPopMatrix();
	glPopMatrix();
	glPopAttrib();
}

bool rotateChair = false;
float chairRotation = 0.0f;

void animateChair() {
	if (rotateChair) {
		chairRotation += 0.1f;
		if (chairRotation > 360.0f) chairRotation = 0.0f;
	}
}
bool rotateTable = false;

void animateTable() {
	if (rotateTable) {
		TableRotation += 0.1f;
		if (TableRotation > 360.0f) TableRotation = 0.0f;
	}
}

void drawChair() {
	glPushAttrib(GL_LIGHTING_BIT | GL_CURRENT_BIT);
	glPushMatrix();
	glTranslatef(10.0f, -0.2f, 15.0);
	glRotatef(chairRotation + 180, 0.0f, 1.0f, 0.0f);
	// Seat (Cube)
	glColor3f(0.5f, 0.35f, 0.05f); // Wood-like color
	glPushMatrix();
	glTranslatef(0.0f, 1.0f, 0.0f);  // Position the seat
	glScalef(2.0f, 0.2f, 2.0f);      // Scale to form the seat
	glutSolidCube(1.0f);             // Draw the seat as a cube
	glPopMatrix();

	// Legs (Cylinders)
	glColor3f(0.3f, 0.2f, 0.1f); // Darker wood color

	// First leg
	glPushMatrix();
	glTranslatef(-0.8f, -1.0f, -0.8f);  // Position the first leg
	glScaled(1.0, 6.0, 1.0);
	glRotatef(-90, 1.0f, 0.0f, 0.0f);  // Rotate the cylinder to align with Z-axis
	GLUquadric* quadric = gluNewQuadric();
	gluCylinder(quadric, 0.05f, 0.05f, 1.0f, 16, 16); // Draw the leg
	glPopMatrix();

	// Second leg
	glPushMatrix();
	glTranslatef(0.8f, -1.0f, -0.8f);  // Position the second leg
	glScaled(1.0, 6.0, 1.0);
	glRotatef(-90, 1.0f, 0.0f, 0.0f); // Rotate to align with Z-axis
	gluCylinder(quadric, 0.05f, 0.05f, 1.0f, 16, 16); // Draw the leg
	glPopMatrix();

	// Third leg
	glPushMatrix();
	glTranslatef(-0.8f, -1.0f, 0.8f);  // Position the third leg
	glScaled(1.0, 2.1, 1.0);
	glRotatef(-90, 1.0f, 0.0f, 0.0f); // Rotate to align with Z-axis
	gluCylinder(quadric, 0.05f, 0.05f, 1.0f, 16, 16); // Draw the leg
	glPopMatrix();

	// Fourth leg
	glPushMatrix();
	glTranslatef(0.8f, -1.0f, 0.8f);   // Position the fourth leg
	glScaled(1.0, 2.1, 1.0);
	glRotatef(-90, 1.0f, 0.0f, 0.0f); // Rotate to align with Z-axis
	gluCylinder(quadric, 0.05f, 0.05f, 1.0f, 16, 16); // Draw the leg
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.75f, 4.8f, -0.8f);   // Position the fourth leg
	glScaled(1.65, 2.1, 1.0);
	glRotatef(-90, 0.0f, 1.0f, 0.0f); // Rotate to align with Z-axis
	gluCylinder(quadric, 0.05f, 0.05f, 1.0f, 16, 16); // Draw the leg
	glPopMatrix();

	glPopMatrix();
	glPopAttrib();
}




void drawScoreboard(float x, float y, int z, char* text) {
	// Switch to orthographic projection for the text
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0, 800, 0, 600);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	// Draw the "Score" label in fixed screen space
	glColor3f(0.0f, 0.0f, 0.0f); // White text
	// Draw the actual score in fixed screen space
	glRasterPos2f(x, y); // Adjust coordinates to place score below the label
	char scoreText[50];
	if (z >= 0) {
		sprintf(scoreText, "%s: %d", text, z);
	}
	else {
		sprintf(scoreText, "%s", text);
	}
	for (const char& c : scoreText) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
	}


	// Restore previous projection and modelview matrices
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
}

// Function to handle game timer
void updateTime() {
	static int lastUpdate = 0;
	int currentTime = glutGet(GLUT_ELAPSED_TIME);
	if (currentTime - lastUpdate >= 1000) {  // Update every second
		timer--;
		lastUpdate = currentTime;
		if (timer <= 0) {
			isOver = true;
		}
	}
}

bool scaleFlag = false;
float FlagScale = 1.0;
bool flagScaleUp = true;
float flagScaleSpeed = 0.05;

void drawArrowsHolder() {
	glPushAttrib(GL_LIGHTING_BIT | GL_CURRENT_BIT);
	glPushMatrix();
	glTranslatef(-10, 0.5, 5);
	glRotatef(90, 0, 1, 0);
	glScalef(FlagScale, FlagScale, FlagScale);
	// Color for the shelf frame
	glColor3f(0.5f, 0.35f, 0.05f);  // Brown

	// Side Panels
	glPushMatrix();
	glTranslatef(-0.6f, 0.0f, 0.0f); // Left side panel
	glScalef(0.1f, 1.5f, 0.3f);
	glutSolidCube(1.0f);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.6f, 0.0f, 0.0f); // Right side panel
	glScalef(0.1f, 1.5f, 0.3f);
	glutSolidCube(1.0f);
	glPopMatrix();

	// Top and Bottom Panels
	glPushMatrix();
	glTranslatef(0.0f, 0.75f, 0.0f); // Top panel
	glScalef(1.2f, 0.1f, 0.3f);
	glutSolidCube(1.0f);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0f, -0.75f, 0.0f); // Bottom panel
	glScalef(1.2f, 0.1f, 0.3f);
	glutSolidCube(1.0f);
	glPopMatrix();

	// Shelves
	for (int i = -1; i <= 1; i++) {
		glPushMatrix();
		glTranslatef(0.0f, i * 0.5f, 0.0f); // Position each shelf
		glScalef(1.2f, 0.1f, 0.3f);
		glutSolidCube(1.0f);
		glPopMatrix();
	}

	// Arrows on shelves
	for (int i = -1; i <= 1; i++) {
		glPushMatrix();
		glTranslatef(0.0f, i * 0.5f, 0.15f);  // Adjust to each shelf level

		for (float j = -0.5f; j <= 0.5f; j += 0.3f) {
			glPushMatrix();
			glTranslatef(j, -0.3f, -0.4);
			glScalef(0.3f, 0.3f, 0.3f);
			drawArrow();
			glPopMatrix();
		}

		glPopMatrix();
	}

	glPopMatrix();
	glPopAttrib();
}


void animateArrowsHolder() {
	if (scaleFlag) {
		if (flagScaleUp) {
			FlagScale += flagScaleSpeed;
			if (FlagScale > 2.0f) flagScaleUp = false;
		}
		else {
			FlagScale -= flagScaleSpeed;
			if (FlagScale < 1.0f) flagScaleUp = true;
		}
	}
}

void drawGameOver() {
	glColor3f(1.0f, 0.0f, 0.0f);  // Set color to red
	glRasterPos2f(300, 300);

	if (score < 3) {
		drawScoreboard(300, 300, -1, "Game Over!");
		engine2->play2D("media/lose.mp3", false);
	}
	else {
		drawScoreboard(300, 300, -1, "Game End!");
		engine2->play2D("media/win.mp3", false);
	}

	glColor3f(1.0f, 1.0f, 1.0f);  // White color for score
	char scoreText[50];
	sprintf(scoreText, "Final Score: %d", score);
	drawScoreboard(300, 400, -1, scoreText);

	glColor3f(1.0f, 1.0f, 1.0f);  // Instructions
	drawScoreboard(300, 500, -1, "Press R to restart!");
}



void Display() {
	setupCamera();
	setupLights();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	if (!isOver) {
		glPushMatrix();

		drawPlayer(0.0f, 1.0f, 0.0f); // Position the player
		drawRoom();
		drawLamp();
		drawOlympicPodium();
		drawChair();
		drawTable(0.6, 0.02, 0.02, 0.3);
		drawScoreboard(80, 550, score, "Score");
		drawScoreboard(80, 520, timer, "Time");
		drawArrowsHolder();
		glPopMatrix();
		updateLegs();
		ShootArrow();
		animateLamp();
		animateChair();
		animateTable();
		animateArrowsHolder();
		updateTime();
	}
	else {
		drawGameOver();
	}

	glFlush();
}


bool isFullscreen = true;  // Start in fullscreen mode

// Function to toggle between fullscreen and windowed mode
void toggleFullscreen() {
	if (isFullscreen) {
		glutReshapeWindow(800, 600);  // Switch back to windowed mode
		glutPositionWindow(100, 100);  // Position the window
		isFullscreen = false;
	}
	else {
		glutFullScreen();  // Switch to fullscreen mode
		isFullscreen = true;
	}
}

void Keyboard(unsigned char key, int x, int y) {
	float d = 0.01;
	float a = 1.0;

	switch (key) {
	case 'w':
		camera.moveY(d);
		break;
	case 's':
		camera.moveY(-d);
		break;
	case 'a':
		camera.moveX(d);
		break;
	case 'd':
		camera.moveX(-d);
		break;
	case 'q':
		camera.moveZ(d);
		break;
	case 'e':
		camera.moveZ(-d);
		break;
	case 'i':
		camera.rotateX(a);
		break;
	case 'k':
		camera.rotateX(-a);
		break;
	case 'j':
		camera.rotateY(a);
		break;
	case 'l':
		camera.rotateY(-a);
		break;

	case 'f':
		toggleFullscreen();
		break;
	case '1': // Top View
		camera.eye = TOP_VIEW_EYE;
		camera.center = TOP_VIEW_CENTER;
		camera.up = TOP_VIEW_UP;
		break;
	case '2': // Side View
		camera.eye = SIDE_VIEW_EYE;
		camera.center = SIDE_VIEW_CENTER;
		camera.up = SIDE_VIEW_UP;
		break;
	case '3': // Front View
		camera.eye = FRONT_VIEW_EYE;
		camera.center = FRONT_VIEW_CENTER;
		camera.up = FRONT_VIEW_UP;
		break;
	case ' ':
		if (!isShoot) engine2->play2D("media/shootSound.mp3", false);
		isShoot = true;
		break;
	case '5':
		bounce = !bounce;
		break;
	case '6':
		changePodColor = !changePodColor;
		break;
	case '7':
		rotateChair = !rotateChair;
		break;
	case '8':
		rotateTable = !rotateTable;
		break;
	case '9':
		scaleFlag = !scaleFlag;
		break;
	case 'r':
		if (isOver) {
			score = 0;
			timer = 30;
			isOver = false;
		}
		break;
	case GLUT_KEY_ESCAPE:
		exit(EXIT_SUCCESS);
	}

	glutPostRedisplay();
}
void Special(int key, int x, int y) {
	float rad = -rotationAngle * 3.14 / 180.0f; // Convert angle to radians for movement
	isWalking = false;
	// Define tentative new positions
	float newX, newZ;

	switch (key) {
	case GLUT_KEY_UP:
		newX = playerX - sin(rad) * moveSpeed;
		newZ = playerZ + cos(rad) * moveSpeed;

		// Apply bounds check before updating
		if (newX < 15.0 && newX > -10.0 && newZ < 25.0 && newZ > -3.5) {
			isWalking = true;
			playerX = newX;
			playerZ = newZ;
		}
		break;

	case GLUT_KEY_DOWN:
		newX = playerX + sin(rad) * moveSpeed;
		newZ = playerZ - cos(rad) * moveSpeed;

		if (newX < 15.0 && newX > -10.0 && newZ < 25.0 && newZ > -3.5) {
			isWalking = true;
			playerX = newX;
			playerZ = newZ;
		}
		break;

	case GLUT_KEY_LEFT:
		newX = playerX + cos(rad) * moveSpeed;
		newZ = playerZ + sin(rad) * moveSpeed;

		if (newX < 15.0 && newX > -10.0 && newZ < 25.0 && newZ > -3.5) {
			isWalking = true;
			playerX = newX;
			playerZ = newZ;
		}
		break;

	case GLUT_KEY_RIGHT:
		newX = playerX - cos(rad) * moveSpeed;
		newZ = playerZ - sin(rad) * moveSpeed;

		if (newX < 15.0 && newX > -10.0 && newZ < 25.0 && newZ > -3.5) {
			isWalking = true;
			playerX = newX;
			playerZ = newZ;
		}
		break;
	}

	glutPostRedisplay();

}

void SpecialUp(int key, int x, int y) {
	isWalking = false;
	glutPostRedisplay();
}




int lastX = 320, lastY = 240; // Initial mouse position
bool leftButtonPressed = false; // Track if left mouse button is held down
bool rightButtonPressed = false;
float sensitivity = 0.1f;
float sensitivityRight = 0.01f;
int lastLeftClickTime = 0;            // Last time the left mouse button was clicked
int lastRightClickTime = 0;           // Last time the right mouse button was clicked
const int DOUBLE_CLICK_TIME = 500;    // Time threshold for double-click detection (in ms)
bool leftDoubleClickDetected = false; // Flag for left double-click
bool rightDoubleClickDetected = false; // Flag for right double-click




// Function to handle mouse button events (press/release)
void mouseButton(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON) {
		if (state == GLUT_DOWN) {
			leftButtonPressed = true; // Start dragging
			lastX = x;
			lastY = y;
		}
		else if (state == GLUT_UP) {
			leftButtonPressed = false; // Stop dragging
		}
	}if (button == GLUT_RIGHT_BUTTON) {
		if (state == GLUT_DOWN) {
			rightButtonPressed = true; // Start dragging
			lastX = x;
			lastY = y;
		}
		else if (state == GLUT_UP) {
			rightButtonPressed = false; // Stop dragging
		}
	}
	int currentTime = glutGet(GLUT_ELAPSED_TIME); // Get current time in ms

	// Handle left button clicks
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		if (currentTime - lastLeftClickTime <= DOUBLE_CLICK_TIME) {
			if (!leftDoubleClickDetected) {
				leftDoubleClickDetected = true; // Mark left double-click detected
				camera.moveZ(0.05);
				glutPostRedisplay();
			}
		}
		else {
			leftDoubleClickDetected = false; // Reset the flag if time exceeds threshold
		}
		lastLeftClickTime = currentTime; // Update last click time for left button
	}

	// Handle right button clicks
	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
		if (currentTime - lastRightClickTime <= DOUBLE_CLICK_TIME) {
			if (!rightDoubleClickDetected) {
				rightDoubleClickDetected = true; // Mark right double-click detected
				camera.moveZ(-0.05);
				glutPostRedisplay();
			}
		}
		else {
			rightDoubleClickDetected = false; // Reset the flag if time exceeds threshold
		}
		lastRightClickTime = currentTime; // Update last click time for right button
	}
}

// Function to handle mouse dragging (moving while holding the left button)
void mouseDrag(int x, int y) {
	if (leftButtonPressed) {
		float xoffset = x - lastX;
		rotationAngle -= xoffset * rotateSpeed;
		if (rotationAngle > 360.0 || rotationAngle < -360.0) {
			rotationAngle = 0.0f;
		}
		//float yoffset = lastY - y; // Reversed since y-coordinates go from bottom to top
		//lastX = x;
		//lastY = y;

		//xoffset *= sensitivity;
		//yoffset *= sensitivity;

		//camera.rotateY(xoffset);
		//camera.rotateX(yoffset);

		// Constrain pitch to avoid flipping
		/*if (camera.pitch > 89.0f) camera.pitch = 89.0f;
		if (camera.pitch < -89.0f) camera.pitch = -89.0f;

		camera.updateCameraDirection();*/
		glutPostRedisplay(); // Request display update
	}if (rightButtonPressed) {
		float xoffset = x - lastX;
		float yoffset = lastY - y; // Reversed since y-coordinates go from bottom to top
		lastX = x;
		lastY = y;

		xoffset *= sensitivityRight;
		yoffset *= sensitivityRight;

		camera.moveX(xoffset);
		camera.moveY(yoffset);

		// Constrain pitch to avoid flipping
		/*if (camera.pitch > 89.0f) camera.pitch = 89.0f;
		if (camera.pitch < -89.0f) camera.pitch = -89.0f;

		camera.updateCameraDirection();*/
		glutPostRedisplay(); // Request display update
	}
}






void main(int argc, char** argv) {
	glutInit(&argc, argv);

	glutInitWindowSize(640, 480);
	glutInitWindowPosition(50, 50);

	glutCreateWindow("3D Archery Game");
	glutDisplayFunc(Display);
	glutKeyboardFunc(Keyboard);
	glutSpecialFunc(Special);
	glutSpecialUpFunc(SpecialUp);
	glutMouseFunc(mouseButton);
	glutMotionFunc(mouseDrag);

	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);
	glEnable(GL_COLOR_MATERIAL);

	glShadeModel(GL_SMOOTH);
	glutFullScreen();
	updateWallColor(0);

	irrklang::ISoundEngine* engine = irrklang::createIrrKlangDevice();
	if (!engine) {
		return;
	}
	engine->setSoundVolume(0.07f);
	irrklang::ISound* sound = engine->play2D("media/main.mp3", true);


	camera.eye = TOP_VIEW_EYE;
	camera.center = TOP_VIEW_CENTER;
	camera.up = TOP_VIEW_UP;
	glutMainLoop();

	engine->drop();// Enter the GLUT event processing loop
	engine2->drop();
}
