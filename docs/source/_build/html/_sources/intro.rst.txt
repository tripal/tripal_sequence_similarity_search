************
Introduction
************

This module supports sequence similarity search on a Tripal website
through a new dual application option. The Tripal module provides
access to the speed increase available through Diamond for
BLASTP/BLASTX style searches as well as traditional NCBI BLAST for
BLASTN. Both applications are integrated into a single interface that
provides file upload or copy/paste sequence support for the query and
access to formatted databases for NCBI BLAST or Diamond. The target
databases can be customized for the categories of whole genome, gene,
protein, and transcriptome/unigene. The administration interface
allows the admin user to set what pre-indexed databases are available
(which show up in a dropdown menu). The module supports execution of
the searches on a remote machine so that the search is not running
directly on the limited resources typically associated with web
servers.