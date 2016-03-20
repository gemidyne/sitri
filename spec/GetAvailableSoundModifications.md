## API Page - GetAvailableSoundModifications ##

Provided a server port number, will return all available sound modifications for the server.

# Input 
Query String parameters

- Argument "port":
 * Must be an Int32. 
 * Cannot be empty.

# Output 
Format: JSON

Notes: 
- The OriginalFile and ReplacementFile are the full path to the sound file, relative to the tf/sounds folder.
- The ReplacementFile is precached and added to the downloads table automatically.
- The modification will only be valid as long as ReplacementFile is set to a value and not empty.

Example: 
```json
"SoundMods":[
	{ 
		"ProductId":123, 
		"Modifications":
		[
			{
				"OriginalFile": "vo/scout_medic01.wav",
				"ReplacementFile": "steven/dxd/xenovia/hello.wav"
			},
			{
				"OriginalFile": "vo/scout_medic02.wav",
				"ReplacementFile": "steven/dxd/xenovia/hello.wav"
			},
			{
				"OriginalFile": "vo/scout_medic03.wav",
				"ReplacementFile": "steven/dxd/xenovia/hello.wav"
			},
			{
				"OriginalFile": "vo/scout_help01.wav",
				"ReplacementFile": "steven/dxd/xenovia/issei1.wav"
			},
			{
				"OriginalFile": "vo/scout_help02.wav",
				"ReplacementFile": "steven/dxd/xenovia/issei2.wav"
			},
			{
				"OriginalFile": "vo/scout_help03.wav",
				"ReplacementFile": "steven/dxd/xenovia/issei3.wav"
			},
		]
	},
	{ 
		"ProductId":456, 
		"Modifications":
		[
			{
				"OriginalFile": "vo/soldier_spy01.wav",
				"ReplacementFile": "steven/dxd/rias/spy1.wav"
			},
			{
				"OriginalFile": "vo/soldier_spy02.wav",
				"ReplacementFile": "steven/dxd/rias/spy2.wav"
			},
			{
				"OriginalFile": "vo/soldier_spy03.wav",
				"ReplacementFile": "steven/dxd/rias/spy3.wav"
			},
			{
				"OriginalFile": "vo/soldier_help01.wav",
				"ReplacementFile": "steven/dxd/rias/medic.wav"
			},
			{
				"OriginalFile": "vo/soldier_help02.wav",
				"ReplacementFile": "steven/dxd/rias/medic.wav"
			},
			{
				"OriginalFile": "vo/soldier_help03.wav",
				"ReplacementFile": "steven/dxd/rias/medic.wav"
			},
		]
	},
]
```