## API Page - GetAvailableSkinManifests ##

Provided a server port number, will return all available skin manifests for the server.

# Input 
- Query String parameters

Argument "port":
	Must be an Int32. 
	Cannot be empty.

# Output 
- Format: JSON

Notes: 
	The strings in the Files array must be the full path to the file, relative to the tf folder.
	The FullModelPath is the full path to the .mdl file, relative to the tf folder.
	The FullModelPath is precached on Map Start. The material files will be precached automatically by the engine.
	The Files array are only added to the Downloads Table. 

Example: 
```json
"PlayerSkins":[
	{ 
		"SkinId":123, 
		"FullModelPath":"models/steven/dxd/xenovia.mdl", 
		"Files":
		[
			"models/steven/dxd/xenovia.mdl",
			"models/steven/dxd/xenovia.phy",
			"models/steven/dxd/xenovia.vvd",
			"models/steven/dxd/xenovia.dx80.vtx",
			"models/steven/dxd/xenovia.dx90.vtx",
			"models/steven/dxd/xenovia.sw.vtx",
		]
	},
	{
		"SkinId":456, 
		"FullModelPath":"models/steven/dxd/rias.mdl", 
		"Files":
		[
			"models/steven/dxd/rias.mdl",
			"models/steven/dxd/rias.phy",
			"models/steven/dxd/rias.vvd",
			"models/steven/dxd/rias.dx80.vtx",
			"models/steven/dxd/rias.dx90.vtx",
			"models/steven/dxd/rias.sw.vtx",
		] 
	},
	{
		"SkinId":789, 
		"FullModelPath":"models/steven/dxd/akeno.mdl", 
		"Files":
		[
			"models/steven/dxd/akeno.mdl",
			"models/steven/dxd/akeno.phy",
			"models/steven/dxd/akeno.vvd",
			"models/steven/dxd/akeno.dx80.vtx",
			"models/steven/dxd/akeno.dx90.vtx",
			"models/steven/dxd/akeno.sw.vtx",
		] 
	}
]
```