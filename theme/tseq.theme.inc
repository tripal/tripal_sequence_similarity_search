<?php
/*
 * Implements Theme Hook
 */
function tripal_seq_theme_registry_alter(&$theme_registry) {
  $theme_registry_copy = $theme_registry;
  $module_path = drupal_get_path('module', 'tripal_seq') . '/theme';
  _theme_process_registry($theme_registry_copy, 'phptemplate', 'theme_engine', 'my_custom_theme', $module_path);
  $theme_registry += array_diff_key($theme_registry_copy, $theme_registry);
  // A list of templates the module will provide templates for
  $hooks = array('page');
  foreach ($hooks as $hook) {
    // Add the key 'theme paths' if it doesn't exist in this theme's registry
    if (!isset($theme_registry[$hook]['theme paths'])) {
      $theme_registry[$hook]['theme paths'] = array();
    }
    // Shift this module's directory to the top of the theme path list
    if (is_array($theme_registry[$hook]['theme paths'])) {
      $first_element = array_shift($theme_registry[$hook]['theme paths']);
      if ($first_element) {
        array_unshift($theme_registry[$hook]['theme paths'], $first_element, $module_path);
      }
      else {
        array_unshift($theme_registry[$hook]['theme paths'], $module_path);
      }
    }
  }
}


