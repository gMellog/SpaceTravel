#include "DrawMessage.h"

void DrawMessage::setLoc(const Vector& newLoc)
{
    loc = newLoc;
}

Vector DrawMessage::getLoc() const noexcept
{
    return loc;
}

void DrawMessage::draw(float deltaTime)
{
    glPushMatrix();
    glColor3fv(reinterpret_cast<GLfloat*>(&color));
    glRasterPos3f(loc.X, loc.Y, loc.Z);
    writeBitmapString(GLUT_BITMAP_8_BY_13, text.c_str());
    glPopMatrix();

    t += deltaTime;

    timeOut = t >= messageTime;
}

bool DrawMessage::isTimeOut() const noexcept
{
    return timeOut;
}

DrawMessage createCollideMessage(const Vector& loc)
{
	return DrawMessage("Cannot - will crush!", loc, { 1.f, 0.f, 0.f }, 0.25f);
}

DrawMessage createGoldMessage(const Vector& loc)
{
	return DrawMessage("You've got a gold!", loc, Color{ 1.f, 215.f / 255.f, 0.f }, 2.f);
}