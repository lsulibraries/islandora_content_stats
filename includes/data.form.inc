<?php

/**
 * @file
 * when queries are run manually, return to this page
 * this page serves as a place to filter and export queries stored in the mysql
 * with a more humant readable format, and links to collection items.
 */


/**
 * Builds form out of query results from db with latest timestamp.
 * accounts for custom content models
 */
function islandora_content_stats_form($form, &$form_state) {
    module_load_include('inc', 'islandora_content_stats', 'include/utilites');
    $form = array();
    $form['filter'] = array(
        '#type' => 'fieldset',
        '#title' => lang('filter_fieldset_#title'),
        '#attributes' => array('class' => array('ics_filters')),
        //this will not work. '#markup' => "<div class='filter'></div>",
    );
    $form['filter']['cmodel'] = array(
        '#type' => 'select',
        '#title' => t('Filter by type of object'),
        '#options' => cmodel_types_for_form(),
        '#description' => t('Shows results to that match a single content model.'),
        '#required' => FALSE,
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
        '#empty_option' => '--none--',
        '#attributes' => array('class' => array('ics_filter_inst')),

    );
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
    $form['download']['button'] = array(
        '#type' => 'submit',
        '#value' => t('Download History as CSV'),
        '#submit' => array('download_data'),
        '#attributes' => array('class' => array('ics_download_button')),
    );
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
function apply_query_filters($form_state, $mysqlpart, $headers) {
//    if (!isset($form_state['input']['all'])) {
//        $mysqlpart = $mysqlpart->condition('timestamp', $timestamp->fetchField());
//    }
    if (isset($form_state['input']['institution'])) {
        $filters = array();
        $filters['inst'] = $form_state['input']['institution'];
        //$filters['coll'] = $form_state['input']['collection'];
        $filters['cmodel'] = $form_state['input']['cmodel'];
        foreach ($filters as $key => $val) {
            if ($val == '') {
                //do nothing to the sqlpart
            } elseif ($val == 'All') {
                //'All' is same as NULL in mysql field
                $mysqlpart = $mysqlpart->isNull($key);
            } else {
                //apply filter if not '' or 'All'
                $mysqlpart = $mysqlpart->condition($key, $val);
            }
        }
    }
    dpm($mysqlpart);
    return $mysqlpart->condition('timestamp', latest_run())
                    //->orderByHeader($headers)
                    ->execute();
}

/**
 * sends the filter options to the islandora_content_stats_csv_download function
 * sent as url parameters to page data/download
 */
function download_data(&$form, &$form_state) {
    $options = array('query' => array(
            'inst' => $form_state['values']['institution'],
            'cmodel' => $form_state['values']['cmodel'],
            //'coll' => $form_state['values']['collection'],
            'all' => FALSE, //$form_state['values']['all'],
        ),
    );
    drupal_goto('data/download', $options);
}

/**
 * implements hook_form_submit().
 */
function islandora_content_stats_form_submit($form, &$form_state) {
    $form_state['rebuild'] = TRUE;
}
