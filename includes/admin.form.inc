<?php

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
    '#title' => t('Latest runtime of cron.'),
    '#type' => 'fieldset',
    '#description' => gmdate("Y-m-d\TH:i:s\Z", variable_get('cron_last')),
    );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit'),
  );
  
  return $form;
}

/**
 * Form submission handler.
 */
function islandora_content_stats_admin_form_submit($form, &$form_state) {
    variable_set('islandora_content_stats_interval', $form_state['input']['duration']);
}