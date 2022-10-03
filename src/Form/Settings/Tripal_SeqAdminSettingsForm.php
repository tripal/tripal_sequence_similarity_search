<?php
namespace Drupal\tripal_seq\Form\Settings;

use Drupal\Core\Form\FormBase;
use Drupal\Core\Form\FormStateInterface;
use Drupal; # is this useful or wasteful? PUMPKIN

class Tripal_SeqAdminSettingsForm extends FormBase {
    /**
     * Form ID.
     * 
     * @return string
     */
    function getFormID() {
        return 'tripal_seq_admin_settings_form';
    }

    /**
     * Build the form
     */
    function buildForm(array $form, FormStateInterface $form_state) {
        
        // Get the current/default values from the database
        $db = \Drupal::database();
        $table_name = 'tseq_settings';
        $query = $db->select($table_name, 'ts')
                ->fields('ts', array(
                    'num_threads',
                    'file_expire_time',
                    'diamond_exe_location',
                    'diamond_exe_version',
                    'blast_exe_location',
                    'blast_exe_version',            // unused currently
                    'preferred_remote_resource',    // unused currently, requires TRJ update
                    'produce_blast_pairwise',
                    'produce_diamond_pairwise',
                ))
                ->condition('settings_id', 0);
        $results = $query->execute();
        $settings = $results->fetchObject();


        // Build the form. The form names match exactly with the database rows.
        $form['num_threads'] = [
            '#type'         =>  'textfield',
            '#title'        =>  'Threads',
            '#size'         =>  '5',
            //'#description'  =>  'Expected number of chance matches in a random model. This number should be give in a decimal format.',
            '#required'     =>  true,
            '#default_value'      =>  $settings->num_threads,
            //'#prefix'       => '<b>Advanced Options</b>',
            '#field_prefix' => 'Define the number of threads the job is allowed to use.',
            '#description' => 'Enter \'0\' to have Diamond/BLAST use as many threads as possible (default behavior)</p>',
        ];
        
        $form['file_expire_time'] = [
            '#type'         =>  'textfield',
            '#title'        =>  'File Expiration',
            '#size'         =>  '5',
            //'#description'  =>  'Expected number of chance matches in a random model. This number should be give in a decimal format.',
            '#required'     =>  true,
            '#default_value'      =>  $settings->file_expire_time,
            //'#prefix'       => '<b>Advanced Options</b>',
            '#field_prefix' => 'Define, in days, how long to keep stored files pertaining to submitted jobs',
        ];
        
        // Remove for now, depends on https://github.com/tripal/tripal_remote_job/issues/1
        /*
        $form['preferred_remote_resource'] = [
            '#type'         => 'select',
            '#title'        => 'Preferred Remote Resource',
            '#options'      => $resourcesToShow,
            '#required'     => false,
            '#default_value'      =>  t($current_settings['preferred_remote_resource']),
            '#field_prefix' => 'Select the remote resource to use. Add additional resources in the Tripal Remote Job module. Only supports SSH/standalone servers.',
        ];
        */
    
        $form['blast_exe_location'] = [
            '#type'         =>  'textfield',
            '#title'        =>  'BLAST Executable Location',
            '#size'         =>  '120',
            '#required'     =>  false,
            '#default_value'      =>  $settings->blast_exe_location,
            '#field_prefix' => 'Location on the remote resource where the BLAST executable file can be found',
            '#description'  => 'If not specified, will try to use BLAST from the default $PATH. Include the executable itself: <code>/path/to/bin/blast</code>',
        ];
        
        $form['diamond_exe_location'] = [
            '#type'         =>  'textfield',
            '#title'        =>  'Diamond Executable Location',
            '#size'         =>  '120',
            '#required'     =>  false,
            '#default_value'      =>  $settings->diamond_exe_location,
            '#field_prefix' => 'Location on the remote resource where the Diamond executable file can be found',
            '#description'  => 'If not specified, will try to use Diamond from the default $PATH. Include the executable itself: <code>/path/to/bin/diamond</code>',
        ]; 
        
        $form['diamond_exe_version'] = [
            '#type'         => 'textfield',
            '#title'        => 'Diamond Executable Version',
            '#size'         => '5',
            '#required'     => false,
            '#default_value'    => $settings->diamond_exe_version,
            '#field_prefix' => 'Version of the Diamond executable on the remote server.',
            '#description'  => 'This can be found by issuing <code>/path/to/diamond --version</code>. If this is not specified, certain functionality will be limited. The documentation will list these limitations',
        ];
    
        $form['produce_blast_pairwise'] = [
            '#type'         => 'checkbox',
            '#title'        => 'Produce pairwise alignment for BLAST jobs',
            '#title_display'=> 'before',
            '#default_value'    => $settings->produce_blast_pairwise,
            '#field_prefix'     => 'Should the jobs also produce a pairwise alignment view?'
        ];
    
        $form['produce_diamond_pairwise'] = [
            '#type'         => 'checkbox',
            '#title'        => 'Produce pairwise alignment for Diamond jobs',
            '#title_display'=> 'before',
            '#default_value'    => $settings->produce_diamond_pairwise,
            '#field_prefix'     => 'Should the jobs also produce a pairwise alignment view?',
            '#description'      => 'This option requires that Diamond version 0.9.24 is installed',
        ];
        
        $form['submit_button'] = [
            '#type' => 'submit',
            '#value' => t('Save'),
        ];

        return $form;
    }

    function validateForm(array &$form, FormStateInterface $form_state) {
    }
    
    /**
     * Submit
     */
    function submitForm(array &$form, FormStateInterface $form_state) {
        $db = \Drupal::database();

        $result = $db->update('tseq_settings')
                ->fields(
                    [
                        'num_threads' => $form_state->getValue('num_threads'),
                        'file_expire_time' => $form_state->getValue('file_expire_time'),
                        'blast_exe_location' => $form_state->getValue('blast_exe_location'),
                        'diamond_exe_location' => $form_state->getValue('diamond_exe_location'),
                        'diamond_exe_version' => $form_state->getValue('diamond_exe_version'),
                        'produce_blast_pairwise' => $form_state->getValue('produce_blast_pairwise'),
                        'produce_diamond_pairwise' => $form_state->getValue('produce_diamond_pairwise'),
                    ]
                )
                ->condition('settings_id', 0)
                ->execute();
    }

}