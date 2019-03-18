#include "mousearea.h"

inline bool sameSign(float const& a, float const& b) {
	return (a >= 0) == (b >= 0);
}

MouseArea::MouseArea() {
	activatorButtons.clear();
	activatorButtons.insert(sf::Mouse::Left);
}

MouseArea::MouseArea(size_t pointCount) : MouseArea() {
	setPointCount(pointCount);
}

MouseArea::MouseArea(sf::Shape const& shape) : MouseArea() {
	copyShape(shape);
}

void MouseArea::copyShape(sf::Shape const& shape) {
	setPointCount(shape.getPointCount());
	for (size_t i = 0; i < m_points.size(); i++) {
		m_points[i] = shape.getPoint(i);
	}
	setOrigin(shape.getOrigin());
	setPosition(shape.getPosition());
	setRotation(shape.getRotation());
	setScale(shape.getScale());
	
	recalculateCoeffs();
	m_inverseTransform = getTransform().getInverse();
	m_previousTransform = getTransform();
}

void MouseArea::setPointCount(size_t count) {
	if (count == m_points.size())
		return;
	if (count == 0) {
		m_points.clear();
		m_coeffs.clear();
	}
	if (count > m_points.size()) {
		do {
			m_points.push_back(sf::Vector2f());
			m_coeffs.push_back(float());
		} while (count > m_points.size());
	}
	else {
		do {
			m_points.pop_back();
			m_coeffs.pop_back();
		} while (count < m_points.size());
	}
	m_shouldRecalculateCoeffs = true;
}

void MouseArea::setPoint(size_t index, sf::Vector2f const& point) {
	if (index >= m_points.size())
		return;
	
	m_points[index] = point;
	m_shouldRecalculateCoeffs = true;	
}

float getCoeff(sf::Vector2f const& a, sf::Vector2f const& b, sf::Vector2f const& c) {
	return (b.y - a.y)*c.x - (b.x - a.x)*c.y + b.x*a.y - b.y*a.x;
}

void MouseArea::updateFromEvent(sf::Event const& event) {
	if (m_points.size() < 3)
		return;
	
	if (event.type == sf::Event::MouseMoved) {
		updateMouseInside(sf::Vector2i(event.mouseMove.x, event.mouseMove.y));
	}
	else if (event.type == sf::Event::MouseButtonPressed && activatorButtons.find(event.mouseButton.button) != activatorButtons.end()) {
		switch (m_mode) {
		case CLICK: {
			if (m_mouseInside) {
				m_state = SELECTED;
				m_triggered = true;
			}
			break;
		}
		case HOLD: {
			if (m_mouseInside) {
				m_state = ACTIVE;
				m_triggered = true;
			}
			break;
		}
		}		
	}
	else if (event.type == sf::Event::MouseButtonReleased && activatorButtons.find(event.mouseButton.button) != activatorButtons.end()) {
		switch (m_mode) {
		case CLICK: {
			if  (m_mouseInside && m_triggered) {
				m_state = ACTIVE;
			}
			else {
				m_state = NONE;
			}
			m_triggered = false;
			break;
		}
		case HOLD: {
			if (m_mouseInside)
				m_state = HOVERED;
			m_triggered = false;
			break;
		}
		}	
	}
	else {
		switch (m_mode) {
		case CLICK: {
			if (m_mouseInside && m_state == ACTIVE)
				m_state = HOVERED;
			break;
		}
		case HOLD: {
			break;
		}
		}	
	}
}

void MouseArea::updateFromGeometryChange(sf::Window const& relativeTo) {
	if (m_points.size() < 3)
		return;
	
	bool shouldUpdateMouseInside = false;
	
	if (m_shouldRecalculateCoeffs) { //Intrinsic geometry has changed
		recalculateCoeffs();
		shouldUpdateMouseInside = true;
	}
	
	if (getTransform() != m_previousTransform) { //Transformed geometry has changed
		m_inverseTransform = getTransform().getInverse();
		m_previousTransform = getTransform();
		shouldUpdateMouseInside = true;
	}
	
	if (shouldUpdateMouseInside)
		updateMouseInside(sf::Mouse::getPosition(relativeTo));
}

void MouseArea::setMode(Mode mode) {
	m_state = NONE;
	m_triggered = false;
	m_mode = mode;
}

void MouseArea::updateMouseInside(sf::Vector2i const& mousePos) {
	sf::Vector2f transformedPos = m_inverseTransform.transformPoint(static_cast<sf::Vector2f>(mousePos));
	m_mouseInside = true;
	for (size_t i = 0; i < m_coeffs.size(); i++) {
		if (!sameSign(m_coeffs[i], getCoeff(m_points[i], m_points[i+1], transformedPos))) {
			m_mouseInside = false;
			break;
		}
	}
	
	switch (m_mode) {
	case CLICK: {
		if (m_mouseInside) {
			if (m_triggered)
				m_state = SELECTED;
			else
				m_state = HOVERED;
		}	
		else {
			m_state = NONE;
		}
		break;
	}
	case HOLD: {
		if (m_mouseInside) {
			if (m_triggered)
				m_state = ACTIVE;
			else if (m_state != ACTIVE)
				m_state = HOVERED;
		}
		else
			m_state = NONE;
		break;
	}
	}	
}

void MouseArea::recalculateCoeffs() {
	m_center = sf::Vector2f();
	for (auto it = m_points.begin(); it != m_points.end(); it++) {
		m_center += *it;
	}
	m_center /= static_cast<float>(m_points.size());
	for (size_t i = 0; i < m_coeffs.size(); i++) {
		m_coeffs[i] = getCoeff(m_points[i], m_points[i+1], m_center);
	}
	m_shouldRecalculateCoeffs = false;
}
