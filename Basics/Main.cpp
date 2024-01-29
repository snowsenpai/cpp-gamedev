#include <SFML/Graphics.hpp>

int main(int argc, char * argv[])
{
	sf::RenderWindow window(sf::VideoMode(720, 360), "SFML Basics!");

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
	playerWalking.setPosition(sf::Vector2f(10.f, 50.f));
	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}
		window.clear();
		window.draw(playerWalk);
		window.draw(playerWalking);
		window.display();
	}

	return 0;
}