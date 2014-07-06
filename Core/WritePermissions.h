#ifndef CUBIQUITY_WRITEPERMISSIONS_H_
#define CUBIQUITY_WRITEPERMISSIONS_H_

namespace Cubiquity
{
	namespace WritePermissions
	{
		enum WritePermission
		{
			ReadOnly,
			ReadWrite
		};
	}
	typedef WritePermissions::WritePermission WritePermission;
}

#endif // CUBIQUITY_WRITEPERMISSIONS_H_
