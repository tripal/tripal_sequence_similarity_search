<?php

/*
 * Main theme for displaying the results of a submitted TSeq job
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


<!-- Show results if job was successful -->
<?php
$outputPath = DRUPAL_ROOT.'/sites/default/files/tripal/jobs/';
echo "Click <a href=\"/TSeq/submit\">here</a> to run another job";
if ($status == 'debug')
{
    echo "This was a debugging run. The job was not submitted. There are no results and there will never be any.";
}
else if ($status == 'Completed')
{
    publish_results($job_id);
    
    // At this point, the results should be in the datgobaase in some form
    // Get the results details from the database
    $job_information = tseq_get_job_information($job_id);
    
    $query = "SELECT * FROM tseq_results WHERE tripal_job_id='$job_id'";
    $queryResults = db_query($query);
    $results_details = $queryResults->fetchAssoc();
    
    // Display this data (from db) into some nice tables
    // Results Details Table
    echo "<h3>Your TSeq Search Results</h3>";
    $summary_headers = array('Summary','Info');
    $summary_rows = array();
    $summary_rows[0] = array('Sequences submitted',$results_details['sequence_count']==-1 ? 'N/A':$results_details['sequence_count']);
    $summary_rows[1] = array('Matches found',$results_details['matches']);
    if ($results_details['database_used'])
    {
        $summary_rows[2] = array('Target Database',$results_details['database_used']);
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
    

    // Download section
    // Get some relevent info
    
    $tseq_db_id = tseq_get_db_id_by_location($job_information['database_file']);
    if($job_information['database_file_type'] == 'database')
    {
        $db_info = tseq_get_db_info($tseq_db_id);
    }
    echo "<h3>File Downloads</h3>";
    if($results_details['summary'] != 'error')
    {
        echo "<li>Click <a href=\"download/$job_id/results\">here</a> to download these results</li>";
    }
    echo "<li>Click <a href=\"download/$job_id/query\">here</a> to download your original query</li>";
    if ($job_information['database_file_type'] != 'database')
    {
        echo "<li>Click <a href=\"download/$job_id/target\">here</a> to download your original target database</li>";
    }
    if ($db_info) {

        if( ($job_information['database_file_type'] == 'database') AND ($db_info['web_location'] != '') )
        {
            echo " <li>Click <a href=\"".$db_info['web_location']."\">here</a> to download the original sequence</li>";
        }
    }    
    else {
        echo " <li>There was an issue looking up the database info.</li>";
    }
    
    // Results Data Table
    if($results_details['summary'] != 'error')
    {
        echo "<h3>Search Results</h3>";
        $results_data_header = array(
            array('data' => 'Query Sequence',           'field' => 'query_label'), 
            array('data' => 'Target',                   'field' => 'target'),
            array('data' => '% Identity',               'field' => 'percent_identity'),
            array('data' => 'Alignment Length',         'field' => 'alignment_length'),
            array('data' => 'Mismatches',               'field' => 'mismatches'),
            array('data' => 'Gap Opens',                'field' => 'gap_opens'),
            array('data' => 'Start Position (Query)',   'field' => 'start_position_query'),
            array('data' => 'End Position (Query)',     'field' => 'end_position_query'),
            array('data' => 'Start Position (Database)','field' => 'start_position_database'),
            array('data' => 'End Position (Database)',  'field' => 'end_position_database'),
            array('data' => 'E-Value',                  'field' => 'e_value',        'sort' => 'asc'),  // Only set default sort order for 1 field. Sort direction needs to be lowercase (asc, not ASC)
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
                "<a href=\"#>" . $row->target . "\">" . $row->target . "</a>",
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
        $results_caption = array(
                'Sort columns by clicking on a header.',
        );
        $results_table_vars = array(
            'header' => $results_data_header,
            'rows'   => $results_data_rows,
            'attributes'    => $results_attributes,
            'caption'       => t('Sort columns by clicking on a header'),
        );
        $output = theme('table',$results_table_vars);

        $output .= theme('pager');
        echo $output;
    }
    
    // Alignments view Section
    // Basic version currently - just display the text file inline
    $outputPath = DRUPAL_ROOT.'/sites/default/files/tripal/jobs/';
    $outputPath .= $job_id;
    echo "<h3>Alignment View</h3>";

    $pairwise_file_name = $outputPath . "/results_pairwise.txt";
    if (file_exists($pairwise_file_name))
    {
        $pairwise_file = file($pairwise_file_name);
        echo "<div class=\"ui-widget ui-widget-content\">";
        echo "<pre>";
        $linecount = 0;
        foreach($pairwise_file as $pairwise_line)
        {
            if ($linecount < 500) {
                // Is this a >header line? If so we want to put an anchor tag here so we can navigate here from the table.
                if (preg_match('/([>]).+/',$pairwise_line)) {
                    echo "<a name=\"" . trim($pairwise_line) . "\"></a>" . $pairwise_line;
                }
                else {
                    echo $pairwise_line;
                }
                $linecount += 1;
            }
            else {
                echo "</pre></div>";
                echo "<p> Rest of alignment view truncated. Download the full text above.</p>";
                return;
            }
        }
    }
    else
    {
        echo "<p>The pairwise alignment results were either not generated or the file could not be found.</p>";
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