#include <smjansson.inc>

// Check the setup guide for more details on how this all works.
// !!! IMPORTANT: The below IWA url should not have external access at all. Site should not be binding to a public IP.
#define IWA_URL "<! Enter your IWA url here. It should be the root of the site address as page URI's will be appended. !>"

public Steam_FullyLoaded()
{
	g_bSteamLoaded = true;

	WebAPI_RequestPlayerSkinManifests();
	WebAPI_RequestSoundModificationManifests();
}

public Steam_Shutdown()
{
	g_bSteamLoaded = false;
}

#include "anarchysteven/sitri/InternalWebAPI.PlayerSkinManifests.inc"
#include "anarchysteven/sitri/InternalWebAPI.PlayerSkinEquipment.inc"
#include "anarchysteven/sitri/InternalWebAPI.SoundModificationManifests.inc"
#include "anarchysteven/sitri/InternalWebAPI.SoundModificationEquipment.inc"