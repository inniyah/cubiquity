/*******************************************************************************
Copyright (c) 2005-2009 David Williams

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not
    claim that you wrote the original software. If you use this software
    in a product, an acknowledgment in the product documentation would be
    appreciated but is not required.

    2. Altered source versions must be plainly marked as such, and must not be
    misrepresented as being the original software.

    3. This notice may not be removed or altered from any source
    distribution. 	
*******************************************************************************/

#define BORDER_LOW(x) ((( x >> this->mVolume->m_uBlockSideLengthPower) << this->mVolume->m_uBlockSideLengthPower) != x)
#define BORDER_HIGH(x) ((( (x+1) >> this->mVolume->m_uBlockSideLengthPower) << this->mVolume->m_uBlockSideLengthPower) != (x+1))
//#define BORDER_LOW(x) (( x % this->mVolume->m_uBlockSideLength) != 0)
//#define BORDER_HIGH(x) (( x % this->mVolume->m_uBlockSideLength) != this->mVolume->m_uBlockSideLength - 1)

namespace PolyVox
{
	/**
	 * \param volume The SimpleVolume you want to sample
	 */
	template <typename VoxelType>
	SimpleVolume<VoxelType>::Sampler::Sampler(SimpleVolume<VoxelType>* volume)
		:BaseVolume<VoxelType>::template Sampler< SimpleVolume<VoxelType> >(volume)
	{
	}

	template <typename VoxelType>
	SimpleVolume<VoxelType>::Sampler::~Sampler()
	{
	}

	template <typename VoxelType>
	VoxelType SimpleVolume<VoxelType>::Sampler::getSubSampledVoxel(uint8_t uLevel) const
	{		
		if(uLevel == 0)
		{
			return getVoxel();
		}
		else if(uLevel == 1)
		{
			VoxelType tValue = getVoxel();
			tValue = (std::min)(tValue, peekVoxel1px0py0pz());
			tValue = (std::min)(tValue, peekVoxel0px1py0pz());
			tValue = (std::min)(tValue, peekVoxel1px1py0pz());
			tValue = (std::min)(tValue, peekVoxel0px0py1pz());
			tValue = (std::min)(tValue, peekVoxel1px0py1pz());
			tValue = (std::min)(tValue, peekVoxel0px1py1pz());
			tValue = (std::min)(tValue, peekVoxel1px1py1pz());
			return tValue;
		}
		else
		{
			const uint8_t uSize = 1 << uLevel;

			VoxelType tValue = (std::numeric_limits<VoxelType>::max)();
			for(uint8_t z = 0; z < uSize; ++z)
			{
				for(uint8_t y = 0; y < uSize; ++y)
				{
					for(uint8_t x = 0; x < uSize; ++x)
					{
						tValue = (std::min)(tValue, this->mVolume->getVoxelAt(this->mXPosInVolume + x, this->mYPosInVolume + y, this->mZPosInVolume + z));
					}
				}
			}
			return tValue;
		}
	}
	
	/**
	 * \return The current voxel
	 */
	template <typename VoxelType>
	VoxelType SimpleVolume<VoxelType>::Sampler::getVoxel(void) const
	{
		if(this->isCurrentPositionValid())
		{
			return *mCurrentVoxel;
		}
		else
		{
			return getVoxelAt(this->mXPosInVolume, this->mYPosInVolume, this->mZPosInVolume);
		}
	}
	
	/**
	 * \param v3dNewPos The position to set to
	 */
	template <typename VoxelType>
	void SimpleVolume<VoxelType>::Sampler::setPosition(const Vector3DInt32& v3dNewPos)
	{
		setPosition(v3dNewPos.getX(), v3dNewPos.getY(), v3dNewPos.getZ());
	}
	
	/**
	 * \param xPos The \a x position to set to
	 * \param yPos The \a y position to set to
	 * \param zPos The \a z position to set to
	 */
	template <typename VoxelType>
	void SimpleVolume<VoxelType>::Sampler::setPosition(int32_t xPos, int32_t yPos, int32_t zPos)
	{
		// Base version updates position and validity flags.
		BaseVolume<VoxelType>::template Sampler< SimpleVolume<VoxelType> >::setPosition(xPos, yPos, zPos);

		// Then we update the voxel pointer
		if(this->isCurrentPositionValid())
		{
			const int32_t uXBlock = this->mXPosInVolume >> this->mVolume->m_uBlockSideLengthPower;
			const int32_t uYBlock = this->mYPosInVolume >> this->mVolume->m_uBlockSideLengthPower;
			const int32_t uZBlock = this->mZPosInVolume >> this->mVolume->m_uBlockSideLengthPower;

			const uint16_t uXPosInBlock = static_cast<uint16_t>(this->mXPosInVolume - (uXBlock << this->mVolume->m_uBlockSideLengthPower));
			const uint16_t uYPosInBlock = static_cast<uint16_t>(this->mYPosInVolume - (uYBlock << this->mVolume->m_uBlockSideLengthPower));
			const uint16_t uZPosInBlock = static_cast<uint16_t>(this->mZPosInVolume - (uZBlock << this->mVolume->m_uBlockSideLengthPower));

			const uint32_t uVoxelIndexInBlock = uXPosInBlock + 
					uYPosInBlock * this->mVolume->m_uBlockSideLength + 
					uZPosInBlock * this->mVolume->m_uBlockSideLength * this->mVolume->m_uBlockSideLength;

			Block* pUncompressedCurrentBlock = this->mVolume->getUncompressedBlock(uXBlock, uYBlock, uZBlock);

			mCurrentVoxel = pUncompressedCurrentBlock->m_tUncompressedData + uVoxelIndexInBlock;
		}
		else
		{
			mCurrentVoxel = 0;
		}
	}
	
	/**
	 * \details
	 * 
	 * This function checks that the current voxel position that you're trying
	 * to set is not outside the volume. If it is, this function returns
	 * \a false, otherwise it will return \a true.
	 * 
	 * \param tValue The value to set to voxel to
	 */
	template <typename VoxelType>
	bool SimpleVolume<VoxelType>::Sampler::setVoxel(VoxelType tValue)
	{
		if(this->m_bIsCurrentPositionValidInX && this->m_bIsCurrentPositionValidInY && this->m_bIsCurrentPositionValidInZ)
		{
			*mCurrentVoxel = tValue;
			return true;
		}
		else
		{
			return false;
		}
	}

	template <typename VoxelType>
	void SimpleVolume<VoxelType>::Sampler::movePositiveX(void)
	{
		// We'll need this in a moment...
		bool bIsOldPositionValid = this->isCurrentPositionValid();

		// Base version updates position and validity flags.
		BaseVolume<VoxelType>::template Sampler< SimpleVolume<VoxelType> >::movePositiveX();

		// Then we update the voxel pointer
		if((this->isCurrentPositionValid()) && bIsOldPositionValid && ((this->mXPosInVolume) % this->mVolume->m_uBlockSideLength != 0))
		{
			//No need to compute new block.
			++mCurrentVoxel;			
		}
		else
		{
			//We've hit the block boundary. Just calling setPosition() is the easiest way to resolve this.
			setPosition(this->mXPosInVolume, this->mYPosInVolume, this->mZPosInVolume);
		}
	}

	template <typename VoxelType>
	void SimpleVolume<VoxelType>::Sampler::movePositiveY(void)
	{
		// We'll need this in a moment...
		bool bIsOldPositionValid = this->isCurrentPositionValid();

		// Base version updates position and validity flags.
		BaseVolume<VoxelType>::template Sampler< SimpleVolume<VoxelType> >::movePositiveY();

		// Then we update the voxel pointer
		if((this->isCurrentPositionValid()) && bIsOldPositionValid && ((this->mYPosInVolume) % this->mVolume->m_uBlockSideLength != 0))
		{
			//No need to compute new block.
			mCurrentVoxel += this->mVolume->m_uBlockSideLength;
		}
		else
		{
			//We've hit the block boundary. Just calling setPosition() is the easiest way to resolve this.
			setPosition(this->mXPosInVolume, this->mYPosInVolume, this->mZPosInVolume);
		}
	}

	template <typename VoxelType>
	void SimpleVolume<VoxelType>::Sampler::movePositiveZ(void)
	{
		// We'll need this in a moment...
		bool bIsOldPositionValid = this->isCurrentPositionValid();

		// Base version updates position and validity flags.
		BaseVolume<VoxelType>::template Sampler< SimpleVolume<VoxelType> >::movePositiveZ();

		// Then we update the voxel pointer
		if((this->isCurrentPositionValid()) && bIsOldPositionValid && ((this->mZPosInVolume) % this->mVolume->m_uBlockSideLength != 0))
		{
			//No need to compute new block.
			mCurrentVoxel += this->mVolume->m_uBlockSideLength * this->mVolume->m_uBlockSideLength;
		}
		else
		{
			//We've hit the block boundary. Just calling setPosition() is the easiest way to resolve this.
			setPosition(this->mXPosInVolume, this->mYPosInVolume, this->mZPosInVolume);
		}
	}

	template <typename VoxelType>
	void SimpleVolume<VoxelType>::Sampler::moveNegativeX(void)
	{
		// We'll need this in a moment...
		bool bIsOldPositionValid = this->isCurrentPositionValid();

		// Base version updates position and validity flags.
		BaseVolume<VoxelType>::template Sampler< SimpleVolume<VoxelType> >::moveNegativeX();

		// Then we update the voxel pointer
		if((this->isCurrentPositionValid()) && bIsOldPositionValid && ((this->mXPosInVolume + 1) % this->mVolume->m_uBlockSideLength != 0))
		{
			//No need to compute new block.
			--mCurrentVoxel;			
		}
		else
		{
			//We've hit the block boundary. Just calling setPosition() is the easiest way to resolve this.
			setPosition(this->mXPosInVolume, this->mYPosInVolume, this->mZPosInVolume);
		}
	}

	template <typename VoxelType>
	void SimpleVolume<VoxelType>::Sampler::moveNegativeY(void)
	{
		// We'll need this in a moment...
		bool bIsOldPositionValid = this->isCurrentPositionValid();

		// Base version updates position and validity flags.
		BaseVolume<VoxelType>::template Sampler< SimpleVolume<VoxelType> >::moveNegativeY();

		// Then we update the voxel pointer
		if((this->isCurrentPositionValid()) && bIsOldPositionValid && ((this->mYPosInVolume + 1) % this->mVolume->m_uBlockSideLength != 0))
		{
			//No need to compute new block.
			mCurrentVoxel -= this->mVolume->m_uBlockSideLength;
		}
		else
		{
			//We've hit the block boundary. Just calling setPosition() is the easiest way to resolve this.
			setPosition(this->mXPosInVolume, this->mYPosInVolume, this->mZPosInVolume);
		}
	}

	template <typename VoxelType>
	void SimpleVolume<VoxelType>::Sampler::moveNegativeZ(void)
	{
		// We'll need this in a moment...
		bool bIsOldPositionValid = this->isCurrentPositionValid();

		// Base version updates position and validity flags.
		BaseVolume<VoxelType>::template Sampler< SimpleVolume<VoxelType> >::moveNegativeZ();

		// Then we update the voxel pointer
		if((this->isCurrentPositionValid()) && bIsOldPositionValid && ((this->mZPosInVolume + 1) % this->mVolume->m_uBlockSideLength != 0))
		{
			//No need to compute new block.
			mCurrentVoxel -= this->mVolume->m_uBlockSideLength * this->mVolume->m_uBlockSideLength;
		}
		else
		{
			//We've hit the block boundary. Just calling setPosition() is the easiest way to resolve this.
			setPosition(this->mXPosInVolume, this->mYPosInVolume, this->mZPosInVolume);
		}
	}

	template <typename VoxelType>
	VoxelType SimpleVolume<VoxelType>::Sampler::peekVoxel1nx1ny1nz(void) const
	{
		if((this->isCurrentPositionValid()) && 	BORDER_LOW(this->mXPosInVolume) && BORDER_LOW(this->mYPosInVolume) && BORDER_LOW(this->mZPosInVolume) )
		{
			return *(mCurrentVoxel - 1 - this->mVolume->m_uBlockSideLength - this->mVolume->m_uBlockSideLength*this->mVolume->m_uBlockSideLength);
		}
		return this->mVolume->getVoxelWithWrapping(this->mXPosInVolume-1,this->mYPosInVolume-1,this->mZPosInVolume-1,this->m_eWrapMode, this->m_tBorder);
	}

	template <typename VoxelType>
	VoxelType SimpleVolume<VoxelType>::Sampler::peekVoxel1nx1ny0pz(void) const
	{
		if((this->isCurrentPositionValid()) && 	BORDER_LOW(this->mXPosInVolume) && BORDER_LOW(this->mYPosInVolume) )
		{
			return *(mCurrentVoxel - 1 - this->mVolume->m_uBlockSideLength);
		}
		return this->mVolume->getVoxelWithWrapping(this->mXPosInVolume-1,this->mYPosInVolume-1,this->mZPosInVolume,this->m_eWrapMode, this->m_tBorder);
	}

	template <typename VoxelType>
	VoxelType SimpleVolume<VoxelType>::Sampler::peekVoxel1nx1ny1pz(void) const
	{
		if((this->isCurrentPositionValid()) && 	BORDER_LOW(this->mXPosInVolume) && BORDER_LOW(this->mYPosInVolume) && BORDER_HIGH(this->mZPosInVolume) )
		{
			return *(mCurrentVoxel - 1 - this->mVolume->m_uBlockSideLength + this->mVolume->m_uBlockSideLength*this->mVolume->m_uBlockSideLength);
		}
		return this->mVolume->getVoxelWithWrapping(this->mXPosInVolume-1,this->mYPosInVolume-1,this->mZPosInVolume+1,this->m_eWrapMode, this->m_tBorder);
	}

	template <typename VoxelType>
	VoxelType SimpleVolume<VoxelType>::Sampler::peekVoxel1nx0py1nz(void) const
	{
		if((this->isCurrentPositionValid()) && 	BORDER_LOW(this->mXPosInVolume) && BORDER_LOW(this->mZPosInVolume) )
		{
			return *(mCurrentVoxel - 1 - this->mVolume->m_uBlockSideLength*this->mVolume->m_uBlockSideLength);
		}
		return this->mVolume->getVoxelWithWrapping(this->mXPosInVolume-1,this->mYPosInVolume,this->mZPosInVolume-1,this->m_eWrapMode, this->m_tBorder);
	}

	template <typename VoxelType>
	VoxelType SimpleVolume<VoxelType>::Sampler::peekVoxel1nx0py0pz(void) const
	{
		if((this->isCurrentPositionValid()) &&  BORDER_LOW(this->mXPosInVolume) )
		{
			return *(mCurrentVoxel - 1);
		}
		return this->mVolume->getVoxelWithWrapping(this->mXPosInVolume-1,this->mYPosInVolume,this->mZPosInVolume,this->m_eWrapMode, this->m_tBorder);
	}

	template <typename VoxelType>
	VoxelType SimpleVolume<VoxelType>::Sampler::peekVoxel1nx0py1pz(void) const
	{
		if((this->isCurrentPositionValid()) &&  BORDER_LOW(this->mXPosInVolume) && BORDER_HIGH(this->mZPosInVolume) )
		{
			return *(mCurrentVoxel - 1 + this->mVolume->m_uBlockSideLength*this->mVolume->m_uBlockSideLength);
		}
		return this->mVolume->getVoxelWithWrapping(this->mXPosInVolume-1,this->mYPosInVolume,this->mZPosInVolume+1,this->m_eWrapMode, this->m_tBorder);
	}

	template <typename VoxelType>
	VoxelType SimpleVolume<VoxelType>::Sampler::peekVoxel1nx1py1nz(void) const
	{
		if((this->isCurrentPositionValid()) &&  BORDER_LOW(this->mXPosInVolume) && BORDER_HIGH(this->mYPosInVolume) && BORDER_LOW(this->mZPosInVolume) )
		{
			return *(mCurrentVoxel - 1 + this->mVolume->m_uBlockSideLength - this->mVolume->m_uBlockSideLength*this->mVolume->m_uBlockSideLength);
		}
		return this->mVolume->getVoxelWithWrapping(this->mXPosInVolume-1,this->mYPosInVolume+1,this->mZPosInVolume-1,this->m_eWrapMode, this->m_tBorder);
	}

	template <typename VoxelType>
	VoxelType SimpleVolume<VoxelType>::Sampler::peekVoxel1nx1py0pz(void) const
	{
		if((this->isCurrentPositionValid()) &&  BORDER_LOW(this->mXPosInVolume) && BORDER_HIGH(this->mYPosInVolume) )
		{
			return *(mCurrentVoxel - 1 + this->mVolume->m_uBlockSideLength);
		}
		return this->mVolume->getVoxelWithWrapping(this->mXPosInVolume-1,this->mYPosInVolume+1,this->mZPosInVolume,this->m_eWrapMode, this->m_tBorder);
	}

	template <typename VoxelType>
	VoxelType SimpleVolume<VoxelType>::Sampler::peekVoxel1nx1py1pz(void) const
	{
		if((this->isCurrentPositionValid()) &&  BORDER_LOW(this->mXPosInVolume) && BORDER_HIGH(this->mYPosInVolume) && BORDER_HIGH(this->mZPosInVolume) )
		{
			return *(mCurrentVoxel - 1 + this->mVolume->m_uBlockSideLength + this->mVolume->m_uBlockSideLength*this->mVolume->m_uBlockSideLength);
		}
		return this->mVolume->getVoxelWithWrapping(this->mXPosInVolume-1,this->mYPosInVolume+1,this->mZPosInVolume+1,this->m_eWrapMode, this->m_tBorder);
	}

	//////////////////////////////////////////////////////////////////////////

	template <typename VoxelType>
	VoxelType SimpleVolume<VoxelType>::Sampler::peekVoxel0px1ny1nz(void) const
	{
		if((this->isCurrentPositionValid()) &&  BORDER_LOW(this->mYPosInVolume) && BORDER_LOW(this->mZPosInVolume) )
		{
			return *(mCurrentVoxel - this->mVolume->m_uBlockSideLength - this->mVolume->m_uBlockSideLength*this->mVolume->m_uBlockSideLength);
		}
		return this->mVolume->getVoxelWithWrapping(this->mXPosInVolume,this->mYPosInVolume-1,this->mZPosInVolume-1,this->m_eWrapMode, this->m_tBorder);
	}

	template <typename VoxelType>
	VoxelType SimpleVolume<VoxelType>::Sampler::peekVoxel0px1ny0pz(void) const
	{
		if((this->isCurrentPositionValid()) &&  BORDER_LOW(this->mYPosInVolume) )
		{
			return *(mCurrentVoxel - this->mVolume->m_uBlockSideLength);
		}
		return this->mVolume->getVoxelWithWrapping(this->mXPosInVolume,this->mYPosInVolume-1,this->mZPosInVolume,this->m_eWrapMode, this->m_tBorder);
	}

	template <typename VoxelType>
	VoxelType SimpleVolume<VoxelType>::Sampler::peekVoxel0px1ny1pz(void) const
	{
		if((this->isCurrentPositionValid()) &&  BORDER_LOW(this->mYPosInVolume) && BORDER_HIGH(this->mZPosInVolume) )
		{
			return *(mCurrentVoxel - this->mVolume->m_uBlockSideLength + this->mVolume->m_uBlockSideLength*this->mVolume->m_uBlockSideLength);
		}
		return this->mVolume->getVoxelWithWrapping(this->mXPosInVolume,this->mYPosInVolume-1,this->mZPosInVolume+1,this->m_eWrapMode, this->m_tBorder);
	}

	template <typename VoxelType>
	VoxelType SimpleVolume<VoxelType>::Sampler::peekVoxel0px0py1nz(void) const
	{
		if((this->isCurrentPositionValid()) &&  BORDER_LOW(this->mZPosInVolume) )
		{
			return *(mCurrentVoxel - this->mVolume->m_uBlockSideLength*this->mVolume->m_uBlockSideLength);
		}
		return this->mVolume->getVoxelWithWrapping(this->mXPosInVolume,this->mYPosInVolume,this->mZPosInVolume-1,this->m_eWrapMode, this->m_tBorder);
	}

	template <typename VoxelType>
	VoxelType SimpleVolume<VoxelType>::Sampler::peekVoxel0px0py0pz(void) const
	{
		if((this->isCurrentPositionValid()))
		{
			return *mCurrentVoxel;
		}
		return this->mVolume->getVoxelWithWrapping(this->mXPosInVolume, this->mYPosInVolume, this->mZPosInVolume,this->m_eWrapMode, this->m_tBorder);
	}

	template <typename VoxelType>
	VoxelType SimpleVolume<VoxelType>::Sampler::peekVoxel0px0py1pz(void) const
	{
		if((this->isCurrentPositionValid()) && BORDER_HIGH(this->mZPosInVolume) )
		{
			return *(mCurrentVoxel + this->mVolume->m_uBlockSideLength*this->mVolume->m_uBlockSideLength);
		}
		return this->mVolume->getVoxelWithWrapping(this->mXPosInVolume,this->mYPosInVolume,this->mZPosInVolume+1,this->m_eWrapMode, this->m_tBorder);
	}

	template <typename VoxelType>
	VoxelType SimpleVolume<VoxelType>::Sampler::peekVoxel0px1py1nz(void) const
	{
		if((this->isCurrentPositionValid()) && BORDER_HIGH(this->mYPosInVolume) && BORDER_LOW(this->mZPosInVolume) )
		{
			return *(mCurrentVoxel + this->mVolume->m_uBlockSideLength - this->mVolume->m_uBlockSideLength*this->mVolume->m_uBlockSideLength);
		}
		return this->mVolume->getVoxelWithWrapping(this->mXPosInVolume,this->mYPosInVolume+1,this->mZPosInVolume-1,this->m_eWrapMode, this->m_tBorder);
	}

	template <typename VoxelType>
	VoxelType SimpleVolume<VoxelType>::Sampler::peekVoxel0px1py0pz(void) const
	{
		if((this->isCurrentPositionValid()) && BORDER_HIGH(this->mYPosInVolume) )
		{
			return *(mCurrentVoxel + this->mVolume->m_uBlockSideLength);
		}
		return this->mVolume->getVoxelWithWrapping(this->mXPosInVolume,this->mYPosInVolume+1,this->mZPosInVolume,this->m_eWrapMode, this->m_tBorder);
	}

	template <typename VoxelType>
	VoxelType SimpleVolume<VoxelType>::Sampler::peekVoxel0px1py1pz(void) const
	{
		if((this->isCurrentPositionValid()) && BORDER_HIGH(this->mYPosInVolume) && BORDER_HIGH(this->mZPosInVolume) )
		{
			return *(mCurrentVoxel + this->mVolume->m_uBlockSideLength + this->mVolume->m_uBlockSideLength*this->mVolume->m_uBlockSideLength);
		}
		return this->mVolume->getVoxelWithWrapping(this->mXPosInVolume,this->mYPosInVolume+1,this->mZPosInVolume+1,this->m_eWrapMode, this->m_tBorder);
	}

	//////////////////////////////////////////////////////////////////////////

	template <typename VoxelType>
	VoxelType SimpleVolume<VoxelType>::Sampler::peekVoxel1px1ny1nz(void) const
	{
		if((this->isCurrentPositionValid()) && BORDER_HIGH(this->mXPosInVolume) && BORDER_LOW(this->mYPosInVolume) && BORDER_LOW(this->mZPosInVolume) )
		{
			return *(mCurrentVoxel + 1 - this->mVolume->m_uBlockSideLength - this->mVolume->m_uBlockSideLength*this->mVolume->m_uBlockSideLength);
		}
		return this->mVolume->getVoxelWithWrapping(this->mXPosInVolume+1,this->mYPosInVolume-1,this->mZPosInVolume-1,this->m_eWrapMode, this->m_tBorder);
	}

	template <typename VoxelType>
	VoxelType SimpleVolume<VoxelType>::Sampler::peekVoxel1px1ny0pz(void) const
	{
		if((this->isCurrentPositionValid()) && BORDER_HIGH(this->mXPosInVolume) && BORDER_LOW(this->mYPosInVolume) )
		{
			return *(mCurrentVoxel + 1 - this->mVolume->m_uBlockSideLength);
		}
		return this->mVolume->getVoxelWithWrapping(this->mXPosInVolume+1,this->mYPosInVolume-1,this->mZPosInVolume,this->m_eWrapMode, this->m_tBorder);
	}

	template <typename VoxelType>
	VoxelType SimpleVolume<VoxelType>::Sampler::peekVoxel1px1ny1pz(void) const
	{
		if((this->isCurrentPositionValid()) && BORDER_HIGH(this->mXPosInVolume) && BORDER_LOW(this->mYPosInVolume) && BORDER_HIGH(this->mZPosInVolume) )
		{
			return *(mCurrentVoxel + 1 - this->mVolume->m_uBlockSideLength + this->mVolume->m_uBlockSideLength*this->mVolume->m_uBlockSideLength);
		}
		return this->mVolume->getVoxelWithWrapping(this->mXPosInVolume+1,this->mYPosInVolume-1,this->mZPosInVolume+1,this->m_eWrapMode, this->m_tBorder);
	}

	template <typename VoxelType>
	VoxelType SimpleVolume<VoxelType>::Sampler::peekVoxel1px0py1nz(void) const
	{
		if((this->isCurrentPositionValid()) && BORDER_HIGH(this->mXPosInVolume) && BORDER_LOW(this->mZPosInVolume) )
		{
			return *(mCurrentVoxel + 1 - this->mVolume->m_uBlockSideLength*this->mVolume->m_uBlockSideLength);
		}
		return this->mVolume->getVoxelWithWrapping(this->mXPosInVolume+1,this->mYPosInVolume,this->mZPosInVolume-1,this->m_eWrapMode, this->m_tBorder);
	}

	template <typename VoxelType>
	VoxelType SimpleVolume<VoxelType>::Sampler::peekVoxel1px0py0pz(void) const
	{
		if((this->isCurrentPositionValid()) && BORDER_HIGH(this->mXPosInVolume) )
		{
			return *(mCurrentVoxel + 1);
		}
		return this->mVolume->getVoxelWithWrapping(this->mXPosInVolume+1,this->mYPosInVolume,this->mZPosInVolume,this->m_eWrapMode, this->m_tBorder);
	}

	template <typename VoxelType>
	VoxelType SimpleVolume<VoxelType>::Sampler::peekVoxel1px0py1pz(void) const
	{
		if((this->isCurrentPositionValid()) && BORDER_HIGH(this->mXPosInVolume) && BORDER_HIGH(this->mZPosInVolume) )
		{
			return *(mCurrentVoxel + 1 + this->mVolume->m_uBlockSideLength*this->mVolume->m_uBlockSideLength);
		}
		return this->mVolume->getVoxelWithWrapping(this->mXPosInVolume+1,this->mYPosInVolume,this->mZPosInVolume+1,this->m_eWrapMode, this->m_tBorder);
	}

	template <typename VoxelType>
	VoxelType SimpleVolume<VoxelType>::Sampler::peekVoxel1px1py1nz(void) const
	{
		if((this->isCurrentPositionValid()) && BORDER_HIGH(this->mXPosInVolume) && BORDER_HIGH(this->mYPosInVolume) && BORDER_LOW(this->mZPosInVolume) )
		{
			return *(mCurrentVoxel + 1 + this->mVolume->m_uBlockSideLength - this->mVolume->m_uBlockSideLength*this->mVolume->m_uBlockSideLength);
		}
		return this->mVolume->getVoxelWithWrapping(this->mXPosInVolume+1,this->mYPosInVolume+1,this->mZPosInVolume-1,this->m_eWrapMode, this->m_tBorder);
	}

	template <typename VoxelType>
	VoxelType SimpleVolume<VoxelType>::Sampler::peekVoxel1px1py0pz(void) const
	{
		if((this->isCurrentPositionValid()) && BORDER_HIGH(this->mXPosInVolume) && BORDER_HIGH(this->mYPosInVolume) )
		{
			return *(mCurrentVoxel + 1 + this->mVolume->m_uBlockSideLength);
		}
		return this->mVolume->getVoxelWithWrapping(this->mXPosInVolume+1,this->mYPosInVolume+1,this->mZPosInVolume,this->m_eWrapMode, this->m_tBorder);
	}

	template <typename VoxelType>
	VoxelType SimpleVolume<VoxelType>::Sampler::peekVoxel1px1py1pz(void) const
	{
		if((this->isCurrentPositionValid()) && BORDER_HIGH(this->mXPosInVolume) && BORDER_HIGH(this->mYPosInVolume) && BORDER_HIGH(this->mZPosInVolume) )
		{
			return *(mCurrentVoxel + 1 + this->mVolume->m_uBlockSideLength + this->mVolume->m_uBlockSideLength*this->mVolume->m_uBlockSideLength);
		}
		return this->mVolume->getVoxelWithWrapping(this->mXPosInVolume+1,this->mYPosInVolume+1,this->mZPosInVolume+1,this->m_eWrapMode, this->m_tBorder);
	}
}

#undef BORDER_LOW
#undef BORDER_HIGH
