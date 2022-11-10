<?php

namespace Drupal\tripal_seq\Form;

use Drupal;
use Drupal\Core\Form\FormInterface;
use Drupal\Core\Form\FormStateInterface;
use Drupal\tripal_seq\api;

class Tripal_SeqSubmitForm implements FormInterface{

  /**
   * {@inheritdoc}
   */
  public function getFormId() {
    return 'tripal_seq_submit_form';
  }

  /**
   * {@inheritdoc}
   */
  public function buildForm(array $form, FormStateInterface $form_state) {
    $this->messenger = \Drupal::messenger();
    // Build the form
    // Check if user is logged in. User must be logged in to submit.
    // @todo this

    // Check if the working directory is available to write.
    if (!tseq_check_local_directories()) {
      $this->messenger->addError('The local working directory for Tripal jobs is not writable. Report this to an administrator using the Contact form.');
    }

    $description = 'Sequence similarity search is supported against genes, '
      . 'proteins, and full genomes.  Nucleotide searches are executed '
      . 'with BLAST (BLASTN) while protein or translated protein searches are executed with '
      . 'Diamond (BLASTX or BLASTP equivalent).  Diamond will execute searches in a fraction '
      . 'of the time of BLAST at a similar sensitivity.  Both packages accept similar input '
      . 'parameters and these can be modified below.  You may upload FASTA formatted sequences '
      . 'or paste FASTA formatted sequences for searching.  You can select from a list of '
      . 'pre-formatted Diamond and BLAST databases OR upload your own.  This can be '
      . 'pre-formatted or provided in FASTA format and formatted for you.';
    $form['Overview'] = array(
      '#type'         => 'fieldset',
      '#title'        => 'Tripal Similarity Search Overview',
      '#collapsible'  => TRUE,
      '#collapsed'    => FALSE,
      '#description'  => t($description),
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
      '#attributes' => [
        'name' => 'QueryType',
      ],
    ];

    // Database search type (Protein. Depends on QueryType selection.
    $form['BlastEquivNuc'] = [
      '#type'         => 'select',
      '#title'        => t('Database Search Type'),
      '#options'      => [
        'blastx' => t('BLASTx (Translated nucleotide query versus protein database)'),
        'blastn'  => t('BLASTn (Nucleotide query versus nucleotide database)'),
      ],
      '#default_value' => 'blastn',
      '#attributes' => [
        'name' => 'BlastEquivNuc',
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
      '#default_value' => 'tblastn',
      '#attributes' => [
        'name' => 'BlastEquivPro',
      ],
      '#states' => [
        'visible' => [
          ':input[name="QueryType"]' => ['value' => 'Protein'],
        ],
      ],
    ];

    /* Query Input Section */
    $form['QueryLabel'] = [
      '#type'           => 'item',
      '#title'          => 'Query',
      '#prefix'         => '<hr>',
    ];

    $form['Query'] = [
      '#type'           => 'container',
      '#prefix'         => 'Either paste in your FASTA sequence(s) or upload them as a file.',
      '#attributes'     => [
        'class' => [
          'tseq_submit_form_major_section',
        ],
      ],
    ];

    $form['Query']['QueryPaste'] = [
      '#type'         => 'textarea',
      '#required'     => FALSE,
    ];

    $form['Query']['QueryFile'] = [
      '#type'         => 'managed_file',
      '#upload_validators' => [
        'file_validate_extensions' => [
          'txt dmnd gz FA FAA FNN FASTA fa faa fnn fasta',
        ],
      ],
      '#upload_location' => 'public://',
    ];

    //////////////////////////////////////////////////////////////////////
    //                                                                  //
    //                         Target Section                           //
    //                                                                  //
    //////////////////////////////////////////////////////////////////////

    $form['TargetLabel'] = [
      '#type'           => 'item',
      '#title'          => 'Target/Database Selection',
      '#prefix'         => '<hr>',
    ];
  
    $form['Target'] = [
      '#type'           => 'container',
      '#attributes'     => [
        'class' => [
          'tseq_submit_form_major_section'
        ],
      ],
    ];

    $form['Target']['OtherTarget']['TargetDataType'] = [
      '#type'         => 'radios',
      '#prefix'        => t('Choose how you will provide your target/database:'),
      '#default_value' => 'database',
      '#options'      => [
        'database' => t('Select from an existing database on this site'),
        'upload' => t('Upload a File'),
        'paste' => t('Type or paste manually'),
      ],
      '#attributes' => [
        'name' => 'TargetDataType',
      ],
    ];

    // Prepare info for displaying available databases per category and type.
    $categories = get_categories(TRUE);
    $types = get_types();

    foreach ($categories as $category) {
      foreach ($types as $type) {
        $count = get_type_category_count($type,$category['category_title']);
        if ($count) {
          $sets[$type][$category['category_title']] = $count;
        }
      }
    }

    unset($categories);
    unset($type);

    // The basis for generating the target lists.
    foreach ($sets as $type => $categories) {
      foreach ($categories as $category => $count) {

        // Now get all databases in these type/category pairs. We want their db_id, name, count, and maybe version so let's grab that.
        unset($databaseSelect);
        $databaseSelect = [];
        $databaseSelect['default'] = '--';
        $databases = get_databases($type, $category, TRUE);

        foreach ($databases as $database) {
          $databaseSelect[$database->db_id] = $database->name;
        }

        unset($fieldVisiblity);
        unset($fieldInvisiblity);

        // Visibility rules, Protein.
        if ($type == 'Protein') {
          $title = 'Protein & blastp OR Genomic & blastx';
          $fieldVisiblity = [
            [
              ':input[name="QueryType"]' => ['value' => 'Protein'],
              ':input[name="BlastEquivPro"]' => ['value' => 'blastp'],
            ],
            [
              ':input[name="QueryType"]' => ['value' => 'Genomic'],
              ':input[name="BlastEquivNuc"]' => ['value' => 'blastx'],
            ],
          ];
          $fieldInvisiblity = [
            ':input[name="TargetDataType"]' => [
              ['value' => 'paste'],
              ['value' => 'upload'],
            ],
          ];
        }
        // Visibility rules, Genome/Gene.
        else {
          $title = "Protein & tblastn OR Genomic & blastn";
          $fieldVisiblity = [
            [
              ':input[name="QueryType"]' => ['value' => 'Protein'],
              ':input[name="BlastEquivPro"]' => ['value' => 'tblastn'],
            ],
            [
              ':input[name="QueryType"]' => ['value' => 'Genomic'],
              ':input[name="BlastEquivNuc"]' => ['value' => 'blastn'],
            ],
          ];
          $fieldInvisiblity = [
            ':input[name="TargetDataType"]' => [
              ['value' => 'paste'],
              ['value' => 'upload'],
            ],
          ];
        }

        // Title configure.
        $title .= '(' . $type . ", ";
        $title .= $category . ')';

        // Assemble the form.
        $form['Target'][$type . '_' . $category] = [
          '#type' => 'select',
          '#title' => $title,
          '#options' => $databaseSelect,
          '#states' => [
            'visible' => $fieldVisiblity,
            'invisible' => $fieldInvisiblity,
          ],
        ];
      }
    }

    // managed_files can't be hidden I guess. Put it in a container.
    $form['Target']['OtherTarget']['TargetFileContainer'] = [
      '#type' => 'container',
      '#states' => [
        'visible' => [
          ':input[name="TargetDataType"]' => ['value' => 'upload'],
        ],
        'invisible' => [
          ':input[name="TargetDataType"]' => [
            ['value' => 'paste'],
            ['value' => 'database'],
          ],
        ],
      ],
    ];

    $form['Target']['OtherTarget']['TargetFileContainer']['TargetFile'] = [
      '#type'         => 'managed_file',
      '#title'        => t('Target File'),
      '#upload_validators' => [
        'file_validate_extensions' => ['txt dmnd gz fasta FASTA faa fnn'],
      ],
      '#upload_location' => 'public://',
    ];

    $form['Target']['OtherTarget']['TargetPaste'] = [
      '#type'         => 'textarea',
      '#title'        => t('Raw target data'),
      '#required'     => FALSE,
      '#states' => [
        'visible' => [
          ':input[name="TargetDataType"]' => ['value' => 'paste'],
        ],
        'invisible' => [
          ':input[name="TargetDataType"]' => [
            ['value' => 'upload'],
            ['value' => 'database'],
          ],
        ],
      ],
    ];

    // ///////////////////////////////////////////////////////////////////
    // /                                                                //
    // /                   Advanced Setting Section                     //
    // /                                                                //
    // ///////////////////////////////////////////////////////////////////

    // Fetch the default values for advanced settings from the database.
    $default_settings = tseq_get_defaults();

    // Make the list of max alignments an array and trim whitespace.
    $default_settings['defaults_max_alignments_list'] = array_map('trim', explode(',', $default_settings['defaults_max_alignments_list']));

    // Prep the list to be used in the form.
    foreach ($default_settings['defaults_max_alignments_list'] as $max_alignment) {
      $max_alignments[$max_alignment] = $max_alignment;
    }

    $form['Advanced'] = [
      '#type' => 'details',
      '#title'  => 'Advanced Options',
    ];

    $form['Advanced']['eValue'] = [
      '#type' => 'textfield',
      '#title' => 'E-value',
      '#size' => '10',
      '#required' => TRUE,
      '#default_value' => $default_settings['defaults_e_value'],
    ];

    $form['Advanced']['targetCoverage'] = [
      '#type' => 'textfield',
      '#title' => 'Target Coverage',
      '#size' => '10',
      '#required' => TRUE,
      '#default_value' => $default_settings['defaults_target_coverage'],
    ];

    $form['Advanced']['queryCoverage'] = [
      '#type' => 'textfield',
      '#title' => 'Query Coverage',
      '#size' => '10',
      '#required' => TRUE,
      '#default_value' => $default_settings['defaults_query_coverage'],
    ];

    $form['Advanced']['maxAlignments'] = [
      '#type' => 'select',
      '#title' => 'Max Alignments',
      '#required' => TRUE,
      '#options' => $max_alignments,
      '#default_value' => $default_settings['defaults_max_alignments_selected'],
    ];

    $submit_description = 'Upon submission, your search will enter the '
      . 'queue with either BLAST or Diamond as the engine. You will '
      . ' be given a link to view the progress and results of the job.';

    $form['submit_button'] = [
      '#type' => 'submit',
      '#value' => t('Submit'),
      'description' => t($submit_description),
    ];

    return $form;
  }

    public function validateForm(array &$form, FormStateInterface $form_state) {
        // Validate the submitted values and the general form state
    }

    public function submitForm(array &$form, FormStateInterface $form_state) {
        // Submit the form and perform other functions related to submissions
    }
}