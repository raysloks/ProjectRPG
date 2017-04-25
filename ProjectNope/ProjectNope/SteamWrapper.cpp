#include "SteamWrapper.h"

#include "steam_api.h"

class SteamWrapper :
	public ISteamWrapper
{
public:
	SteamWrapper()
	{
		SteamAPI_Init();

		SteamFriends()->SetRichPresence("status", "testing testing 123 abc");
	}
	~SteamWrapper()
	{
		SteamAPI_Shutdown();
	}
};

ISteamWrapper * ISteamWrapper::make()
{
	return new SteamWrapper();
}