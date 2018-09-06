<?php


function tseq_get_job_information($tripal_job_id)
{
    $query = "select * from tseq_job_information where tripal_job_id=".$tripal_job_id;
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