<?php

namespace Drupal\tripal_seq\Form\Categories;

use Drupal\Core\Form\FormBase;
use Drupal\Core\Form\FormStateInterface;

class Tripal_SeqAdminCategoryDeleteForm extends FormBase {

  /**
   * {@inheritdoc}
   */
  public function getFormId() {
    return 'tripal_seq_admin_category_delete_form';
  }

    /**
     * Build the form
     * 
     * @todo Alert the user that there are x number of databases with this category
     */
    function buildForm(array $form, FormStateInterface $form_state, String $category_id = NULL) {

        /**
         * Check if the database entry exists with the given category_id
         */
        $db = \Drupal::database();
        $table_name = 'tseq_categories';
        $query = $db->select($table_name, 'tseq_categories')
            ->fields('tseq_categories',['category_title', 'enabled'])
            ->condition('tseq_categories.category_id',$category_id)
            ->execute();
        
        $results = $query->fetchObject();

        $form['Instructions'] = [
            '#type' => 'fieldset',
            '#title' => 'Are you sure?',
            '#collapsible' => FALSE,
            '#description' => $this->t('This will delete the category.'),
            '#suffix' => $this->t('You are attempting to delete<i> ' . $results->category_title . ' </i>'),
        ];

        // Store the category_id in the form for validation and submission
        $form['category_id'] = [
            '#type'         =>  'value',
            '#value'        =>  $category_id,
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
     * @todo handle the 'Cancel' button above (redirect back to category list)
     */
    function submitForm(array &$form, FormStateInterface $form_state) {
        $db = \Drupal::database();

        $deleted = $db->delete('tseq_categories')
            ->condition('category_id', $form_state->getValue('category_id'))
            ->execute();
        $this->messenger = \Drupal::messenger();
        $this->messenger->addMessage('Category successfully deleted. Use the link in the breadcrumb above to return to the Database list.', 'status');

    }
}