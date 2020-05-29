#include <SFML/Graphics.hpp>
#include <mousearea.h>
#include <iostream>

void colorShape(sf::Shape& shape, MouseArea const& area) {
	switch (area.getState()) {
	case MouseArea::NONE: shape.setFillColor(sf::Color::White); break;
	case MouseArea::HOVERED: shape.setFillColor(sf::Color::Blue); break;
	case MouseArea::SELECTED: shape.setFillColor(sf::Color::Green); break;
	case MouseArea::ACTIVE: shape.setFillColor(sf::Color::Red); break;
	}
}

int main() {
	
	sf::RenderWindow window(sf::VideoMode(800, 600), "SFML Project", sf::Style::Close);
	window.setFramerateLimit(60);
	window.setVerticalSyncEnabled(true);

	sf::Event haps;
	
	sf::RectangleShape rect(sf::Vector2f(150, 100));
	rect.setFillColor(sf::Color::White);
	rect.setPosition(200, 100);
	
	MouseArea a(rect);
	a.setMode(MouseArea::HOLD);
	
	sf::CircleShape circ(100);
	circ.setFillColor(sf::Color::White);
	circ.setPosition(500, 100);
	
	MouseArea a2(circ);
	a2.activatorButtons.clear();
	a2.activatorButtons.insert(sf::Mouse::Right);

	sf::Clock clock;
	sf::Time t0 = clock.getElapsedTime();
	
	while (window.isOpen()) {
		rect.setRotation((clock.getElapsedTime() - t0).asMilliseconds() * 0.1f);
		a.setRotation(rect.getRotation());
		a.updateFromGeometryChange(window);
		
		while (window.pollEvent(haps)) {
			if (haps.type == sf::Event::Closed)
				window.close();
			a.updateFromEvent(haps);
			a2.updateFromEvent(haps);
			if (a2.getState() == MouseArea::ACTIVE)
				std::cout << "boop" << rand() << std::endl;
		}
		
		if (a.getState() == MouseArea::ACTIVE)
			std::cout << "beep" << rand() << std::endl;
		
		colorShape(rect, a);
		colorShape(circ, a2);
		
		window.clear(sf::Color::Black);
		window.draw(rect);
		window.draw(circ);
		window.display();
	}
}
