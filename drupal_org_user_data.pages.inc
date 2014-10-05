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
    return drupal_render(drupal_get_form('drupal_org_user_data_rebuild_form'));
  }
  // statically cache future calls
  $data = &drupal_static($name);
  if (!isset($data)) {
    $data = _drupal_org_user_project_data($name, $extra);
  }
  // sort by downloads
  $downloads = array();
  foreach ($data as $key => $row) {
    $downloads[$key]  = $row['downloads'];
  }
  array_multisort($downloads, SORT_DESC, SORT_NUMERIC, $data);
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
      if (is_numeric($name)) {
        $link = DRUPAL_ORG_USER_PATH . $name;
      }
      else {
        $link = DRUPAL_ORG_NAME_PATH . $name;
      }
      $output .= l(t('Learn more about @name on d.o.', array('@name' => $name)), $link);
      $header = array("name", "project", "href", "commits", "downloads", "installs");
      // create the overview row at the footer
      $overview = array('Totals', 0, '');
      foreach ($data as $row) {
        $overview[1]++;
        $overview['commits'] += $row['commits'];
        $overview['downloads'] += $row['downloads'];
        $overview['installs'] += $row['installs'];
      }
      // better styling for the output for readability
      $data[] = $overview;
      foreach ($data as $key => $row) {
        $data[$key]['commits'] = array('data' => number_format($row['commits']), 'style' => 'text-align: right;');
        $data[$key]['downloads'] = array('data' => number_format($row['downloads']), 'style' => 'text-align: right;');
        $data[$key]['installs'] = array('data' => number_format($row['installs']), 'style' => 'text-align: right;');
      }
      $data[count($data)-1] = array('data' => $data[count($data)-1], 'style' => 'font-weight:bold;');
      $output .= theme('table', array('header' => $header, 'rows' => $data, 'caption' => t('%name\'s Drupal.org user project data per project', array('%name' => $name))));
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
    '#title' => t('d.o. user'),
    '#description' => t('This will batch request data for this user. It supports both user names and user numbers.'),
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
  // hit d.o. for data
  if (is_numeric($name)) {
    $response = _drupal_org_user_data_request(DRUPAL_ORG_USER_PATH . $name);
  }
  else {
    $response = _drupal_org_user_data_request(DRUPAL_ORG_NAME_PATH . $name);
  }
  if (isset($response->error)) {
    form_set_error('name', t('Drupal.org username does not exist!'));
  }
  else {
    // user stored rendering and cached project array so it rebuilds
    if (is_numeric($name)) {
      cache_clear_all(DRUPAL_ORG_USER_PATH . $name, 'cache_drupal_org_user_data');
    }
    else {
      cache_clear_all(DRUPAL_ORG_NAME_PATH . $name, 'cache_drupal_org_user_data');
    }
    cache_clear_all($name . '_project_array', 'cache_drupal_org_user_data');
    $form_state['redirect'] = 'drupal_org_user_data/' . $name;
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
