<?php

/**
 *  @param int $tripal_job_id The ID of the tripal Job about which to return information
 * 
 *  @return array Details from the database about the given job
 *   
 */
function tseq_get_job_information($tripal_job_id)
{
    $query = "select * from tseq_job_information where tripal_job_id=".$tripal_job_id;
    $results = db_query($query);
    
    $details = $results->fetchAssoc();
    return $details;
}

/**
 *  Retrieve the job results (all data)
 *  @param integer $job_id The Tripal Job ID used to run this job
 * 
 *  @return array Array of results from the database concerning this job
 */
function tseq_get_job_results($tripal_job_id)
{
    $query = "select * from tseq_results where tripal_job_id=".$tripal_job_id;
    $results = db_query($query);
    
    $details = $results->fetchAssoc();
    return $details;
}

/**
 * Retrieve the database ID of the database whose path is given.
 * This seems a little dirty since 'location' is not a primary key and there are no uniqueness constraints on it
 * @param string $location A full path-on-disk to where the file is
 * 
 * @return int The ID of the database that has the associated location
 */
function tseq_get_db_id_by_location($location)
{
    $query = "SELECT db_id FROM tseq_db_existing_locations WHERE location = '$location'";
    $results = db_query($query);
    
    $db_id = $results->fetchAssoc();
    return $db_id['db_id'];
}

/**
 * Retrieve information about a given database
 * @param int $db_id The database ID of the desired database about which to return info about
 * 
 * @return array Details about the database or NULL if the database doesn't exist.
 */
function tseq_get_db_info($db_id)
{
    $query = "SELECT * FROM tseq_db_existing_locations WHERE db_id = '$db_id'";
    try {
        $results = db_query($query);
        $details = $results->fetchAssoc();
        return $details;
    } catch (Exception $e) {
        echo "Error looking up info for $db_id\n";
    }
    return NULL;
    
}

/**
 * Get the list of databases stored in the database
 * (table: tseq_db_existing_locations)
 */
function tseq_get_all_dbs()
{
    $query = "SELECT * FROM tseq_db_existing_locations";
    $results = db_query($query);

    $databases = $results->fetchAll();
    return $databases;
}

/**
 * Retrieve the number of sequences in the user's query
 * @param string $query_file Uses simple (hopefully POSIX compliant) shell commands
 * to fetch the number of sequences in the given $query_file
 * 
 * @return int number of sequences
 */
function tseq_get_query_sequence_count($query_file)
{
    $cmd = "grep '>' $query_file | wc -l";
    exec($cmd, $output);
    
    return trim($output[0]);
}

/**
 * Retrieve the number of matches in the output file
 * @param string $output_file The file in which to count the matches
 * 
 * @return int The number of matches found (based on line count)
 */
function tseq_get_matches_count($output_file)
{
    $matches = intval(exec("wc -l '$output_file'"));
    return $matches;
}

/**
 * Checks to see if the results for the requested job are in the table yet
 * @param int $tripal_job_id The job ID
 * 
 * @return int A count of the results that exist in the database. 0 indicates none,
 * i.e. job still not completed, or other error.
 */
function tseq_results_exist($tripal_job_id)
{
    $query = "SELECT count(tripal_job_id) from tseq_results where tripal_job_id = $tripal_job_id";
    $results = db_query($query);
    
    $count = $results->fetchAssoc();
    return $count['count'];
}


/**
 * Determines which program to use based on the types of files provided by the user.
 * Probably deprecated
 * @param string $sequence_query_type
 * @param string $database_search_type
 * @return array Type of job, program name
 */
function tseq_get_program($sequence_query_type, $database_search_type)
{
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
    
    if ($sequence_query_type == 'genomic')
    {
        if ($database_search_type == 'protein')
        {
            return array('Diamond','blastx');
        }
        else
        {
            return array('BLAST','blastn');
        }
    }
    else
    {
        if ($database_search_type == 'protein')
        {
            return array('Diamond','blastp');
        }
        else
        {
            return array('BLAST','tblastn');
        }
    }
}

/**
 * Will add missing headers (numbered) and proper newline formatting to query submissions
 * 
 * @param string $query_text
 * 
 * @return string A fixed (if applicable) sequence string
 */
function tseq_fix_query($query_text)
{
    # Keep track of inserted sequence #
    $sequenceNo = 1;
    ## Process the file with regular expressions
	#$fixed = preg_replace_callback($pattern, functionName, $contents);
    #
    # Step 1: Find instances of 2 newline characters and no headers
    #
    function step1($matches){
            global $sequenceNo;
            return "\n\n>ADDEDSEQUENCE_TYPE1 " . $sequenceNo++ . "\n" . $matches[2];
    }
    $pattern = '/([\n]{2})([A-Z])/';	
    $fixed = preg_replace_callback($pattern, 'step1', $query_text);

    #	
    # Step 2: Make sure at least the first sequence has a header
    #
    function step2($matches){
            global $sequenceNo;
            return ">ADDEDSEQUENCE_TYPE2 " . $sequenceNo++ . "\n" . $matches[0];
    }
    $pattern2 = '/(^[A-Z])/';
    $fixed2 = preg_replace_callback($pattern2, 'step2', $fixed);

	
	
    #
    # Step 3: Put a newline between the end of a sequence and the header of the next
    #
    function step3($matches){
            return $matches[1] . "\n" . $matches[2] . $matches[3];
    }
    $pattern3 ='/([A-z])(\n{1})(>)/';
    $fixed3 = preg_replace_callback($pattern3, 'step3', $fixed2);

        
    return $fixed3;
}

/**
 * Checks to see if the user's provided search sequence is valid
 * @param string $type The type of sequence (nucleotide - "Genomic" or Protein - "Protein)
 * @param string $sequence The user-submitted sequence to validate
 * 
 * @return boolean True if the sequence is valid, false if not
 */
function tseq_validate_sequence($type, $sequence)
{
    // Are we dealing with a nucleotide or a protein?
    if ($type == 'Genomic')
    {
        $fastaIdRegEx = '/^>.*(\\n|\\r)/';
        $fastaSeqRegEx = '/[^ATCGNUKMBVSWDYRHatcgnukmbvswdyrh\n\r]/'; //Includes IUPAC codes.
        if ( preg_match($fastaSeqRegEx,$sequence) && !(preg_match($fastaIdRegEx,$sequence)) )
        {
            return FALSE;
        }
        else
        {
            return TRUE;
        }
    }
    else if ($type == 'Protein')
    {
        $fastaIdRegEx = '/^>.*(\\n|\\r)/';
        $fastaSeqRegEx = '/[^ABCDEFGHIKLMNPQRSTUVWYZXabcdefghiklmnpqrstuvwyzx\*\-\n\r]/';
        if ( preg_match($fastaSeqRegEx,$sequence) && !(preg_match($fastaIdRegEx,$sequence)) )
        {
            return FALSE;
        }
        else
        {
            return TRUE;
        }
    }
    else
    {
        return "error";
    }
}