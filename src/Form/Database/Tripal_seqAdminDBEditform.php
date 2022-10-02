<?php

namespace Drupal\tripal_seq\Form\Database;

use Drupal;
use Drupal\Core\Form\FormBase;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\Link;
use Drupal\Core\Messenger\MessengerInterface;

class Tripal_SeqAdminDBEditForm extends FormBase {
    /**
     * Form ID.
     * 
     * @return string
     */
    function getFormID() {
        return 'tripal_seq_admin_db_edit_form';
    }

    /**
     * Build the form
     */
    function buildForm(array $form, FormStateInterface $form_state, String $db_id = NULL) {
        /**
         * Get the current values from the database for this... database
         */
        $db = \Drupal::database();
        $table_name = 'tseq_db_existing_locations';
        $query = $db->select($table_name, 'tseq_db')
                ->fields('tseq_db')
                ->condition('tseq_db.db_id',$db_id);
        
        $results = $query->execute();
        $current_value = $results->fetchObject();

        /**
         * Get the current list of categories from the database
         * @todo make this into a function because it is reused
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

        // Put a link back to the database list 
        /**
         * Lay out the form fields
         */
        $form['Name'] = [
            '#type' => 'textfield',
            '#title' => $this->t('Name'),
            '#description' => 'The name of the target database.',
            '#default_value' => $current_value->name,
        ];

        $form['Type'] = [
            '#type' => 'select',
            '#title' => $this->t('Type'),
            '#options' => [
                'Protein'   => 'Protein',
                'Genome'    => 'Genome',
                'Gene'      => 'Gene',
            ],
            '#default_value' => $current_value->type,
        ];

        $form['Category'] = [
            '#type' => 'select',
            '#title' => $this->t('Category'),
            '#options' => $category_options,
            '#default_value' => $current_value->category,
        ];
        
        $form['Version'] = [
            '#type'         => 'textfield',
            '#title'        => $this->t('Version'),
            '#size'         => '7',
            '#description'  => 'The version of the added database',
            '#required'     => true,
            '#default_value' => $current_value->version,
        ];
    
        $form['Location'] = [
            '#type'         => 'textfield',
            '#title'        => $this->t('File location'),
            '#size'         => '120',
            '#description'  => 'The path to the indexed database on disk (accessible on the remote server). If loading a BLAST index, leave off the extensions like nhr, nin, nog, etc.',
            '#suffix'       => '',
            '#required'     => true,
            '#default_value' => $current_value->location,
        ];
    
        $form['WebLocation'] = [
            '#type'         => 'textfield',
            '#title'        => $this->t('Web Location (Optional)'),
            '#size'         => '120',
            '#description'  => 'If the original sequence (non-indexed) is publicly available for download, set the URL here (FTP, HTTP)',
            '#required'     => false,
            '#default_value' => $current_value->web_location,
        ];
    
        $form['Count'] = [
            '#type'         => 'textfield',
            '#title'        => $this->t('Genes, Proteins, or Scaffolds (Optional)'),
            '#size'         => '10',
            '#description'  => 'How many genes, proteins, or scaffolds the sequence contains',
            '#default_value' => $current_value->count,
        ];

        // Store the db_id in the form for validation and submission
        $form['db_id'] = [
            '#type'         =>  'value',
            '#value'        =>  $db_id,
        ];

        // and finally, submit changes
        $form['actions']['#type'] = 'actions';
        $form['actions']['submit'] = [
            '#type' => 'submit',
            '#value' => $this->t('Save'),
            '#button_type' => 'primary',
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

        $result = $db->update('tseq_db_existing_locations')
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
                ->condition('db_id', $form_state->getValue('db_id'))
                ->execute();

        $this->messenger = \Drupal::messenger();
        // Craft link for returning to the Database list
        /* Rely on the breadcrumb for now
            $return_link = Link::createFromRoute('here', 'tripal_seq.config')
                ->toString()
                ->getGeneratedLink();
         */
        $this->messenger->addMessage('Database successfully updated. Use the link in the breadcrumb above to return to the Database list.', 'status');
    }
}