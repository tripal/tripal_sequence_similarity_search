<?php

function tseq_user_analysis_page($uid) {
  
    /*
     * Get the jobs belonging to this user
     */
    global $user;
    $user_id = $user->uid;
        
    /*
     *  Build the Table
     */
    
    // Header
    $header = array(
        array('data' => 'Job ID',       'field' => 'd.tripal_job_id',           'sort' => 'asc'),
        array('data' => 'Submit Time',  'field' => 'd.submit_time'),
        array('data' => 'Query Type',   'field' => 'd.sequence_query_type'),
        array('data' => 'Search Type',  'field' => 'd.database_search_type'),
        array('data' => 'Link'),
        array('data' => 'Status',       'field' => 't.status')
    );
    
    // Query
    $query = db_select('tseq_job_information','d');
    
    // Join tseq_job_information and tripal_job on tripal_job_id=job_id, respectively
    $query->join('tripal_jobs','t','d.tripal_job_id=t.job_id');
    
    // Which fields are we interested in the database?
    $query->fields('d', array('tripal_job_id','submit_time','sequence_query_type','database_search_type','user_id'))
          ->fields('t', array('status'));
    
    // Extend the table with pagination and sorting, add other specifications
    $query->extend('PagerDefault')
          ->extend('TableSort')
          ->limit(5)
          ->orderByHeader($header);
    
    // Only return jobs belonging to the user
    $query->condition('d.user_id',$user_id,'=');
    
    // Execute the query
    $userJobs = $query->execute();
    
    // Build the table rows from the returned database data
    $job_list = array();
    foreach($userJobs as $job)
    {
        $link = l('Your results','/TSeq/results/'.$job->tripal_job_id);
        
        $job_list[] = array(
            $job->tripal_job_id,
            format_date($job->submit_time, 'short'),
            ucwords($job->sequence_query_type),
            ucwords($job->database_search_type),
            $link,
            $job->status,
        );
    }
    
    // Give the table a class if styling is desired 
    $job_table_attributes = array(
        'class' => array(
            'jobTable',
        ),
    );
    
    // Let the users know they can sort the columns
    $job_table_caption = 'Sort columns by clicking on a header';
    
    // Build the table variable array
    $job_table_vars = array(
            'header'        => $header,
            'rows'          => $job_list,
            'attributes'    => $job_table_attributes,
            'caption'       => $job_table_caption
        );
    
    $content = theme('table',$job_table_vars);
    $content .= theme('pager');
    /* Debug
    dpm($user_id);
    dpm($header);
    dpm($userJobs);
    */
    
    
    return $content;
}