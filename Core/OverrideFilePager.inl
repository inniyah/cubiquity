#include "FileSystem.h"

namespace Cubiquity
{
	/// Constructor
	template <typename VoxelType>
	OverrideFilePager<VoxelType>::OverrideFilePager(const std::string& strFolderName)
		:Pager<VoxelType>()
		,m_strFolderName(strFolderName)
	{
		logDebug() << "Creating OverrideFilePager with folder '" << m_strFolderName << "'";

		// Validate the folder name
		POLYVOX_THROW_IF(m_strFolderName.empty(), std::invalid_argument, "You must provide a folder name for the OverrideFilePager");
		if((m_strFolderName.back() != '/') && (m_strFolderName.back() != '\\'))
		{
			//logWarning() << "Folder name " << m_strFolderName << " is missing a trailing '/' or '\\'. Please to provide this to avoid confusion!";
			m_strFolderName.append("/");
		}

		// The folder where the override data is stored.
		m_strOverrideFolderName = strFolderName + "override/";

		// Make sure the folders exist, creating them if necessary.
		ensureDirectoryExists(m_strFolderName);
		ensureDirectoryExists(m_strOverrideFolderName);
	}

	/// Destructor
	template <typename VoxelType>
	OverrideFilePager<VoxelType>::~OverrideFilePager() {};

	template <typename VoxelType>
	void OverrideFilePager<VoxelType>::pageIn(const Region& region, PolyVox::CompressedBlock<VoxelType>* pBlockData)
	{
		POLYVOX_ASSERT(pBlockData, "Attempting to page in NULL block");

		std::stringstream ss;
		ss << region.getLowerX() << "_" << region.getLowerY() << "_" << region.getLowerZ() << "_"
				<< region.getUpperX() << "_" << region.getUpperY() << "_" << region.getUpperZ();

		// Load from override folder
		{
			std::string filename = m_strOverrideFolderName + ss.str();
			FILE* pFile = fopen(filename.c_str(), "rb");
			if(pFile)
			{
				logTrace() << "Paging in override data for " << region << " from " << m_strOverrideFolderName;

				fseek(pFile, 0L, SEEK_END);
				size_t fileSizeInBytes = ftell(pFile);
				fseek(pFile, 0L, SEEK_SET);
				
				uint8_t* buffer = new uint8_t[fileSizeInBytes];
				fread(buffer, sizeof(uint8_t), fileSizeInBytes, pFile);
				pBlockData->setData(buffer, fileSizeInBytes);
				delete[] buffer;

				if(ferror(pFile))
				{
					POLYVOX_THROW(std::runtime_error, "Error reading in block data, even though a file exists.");
				}

				fclose(pFile);
				return;
			}			
		}

		// Load from normal folder
		{
			std::string filename = m_strFolderName + ss.str();
			FILE* pFile = fopen(filename.c_str(), "rb");
			if(pFile)
			{
				logTrace() << "Paging in original data for " << region << " from " << m_strFolderName;

				fseek(pFile, 0L, SEEK_END);
				size_t fileSizeInBytes = ftell(pFile);
				fseek(pFile, 0L, SEEK_SET);
				
				uint8_t* buffer = new uint8_t[fileSizeInBytes];
				fread(buffer, sizeof(uint8_t), fileSizeInBytes, pFile);
				pBlockData->setData(buffer, fileSizeInBytes);
				delete[] buffer;

				if(ferror(pFile))
				{
					POLYVOX_THROW(std::runtime_error, "Error reading in block data, even though a file exists.");
				}

				fclose(pFile);
				return;
			}
		}
		
		// No data found
		{
			logTrace() << "No data found for " << region << " during paging in.";
			return;
		}
	}

	template <typename VoxelType>
	void OverrideFilePager<VoxelType>::pageOut(const Region& region, PolyVox::CompressedBlock<VoxelType>* pBlockData)
	{
		POLYVOX_ASSERT(pBlockData, "Attempting to page out NULL block");

		logTrace() << "Paging out data for " << region;

		std::stringstream ss;
		ss << region.getLowerX() << "_" << region.getLowerY() << "_" << region.getLowerZ() << "_"
				<< region.getUpperX() << "_" << region.getUpperY() << "_" << region.getUpperZ();

		std::string filename = m_strOverrideFolderName + ss.str();

		// FIXME - This should be replaced by C++ style IO, but currently this causes problems with
		// the gameplay-cubiquity integration. See: https://github.com/blackberry/GamePlay/issues/919

		FILE* pFile = fopen(filename.c_str(), "wb");
		if(!pFile)
		{
			POLYVOX_THROW(std::runtime_error, "Unable to open file to write out block data.");
		}

		fwrite(pBlockData->getData(), sizeof(uint8_t), pBlockData->getDataSizeInBytes(), pFile);

		if(ferror(pFile))
		{
			POLYVOX_THROW(std::runtime_error, "Error writing out block data.");
		}

		fclose(pFile);
	}

	template <typename VoxelType>
	void OverrideFilePager<VoxelType>::ensureDirectoryExists(const std::string& folderName)
	{
		if(directoryExists(folderName))
		{
			logDebug() << "Directory '" << folderName << "' already exists.";
			return;
		}

		if(createDirectory(folderName))
		{
			logDebug() << "Directory '" << folderName << "' has been created.";
		}
		else
		{
			std::stringstream ss;
			ss << "Directory '" << folderName << "' does not exist and could not be created";
			POLYVOX_THROW(std::runtime_error, ss.str());
		}
	}
}
