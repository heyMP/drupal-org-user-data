<?php

/**
 * @file
 * Drush integration for drupal_org_user_data.
 */


/**
 * Implements hook_drush_cache_clear().
 */
function drupal_org_user_data_drush_cache_clear(&$types) {
  $types['drupal_org_user_data'] = 'drupal_org_invalidate_cache';
}


/**
 * Implements hook_drush_command().
 */
function drupal_org_user_data_drush_command() {
  $items = array();
  $items['drupal-org-load-user-data'] = array(
    'description' => dt('Spider a user profile and save project data'),
    'arguments' => array(
      'name' => dt('drupal.org username to pull data for.'),
    ),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_FULL,
    'aliases' => array('doud'),
    'examples' => array(
      'drush doud btopro' =>
        'Load all projects btopro has commits to and pull in other data, 5 projects at a time in batch til they are all done.',
    ),
  );
  return $items;
}

/**
 * Implements hook_drush_help().
 */
function drupal_org_user_data_drush_help($section) {
  switch ($section) {
    case 'drush:drupal-org-load-user-data':
      return dt('Pull data from drupal.org about a user');
  }
}

/**
 * Load remote data from drupal.org about a user's project contributions
 * @param $name drupal.org user name to request data for
 */
function drush_drupal_org_user_data_drupal_org_load_user_data($name) {
  $start = time();
  if (empty($name)) {
    drush_log(dt('Drupal.org user name required'), 'error');
    return FALSE;
  }
  // clear the cache bins for these two values so they are rebuilt
  if (is_numeric($name)) {
    cache_clear_all(DRUPAL_ORG_USER_PATH . $name, 'cache_drupal_org_user_data');
  }
  else {
    cache_clear_all(DRUPAL_ORG_NAME_PATH . $name, 'cache_drupal_org_user_data');
  }
  cache_clear_all($name . '_project_array', 'cache_drupal_org_user_data');
  // Lets try things in a batch.
  _drupal_org_load_user_data_batch($name);
  $batch =& batch_get();
  $batch['progressive'] = FALSE;
  drush_backend_batch_process();
}
