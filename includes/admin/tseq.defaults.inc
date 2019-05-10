<?php

/**
 * Implements hook_form()
 *   Builds a form for the admin to set configured job defaults such as e_value
 *   and max_alignments
 */
function tseq_configuration_defaults_form($form, &$form_state)
{
    $breadcrumb = array();
    $breadcrumb[] = l('Home', '<front>');
    $breadcrumb[] = l('Administration', 'admin');
    $breadcrumb[] = l('Tripal','admin/tripal');
    $breadcrumb[] = l('Extensions', 'admin/tripal/extension');
    $breadcrumb[] = l('TSeq', 'admin/tripal/extension/tseq/config');
    $breadcrumb[] = l('Submission Defaults','admin/tripal/extension/tseq/config/defaults');
    drupal_set_breadcrumb($breadcrumb);
    /*
     * Get the currently saved settings (or substitute in defaults)
     * These are required fields so if they don't exist in the database, use these values as default
     */
   /**/ $e_value = '0.003';
   /**/ $target_coverage = '50';
   /**/ $query_coverage = '50';
   /**/ $max_alignments_list = '5, 10, 25, 50, 100, 250, 500, 750, 1000';
   /**/ $max_alignments_selected = '10';
    
    $query = 'select defaults_e_value, defaults_target_coverage, defaults_query_coverage, defaults_max_alignments_list, defaults_max_alignments_selected from tseq_settings';
    $results = db_query($query);
    
    $current_defaults = $results->fetchAssoc();
    // Substitute values from DB if they exist
    if ($current_defaults['defaults_e_value'])
    {
        $e_value = $current_defaults['defaults_e_value'];
    }
    if ($current_defaults['defaults_target_coverage'])
    {
        $target_coverage = $current_defaults['defaults_target_coverage'];
    }
    if ($current_defaults['defaults_query_coverage'])
    {
        $query_coverage = $current_defaults['defaults_query_coverage'];
    }
    if ($current_defaults['defaults_max_alignments_list'])
    {
        $max_alignments_list = $current_defaults['defaults_max_alignments_list'];
    }
    if ($current_defaults['defaults_max_alignments_selected'])
    {
        $max_alignments_selected = $current_defaults['defaults_max_alignments_selected'];
    }
        
    $form['Overview'] = array(
        '#type'         => 'fieldset',
        '#title'        => 'Information',
        '#collapsible'  => TRUE,
        '#collapsed'    => FALSE,
        '#description'  => t('Use this form to set the default values for the "Advanced Options" portion of the'
                . ' submission form. '),
    );
    
    $form['defaults']['e_value'] = array(
        '#type'         =>  'textfield',
        '#title'        =>  'E-Value',
        '#size'         =>  '5',
        '#required'     =>  true,
        '#default_value'      =>  t($e_value),
        //'#field_prefix' => 'Define the e-value',
    );
    
    $form['defaults']['target_coverage'] = array(
        '#type'         =>  'textfield',
        '#title'        =>  'Target Coverage',
        '#size'         =>  '5',
        '#required'     =>  true,
        '#default_value'      =>  t($target_coverage),
        //'#field_prefix' => 'Define, in days, how long to keep stored files pertaining to submitted jobs',
    );
    
    $form['defaults']['query_coverage'] = array(
        '#type'         =>  'textfield',
        '#title'        =>  'Query Coverage',
        '#size'         =>  '5',
        '#required'     =>  true,
        '#default_value'      =>  t($query_coverage),
        //'#field_prefix' => 'Define, in days, how long to keep stored files pertaining to submitted jobs',
    );
    
    $form['defaults']['max_alignments_list'] = array(
        '#type'         =>  'textfield',
        '#title'        =>  'Available Max Alignments',
        '#size'         =>  '50',
        '#required'     =>  true,
        '#default_value'      =>  t($max_alignments_list),
        '#description' => 'Provide a comma-seperated list of values available to the user to define Max Alignment value in their search',
    );
    
    $form['defaults']['max_alignments_selected'] = array(
        '#type'         =>  'textfield',
        '#title'        =>  'Max Alignments (Default)',
        '#size'         =>  '5',
        '#required'     =>  true,
        '#default_value'      =>  t($max_alignments_selected),
        //'#field_prefix' => 'Define, in days, how long to keep stored files pertaining to submitted jobs',
    );
    
    $form['submit_button'] = array(
        '#type' => 'submit',
        '#value' => t('Save'),
    );
    return $form;
}

/**
 * Implements hook_form_validate()
 *   Validates the entered values make basic business sense 
 *   (positive numbers, etc).
 */
function tseq_configuration_defaults_form_validate($form, &$form_state)
{  
    /*
     *  Checks to be made:
     *  - max_alignments_selected is one of the options in the max_alignments_list
     *  - any logical/business/biological restrictions, non-numeric characters probably
     */
    
    $max_alignment_array = explode(',',$form_state['values']['max_alignments_list']);
    $in_list = 0;
    foreach($max_alignment_array as $max_alignment)
    {
        if($max_alignment == $form_state['values']['max_alignments_selected'])
        {
            $in_list = 1;
            break;
        }
    }
    if (!$in_list)
    {
        form_set_error('max_alignments_selected',t("Make sure the Max Alignments (Default) value is in the Available Max Alignments list."));
    }
}

/**
 * Implements hook_form_submit()
 *   Submits the new values to be used as job defaults
 */
function tseq_configuration_defaults_form_submit($form, &$form_state)
{
 $tseq_defaults = array(
        'settings_id'            => '0',
        'defaults_e_value'                => $form_state['values']['e_value'],
        'defaults_target_coverage'        => $form_state['values']['target_coverage'],
        'defaults_query_coverage'         => $form_state['values']['query_coverage'],
        'defaults_max_alignments_list' => $form_state['values']['max_alignments_list'],
        'defaults_max_alignments_selected' => $form_state['values']['max_alignments_selected'],        
    ); 
    drupal_write_record('tseq_settings', $tseq_defaults,'settings_id');
    drupal_set_message("The settings have been saved");
}

