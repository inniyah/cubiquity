#include "Brush.h"

namespace Cubiquity
{
	Brush::Brush(float innerRadius, float outerRadius, float opacity)
		:mInnerRadius(innerRadius)
		,mOuterRadius(outerRadius)
		,mOpacity(opacity)
	{
	}

	float Brush::innerRadius(void) const
	{
		return mInnerRadius;
	}

	float Brush::outerRadius(void) const
	{
		return mOuterRadius;
	}

	float Brush::opacity(void) const
	{
		return mOpacity;
	}

	void Brush::setInnerRadius(float value)
	{
		mInnerRadius = value;
	}

	void Brush::setOuterRadius(float value)
	{
		mOuterRadius = value;
	}

	void Brush::setOpacity(float value)
	{
		mOpacity = value;
	}
}
