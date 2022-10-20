<?php

namespace Drupal\tripal_seq\Form;

use Drupal;
use Drupal\Core\Form\FormInterface;
use Drupal\Core\Form\FormStateInterface;

//use Drupal\tripal_seq\api;

class Tripal_SeqSubmitForm implements FormInterface{

  /**
   * Form ID.
   * 
   * @return string
   */
  function getFormID() {
    return 'tripal_seq_submi_form';
  }

  function buildForm(array $form, FormStateInterface $form_state) {
    $this->messenger = \Drupal::messenger();
    // Build the form
    // Check if user is logged in. User must be logged in to submit.
    // @todo this

    // Check if the working directory is available to write.
    if (!tseq_check_local_directories()) {
      $this->messenger->addError('The local working directory for Tripal jobs is not writable. Report this to an administrator using the Contact form.');
    }

    $form['debug'] = [
      '#type' => 'select',
      '#title' => 'debug',
      '#options' => [
        get_categories(TRUE),
      ],
    ];

    $form['Overview'] = array(
      '#type'         => 'fieldset',
      '#title'        => 'Tripal Similarity Search Overview',
      '#collapsible'  => TRUE,
      '#collapsed'    => FALSE,
      '#description'  => t('Sequence similarity search is supported against genes, '
              . 'proteins, and full genomes.  Nucleotide searches are executed '
              . 'with BLAST (BLASTN) while protein or translated protein searches are executed with '
              . 'Diamond (BLASTX or BLASTP equivalent).  Diamond will execute searches in a fraction '
              . 'of the time of BLAST at a similar sensitivity.  Both packages accept similar input '
              . 'parameters and these can be modified below.  You may upload FASTA formatted sequences '
              . 'or paste FASTA formatted sequences for searching.  You can select from a list of '
              . 'pre-formatted Diamond and BLAST databases OR upload your own.  This can be '
              . 'pre-formatted or provided in FASTA format and formatted for you.'),
    );

    //////////////////////////////////////////////////////////////////////
    //                                                                  //
    //                          Query Section                           //
    //                                                                  //
    //////////////////////////////////////////////////////////////////////

    $form['QueryType'] = [
      '#type'          => 'radios',
      '#default_value' => 'Protein',
      '#options'       => [
          'Protein'       => t('Protein'),
          'Genomic'       => t('Nucleotide (coding or whole genome)')
      ],
      '#title'        => t('Query Type'),
    ];

    // Database search type (Protein. Depends on QueryType selection.
    $form['BlastEquivNuc'] = [
      '#type'         => 'select',
      '#title'        => t('Database Search Type'),
      '#options'      => [
        'blastx' => t('BLASTx (Translated nucleotide query versus protein database)'),
        'blastn'  => t('BLASTn (Nucleotide query versus nucleotide database)'),
      ],
      '#states' => [
        'visible' => [
          ':input[name="QueryType"]' => ['value' => 'Genomic'],
        ],
      ],
    ];

    // Database serach type (Nucleic). Depends on QueryType selection.
    $form['BlastEquivPro'] = [
      '#type'         => 'select',
      '#title'        => t('Database Search Type'),
      '#options'      => [
        'blastp' => t('BLASTp (Protein query versus protein database)'),
        'tblastn' => t('tBLASTn (Protein query versus translated nucleotide database)'),
      ],
      '#states' => [
        'visible' => [
          ':input[name="QueryType"]' => ['value' => 'Protein'],
        ],
      ],
    ];

    /* Query Input Section */
    $form['QueryLabel'] = array(
      '#type'           => 'item',
      '#title'          => 'Query',
      '#prefix'         => '<hr>',
    );

    $form['Query'] = array(
      '#type'           => 'container',
      '#prefix'         => 'Either paste in your FASTA sequence(s) or upload them as a file.',
      '#attributes'     => array(
        'class' => array(
          'tseq_submit_form_major_section'),
      ),
    );

    return $form;
    }

    public function validateForm(array &$form, FormStateInterface $form_state) {
        // Validate the submitted values and the general form state
    }

    public function submitForm(array &$form, FormStateInterface $form_state) {
        // Submit the form and perform other functions related to submissions
    }
}