<?php
require_once dirname(__FILE__) . '/queries.inc';
require_once dirname(__FILE__) . '/../islandora_content_stats.module';
module_load_include('inc','islandora','/includes/utilities');
/**
 * Builds form out of query results from db with latest timestamp.
 * accounts for custom content models
 * to be displayed as a block on homepage, institution page, and collection page
 */
function islandora_content_stats_table_page_form($form, &$form_state){
  $cmodel_map = islandora_get_content_models();
  $variables = [];
  $rows = [];
  $headers =  array(
    array('data' =>t('Collection/Institution'),'field' => 'stats.coll','class' => array('islandora_content_stats_trow_coll'),),
    array('data'=> t('Cmodel'),'field' =>'stats.id','class' => array('islandora_content_stats_trow_model'),),
    array('data'=> t('Count'),'field' =>'stats.query','class' => array('islandora_content_stats_trow_count'),),
  );
  //get latest timestamp from run queries
  $max_time = '';
  $q_time = db_query('select MAX(timestamp) from ldl.islandora_content_stats');
  foreach($q_time as $return){
    foreach($return as $key => $value){
        $max_time = $value;
    }
  }
  //check path to populate queries according to path
  if(strpos(request_uri(), 'collection')){
    $counts = islandora_content_stats_post_collection_stats();
  }
  elseif(request_uri() == '/'){
    $counts = islandora_root_page_stats();
  }
  else{
    $counts = islandora_content_stats_post_institution_stats();
  }
  if($counts){
    foreach($counts as $id => $count){
      $id_part =explode(' ', $id);
      if(count($id_part) == 3){
        $rows[$id] = array(
          'collection' => array(
            'data' => $id_part[1],
            'class' => array('content_stats_origin'),
          ),
          'model' => array(
            'data' => $cmodel_map[$id_part[2]]['label'],
            'class' => array('content_stats_model'),
          ),

          'count' => array(
            'data' => $count,
            'class' => array('content_stats_count',),
          ),
        );
      }
      elseif(count($id_part == 1)){
        $rows[$id] = array(
          'collection' => array(
            'data' => $id_part[0],
            'class' => array('content_stats_origin'),
          ),
          'model' => array(
            'data' => $cmodel_map[$id_part[1]]['label'],
            'class' => array('content_stats_model'),
          ),
          'count' => array(
            'data' => $count,
            'class' => array ('content_stats_count'),
          ),
        );
      }
      else{
        $rows[$id] = array(
          'collection' => array(
            'data' => $id_part[0],
            'class' => array('content_stats_origin'),
          ),
          'model' => array(
            'data' => $cmodel_map[$id_part[1]]['label'],
            'class' => array('content_stats_model'),
          ),
          'count' => array(
            'data' => $count,
            'class' => array('content_stats_count'),
          ),
        );
      }
    }
  }
  $form = [];
  $form['table'] = array(
    '#theme' => 'table',
    '#header' => $headers,
    '#rows' => $rows,
    '#empty' => t('table is empty'),
    '#attributes' => array('class' => array('islandora_content_stats_page_table')),
    );
  return $form;
}


function islandora_content_stats_table_page_form_submit($form, &$form_state){
  $form_state['rebuild'] = TRUE;
}
