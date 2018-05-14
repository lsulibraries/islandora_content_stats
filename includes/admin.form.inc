<?php

require_once dirname(__FILE__) . '/queries.inc';

/**
 * @file
 * Admin form and submission handler.
 */
function islandora_content_stats_admin_form($form, &$form_state) {
   $form['interval'] = array(
    '#type' => 'textfield',
    '#title' => t('Update Interval'),
    '#description' => t('Inteval of time to update database w/ new queries statsistics.'),
  );
    $form['current_interval'] = array(
    '#title' => t('Current Interval'),
    '#type' => 'fieldset',
    '#description' => variable_get('islandora_content_stats_interval')
    );
    $form['run'] = array(
    '#type' => 'submit',
    '#value' => t('Run Query Now'),
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
