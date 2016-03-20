## API Page - GetSkinEquipmentForPlayer ##

Provided a valid STEAM ID, will return the equipment information of skins for a specific player.

# Input 
- Query String parameters

Argument "steamid":
	Must be a string. 
	Must be a STEAM2 ID in the format of STEAM_0:X:XXXXXX 
	Cannot be empty.

# Output 
- Format: JSON

JSON array of object: 
{ 
	"SkinId":<int>, 
	"Class":<short> 
}

Notes: Class is zerobased. ie 0=Scout, 1=Soldier, 2=Pyro, 3=DemoMan, 4=Heavy, 5=Engineer, 6=Medic, 7=Sniper, 8=Spy

Example: 
{
	{
		"SkinId": 123,		
		"Class": 3
	},
	{
		"SkinId": 456,
		"Class": 0
	}
}