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
    module_load_include('inc', 'islandora_content_stats', '/includes/queries');
    module_load_include('inc', 'islandora_content_stats', '/includes/utilities');
    module_load_include('inc', 'islandora_content_stats', 'includes/language');
    $rows = array();
    $headers = array(
//        array('data' => t('ID'),
//              'field' => 'stats.id',
//              'sort' => 'desc',
//              'class' => array('id_header'),
//            ),
        array('data' => t('Instituton/Context'),
              'field' => 'stats.inst',
              'class' => array('inst_header'),
            ),
//        array('data' => t('Collection (optional)'),
//              'field' => 'stats.coll',
//              'class' => array('coll_header'),
//            ),
        array('data' => t('Content Model'),
              'field' => 'stats.cmodel',
              'class' => array('cmodel_header'),
            ),
        array('data' => t('Count'),
              'field' => 'stats.count',
              'class' => array('count_header'),
            ),
//        array('data' => t('Timestamp'),
//              'field' => 'stats.timestamp',
//              'class' => array('stamp_header'),
//            ),
    );
    $max_timestamp = db_query('select MAX(timestamp) from {islandora_content_stats}');
    //second argument in db select is an alias for the table (which is the first argument.)
    $query = db_select('islandora_content_stats', 'stats')
            ->extend('TableSort');
    $query = $query->fields('stats', array('id', 'inst', 'coll', 'cmodel', 'count', 'timestamp'));
    $results = apply_query_filters($form_state, $query, $max_timestamp, $headers);
    foreach ($results as $record) {
        //does not build row when query's cmodel is fedora-Object or ContentModel
        $not_fobj = $record->cmodel != 'fedora-system:FedoraObject-3.0';
        $not_cmobj = $record->cmodel != 'fedora-system:ContentModel-3.0';
        //does not build row when query's inst is is_null
        $not_nullinst = $record->inst != NULL;
        if ($record->count != 0) {
            if (($not_fobj) && ($not_cmobj) && ($not_nullinst)) {
                $rows[$record->id] = array(
//                    'id' => $record->id,
                    'inst' => ($record->inst == '' ? 'All' : map_institution($record->inst)),
//                    'coll' => ($record->coll == '' ? 'All' : map_collection($record->coll, 'form')),
                    'cmodel' => map_cmodels($record->cmodel),
                    'count' => $record->count,
//                    'timestamp' => gmdate('Y-m-d\TH:i:s\Z', $record->timestamp),
                );
            }
        }
    }
    $total_counts = array();
    $timestamp = db_query('select MAX(timestamp) from {islandora_content_stats}');
    $stamp = $timestamp->fetchField();
    $total_query = db_select('islandora_content_stats', 'total');
    $t_fields = array('id', 'inst', 'cmodel', 'count', 'timestamp');
    $totals = $total_query->fields('total', $t_fields);
    $totals_cond = $totals->condition('inst', NULL)
            ->condition('timestamp', $stamp)
            ->execute();
    foreach ($totals_cond as $total) {
        //when query's cmodel is fedora-Object or ContentModel don't build total
        $not_t_fobj = $total->cmodel != 'fedora-system:FedoraObject-3.0';
        $not_t_cmobj = $total->cmodel != 'fedora-system:ContentModel-3.0';
        if ($total->count != 0) {
            if (($not_t_fobj) && ($not_t_cmobj)) {
                $total_counts[$total->id] = array(
                    'cmodel' => map_cmodels($total->cmodel),
                    'count' => $total->count,
                );
            }
        }
    }
    $timestamp = db_query('select MAX(timestamp) from {islandora_content_stats}');
    $inst_totals = array();
    $inst_query = db_select('islandora_content_stats','inst');
    $i_fields = array('id', 'inst', 'coll', 'cmodel', 'count', 'timestamp');
    $i_totals = $inst_query->fields('inst', $i_fields);
    $i_cond = $i_totals->condition('coll', NULL)
            ->condition('timestamp', $stamp)
            ->execute();

    foreach($i_cond as $icount) {
        $not_t_fobj = $total->cmodel != 'fedora-system:FedoraObject-3.0';
        $not_t_cmobj = $total->cmodel != 'fedora-system:ContentModel-3.0';
        if ($icount->count != 0) {
            if (($not_t_fobj) && ($not_t_cmobj)) {
                $inst_totals[$icount->id] = array(
                    'cmodel' => map_cmodels($icount->cmodel),
                    'count' => $icount->count,
                    'inst' => $icount->inst,
                );
            }
        }

    }
    $form = array();
    module_load_include('inc', 'islandora_content_stats', 'include/utilites');
    module_load_include('inc', 'islandora_content_stats', 'include/queries');
    $form['totals']['item'] = array(
        '#type' => 'fieldset',
        '#title' => lang('totals_item_#title'),
        '#attributes' => array('class' => array('ics_totals', 'global')),
    );
    $globals_string = "<div class='global_totals'>";
    foreach ($total_counts as $id => $count) {
             $globals_string .= "<div class='global'><div class='cmodel'>" . $count['cmodel'] . "</div><div class='total'> " . $count['count'] . "</div></div>";

    }
    $globals_string .= "</div>";
    $form['timestamp'] = array(
        '#markup' => "<div class='last_run'>" ."Last run at: " .t(gmdate("M d Y H:i:s", $stamp)) .  "</div>"
    );
    $form['totals']['item'][$id] = array(
      '#markup' => $globals_string,
    );
    $string = '';
    $prefixes = get_all_prefixes();
    sort($prefixes, SORT_NATURAL);
    foreach ($prefixes as $prefix){
    $string = "<div class='inst_wrapper $prefix'>";
      foreach ($inst_totals as $id => $count) {
        if($count['inst'] == $prefix.'-'){
            dpm(map_institution($prefix));
        $string .= "<div class='cmodel_wrapper_inst'><div class='inst'>" . map_institution($prefix) . "</div><div class='cmodel'>" . $count['cmodel'] . "</div><div class='total'> " . $count['count'] ."</div></div>";
        }
      }
        $string .="</div>";
        $form['totals']['item'][$prefix] = array(
            '#markup' => $string,
        );
      }
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
    $form['table-results'] = array(
        '#type' => 'fieldset',
        '#title' => lang('table-collapse_fieldset_#title'),
        '#attributes' => array('class' => array('ics_table_collapse')),
    );
    $form['table-results']['explain'] = array(
        '#markup' => '<div class="ics_table_explain">' . lang('table-collapse_explain') . "</div>",
    );
    $form['table-results']['table'] = array(
        '#theme' => 'table',
        '#header' => $headers,
        '#rows' => $rows,
        '#empty' => lang('table-collapse_table_#empty'),
        '#attributes' => array('class' => array('ics_table')),

    );
    // $form['table-collapse']['pager'] = array(
    //     '#markup' => theme('pager'),
    //     '#attributes' => array('class' => array('ics_table_pager')),
    //     );
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
function apply_query_filters($form_state, $mysqlpart, $timestamp, $headers) {
    if (!isset($form_state['input']['all'])) {
        $mysqlpart = $mysqlpart->condition('timestamp', $timestamp->fetchField());
    }
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
    return $mysqlpart->orderByHeader($headers)
                    ->extend('TableSort')->extend('PagerDefault')->limit(1000)
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
            'all' => TRUE, //$form_state['values']['all'],
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
