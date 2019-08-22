#pragma once
#include "../core.h"

#include "Signal.h"

namespace pe
{
	class Object;
	class Scene;

	class PIXEL_ENGINE_API Timer
	{
	public:
		Timer();
		Timer(const std::string& name, float time=0.f, bool loop = false);

		inline void setName(const std::string& name) { m_name = name; }
		inline void setScene(Scene* scene) { m_scene = scene; }
		void start(float time = -1);
		void update();

		void setSignalReciever(Object* obj);
		inline void setLoop(bool loop) { m_loop = loop; }
		inline void setTotalTime(float time) { m_time = time; }

		double getRemainingTime() const;
		inline const std::string& getName() const { return m_name; }
		inline float getTotalTime() const { return m_time; }

	private:

		friend class Scene;
		std::string m_name;
		float m_time = 1.f; // default 1s
		bool m_loop = false;
		sf::Clock m_clock;
		Signal m_signal;
		Scene* m_scene;
		Object* m_signal_reciever;

		bool m_signal_emitted = false;
	};
}