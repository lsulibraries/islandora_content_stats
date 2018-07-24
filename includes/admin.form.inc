<?php

/**
 * @file
 * Can be found:
 * Admin -> Islandora -> Islandora Utility Modules -> Content Statistics
 */

/**
 * Admin form and submission handler.
 */

function islandora_content_stats_admin_form($form, &$form_state) {
  module_load_include('inc', 'islandora_content_stats', 'includes/queries');
  $form['when']['date'] = array(
    '#title' => 'Every Nth Days',
    '#type' => 'textfield',
    //'#type' => 'select',
    //'#options' => array_combine(range(1,31),range(1,31)),
    '#default_value' => variable_get('islandora_content_stats_interval'),
  );
  // $days = array('Monday',
  //   'Tuesday',
  //   'Wednesday',
  //   'Thursday',
  //   'Friday',
  //   'Saturday',
  //   'Sunday',
  // );
   $form['when']['off_start'] = array(
    '#title' => 'Begin off-hours',
    '#type' => 'select',
    '#options' => range(0,23),
    '#default_value' => variable_get('islandora_content_stats_off_start',20),
   );
  $form['when']['off_end'] = array(
    '#title' => 'End off-hours',
    '#type' => 'select',
    '#options' => range(0,23),
    '#default_value' =>  variable_get('islandora_content_stats_off_end',23),
  );
  $run_interval = variable_get('islandora_content_stats_interval');
  $run_interval = (isset($run_interval) ? $run_interval : 'not set' );
//  $hour =variable_get('islandora_content_stats_query_run_hour');
//  $hour = (isset($hour) ? $hour : 'not set' );
  $descr_str = "queries update every $run_interval days";
  $form['current_setting'] = array(
    '#title' => t('Currently setting'),
    '#type' => 'fieldset',
    '#description' => t($descr_str),
  );

  $form['language'] = array(
    '#type' => 'fieldset',
    '#title' => 'Language strings',
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    // https://drupal.stackexchange.com/questions/89581/submitting-fieldsets
    '#tree' => TRUE,
  );
  module_load_include('inc', 'islandora_content_stats', 'includes/language');
  $strings = islandora_content_stats_language();
  foreach ($strings as $key => $str) {
    $form['language'][$key] = [
      '#title' => $str['title'],
      '#default_value' => variable_get("islandora_content_stats_language_$key", $str['default']),
      '#type' => isset($str['type']) ? $str['type'] : 'textfield',
    ];
  }
  $form['run'] = array(
    '#type' => 'submit',
    '#value' => t('Run Queries Now'),
    '#description' => t('press to populate database. (must press)'),
    '#submit' => array(
      'islandora_content_stats_run_queries',
      'direct_to_reports'),
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit'),
  );
  return $form;
}



/**
 *redirects to data.form page example.com/data
 */

function direct_to_reports() {
  drupal_goto('data');
}

/**
 * Form submission handler.
 */

function islandora_content_stats_admin_form_submit($form, &$form_state) {
  $vset_str = 'islandora_content_stats_interval';
  variable_set($vset_str , $form_state['values']['date']);
  variable_set('islandora_content_stats_off_start', $form_state['values']['off_start']);
  variable_set('islandora_content_stats_off_end', $form_state['values']['off_end']);
  foreach ($form_state['values']['language'] as $key => $langStr) {
    $prefix = 'islandora_content_stats_language_';
    variable_set($prefix . $key, $langStr);
  }
}
