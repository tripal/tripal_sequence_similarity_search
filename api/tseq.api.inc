<?php


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

// Retrieve the db_id of one of the stored existing databases (tseq_db_existing_locations)
// This seems a little dirty since 'location' is not a primary key and there are no uniqueness constraints on it
function tseq_get_db_id_by_location($location)
{
    $query = "SELECT db_id FROM tseq_db_existing_locations WHERE location = '$location'";
    $results = db_query($query);
    
    $db_id = $results->fetchAssoc();
    return $db_id['db_id'];
}

// Retrieve information about a given database 
function tseq_get_db_info($db_id)
{
    $query = "SELECT * FROM tseq_db_existing_locations WHERE db_id = '$db_id'";
    $results = db_query($query);
    
    $details = $results->fetchAssoc();
    return $details;
}

// Retrieve the number of sequences in the user's query
function tseq_get_query_sequence_count($query_file)
{
    $cmd = "grep '>' $query_file | wc -l";
    exec($cmd, $output);
    
    return $output[0];
}

// Retrieve the number of matches in the output file
function tseq_get_matches_count($output_file)
{
    $cmd = "wc -l $output_file";
    exec($cmd, $output);
    $line_count = explode(' ', $output[0]);
    return $line_count[0];
}

/*
 *  Are the results for the requested job in the table yet?
 */
function tseq_results_exist($tripal_job_id)
{
    $query = "SELECT count(tripal_job_id) from tseq_results where tripal_job_id = $tripal_job_id";
    $results = db_query($query);
    
    $count = $results->fetchAssoc();
    return $count['count'];
}


/**
 * 
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
 * @param type $query_text
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
    $pattern = '/([\n]{2})([A-Z])/';	
    $fixed = preg_replace_callback($pattern, 'step1', $contents);

    function step1($matches){
            global $sequenceNo;
            return "\n\n>ADDEDSEQUENCE_TYPE1 " . $sequenceNo++ . "\n" . $matches[2];
    }
    #	
    # Step 2: Make sure at least the first sequence has a header
    #
    $pattern2 = '/(^[A-Z])/';
    $fixed2 = preg_replace_callback($pattern2, 'step2', $fixed);

    function step2($matches){
            global $sequenceNo;
            return ">ADDEDSEQUENCE_TYPE2 " . $sequenceNo++ . "\n" . $matches[0];
    }
	
	
    #
    # Step 3: Put a newline between the end of a sequence and the header of the next
    #
    $pattern3 ='/([A-z])(\n{1})(>)/';
    $fixed3 = preg_replace_callback($pattern3, 'step3', $fixed2);

    function step3($matches){
            return $matches[1] . "\n" . $matches[2] . $matches[3];
    }
        
    return $fixed3;
}


// Validate the user's sequences
// Returns true on valid sequence
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