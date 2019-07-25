#pragma once

#include "Core.h"
#include "Input/Event.h"


namespace pe {
	class Application;
	class Input;
}


namespace pe
{
	class PIXEL_ENGINE_API Window
	{
	public:
		struct Prop
		{
			std::string title;
			vec2 size;
			vec2 position;
		};
		virtual void Init() = 0;
		
		// getters
		virtual vec2 getSize() const = 0;
		virtual vec2 getPosition() const = 0;
		virtual std::string getTitle() const = 0;
		virtual bool isOpen() const = 0;
		virtual bool pollEvent(std::unique_ptr<Event>& event) = 0;

		// setters
		virtual void setPosition(const vec2& pos) = 0;

	protected:
		Prop m_prop;
		static bool s_is_window_created;

	private:
		friend Application;
		static std::shared_ptr<Window> create(const Window::Prop& prop = { "Pixel-Engine", vec2(640, 480), vec2(-1,-1) });
	};
}