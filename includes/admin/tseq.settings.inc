<?php

function tseq_configuration_settings_form($form, &$form_state)
{
    /*
     * Get the currently saved settings (or substitute in defaults
     */
    //Do not change these $default_ variables here. If you really need to, 
    //make sure they are actually set
    $default_num_threads = 1;
    $default_expiry = 30;
    
    $query = 'select * from tseq_settings';
    $results = db_query($query);
    
    $current_settings = $results->fetchAssoc();
    
    if ($current_settings['num_threads'] === NULL){
        $num_threads = $default_num_threads;              
    }
    else {
        $num_threads = $current_settings['num_threads'];
    }
    
    if ($current_settings['file_expire_time'] === NULL){
        $file_expire_time = $default_expiry; 
    } 
    else {
        $file_expire_time = $current_settings['file_expire_time'];
    }
    /*
     * Get the available Tripal Remote Job resources
     */
    module_load_include('inc', 'tripal_remote_job', '/includes/TripalRemoteResource');
    module_load_include('inc', 'tripal_remote_job', '/includes/TripalRemoteSSH');
    
    $resources = TripalRemoteResource::getResources();
    $resourcesToShow[0] = '--';
    foreach ($resources as $resource)
    {
        if ($resource->getType() == 'alone') //Only support SSH servers for now
        {
            $resourcesToShow[$resource->getResourceID()] = $resource->getName();
        }
    }
    
    
    $form['settings']['threads'] = array(
        '#type'         =>  'textfield',
        '#title'        =>  'Threads',
        '#size'         =>  '5',
        //'#description'  =>  'Expected number of chance matches in a random model. This number should be give in a decimal format.',
        '#required'     =>  true,
        '#default_value'      =>  t($num_threads),
        //'#prefix'       => '<b>Advanced Options</b>',
        '#field_prefix' => 'Define the number of threads the job is allowed to use.',
        '#description' => 'Enter \'0\' to have Diamond/BLAST use as many threads as possible (default behavior)</p>',
    );
    
    $form['settings']['expiry'] = array(
        '#type'         =>  'textfield',
        '#title'        =>  'File Expiration',
        '#size'         =>  '5',
        //'#description'  =>  'Expected number of chance matches in a random model. This number should be give in a decimal format.',
        '#required'     =>  true,
        '#default_value'      =>  t($file_expire_time),
        //'#prefix'       => '<b>Advanced Options</b>',
        '#field_prefix' => 'Define, in days, how long to keep stored files pertaining to submitted jobs',
    );
    
    $form['settings']['preferred_remote_resource'] = array(
        '#type'         => 'select',
        '#title'        => 'Preferred Remote Resource',
        '#options'      => $resourcesToShow,
        '#required'     => false,
        '#default_value'      =>  t($current_settings['preferred_remote_resource']),
        '#field_prefix' => 'Select the remote resource to use. Add additional resources in the Tripal Remote Job module. Only supports SSH/standalone servers.',
    );
    // Todo - make this a dropdown with available remote resources
    $form['settings']['blast_exe_location'] = array(
        '#type'         =>  'textfield',
        '#title'        =>  'BLAST Executable Location',
        '#size'         =>  '120',
        '#required'     =>  false,
        '#default_value'      =>  t($current_settings['blast_exe_location']),
        '#field_prefix' => 'Location on the remote resource where the BLAST executable files can be found',
        '#description'  => 'If not specified, will try to use BLAST from the default $PATH. The trailing slash is not required.',
    );
    
    $form['settings']['diamond_exe_location'] = array(
        '#type'         =>  'textfield',
        '#title'        =>  'Diamond Executable Location',
        '#size'         =>  '120',
        '#required'     =>  false,
        '#default_value'      =>  t($current_settings['diamond_exe_location']),
        '#field_prefix' => 'Location on the remote resource where the Diamond executable files can be found',
        '#description'  => 'If not specified, will try to use Diamond from the default $PATH. The trailing slash is not required.',
    ); 
    
    $form['submit_button'] = array(
        '#type' => 'submit',
        '#value' => t('Save'),
    );
    return $form;
}
function tseq_configuration_settings_form_validate($form, &$form_state)
{  
    //Todo
}
function tseq_configuration_settings_form_submit($form, &$form_state)
{
 $tseq_settings = array(
        'settings_id'               => '0',
        'num_threads'               => $form_state['values']['threads'],
        'file_expire_time'          => $form_state['values']['expiry'],
        'blast_exe_location'        => $form_state['values']['blast_exe_location'],
        'diamond_exe_location'      => $form_state['values']['diamond_exe_location'],
        'preferred_remote_resource' => $form_state['values']['preferred_remote_resource'],        
    );
    
    drupal_write_record('tseq_settings', $tseq_settings,'settings_id');
    drupal_set_message("The settings have been saved");
}

