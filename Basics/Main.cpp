#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <memory>
#include <string>
#include <fstream>

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

struct FontProp
{
	sf::Font font;
	int r, g, b, charSize;
};

class Shape2D
{
public:
	std::shared_ptr<sf::Shape> shape;
	sf::Text shapeText;
	std::string shapeName;
	static FontProp s_fontProp;
	sf::Vector2f velocity;
	shape_directions directions = {0,0,0,0};
	Shape2D(std::shared_ptr<sf::Shape> shape, sf::Vector2f velocity, std::string shapeName)
		: shape(shape)
		, velocity(velocity)
		, shapeName(shapeName)
	{}

	void setDirections()
	{
		if (this->velocity.x >= 0)
		{
			this->directions.right = this->velocity.x;
			this->directions.left = this->velocity.x * -1.f;
		}
		else
		{
			this->directions.right = this->velocity.x * -1.f;
			this->directions.left = this->velocity.x;
		}
		if (this->velocity.y >= 0)
		{
			this->directions.up = this->velocity.y * -1.f;
			this->directions.down = this->velocity.y;
		}
		else
		{
			this->directions.up = this->velocity.y;
			this->directions.down = this->velocity.y * -1.f;
		}
	}
	void bounce(sf::RenderWindow& window)
	{
		if ((this->shape->getPosition().x + this->shape->getGlobalBounds().width) >= window.getSize().x)
		{
			this->velocity.x = this->directions.left;
		}
		else if (this->shape->getPosition().x <= 0)
		{
			this->velocity.x = this->directions.right;
		}
		else if ((this->shape->getPosition().y + this->shape->getGlobalBounds().height) >= window.getSize().y)
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
	void freeze() {}
	void setText()
	{
		this->shapeText.setFont(Shape2D::s_fontProp.font);
		this->shapeText.setString(this->shapeName);
		this->shapeText.setCharacterSize(Shape2D::s_fontProp.charSize);
		this->shapeText.setFillColor(sf::Color(Shape2D::s_fontProp.r, Shape2D::s_fontProp.g, Shape2D::s_fontProp.b));
	}
	void alignText()
	{
		sf::FloatRect shapeBounds(this->shape->getGlobalBounds());
		sf::FloatRect textBounds(this->shapeText.getGlobalBounds());

		float sizeDiffWidth = (shapeBounds.width - textBounds.width) / 2.f;
		float sizeDiffHeight = (shapeBounds.height - textBounds.height) / 2.f;

		this->shapeText.setPosition(this->shape->getPosition().x + sizeDiffWidth, this->shape->getPosition().y + sizeDiffHeight);
	}
};

FontProp Shape2D::s_fontProp = { sf::Font(), 0,0,0,0 };

class BouncingShapes
{
public:
	std::vector<Shape2D> shapes;
	BouncingShapes() {}

	void addShape(const Shape2D& shapeObj)
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
	void bounce(sf::RenderWindow& window)
	{
		for (Shape2D& shapeObj : shapes)
		{
			shapeObj.bounce(window);
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
	void setText()
	{
		for (Shape2D& shapeObj: shapes)
		{
			shapeObj.setText();
		}
	}
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
			window.draw(shapeObj.shapeText);
		}
	}
};

struct Config
{
	int error = 0;
	int windowW = 100;
	int windowH = 100;
	BouncingShapes bouncingShapes;
};

static Config loadConfigFile(std::string filePath)
{
	std::ifstream configFile(filePath);

	std::string configType;
	std::string name, fontPath;
	float posX, posY, velX, velY, radius, rectX, rectY;
	int windowWidth, windowHeight, fontSize, r, g, b, sides;

	BouncingShapes bShapes;

	Config config;

	if (!configFile.is_open())
	{
		std::cout << "failed to open configuration file " << filePath << std::endl;
		config.error = 2;
		return config;
	}
	
	while (configFile >> configType)
	{
		if (configType == "Window")
		{
			configFile >> windowWidth >> windowHeight;
			config.windowW = windowWidth;
			config.windowH = windowHeight;
		}
		else if (configType == "Font")
		{
			configFile >> fontPath >> fontSize >> r >> g >> b;
			sf::Font textFont;
			if (!textFont.loadFromFile(fontPath))
			{
				std::cout << "failed to load font " << fontPath << std::endl;
				config.error = 1;
				return config;
			}
			Shape2D::s_fontProp.font = textFont;
			Shape2D::s_fontProp.charSize = fontSize;
			Shape2D::s_fontProp.r = r;
			Shape2D::s_fontProp.g = g;
			Shape2D::s_fontProp.b = b;
		}
		else if (configType == "Circle")
		{
			configFile >> name >> posX >> posY >> velX >> velY >> r >> g >> b >> radius;
			std::shared_ptr<sf::Shape> circle = circleFactory(posX, posY, r, g, b, radius);
			Shape2D circleShape(circle, sf::Vector2f(velX, velY), name);
			bShapes.addShape(circleShape);
		}
		else if (configType == "Rectangle")
		{
			configFile >> name >> posX >> posY >> velX >> velY >> r >> g >> b >> rectX >> rectY;
			std::shared_ptr<sf::Shape> rectangle = rectangleFactory(posX, posY, r, g, b, rectX, rectY);
			Shape2D rectangleShape(rectangle, sf::Vector2f(velX, velY), name);
			bShapes.addShape(rectangleShape);
		}
		else if (configType == "Polygon")
		{
			configFile >> name >> posX >> posY >> velX >> velY >> r >> g >> b >> radius >> sides;
			std::shared_ptr<sf::Shape> polygon = polygonFactory(posX, posY, r, g, b, radius, sides);
			Shape2D polygonShape(polygon, sf::Vector2f(velX, velY), name);
			bShapes.addShape(polygonShape);
		}
	}
	config.bouncingShapes = bShapes;
	return config;
}

int main(int argc, char * argv[])
{
	Config config = loadConfigFile("config.txt");
	if (config.error) return config.error;

	sf::RenderWindow window(sf::VideoMode(config.windowW, config.windowH), "SFML Basics!");
	window.setFramerateLimit(60);
	window.setKeyRepeatEnabled(false);
	const float MAX_VOL = 100.f;
	const float MIN_VOL = 0.f;
	
	BouncingShapes bouncingShapes = config.bouncingShapes;

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
	//zero anad max bound checks i.e use <= and >= instead
	bool changeVol = forestLoopVol > MIN_VOL && forestLoopVol < MAX_VOL;

	bouncingShapes.setDirections();
	bouncingShapes.setText();

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
		bouncingShapes.bounce(window);
		
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