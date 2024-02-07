#include "Helper.h"

HMODULE GCM()
{
	HMODULE hModule = NULL;
	GetModuleHandleEx(
		GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,
		(LPCTSTR)GCM,
		&hModule
	);
	return hModule;
}

typedef struct
{
	size_t size;
	void* ptr;
	HGLOBAL hData;
} Resource;

Resource OpenResource(int resourceID, int resourceType)
{
	Resource result = { 0, NULL };

	HMODULE hModule = GCM();
	HRSRC hResource = FindResource(hModule, MAKEINTRESOURCE(resourceID), MAKEINTRESOURCE(resourceType));
	if (hResource != 0)
	{
		result.hData = LoadResource(hModule, hResource);
		if (result.hData != 0)
		{
			result.size = SizeofResource(hModule, hResource);
			if (result.size != 0)
				result.ptr = LockResource(result.hData);
		}
	}
	return result;
}

void CloseResource(Resource resource)
{
	FreeResource(resource.hData);
}