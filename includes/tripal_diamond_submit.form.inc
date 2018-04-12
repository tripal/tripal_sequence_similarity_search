<?php
error_reporting(E_ALL);

//Don't break the form by uploading a file
    function fix_ajax_upload($element, &$form_state, $form) {

    // process $element as normal
    $element = file_managed_file_process($element, $form_state, $form);

    // remove path, add callback
    unset($element['upload_button']['#ajax']['path']);
    $element['upload_button']['#ajax']['callback'] = 'file_ajax_upload_callback';

     return $element;

    }
    
function tripal_diamond_submit_form($form, &$form_state)
{
    /*
     * TODO:    Make sure user is logged in with permission
     *
     */
    $form['Overview'] = array(
        '#type'         => 'fieldset',
        '#title'        => 'Similarity Search Overview',
        '#collapsible'  => FALSE,
        //'#description'  => t('DIAMOND is a BLAST-compatible local aligner for mapping translated DNA query sequences against a protein reference database ( BLASTX alignment mode). The speedup over BLAST many times faster on short reads at a typical sensitivity of 90 - 99% relative to BLAST depending on the data and settings. <br/ >Use the following form to submit a new Diamond job to TreeGenes')
        '#description'  => t('Sequence similarity search is supported in TreeGenes against genes, TreeGenes UniGenes, proteins, and full genomes.  Nucleotide searches are executed with BLAST (BLASTN) while protein or translated protein searches are executed with DIAMOND (BLASTX or BLASTP equivalent).  DIAMOND will execute searches in a fraction of the time of BLAST at a similar sensitivity.  Both packages accept similar input parameters and these can be modified below.  You may upload FASTA formatted sequences or paste FASTA formatted sequences for searching.  You can select from a list of pre-formatted DIAMOND and BLAST databases OR upload your own.  This can be pre-formatted or provided in FASTA format and formatted for you.  In order to execute a search, you must be logged in to the database.  Results are provided to the user in text format.'),
        '#suffix'       => '<br /><br /><h3>Type</h3>'
    );

    $form['QueryType'] = array (
        '#type'         => 'radios',
        //'#title'        => t('Select sequence search type:'),
        //'#description'  => t('Choose the type of query'),
        '#options'      => array(
            'Protein'   => t('Protein'),
            'Gene'      => t('Gene'),
            'Genome'    => t('Genome'),
        ),
        '#prefix'       => 'To get started, choose your search type:'
    );
    
    /*  Query		Target		Program
        Nucleotide	Nucleotide	blastn  (BLAST)
        Nucleotide	Protein		blastx	(Diamond)
        Protein		Nucleotide	tblastn (BLAST)
        Protein		Protein		blastp	(Diamond)  
     */
    
    
    $form['BlastEquivNuc'] = array(
        '#type'         => 'select',
        '#title'        => t('Database Search Type'),
        '#description'  => t('Select what type of Search you would like to run:'),
        '#options'      => array(//'blastn' => t('BLASTn (for use with a Nucleotide Database)'), //Diamond doesn't do this yet
            'blastx' => t('BLASTx (for use with a Protein target database, uses Diamond)'),
            'blastn'  => t('BLASTn (for use with a Nucleotide target database)'),
            ),
        '#states' => array(
            'visible' => array(
                array(
                    array(':input[name="QueryType"]' => array('value' => 'Genome'),),              
                    'and',
                     array(':input[name="QueryType"]' => array('value' => 'Gene'),),
                ),
                
            ),
        ),
        
    );
    
    $form['BlastEquivPro'] = array(
        '#type'         => 'select',
        '#title'        => t('Database Search Type'),
        //'#description'  => t('Select what type of Search you would like to run:'),
        '#options'      => array(//'tblastn' => t('tBLASTn (for use with a Nucleotide Database)'), //Diamond doesn't do this yet
            'blastp' => t('BLASTp (for use with a Protein target database, uses Diamond)'),
            'tblastn' => t('tBLASTn (for use with a Nucleotide target database)'),
            ),
        '#states' => array(
            'visible' => array(
                ':input[name="QueryType"]' => array('value' => 'Protein'),
            ),
        ),
        '#suffix' => '<div class="block"><h3></h3></div>',
    );
    
    $form['QueryDataType'] = array(
        '#type'         => 'radios',
        '#title'        => t('Provide sequence set for search:'),
        //'#description'  => t('Select how you want to upload your query.'),
        '#options'      => array(
            'upload' => t('Upload a File'),
            'paste' => t('Type or paste manually')),
        '#prefix'   =>  '<h3>Query</h3>',
    );
    
    $form['QueryFile'] = array(
        '#type'         => 'managed_file',
        '#title'        => t('Query File'),
        //'#description'  => t('Don\'t forget to hit \'upload\''),
        '#process'      => array('fix_ajax_upload'),
        '#states' => array(
            'visible' => array(
                ':input[name="QueryDataType"]' => array('value' => 'upload'),
            ),
        ),
    );

    
    $form['QueryPaste'] = array(
        '#type'         => 'textarea',
        '#title'        => t('Raw query data'),
        //'#description'  => t('Paste or type your query data into the box above'),
        '#required'     => false,
        '#states' => array(
            'visible' => array(
                ':input[name="QueryDataType"]' => array('value' => 'paste'),
            ),
        ),
    );
    
    /*
     * Provides the user with an example of what the form is expecting
     * Should be query data that actually works
     */
    /*$form['QueryPasteDemo'] = array(
        '#type'         => 'checkbox',
        '#title'        => 'Show an example single sequence',
       // //'#description'  => 'Example Description',
        '#states' => array(
            'visible' => array(
                ':input[name="QueryDataType"]' => array('value' => 'paste'),
            ),
        ),
        '#suffix' => '<div class="block"><h3></h3></div>',
    );*/
    
    
    
    /*
     * Get the available databases
     * Argument specifies type wanted: e.g. 'Nucleotide', 'Protein', or '%' for all
     */
    $databaseListProtein =  DiamondDatabases::getDatabases('Protein');
    $databaseListGene =     DiamondDatabases::getDatabases('Gene');
    $databaseListGenome =   DiamondDatabases::getDatabases('Genome');
    $databaseList_toShow = array();
    $databaseList_toShowProtein['default'] = t('--');
    $databaseList_toShowGene['default'] = t('--');
    $databaseList_toShowGenome['default'] = t('--');
    foreach($databaseListProtein as $dbList)
    {
        $databaseList_toShowProtein[$dbList['db_id']] = $dbList['name'].', version '.$dbList['version'];
    }
    foreach($databaseListGene as $dbList)
    {
        $databaseList_toShowGene[$dbList['db_id']] = $dbList['name'].', version '.$dbList['version'];
    }
    foreach($databaseListGenome as $dbList)
    {
        $databaseList_toShowGenome[$dbList['db_id']] = $dbList['name'].', version '.$dbList['version'];
    }
    
    $form['QueryTargetDBProtein'] = array(
        '#type'         => 'select',
        //'#title'        => t('Query target database (Protein)'),
        //'#description'  => t('Select which database to run against.'),
        '#options'      => $databaseList_toShowProtein,
        '#field_prefix' => '<h3>Target</h3>',
        '#states' => array(
            'visible' => array(
                //':input[name="TargetDataType"]' => array('value' => 'database'),
                ':input[name="QueryType"]' => array('value' => 'Protein'),
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
    
    $form['QueryTargetDBGene'] = array(
        '#type'         => 'select',
        //'#title'        => t('Query target database (Gene)'),
        //'#description'  => t('Select which database to run against.'),
        '#options'      => $databaseList_toShowGene,
        '#field_prefix' => '<h3>Target</h3>',
        '#states' => array(
            'visible' => array(
                //':input[name="TargetDataType"]' => array('value' => 'database'),
                ':input[name="QueryType"]' => array('value' => 'Gene'),
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
    
    $form['QueryTargetDBGenome'] = array(
        '#type'         => 'select',
        //'#title'        => t('Query target database (Genome)'),
        //'#description'  => t('Select which database to run against.'),
        '#options'      => $databaseList_toShowGenome,
        '#field_prefix' => '<h3>Target</h3>',
        '#states' => array(
            'visible' => array(
                //':input[name="TargetDataType"]' => array('value' => 'database'),
                ':input[name="QueryType"]' => array('value' => 'Genome'),
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
    
    $form['OtherTarget'] = array(
      '#type'           => 'fieldset',
      '#title'          => 'Other Target upload options',
      '#collapsible'    => TRUE,
      '#collapsed'      => TRUE,
    );
    
    $form['OtherTarget']['TargetDataType'] = array(
        '#type'         => 'radios',
        '#title'        => t('Provide/Select search database:'),
        //'#description'  => t('Select the database target type'),
        '#default_value'=> 'database',
        '#options'      => array(
            'database' => t('Select from an existing database'),
            'upload' => t('Upload a File'),
            'paste' => t('Type or paste manually'),
            ),
        //'#prefix' => '<h3>Target</h3>'
    );
    
    $form['OtherTarget']['QueryTargetFile'] = array(
        '#type'         => 'managed_file',
        '#title'        => t('Target File'),
        //'#description'  => t('Don\'t forget to hit \'Upload\''),
        '#upload_validators' => array(
            'file_validate_extensions' => array('txt dmnd gz'),
        ),
        '#states' => array(
            'visible' => array(
                ':input[name="TargetDataType"]' => array('value' => 'upload'),
            ),
        ),
    );

    $form['OtherTarget']['QueryTargetPaste'] = array(
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
    
    
    
    function reset_Target($form, &$form_state) {
        $form_state['values']['TargetDataType'] = '';
    }
    
    /*
     * Forms for simple command line options 
     */
    $form['advanced'] = array(
      '#type'           => 'fieldset',
      '#title'          => 'Advanced Options',
      '#collapsible'    => TRUE,
      '#collapsed'      => TRUE,
    );
    
    
    $form['advanced']['eValue'] = array(
        '#type'         =>  'textfield',
        '#title'        =>  'E-value',
        '#size'         =>  '10',
        //'#description'  =>  'Expected number of chance matches in a random model. This number should be give in a decimal format.',
        '#required'     =>  true,
        '#default_value'      =>  t('0.003'),
        //'#prefix'       => '<b>Advanced Options</b>',
        '#field_prefix' => 'Expected number of chance matches in a random model. This number should be give in a decimal format.<br />',
    );
    
    $form['advanced']['targetCoverage'] = array(
        '#type'         =>  'textfield',
        '#title'        =>  'Target Coverage',
        '#size'         =>  '10',
        '#field_prefix'  =>  'Report only alignments above the given percentage of subject cover.<br />',
        '#required'     =>  true,
        '#default_value'      =>  t('50'),
    );
    
    $form['advanced']['queryCoverage'] = array(
        '#type'         =>  'textfield',
        '#title'        =>  'Query Coverage',
        '#size'         =>  '10',
        '#field_prefix'  =>  'Report only alignments above the given percentage of query cover.<br />',
        '#required'     =>  true,
        '#default_value'      =>  t('50'),
    );
    
    $form['advanced']['maxAlignments'] = array(
        '#type'         =>  'select',
        '#title'        =>  'Max Alignments',
        '#field_prefix'  =>  'The maximum number of target sequences per query to report alignments for.<br />',
        '#required'     =>  false,
        '#options'      => array(
            '5'         => t('5'),
            '10'        => t('10'),
            '25'        => t('25'),
            '50'        => t('50'),
            '100'       => t('100'),
            '250'       => t('250'),
            '500'       => t('500'),
            '750'       => t('750'),
            '1000'      => t('1000'),            
            ),
        '#default_value' => '10',
    );
           

    $form['submit_button'] = array(
        '#type' => 'submit',
        '#value' => t('Submit'),
    );
    
    return $form;
}

function tripal_diamond_submit_form_validate($form, &$form_state)
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
    }
    
    /*
     * QueryDataType
     */
    if (empty($form_state['values']['QueryDataType']))
    {
        form_set_error('QueryDataType',t("Please choose a Query Data Type."));
    }
    
    /*
     * QueryFile, QueryPaste
     */
    if (!empty($form_state['values']['QueryDataType']))
    {
        if($form_state['values']['QueryDataType'] == "upload")
        {
            if(empty($form_state['values']['QueryFile']))
            {
                form_set_error('QueryFile',t("Please upload a Query Sequence File"));
            }
        }
        if($form_state['values']['QueryDataType'] == "paste")
        {
            if(empty($form_state['values']['QueryPaste']))
            {
                form_set_error('QueryPaste',t("Please paste a Query Sequence"));
            }
        }        
    }
    
    /*
     * TargetDataType 
    */
    if (empty($form_state['values']['TargetDataType']))
    {
        form_set_error('TargetDataType',t("Please choose a Target Data Type."));
    }    
    
    /*
     * QueryTargetDB, QueryTargetFile, QueryTargetPaste
     */    
    if (!empty($form_state['values']['TargetDataType']))
    {
        if($form_state['values']['TargetDataType'] == "database")
        {
            /*if (($form_state['values']['QueryTargetDB']) == "--")
            {
                form_set_error('QueryTargetDB',t("Please choose a Target Database."));
            }
             * PUMPKIN - make this check all three types as well
             */
        }
        if($form_state['values']['TargetDataType'] == "upload")
        {
            if (empty($form_state['values']['QueryTargetFile']))
            {
                form_set_error('QueryTargetFile',t("Please upload a Target Database File."));
            }     
        }
        if($form_state['values']['TargetDataType'] == "paste")
        {
            if (empty($form_state['values']['QueryTargetPaste']))
            {
                form_set_error('QueryTargetPaste',t("Please paste Target Database raw text."));
            }
        }
            
    }
}

function tripal_diamond_submit_form_submit($form, &$form_state)
{
    
    /*
     * Build the command
     */
    
    //diamond blast* --threads # --db xxxx --(q)uery query.data
    //Get the options/parameters
    if ($form_state['values']['QueryType'] == 'Protein')
    {
        $blastEquiv = 'diamond '.$form_state['values']['BlastEquivPro'];
    }
    else
    {
        $blastEquiv = $form_state['values']['BlastEquivNuc'];
    }
    
    $eValue = $form_state['values']['eValue'];
    $targetCoverage = $form_state['values']['targetCoverage'];
    $queryCoverage = $form_state['values']['queryCoverage'];
    $maxAlignments = $form_state['values']['maxAlignments'];
     
    
    /*
     * Deal with the Query
     */
   //Get any pasted data and save as a file to send
    //Pasted Query:
    if($form_state['values']['QueryDataType'] == 'paste')
    {
        $queryFile = "/tmp/".date('YMd_His').'.fasta';
        file_put_contents($queryFile, $form_state['values']['QueryPaste']);
    }
 
    
    //Get information for uploaded files
    //Query Upload
    if($form_state['values']['QueryDataType'] == 'upload')
    {
        $queryFile = file_load($form_state['values']['QueryFile']);
        if (is_object($queryFile))
        {
          $queryFile = drupal_realpath($queryFile->uri);
          drupal_set_message("Query File: ".$queryFile);
        }
        else
        {
            drupal_set_message("Query File uplaod failed.");//pumpkin
        }        
    }
    
    /*
     * Deal with the Target
     */
    //Pasted Target
    if($form_state['values']['TargetDataType'] == 'paste')
    {
        $targetFile = "/tmp/".date('YMd_His').'.dmnd';
        file_put_contents($targetFile, $form_state['values']['QueryTargetPaste']);
    }
    
    //Target Upload
    if($form_state['values']['TargetDataType'] == 'upload')
    {
        $targetFile = file_load($form_state['values']['QueryTargetFile']);
        if (is_object($targetFile))
        {
          $targetFile = drupal_realpath($targetFile->uri);
          drupal_set_message("Target File: ".$targetFile);
        }
        else
        {
            drupal_set_message("Target File upload failed.");
        }        
    }
    //Get path for selected pre-existing database
    //Target DB
    if($form_state['values']['TargetDataType'] == "database")
    {
        if($form_state['values']['QueryType'] == "Protein")
        {
            $targetDB = $form_state['values']['QueryTargetDBProtein'];
            drupal_set_message($form_state['values']['QueryType']);
        }
        if($form_state['values']['QueryType'] == "Gene")
        {
            $targetDB = $form_state['values']['QueryTargetDBGene'];
            drupal_set_message($form_state['values']['QueryType']);
        }
        if($form_state['values']['QueryType'] == "Genome")
        {
            $targetDB = $form_state['values']['QueryTargetDBGenome'];
            drupal_set_message($form_state['values']['QueryType']);
        }
        $targetDBU = strtoupper($targetDB);
        //$targetDBPath = "/linuxshare/projects/treegenes/tgwebprod_store/FTP/Genomes/".$targetDB."/index/".$targetDBU."_dmndBlast/".$targetDBU."_diamond/";
        //$targetDBFile = $targetDBU.".dmnd";
        //
        $targetFile = DiamondDatabases::getDBInfo($targetDB,"location");
        drupal_set_message("targetdbfile: ".$targetDB);
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
    
    drupal_set_message("Query File No path: ".$queryFileNoPath);
    drupal_set_message("Target File No path: ".$targetFileNoPath);
    
    //Are we running a Diamond job or straight BLAST?
    // Is this block useless? pumpkin
    if ($form_state['values']['QueryType'] == "Protein")
    {
        $command_to_run = "diamond ";
    }
    else
    {
        $command_to_run = " ";
    }
    
    
    if ($form_state['values']['TargetDataType'] == 'database')
    {
        $command_to_run = $blastEquiv." --threads 1 --db ".$targetFile." --query ".$queryFileNoPath." --more-sensitive -k ".$maxAlignments." -e ".$eValue." --subject-cover ".$targetCoverage." --query-cover ".$queryCoverage;
        $command_to_run = $command_to_run." & echo $! > PID"; //Get PID so we can check up on the job
        drupal_set_message("Will run command: ".$command_to_run);
        //$remote_diamond_job_id = tripal_add_remote_job("Diamond",$command_to_run, array($queryFile),FALSE);
    }
    else
    {
        $command_to_run = $blastEquiv." --threads 1 --db ".$targetFileNoPath." --query ".$queryFileNoPath." --more-sensitive -k ".$maxAlignments." -e ".$eValue." --subject-cover ".$targetCoverage." --query-cover ".$queryCoverage;
        $command_to_run = $command_to_run." & echo $! > PID"; //Get PID so we can check up on the job
        drupal_set_message("Will run command: ".$command_to_run);
        //$remote_diamond_job_id = tripal_add_remote_job("Diamond",$command_to_run, array($targetFile,$queryFile),FALSE);
    }
    
    drupal_set_message("Thank you for submitting. An email will be sent to you when the job has finished. You will be able to see the results <a href=\"/Drupal/Diamond/results/$remote_diamond_job_id\"'>here</a>");
    
    
    //$form_state['redirect'] = 'Diamond/results/'.$remote_diamond_job_id;
    /************************************************************/
    /************************************************************/
    /************************************************************/
    
    /*
     * Some old garbage below for reference
     */
    
    /*drupal_set_message(t('The thing has been did'));
    $form_values = $form_state['values'];
    $thing = $form_values['QueryTargetPaste'];
    drupal_set_message($thing);
     * 
     */
    
    //$targetType = $form_state['values']['TargetType'];
    
    //drupal_set_message("Everything looks good!");
    //drupal_set_message("Make sure everything is correct below and then hit \"Submit\"");
   
    
    //tripal_diamond_confirm_form($form, $form_state, $form_values);
}