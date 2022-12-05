<?php
/**
 * @file
 * Provides an API for managing the files used by
 * Tripal Sequence Similarity Search
 */

/**
 * @defgroup tripal_seq_files_api Files
 * @ingroup tripal_seq_api
 * @{
 * Provides an API for managing the files used by
 * Tripal Sequence Similarity Search
 * }@
 * 
 */

/**
 * Check to see if the webserver directories are set up properly.
 * The configured file path must be writeable by the web user.
 */
function tseq_check_local_directories() {
    // Check the sites/default/files/tripal/jobs directory. This is default.
    // @todo make this a dynamic option in case we don't want to stuff
    // the webserver with too many job files

    // Get the path to the files/ directory
    //$real_path = drupal_realpath('public://');
    $public_path = \Drupal::config('system.file')->get('default_scheme');
    $real_path = \Drupal::service('file_system')->realpath($public_path . '://');

    $main_working_dir = $real_path.'/tripal/jobs';
    //echo $main_working_dir;
    return is_writable($main_working_dir);
}