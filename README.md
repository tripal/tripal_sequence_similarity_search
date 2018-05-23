## Tripal Diamond Search
A module to allow Tripal to run Diamond Searches.

## Requirements
- [Tripal Remote Job](https://gitlab.com/TreeGenes/tripal-remote-job) 
- [Tripal 3](http://tripal.info/)

## Installation / Useful Pages & Info
1. Visit /admin/config/diamond to set up databases and other options
2. Permission role 'administer diamond' created for site admins

- Diamond                           Main Page
- Diamond/submit                    Main Submission Page
- Diamond/results/#                 Job results page (# is the Tripal Job ID)
- admin/tripal/extension/diamond    Diamond Module Administration

## Status
- [x] Submit Form
- [x] Admin
- [x] Add/Delete/Modify Target Database List
- [x] Permissions
- [x] Basic Results Page
- [x] Advanced Results Page
- [x] Database categorization
- [ ] Check if existing database files are readable (for purposes of running against)
- [ ] Email user when job has finished
