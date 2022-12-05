<?php

/**
 * @settings
 * Provides an API for managing settings used by
 * Tripal Sequence Similarity Search
 */

/**
 * @defgroup tripal_seq_settings_api Settings
 * @ingroup tripal_seq_api
 * @{
 * Provides an API for managing settings used by
 * Tripal Sequence Similarity Search
 * }@
 */

/**
 * Returns an array of categories.
 *
 * If specified, can return only those
 * that are set to 'enabled' (1) in the database. Defaults to FALSE (return all)
 *
 * @param enabled boolean Only return enabled categories?
 * @return array of category_id, category_name, and enabled values
 */
function get_categories($only_enabled = FALSE) {
  $db = \Drupal::database();

  $table_name = 'tseq_categories';
  $enabled = ($only_enabled) ? '1' : '0';
  $query = $db->select($table_name, 'tseq_categories')
    ->fields('tseq_categories', ['category_id', 'category_title', 'enabled'])
    ->condition('tseq_categories.enabled', $enabled);
  $results = $query->execute();

  foreach ($results as $result) {
    $categories[] = [
      'category_id' => $result->category_id,
      'category_title' => $result->category_title,
      'enabled' => $result->enabled,
    ];
  }
  
  return $categories;
}

/**
 * Get list of category counts. Is this even necessary?
 *
 * @param boolean $only_enabled
 *
 * @return array
 */
// function get_categories_counts($only_enabled = FALSE) {
//   $db = \Drupal::database();
//   $table_name = 'tseq_categories';
//   $enabled = ($only_enabled) ? '1' : '0';
//   $query = $db->select($table_name, 'tseq_categories')
//     ->fields('tseq_categories', ['category_id', 'category_title', 'enabled'])
//     ->condition('tseq_categories.enabled', $enabled);
//   $query->addExpression('count(category_tile)'
// }

/**
 * Returns an array of distinct types (Protein, Nucleotide)
 */
function get_types() {
  $db = \Drupal::database();

  $table_name = 'tseq_db_existing_locations';
  $query = $db->select($table_name, 'tseq_db_existing_locations')
    ->fields('tseq_db_existing_locations', ['type']);
  $results = $query->distinct()->execute();
  foreach ($results as $result) {
    $types[] = $result->type;
  }

  return $types;
}

/**
 * Get distinct types of databases and the quantity of each.
 */
function get_type_counts() {
  $db = \Drupal::database();

  $table_name = 'tseq_db_existing_locations';
  // Do each part of the query separately for some reason.
  $query = $db->select($table_name, 'db_locs')
    ->fields('db_locs', ['type']);
  $query->addExpression('count(type)', 'type_count');
  $query->groupBy('db_locs.type');
  $results = $query->execute()->fetchAllAssoc('type');

  foreach ($results as $result) {
    $type_counts[$result->type] = $result->type_count;
  }
}

/**
 * Get count of how many rows that match criteria (type and category).
 * 
 * @todo Implement status here. We only want to return databases that are available
 *       which is indicated by status = 0
 */
function get_type_category_count($type, $category, $status = 0) {
  $db = \Drupal::database();

  $table_name = 'tseq_db_existing_locations';
  $query = $db->select($table_name, 'db_locs');
  $andFilter = $query->andConditionGroup()
    ->condition('db_locs.type', $type, '=')
    ->condition('db_locs.category', $category, '=');
  $query->condition($andFilter);
  $results = $query->countQuery()->execute()->fetchField();
  return $results;
}

/**
 * Get Diamond/BLAST databases from the database based on type and category.
 * 
 * @return array of database values (id, name, count, and version )
 * 
 * @todo Implement status here. We only want to return databases that are available
 *       which is indicated by status = 0
 */
function get_databases($type,$category,$only_enabled) {
  $db = \Drupal::database();

  $table_name = 'tseq_db_existing_locations';
  $query = $db->select($table_name, 'db_locs')
    ->fields('db_locs', ['db_id', 'name', 'count', 'version']);
  $andFilter = $query->andConditionGroup()
    ->condition('db_locs.type', $type, '=')
    ->condition('db_locs.category', $category, '=');
  $results = $query->condition($andFilter)->execute()->fetchAll();

  return $results;
}

/**
 * Get default configuration for submissions.
 *
 * @return array
 *   A mixed array of default configuration settings.
 *   Current returned values:
 *    - defaults_e_value
 *    - defaults_target_coverage
 *    - defaults_query_coverage
 *    - defaults_max_alignments_list
 *    - defaults_max_alignments_selected
 */
function tseq_get_defaults() {
  $db = \Drupal::database();

  $table_name = 'tseq_settings';
  $query = $db->select($table_name, 'db_settings')
    ->fields('db_settings', [
      'defaults_e_value',
      'defaults_target_coverage',
      'defaults_query_coverage',
      'defaults_max_alignments_list',
      'defaults_max_alignments_selected',
    ]
  );
  $results = $query->execute()->fetchAssoc();

  return $results;
}