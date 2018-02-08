<?php

function tripal_diamond_download($job_id)
{
    $file = "/var/www/html/Drupal/sites/default/files/tripal/jobs/$job_id/STDOUT.txt";
    
    if (file_exists($file)) {
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