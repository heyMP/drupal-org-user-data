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
    'options' => array(
      'rebuild' => dt('flushes current cache bins for drupal.org data'),
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
  // allow for forcibly rebuilding all cached drupal.org user data
  if (drush_get_option('rebuild')) {
    drupal_org_invalidate_cache();
  }
  // Lets try things in a batch.
  _drupal_org_load_user_data_batch($name);
  $batch =& batch_get();
  $batch['progressive'] = FALSE;
  drush_backend_batch_process();
}
