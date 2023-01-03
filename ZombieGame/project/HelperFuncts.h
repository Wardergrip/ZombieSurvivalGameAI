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

	inline int GetIndex(int rowIdx, int colIdx, int nrCols)
	{
		return (rowIdx * nrCols + colIdx);
	}

	// First = row, Second = col
	inline std::pair<int, int> GetRowColFromIndex(int idx, int divisions)
	{
		return std::make_pair(idx / divisions, idx % divisions);
	}

	inline bool IsPointInRect(const Elite::Vector2& topLeft, float width, float height, const Elite::Vector2& point)
	{
		return point.x >= topLeft.x && point.x < topLeft.x + width && point.y >= topLeft.y && point.y < topLeft.y + height;
	}
}
#endif