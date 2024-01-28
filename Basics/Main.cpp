#include <SFML/Graphics.hpp>

int main()
{
	sf::RenderWindow window(sf::VideoMode(720, 360), "SFML works!");
	sf::CircleShape shape(100.f);
	shape.setFillColor(sf::Color::Magenta);

	// relative and absolute file paths fails
	sf::Texture playerWalk1;
	if (!playerWalk1.loadFromFile("assets/art/playerGery_walk1.png"))
		return EXIT_FAILURE;
	playerWalk1.setSmooth(true);
	playerWalk1.setRepeated(true);

	sf::Texture playerWalk2;
	if (!playerWalk2.loadFromFile("assets/art/playerGery_walk2.png"))
		return EXIT_FAILURE;
	playerWalk2.setSmooth(true);
	playerWalk2.setRepeated(true);

	sf::Sprite playerWalk;
	playerWalk.setTexture(playerWalk1);
	playerWalk.setTexture(playerWalk2);

	playerWalk.setPosition(sf::Vector2f(10.f, 50.f));
	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}
		window.clear();
		window.draw(shape);
		window.display();
	}

	return 0;
}