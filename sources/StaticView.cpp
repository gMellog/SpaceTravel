#include "StaticView.h"
#include "SpaceTravel.h"
#include "GoldenAsteroid.h"

void StaticView::draw()
{
    if (spacecraft == nullptr)
    {
        spacecraft = &SpaceTravel::getActorRef<Spacecraft>();
        spacecraft->addCollideListener([this](const std::string& collideTag)
            {
                if (collideTag == Tags::AsteroidTag)
                    addMessage(createCollideMessage(upLoc));
            });
        spacecraft->addCollideListener([this](const std::string& collideTag) {
            if (collideTag == Tags::GoldenAsteroidTag)
                addMessage(createGoldMessage(upLoc));
            });
    }

	glViewport(viewportStart.X, viewportStart.Y, viewportSize.X, viewportSize.Y);
	glLoadIdentity();

	drawMessages();

	setupCamera();

}

Vector StaticView::getAmountOfSteps() const noexcept
{
    const auto spacecraftLoc = spacecraft->getTransform().translation;
    const auto cameraEyeOnPlane = Vector{ cameraEye.X, 0.f, cameraEye.Z };
    const auto diff = cameraEyeOnPlane - spacecraftLoc;

    return {
        diff.X / SpaceTravel::getXOffset() * (diff.X >= 0 ? 1.f : -1.f),
        0.f,
        diff.Z / SpaceTravel::getZOffset()
    };
}

void StaticView::setupCamera()
{   
    if(!boundToAsteroid)
    {
        boundToAsteroid = SpaceTravel::getActorPointer<GoldenAsteroid>();
        if(boundToAsteroid != nullptr)
        {
            boundToAsteroid->onDie( 
            [this]()
            {
               boundToAsteroid = nullptr;
            });

            const auto loc = boundToAsteroid->getTransform().translation;
            cameraEye = { loc.X, 20.f, loc.Z };

            if(setupCameraListener)
            {
                setupCameraListener();
            }

        }
        else
        {
            cameraEye = {10.f, 20.f, 0.f};
        }
    }

    const auto spacecraftLoc = spacecraft->getTransform().translation;
    gluLookAt(cameraEye.X, cameraEye.Y, cameraEye.Z, spacecraftLoc.X, spacecraftLoc.Y, spacecraftLoc.Z, 0.0, 1.0, 0.0);
}

void StaticView::drawMessages()
{
    std::vector<DrawMessage*> expiredMessages;

    for (auto& message : messages)
    {
        if (!message.isTimeOut())
            message.draw(SpaceTravel::getDeltaTime());
        else
            expiredMessages.push_back(&message);
    }

    auto firstExpiredMessage = std::remove_if(std::begin(messages), std::end(messages),
        [&expiredMessages]
    (const DrawMessage& message)
        {
            for (auto dieMessage : expiredMessages)
            {
                if (&message == dieMessage)
                {
                    return true;
                }
            }

            return false;
        });

    messages.erase(firstExpiredMessage, std::end(messages));
}

void StaticView::addMessage(const DrawMessage& drawMessage)
{
    for (auto& message : messages)
    {
        auto messageLoc = message.getLoc();
        messageLoc.Y -= 2.f;
        message.setLoc(messageLoc);
    }

    messages.push_back(drawMessage);
}
