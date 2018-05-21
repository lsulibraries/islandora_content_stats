<?php

require_once dirname(__FILE__) . '/queries.inc';

/**
 * @file
 * Admin form and submission handler.
 */
function islandora_content_stats_admin_form($form, &$form_state) {
   $form['interval'] = array(
    '#type' => 'textfield',
    '#title' => t('Update Interval: can be set to "1 WEEK" or "2 MONTHS" etc. Defaults to 3 days '),
    '#description' => t('Interval of time to update database after. only checks time after cron has run.'),
  );
    $form['current_interval'] = array(
    '#title' => t('Current Interval'),
    '#type' => 'fieldset',
    '#description' => variable_get('islandora_content_stats_interval')
    );
    $form['run'] = array(
    '#type' => 'submit',
    '#value' => t('Run Queries Now'),
    '#description' => t('press to populate database. (must press)'),
    '#submit' => array('islandora_content_stats_run_queries', 'direct_to_reports'),
    );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit'),
  );
  return $form;
}

function direct_to_reports(){
  drupal_goto('admin/reports/content_stats');
}

/**
 * Form submission handler.
 */
function islandora_content_stats_admin_form_submit($form, &$form_state) {
    variable_set('islandora_content_stats_interval', $form_state['input']['interval']);
}
