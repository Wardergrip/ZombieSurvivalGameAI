#ifndef DAE_REI_HF
#define DAE_REI_HF
//#define SAFE_DELETE(ptr) {if(ptr){delete ptr; ptr = nullptr;}}

namespace HF
{
	inline constexpr float ToRadians(float degrees) noexcept
	{
		return degrees * (static_cast<float>(M_PI) / 180.f);
	}

	inline constexpr float ToDegrees(float radians) noexcept
	{
		return radians * (180.f / static_cast<float>(M_PI));
	}

	inline Elite::Vector2 AngleToLookDirection(float angleInRadians)
	{
		return Elite::Vector2{ std::cosf(angleInRadians),std::sinf(angleInRadians) };
	}
}
#endif