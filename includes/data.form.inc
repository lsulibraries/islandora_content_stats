<?php

/**
 * @file
 * when queries are run manually, return to this page
 * this page serves as a place to filter and export queries stored in the mysql
 * with a more humant readable format, and links to collection items.
 */

function islandora_content_stats_data_filter_form($form, &$form_state) {
    // important to note that this pulls $_GET by default https://api.drupal.org/api/drupal/includes%21common.inc/function/drupal_get_query_parameters/7.x
    // forms usually get POST-ed
    $gets = pageDefaultParams(islandora_content_stats_get_query_params());
    module_load_include('inc', 'islandora_content_stats', 'includes/utilites');
    module_load_include('inc', 'islandora_content_stats', 'includes/language');
    $form = array();
    $form['filter'] = array(
        '#type' => 'fieldset',
        '#title' => stats_lang('filter_fieldset_#title'),
        '#attributes' => array('class' => array('ics_filters')),
    );
    $cmodel = array_key_exists('cmodel', $gets) ? $gets['cmodel'] : NULL;
    $form['filter']['inst'] = array(
        '#type' => 'select',
        '#title' => stats_lang('filterInst'),
        '#options' => institutions_for_form(),
        '#required' => FALSE,
        '#default_value' => array_key_exists('inst', $gets) ? $gets['inst'] : '--none--',
        '#empty_option' => '--none--',
        '#attributes' => array('class' => array('ics_filter_inst')),
    );
    $form['filter']['cmodel'] = array(
        '#type' => 'select',
        '#title' => stats_lang('filterCmodel'),
        '#options' => cmodel_types_for_form(),
        '#required' => FALSE,
        '#default_value' => $cmodel,
        '#empty_option' => '--none--',
        '#attributes' => array('class' => array('ics_filter_cmodel')),
    );
    $form['filter']['sort'] = array(
        '#type' => 'hidden',
        '#default_value' => $gets['sort'],
    );
    $form['filter']['order'] = array(
        '#type' => 'hidden',
        '#default_value' => $gets['order'],
    );
    $form['filter']['filterdesc'] = array(
        '#type' => 'item',
        '#title' => stats_lang('filterSubmitDesc'),
        '#attributes' => array('class' => array('ics_filter_submit_description')),
    );
    $form['filter']['submit'] = array(
        '#type' => 'submit',
        '#value' => stats_lang('filterSubmit'),
        '#attributes' => array('class' => array('ics_filter_submit')),
    );
    return $form;
}

/**
 * Download function
 * applies filters to queries and returns as csv file
 */
function islandora_content_stats_csv_download() {
  module_load_include('inc','islandora_content_stats','includes/utilities');
  $max_time = latest_run();
  $query = 'SELECT * FROM {islandora_content_stats} WHERE timestamp = :max_time';
  $qargs = [':max_time' => $max_time];
  foreach (pageDefaultParams(islandora_content_stats_get_query_params()) as $key => $filter) {
    if(in_array($key, ['cmodel', 'inst', 'coll']) && $filter) {
      $query .= " AND $key = :$key";
      $qargs[$key] = $key == 'inst' ? "$filter-" : $filter;
    }
  }

  $csv = [implode(',', array('QID', 'Institution', 'Cmodel', 'Count', 'Timestamp', 'URL'))];

  $results = db_query($query, $qargs);
  foreach ($results as $result) {
    $rootcoll = variable_get('islandora_repository_pid', 'islandora:root');
    $ignored_cmodel = in_array($result->cmodel, ['fedora-system:FedoraObject-3.0', 'fedora-system:ContentModel-3.0']);
    if(!$ignored_cmodel && $result->coll != $rootcoll) {

      $mapped_results = array(
        $result->id,
        map_institution($result->inst),
        $result->cmodel,
        $result->count,
        strftime('%d:%m:%Y:%r', $result->timestamp),
      );
      $csv[] = implode(',', $mapped_results);
    }
  }
  $csv2string = implode("\n", $csv);
  $filename = file_unmanaged_save_data($csv2string, "temporary://data.csv", FILE_EXISTS_RENAME);
  $headers = [
    'Content-type' => 'text/csv; utf-8',
    'Content-Dispositon' => 'attachment; filename=islandora_content_stats.csv'
  ];
  file_transfer($filename, $headers);
}
