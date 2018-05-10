<?php
require_once dirname(__FILE__) . '/queries.inc';
require_once dirname(__FILE__) . '/utilities.inc';

/**
 * Builds form out of query of all results from db.
 * accounts for custom content models
 */
function islandora_content_stats_table_all_form($form, &$form_state){
  $variables = [];
  $rows = [];
  $headers = array(
    array('data'=> t('ID'),'field' =>'stats.id','sort' => 'desc'),
    array('data'=> t('Query'),'field' =>'stats.query',),
    array('data'=> t('Count'),'field' =>'stats.count',),
    array('data'=> t('Timestamp'),'field' =>'stats.timestamp',),
  );
  //second argument in db select is an alias for the table(which is the first argument.)
  $query = db_select("islandora_content_stats","stats")
    ->extend('TableSort');

  if(!empty($form_state['input'])){
    //somehow get a variable from a textfield with a submit into the db_like function...
    $results = $query->fields('stats',array('id','query','count','timestamp'))
      ->orderByHeader($headers)
      ->condition('query', '%'. db_like($form_state['input']['institution']). '%', 'LIKE')
      ->condition('query', '%'. db_like($form_state['input']['cmodel']). '%', 'LIKE')
      ->condition('query', '%'. db_like($form_state['input']['collection']). '%', 'LIKE')
      ->extend('TableSort')->extend('PagerDefault')->limit(144)
      ->execute();
  }
  else{
    $results = $query->fields('stats',array('id','query','count','timestamp'))
      ->orderByHeader($headers)
      ->extend('TableSort')->extend('PagerDefault')->limit(144)
      ->execute();
  }
  foreach($results as $record){
    if($record->count != 0){
      if(strpos($record->query,'fedora-system:FedoraObject-3.0') || strpos($record->query,'fedora-system:ContentModel-3.0')){}
        else{
          $label = map_islandora_cmodels($record->query);
          $rows[$record->id] = array(
            'id' => $record->id,
            'query' => $label,
            'count' => $record->count,
            'timestamp' => gmdate("Y-m-d\TH:i:s\Z",$record->timestamp)
          );
      }
    }
  }
  $form = [];
  $form['filter'] = array(
    '#type' => 'fieldset',
    '#title' => t('Filter Query Results'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );
  $form['filter']['cmodel'] = array(
    '#type' => 'select',
    '#title' => t('Filter by type of object'),
    '#options' => cmodel_types_for_form(),
    '#description' => t('content model names'),
    '#required' => FALSE,
    '#empty_option' => '--none--',
  );
  $form['filter']['institution'] = array(
    '#type' => 'select',
    '#title' => t('Filter by institution or sub-institution'),
    '#options' => institutions_for_form(),
    '#required' => FALSE,
    '#description' => t('institution or sub-institution prefix (.i.e. "lsu")'),
    '#empty_option' => '--none--',
  );
  $form['filter']['collection'] = array(
    '#type' => 'select',
    '#title' => t('Filter by specific Collection'),
    '#options' => collection_pid_labels_for_form(),
    '#required' => FALSE,
    '#empty_option' => '--none--',
  );
  // $form['filter']['institution'] = array(
  //   '#type' => 'textfield',
  //   '#title' => t('Institution'),
  //   '#size' => 15,
  //   '#description' => t('institution or sub-institution prefix (.i.e. "lsu")')
  // );
  // $form['filter']['cmodel'] = array(
  //   '#type' => 'textfield',
  //   '#title' => t('Content Model'),
  //   '#size' => 15,
  //   '#description' => t('content model string/sub-string (.i.e. "large_image" or "FedoraObject")')
  // );
  // $form['filter']['collection'] = array(
  //   '#type' => 'textfield',
  //   '#title' => t('Collection PID'),
  //   '#size' => 15,
  //      '#description' => t('PID string/sub-string (.i.e. "dolls:collection" or "lsu-hpl")')
  // );
  $form['filter']['download'] = array(
    '#type' => 'submit',
    '#value' => t('Download as CSV'),
    '#submit' => array('download_data_all'),
  );
  $form['filter']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Filter'),
  );
  $form['table'] = array(
    '#theme' => 'table',
    '#header' => $headers,
    '#rows' => $rows,
    '#empty' => t('table is empty')
  );
  $form['pager'] = array('#markup' => theme('pager'));
  $form['show_latest'] = array (
    '#type' => 'submit',
    '#value' => t('Show Most Recent Queries'),
    '#submit' => array('go_to_recent')
  );
  return $form;
}

function download_data_all(&$form,&$form_state){
  $options = array('query' => array('inst' => $form_state['values']['institution'],'cmodel' => $form_state['values']['cmodel'], 'coll' => $form_state['values']['collection'], 'all' => 'all'));
  drupal_goto('/admin/reports/content_stats/download', $options);
}

function go_to_recent(){
  drupal_goto('/admin/reports/content_stats');
}

function islandora_content_stats_table_all_form_submit($form, &$form_state){
  $form_state['rebuild'] = TRUE;
}
