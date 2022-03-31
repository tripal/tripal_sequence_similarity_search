<?php
error_reporting(E_ALL);

//Don't break the form by uploading a file
    function fix_ajax_upload($element, &$form_state, $form)
    {
        // process $element as normal
        $element = file_managed_file_process($element, $form_state, $form);

        // remove path, add callback
        unset($element['upload_button']['#ajax']['path']);
        $element['upload_button']['#ajax']['callback'] = 'file_ajax_upload_callback';

        return $element;
    }

    function my_own_test($element, &$form_state, $form)
    {
        dpm($element);
        return $element;
    }
    

function tseq_submit_form($form, &$form_state)
{
    global $user;
    if (!$user->uid) {
        $loginPage = $GLOBALS['base_url'].'/user';
        drupal_set_message("In order to execute a search, you must be logged into the database. "
                . "Click <a href=\"$loginPage\">here</a> to log in.",'error');
    }
    
    $form['Overview'] = array(
        '#type'         => 'fieldset',
        '#title'        => 'Similarity Search Overview',
        '#collapsible'  => TRUE,
        '#collapsed'    => FALSE,
        //'#description'  => t('DIAMOND is a BLAST-compatible local aligner for mapping translated DNA query sequences against a protein reference database ( BLASTX alignment mode). The speedup over BLAST many times faster on short reads at a typical sensitivity of 90 - 99% relative to BLAST depending on the data and settings. <br/ >Use the following form to submit a new Diamond job to TreeGenes')
        '#description'  => t('Sequence similarity search is supported in TreeGenes against genes, '
                . 'TreeGenes UniGenes, proteins, and full genomes.  Nucleotide searches are executed '
                . 'with BLAST (BLASTN) while protein or translated protein searches are executed with '
                . 'DIAMOND (BLASTX or BLASTP equivalent).  DIAMOND will execute searches in a fraction '
                . 'of the time of BLAST at a similar sensitivity.  Both packages accept similar input '
                . 'parameters and these can be modified below.  You may upload FASTA formatted sequences '
                . 'or paste FASTA formatted sequences for searching.  You can select from a list of '
                . 'pre-formatted DIAMOND and BLAST databases OR upload your own.  This can be '
                . 'pre-formatted or provided in FASTA format and formatted for you.'),
    );

    $form['QueryType'] = array (
        '#type'         => 'radios',
        //'#title'        => t('Select sequence search type:'),
        //'#description'  => t('Choose the type of query'),
        '#options'      => array(
            'Protein'       => t('Protein'),
            'Genomic'       => t('Nucleotide (coding or whole genome)')
            #'Genomic'       => t('Genomic (coding or non-coding)'),
            #'Genome'    => t('Genome'),
        ),
        '#prefix'           => '<hr><h2>Sequence Query Type</h2>',
        //'#field_prefix'       => 'To get started, choose your search type:',
    );
    
    /*  Query		Target		Program
        Nucleotide	Protein		blastx	(Diamond)
        Protein		Protein		blastp	(Diamond)
        Nucleotide	Nucleotide	blastn  (BLAST)
        Protein		Nucleotide	tblastn (BLAST)
      
        Target           Query           Program
        Protein          Nucleotide      blastx  (Diamond)
        Protein          Protein         blastp  (Diamond)
        Nucleotide       Nucleotide      blastn  (BLAST)
        Nucleotide       Protein         tblastn (BLAST)          
     */
    
    
    $form['BlastEquivNuc'] = array(
        '#type'         => 'select',
        '#title'        => t('Database Search Type'),
        //'#description'  => t('Select what type of Search you would like to run:'),
        '#options'      => array(//'blastn' => t('BLASTn (for use with a Nucleotide Database)'), //Diamond doesn't do this yet
            #'blastx' => t('BLASTx (for use with a Protein target database, uses Diamond)'),
            #'blastn'  => t('BLASTn (for use with a Nucleotide target database)'),
            'blastx' => t('BLASTx (Translated nucleotide query versus protein database)'),
            'blastn'  => t('BLASTn (Nucleotide query versus nucleotide database)'),
            ),
        '#states' => array(
            'visible' => array(
                ':input[name="QueryType"]' => array('value' => 'Genomic'),
            ),
        ),
        
    );
    
    $form['BlastEquivPro'] = array(
        '#type'         => 'select',
        '#title'        => t('Database Search Type'),
        //'#description'  => t('Select what type of Search you would like to run:'),
        '#options'      => array(//'tblastn' => t('tBLASTn (for use with a Nucleotide Database)'), //Diamond doesn't do this yet
            #'blastp' => t('BLASTp (for use with a Protein target database, uses Diamond)'),
            #'tblastn' => t('tBLASTn (for use with a Nucleotide target database)'),
            'blastp' => t('BLASTp (Protein query versus protein database)'),
            'tblastn' => t('tBLASTn (Protein query versus translated nucleotide database)'),
            ),
        '#states' => array(
            'visible' => array(
                ':input[name="QueryType"]' => array('value' => 'Protein'),
            ),
        ),
    );
    
    /*
     *  Query Input Section
     */
    
    $form['Query'] = array(
      '#type'           => 'container',
      '#prefix'         => '<h2>Provide/load Query Sequence</h2>',
    );
    
    $form['Query']['QueryPaste'] = array(
        '#type'         => 'textarea',
        //'#title'        => t('Raw query data'),
        '#required'     => false,
    );
    
    $form['Query']['QueryFile'] = array(
        '#type'         => 'managed_file',
        //'#title'        => t('Query File'),
        '#upload_validators' => array(
            'file_validate_extensions' => array('txt dmnd gz FA FAA FNN FASTA fa faa fnn fasta'),
            'test_the_file',
        ),
        #'#process'      => array('fix_ajax_upload'),
        #'#process'      => array('my_own_test'),
        #'#progress_indicator'   => array('bar'),
        '#upload_location' => 'public://',
    );
    
    /*
     * Target
     */
    
    $form['Target'] = array(
      '#type'           => 'container',
      '#prefix'         => '<hr><h2>Target/Database Selection</h2><div id="targetContainer" style="padding-left:15px;">',
      '#suffix'         => '</div>',
    );
    /*
     * Prepare the categories to show for each type (Protein/Nucleotide)
     */
    //Get Enabled Categories
    $query = "select category_title from tseq_categories where enabled=1";
    $results = db_query($query);
    
    $categories = array();
    $types = array();
       
    //Iterate through the returned categories 
    for($i = 0; $i< $results->rowCount();$i++)
    {
      $categories[$i] = $results->fetchAssoc();
    }
    
    //Get distinct types of available databases (should be any combination of Gene, Genome, and Protein)
    $query = "select distinct(type) from tseq_db_existing_locations";
    $results = db_query($query);
    $types = array();
    for($j = 0; $j < $results->rowCount(); $j++)
    {
        $types[$j] = $results->fetchAssoc();
    }
    
     
    $databaseListToShow = array();
    $databaseCount = array();
    $listsToVerify = array();
    
    foreach($categories as $category)
    {
        foreach($types as $type)
        {
            // Get the count of distinct databases that fit these criteria. Reject if there are none
            $query = "select count(db_id) from tseq_db_existing_locations where type='".$type['type']."' and category='".$category['category_title']."'";
            $results = db_query($query);
            $databaseCount[$type['type']][$category['category_title']] = $results->fetchAssoc();
                       
        }
    }
    
    foreach($categories as $category)
    {
        foreach($types as $type)
        {
            //Only do it for pairs that exist
            if ($databaseCount[$type['type']][$category['category_title']]['count'] > 0)
            {
                //echo $type['type'].", ".$category['category_title'].": ".$databaseCount[$type['type']][$category['category_title']]['count']."<br />"; 
                //Get databases belonging to the category and type
                $databaseListToShow[$type['type']][$category['category_title']] = TSeqDatabases::getDatabasesByTypeCategory($type['type'], $category['category_title']);
                
                //Prepare them nicely
                unset($databaseSelect); //Flush the list
                $databaseSelect['default'] = t('--');
                foreach($databaseListToShow[$type['type']][$category['category_title']] as $dbList)
                {
                    
                    $databaseSelect[$dbList['db_id']] = $dbList['name'].', version '.$dbList['version'];
                    $dbList['count'] == '' ? $databaseSelect[$dbList['db_id']] .= '' : $databaseSelect[$dbList['db_id']] .= ', ('.$dbList['count'].' sequences)';
                }
                
                //Create the form element to display them
                if ($category['category_title'] == 'Standard')
                {
                    $listTitle = $type['type'];
                }
                else
                {
                    $listTitle = $type['type']." (".$category['category_title'].")";
                }
                $listName = $type['type']."_".$category['category_title'];
                array_push($listsToVerify, $listName);
                if ($type['type'] == 'Protein')
                {
                    $form['Target'][$listName] = array(
                        '#type'     => 'select',
                        '#title'    => t($listTitle),
                        '#options'      => $databaseSelect,
                        '#states' => array(
                            'visible' => array(
                                array(
                                    ':input[name="QueryType"]' => array('value' => 'Protein'),
                                    ':input[name="BlastEquivPro"]' => array('value' => 'blastp'),
                                ),
                                array(
                                    ':input[name="QueryType"]' => array('value' => 'Genomic'),
                                    ':input[name="BlastEquivNuc"]' => array('value' => 'blastx'),
                                ),
                            ),
                            'disabled' => array(
                                array(
                                    array(':input[name="TargetDataType"]' => array('value' => 'upload')),
                                    'or',
                                    array(':input[name="TargetDataType"]' => array('value' => 'paste')),
                                ),
                            ),
                        ),
                    );
                }
                else
                {
                    $form['Target'][$listName] = array(
                        '#type'     => 'select',
                        '#title'    => t($listTitle),
                        '#options'      => $databaseSelect,
                        '#states' => array(
                            'visible' => array(
                                //':input[name="TargetDataType"]' => array('value' => 'database'),
                                //':input[name="QueryType"]' => array('value' => 'Genomic'),
                                array(
                                    ':input[name="QueryType"]' => array('value' => 'Protein'),
                                    ':input[name="BlastEquivPro"]' => array('value' => 'tblastn'),
                                ),
                                array(
                                    ':input[name="QueryType"]' => array('value' => 'Genomic'),
                                    ':input[name="BlastEquivNuc"]' => array('value' => 'blastn'),
                                ),
                            ),
                            'disabled' => array(
                                array(
                                    array(':input[name="TargetDataType"]' => array('value' => 'upload')),
                                    'or',
                                    array(':input[name="TargetDataType"]' => array('value' => 'paste')),
                                ),
                            ),
                        ),
                    );
                }
                    
                 
                 
            }
            //dpm($form['Target'][$listName]);
            //dpm($databaseCount[$type['type']][$category['category_title']]);
        }
    }
    
    //Save a list of forms to check in _verification and _submit stage
    $form['Target']['TargetList'] = array(
        '#type'         =>  'value',
        '#value'        =>  $listsToVerify,
    );
    
    $form['Target']['OtherTarget'] = array(
      '#type'           => 'fieldset',
      '#title'          => 'Upload your own database/target',
      '#collapsible'    => TRUE,
      '#collapsed'      => TRUE,
      '#prefix'         => '<br />',
    );
    
    $form['Target']['OtherTarget']['TargetDataType'] = array(
        '#type'         => 'radios',
        '#title'        => t('Provide/Select search database:'),
        //'#description'  => t('Select the database target type'),
        '#default_value'=> 'database',
        '#options'      => array(
            'database' => t('Select from an existing database'),
            'upload' => t('Upload a File'),
            'paste' => t('Type or paste manually'),
            ),
    );

    $form['Target']['OtherTarget']['TargetPaste'] = array(
        '#type'         => 'textarea',
        '#title'        => t('Raw target data'),
        //'#description'  => t('Paste or type your target data into the box above'),
        '#required'     => false,
        '#states' => array(
            'visible' => array(
                ':input[name="TargetDataType"]' => array('value' => 'paste'),
            ),
        ),
        // Last ditch attempt at a solution using JS
        //'#suffix' => '<a onclick="resetTargetDataType()">Reset</a>',
    );
    
    $form['Target']['OtherTarget']['TargetFile'] = array(
        '#type'         => 'managed_file',
        '#title'        => t('Target File'),
        //'#description'  => t('Don\'t forget to hit \'Upload\''),
        '#upload_validators' => array(
            'file_validate_extensions' => array('txt dmnd gz fasta FASTA faa fnn'),
        ),
        '#upload_location' => 'public://',
        '#states' => array(
            'visible' => array(
                ':input[name="TargetDataType"]' => array('value' => 'upload'),
            ),
        ),
    );
        
    /*
     * Forms for simple command line options 
     */
    
    // Get the default values from the database
    $query = 'select defaults_e_value, defaults_target_coverage, defaults_query_coverage, defaults_max_alignments_list, defaults_max_alignments_selected, produce_blast_pairwise, produce_diamond_pairwise from tseq_settings';
    $results = db_query($query);
    $advanced_options = $results->fetchAssoc();
    
    $form['Advanced'] = array(
      '#type'           => 'fieldset',
      '#title'          => 'Advanced Options',
      '#collapsible'    => TRUE,
      '#collapsed'      => TRUE,
      '#prefix'         => '<hr>',
    );
    
    
    $form['Advanced']['eValue'] = array(
        '#type'         =>  'textfield',
        '#title'        =>  'E-value',
        '#size'         =>  '10',
        //'#description'  =>  'Expected number of chance matches in a random model. This number should be give in a decimal format.',
        '#required'     =>  true,
        '#default_value'      =>  t($advanced_options['defaults_e_value']),
        //'#prefix'       => '<b>Advanced Options</b>',
        '#field_prefix' => 'Expected number of chance matches in a random model. This number should be give in a decimal format.<br />',
    );
    
    $form['Advanced']['targetCoverage'] = array(
        '#type'         =>  'textfield',
        '#title'        =>  'Target Coverage',
        '#size'         =>  '10',
        '#field_prefix'  =>  'Report only alignments above the given percentage of subject cover.<br />',
        '#required'     =>  true,
        '#default_value'      =>  t($advanced_options['defaults_target_coverage']),
    );
    
    $form['Advanced']['queryCoverage'] = array(
        '#type'         =>  'textfield',
        '#title'        =>  'Query Coverage',
        '#size'         =>  '10',
        '#field_prefix'  =>  'Report only alignments above the given percentage of query cover.<br />',
        '#required'     =>  true,
        '#default_value'      =>  t($advanced_options['defaults_query_coverage']),
    );
    
            // Get the list of default options for max alignments
            $max_alignment_array = explode(',',$advanced_options['defaults_max_alignments_list']);
            $max_alignments_to_show = array();
            foreach($max_alignment_array as $entry)
            {
                $max_alignment_to_show[$entry] = array(trim($entry) => t(trim($entry)));
            }
            
    $form['Advanced']['maxAlignments'] = array(
        '#type'         =>  'select',
        '#title'        =>  'Max Alignments',
        '#field_prefix'  =>  'The maximum number of target sequences per query to report alignments for.<br />',
        '#required'     =>  false,
        '#options'      =>  $max_alignment_to_show,
        '#default_value'      => $advanced_options['defaults_max_alignments_selected'],
    );
           
    if ($user->uid) {
        $form['submit_button'] = array(
        '#type' => 'submit',
        '#value' => t('Submit'),
        '#suffix' => t('<p>Upon Submission, your search will enter the '
                . 'queue with either BLAST or Diamond as the engine. You will'
                . ' be given a link to view the progress and results of the '
                . 'job.</p>')
        );
    }
    else
    {
        $form['submit_button'] = array(
        '#type' => 'submit',
        '#value' => t('Submit'),
        '#disabled' => true,
        '#suffix' => t('<p>Upon Submission, your search will enter the '
                . 'queue with either BLAST or Diamond as the engine. You will'
                . ' be given a link to view the progress and results of the '
                . 'job.</p>')
        );
    }
    
    return $form;
    
}

function tseq_submit_form_validate($form, &$form_state)
{
    /*
     * Currently very basic form validation
     * Does not check if the uploaded or pasted data is actually correct,
     * it only checks if there is something there (hence all the empty()
     * functions. 
     * TODO: Implement business logic 
     *       see validate_fasta_sequence() from tripal_blast as an example
     *       Turn instructional lines red somehow when user can't into form
     */
    
    
    
    /*
     * QueryType
     */
    if (empty($form_state['values']['QueryType']))
    {
        form_set_error('QueryType',t("Please choose a Query Type."));
    }
    
    /*
     * Determine the Target type (Protein vs Nucleotide) (for convenience later in validation)
     */
    if ($form_state['values']['QueryType'] == 'Protein')
    {
        if ($form_state['values']['BlastEquivPro'] == 'blastp')
        {
            $targetType = 'Protein';
        }
        else
        {
            $targetType = 'Nucleotide';
        }
    }
    else
    {
        if ($form_state['values']['BlastEquivNuc'] == 'blastx')
        {
            $targetType = 'Protein';
        }
        else
        {
            $targetType = 'Nucleotide';
        }
    }
    
    $targetsProvidedPro = 0;
    $targetsProvidedNuc = 0;
    //Check each target selection to see if they have an option selected,
    //  making sure that only 1 is selected 
    //Proteins
    $targetList = $form_state['values']['TargetList'];
    foreach($targetList as $target)
    {
        if ($form_state['values'][$target] != 'default')
        {
            $type = explode('_',$target);
            if ($type[0] == 'Protein')
            {
                $targetsProvidedPro = $targetsProvidedPro + 1;
            }
            else
            {
                $targetsProvidedNuc = $targetsProvidedNuc + 1;
            }
        }
    }
    
    //Handle if there are more than 1 targets selected
    if ($targetType == 'Protein' && $form_state['values']['TargetDataType'] == 'database')
    {
        if ($targetsProvidedPro > 1)
        {
            form_set_error('Target',t("Please choose only 1 protein target."));
            /*if ($form_state['values']['TargetDBProtein'] != 'default')
            {
               form_set_error('TargetDBProtein');
            }
            if ($form_state['values']['TargetDBUnigenePro'] != 'default')
            {
               form_set_error('TargetDBUnigenePro');
            }
             *
             */
            //form_set_error('Target');
        }
        else if ($targetsProvidedPro < 1)
        {
            form_set_error('Target',t("Please choose a protein target database"));
        }
    }
    else if ($targetType == 'Nucleotide' && $form_state['values']['TargetDataType'] == 'database')
    {
        if ($targetsProvidedNuc > 1)
        {
            form_set_error('Target',t("Please choose only 1 nucleotide target."));
            /*if ($form_state['values']['TargetDBGene'] != 'default')
            {
               form_set_error('TargetDBGene');
            }
            if ($form_state['values']['TargetDBGenome'] != 'default')
            {
               form_set_error('TargetDBGenome');
            }
            if ($form_state['values']['TargetDBUnigeneNuc'] != 'default')
            {
              form_set_error('TargetDBUnigeneNuc');
            }
             * 
             */
        }
        else if ($targetsProvidedNuc < 1)
        {
            form_set_error('Target',t("Please choose a nucleotide target database"));
        }
    }
        
    /*
     * BlastEquivNuc (Requires QueryType = Nucleotide)
     * Not strictly necessary: this is a 'select' which already has a default
     * value
     */
    if (!empty($form_state['values']['QueryType']))
    {
        if (empty($form_state['values']['BlastEquivNuc']))
        {
            form_set_error('BlastEquivNuc',t("Please choose a Diamond Search Type."));
        }
        if (empty($form_state['values']['BlastEquivPro']))
        {
            form_set_error('BlastEquivNuc',t("Please choose a Diamond Search Type."));
        }
    }
    
    //Query 
    
    $querysProvided = 0;
    $queryType = '';
    if(!empty($form_state['values']['QueryFile']))
    {
        $querysProvided = $querysProvided + 1;  // Keep track of # of queries provided
        $queryType = 'upload';
    }
    if(!empty($form_state['values']['QueryPaste']))
    {
        $querysProvided = $querysProvided + 1;
        $queryType = 'paste';
    }
    if ($querysProvided > 1)
    {
        form_set_error('Query',t("Please only provide one sequence."));
        $queryType = NULL;
    }
    else if ($querysProvided < 1)
    {
        form_set_error('Query',t("Please provide a sequence to query."));
        form_set_error('QueryFile');
        form_set_error('QueryPaste');
    }
    
    // Validate the user's query
    // Query uploaded as file:
    if ($queryType == 'upload')
    {
        // open the file
        $queryFile = file_load($form_state['values']['QueryFile']);
        $queryFileURI = drupal_realpath($queryFile->uri);
        if ($queryFileHandle = fopen($queryFileURI, 'r'))
        {
            $querySequence = fread($queryFileHandle, filesize($queryFileURI));
            // pass its contents through validation
            if(!tseq_validate_sequence($form_state['values']['QueryType'],$querySequence))
            {
                form_set_error('QueryFile','The query file you uploaded has invalid '
                        . 'syntax. Please check your file and reupload.');
            }
            // Close the file
            fclose($queryFileHandle);
                    
        }
        else
        {
            drupal_set_message("Could not open the submitted file. Please contact an administrator",'error');
        }
        
    }
    
    // Query uploaded as pasted text:
    else if ($queryType == 'paste')
    {
        // get the text
        $querySequence = $form_state['values']['QueryPaste'];
        // pass it through validation
        if(!tseq_validate_sequence($form_state['values']['QueryType'],$querySequence))
            {
                form_set_error('QueryPaste','The query file you uploaded has invalid '
                        . 'syntax. Please check your file and reupload.');
            }
    }
        
    
}

function tseq_submit_form_submit($form, &$form_state)
{
    $debug = FALSE;         //Debug Mode (does everything except submit a job)
    $prepError = FALSE;     //Errors with preparing the job (checked prior
                            //to launching job
    /*
     * Build the command
     */
    
    //Get the advanced settings    
    $eValue = $form_state['values']['eValue'];
    $targetCoverage = $form_state['values']['targetCoverage'];
    $queryCoverage = $form_state['values']['queryCoverage'];
    $maxAlignments = $form_state['values']['maxAlignments'];
    
    /*
     * Deal with the Query
     */
    //Get any pasted data and save as a file to send
    //Pasted Query:
    if(!empty($form_state['values']['QueryPaste']))
    {
        $queryFileType = 'paste';
        //$queryFile = "/tmp/".date('YMd_His').'.fasta';
        $queryFile = drupal_realpath("public://").'/'.date('YMd_His').'.fasta';
        $queryFileWrite = file_put_contents($queryFile, $form_state['values']['QueryPaste']);
        if ($queryFileWrite === FALSE){$prepError = TRUE;}
    }
 
    
    //Get information for uploaded files
    //Query Upload
    if(!empty($form_state['values']['QueryFile']))
    {
        $queryFileType = 'upload';
        $queryFile = file_load($form_state['values']['QueryFile']);
        if (is_object($queryFile))
        {
            $queryFile = drupal_realpath($queryFile->uri);
            if($debug){drupal_set_message("Query File: ".$queryFile);}
        }
        else
        {
            if($debug){drupal_set_message("Query File uplaod failed.");}
        }        
    }
    
    /*
     * Deal with the Target
     */
    //Pasted Target
    if($form_state['values']['TargetDataType'] == 'paste')
    {
        $targetFileType = 'paste';
        //$targetFile = "/tmp/".date('YMd_His').'.dmnd';
        $targetFile = drupal_realpath("public://").'/'.date('YMd_His').'.dmnd';
        $targetFileWrite = file_put_contents($targetFile, $form_state['values']['TargetPaste']);
        if ($targetFileWrite === FALSE){$prepError = TRUE;}
    }
    
    //Target Upload
    if($form_state['values']['TargetDataType'] == 'upload')
    {
        $targetFileType = 'upload';
        $targetFile = file_load($form_state['values']['TargetFile']);
        if (is_object($targetFile)) {
            $targetFile = drupal_realpath($targetFile->uri);
            if ($debug) {
                drupal_set_message("Target File: ".$targetFile);
            }
        } else {
            drupal_set_message("Target File upload failed.");
        }        
    }
    //Get path for selected pre-existing database
    //Target DB
    if($form_state['values']['TargetDataType'] == "database")
    {
        $targetFileType = 'database';
            $targetList = $form_state['values']['TargetList'];
            foreach($targetList as $target)
            {
                if ($form_state['values'][$target] != 'default')
                {
                    $type = explode('_',$target);
                    if ($type[0] == 'Protein')
                    {
                        $targetDB = $form_state['values'][$target];
                    }
                    else
                    {
                        $targetDB = $form_state['values'][$target];
                    }
                }
            }      
        
        $targetDBU = strtoupper($targetDB);
        //$targetDBPath = "/linuxshare/projects/treegenes/tgwebprod_store/FTP/Genomes/".$targetDB."/index/".$targetDBU."_dmndBlast/".$targetDBU."_diamond/";
        //$targetDBFile = $targetDBU.".dmnd";
        //
        $targetFile = TSeqDatabases::getDBInfo($targetDB,"location");
        if($debug){drupal_set_message("targetdbfile: ".$targetDB);}
        //drupal_set_message("Target DB path: ".$targetDBPath);
        //drupal_set_message("Full path to file: ".$targetDBPath.$targetDBFile);
        //$targetFile = $targetDBPath.$targetDBFile;
    }
    /************************************************************/
    
    
    //drupal_set_message("Query File: ".$queryFile);
    //drupal_set_message("Target File: ".$targetFile);
    
    $queryFileParts = explode("/",$queryFile);
    $targetFileParts = explode("/",$targetFile);
    
    $queryFileNoPath = $queryFileParts[count($queryFileParts)-1];
    $targetFileNoPath = $targetFileParts[count($targetFileParts)-1];
    
    if($debug){drupal_set_message("Query File No path: ".$queryFileNoPath);}
    if($debug){drupal_set_message("Target File No path: ".$targetFileNoPath);}
    
    //Are we running a Diamond job or straight BLAST?
    if ($form_state['values']['QueryType'] == "Protein")
    {
        if ($form_state['values']['BlastEquivPro'] == "blastp")
        {
            $blastEquiv = "blastp";     //run diamond blastp
        }
        elseif ($form_state['values']['BlastEquivPro'] == "tblastn")
        {
            $blastEquiv = "tblastn";        //run tblastn
        }
    }
    elseif ($form_state['values']['QueryType'] == "Genomic")
    {
        if ($form_state['values']['BlastEquivNuc'] == "blastx")
        {
            $blastEquiv = "blastx";  //run diamond blastx
        }
        elseif ($form_state['values']['BlastEquivNuc'] == "blastn")
        {
            $blastEquiv = "blastn";          //run blastn
        }
    }
    
    /*
     *  Get other parameters for the build
     *  -Threads (set via admin)
     *  -Executable locations (BLAST+, Diamond)
     *  -Pairwise alignment generation (BLAST, Diamond)
     */

    $query = 'select num_threads, blast_exe_location, diamond_exe_location, produce_blast_pairwise, produce_diamond_pairwise from tseq_settings';
    $results = db_query($query);
    $job_options = $results->fetchAssoc();

    // TODO Remove this, replace this usage throughout with variables from above
    $query = "select num_threads,blast_exe_location, diamond_exe_location from tseq_settings";
    $results = db_query($query);
    
    $jobSettings = array();
    for($i = 0; $i < $results->rowCount(); $i++)
        {
            $jobSettings[$i] = $results->fetchAssoc();
        }
    if($debug){dpm($jobSettings);}
    

    // Check if there were preperation errors
    if (!$prepError)
    {        
        $remote_diamond_job_id = -1;
        // Get the diamond executable
        if ($job_options['diamond_exe_location'])
        {
            $diamond_executable = $job_options['diamond_exe_location'];
        }
        else
        {
            $diamond_executable = 'diamond';
        }
        if ($form_state['values']['TargetDataType'] == 'database')
        {
            //$command_to_run = $blastEquiv." ".$threadsString." 1 --db ".$targetFile." --query ".$queryFileNoPath." --more-sensitive -k ".$maxAlignments." -e ".$eValue." --subject-cover ".$targetCoverage." --query-cover ".$queryCoverage;
            // If we're running with Diamond
            if ($blastEquiv == 'blastp' || $blastEquiv == 'blastx')
            {
                // Do we produce standard output or BLASTXML + Pairwise?
                if ($job_options['produce_diamond_pairwise'])
                {
                    $command_to_run = $diamond_executable . " " . $blastEquiv . " \\" . PHP_EOL .
                    "    --threads "       . $jobSettings[0]['num_threads']   . " \\" . PHP_EOL .
                    "    --db "            . $targetFile                      . " \\" . PHP_EOL .
                    "    --query "         . $queryFileNoPath                 . " \\" . PHP_EOL .
                    "    --more-sensitive"                                    . " \\" . PHP_EOL .
                    "    -k "              . $maxAlignments                   . " \\" . PHP_EOL .
                    "    -e "              . $eValue                          . " \\" . PHP_EOL .
                    "    --subject-cover " . $targetCoverage                  . " \\" . PHP_EOL .
                    "    --query-cover "   . $queryCoverage                   . " \\" . PHP_EOL .
                    "    --outfmt 100 "                                       . " \\" . PHP_EOL .
                    "    --out results.daa "                                  . " \\" . PHP_EOL ;

                    $command_to_run .= PHP_EOL; // Nice formatting

                    // Take DAA format, create BLAST XML output (regular table) and Pairwise output (for showing alignments)
                    $command_to_run .= $diamond_executable . " view"                         . " \\" . PHP_EOL .
                    "    --daa results.daa "                                  . " \\" . PHP_EOL .
                    "    --outfmt 6 qseqid sseqid pident length mismatch gapopen qstart qend sstart send evalue bitscore" . " \\" . PHP_EOL .
                    "    --out results_blastxml.txt"                          . " \\" . PHP_EOL ;

                    $command_to_run .= PHP_EOL; // Nice formatting

                    $command_to_run .= $diamond_executable . " view"                         . " \\" . PHP_EOL .
                    "    --daa results.daa "                                  . " \\" . PHP_EOL .
                    "    --outfmt 0 "                                         . " \\" . PHP_EOL .
                    "    --out results_pairwise.txt "                         . " & echo $! > PID" . PHP_EOL ;
                }
                // Just produce regular output (legacy)
                else
                {
                    $command_to_run = $diamond_executable . " " . $blastEquiv.
                    "    --threads "       . $jobSettings[0]['num_threads']   . " \\" . PHP_EOL .
                    "    --db "            . $targetFile                      . " \\" . PHP_EOL .
                    "    --query "         . $queryFileNoPath                 . " \\" . PHP_EOL .
                    "    --more-sensitive "                                   . " \\" . PHP_EOL . 
                    "    -k "              . $maxAlignments                   . " \\" . PHP_EOL .
                    "    -e "              . $eValue                          . " \\" . PHP_EOL .
                    "    --subject-cover " . $targetCoverage                  . " \\" . PHP_EOL .
                    "    --query-cover "   . $queryCoverage                   . " \\" . PHP_EOL .
                    "    --out results_blastxml.txt"                          . " & echo $! > PID" . PHP_EOL ;
                }

            }
            // If we're running standard BLAST (tblastn or blastn)
            else
            {
                $command_to_run = $blastEquiv                             . " \\" . PHP_EOL .
                "    -num_threads "     . $jobSettings[0]['num_threads']  . " \\" . PHP_EOL .
                "    -db "              . $targetFile                     . " \\" . PHP_EOL .
                "    -query "           . $queryFileNoPath                . " \\" . PHP_EOL .
                "    -num_alignments "  . $maxAlignments                  . " \\" . PHP_EOL .
                "    -evalue "          . $eValue                         . " \\" . PHP_EOL . 
                "    -outfmt 11"                                          . " \\" . PHP_EOL .
                "    -out blast.archive"                                  . " \\" . PHP_EOL ;

                $command_to_run .= PHP_EOL; // Nice formatting

                # Use blast_formatter (basically diamond view) to generate desired outputs (BLASTXML and Pairwise)
                # BLASTXML
                $command_to_run .= "blast_formatter"                      . " \\" . PHP_EOL .
                "    -archive blast.archive"                              . " \\" . PHP_EOL .
                "    -outfmt 6"                                           . " \\" . PHP_EOL .
                "    -out results_blastxml.txt"                           . " \\" . PHP_EOL ;

                $command_to_run .= PHP_EOL; // Nice formatting

                # Pairwise
                $command_to_run .= "blast_formatter"                      . " \\" . PHP_EOL .
                "    -archive blast.archive"                              . " \\" . PHP_EOL .
                "    -outfmt 0"                                           . " \\" . PHP_EOL .
                "    -out results_pairwise.txt"                           . " & echo $! > PID" . PHP_EOL ;
            

            }
            
            if($debug){drupal_set_message("Will run command: ".$command_to_run);}
            if(!$debug){$remote_diamond_job_id = tripal_add_remote_job("TSeq",$command_to_run,'tripal_seq', array($queryFile),TRUE);}
        }
        else
        {
            //$command_to_run = $blastEquiv." ".$threadsString." 1 --db ".$targetFileNoPath." --query ".$queryFileNoPath." --more-sensitive -k ".$maxAlignments." -e ".$eValue." --subject-cover ".$targetCoverage." --query-cover ".$queryCoverage;
            
            if ($blastEquiv == 'blastp' || $blastEquiv == 'blastx')
            {
                // Do we produce standard output or BLASTXML + Pairwise?
                if ($job_options['produce_diamond_pairwise'])
                {
                    $command_to_run = $diamond_executable . " " . $blastEquiv.
                    "    --threads "         . $jobSettings[0]['num_threads'] . " \\" . PHP_EOL .  
                    "    --db "              . $targetFileNoPath              . " \\" . PHP_EOL .
                    "    --query "           . $queryFileNoPath               . " \\" . PHP_EOL .
                    "    --more-sensitive "                                   . " \\" . PHP_EOL .
                    "    -k "                . $maxAlignments                 . " \\" . PHP_EOL .
                    "    -e "                . $eValue                        . " \\" . PHP_EOL .
                    "    --subject-cover "   . $targetCoverage                . " \\" . PHP_EOL .
                    "    --query-cover "     . $queryCoverage                 . " \\" . PHP_EOL .
                    "    --outfmt 100 "                                       . " \\" . PHP_EOL .
                    "    --out results.daa "                                  . " \\" . PHP_EOL ;

                    // Take DAA format, create BLAST XML output (regular table) and Pairwise output (for showing alignments)
                    $command_to_run .= PHP_EOL;
                    $command_to_run .= $diamond_executable . " view"                         . " \\" . PHP_EOL .
                    "    --daa results.daa "                                  . " \\" . PHP_EOL .
                    "    --outfmt 6 qseqid sseqid pident length mismatch gapopen qstart qend sstart send evalue bitscore" . " \\" . PHP_EOL .
                    "    --out results_blastxml.txt"                          . " \\" . PHP_EOL ;

                    $command_to_run .= PHP_EOL;
                    $command_to_run .= $diamond_executable . " view"                         . " \\" . PHP_EOL .
                    "    --daa results.daa "                                  . " \\" . PHP_EOL .
                    "    --outfmt 0 "                                         . " \\" . PHP_EOL .
                    "    --out results_pairwise.txt "                         . " & echo $! > PID" . PHP_EOL ;
                }
                // Just produce regular output (legacy)
                else
                {
                    $command_to_run = $diamond_executable . " " . $blastEquiv.
                    "    --threads "       . $jobSettings[0]['num_threads']   . " \\" . PHP_EOL .
                    "    --db "            . $targetFileNoPath                      . " \\" . PHP_EOL .
                    "    --query "         . $queryFileNoPath                 . " \\" . PHP_EOL .
                    "    --more-sensitive "                                   . " \\" . PHP_EOL . 
                    "    -k "              . $maxAlignments                   . " \\" . PHP_EOL .
                    "    -e "              . $eValue                          . " \\" . PHP_EOL .
                    "    --subject-cover " . $targetCoverage                  . " \\" . PHP_EOL .
                    "    --query-cover "   . $queryCoverage                   . " \\" . PHP_EOL .
                    "    --out results_blastxml.txt"                          . " & echo $! > PID" . PHP_EOL ;
                }
                
            }
            // If we're running standard BLAST (tblastn or blastn)
            else
            {
                $command_to_run = $blastEquiv                             . " \\" . PHP_EOL .
                "    -num_threads "      . $jobSettings[0]['num_threads'] . " \\" . PHP_EOL .
                "    -db "               . $targetFileNoPath              . " \\" . PHP_EOL .
                "    -query "            . $queryFileNoPath               . " \\" . PHP_EOL .
                "    -num_alignments "   . $maxAlignments                 . " \\" . PHP_EOL .
                "    -evalue "           . $eValue                        . " \\" . PHP_EOL .
                "    -outfmt 11"                                          . " \\" . PHP_EOL .
                "    -out blast.archive"                                  . " \\" . PHP_EOL ;

                $command_to_run .= PHP_EOL; // Nice formatting

                # Use blast_formatter (basically diamond view) to generate desired outputs (BLASTXML and Pairwise)
                # BLASTXML
                $command_to_run .= "blast_formatter"                      . " \\" . PHP_EOL .
                "    -archive blast.archive"                              . " \\" . PHP_EOL .
                "    -outfmt 6"                                           . " \\" . PHP_EOL .
                "    -out results_blastxml.txt"                           . " \\" . PHP_EOL ;

                $command_to_run .= PHP_EOL; // Nice formatting

                # Pairwise
                $command_to_run .= "blast_formatter"                      . " \\" . PHP_EOL .
                "    -archive blast.archive"                              . " \\" . PHP_EOL .
                "    -outfmt 0"                                           . " \\" . PHP_EOL .
                "    -out results_pairwise.txt"                           . " & echo $! > PID" . PHP_EOL ;
            }
            
            if($debug)drupal_set_message("Will run command: ".$command_to_run);
            if(!$debug){$remote_diamond_job_id = tripal_add_remote_job("TSeq",$command_to_run,'tripal_seq', array($targetFile,$queryFile),TRUE);}
        }
        $baseP = base_path();
        
        // Prepare some data for storing in the database (below) and for providing a link
        //Submit time
            $submit_time = time();
        //tseq_unique_id    // Used for identifying this job absolutely and providing links
            if ($blastEquiv == 'blastp' || $blastEquiv == 'blastx') // We are dealing with diamond tool
            {
                $tool_type = 'diamond';
            }
            else
            {
                $tool_type = 'blast';
            }
            //Prepare the entire ID for the job.
            // Get the # of sequences
            $sequence_count = tseq_get_query_sequence_count($queryFile);
            
            $tseq_unique_id = "TUI_".$remote_diamond_job_id."_".$sequence_count."seqs_".$tool_type."_".format_date($submit_time, 'custom', 'Ymd_His');
            drupal_set_message("New unique ID: ".$tseq_unique_id);
        
        //drupal_set_message("Thank you for submitting. An email will be sent to you when the job has finished. You will be able to see the results <a href=\"".$baseP."Diamond/results/$remote_diamond_job_id\"'>here</a>");
        drupal_set_message("Thank you for submitting. You will be able to see the results <a href=\"".$baseP."TSeq/results/$tseq_unique_id\"'>here</a>");
        drupal_set_message("You can submit another job below.");
    }
    else
    {
        drupal_set_message("There seems to have been errors generating the requested job.");
    }
    
    
    /*
     *  Add this requested job to the tseq_job_information table
     */
    
    /*
     *      tseq_job_id             (this is type='serial' so it automatically increments)
     *      tripal_job_id           $remote_diamond_job_id
     *      user_id                     gathered below
     *      submit_time             time();
     *      sequence_query_type     $form_state['values']['QueryType']
     *      database_search_type        generated below
     *      sequence_file_type      $queryFileType
     *      database_file_type      $targetFileType
     *      sequence_file           $queryFile
     *      database_file           $targetFile
     *      command                 $command_to_run
     *      tseq_unique_id              generated above
     */
    
    //ID of User:
        global $user;
        $user_id = $user->uid;
    //Database Search Type
        switch($blastEquiv) {
            case "blastp":
                $database_search_type = 'protein';
                break;
            case "blastx":
                $database_search_type = 'protein';
                break;
            case "tblastn":
                $database_search_type = 'nucleotide';
                break;
            case "blastn":
                $database_search_type = 'nucleotide';
        }
    
    $new_job = array(
        'tripal_job_id'         => $remote_diamond_job_id,
        'user_id'               => $user_id,
        'submit_time'           => $submit_time,
        'sequence_query_type'   => strtolower($form_state['values']['QueryType']),      // i.e. "protein"
        'database_search_type'  => $database_search_type,                               // i.e. "protein"
        'sequence_file_type'    => $queryFileType,                                      // i.e. "upload"
        'database_file_type'    => $targetFileType,                                     // i.e. "database"
        'sequence_file'         => $queryFile,
        'database_file'         => $targetFile,
        'command'               => $command_to_run,
        'tseq_unique_id'        => $tseq_unique_id,
    );
    
    drupal_write_record('tseq_job_information', $new_job);
}