<?php

function tripal_diamond_user_analysis_page($uid) {
  
    /*
     * Get the jobs belonging to this user
     */
    global $user;
        $user_id = $user->uid;
    $query = "select * from diamond_job_information where user_id=$user_id";
    $results = db_query($query);
    
    
    
    drupal_set_message("Your User ID: ".$user_id);
    $content = array(
    'page_title' => array(
        '#type' => 'markup',
        '#markup' => '<h2>Your Tripal Sequence Similarity Searches</h2>',
    ),
    'page_description' => array(
        '#type' => 'markup',
        '#markup' => '<p>' . t('Below you can find your previously submitted TSeq jobs') . '</p>',
    )
  );
  return $content;
}