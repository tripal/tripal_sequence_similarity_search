<?php

/*
 * Main theme for displaying the results of a submitted Diamond job
 */

drupal_set_title('Status of Job #'.$job_id);
//$outputPath = '/var/www/html/Drupal/sites/default/files/tripal/jobs/';

?>
<?php
    $status_error = explode(' ', $status);
    if ($status_error[0] == 'Error:')
    {
        $statusError = 'Error';
        echo "There was an error running your job. Please contact an adminstrator and reference Tripal Job Number: ".$job_id;
    }
    else
    {
        echo "Current status of your job: ".$status;
    }
        
?>
<br />
<!-- Current status of your job on the Remote Server: <?php //echo $remote_status; ?> -->
<!--<div id="tseq_test">Dingo</div>
<script type="text/javascript">
    function loady()
    {
        var dingo = document.getElementById("tseq_test");
        dingo.innerHTML = tseq_test();
    }
</script>
-->


<!-- Show results if job was successful -->
<?php
$outputPath = DRUPAL_ROOT.'/sites/default/files/tripal/jobs/';
if ($status == 'debug')
{
    echo "This was a debugging run. The job was not submitted. There are no results and there will never be any.";
}
else if ($status == 'Completed')
{
    // Get the Job information to be used throughout this function
    $job_information = tseq_get_job_information($job_id);
    // Do the results of this job exist in the database?
    $resultsFound = FALSE;
    if (!tseq_results_exist($job_id)) // No
    {
        //Prepare the data to be inserted into table `tseq_results`
        /*
         *  tripal_job_id       $job_id
         *  user_id             $job_information['user_id'];
         *  tseq_db_id          tseq_get_db_id_by_location($job_information['database_file'])
         *  summary             ?
         *  result_type         assume BLAST XML
         *  data                ...
         *  matches             ...
         *  database_used       
         */
        $user_id = $job_information['user_id'];
        $tseq_db_id = tseq_get_db_id_by_location($job_information['database_file']);
        $result_type = "BLAST XML";
        // Get status and data
        $matches_found = 0;
        //Database used
        if ($job_information['database_file_type'] == 'database')
        {
            // Make some spaghetti getting this information because the sequence database name isn't stored in the tseq db
            $tseq_db_id = tseq_get_db_id_by_location($job_information['database_file']);
            $db_info = tseq_get_db_info($tseq_db_id);        
            //drupal_set_message("Database used: ".$db_info['name'].", version ".$db_info['version']." (".$db_info['category'].")");
            $database_used = $db_info['name'].", version ".$db_info['version']." (".$db_info['type']." - ".$db_info['category'].")";
            // Supply a download link
            // Pumpkin - do we really want to have this here? 
            if ($db_info['web_location'] != '')
            {
                $database_used = $database_used." <a href=\"".$db_info['web_location']."\">Download</a>";
            }
        }
        else
        {
            $database_used = '';
        }
        //Sequences queried
        $query_file_no_path = explode('/',$job_information['sequence_file']);
        $query_file = $outputPath.$job_id.'/'.$query_file_no_path[count($query_file_no_path)-1];
        if (file_exists($query_file))
        {
            $sequence_count = tseq_get_query_sequence_count($query_file);
        }
        else
        {
            $sequence_count = -1;
        }
        
        if (file_exists($outputPath.$job_id.'/STDOUT.txt') && file_exists($outputPath.$job_id.'/STDERR.txt'))
        {
            //Prepare output if there is any data to show in STDOUT
            if (filesize($outputPath.$job_id.'/STDOUT.txt') > 0)
            {
                $jobResults = file($outputPath.$job_id.'/STDOUT.txt');
                $resultsFound = TRUE;
                foreach($jobResults as $resultLine)
                {
                    if ($resultLine == "***** No hits found *****")
                    {   $resultsFound = FALSE;
                        $summary = "no results found";
                        $data = "no results found";
                    }
                }
                if ($resultsFound)
                {
                    $summary = "results found";
                    $data = 'Look for data in tseq_results_data table';
                }
                // Gather other details
                //Matches found
                $matches_found = tseq_get_matches_count($outputPath.$job_id.'/STDOUT.txt');
                
            }
            else if (filesize($outputPath.$job_id.'/STDERR.txt') > 0)
            {
                $data = file_get_contents($outputPath.$job_id.'/STDERR.txt');
                $summary = "error";
            }
            else
            {
                $data = "There were no matches found.";
                $summary = "error";
            }
        }
        else
        {
            $data = "Results not found. Please contact an adminstrator and reference Tripal Job Number: ".$job_id;        // This means there was no STDERR or STDOUT files, and possibly even the job dir (most likely due to system)
            $summary = "error";
            if (file_exists($query_file))
            {
                $sequence_count = 0;
            }
            $database_used ='';
        }
                
        
        // and finally, insert (if there is anything to insert
        
        // Results details (always insert these)
        $newDB = array(
            'tripal_job_id' => $job_id,
            'user_id'       => $user_id,
            'tseq_db_id'    => $tseq_db_id,
            'data'          => $data,
            'summary'       => $summary,
            'result_type'   => $result_type,
            'sequence_count'=> $sequence_count,
            'matches'       => $matches_found,
            'database_used' => $database_used,
        );
        drupal_write_record('tseq_results', $newDB); // Insert the job details

        // Results data (Insert these only if they exist)
        if ($resultsFound)
        {
            foreach($jobResults as $key => $jobResult)
            {
                $columns = explode("\t", $jobResult);
                $job_results_data[$key] = array(
                    'tripal_job_id' => $job_id,
                    'match_id'      => $key,
                    'query_label'   => $columns[0],
                    'target'        => $columns[1],
                    'percent_identity'  => $columns[2],
                    'alignment_length'  => $columns[3],
                    'mismatches'        => $columns[4],
                    'gap_opens'         => $columns[5],
                    'start_position_query'  => $columns[6],
                    'end_position_query'     => $columns[7],
                    'start_position_database'  => $columns[8],
                    'end_position_database'     => $columns[9],
                    'e_value'   =>  $columns[10],
                    'bit_score' =>  $columns[11],
                );
                drupal_write_record('tseq_results_data',$job_results_data[$key]);
            }
        }
    }
    // At this point, the results should be in the database in some form
    // Get the results details from the database
    $query = "SELECT * FROM tseq_results WHERE tripal_job_id='$job_id'";
    $queryResults = db_query($query);
    $results_details = $queryResults->fetchAssoc();
    
    // Display this data (from db) into some nice tables
    // Results Details Table
    echo "<h3>Summary of your job:</h3>";
    $summary_headers = array('Summary','Info');
    $summary_rows = array();
    $summary_rows[0] = array('Sequences submitted',$results_details['sequence_count']==-1 ? 'N/A':$results_details['sequence_count']);
    $summary_rows[1] = array('Matches found',$results_details['matches']);
    if ($results_details['database_used'])
    {
        $summary_rows[2] = array('Database searched against',$results_details['database_used']);
    }
    if ($results_details['summary'] == 'error')
    {
        $summary_rows[3] = array('Error',$results_details['data']);
    }
    $summary_attributes = array(
        'class' => array(
            'summaryTable',
        ),
    );
    
    $summary_table_vars = array(
        'header' => $summary_headers,
        'rows'   => $summary_rows,
        'attributes'    => $summary_attributes
    );
    echo theme('table',$summary_table_vars);
    
    
    
    // Results Data Table
    if($results_details['summary'] != 'error')
    {
        echo "<h3>Job Results:</h3>";
        $results_data_header = array(
            array('data' => 'Query Label',              'field' => 'query_label',        'sort' => 'ASC'), // Only set default sort order for 1 field
            array('data' => 'Target',                   'field' => 'target'),
            array('data' => '% Identity',               'field' => 'percent_identity'),
            array('data' => 'Alignment Length',         'field' => 'alignment_length'),
            array('data' => 'Mismatches',               'field' => 'mismatches'),
            array('data' => 'Gap Opens',                'field' => 'gap_opens'),
            array('data' => 'Start Position (Query)',   'field' => 'start_position_query'),
            array('data' => 'End Position (Query)',     'field' => 'end_position_query'),
            array('data' => 'Start Position (Database)','field' => 'start_position_database'),
            array('data' => 'End Position (Database)',  'field' => 'end_position_database'),
            array('data' => 'E-Value',                  'field' => 'e_value'),
            array('data' => 'Bit Score',                'field' => 'bit_score'),
        );
        $select = db_select('tseq_results_data','t')
                ->extend('PagerDefault')
                ->extend('TableSort');
        $select->condition('tripal_job_id',$job_id,'=')
               ->fields('t',array('query_label','target','percent_identity','alignment_length','mismatches','gap_opens','start_position_query','end_position_query','start_position_database','end_position_database','e_value','bit_score'))
               ->limit(15)
               ->orderByHeader($results_data_header);
        $results_data = $select->execute();
        $results_data_rows = array();
        foreach ($results_data as $row) 
        {
            $results_data_rows[] = array(
                $row->query_label,
                $row->target,
                $row->percent_identity,
                $row->alignment_length,
                $row->mismatches,
                $row->gap_opens,
                $row->start_position_query,
                $row->end_position_query,
                $row->start_position_database,
                $row->end_position_database,
                $row->e_value,
                $row->bit_score,
            );
        }
        $results_attributes = array(
            'class' => array(
                'resultsTable',
            ),
        );
        $results_table_vars = array(
            'header' => $results_data_header,
            'rows'   => $results_data_rows,
            'attributes'    => $results_attributes
        );
        $output = theme('table',$results_table_vars);

        $output .= theme('pager');
        echo $output;
    }
    
    // Download section
    echo "<h3>Downloads:</h3>";
    if($results_details['summary'] != 'error')
    {
        echo "<li>Click <a href=\"download/$job_id/results\">here</a> to download these results</li>";
    }
    echo "<li>Click <a href=\"download/$job_id/query\">here</a> to download your original query</li>";
    if ($job_information['database_file_type'] != 'database')
    {
        echo "<li>Click <a href=\"download/$job_id/target\">here</a> to download your original target database</li>";
    }
}
else if ($status == 'Error')
{
    
}
else if ($status == 'Cancelled')
{
    
}
else
{
  // Job evidently not completed. Keep checking
  $meta = array(
    '#tag' => 'meta',
    '#attributes' => array(
      'http-equiv' => 'refresh',
      'content' =>  '10',
    )
  );
  drupal_add_html_head($meta, 'tripal_job_status_page');
}

//echo "On the remote server, your job is: ".TripalRemoteSSH::isJobRunning(tripal_get_job($job_id));
        //TripalRemoteSSH::isJobRunning(tripal_get_job($job_id));;