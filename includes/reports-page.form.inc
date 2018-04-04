<?php
require_once dirname(__FILE__) . '/queries.inc';
require_once dirname(__FILE__) . '/../islandora_content_stats.module';

/**
 * Builds form out of query results from db with latest timestamp.
 * accounts for custom content models
 */
function islandora_content_stats_table_page_form($form, &$form_state){
  $variables = [];
  $rows = [];
  $headers = array(
    array('data'=> t('Cmodel'),'field' =>'stats.id','sort' => 'desc'),
    array('data'=> t('Count'),'field' =>'stats.query',),
  );
  //get latest timestamp from run queries
  $max_time = '';
  $q_time = db_query('select MAX(timestamp) from ldl.islandora_content_stats');
  foreach($q_time as $return){
    foreach($return as $key => $value){
        $max_time = $value;
    }
  }

  if(strpos(request_uri(), 'collection')){
    $counts = islandora_content_stats_post_collection_stats();
  }
  else{
    $counts = islandora_content_stats_post_institution_stats();
  }

  if($counts){
  foreach($counts as $id => $count){
    $id_part =explode('islandora:', $id);
    $rows[$id] = array(
      'model' => $id_part[1],
      'count' => $count,
    );
  }
  $form = [];
  $form['table'] = array(
    '#theme' => 'table',
    '#header' => $headers,
    '#rows' => $rows,
    '#empty' => t('table is empty')
  );

  return $form;
}
}

function islandora_content_stats_table_page_form_submit($form, &$form_state){
  $form_state['rebuild'] = TRUE;
}
