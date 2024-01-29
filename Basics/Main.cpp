#include <SFML/Graphics.hpp>

int main(int argc, char * argv[])
{
	sf::RenderWindow window(sf::VideoMode(720, 360), "SFML Basics!");
	window.setFramerateLimit(60);

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

	sf::CircleShape octagon(30.f, 8);
	octagon.setFillColor(sf::Color(250, 200, 150));
	octagon.setPosition(sf::Vector2f(100.f, 30.f));

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
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