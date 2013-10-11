namespace Cubiquity
{
	/// Constructor
	template <typename VoxelType>
	SQLitePager<VoxelType>::SQLitePager(const std::string& dbName)
		:Pager<VoxelType>()
	{
		logDebug() << "Creating SQLitePager from '" << dbName << "'";
		
		int rc = sqlite3_open(dbName.c_str(), &pDatabase);
		if(rc != SQLITE_OK)
		{
			std::stringstream ss;
			ss << "Failed to open SQLite database. Message was: \"" << sqlite3_errmsg(pDatabase) << "\"";
			throw std::runtime_error(ss.str().c_str());
		}

		char* sql = "CREATE TABLE BLOCKS("  \
         "ID INT PRIMARY KEY     NOT NULL," \
         "DATA         BLOB );";

		char* pErrorMsg = 0;
		rc = sqlite3_exec(pDatabase, sql, 0, 0, &pErrorMsg);
		if(rc != SQLITE_OK)
		{
			std::stringstream ss;
			ss << "Failed to open SQLite database. Message was: \"" << pErrorMsg << "\"";
			sqlite3_free(pErrorMsg);
			throw std::runtime_error(ss.str().c_str());
		}
	}

	/// Destructor
	template <typename VoxelType>
	SQLitePager<VoxelType>::~SQLitePager()
	{
		sqlite3_close(pDatabase);
	}

	template <typename VoxelType>
	void SQLitePager<VoxelType>::pageIn(const Region& region, PolyVox::CompressedBlock<VoxelType>* pBlockData)
	{
		POLYVOX_ASSERT(pBlockData, "Attempting to page in NULL block");
	}

	template <typename VoxelType>
	void SQLitePager<VoxelType>::pageOut(const Region& region, PolyVox::CompressedBlock<VoxelType>* pBlockData)
	{
		POLYVOX_ASSERT(pBlockData, "Attempting to page out NULL block");

		logTrace() << "Paging out data for " << region;

		
	}
}
