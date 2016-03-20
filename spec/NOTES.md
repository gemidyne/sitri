Web API Notes
=====================================

Sitri expects all API pages to end in the ".aspx" file extension. This can be changed easily in the InternalWebAPI.*.inc files to .PHP, or whatever extension you want to use.

Sitri expects all API pages to live in the "Sitri" folder on the IWA_URL. Example: http://myinternalapi.gameservers.stsv.tf/Sitri/<API Page URI here>

Sitri expects all API pages to return valid JSON responses.

Sitri has a 60 second timeout when requesting a API page.