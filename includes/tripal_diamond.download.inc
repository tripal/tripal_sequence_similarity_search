<?php

/*
 * @param $job_id
 *  The numeric id of the Tripal Job to which the desired results belong
 * 
 * @return file
 *  The results of the job (STDOUT.txt). This is only available to jobs that run successfully
 *  so there is no need for returning the STDERR file (for now)
 * 
 */

function tripal_diamond_download($job_id)
{
    //Pumpkin MAKE DYNAMIC
    //$file = "/var/www/Drupal/sites/default/files/tripal/jobs/$job_id/STDOUT.txt";
    $file = DRUPAL_ROOT.base_path()."/sites/default/files/tripal/jobs/$job_id/STDOUT.txt";
            
    if (file_exists($file))
    {
        header('Content-Description: File Transfer');
        header('Content-Type: application/octet-stream');
        header('Content-Disposition: attachment; filename="'.basename($file).'"');
        header('Expires: 0');
        header('Cache-Control: must-revalidate');
        header('Pragma: public');
        header('Content-Length: ' . filesize($file));
        readfile($file);
        exit;
    }
    else
    {
        echo "The file ".$file." no longer exists or could not be found";
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
function tripal_diamond_db_export($db, $type)
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