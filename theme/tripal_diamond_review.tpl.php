<?php

/*
 * Main theme for displaying the results of a submitted Diamond job
 */
?>
<!--<script>
Drupal.behaviors.diamondSetTimeout = {
  attach: function (context, settings) {
    setTimeout(function(){
       window.location.reload(1);
    }, 5000);
  }
};

</script>
-->
<?php
drupal_set_title('Status of Job #'.$job_id);
$outputPath = '/var/www/html/Drupal/sites/default/files/tripal/jobs/';

// Not necessary to let the user know about
/*
if(file_exists($outputPath.$job_id.'/PID'))
{
    $remotePID = file_get_contents($outputPath.$job_id.'/PID');
    echo "Pid: ".$remotePID;
}
else
{
    echo $outputPath.$job_id.'/PID file does not exist';
}
 * 
 */

//pumpkin echo tripal_remote_job_SSH_check();


?>
<hr />

Current status of your job in the Tripal Job System: <?php echo $status; ?>
<br />
<!-- Current status of your job on the Remote Server: <?php //echo $remote_status; ?> -->


<!-- Show results if job was successful -->
<?php
$outputPath = DRUPAL_ROOT.'/sites/default/files/tripal/jobs/';
if ($status == 'Completed')
{   
    //echo "OUT: ".filesize($outputPath.$job_id.'/STDOUT.txt');
    //echo "ERR: ".filesize($outputPath.$job_id.'/STDERR.txt');
    if (file_exists($outputPath.$job_id.'/STDOUT.txt'))
    {
        if (filesize($outputPath.$job_id.'/STDOUT.txt') > 0)
        {
           echo "Your job results: <br /><br />";
           $jobResults = file($outputPath.$job_id.'/STDOUT.txt');
           //$jobResults2 = explode("\t", $jobResults[0]);
           //Table header
           echo "<table>"
                   . "<tr>"
                   . "<th>Query Label</th>"
                   . "<th>Target</th>"
                   . "<th>% Identity</th>"
                   . "<th>Alignment Length</th>"
                   . "<th>Mismatches</th>"
                   . "<th>Gap opens</th>"
                   . "<th>Start Position (query)</th>"
                   . "<th>End Position (query)</th>"
                   . "<th>Start Position (target)</th>"
                   . "<th>End Position (target)</th>"
                   . "<th>E-value</th>"
                   . "<th>Bit score</th>"
                   . "</tr><tr>";
           
           foreach($jobResults as $resultLine)
           {
               //Possible pumpkin
               $resultLineE = explode("\t", $resultLine);
               foreach ($resultLineE as $resultData)
               {
                   echo "<td>$resultData</td>";                   
               }
               echo "</tr><tr>";
           }
           echo "</tr></table>";
           echo "Click <a href=\"download/$job_id\">here</a> to download";
        }
        echo "<hr />";
    }
    elseif (file_exists($outputPath.$job_id.'/STDERR.txt'))
    {
        if (filesize($outputPath.$job_id.'/STDERR.txt') > 0)
            {
               echo "Looks like your job failed: ";
               readfile($outputPath.$job_id.'/STDERR.txt'); 
            }
    }
    else
    {
        echo "The results of that job can not be found. Be aware that results for Diamond jobs expire after 60 days";
    }
}

//echo "On the remote server, your job is: ".TripalRemoteSSH::isJobRunning(tripal_get_job($job_id));
        //TripalRemoteSSH::isJobRunning(tripal_get_job($job_id));;