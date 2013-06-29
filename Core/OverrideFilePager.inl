namespace Cubiquity
{
	/// Constructor
	template <typename VoxelType>
	OverrideFilePager<VoxelType>::OverrideFilePager(const std::string& strFolderName)
		:Pager<VoxelType>()
		,m_strFolderName(strFolderName)
	{
		m_strOverrideFolderName = strFolderName + "/override/";

		// TODO: We should create the directories if they don't exist.
	}

	/// Destructor
	template <typename VoxelType>
	OverrideFilePager<VoxelType>::~OverrideFilePager() {};

	template <typename VoxelType>
	void OverrideFilePager<VoxelType>::pageIn(const Region& region, PolyVox::Block<VoxelType>* pBlockData)
	{
		POLYVOX_ASSERT(pBlockData, "Attempting to page in NULL block");
		POLYVOX_ASSERT(pBlockData->hasUncompressedData() == false, "Block should not have uncompressed data");

		std::stringstream ss;
		ss << region.getLowerX() << "_" << region.getLowerY() << "_" << region.getLowerZ() << "_"
				<< region.getUpperX() << "_" << region.getUpperY() << "_" << region.getUpperZ();

		// Load from override folder
		{
			std::string filename = m_strOverrideFolderName + ss.str();
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
			return;
		}

		// Load from normal folder
		{
			std::string filename = m_strFolderName + ss.str();
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
			return;
		}
		
		// No data found
		{
			logTrace() << "No data found for " << region << " during paging in.";
			return;
		}
	}

	template <typename VoxelType>
	void OverrideFilePager<VoxelType>::pageOut(const Region& region, PolyVox::Block<VoxelType>* pBlockData)
	{
		POLYVOX_ASSERT(pBlockData, "Attempting to page out NULL block");
		POLYVOX_ASSERT(pBlockData->hasUncompressedData() == false, "Block should not have uncompressed data");

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

		fwrite(pBlockData->getCompressedData(), sizeof(uint8_t), pBlockData->getCompressedDataLength(), pFile);

		if(ferror(pFile))
		{
			POLYVOX_THROW(std::runtime_error, "Error writing out block data.");
		}

		fclose(pFile);
	}
}
