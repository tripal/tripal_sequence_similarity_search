**************
Administration
**************

Prerequisites
---------

* Installed on the server:
    - `Diamond, version 0.9.24`_
    - `NCBI's BLAST+`_, latest
* Drupal Modules:
    - `Tripal Remote Job`_

.. _Diamond, version 0.9.24: https://github.com/bbuchfink/diamond
.. _NCBI's BLAST+: https://blast.ncbi.nlm.nih.gov/Blast.cgi?PAGE_TYPE=BlastDocs&DOC_TYPE=Download
.. _Tripal Remote Job: https://github.com/tripal/tripal_remote_job

Installation
------------
    1. Install all prerequisites.
    2. Download/clone the github repository to somewhere within your site's ``sites/all/modules`` folder
    3. If you have Drush installed, run 
    
      ``drush pm-enable tripal_seq``

      If drush is not installed, you can enable the module by navigating to your Modules page and enabling it there
    4. Thingy

Configuration
-------------