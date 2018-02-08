<?php

/**
 * @file
 * Admin form and submission handler.
 */

function islandora_content_stats_admin_form($form, &$form_state) {
   $form['duration'] = array(
    '#type' => 'textfield',
    '#value' => t('1'),
    '#title' => t('Update Interval'),
    '#description' => t('Inteval of time to update database w/ new queries statsistics.'),
  );
    $form['last'] = array(
    '#title' => t('Last Cron job'),
    '#type' => 'textfield',
    '#description' => t('latest runtime of cron.'),
    '#default_value' => gmdate("Y-m-d\TH:i:s\Z", variable_get('cron_last')),
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
function islandora_content_stats_admin_form_validate(&$form, &$form_state) {
}

/**
 * Form submission handler.
 */
function islandora_content_stats_admin_form_submit(&$form, &$form_state) {
}
