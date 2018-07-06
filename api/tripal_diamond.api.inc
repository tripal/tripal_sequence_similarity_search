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