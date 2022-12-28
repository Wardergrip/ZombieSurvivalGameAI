#ifndef TIMER
#define TIMER
struct Timer final
{
public:
	Timer(float timeGoal = 0, bool enable = true)
		: m_TimeGoal{ timeGoal }
		, m_AccumulateSeconds{ 0.f }
		, m_IsTimerDone{ false }
		, m_IsEnabled{enable}
	{
	}

	void Update(float elapsedSec)
	{
		if (m_IsEnabled == false) return;

		if (m_IsTimerDone == false)
		{
			m_AccumulateSeconds += elapsedSec;
			if (m_AccumulateSeconds >= m_TimeGoal)
			{
				m_IsTimerDone = true;
			}
		}
	}

	void SetTimeGoal(float seconds)
	{
		m_TimeGoal = seconds;
	}
	void Enable()
	{
		m_IsEnabled = true;
	}
	void Disable()
	{
		m_IsEnabled = false;
	}
	void ResetTimer()
	{
		m_AccumulateSeconds = 0;
		m_IsTimerDone = false;
	}

	float GetTime() const
	{
		return m_AccumulateSeconds;
	}
	const float& GetTimeRef() const
	{
		return m_AccumulateSeconds;
	}
	float GetPercentageDone() const
	{
		if (m_IsTimerDone)
		{
			return 1.00f;
		}
		return (m_AccumulateSeconds / m_TimeGoal);
	}

	bool IsDone() const
	{
		return m_IsTimerDone;
	}
	bool IsEnabled() const
	{
		return m_IsEnabled;
	}

private:
	float m_TimeGoal;
	float m_AccumulateSeconds;

	bool m_IsTimerDone;
	bool m_IsEnabled;
};
#endif
