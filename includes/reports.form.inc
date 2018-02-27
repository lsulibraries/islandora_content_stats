<?php
require_once dirname(__FILE__) . '/queries.inc';
/**
 * Builds form out of query results from db.
 * accounts for custom content models
 */
function islandora_content_stats_reports_form($form, &$form_state) {
  $latest_query = db_query("SELECT * FROM ldl.islandora_content_stats  ORDER by timestamp DESC");
  foreach($latest_query as $record){
    $form[$record->id] = array(
             '#type' => 'fieldset',
             '#value' =>"Items: " . $record->count . " Query increment: " . $record->id ,
             '#title' => "Number of items with '" . $record->query . "' content model",
             '#description' => "Date:  ". gmdate("Y-m-d\TH:i:s\Z",$record->timestamp), 
            );
  }
//  $form['table'] = islandora_content_stats_theme_table();
  return $form;
}
 
//trying to do same above but with a form submit. 
function islandora_content_stats_table_form($form, &$form_state){
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
    $rows[$record->id] = array(
      'id' => $record->id,
      'query' => $record->query,
      'count' => $record->count,
      'timestamp' => gmdate("Y-m-d\TH:i:s\Z",$record->timestamp)
    );
  }
  $form =[];
  $form['filter'] = array(
    '#type' => 'fieldset',
    '#title' => t('Filter Query Results'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );
  $form['filter']['institution'] = array(
    '#type' => 'textfield',
    '#title' => t('Institution'),
    '#size' => 15,
    '#description' => t('institution or sub-institution prefix (.i.e. "lsu")')
  );
  $form['filter']['cmodel'] = array(
    '#type' => 'textfield',
    '#title' => t('Content Model'),
    '#size' => 15,
    '#description' => t('content model string/sub-string (.i.e. "large_image" or "FedoraObject")')
  );
  $form['filter']['collection'] = array(
    '#type' => 'textfield',
    '#title' => t('Collection PID'),
    '#size' => 15,
       '#description' => t('PID string/sub-string (.i.e. "dolls:collection" or "lsu-hpl")')
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
  return $form;
}

function islandora_content_stats_table_form_submit($form, &$form_state){
    //rebuild is totes required
    $form_state['rebuild'] = TRUE;
}