<?php

/*
 * @param $job_id
 *  The numeric id of the Tripal Job to which the desired results belong
 * 
 * @param $type
 *  Defines which file we are trying to output.
 *  Currently supported as $type:
 *      results
 *      query
 *      target
 *      original
 * 
 * @return file
 *  The results of the job (STDOUT.txt). This is only available to jobs that run successfully
 *  so there is no need for returning the STDERR file (for now)
 * 
 * @todo
 *  Change the filename of the downloaded results to not be "STDOUT.txt"
 * 
 */

function tseq_download($job_id, $type)
{
    //Pumpkin MAKE DYNAMIC
    //$file = "/var/www/Drupal/sites/default/files/tripal/jobs/$job_id/STDOUT.txt";
    
    // Get the job details for the job to provide accurate file links below
    $outputPath = DRUPAL_ROOT.'/sites/default/files/tripal/jobs/';
    $job_details = tseq_get_job_information($job_id);
    
    switch ($type)
    {
        case "results":
            $file = drupal_realpath('public://')."/tripal/jobs/$job_id/STDOUT.txt";
            break;
        
        // The original query provided by the user
        case "query":
            $query_file_no_path = explode('/',$job_details['sequence_file']);
            $file = $outputPath.$job_id.'/'.$query_file_no_path[count($query_file_no_path)-1];
            break;
        
        // Downloading the target database index is only 
        // possible if it was uploaded by the user (and is therefore guaranteed 
        // to exist on the webserver).
        case "target":
            // Did the user upload their own target? (any file type except 'database')
            // Theoretically this case will only be called by pages where this is already the case (and has been
            //     checked, but let's just be sure before we try to send a non-existant file
            if ($job_details['database_file_type'] != 'database')
            {
                $target_file_no_path = explode('/',$job_details['target_file']);
                $file = $outputPath.$job_id.'/'.$query_file_no_path[count($query_file_no_path)-1];
            }
            
            break;
        
        // Download the original file (FASTA) that was used to generate the Diamond/BLAST index
        // This is only possible if the admin supplied that path  and
        // if the module can make items available from remote servers downloadable.
        // This probably won't be used and should be removed
        case "original":
            $file = drupal_realpath('public://')."/tripal/jobs/$job_id/gurp.dmnd";
            break;
        default:
            echo "The download you requested could not be fulfilled: erroneous type of file.";
    }
    
    if (file_exists($file))
    {
        header('Content-Description: File Transfer');
        header('Content-Type: application/octet-stream');
        header('Content-Disposition: attachment; filename="'.$job_details['tseq_unique_id'].'"');
        header('Expires: 0');
        header('Cache-Control: must-revalidate');
        header('Pragma: public');
        header('Content-Length: ' . filesize($file));
        readfile($file);
        exit;
    }
    else
    {
        echo "The file ".$file." no longer exists or could not be found.";
    }
    
}
/*
 * @pararm $db
 *  An array that represents the database to be exported
 * 
 * @param $type
 *  Specify which type of database to export (Genome, Gene, Protein, or All)
 * 
 * @return boolean
 *  Specify if the export operation was a success or not
 */
function tseq_db_export($db, $type)
{
    if ($type = 'csv')
    {
        //header('Content-Description: File Transfer');
        header('Content-type: text/plain');
        header('Content-Disposition: attachment; filename="DiamondDatabases.csv"');
        header('Pragma: public');
        $dbOut = '';
        
        //drupal_set_message("db: ".$db[1][0]);
        
        foreach($db as $dbLine)
        {
                $dbOut .= $dbLine['type'].', '.$dbLine['name'].', '.$dbLine['version'].', '.$dbLine['location'];
            
            /*foreach($dbLine as $dbDatapoint)
            {
                drupal_set_message(var_dump($dbLine));
                $dbOut = $dbOut.$dbDatapoint;
                $dbOut = $dbOut.', ';
            }*/
              
             
            $dbOut = $dbOut."\n";
        }
        $dbOut = $dbOut."\r\n";
        echo $dbOut;
        exit;
       
        drupal_set_message("Contents of CSV: ".$dbOut);
        
    }
    else
    {
        echo ("We currently do not support other file types.");
    }
    
    return TRUE;   //Returns success/failure of file offering (can't tell if user actually saved/opened it
}