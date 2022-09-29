<?php

namespace Drupal\tripal_seq\Form\Database;

use Drupal\Core\Form\FormBase;
use Drupal\Core\Form\FormStateInterface;
use Drupal;

class Tripal_SeqAdminDBAddForm extends FormBase {
    /**
     * Form ID.
     * 
     * @return string
     */
    function getFormID() {
        return 'tripal_seq_admin_db_add_form';
    }

    /**
     * Build the form
     */
    function buildForm(array $form, FormStateInterface $form_state) {

        /**
         * Get the default/current values from the database
         */
        // Categories
        $db = \Drupal::database();
        $table_name = 'tseq_categories';
        $query = $db->select($table_name, 'tseq_cat')
                ->fields('tseq_cat');
        $results = $query->execute();

        $category_options = [];
        while(($result = $results->fetchObject())) {
            if ($result->enabled == 1) {
                $category_options[$result->category_id] = $result->category_title; 
            }
            else {
                $category_options[$result->category_id] = $result->category_title . ' (disabled)'; 
            }
        }

        // Fields for this form
        $form['Name'] = [
            '#type' => 'textfield',
            '#title' => $this->t('Name'),
            '#description' => 'The name of the target database.',
        ];

        $form['Type'] = [
            '#type' => 'select',
            '#title' => $this->t('Type'),
            '#options' => [
                'Protein'   => 'Protein',
                'Genome'    => 'Genome',
                'Gene'      => 'Gene',
            ],
        ];

        $form['Category'] = [
            '#type' => 'select',
            '#title' => $this->t('Category'),
            '#options' => $category_options,
        ];
        
        $form['Version'] = [
            '#type'         => 'textfield',
            '#title'        => $this->t('Version'),
            '#size'         => '7',
            '#description'  => 'The version of the added database',
            '#required'     => true,
        ];
    
        $form['Location'] = [
            '#type'         => 'textfield',
            '#title'        => $this->t('File location'),
            '#size'         => '120',
            '#description'  => 'The path to the indexed database on disk (accessible on the remote server). If loading a BLAST index, leave off the extensions like nhr, nin, nog, etc.',
            '#suffix'       => '',
            '#required'     => true,
        ];
    
        $form['WebLocation'] = [
            '#type'         => 'textfield',
            '#title'        => $this->t('Web Location (Optional)'),
            '#size'         => '120',
            '#description'  => 'If the original sequence (non-indexed) is publicly available for download, set the URL here (FTP, HTTP)',
            '#required'     => false,
        ];
    
        $form['Count'] = [
            '#type'         => 'textfield',
            '#title'        => $this->t('Genes, Proteins, or Scaffolds (Optional)'),
            '#size'         => '10',
            '#description'  => 'How many genes, proteins, or scaffolds the sequence contains'
        ];

        // and finally, submit
        $form['actions']['#type'] = 'actions';
        $form['actions']['submit'] = [
            '#type' => 'submit',
            '#value' => $this->t('Add'),
            '#button_type' => 'primary',
        ];

        return $form;
    }

    /**
     * Validate!
     */
    public function validateForm(array &$form, FormStateInterface $form_state) {
        // placeholder because required by FormInterface definition (not documented)
        /**
         * Validations
         *  [Name, Version, Type] must be unique and not in the database
         *  File location is accessible to whichever user (this is tricky)
         */
    }

    /**
     * Submit
     */
    function submitForm(array &$form, FormStateInterface $form_state) {
        // If we got here, we must have passed validation. Let's store the values in the database.
        $db = \Drupal::database();

        $result = $db->insert('tseq_db_existing_locations')
                ->fields(
                    [
                        'type' => $form_state->getValue('Type'),
                        'name' => $form_state->getValue('Name'),
                        'version' => $form_state->getValue('Version'),
                        'location' => $form_state->getValue('Location'),
                        'category' => $form_state->getValue('Category'),
                        'web_location' => $form_state->getValue('WebLocation'),
                        'count' => $form_state->getValue('Count'),
                        'status' => 1,
                    ]
                )
                ->execute();
    }
}