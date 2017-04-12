#pragma once

struct AppTime
{
public:
	AppTime(uint64_t ticksPerSecond) :
		_ticksPerSecond(ticksPerSecond),
		_elapsedTicks(0),
		_totalTicks(0)
	{}

	uint64_t GetElapsedTicks() const { return _elapsedTicks; }
	double GetElapsedSeconds() const { return TicksToSeconds(GetElapsedTicks()); }
	uint64_t GetTotalTicks() const { return _totalTicks; }
	double GetTotalSeconds() const { return TicksToSeconds(GetTotalTicks()); }

	void SetElapsedTicks(uint64_t elapsedTicks) { _elapsedTicks = elapsedTicks; }
	void SetTotalTicks(uint64_t totalTicks) { _totalTicks = totalTicks; }

	double TicksToSeconds(uint64_t ticks) const { return ((double)(ticks)) / _ticksPerSecond; }
	uint64_t SecondsToTicks(double secs) const { return (uint64_t)(secs * _ticksPerSecond); }
private:
	uint64_t _ticksPerSecond;
	// ticks since last frame
	uint64_t _elapsedTicks;
	// ticks since start of the app
	uint64_t _totalTicks;
};
