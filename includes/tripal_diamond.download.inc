<?php

function tripal_diamond_download($job_id)
{
    $file = "/var/www/html/Drupal/sites/default/files/tripal/jobs/$job_id/STDOUT.txt";
    
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
    
}

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