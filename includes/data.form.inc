<?php

/**
 * @file
 * when queries are run manually, return to this page
 * this page serves as a place to filter and export queries stored in the mysql
 * with a more humant readable format, and links to collection items.
 */


function islandora_content_stats_data_filter_form($form, &$form_state) {
    module_load_include('inc', 'islandora_content_stats', 'include/utilites');
    $form = array();
    //    $form['filter']['collection'] = array(
//        '#type' => 'select',
//        '#title' => t('Filter by specific collection'),
//        '#description' => t('Shows counts of object types
// for an individual collection.'),
//        '#options' => collection_pid_labels_for_form(),
//        '#required' => FALSE,
//        '#empty_option' => '--none--',
//        '#attributes' => array('class' => array('ics_filter_coll')),
//    );
//    $form['filter']['all'] = array(
//        '#type' => 'checkbox',
//        '#title' => t('Show all results (history)'),
//        '#attributes' => array('class' => array('ics_filter_all')),
//
//    );

    $form['filter'] = array(
        '#type' => 'fieldset',
        '#title' => lang('filter_fieldset_#title'),
        '#attributes' => array('class' => array('ics_filters')),
    );
    $form['filter']['cmodel'] = array(
        '#type' => 'select',
        '#title' => t('Filter by type of object'),
        '#options' => cmodel_types_for_form(),
        '#description' => t('Shows results to that match a single content model.'),
        '#required' => FALSE,
        // '#default_value' => variable_get('content_stats_cmodel_filter'),
        '#empty_option' => '--none--',
        '#attributes' => array('class' => array('ics_filter_cmodel')),
    );
    $form['filter']['institution'] = array(
        '#type' => 'select',
        '#title' => t('Filter according to ownership by
 institution or sub-institution'),
        '#options' => institutions_for_form(),
        '#required' => FALSE,
        '#description' => t('Shows results for any items owned
 by chosen institution or sub-institution.'),
        //'#default_value' => variable_get('content_stats_inst_filter'),
        '#empty_option' => '--none--',
        '#attributes' => array('class' => array('ics_filter_inst')),

    );
    $form['filter']['filtertitle'] = array(
        '#type' => 'item',
        '#title' => t('Press to apply filters to page results.'),
        '#attributes' => array('class' => array('ics_filter_submit_description')),
    );
    $form['filter']['submit'] = array(
        '#type' => 'submit',
        '#value' => t('Filter'),
        '#attributes' => array('class' => array('ics_filter_submit')),
    );
    $form['download']['dltitle'] = array(
        '#markup' => "<div class='dl_txt'>" . t('Downloads all historic results for use in a spreadsheet.
    (always applies current filters)') . "</div>",
        '#attributes' => array('class' => array('ics_download_title')),
    );
    $form['download']['submit'] = array(
        '#type' => 'submit',
        '#value' => t('Download History as CSV'),
        '#submit' => array('islandora_content_stats_csv_download'),
        '#attributes' => array('class' => array('ics_download_button')),
    );
    $form['actions']['download']['submit']['#submit'][] = 'islandora_content_stats_csv_download';
    $form['actions']['filter']['submit']['#submit'][] = 'islandora_content_stats_data_filter_form_submit';
    return $form;
}


/**
 * @param array $form_state
 * containing filter strings.
 * @param object $mysqlpart
 * query to apply filters on.
 * @param string $timestamp
 * last time queries were run, or empty when history is requested
 * is applied only if recent queries are requested,
 * @param array $headers
 * for the orderby rows
 * @return object
 * $mysqlpart-executed() with applied headers
 */
function apply_query_filters($filters, $mysqlpart,$headers) {
  module_load_include('inc','islandora_content_stats','includes/queries');
        foreach ($filters as $key => $val) {
            if ($val != '') {
                //apply filter if not '' or 'All'
                dpm($val);
                $mysqlpart = $mysqlpart->condition($key, $val);
                dpm($mysqlpart);
            }
        }
     return $mysqlpart->condition('timestamp', latest_run())
                     //->orderByHeader($headers)
                     ->execute();
}



/**
 * Download function
 * applies filters to queries and returns as csv file
 */
function islandora_content_stats_csv_download() {
  $query = 'SELECT * FROM {islandora_content_stats}';
  $filters = array();
   foreach ($_GET as $key => $filter) {
       if ($key == 'cmodel' || $key == 'inst') {
           $filters[$key] = $filter;
       }
   }
    //if requesting historic data
    // if (isset($filters['all'])) {
    //     if ($filters['all'] == 1) {
    //         array_pop($filters);
    //         foreach ($filters as $key => $filter) {
    //             if (($key == 'inst') || ($key == 'cmodel') || ($key == 'coll')) {
    //                 if (strpos($query, 'WHERE') === FALSE) {
    //                     if ($filter != '') {
    //                         $query .= ' WHERE ' . $key . ' = \'' . $filter . '\'';
    //                     }
    //                 } else {
    //                     if ($filter != '') {
    //                         $query .= ' AND ' . $key . ' = \'' . $filter . '\'';
    //                     }
    //                 }
    //             }
    //         }
    //     }
        //when requesting latest data
        // else {
            $max_time = '';
            $q_time = db_query('select MAX(timestamp) from {islandora_content_stats}');
            $max_time = $q_time->fetchField();
            $query .= ' WHERE timestamp = \'' . $max_time . '\'';
            array_pop($filters);
            foreach ($filters as $key => $filter) {
                if ($filter != '') {
                    $query .= ' AND ' . $key . ' = \'' . $filter . '\'';
                }
            }
        //}
    //}
    $results = db_query($query) or die('Sql error : ' . mysql_error());
    drupal_add_http_header('Content-type', 'text/csv; utf-8');
    $attach_str = 'attachment; filename=islandora_content_stats.csv';
    drupal_add_http_header('Content-Disposition', $attach_str);
    $fh = fopen('php://output', 'w');
    fputcsv($fh, array('ID', 'Institution', 'Collection', 'Cmodel', 'Count', 'Timestamp', 'URL'));
    foreach ($results as $result) {
        $not_fobj = $result->cmodel != 'fedora-system:FedoraObject-3.0';
        $not_cmobj = $result->cmodel != 'fedora-system:ContentModel-3.0';
        $not_rootcoll = $result->coll != 'islandora:root';
        if (($not_fobj) && ($not_cmobj) && ($not_rootcoll)) {
            $mapped_results = array(
                $result->id,
                map_institution($result->inst),
                map_collection($result->coll),
                map_cmodels($result->cmodel),
                $result->count,
                strftime('%d:%m:%Y:%r', $result->timestamp),
                map_collection($result->coll, 'csv')
            );
            fputcsv($fh, $mapped_results);
        }
    }
}

/**
 * implements hook_form_submit().
 */

function islandora_content_stats_data_filter_form_submit($form, &$form_state) {
   $form_state['rebuild'] = TRUE;
}
