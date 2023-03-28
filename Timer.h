#pragma once

#include <iostream>
#include <string>
#include <chrono>
#include "Log.h"

	class Timer
	{
	public:
		Timer() noexcept
		{
			Reset();
		}

		void Reset() noexcept
		{
			m_Start = std::chrono::high_resolution_clock::now();
		}

		float Elapsed() noexcept
		{
			return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - m_Start).count() * 0.001f * 0.001f * 0.001f;
		}

		float ElapsedMillis()
		{
			return Elapsed() * 1000.0f;
		}

	private:
		std::chrono::time_point<std::chrono::high_resolution_clock> m_Start;
	};

	class ScopedTimer
	{
	public:
		ScopedTimer(const std::string& name)
			: m_Name(name) {}
		~ScopedTimer()
		{
			ML_TRACE("[TIMER] {} {}ms\n", m_Name, m_Timer.ElapsedMillis());
		}
	private:
		std::string m_Name;
		Timer m_Timer;
	};