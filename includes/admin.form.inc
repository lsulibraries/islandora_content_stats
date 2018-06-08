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
    '#title' => 'Day of the month',
    '#type' => 'select',
    '#options' => array_combine(range(1,31),range(1,31)),
  );
  // $days = array('Monday',
  //   'Tuesday',
  //   'Wednesday',
  //   'Thursday',
  //   'Friday',
  //   'Saturday',
  //   'Sunday',
  // );
  //  $form['when']['day'] = array(
  //   '#title' => 'Day',
  //   '#type' => 'select',
  //   '#options' => array_combine($days, $days),
  //   '#empty_option' => '--none--',
  //  );
  $form['when']['hour'] = array(
    '#title' => 'on the hour of',
    '#type' => 'select',
    '#options' => array_combine(range(0, 23), range(0, 23)),
  );
  $date = variable_get('islandora_content_stats_query_run_date');
  $date = (isset($date) ? $date : 'not set' );
  $hour =variable_get('islandora_content_stats_query_run_hour');
  $hour = (isset($hour) ? $hour : 'not set' );
  $descr_str = 'when cron runs and the day of the month is: ';
  $form['current_setting'] = array(
    '#title' => t('Currently set to run'),
    '#type' => 'fieldset',
    '#description' => t($descr_str . $date . ' The hour is: ' . $hour),
  );
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
  $vset_str = 'islandora_content_stats_query_run';
  variable_set($vset_str . '_date', $form_state['input']['date']);
  //variable_set($vset_str . '_day', $form_state['input']['day']);
  variable_set($vset_st . '_hour', $form_state['input']['hour']);
}
