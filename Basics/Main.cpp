#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>

int main(int argc, char * argv[])
{
	sf::RenderWindow window(sf::VideoMode(720, 360), "SFML Basics!");
	window.setFramerateLimit(60);
	window.setKeyRepeatEnabled(true);
	const float MAX_VOL = 100.f;
	const float MIN_VOL = 0.f;

	sf::Texture playerWalk1;
	if (!playerWalk1.loadFromFile("assets/art/playerGrey_walk1.png"))
		return EXIT_FAILURE;
	playerWalk1.setSmooth(true);
	
	sf::Texture playerWalk2;
	if (!playerWalk2.loadFromFile("assets/art/playerGrey_walk2.png"))
		return EXIT_FAILURE;
	playerWalk2.setSmooth(true);

	sf::Sprite playerWalk;
	sf::Sprite playerWalking;
	playerWalk.setTexture(playerWalk1);
	playerWalking.setTexture(playerWalk2);

	playerWalk.setPosition(sf::Vector2f(10.f, 50.f));

	sf::Font xoloniumR;
	if (!xoloniumR.loadFromFile("assets/fonts/Xolonium-Regular.ttf"))
		return EXIT_FAILURE;

	sf::Text hello;
	hello.setFont(xoloniumR);
	hello.setString("Hello world");
	hello.setCharacterSize(24);
	hello.setFillColor(sf::Color::White);
	hello.setStyle(sf::Text::Bold | sf::Text::Underlined);

	sf::CircleShape circle(50.f);
	circle.setTexture(&playerWalk2);
	circle.setTextureRect(sf::IntRect(10, 10, 100, 100));
	circle.setPosition(sf::Vector2f(200.f, 30.f));

	sf::RectangleShape rectangle(sf::Vector2f(120.f, 50.f));
	rectangle.setPosition(sf::Vector2f(10.f, 10.f));
	rectangle.setFillColor(sf::Color(100, 250, 70));;
	rectangle.setSize(sf::Vector2f(100.f, 100.f));

	sf::CircleShape triangle(30.f, 3);
	triangle.setFillColor(sf::Color(20, 40, 50));
	triangle.setPosition(sf::Vector2f(20.f, 40.f));
	
	sf::CircleShape square(30.f, 4);
	square.setFillColor(sf::Color(134, 200, 80));
	square.setPosition(sf::Vector2f(50.f, 10.f));
	square.move(30.f, 20.f);
	sf::Vector2f squarePos = square.getPosition();
	square.setRotation(30.f);
	square.rotate(15.f);
	float rotation = square.getRotation();
	square.setScale(2.f, 2.f);
	square.scale(1.5f, 1.5f);
	sf::Vector2f squareScale = square.getScale();
	square.setOrigin(10.f, 20.f);
	sf::Vector2f squareOrigin = square.getOrigin();
	sf::FloatRect squareBoundingBox = square.getGlobalBounds();

	if (squareBoundingBox.intersects(hello.getGlobalBounds()))
		std::cout << "sqaure is touching hello text" << std::endl;
	else
		std::cout << "sqaure is not touching hello text" << std::endl;

	sf::CircleShape octagon(30.f, 8);
	octagon.setFillColor(sf::Color(250, 200, 150));
	octagon.setPosition(sf::Vector2f(100.f, 30.f));

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
	//bool changeVol = forestLoopVol > MIN_VOL && forestLoopVol < MAX_VOL;

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}

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
			std::cout << "volume " << forestLoop.getVolume() << std::endl;
			break;
		default:
			break;
		}

		window.clear();
		window.draw(hello);
		window.draw(circle);
		window.draw(playerWalk);
		window.draw(rectangle);
		window.draw(triangle);
		window.draw(square);
		window.draw(octagon);
		window.display();
	}

	return 0;
}