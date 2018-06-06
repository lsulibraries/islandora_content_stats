<?php

require_once dirname(__FILE__) . '/queries.inc';

/**
 * @file
 * Admin form and submission handler.
 */
function islandora_content_stats_admin_form($form, &$form_state) {

  $form['when']['date'] = array(
    '#title' => 'Day of the month',
    '#type' => 'select',
    '#options' => array_combine(range(1,31),range(1,31)),
  );
  // $days = array('Monday','Tuesday','Wednesday','Thursday','Friday','Saturday','Sunday');
  //  $form['when']['day'] = array(
  //   '#title' => 'Day',
  //   '#type' => 'select',
  //   '#options' => array_combine($days,$days),
  //   '#empty_option' => '--none--',
  //  );
   $form['when']['hour'] = array(
    '#title' => 'on the hour of',
    '#type' => 'select',
    '#options' => array_combine(range(0,23), range(0,23)),
   );
   $date = variable_get('islandora_content_stats_query_run_date');
   $date = (isset($date) ? $date : 'not set' );
   $hour =variable_get('islandora_content_stats_query_run_hour');
   $hour = (isset($hour) ? $hour : 'not set' );
    $form['current_setting'] = array(
    '#title' => t('Currently set to run'),
    '#type' => 'fieldset',
    '#description' => t('when cron runs and the day of the month is: '.$date.' The hour is: '.$hour),
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

 //if an empty texfield is submitted it defaults to a resonable interval.
 if(!variable_get('islandora_content_stats_interval')){
     variable_set('islandora_content_stats_interval', '3 days');
 }


function direct_to_reports(){
  drupal_goto('data');
}

/**
 * Form submission handler.
 */
function islandora_content_stats_admin_form_submit($form, &$form_state){
  variable_set('islandora_content_stats_query_run_date',$form_state['input']['date']);
  //variable_set('islandora_content_stats_query_run_day',$form_state['input']['day']);
  variable_set('islandora_content_stats_query_run_hour', $form_state['input']['hour']);
}
