<?php
namespace Drupal\tripal_seq\Form\Settings;

use Drupal\Core\Form\FormBase;
use Drupal\Core\Form\FormStateInterface;
use Drupal;

class Tripal_SeqAdminDefaultsForm extends FormBase {

  /**
   * {@inheritdoc}
   */
  public function getFormId() {
    return 'tripal_seq_admin_defaults_form';
  }

    /**
     * Build the form
     */
    function buildForm(array $form, FormStateInterface $form_state) {
        
        // Get the current/default values form the database
        $db = \Drupal::database();
        $table_name ='tseq_settings';
        $query = $db->select($table_name, 'ts')
                ->fields('ts', array(
                    'defaults_e_value',
                    'defaults_target_coverage',
                    'defaults_query_coverage',
                    'defaults_max_alignments_list',
                    'defaults_max_alignments_selected'
                ))
                ->condition('settings_id', 0);
        $results = $query->execute();
        $defaults = $results->fetchObject();

        // Build the form. The form names match exactly with the database rows.
        $form['Overview'] = [
            '#type'         => 'fieldset',
            '#title'        => 'Information',
            '#collapsible'  => TRUE,
            '#collapsed'    => FALSE,
            '#description'  => t('Use this form to set the default values for the "Advanced Options" portion of the'
                    . ' submission form. '),
        ];
        
        $form['defaults_e_value'] = [
            '#type'         =>  'textfield',
            '#title'        =>  'E-Value',
            '#size'         =>  '5',
            '#required'     =>  true,
            '#default_value'      => $defaults->defaults_e_value,
            //'#field_prefix' => 'Define the e-value',
        ];
        
        $form['defaults_target_coverage'] = [
            '#type'         =>  'textfield',
            '#title'        =>  'Target Coverage',
            '#size'         =>  '5',
            '#required'     =>  true,
            '#default_value'      => $defaults->defaults_target_coverage,
            //'#field_prefix' => 'Define, in days, how long to keep stored files pertaining to submitted jobs',
        ];
        
        $form['defaults_query_coverage'] = [
            '#type'         =>  'textfield',
            '#title'        =>  'Query Coverage',
            '#size'         =>  '5',
            '#required'     =>  true,
            '#default_value'      => $defaults->defaults_query_coverage,
            //'#field_prefix' => 'Define, in days, how long to keep stored files pertaining to submitted jobs',
        ];
        
        $form['defaults_max_alignments_list'] = [
            '#type'         =>  'textfield',
            '#title'        =>  'Available Max Alignments',
            '#size'         =>  '50',
            '#required'     =>  true,
            '#default_value'      => $defaults->defaults_max_alignments_list,
            '#description' => 'Provide a comma-seperated list of values available to the user to define Max Alignment value in their search',
        ];
        
        $form['defaults_max_alignments_selected'] = [
            '#type'         =>  'textfield',
            '#title'        =>  'Max Alignments (Default)',
            '#size'         =>  '5',
            '#required'     =>  true,
            '#default_value'      => $defaults->defaults_max_alignments_selected,
            //'#field_prefix' => 'Define, in days, how long to keep stored files pertaining to submitted jobs',
        ];
        
        $form['submit_button'] = [
            '#type' => 'submit',
            '#value' => t('Save'),
        ];

        return $form;
    }

    /**
     * Validate the form
     */
    function validateForm (array &$form, FormStateInterface $form_state) {

    }

    /**
     * Submit the form
     */
    function submitForm(array &$form, FormStateInterface $form_state) {
        $db = \Drupal::database();

        $result = $db->update('tseq_settings')
        ->fields(
            [
                'defaults_e_value' => $form_state->getValue('defaults_e_value'),
                'defaults_target_coverage' => $form_state->getValue('defaults_target_coverage'),
                'defaults_query_coverage' => $form_state->getValue('defaults_query_coverage'),
                'defaults_max_alignments_list' => $form_state->getValue('defaults_max_alignments_list'),
                'defaults_max_alignments_selected' => $form_state->getValue('defaults_max_alignments_selected'),
            ]
        )
        ->condition('settings_id', 0)
        ->execute();
    }

}