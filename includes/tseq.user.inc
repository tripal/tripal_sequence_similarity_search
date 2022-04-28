<?php

/**
 * Implements hook_page()
 *   Builds a tab that appears within the user's profile and displays a table
 *   of all their previous TSeq jobs with links to the individual results 
 *   pages.
 */
function tseq_user_analysis_page($uid) {

    /*
     * Get the jobs belonging to this user (do not rely on the URL)
     */
    global $user;
    $user_id = $user->uid;

    // Tell the user that a different user's page was requested but we will not show it.
    if ($uid != $user_id) {
        drupal_set_message("A different user ID was given in the URL than yours. However, this page still reflects your results. 
        If you want to view the results of another user, please contact them directly. Click " . 
        l("here", "user/" . $user_id . "/tseq_analysis") . " to get your direct URL.","warning");
    }

    /*
     *  Build the Table
     */

    // Header
    $header = array(
        array('data' => 'Tripal ID',       'field' => 'd.tseq_unique_id',),
        array('data' => 'Submit Time',  'field' => 'd.submit_time',             'sort' => 'desc'),
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
    $query->fields('d', array('tripal_job_id','submit_time','sequence_query_type','database_search_type','user_id','tseq_unique_id'))
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
        $link = l('Your results','/TSeq/results/'.$job->tseq_unique_id);
        
        $job_list[] = array(
            $job->tseq_unique_id,
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

    return $content;
}