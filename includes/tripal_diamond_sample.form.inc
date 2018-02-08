<?php

function tripal_diamond_submit_form($form, &$form_state)
{
    $form['Name'] = array(
      '#type' => 'textfield',
      '#title' => t('This is a textfield'),
      '#required' => true,
    );
    
    $form['Location'] = array(
      '#type' => 'textfield',
      '#title' => t('This is another textfield'),
      '#required' => true,
    );
    
    $form['Article'] = array(
      '#type' => 'textfield',
      '#title' => t('This is the third textfield'),
      '#required' => true,
    );
    
    $form['Paper'] = array(
      '#type' => 'textfield',
      '#title' => t('This is sparta!'),
      '#required' => true,
    );

    $form['Regular text'] = array(
      '#type' => 'fieldset',
      '#title' => 'This is the title text',
      '#collapsible' => TRUE,
      '#description' => t('This is some straight up text with no other elements attached.')
    );

    $form['submit_button'] = array(
        '#type' => 'submit',
        '#value' => t('Next'),
    );

    return $form;
}