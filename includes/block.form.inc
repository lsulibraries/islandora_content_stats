<?php

/**
 * @file
 * This block displays contextual content model totals for either
 * islandora:root, or an individual collection.
 * Block can be enabled on islandora_content_stats.module
 * Change 'status' => 0, to 'status' => 1,
 */

/**
 * create row for table displayed
 * @param string $cmodel
 * content model machine name
 * @param string $count
 * count of items with cmodel
 */

function getRow($cmodel, $count) {
  module_load_include('inc', 'islandora', '/includes/utilities');
  if ($count != 0) {
    return array(
       'model' => array(
        'data' => map_cmodels($cmodel),
        'class' => array('content_stats_model'),
       ),
       'count' => array(
        'data' => $count,
        'class' => array('content_stats_count'),
       ),
    );
  }
}

/**
 * builds rows for display in a table
 * @param array $counts
 * return from a query, array of content model counts
 */

function getRow_helper($counts) {
  $rows = array();
  if (isset($counts)) {
    foreach($counts as $id => $fields) {
      $rows[$id] = getRow($fields['cmodel'], $fields['count']);
    }
  }
  return $rows;
}


/**
 * Builds form out of query results from db with latest timestamp.
 * accounts for custom content models
 * to be displayed as a block on homepage, institution page, and collection page
 * not currently implemented by default
 */

function islandora_content_stats_mini_form($form, &$form_state) {
  module_load_include('inc','islandora','includes/utilities');
  module_load_include('inc','islandora_content_stats','includes/utilities');
  module_load_include('inc', 'islandora_content_stats', 'includes/queries');
  $cmodel_map = islandora_get_content_models();
  $variables = array();
  //get latest timestamp from run queries
  $q_time = db_query('select MAX(timestamp) from ldl.islandora_content_stats');
  $max_time = $q_time->fetchField();
  $all_inst = all_ns_prefix_incl_subs();
  $uri_split = explode('/', request_uri());
  //find the context of the page: is it front, institution or collection?
  if (strpos(request_uri(), 'collection')) {
    $counts = islandora_content_stats_post_collection_stats();
    $rows = getRow_helper($counts);
  }
  elseif ( drupal_is_front_page() ) {
    $counts = islandora_root_stats_queries();
    $rows = getRow_helper($counts);
  }
  else {
    $counts = islandora_content_stats_post_institution_stats();
    $rows = getRow_helper($counts);
  }
  //build up the form
  $form = array();
    $headers =  array(
      array(
        'data' => t('Cmodel'),
        'field' => 'stats.id',
        'class' => array('islandora_content_stats_trow_model'),
      ),
      array(
        'data' => t('Count'),
        'field' => 'stats.query',
        'class' => array('islandora_content_stats_trow_count'),
      ),
    );

    $form['table'] = array(
      '#theme' => 'table',
      '#header' => $headers,
      '#rows' => $rows,
      '#empty' => t('table is empty'),
      '#attributes' => array(
        'class' => array('islandora_content_stats_block_table'),
      ),
    );
  return $form;
}

/**
 * implements hook_form_submit().
 */

function islandora_content_stats_mini_form_submit($form, &$form_state) {
  $form_state['rebuild'] = TRUE;
}

/**
 * if uri has 'collection'
 * @return array
 * of latest content model total for the collection
 */

function islandora_content_stats_post_institution_stats() {
  module_load_include('inc','islandora_content_stats','includes/queries');
  if ( check_path() ) {
    $inst_prefix = str_replace('/', '', request_uri());
    return islandora_content_stats_block_inst($inst_prefix.'-');
  }
  else {
    return;
  }
}

/**
 * checks path against each cached namespace value.
 */

function check_path() {
  module_load_include('php', 'islandora_namespace_homepage', 'includes/utilities');
  $ns_prefixes = update_namespace_prefixes_cache();
  $page = str_replace('/','',request_uri());
    foreach($ns_prefixes as $prefix) {
      if ($prefix == $page) {
        return true;
      }
    }
}

/**
 * @return array
 * array of collection cmodel totals for block
 */
function islandora_content_stats_post_collection_stats() {
  if (strpos(request_uri(),'collection')) {
    $str = request_uri();
    $coll = str_replace('/islandora/object/','', $str);
    return islandora_content_stats_block_coll($coll);
  }
}
