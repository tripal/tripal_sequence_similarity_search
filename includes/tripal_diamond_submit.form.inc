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
        '#title'        => t('Select sequence search type:'),
        //'#description'  => t('Choose the type of query'),
        '#options'      => array(
            'Protein'   => t('Protein'),
            'Genomic'      => t('Genomic (coding or non-coding)'),
            #'Genome'    => t('Genome'),
        ),
        '#prefix'           => '<h2>Type</h2>',
        //'#field_prefix'       => 'To get started, choose your search type:',
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
            #'blastx' => t('BLASTx (for use with a Protein target database, uses Diamond)'),
            #'blastn'  => t('BLASTn (for use with a Nucleotide target database)'),
            'blastx' => t('BLASTx (Translated nucleotide versus protein)'),
            'blastn'  => t('BLASTn (Nucleotide versus nucleotide)'),
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
            'blastp' => t('BLASTp (Protein versus protein)'),
            'tblastn' => t('tBLASTn (Protein versus translated nucleotide)'),
            ),
        '#states' => array(
            'visible' => array(
                ':input[name="QueryType"]' => array('value' => 'Protein'),
            ),
        ),
    );
    
    /*
     * Get the available databases
     */
    $databaseListProtein =  DiamondDatabases::getDatabasesFiltered(array('Protein'), array('type'));
    $databaseListGene =     DiamondDatabases::getDatabasesFiltered(array('Gene'), array('type'));
    $databaseListGenome =   DiamondDatabases::getDatabasesFiltered(array('Genome'), array('type'));
    $databaseListUnigenePro =  DiamondDatabases::getDatabasesFiltered(array('Unigene','Protein'), array('name','Type'));
    $databaseListUnigeneNuc =  DiamondDatabases::getDatabasesFiltered(array('Unigene','Gen'), array('name','Type'));
    $databaseList_toShow = array();
    $databaseList_toShowProtein['default'] = t('--');
    $databaseList_toShowGene['default'] = t('--');
    $databaseList_toShowGenome['default'] = t('--');
    $databaseList_toShowUnigenePro['default'] = t('--');
    $databaseList_toShowUnigeneNuc['default'] = t('--');
    
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
    foreach($databaseListUnigenePro as $dbList)
    {
        $databaseList_toShowUnigenePro[$dbList['db_id']] = $dbList['name'].', version '.$dbList['version'];
    }
    foreach($databaseListUnigeneNuc as $dbList)
    {
        $databaseList_toShowUnigeneNuc[$dbList['db_id']] = $dbList['name'].', version '.$dbList['version'];
    }
    
    $form['Target'] = array(
      '#type'           => 'container',
      '#prefix'         => '<h2>Target/Database Selection</h2><div id="targetContainer" style="padding-left:15px;">',
      '#suffix'         => '</div>',
    );
    
    /*
     * Target Type: Protein
     * Target Name: TreeGenes Protein
     */
    $form['Target']['TargetDBProtein'] = array(
        '#type'         => 'select',
        '#title'        => t('TreeGenes Proteins'),
        //'#description'  => t('Select which database to run against.'),
        '#options'      => $databaseList_toShowProtein,
        //'#field_prefix' => '<h3>TreeGenes Proteins</h3>',
        '#prefix' => 'Choose one of the following',
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
    /*
     * Target Type: Protein
     * Target Name: UniGenes Protein
     */
    $form['Target']['TargetDBUnigenePro'] = array(
        '#type'         => 'select',
        '#title'        => t('TreeGenes UniGenes Protein'),
        //'#description'  => t('Select which database to run against.'),
        '#options'      => $databaseList_toShowUnigenePro,
       //'#field_prefix' => '<h3>TreeGenes Unigene Protein</h3>',
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
        //Move field over a little '#prefix' => '<div style="padding-left:50px">',
        //Move field over a little '#suffix' => '</div>',
    );
    
    /*
     * Target Type: Genomic
     * Target Name: TreeGenes Genes
     */
    $form['Target']['TargetDBGene'] = array(
        '#type'         => 'select',
        '#title'        => t('TreeGenes Genes'),
        //'#description'  => t('Select which database to run against.'),
        '#options'      => $databaseList_toShowGene,
        //'#field_prefix' => '<h3>TreeGenes Genes</h3>',
        '#states' => array(
            'visible' => array(
                //':input[name="TargetDataType"]' => array('value' => 'database'),
                ':input[name="QueryType"]' => array('value' => 'Genomic'),
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
    
    /*
     * Target Type: Genomic
     * Target Name: TreeGenes Genomes
     */
    $form['Target']['TargetDBGenome'] = array(
        '#type'         => 'select',
        '#title'        => t('TreeGenes Genomes'),
        //'#description'  => t('Select which database to run against.'),
        '#options'      => $databaseList_toShowGenome,
        //'#field_prefix' => '<h3>TreeGenes Genomes</h3>',
        '#states' => array(
            'visible' => array(
                //':input[name="TargetDataType"]' => array('value' => 'database'),
                ':input[name="QueryType"]' => array('value' => 'Genomic'),
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
    
    /*
     * Target Type: Genomic
     * Target Name: TreeGenes UniGenes Nucleotide
     */
    $form['Target']['TargetDBUnigeneNuc'] = array(
        '#type'         => 'select',
        '#title'        => t('TreeGenes Unigenes Nucleotide'),
        //'#description'  => t('Select which database to run against.'),
        '#options'      => $databaseList_toShowUnigeneNuc,
        //'#field_prefix' => '<h3>TreeGenes Unigene Nucleotide</h3>',
        '#states' => array(
            'visible' => array(
                //':input[name="TargetDataType"]' => array('value' => 'database'),
                ':input[name="QueryType"]' => array('value' => 'Genomic'),
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
    
    $form['Target']['OtherTarget'] = array(
      '#type'           => 'fieldset',
      '#title'          => 'Other Target upload options',
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
    
    $form['Target']['OtherTarget']['TargetFile'] = array(
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
    /*
     *  Query Input Section
     */
    
    $form['Query'] = array(
      '#type'           => 'container',
      '#prefix'         => '<h2>Query Sequence</h2>',
    );
    //We don't need this anymore since we're always showing everything
    /*$form['QueryDataType'] = array(
        '#type'         => 'radios',
        '#title'        => t('Provide sequence set for search:'),
        //'#description'  => t('Select how you want to upload your query.'),
        '#options'      => array(
            'upload' => t('Upload a File'),
            'paste' => t('Type or paste manually')),
        '#prefix'   =>  '<h2>Query</h2>',
    );*/
    
    $form['Query']['QueryFile'] = array(
        '#type'         => 'managed_file',
        '#title'        => t('Query File'),
        //'#description'  => t('Don\'t forget to hit \'upload\''),
        '#process'      => array('fix_ajax_upload'),
        /*'#states' => array(
            'visible' => array(
                ':input[name="QueryDataType"]' => array('value' => 'upload'),
            ),
        ),*/
    );

    
    $form['Query']['QueryPaste'] = array(
        '#type'         => 'textarea',
        '#title'        => t('Raw query data'),
        //'#description'  => t('Paste or type your query data into the box above'),
        '#required'     => false,
        '#prefix'       => '<b>OR</b>',
        /*'#states' => array(
            'invisible' => array(
                ':input[name="QueryDataType"]' => array('value' => 'upload'),
            ),
        ),*/
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
     * Forms for simple command line options 
     */
    $form['Advanced'] = array(
      '#type'           => 'fieldset',
      '#title'          => 'Advanced Options',
      '#collapsible'    => TRUE,
      '#collapsed'      => TRUE,
    );
    
    
    $form['Advanced']['eValue'] = array(
        '#type'         =>  'textfield',
        '#title'        =>  'E-value',
        '#size'         =>  '10',
        //'#description'  =>  'Expected number of chance matches in a random model. This number should be give in a decimal format.',
        '#required'     =>  true,
        '#default_value'      =>  t('0.003'),
        //'#prefix'       => '<b>Advanced Options</b>',
        '#field_prefix' => 'Expected number of chance matches in a random model. This number should be give in a decimal format.<br />',
    );
    
    $form['Advanced']['targetCoverage'] = array(
        '#type'         =>  'textfield',
        '#title'        =>  'Target Coverage',
        '#size'         =>  '10',
        '#field_prefix'  =>  'Report only alignments above the given percentage of subject cover.<br />',
        '#required'     =>  true,
        '#default_value'      =>  t('50'),
    );
    
    $form['Advanced']['queryCoverage'] = array(
        '#type'         =>  'textfield',
        '#title'        =>  'Query Coverage',
        '#size'         =>  '10',
        '#field_prefix'  =>  'Report only alignments above the given percentage of query cover.<br />',
        '#required'     =>  true,
        '#default_value'      =>  t('50'),
    );
    
    $form['Advanced']['maxAlignments'] = array(
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
        '#suffix' => t('<p>Upon Submission, your search will enter the '
                . 'queue with either BLAST or Diamond as the engine. You will'
                . ' be given a link to view the progress and results of the '
                . 'job.</p>')
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
    
    $targetsProvidedPro = 0;
    $targetsProvidedNuc = 0;
    //Check each target selection to see if they have stuff
    //Proteins
    if ($form_state['values']['TargetDBProtein'] != 'default')
    {
        $targetsProvidedPro = $targetsProvidedPro + 1;
    }
    if ($form_state['values']['TargetDBUnigenePro'] != 'default')
    {
        $targetsProvidedPro = $targetsProvidedPro + 1;
    }
    //Genomic
    if ($form_state['values']['TargetDBGene'] != 'default')
    {
        $targetsProvidedNuc = $targetsProvidedNuc + 1;
    }
    if ($form_state['values']['TargetDBGenome'] != 'default')
    {
        $targetsProvidedNuc = $targetsProvidedNuc + 1;
    }
    if ($form_state['values']['TargetDBUnigeneNuc'] != 'default')
    {
        $targetsProvidedNuc = $targetsProvidedNuc + 1;
    }
    
    
    //Handle if there are more than 1 targets selected
    if ($form_state['values']['QueryType'] == 'Protein' && $form_state['values']['TargetDataType'] == 'database')
    {
        if ($targetsProvidedPro > 1)
        {
            form_set_error('Target',t("Please choose only 1 target."));
            if ($form_state['values']['TargetDBProtein'] != 'default')
            {
               form_set_error('TargetDBProtein');
            }
            if ($form_state['values']['TargetDBUnigenePro'] != 'default')
            {
               form_set_error('TargetDBUnigenePro');
            }
        }
        else if ($targetsProvidedPro < 1)
        {
            form_set_error('Target',t("Please choose a target"));
        }
    }
    else if ($form_state['values']['QueryType'] == 'Genomic' && $form_state['values']['TargetDataType'] == 'database')
    {
        if ($targetsProvidedNuc > 1)
        {
            form_set_error('Target',t("Please choose only 1 target."));
            if ($form_state['values']['TargetDBGene'] != 'default')
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
        }
        else if ($targetsProvidedNuc < 1)
        {
            form_set_error('Target',t("Please choose a target"));
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
    if(!empty($form_state['values']['QueryFile']))
    {
        $querysProvided = $querysProvided + 1;
    }
    if(!empty($form_state['values']['QueryPaste']))
    {
        $querysProvided = $querysProvided + 1;
    }
    if ($querysProvided > 1)
    {
        form_set_error('Query',t("Please only provide one sequence."));
    }
    else if ($querysProvided < 1)
    {
        form_set_error('Query',t("Please provide a sequence to query."));
        form_set_error('QueryFile');
        form_set_error('QueryPaste');
    }
    /*
     * QueryDataType
     
    if (empty($form_state['values']['QueryDataType']))
    {
        form_set_error('QueryDataType',t("Please choose a Query Data Type."));
    }*/
    
    /*
     * QueryFile, QueryPaste
     
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
     * 
     */
    
    /*
     * TargetDataType 
    
    if (empty($form_state['values']['TargetDataType']))
    {
        form_set_error('TargetDataType',t("Please choose a Target Data Type."));
    } */   
    
    /*
     * QueryTargetDB, QueryTargetFile, QueryTargetPaste
     */    
    /*
    if (!empty($form_state['values']['TargetDataType']))
    {
        if($form_state['values']['TargetDataType'] == "database")
        {
            
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
    */
    
}

function tripal_diamond_submit_form_submit($form, &$form_state)
{
    $debug = FALSE;
    /*
     * Build the command
     */
    
    //diamond blast* --threads # --db xxxx --(q)uery query.data
    //Get the options/parameters
    
    
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
        $queryFile = "/tmp/".date('YMd_His').'.fasta';
        file_put_contents($queryFile, $form_state['values']['QueryPaste']);
    }
 
    
    //Get information for uploaded files
    //Query Upload
    if(!empty($form_state['values']['QueryFile']))
    {
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
        $targetFile = "/tmp/".date('YMd_His').'.dmnd';
        file_put_contents($targetFile, $form_state['values']['TargetPaste']);
    }
    
    //Target Upload
    if($form_state['values']['TargetDataType'] == 'upload')
    {
        $targetFile = file_load($form_state['values']['TargetFile']);
        if (is_object($targetFile))
        {
          $targetFile = drupal_realpath($targetFile->uri);
          if($debug){drupal_set_message("Target File: ".$targetFile);}
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
            if (($form_state['values']['TargetDBProtein']) != 'default')
            {
                echo "TargetDBProtein was selected";
                $targetDB = $form_state['values']['TargetDBProtein'];
            }
            if (($form_state['values']['TargetDBUnigenePro']) != 'default')
            {
                echo "TargetDBUnigenePro was selected";
                $targetDB = $form_state['values']['TargetDBUnigenePro'];
            }            
        }
        if($form_state['values']['QueryType'] == "Genomic")
        {
            if (($form_state['values']['TargetDBGene']) != 'default')
            {
                $targetDB = $form_state['values']['TargetDBGene'];
            }
            if (($form_state['values']['TargetDBGenome']) != 'default')
            {
                $targetDB = $form_state['values']['TargetDBGenome'];
            }
            if (($form_state['values']['TargetDBUnigeneNuc']) != 'default')
            {
                $targetDB = $form_state['values']['TargetDBUnigeneNuc'];
            }
        }
        
        $targetDBU = strtoupper($targetDB);
        //$targetDBPath = "/linuxshare/projects/treegenes/tgwebprod_store/FTP/Genomes/".$targetDB."/index/".$targetDBU."_dmndBlast/".$targetDBU."_diamond/";
        //$targetDBFile = $targetDBU.".dmnd";
        //
        $targetFile = DiamondDatabases::getDBInfo($targetDB,"location");
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
            $blastEquiv = "diamond blastp";
        }
        elseif ($form_state['values']['BlastEquivPro'] == "tblastn")
        {
            $blastEquiv = "tblastn";
        }
    }
    elseif ($form_state['values']['QueryType'] == "Genomic")
    {
        if ($form_state['values']['BlastEquivNuc'] == "blastx")
        {
            $blastEquiv = "diamond blastx";
        }
        elseif ($form_state['values']['BlastEquivNuc'] == "blastn")
        {
            $blastEquiv = "blastn";
        }
    }
    
    
    /*
    if (($form_state['values']['QueryType'] == "Protein") && ($form_state['values']['BlastEquivPro'] == "blastx"))
    {
        $command_to_run = "diamond ";
        $blastEquiv = "blastx";
    }
    elseif (($form_state['values']['QueryType'] == "Genomic") && ($form_state['values']['BlastEquivNuc'] == "blastp"))
    {
        $command_to_run = "diamond ";
        $blastEquiv = "blastp";
    }
    elseif (($form_state['values']['QueryType'] == "Protein") && ($form_state['values']['BlastEquivPro'] == "tblastn"))
    {
        $command_to_run = " ";
        $blastEquiv = "tblastn";
    }
    elseif (($form_state['values']['QueryType'] == "Genomic") && ($form_state['values']['BlastEquivNuc'] == "blastn"))
    {
        $command_to_run = " ";
        $blastEquiv = "blastn";
    }
    */
        
    $remote_diamond_job_id = -1;
    if ($form_state['values']['TargetDataType'] == 'database')
    {
        $command_to_run = $blastEquiv." --threads 1 --db ".$targetFile." --query ".$queryFileNoPath." --more-sensitive -k ".$maxAlignments." -e ".$eValue." --subject-cover ".$targetCoverage." --query-cover ".$queryCoverage;
        //$command_to_run = $command_to_run." & echo $! > PID"; //Get PID so we can check up on the job
        if($debug){drupal_set_message("Will run command: ".$command_to_run);}
        if(!$debug){$remote_diamond_job_id = tripal_add_remote_job("Diamond",$command_to_run, array($queryFile),FALSE);}
    }
    else
    {
        $command_to_run = $blastEquiv." --threads 1 --db ".$targetFileNoPath." --query ".$queryFileNoPath." --more-sensitive -k ".$maxAlignments." -e ".$eValue." --subject-cover ".$targetCoverage." --query-cover ".$queryCoverage;
        //$command_to_run = $command_to_run." & echo $! > PID"; //Get PID so we can check up on the job
        if($debug)drupal_set_message("Will run command: ".$command_to_run);
        if(!$debug){$remote_diamond_job_id = tripal_add_remote_job("Diamond",$command_to_run, array($targetFile,$queryFile),FALSE);}
    }
    $baseP = base_path();
    drupal_set_message("Thank you for submitting. An email will be sent to you when the job has finished. You will be able to see the results <a href=\"".$baseP."Diamond/results/$remote_diamond_job_id\"'>here</a>");
    
    
    //$form_state['redirect'] = 'Diamond/results/'.$remote_diamond_job_id;
 
}