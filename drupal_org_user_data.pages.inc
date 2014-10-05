<?php

/**
 * Endpoint for drupal.org user data that's been pulled through site
 * @param  string $name   user name on d.o. to look up
 * @param  string $format the format to output to the user, null = theme
 * @return data           data in whatever format was just requested
 */
function drupal_org_user_data_endpoint($name = NULL, $format = NULL) {
  $extra = array();
  if (empty($name)) {
    $name = $GLOBALS['user']->name;
  }
  // statically cache future calls
  $data = &drupal_static($name);
  if (!isset($data)) {
    $data = _drupal_org_user_project_data($name, $extra);
  }
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
      print _drupal_org_user_data_encode($data, $format);
      drupal_exit();
    break;
    case NULL:
      $output = drupal_render(drupal_get_form('drupal_org_user_data_rebuild_form'));
      $header = array("name", "project", "href", "commits", "downloads", "installs");
      $output .= theme('table', array('header' => $header, 'rows' => $data));
      $output .= _drupal_org_user_data_overview($data, $extra);
    break;
  }
  return $output;
}

/**
 * Page to rebuild form manually
 * @return [type] [description]
 */
function drupal_org_user_data_rebuild_form() {
  $form = array();
  $form['username'] = array(
    '#type' => 'textfield',
    '#title' => t('d.o. user name'),
    '#description' => t('This will batch request data for this user'),
    '#required' => TRUE,
  );
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Get data!'),
  );
  return $form;
}

/**
 * Implements hook_form_validate().
 */
function drupal_org_user_data_rebuild_form_validate($form, &$form_state) {
  // hit d.o. for data
  $name = $form_state['values']['username'];
  cache_clear_all(DRUPAL_ORG_USER_PATH . $name, 'cache_drupal_org_user_data');
  $response = _drupal_org_user_data_request(DRUPAL_ORG_USER_PATH . $name);
  $form_state['redirect'] = 'drupal_org_user_data/' . $name;
  if (isset($response->error)) {
    form_set_error('name', t('Drupal.org username does not exist!'));
  }
}

/**
 * Implementation of hook_form_submit().
 */
function drupal_org_user_data_rebuild_form_submit($form, $form_state) {
  /* Your code here */
  $name = $form_state['values']['username'];
  _drupal_org_load_user_data_batch($name);
}
