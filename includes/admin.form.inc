<?php

require_once dirname(__FILE__) . '/queries.inc';


/**
 * @file
 * Admin form and submission handler.
 */
function islandora_content_stats_admin_form($form, &$form_state) {
   $form['duration'] = array(
    '#type' => 'textfield',
    '#title' => t('Update Interval'),
    '#description' => t('Inteval of time to update database w/ new queries statsistics.'),
  );
    $form['last'] = array(
    '#title' => t('Current Interval'),
    '#type' => 'fieldset',
    '#description' => variable_get('islandora_content_stats_interval')
    );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit'),
  );
  $form['now'] = array(
  '#type' => 'button',
  '#value' => t('Run Query Now'),
  '#process' => array('islandora_content_stats_test'),
);


  return $form;
}

/**
 * Form submission handler.
 */
function islandora_content_stats_admin_form_submit($form, &$form_state) {
    variable_set('islandora_content_stats_interval', $form_state['input']['duration']);
}
