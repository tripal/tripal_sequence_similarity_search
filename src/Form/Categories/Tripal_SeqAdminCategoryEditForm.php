<?php

namespace Drupal\tripal_seq\Form\Categories;

use Drupal;
use Drupal\Core\Form\FormBase;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\Link;
use Drupal\Core\Messenger\MessengerInterface;

class Tripal_SeqAdminCategoryEditForm extends FormBase {
    /**
     * Form ID.
     * 
     * @return string
     */
    function getFormID() {
        return 'tripal_seq_admin_category_edit_form';
    }

    /**
     * Build the form
     */
    function buildForm(array $form, FormStateInterface $form_state, String $category_id = NULL) {
        /**
         * Get the current values from the database for this category
         */
        $db = \Drupal::database();
        $table_name = 'tseq_categories';
        $query = $db->select($table_name, 'tc')
                ->fields('tc')
                ->condition('tc.category_id',$category_id);
        
        $results = $query->execute();
        $current_value = $results->fetchObject();

        $form['category_title'] = [
            '#type'         =>  'textfield',
            '#title'        =>  'Category Title',
            '#size'         =>  '50',
            '#description'  =>  'The name of the category.',
            '#required'     =>  true,
            '#default_value'      =>  $current_value->category_title,
        ];
        
        $form['enabled'] = [
            '#type'         => 'select',
            '#title'        => 'Enabled',
            '#description'  => 'Show this category on the Submit page',
            '#options'      => array(
                '1'             => t('Yes'),
                '0'             => t('No'),
            ),
            '#default_value'      =>  $current_value->enabled,
        ];
        
        $form['category_id'] = [
            '#type'         =>  'value',
            '#value'        =>  $category_id,
        ];
        
        $form['submit_button'] = [
            '#type' => 'submit',
            '#value' => t('Update values'),
        ];

        return $form;
    }

    function validateForm(array &$form, FormStateInterface $form_state) {
    }

    function submitForm(array &$form, FormStateInterface $form_state) {
        $db = \Drupal::database();

        $result = $db->update('tseq_categories')
                ->fields(
                    [
                        'category_title' => $form_state->getValue('category_title'),
                        'enabled' => $form_state->getValue('enabled'),
                    ]
                )
                ->condition('category_id', $form_state->getValue('category_id'))
                ->execute();
        
        $this->messenger = \Drupal::messenger();
        $this->messenger->addMessage('Category successfully updated. Use the link in the breadcrumb above to return to the Database list.', 'status');
    }
}