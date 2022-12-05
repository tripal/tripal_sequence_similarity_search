<?php

namespace Drupal\tripal_seq\Form;

use Drupal\Core\Form\FormBase;
use Drupal\Core\Form\FormInterface;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\File\FileInterface;
use Drupal\Core\File\FileRepositoryInterface;
use Drupal\Core\File\FileSystemInterface;
use Drupal\tripal_seq\api;

class Tripal_SeqSubmitForm extends FormBase{

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

    // Allow managed_file fields to have values. https://www.drupal.org/project/drupal/issues/2647812#comment-11683961
    $form_state->disableCache();

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
echo "<hr> U <hr>";
    //////////////////////////////////////////////////////////////////////
    //                                                                  //
    //                          Query Section                           //
    //                                                                  //
    //////////////////////////////////////////////////////////////////////

    $form['QueryType'] = [
      '#type'          => 'radios',
      '#options'       => [
        'Protein'       => t('Protein'),
        'Genomic'       => t('Nucleotide (coding or whole genome)'),
      ],
      '#default_value' => 'Protein',
      '#title'        => t('Query Type'),
      '#attributes' => [
        'name' => 'QueryType',
      ],
    ];

    // Database search type (Protein). Depends on QueryType selection.
    $form['BlastEquivNuc'] = [
      '#type'         => 'select',
      '#title'        => t('Database Search Type (Nucleotide)'),
      '#empty_value' => '',
      '#default_value' => NULL,
      '#options'      => [
        'blastx' => t('BLASTx (Translated nucleotide query versus protein database)'),
        'blastn'  => t('BLASTn (Nucleotide query versus nucleotide database)'),
      ],
      '#attributes' => [
        'name' => 'BlastEquivNuc',
      ],
      '#states' => [
        'visible' => [
          ':input[name="QueryType"]' => ['value' => 'Genomic'],
        ],
      ],
    ];

    // Database search type (Nucleic). Depends on QueryType selection.
    $form['BlastEquivPro'] = [
      '#type'         => 'select',
      '#title' => t('Database Search Type (Protein)'),
      '#empty_value' => '',
      '#default_value' => NULL,
      '#options'      => [
        'blastp' => t('BLASTp (Protein query versus protein database)'),
        'tblastn' => t('tBLASTn (Protein query versus translated nucleotide database)'),
      ],
      '#attributes' => [
        'name' => 'BlastEquivPro',
      ],
      '#states' => [
        'visible' => [
          ':input[name="QueryType"]' => ['value' => 'Protein'],
        ],
      ],
    ];

    $form['Query']['QueryUploadType'] = [
      '#type' => 'radios',
      '#title' => 'Query',
      '#description'         => 'Either paste in your FASTA sequence(s) or upload them as a file.',
      '#options' => [
        'paste' => t('Paste'),
        'file_upload' => t('File upload'),
      ],
      '#default_value' => 'paste',
      '#title' => 'Query Upload method',
      '#attributes' => [
        'name' => 'QueryUploadType',
      ],
    ];

    $form['Query']['QueryPaste'] = [
      '#type'         => 'textarea',
      '#required'     => FALSE,
      '#states' => [
        'invisible' => [
          ':input[name="QueryUploadType"]' => ['value' => 'file_upload'],
        ],
      ],
    ];

    // managed_files can't be hidden. Put it in a container.
    $form['Query']['QueryFileContainer'] = [
      '#type' => 'container',
      '#states' => [
        'invisible' => [
          ':input[name="QueryUploadType"]' => ['value' => 'paste'],
        ],
      ],
    ];

    $form['Query']['QueryFileContainer']['QueryFile'] = [
      '#type'         => 'managed_file',
      '#upload_validators' => [
        'file_validate_extensions' => [
          'txt dmnd gz FA FAA FNN FASTA fa faa fnn fasta',
        ],
      ],
      '#upload_location' => 'public://tripal_seq/',
      '#attributes' => [
        'name' => 'QueryFile',
      ],
    ];

    $form['Otherfile'] = [
      '#type' => 'managed_file',
      '#title' => 'Otherfile',
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
        $count = get_type_category_count($type, $category['category_title']);
        if ($count) {
          $sets[$type][$category['category_title']] = $count;

          // Assemble the forms here.
          $databases = get_databases($type, $category, TRUE);

          // Assemble the #options array.
          unset($databaseSelect);
          $databaseSelect['default'] = '';
          foreach ($databases as $database) {
            $databaseSelect[$database->db_id] = $database->name;
          }

          // Assemble the #states=>'visible'/'invisible' array.
          $fieldVisibility = generateFieldVisibility($type);
        }
      }
    }

    // managed_files can't be hidden. Put it in a container.
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
      '#suffix' => t($submit_description),
    ];

    return $form;
  }

  /**
   * Validate the submission form.
   *
   *   There are basic validatiosn to be made.
   *   - Ensure each set of required fields has a value.
   *   - Ensure only one choice is made when there are multiple
   *     options.
   *
   *   There are also advanced validations to be made.
   *   - FASTA format for submitted queries is proper.
   *   - User uploaded proper type (gene/genome vs protein)
   *
   * @param array $form
   *   The form.
   * @param FormStateInterface $form_state 
   *   The form state.
   */
  public function validateForm(array &$form, FormStateInterface $form_state) {
    //dpm($form_state);
    // Basic validation: QueryType has been chosen.
    if ($form_state->getValue('QueryType') == NULL) {
      $form_state->setErrorByName('QueryType', $this->t('Please choose the type of query.'));
    }

    // Basic validation: Database Search Type (BlastEquivNuc or BlastEquivPro) has been chosen.
    if ($form_state->getValue('QueryType') == 'Protein') {
      // Check that BlastEquivPro has a value.
      if (!$form_state->getValue('BlastEquivPro')) {
        $form_state->setErrorByName('BlastEquivPro', $this->t('Please choose the database search type.'));
      }
    }
    elseif ($form_state->getvalue('QueryType') == 'Genomic') {
      // Check that BlastEquivNuc has a value.
      if (!$form_state->getValue('BlastEquivNuc')) {
        $form_state->setErrorByName('BlastEquivNuc', $this->t('Please choose the type of query.'));
      }
    }

    // Basic validation: pasted or uploaded queries have contents.
    if ($form_state->getValue('QueryUploadType') == 'paste' && !$form_state->getValue('QueryPaste')) {
      $form_state->setErrorByName('QueryPaste', $this->t('Please paste your query text.'));
    }
    if ($form_state->getValue('QueryUploadType') == 'file_upload' && !$form_state->getValue('QueryFile')) {
      $form_state->setErrorByName('QueryFile', $this->t('Please upload your query file.'));
      dpm($form_state->getValues('QueryFile'));
    }

    // Basic validation: Target.
    if ($form_state->getvalue('TargetDataType') == 'database') {
      // QueryType = Protein, BlastEquivNuc = blastx.
      if ($form_state->getValue('QueryType') == 'Protein' && $form_state->getValue('BlastEquivNuc' == 'blastx')) {
        echo 'd'; // Pumpkin.
      }
    }
  }

  /**
   * Submit the Diamond/BLAST submission form.
   *
   * @param array $form
   *   The form. From the form, input the following fields into
   *   the tseq_job_information table.
   *    - 
   *    - user_id.
   * @param FormStateInterface $form_state
   *   The form state.
   */
  public function submitForm(array &$form, FormStateInterface $form_state) {
    // Get the basic form values.
    $advanced['eValue'] = $form_state->getValue('eValue');
    $advanced['targetCoverage'] = $form_state->getValue('targetCoverage');
    $advanced['queryCoverage'] = $form_state->getValue('queryCoverage');
    $advanced['maxAlignments'] = $form_state->getValue('maxAlignments');

    // Query handling: pasted.
    if ($form_state->getValue('QueryUploadType') == 'paste') {
      // Create a file with the user's pasted values.
      $file = \Drupal::service('file.repository');
      $pasted_file = $file->writeData($form_state->getValue('QueryPaste'), 'public://tripal_seq/' . date('YMd_His') . '.fasta');

      // Get the file URI and convert it to an absolute path.
      $query_file_path = \Drupal::service('file_system')->realpath($pasted_file->getFileUri());
    }
    // Query handling: file upload.
    else {
      $queryFile = $form_state->getValue('QueryFile');

      // File uploads are not set permanent by default
      // $file = File::load($queryFile[0]);.
  
      // $file = \Drupal\file\Entity\File::load($queryFile[0]);
      // $file->setPermanent();
      // $file->save();
    }

    // If searching a database, which one?

    // dpm($form_state);
  }
}