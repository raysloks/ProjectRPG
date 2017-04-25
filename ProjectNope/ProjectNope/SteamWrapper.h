#ifndef STEAM_WRAPPER_H
#define STEAM_WRAPPER_H

class ISteamWrapper
{
public:
	virtual ~ISteamWrapper() {};

	static ISteamWrapper * make();
};

#endif