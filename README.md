Sitri
============
A SourceMod plugin for Team Fortress 2 which allows PlayerSkins and SoundModifications to be used and controlled server-side.

## WARNING
Please note that this plugin will not work by just compiling it. It requires extensive configuration and setup. See the "How it works" section below for more details.

## How it works
Sitri talks via HTTP requests to a Web API which provides it with information. It does not execute database queries directly. 

The Web API I used in StSv is not open source as of the time of writing this README file. *You will need to create your own database schema & Web API for this to work.*

I have created some specification files to describe what Sitri expects from a Web API - they are in the "webapi-spec" folder of this repository.

The Web API should never be exposed publicly or bound to a public IP address. It should only ever be accessible by the servers that should need access to it.

Once the Web API has been setup, you will need to edit the IWA_URL #define in [src/anarchysteven/sitri/InternalWebAPI.inc](src/anarchysteven/sitri/InternalWebAPI.inc). 
Then, provided the data from the API is valid and correct, everything should just start working.

## Extensions required 
- TF2 extension (bundled with SourceMod)
- SteamTools (for making HTTP requests)

## Plugins required
- None

## Custom includes required
- morecolours
- smlib

## Console Variables
- None

## License
Licensed under the [GNU GPL License](LICENSE.md)