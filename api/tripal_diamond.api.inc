<?php


function tseq_get_job_information($tripal_job_id)
{
    $query = "select * from tseq_job_information where tripal_job_id=".$tripal_job_id;
    $results = db_query($query);
    
    $details = $results->fetchAssoc();
    return $details;
}