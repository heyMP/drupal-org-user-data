<?php

/**
 * Implements hook_drupal_org_user_projects_alter().
 * @param  array  &$data array of project data from their d.o. user page
 * @param  string $name  username we have requested data for
 */
function hook_drupal_org_user_projects_alter(&$data, $name) {
  // remove my personal projects from the elmsln ones
  if ($name == 'btopro') {
    $personal = array(
      'nittany',
      'edudu_kickstart',
      'lar',
      'personal_photo_share',
      'learning_registry',
      'rlab',
      'geoip_redirect',
      'course_manager',
      'no_devel',
    );
    foreach ($personal as $project) {
      unset($data[$project]);
    }
  }
}
