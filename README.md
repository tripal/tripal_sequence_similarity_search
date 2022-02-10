[![Documentation Status](https://readthedocs.org/projects/tripal_sequence_similarity_search/badge/?version=latest)](https://tripal-sequence-similarity-search.readthedocs.io/en/latest/)
[![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.3380529.svg)](https://doi.org/10.5281/zenodo.3380529)


## Tripal Sequence Similarity Search (TSeq)
This module supports sequence similarity search on a Tripal website through a new dual application option. The Tripal module provides access to the speed increase available through Diamond for BLASTP/BLASTX style searches as well as traditional NCBI BLAST for BLASTN. Both applications are integrated into a single interface that provides file upload or copy/paste sequence support for the query and access to formatted databases for NCBI BLAST or Diamond. The target databases can be customized for the categories of whole genome, gene, protein, and transcriptome/unigene. The administration interface allows the admin user to set what pre-indexed databases are available (which show up in a dropdown menu). The module supports execution of the searches on a remote machine so that the search is not running directly on the limited resources typically associated with web servers. 

## Further Documentation

Extended documentation for this module lives here on [Read The Docs](https://tripal-sequence-similarity-search.readthedocs.io/en/latest/)


## Requirements
- [Tripal Remote Job](https://github.com/tripal/tripal_remote_job) 
- [Tripal 3](http://tripal.info/)
- diamond version 0.9.24 (https://github.com/bbuchfink/diamond)
- NCBI Blast version 2.7.1+ (ftp://ftp.ncbi.nlm.nih.gov/blast/executables/blast+/LATEST/)

## Installation / Useful Pages & Info
1. Visit /admin/tripal/extension/tseq/config to set up databases and other options
2. Permission role 'administer diamond' created for site admins
3. It is **strongly** recommended to use the [Tripal Daemon](https://tripal.readthedocs.io/en/latest/user_guide/job_management.html) to automate the tasks within
for this module. Using the daemon ensures that new jobs are launched in a timely and
consistent manner. It is especially important that this system be used to reduce any
problems that may arise from file permission issues.

| Path                               | Description                               |
|------------------------------------|-------------------------------------------|
| TSeq                               | Main Page                                 |
| TSeq/submit                        | Search submission page                    |
| TSeq/results/#                     | Job results page (# is the Tripal Job ID) |
| admin/tripal/extension/tseq/config | Module administration page                |

## Status
- [x] Submit Form
- [x] Admin
- [x] Add/Delete/Modify Target Database List
- [x] Categories
- [x] Permissions
- [x] Basic Results Page
- [x] Advanced Results Page
- [x] Results list on User Profile "TSeq Analysis" tab
- [x] Database categorization
- [ ] Check if existing database files are readable (for purposes of running against)
- [x] Email user when job has finished
- [x] Show the number of sequences in each target database
- [ ] Better user input validation (biological)
- [ ] Admin option: temp directory (--tmpdir)
