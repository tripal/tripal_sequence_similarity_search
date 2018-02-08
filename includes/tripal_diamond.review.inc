<?php

function tripal_diamond_review($jobID)
{
    
    $jobInfo = tripal_get_job($jobID);
    //print_r($jobInfo);
    // 1) Job is in the Queue
  //if ($jobInfo->start_time === NULL AND $jobInfo->end_time == NULL)
    if ($jobInfo->status == 'Waiting')
  {
    return theme('tripal_diamond_results', array('status_code' => 0, 'status' => 'Pending. It may take up to 20 seconds to process', 'job_id' => $jobID));
  }
  // 2) Job has been Cancelled
  elseif ($jobInfo->status == 'Cancelled')
  {
    return theme('tripal_diamond_results', array('status_code' => 999, 'status' => 'Cancelled', 'job_id' => $jobID));
  }
  // 3) Job is Complete
  elseif ($jobInfo->status == 'Completed')
  {
    // Return the Results :)
    return theme('tripal_diamond_results', array('status_code' => 7, 'status' => 'Completed', 'job_id' => $jobID));
  }
  // 4) Job errored during Tripal Job processing (before remote execution)
  elseif ($jobInfo->status == 'Error')
  {
      return theme('tripal_diamond_results', array('status_code' => 9, 'status' => 'Error: '.$jobInfo->error_msg.' This is a most likely a server issue, please contact an administrator.', 'job_id' => $jobID));
  }
  // 5) Job is in Progress
  else
  {
    return theme('tripal_diamond_results', array('status_code' => 1, 'status' => 'Running', 'job_id' => $jobID));
  }
  return '';
    /*print_r($jobb);
    $jobInfo = tripal_get_job('9');
    print_r($jobInfo);*/
}