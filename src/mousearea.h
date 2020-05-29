#ifndef MOUSEAREA_H
#define MOUSEAREA_H

#include <vector>
#include <set>
#include <SFML/Graphics.hpp>

class MouseArea : public sf::Transformable
{
public:
	enum State {
		NONE, HOVERED, SELECTED, ACTIVE
	};
	
	enum Mode {
		CLICK, HOLD
	};
	
	MouseArea();
	MouseArea(size_t pointCount);
	MouseArea(sf::Shape const& shape);
	MouseArea(sf::Sprite const& sprite);
	
	void copyShape(sf::Shape const& shape);
	void copySprite(sf::Sprite const& sprite);

	void setExtraTransform(sf::Transform const& transform);
	
	void setPointCount(size_t count);
	size_t getPointCount() const { return m_points.size(); }
	void setPoint(size_t index, sf::Vector2f const& point);
	sf::Vector2f getPoint(size_t index) const { return m_points[index]; }
	
	//Updates the area state due to an event having happened
	void updateFromEvent(sf::Event const& event);
	//Updates the area state due to other reasons. Changes to geometry or transform are detected automatically and handled appropriately.
	//For other "unnoticeable" changes (e.g. toggling whether or not the area is being drawn), you can force the mouse detection algorithm's execution.
	void updateFromGeometryChange(sf::Window const& relativeTo, bool forceMouseUpdate = false);
	
	State getState() const { return m_state; }
	
	Mode getMode() const { return m_mode; }
	void setMode(Mode mode);
	
	std::set<sf::Mouse::Button> activatorButtons;
private:
	std::vector <sf::Vector2f> m_points;
	sf::Vector2f m_center;
	std::vector <float> m_coeffs;
	bool m_shouldRecalculateCoeffs = false;
		
	sf::Transform m_extraTransform = sf::Transform::Identity;
	sf::Transform m_previousTransform = sf::Transform::Identity;
	sf::Transform m_inverseTransform = sf::Transform::Identity;
	
	bool m_mouseInside = false;
	bool m_triggered = false;
	State m_state = NONE;
	Mode m_mode = CLICK;
	
	void recalculateCoeffs();
	void updateMouseInside(sf::Vector2i const& mousePos);
};

#endif // MOUSEAREA_H
