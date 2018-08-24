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
        '#title' => lang('filter_fieldset_#title'),
        '#attributes' => array('class' => array('ics_filters')),
    );
    $ternary = array_key_exists('cmodel', $gets) ? $gets['cmodel'] : NULL;
    $form['filter']['cmodel'] = array(
        '#type' => 'select',
        '#title' => lang('filterCmodel'),
        '#options' => cmodel_types_for_form(),
        '#required' => FALSE,
        '#default_value' => $ternary,
        '#empty_option' => '--none--',
        '#attributes' => array('class' => array('ics_filter_cmodel')),
    );
    $form['filter']['inst'] = array(
        '#type' => 'select',
        '#title' => lang('filterInst'),
        '#options' => institutions_for_form(),
        '#required' => FALSE,
        '#default_value' => array_key_exists('inst', $gets) ? $gets['inst'] : '--none--',
        '#empty_option' => '--none--',
        '#attributes' => array('class' => array('ics_filter_inst')),
    );
    $form['filter']['filterdesc'] = array(
        '#type' => 'item',
        '#title' => lang('filterSubmitDesc'),
        '#attributes' => array('class' => array('ics_filter_submit_description')),
    );
    $form['filter']['submit'] = array(
        '#type' => 'submit',
        '#value' => lang('filterSubmit'),
        '#attributes' => array('class' => array('ics_filter_submit')),
    );
    $form['download']['dltitle'] = array(
        '#markup' => "<div class='dl_txt'>" . lang('dlSubmitDesc') . "</div>",
        '#attributes' => array('class' => array('ics_download_title')),
    );
    $form['download']['submit'] = array(
        '#type' => 'submit',
        '#value' => lang('dlSubmit'),
        '#submit' => array('islandora_content_stats_csv_download'),
        '#attributes' => array('class' => array('ics_download_button')),
    );
    return $form;
}


/**
 * Download function
 * applies filters to queries and returns as csv file
 */
function islandora_content_stats_csv_download() {
  module_load_include('inc','islandora_content_stats','includes/utilities');
  $query = 'SELECT * FROM {islandora_content_stats}';
  $filters = array();
   foreach ($_GET as $key => $filter) {
   if($key === 'cmodel' || $key === 'inst' || $key == 'coll') {
           $filters[$key] = $filter;
       }
   }
  $max_time = '';
  $q_time = db_query('select MAX(timestamp) from {islandora_content_stats}');
  $max_time = $q_time->fetchField();
  $query .= ' WHERE timestamp = \'' . $max_time . '\'';
  foreach ($filters as $key => $filter) {
    if ($filter != '') {
       $query .= ' AND ' . $key . ' = \'' . $filter . '\'';
    }
  }
  $csv = [];
  $results = db_query($query) or die('Sql error : ' . mysql_error()); // why is `die()` in this code ? watchdog or similar would be better.
  $csv[] = implode(',', array('QID', 'Institution', 'Collection', 'Cmodel', 'Count', 'Timestamp', 'URL'));
  foreach ($results as $result) {
      $not_fobj = $result->cmodel != 'fedora-system:FedoraObject-3.0';
      $not_cmobj = $result->cmodel != 'fedora-system:ContentModel-3.0';
      $not_rootcoll = $result->coll != 'islandora:root';
      if (($not_fobj) && ($not_cmobj) && ($not_rootcoll)) {
          $mapped_results = array(
              $result->id,
              map_institution($result->inst),
              map_collection($result->coll),
              $result->cmodel,
              $result->count,
              strftime('%d:%m:%Y:%r', $result->timestamp),
              map_collection($result->coll, 'csv')
          );
          $csv[] = implode(',', $mapped_results);
      }
  }
  $csv = implode("\n", $csv);
  $filename = file_unmanaged_save_data($csv, "temporary://" . 'data.csv', FILE_EXISTS_RENAME);
  $headers = [
    'Content-type' => 'text/csv; utf-8',
    'Content-Dispositon' => 'attachment; filename=islandora_content_stats.csv'
  ];
  file_transfer($filename, $headers);
}
