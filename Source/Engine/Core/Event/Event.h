#pragma once
#include "Core.h"
#include "vec2.hpp"

namespace ST {

enum class EventType {
	None=0,
	MouseMoved, MouseScrolled, MousePressed, MouseReleased,
	KeyboardPressed, KeyboardReleased, KeyboardTyped,
	WindowClose, WindowFocus, WindowLostFocus, WindowMoved, WindowResize,
};

#define DEFINE_DEFAULT_EVENT_CONSTRUCT(TYPE) \
    TYPE()

struct Event {
public:
	constexpr Event(EventType type): _type(type) {}

	static constexpr bool IsEventSameType(const Event& e1, const Event& e2) { return e1._type == e2._type; }

	EventType GetType() const { return _type; }

protected:
	const EventType _type;
};

struct MouseButtonPressedEvent : public Event {
public:
	MouseButtonPressedEvent(): Event(EventType::MousePressed) {}

	constexpr MouseButtonPressedEvent(int button): Event(EventType::MousePressed), _button(button) {}

	constexpr int GetButton() const { return _button; }

protected:
	int _button;
};

struct MouseButtonReleasedEvent : public Event {
public:
	MouseButtonReleasedEvent(): Event(EventType::MouseReleased) {}

	constexpr MouseButtonReleasedEvent(int button): Event(EventType::MouseReleased), _button(button) {}

	constexpr int GetButton() { return _button; }

protected:
	int _button;
};

struct MouseMovedEvent : public Event {
public:
	MouseMovedEvent(): Event(EventType::MouseMoved) {}

	constexpr MouseMovedEvent(double xPos, double yPos): Event(EventType::MouseMoved), _xPos(xPos), _yPos(yPos) {}

	glm::vec2 GetMousePos() const { return {_xPos, _yPos}; }

protected:
	double _xPos;

	double _yPos;
};

struct MouseScrollChangedEvent : public Event {
public:
	MouseScrollChangedEvent(): Event(EventType::MouseScrolled) {}

	constexpr MouseScrollChangedEvent(double xOffset, double yOffset): Event(EventType::MouseScrolled),
		_xOffset(xOffset), _yOffset(yOffset) {}

	glm::vec2 GetMouseScrollOffset() const { return {_xOffset, _yOffset}; }

protected:
	double _xOffset;

	double _yOffset;
};

struct WindowClosedEvent : public Event {
public:
	constexpr WindowClosedEvent(): Event(EventType::WindowClose) {}
};

struct WindowResizedEvent : public Event {
public:
	WindowResizedEvent(): Event(EventType::WindowResize) {}

	constexpr WindowResizedEvent(double xSize, double ySize): Event(EventType::WindowResize), _xSize(xSize),
		_ySize(ySize) {}

	glm::vec2 GetWindowSize() const { return {_xSize, _ySize}; }

protected:
	double _xSize;

	double _ySize;
};

#define EVENT_IS_TYPE(event,type) \
Event::IsEventSameType(Event(EventType::##type),event)

class EventDisPatcher {
public:
	template <class T>
	bool Dispatch(const Event& e, ST_FUNC<bool(const T&)> func);
};

template <class T>
bool EventDisPatcher::Dispatch(const Event& e, ST_FUNC<bool(const T&)> func) {
	if (Event::IsEventSameType(e, T())) { return func(*(T*)(&e)); }
	return false;
}

}
