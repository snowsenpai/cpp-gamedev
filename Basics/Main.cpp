#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <memory>
#include <string>

struct shape_directions
{
	float left;
	float right;
	float up;
	float down;
};

static std::shared_ptr<sf::Shape> circleFactory(float posX, float posY, unsigned int r, unsigned int g, unsigned int b, float radius)
{
	std::shared_ptr<sf::Shape> circle = std::make_shared<sf::CircleShape>(radius);
	circle->setPosition(sf::Vector2f(posX, posY));
	circle->setFillColor(sf::Color(r, g, b));
	return circle;
}
static std::shared_ptr<sf::Shape> rectangleFactory(float posX, float posY, unsigned int r, unsigned int g, unsigned int b, float rectX, float rectY)
{
	std::shared_ptr<sf::Shape> rectangle = std::make_shared<sf::RectangleShape>(sf::Vector2f(rectX, rectY));
	rectangle->setPosition(sf::Vector2f(posX, posY));
	rectangle->setFillColor(sf::Color(r, g, b));
	return rectangle;
}
static std::shared_ptr<sf::Shape> polygonFactory(float posX, float posY, unsigned int r, unsigned int g, unsigned int b, float radius, size_t pointCounts)
{
	std::shared_ptr<sf::Shape> polygon = std::make_shared<sf::CircleShape>(radius, pointCounts);
	polygon->setPosition(sf::Vector2f(posX, posY));
	polygon->setFillColor(sf::Color(r, g, b));
	return polygon;
}

class Shape2D
{
public:
	std::shared_ptr<sf::Shape> shape;
	sf::Text shapeName;
	sf::Vector2f velocity;
	shape_directions directions = {0,0,0,0};
	// created circle should have its radius
	// created rectangle should have its size of w and h
	Shape2D(std::shared_ptr<sf::Shape> shape, sf::Vector2f velocity, sf::Text shapeName)
		: shape(shape)
		, velocity(velocity)
		, shapeName(shapeName)
	{}

	void setDirections()
	{
		this->directions.left = -this->velocity.x;
		this->directions.right = this->velocity.x;
		this->directions.up = -this->velocity.y;
		this->directions.down = this->velocity.y;
	}
	void bounce(unsigned int maxWidth, unsigned int maxHeight) 
	{
		if ((this->shape->getPosition().x + this->shape->getGlobalBounds().width) >= maxWidth)
		{
			this->velocity.x = this->directions.left;
		}
		else if (this->shape->getPosition().x <= 0)
		{
			this->velocity.x = this->directions.right;
		}
		else if ((this->shape->getPosition().y + this->shape->getGlobalBounds().height) >= maxHeight)
		{
			this->velocity.y = this->directions.up;
		}
		else if (this->shape->getPosition().y <= 0)
		{
			this->velocity.y = this->directions.down;
		}
	}
	void move(float delta) const
	{
		float distanceX = this->velocity.x * delta;
		float distanceY = this->velocity.y * delta;

		this->shape->move(distanceX, distanceY);
	}
	void alignText()
	{
		sf::FloatRect shapeBounds(this->shape->getGlobalBounds());
		sf::FloatRect textBounds(this->shapeName.getGlobalBounds());

		float sizeDiffWidth = (shapeBounds.width - textBounds.width) / 2.f;
		float sizeDiffHeight = (shapeBounds.height - textBounds.height) / 2.f;

		this->shapeName.setPosition(this->shape->getPosition().x + sizeDiffWidth, this->shape->getPosition().y + sizeDiffHeight);
	}
};

class BouncingShapes
{
public:
	std::vector<Shape2D> shapes;
	BouncingShapes() {}

	void addShape(const Shape2D shapeObj)
	{
		shapes.push_back(shapeObj);
	}
	void setDirections()
	{
		for (Shape2D& shapeObj : shapes)
		{
			shapeObj.setDirections();
		}
	}
	void bounce(unsigned int maxWidth, unsigned int maxHeight)
	{
		for (Shape2D& shapeObj : shapes)
		{
			shapeObj.bounce(maxWidth, maxHeight);
		}
	}
	void move(float delta)
	{
		for (Shape2D& shapeObj : shapes)
		{
			shapeObj.move(delta);
		}
	}
	void freeze() {}
	void alignText()
	{
		for (Shape2D& shapeObj : shapes)
		{
			shapeObj.alignText();
		}
	}
	void drawShapesAndTexts(sf::RenderWindow& window)
	{
		for (Shape2D& shapeObj : shapes)
		{
			window.draw(*shapeObj.shape);
			window.draw(shapeObj.shapeName);
		}
	}
};

static sf::Font loadFont(std::string fontFile)
{
	// if load fails font will be empty
	sf::Font font;
	if (!font.loadFromFile(fontFile)) return font;
	return font;
}

static sf::Text createText(sf::Font& font, std::string textString, unsigned int characterSize, const sf::Color& color)
{
	sf::Text text;
	text.setFont(font);
	text.setString(textString);
	text.setCharacterSize(characterSize);
	text.setFillColor(color);
	text.setStyle(sf::Text::Bold | sf::Text::Underlined);
	return text;
}

int main(int argc, char * argv[])
{
	// load and read config file
	unsigned int windowWidth = 720;
	unsigned int windowHeight = 480;
	sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), "SFML Basics!");
	window.setFramerateLimit(60);
	window.setKeyRepeatEnabled(false);
	const float MAX_VOL = 100.f;
	const float MIN_VOL = 0.f;
	
	BouncingShapes bouncingShapes;

	sf::Font textFont = loadFont("assets/fonts/Xolonium-Regular.ttf");
	sf::Color textColor(255, 255, 255);
	unsigned int characterSize = 17;

	std::shared_ptr<sf::Shape> poly2 = polygonFactory(400.f, 180.f, 250, 0, 0, 70.f, 7);
	sf::Text polyText = createText(textFont, "poly text", characterSize, textColor);
	Shape2D polyShape(poly2, sf::Vector2f(100.f, 200.f), polyText);

	std::shared_ptr<sf::Shape> circ = circleFactory(200.f, 100.f, 0, 250, 0, 50.f);
	sf::Text circText = createText(textFont, "circ text", characterSize, textColor);
	Shape2D circShape(circ, sf::Vector2f(100.f, 150.f), circText);

	std::shared_ptr<sf::Shape> recta = rectangleFactory(100.f, 90.f, 0, 0, 250, 100.f, 50.f);
	sf::Text rectaText = createText(textFont, "recta text", characterSize, textColor);
	Shape2D rectaShape(recta, sf::Vector2f(200.f, 100.f), rectaText);

	std::shared_ptr<sf::Shape> polyg = polygonFactory(400.f, 200.f, 50, 150, 200, 60.f, 6);
	sf::Text polygText = createText(textFont, "polyg text", characterSize, textColor);
	Shape2D polygShape(polyg, sf::Vector2f(200.f, 200.f), polygText);

	bouncingShapes.addShape(polyShape);
	bouncingShapes.addShape(circShape);
	bouncingShapes.addShape(rectaShape);
	bouncingShapes.addShape(polygShape);

	sf::SoundBuffer gameoverBuffer;
	if (!gameoverBuffer.loadFromFile("assets/audio/gameover.wav"))
		return -1;

	sf::Sound gameover;
	gameover.setBuffer(gameoverBuffer);

	sf::Music forestLoop;
	if (!forestLoop.openFromFile("assets/audio/House In a Forest Loop.ogg"))
		return -1;
	forestLoop.setVolume(50.f);
	forestLoop.setLoop(true);
	forestLoop.play();
	float forestLoopVol = forestLoop.getVolume();
	//prevent vol increase and decrease if getVolume is at 100 and 0 respectively
	bool changeVol = forestLoopVol > MIN_VOL && forestLoopVol < MAX_VOL;

	bouncingShapes.setDirections();
	sf::Clock clock;
	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}

		// improve control flow
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::F))
			gameover.play();

		sf::Sound::Status forestLoopStatus = forestLoop.getStatus();
		switch (forestLoopStatus)
		{
		case sf::SoundSource::Stopped:
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::Space))
				forestLoop.play();
			break;
		case sf::SoundSource::Paused:
//using space key to pause and play isn't smooooth, key is triggered multiple times, same for gameover sound
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::C))
				forestLoop.play();
			break;
		case sf::SoundSource::Playing:
			//pause or stop the music
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::V))
				forestLoop.pause();
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::Backspace))
				forestLoop.stop();

			//increase/reduce volume
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::Up))
				// increment and decrement values are not by 1 for floats but btw 6,7,8
				forestLoop.setVolume(forestLoopVol += 1);
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::Down))
				forestLoop.setVolume(forestLoopVol -= 1);
			//std::cout << "volume " << forestLoop.getVolume() << std::endl;
			break;
		default:
			break;
		}
		bouncingShapes.bounce(windowWidth, windowHeight);
		
		float delta = clock.restart().asSeconds();
	
		// animate sprite and text
		bouncingShapes.move(delta);
		bouncingShapes.alignText();
		
		window.clear();
		bouncingShapes.drawShapesAndTexts(window);
		window.display();
	}
	return 0;
}