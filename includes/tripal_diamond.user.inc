<?php

function tripal_diamond_user_analysis_page($uid) {
  
    /*
     * Get the jobs belonging to this user
     */
    global $user;
        $user_id = $user->uid;
    //$query = "select * from tseq_job_information where user_id=$user_id";
    $query = "select d.tseq_job_id, d.submit_time, d.user_id, d.tripal_job_id, t.status from tseq_job_information d join tripal_jobs t on d.tripal_job_id=t.job_id where user_id=$user_id order by d.tseq_job_id";
    $results = db_query($query);
    
    $job_list = array();
    for($i = 0; $i < $results->rowCount(); $i++)
        {
          $job_list[$i] = $results->fetchAssoc();
        }
    $rows = array();
    foreach ($job_list as $job)
    {        
        $rows[] = array(
            $job['tseq_job_id'],
            format_date($job['submit_time'], 'short'),
            l('Your results', '/Diamond/results/'.$job['tripal_job_id']),
            $job['status'],
            );
    }
    $header = array('System Job ID', 'Date Submitted', 'Link', 'Status');
    
    
    //Loop through returned jobs from database, create rows out of them
    
    $resultsTable = theme('table', array('header' => $header, 'rows' => $rows));
    
    $content = array(
    'page_title' => array(
        '#type' => 'markup',
        '#markup' => '<h2>Your Tripal Sequence Similarity Searches</h2>',
    ),
    'page_description' => array(
        '#type' => 'markup',
        '#markup' => '<p>' . t('Below you can find your previously submitted TSeq jobs') . '</p>',
    ),
    'results_table' => array(
        '#type' => 'markup',
        '#markup' => $resultsTable,
    ),
  );
    
    
    
  return $content;
}