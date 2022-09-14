<?php

namespace Drupal\tripal_seq\Controller;

Use Drupal;
use Drupal\Core\Controller\ControllerBase;
/**
 * This controller handles Results and everything to do with them
 *  - displaying the results
 *  - providing downloads
 *  
 */

class Tripal_SeqResultsController {

    /**
     * Undocumented function
     *
     * @param integer $job_id
     * @param string $type The type of file download, options are:
     *  results, query, target, and original
     * @return file? PUMPKIN
     */
    function download($job_id,$type) {
        return [
            '#markup' => 'Job ID: ' . $job_id . ' and type: ' . $type
        ];
    }

    function results($job_id) {
        return [
            '#markup' => 'The results for ' . $job_id . ' can be found here eventually'
        ];
    }
}