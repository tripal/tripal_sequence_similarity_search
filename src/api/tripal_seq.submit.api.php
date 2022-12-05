<?php

/**
 * @file
 * API for the functions for use on the main submission page.
 */

/**
 * @defgroup tripal_seq_submit_api Submit
 * @ingroup tripal_seq_api
 * @{
 * API for the functions for use on the main submission page.
 * }@
 */

/**
 * Returns an array of (in)visibility rules.
 *
 * These are for the database select lists on the main
 * submission page.
 *
 * @param string $type
 *   The type of database listed (protein/nucleotide).
 *
 * @return array
 *   An associative array with keys 'visible' and 'invisible'.
 */
function generateFieldVisibility($type) {
  if ($type == 'Protein') {
    $fieldVisibility['visible'] = [
      [
        ':input[name="QueryType"]' => ['value' => 'Protein'],
        ':input[name="BlastEquivPro"]' => ['value' => 'blastp'],
      ],
      [
        ':input[name="QueryType"]' => ['value' => 'Genomic'],
        ':input[name="BlastEquivNuc"]' => ['value' => 'blastx'],
      ],
    ];
    $fieldVisibility['invisible'] = [
      ':input[name="TargetDataType"]' => [
        ['value' => 'paste'],
        ['value' => 'upload'],
      ],
    ];

    return $fieldVisibility;
  }
}