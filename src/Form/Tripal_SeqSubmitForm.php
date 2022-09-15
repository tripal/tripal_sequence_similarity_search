<?php

namespace Drupal\tripal_seq\Form;

use Drupal;
use Drupal\Core\Form\FormInterface;
use Drupal\Core\Form\FormStateInterface;

class Tripal_SeqSubmitForm implements FormInterface{

    public function buildForm(array $form, FormStateInterface $form_state) {
        // Build the form
    }

    public function validateForm(array &$form, FormStateInterface $form_state) {
        // Validate the submitted values and the general form state
    }

    public function submitForm(array &$form, FormStateInterface $form_state) {
        // Submit the form and perform other functions related to submissions
    }


}