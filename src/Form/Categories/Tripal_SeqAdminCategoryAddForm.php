<?php

namespace Drupal\tripal_seq\Form\Categories;

use Drupal\Core\Form\FormBase;
use Drupal\Core\Form\FormStateInterface;
use Drupal;

class Tripal_SeqAdminCategoryAddForm extends FormBase {

  /**
   * {@inheritdoc}
   */
  public function getFormId() {
    return 'tripal_seq_admin_category_add_form';
  }

    /**
     * Build the form
     */
    function buildForm(array $form, FormStateInterface $form_state) {

        $form['category_title'] = array(
            '#type'         => 'textfield',
            '#title'        => 'Name',
            '#size'         => '50',
            '#description'  => 'The name of the category',
            '#required'     => true,
        );
        
        $form['enabled'] = array(
            '#type'         => 'select',
            '#title'        => 'Enabled',
            '#description'  => 'Show this category on the Submit page',
            '#options'      => array(
                '1'       => $this->t('Yes'),
                '0'       => $this->t('No'),
            ),
            '#default_value'    => '1',
            '#required'     => true,
        );

        $form['submit_button'] = array(
            '#type' => 'submit',
            '#value' => t('Add'),
        );
        
        return $form;
    }

    /**
     * Validate!
     */
    // public function validateForm(array &$form, FormStateInterface $form_state) {
    //     // placeholder because required by FormInterface definition (not documented)
    //     /**
    //      * Validations
    //      *  Name must be unique and not in the database
    //      */
    // }

    function submitForm(array &$form, FormStateInterface $form_state) {
        $db = \Drupal::database();

        $result = $db->insert('tseq_categories')
                ->fields(
                    [
                        'category_title' => $form_state->getValue('category_title'),
                        'enabled' => $form_state->getValue('enabled'),
                    ]
                )
                ->execute();
    }
}