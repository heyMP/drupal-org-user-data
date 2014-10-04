<?php

/**
 * Endpoint for drupal.org user data that's been pulled through site
 * @param  string $name   user name on d.o. to look up
 * @param  string $format the format to output to the user, null = theme
 * @return data           data in whatever format was just requested
 */
function drupal_org_user_data_endpoint($name = NULL, $format = NULL) {
  $extra = array();
  $data = _drupal_org_user_project_data($name, $extra);
  /*
  // display d.o. aggregate json data
  // @ignore production_code
  if (module_exists('devel') && $GLOBALS['user']->uid == 1) {
    // @ignore production_code
    dpm($data);
  // @ignore production_code
  }
  */
  // display it in the format requested
  switch ($format) {
    case 'xml':
    case 'yaml':
    case 'json':
      return _drupal_org_user_data_encode($data, $format);
    break;
    case NULL:
      $header = array("name", "project", "href", "commits", "downloads", "installs");
      $output = theme('table', array('header' => $header, 'rows' => $data));
      $output .= _drupal_org_user_data_overview($data, $extra);
    break;
  }

  return $output;
}
