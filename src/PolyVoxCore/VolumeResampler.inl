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

#include "PolyVoxImpl/Utility.h"

namespace PolyVox
{
	template< typename SrcVolumeType, typename DstVolumeType>
	VolumeResampler<SrcVolumeType, DstVolumeType>::VolumeResampler(SrcVolumeType* pVolSrc, Region regSrc, DstVolumeType* pVolDst, Region regDst)
		:m_pVolSrc(pVolSrc)
		,m_regSrc(regSrc)
		,m_pVolDst(pVolDst)
		,m_regDst(regDst)
	{
	}

	template< typename SrcVolumeType, typename DstVolumeType>
	void VolumeResampler<SrcVolumeType, DstVolumeType>::execute()
	{
		int32_t uSrcWidth = m_regSrc.getUpperCorner().getX() - m_regSrc.getLowerCorner().getX() + 1;
		int32_t uSrcHeight = m_regSrc.getUpperCorner().getY() - m_regSrc.getLowerCorner().getY() + 1;
		int32_t uSrcDepth = m_regSrc.getUpperCorner().getZ() - m_regSrc.getLowerCorner().getZ() + 1;

		int32_t uDstWidth = m_regDst.getUpperCorner().getX() - m_regDst.getLowerCorner().getX() + 1;
		int32_t uDstHeight = m_regDst.getUpperCorner().getY() - m_regDst.getLowerCorner().getY() + 1;
		int32_t uDstDepth = m_regDst.getUpperCorner().getZ() - m_regDst.getLowerCorner().getZ() + 1;

		if((uSrcWidth == uDstWidth) && (uSrcHeight == uDstHeight) && (uSrcDepth == uDstDepth))
		{
			resampleSameSize();
		}
		else if((uSrcWidth == uDstWidth * 2 - 1) && (uSrcHeight == uDstHeight * 2 - 1) && (uSrcDepth == uDstDepth * 2 - 1))
		{
			resampleJustOverHalfSize();
		}
		else
		{
			resampleArbitrary();
		}
	}

	template< typename SrcVolumeType, typename DstVolumeType>
	void VolumeResampler<SrcVolumeType, DstVolumeType>::resampleSameSize()
	{
		for(int32_t sz = m_regSrc.getLowerCorner().getZ(), dz = m_regDst.getLowerCorner().getZ(); dz <= m_regDst.getUpperCorner().getZ(); sz++, dz++)
		{
			for(int32_t sy = m_regSrc.getLowerCorner().getY(), dy = m_regDst.getLowerCorner().getY(); dy <= m_regDst.getUpperCorner().getY(); sy++, dy++)
			{
				for(int32_t sx = m_regSrc.getLowerCorner().getX(), dx = m_regDst.getLowerCorner().getX(); dx <= m_regDst.getUpperCorner().getX(); sx++,dx++)
				{
					const typename SrcVolumeType::VoxelType& tSrcVoxel = m_pVolSrc->getVoxelAt(sx,sy,sz);
					const typename DstVolumeType::VoxelType& tDstVoxel = static_cast<typename DstVolumeType::VoxelType>(tSrcVoxel);
					m_pVolDst->setVoxelAt(dx,dy,dz,tDstVoxel);
				}
			}
		}
	}

	template< typename SrcVolumeType, typename DstVolumeType>
	void VolumeResampler<SrcVolumeType, DstVolumeType>::resampleJustOverHalfSize()
	{
		typename SrcVolumeType::Sampler srcSampler(m_pVolSrc);

		for(int32_t dz = m_regDst.getLowerCorner().getZ(); dz <= m_regDst.getUpperCorner().getZ(); dz++)
		{
			for(int32_t dy = m_regDst.getLowerCorner().getY(); dy <= m_regDst.getUpperCorner().getY(); dy++)
			{
				for(int32_t dx = m_regDst.getLowerCorner().getX(); dx <= m_regDst.getUpperCorner().getX(); dx++)
				{
					int32_t sx = (dx - m_regDst.getLowerCorner().getX()) * 2;
					int32_t sy = (dy - m_regDst.getLowerCorner().getY()) * 2;
					int32_t sz = (dz - m_regDst.getLowerCorner().getZ()) * 2;

					sx += m_regSrc.getLowerCorner().getX();
					sy += m_regSrc.getLowerCorner().getY();
					sz += m_regSrc.getLowerCorner().getZ();

					typename SrcVolumeType::VoxelType result;

					typedef Vector<4, float> AccumulationType;
					AccumulationType tSrcVoxel(0);

					for(int32_t iOffsetZ = -1; iOffsetZ <=1; iOffsetZ++)
					{
						for(int32_t iOffsetY = -1; iOffsetY <=1; iOffsetY++)
						{
							for(int32_t iOffsetX = -1; iOffsetX <=1; iOffsetX++)
							{
								int32_t x = sx + iOffsetX;
								int32_t y = sy + iOffsetY;
								int32_t z = sz + iOffsetZ;

								// This effectively does clamping to prevent sampling outside the source volume.
								// It would probably be better if wrap modes could be set on samplers (rather than
								// volumes) so that we could use the peekXXX() functions here.
								x = min(x, m_regSrc.getUpperCorner().getX());
								y = min(y, m_regSrc.getUpperCorner().getY());
								z = min(z, m_regSrc.getUpperCorner().getZ());

								x = max(x, m_regSrc.getLowerCorner().getX());
								y = max(y, m_regSrc.getLowerCorner().getY());
								z = max(z, m_regSrc.getLowerCorner().getZ());

								tSrcVoxel += static_cast<AccumulationType>(m_pVolSrc->getVoxelAt(x, y, z));
							}
						}
					}

					tSrcVoxel /= 27;

					result = static_cast<typename SrcVolumeType::VoxelType>(tSrcVoxel);

					/*srcSampler.setPosition(sx,sy,sz);
					if((dx > m_regDst.getLowerCorner().getX()) && (dx < m_regDst.getUpperCorner().getX()) && (dy > m_regDst.getLowerCorner().getY()) && (dy < m_regDst.getUpperCorner().getY()) && (dz > m_regDst.getLowerCorner().getZ()) && (dz < m_regDst.getUpperCorner().getZ()))
					{
						typedef Vector<4, float> AccumulationType;
						AccumulationType tSrcVoxel(0);

						tSrcVoxel += static_cast<AccumulationType>(srcSampler.peekVoxel1nx1ny1nz());
						tSrcVoxel += static_cast<AccumulationType>(srcSampler.peekVoxel1nx1ny0pz());
						tSrcVoxel += static_cast<AccumulationType>(srcSampler.peekVoxel1nx1ny1pz());
						tSrcVoxel += static_cast<AccumulationType>(srcSampler.peekVoxel1nx0py1nz());
						tSrcVoxel += static_cast<AccumulationType>(srcSampler.peekVoxel1nx0py0pz());
						tSrcVoxel += static_cast<AccumulationType>(srcSampler.peekVoxel1nx0py1pz());
						tSrcVoxel += static_cast<AccumulationType>(srcSampler.peekVoxel1nx1py1nz());
						tSrcVoxel += static_cast<AccumulationType>(srcSampler.peekVoxel1nx1py0pz());
						tSrcVoxel += static_cast<AccumulationType>(srcSampler.peekVoxel1nx1py1pz());

						tSrcVoxel += static_cast<AccumulationType>(srcSampler.peekVoxel0px1ny1nz());
						tSrcVoxel += static_cast<AccumulationType>(srcSampler.peekVoxel0px1ny0pz());
						tSrcVoxel += static_cast<AccumulationType>(srcSampler.peekVoxel0px1ny1pz());
						tSrcVoxel += static_cast<AccumulationType>(srcSampler.peekVoxel0px0py1nz());
						tSrcVoxel += static_cast<AccumulationType>(srcSampler.peekVoxel0px0py0pz());
						tSrcVoxel += static_cast<AccumulationType>(srcSampler.peekVoxel0px0py1pz());
						tSrcVoxel += static_cast<AccumulationType>(srcSampler.peekVoxel0px1py1nz());
						tSrcVoxel += static_cast<AccumulationType>(srcSampler.peekVoxel0px1py0pz());
						tSrcVoxel += static_cast<AccumulationType>(srcSampler.peekVoxel0px1py1pz());

						tSrcVoxel += static_cast<AccumulationType>(srcSampler.peekVoxel1px1ny1nz());
						tSrcVoxel += static_cast<AccumulationType>(srcSampler.peekVoxel1px1ny0pz());
						tSrcVoxel += static_cast<AccumulationType>(srcSampler.peekVoxel1px1ny1pz());
						tSrcVoxel += static_cast<AccumulationType>(srcSampler.peekVoxel1px0py1nz());
						tSrcVoxel += static_cast<AccumulationType>(srcSampler.peekVoxel1px0py0pz());
						tSrcVoxel += static_cast<AccumulationType>(srcSampler.peekVoxel1px0py1pz());
						tSrcVoxel += static_cast<AccumulationType>(srcSampler.peekVoxel1px1py1nz());
						tSrcVoxel += static_cast<AccumulationType>(srcSampler.peekVoxel1px1py0pz());
						tSrcVoxel += static_cast<AccumulationType>(srcSampler.peekVoxel1px1py1pz());

						tSrcVoxel /= 27;

						result = static_cast<typename SrcVolumeType::VoxelType>(tSrcVoxel);
					}
					else
					{
						const typename SrcVolumeType::VoxelType& voxel000 = srcSampler.peekVoxel0px0py0pz();
						result = voxel000;
					}*/
					
					m_pVolDst->setVoxelAt(dx,dy,dz,result);
				}
			}
		}
	}

	template< typename SrcVolumeType, typename DstVolumeType>
	void VolumeResampler<SrcVolumeType, DstVolumeType>::resampleArbitrary()
	{
		float srcWidth  = m_regSrc.getUpperCorner().getX() - m_regSrc.getLowerCorner().getX();
		float srcHeight = m_regSrc.getUpperCorner().getY() - m_regSrc.getLowerCorner().getY();
		float srcDepth  = m_regSrc.getUpperCorner().getZ() - m_regSrc.getLowerCorner().getZ();

		float dstWidth  = m_regDst.getUpperCorner().getX() - m_regDst.getLowerCorner().getX();
		float dstHeight = m_regDst.getUpperCorner().getY() - m_regDst.getLowerCorner().getY();
		float dstDepth  = m_regDst.getUpperCorner().getZ() - m_regDst.getLowerCorner().getZ();

		float fScaleX = srcWidth / dstWidth;
		float fScaleY = srcHeight / dstHeight;
		float fScaleZ = srcDepth / dstDepth;

		typename SrcVolumeType::Sampler sampler(m_pVolSrc);

		for(int32_t dz = m_regDst.getLowerCorner().getZ(); dz <= m_regDst.getUpperCorner().getZ(); dz++)
		{
			for(int32_t dy = m_regDst.getLowerCorner().getY(); dy <= m_regDst.getUpperCorner().getY(); dy++)
			{
				for(int32_t dx = m_regDst.getLowerCorner().getX(); dx <= m_regDst.getUpperCorner().getX(); dx++)
				{
					float sx = (dx - m_regDst.getLowerCorner().getX()) * fScaleX;
					float sy = (dy - m_regDst.getLowerCorner().getY()) * fScaleY;
					float sz = (dz - m_regDst.getLowerCorner().getZ()) * fScaleZ;

					sx += m_regSrc.getLowerCorner().getX();
					sy += m_regSrc.getLowerCorner().getY();
					sz += m_regSrc.getLowerCorner().getZ();

					sx = floor(sx);
					sy = floor(sy);
					sz = floor(sz);

					int32_t iSx = roundToInteger(sx);
					int32_t iSy = roundToInteger(sy);
					int32_t iSz = roundToInteger(sz);

					sampler.setPosition(iSx,iSy,iSz);
					const typename SrcVolumeType::VoxelType& voxel000 = sampler.peekVoxel0px0py0pz();
					const typename SrcVolumeType::VoxelType& voxel001 = sampler.peekVoxel0px0py1pz();
					const typename SrcVolumeType::VoxelType& voxel010 = sampler.peekVoxel0px1py0pz();
					const typename SrcVolumeType::VoxelType& voxel011 = sampler.peekVoxel0px1py1pz();
					const typename SrcVolumeType::VoxelType& voxel100 = sampler.peekVoxel1px0py0pz();
					const typename SrcVolumeType::VoxelType& voxel101 = sampler.peekVoxel1px0py1pz();
					const typename SrcVolumeType::VoxelType& voxel110 = sampler.peekVoxel1px1py0pz();
					const typename SrcVolumeType::VoxelType& voxel111 = sampler.peekVoxel1px1py1pz();

					//FIXME - should accept all float parameters, but GCC complains?
					/*double dummy;
					sx = modf(sx, &dummy);
					sy = modf(sy, &dummy);
					sz = modf(sz, &dummy);*/

					sx = sx - iSx;
					sy = sy - iSy;
					sz = sz - iSz;

					typename SrcVolumeType::VoxelType tInterpolatedValue = trilinearlyInterpolate(voxel000,voxel100,voxel010,voxel110,voxel001,voxel101,voxel011,voxel111,sx,sy,sz);

					typename DstVolumeType::VoxelType result = static_cast<typename DstVolumeType::VoxelType>(tInterpolatedValue);
					m_pVolDst->setVoxelAt(dx,dy,dz,result);
				}
			}
		}
	}
}
