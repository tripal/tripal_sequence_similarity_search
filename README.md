[![Documentation Status](https://readthedocs.org/projects/tripal_sequence_similarity_search/badge/?version=latest)](https://tripal-sequence-similarity-search.readthedocs.io/en/latest/)

## Tripal Sequence Similarity Search (TSeq)
This module supports sequence similarity search on a Tripal website through a new dual application option. The Tripal module provides access to the speed increase available through Diamond for BLASTP/BLASTX style searches as well as traditional NCBI BLAST for BLASTN. Both applications are integrated into a single interface that provides file upload or copy/paste sequence support for the query and access to formatted databases for NCBI BLAST or Diamond. The target databases can be customized for the categories of whole genome, gene, protein, and transcriptome/unigene. The administration interface allows the admin user to set what pre-indexed databases are available (which show up in a dropdown menu). The module supports execution of the searches on a remote machine so that the search is not running directly on the limited resources typically associated with web servers. 

## Further Documentation

Extended documentation for this module lives here on [Read The Docs](https://tripal-sequence-similarity-search.readthedocs.io/en/latest/)


## Requirements
- [Tripal Remote Job](https://gitlab.com/TreeGenes/tripal-remote-job) 
- [Tripal 3](http://tripal.info/)
- diamond version 0.9.24 (https://github.com/bbuchfink/diamond)
- NCBI Blast version 2.7.1+ (ftp://ftp.ncbi.nlm.nih.gov/blast/executables/blast+/LATEST/)

## Installation / Useful Pages & Info
1. Visit /admin/tripal/extension/tseq/config to set up databases and other options
2. Permission role 'administer diamond' created for site admins

- TSeq                                  Main Page
- TSeq/submit                           Main Submission Page
- TSeq/results/#                        Job results page (# is the Tripal Job ID)
- admin/tripal/extension/tseq/config    Tripal Seq Module Administration

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
