#ifndef CUBIQUITY_BRUSH_H
#define CUBIQUITY_BRUSH_H

namespace Cubiquity
{
	class Brush
	{
	public:
		Brush(float innerRadius, float outerRadius, float opacity);

		float innerRadius(void) const;
		float outerRadius(void) const;
		float opacity(void) const;

		void setInnerRadius(float value);
		void setOuterRadius(float value);
		void setOpacity(float value);

	private:
		float mInnerRadius;
		float mOuterRadius;
		float mOpacity;
	};
}

#endif //CUBIQUITY_BRUSH_H
