<?php

namespace Drupal\tripal_seq\Form;

use Drupal\Core\Form\FormBase;
use Drupal\Core\Form\FormStateInterface;

class Tripal_SeqAdminDBDeleteForm extends FormBase {
    /**
     * Form ID.
     * 
     * @return string
     */
    function getFormID() {
        return 'tripal_seq_admin_db_delete_form';
    }

    /**
     * Build the form
     */
    function buildForm(array $form, FormStateInterface $form_state, String $db_id = NULL) {

        /**
         * Check if the database entry exists with the given db_id
         */
        $db = \Drupal::database();
        $table_name = 'tseq_db_existing_locations';
        $query = $db->select($table_name, 'tseq_db')
            ->fields('tseq_db',['name', 'version'])
            ->condition('tseq_db.db_id',$db_id)
            ->execute();
        
        $results = $query->fetchObject();

        $form['Instructions'] = [
            '#type' => 'fieldset',
            '#title' => 'Are you sure?',
            '#collapsible' => FALSE,
            '#description' => $this->t('This will only delete the entry, it will not delete the file from the server'),
            '#suffix' => $this->t('You are attempting to delete<i> ' . $results->name . ' </i>, version ' . $results->version),
        ];

        // Store the db_id in the form for validation and submission
        $form['db_id'] = [
            '#type'         =>  'value',
            '#value'        =>  $db_id,
        ];

        $form['actions'] = [
            '#type' => 'actions',
        ];

        $form['actions']['delete'] = [
            '#type' => 'submit',
            '#value' => $this->t('Delete'),
            '#button_type' => 'primary',
        ];
        $form['actions']['cancel'] = [
            '#type' => 'button',
            '#value' => $this->t('Cancel'),
            '#button_type' => 'secondary',
        ];

        return $form;
    }

    function validateForm(array &$form, FormStateInterface $form_state) {

    }

    /**
     * Delete
     * @todo validate that the value gets deleted, warn if not
     * @todo redirect properly after deleting success
     * @todo handle the 'Cancel' button above (redirect back to db list)
     */
    function submitForm(array &$form, FormStateInterface $form_state) {
        $db = \Drupal::database();

        $deleted = $db->delete('tseq_db_existing_locations')
            ->condition('db_id', $form_state->getValue['db_id'])
            ->execute();
        $this->messenger = \Drupal::messenger();
        $this->messenger->addMessage('Database successfully deleted. Use the link in the breadcrumb above to return to the Database list.', 'status');

    }
}