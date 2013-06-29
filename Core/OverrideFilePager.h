#ifndef __CUBIQUITY_OVERRIDE_FILE_PAGER_H__
#define __CUBIQUITY_OVERRIDE_FILE_PAGER_H__

#include "PolyVoxCore/Impl/TypeDef.h"

#include "PolyVoxCore/Pager.h"
#include "PolyVoxCore/Region.h"

#include <stdexcept>
#include <string>

namespace Cubiquity
{
	/**
	 * Provides an interface for performing paging of data.
	 */
	template <typename VoxelType>
	class OverrideFilePager : public PolyVox::Pager<VoxelType>
	{
	public:
		/// Constructor
		OverrideFilePager(const std::string& strFolderName)
			:Pager<VoxelType>()
			,m_strFolderName(strFolderName)
		{
		}

		/// Destructor
		virtual ~OverrideFilePager() {};

		virtual void pageIn(const Region& region, PolyVox::Block<VoxelType>* pBlockData)
		{
			POLYVOX_ASSERT(pBlockData, "Attempting to page in NULL block");
			POLYVOX_ASSERT(pBlockData->hasUncompressedData() == false, "Block should not have uncompressed data");

			std::stringstream ss;
			ss << region.getLowerX() << "_" << region.getLowerY() << "_" << region.getLowerZ() << "_"
				 << region.getUpperX() << "_" << region.getUpperY() << "_" << region.getUpperZ();

			std::string filename = m_strFolderName + ss.str();

			// FIXME - This should be replaced by C++ style IO, but currently this causes problems with
			// the gameplay-cubiquity integration. See: https://github.com/blackberry/GamePlay/issues/919

			FILE* pFile = fopen(filename.c_str(), "rb");
			if(pFile)
			{
				logTrace() << "Paging in data for " << region;

				fseek(pFile, 0L, SEEK_END);
				size_t fileSizeInBytes = ftell(pFile);
				fseek(pFile, 0L, SEEK_SET);
				
				uint8_t* buffer = new uint8_t[fileSizeInBytes];
				fread(buffer, sizeof(uint8_t), fileSizeInBytes, pFile);
				pBlockData->setCompressedData(buffer, fileSizeInBytes);
				delete[] buffer;

				if(ferror(pFile))
				{
					POLYVOX_THROW(std::runtime_error, "Error reading in block data, even though a file exists.");
				}

				fclose(pFile);
			}
			else
			{
				logTrace() << "No data found for " << region << " during paging in.";
			}
		}

		virtual void pageOut(const Region& region, PolyVox::Block<VoxelType>* pBlockData)
		{
			POLYVOX_ASSERT(pBlockData, "Attempting to page out NULL block");
			POLYVOX_ASSERT(pBlockData->hasUncompressedData() == false, "Block should not have uncompressed data");

			logTrace() << "Paging out data for " << region;

			std::stringstream ss;
			ss << region.getLowerX() << "_" << region.getLowerY() << "_" << region.getLowerZ() << "_"
				 << region.getUpperX() << "_" << region.getUpperY() << "_" << region.getUpperZ();

			std::string filename = m_strFolderName + ss.str();

			// FIXME - This should be replaced by C++ style IO, but currently this causes problems with
			// the gameplay-cubiquity integration. See: https://github.com/blackberry/GamePlay/issues/919

			FILE* pFile = fopen(filename.c_str(), "wb");
			if(!pFile)
			{
				POLYVOX_THROW(std::runtime_error, "Unable to open file to write out block data.");
			}

			fwrite(pBlockData->getCompressedData(), sizeof(uint8_t), pBlockData->getCompressedDataLength(), pFile);

			if(ferror(pFile))
			{
				POLYVOX_THROW(std::runtime_error, "Error writing out block data.");
			}

			fclose(pFile);
		}

	protected:
		std::string m_strFolderName;
	};
}

#endif //__CUBIQUITY_OVERRIDE_FILE_PAGER_H__
