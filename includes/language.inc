<?php
function islandora_content_stats_language() {
  return [

    'pageHeader' => [
      'title' => 'Name of page',
      'default' => ('Content Statistics'),
    ],
    'pageHeader_description' => [
      'title' => 'Describe the page',
      'default' => t('Page was last updated: '),
    ],
    'globalHeader' => [
      'title' => 'Global totals header text',
      'default' => t('Overall'),
    ],
    'globalHeader_description' => [
      'title' => "Description for global totals",
      'default' => t('Totals for each content model accross all collections')
    ],
    'instHeader' => [
      'title' => 'Institution totals header text',
      'default' => t('By Institution'),
    ],
    'instHeader_description' => [
      'title' => "Description for institution totals",
      'default' => t('Totals for each content model, sub institutions of a given institution count towards the parent institution/organization')
    ],
    'filterCmodel_desc' => [
      'title' => 'cmodel filter label',
      'default' => t('Filter by type of object'),
  ],
    'filtersGrouping' => [
      'title' => "Label filter fieldset",
      'default' => t('Filter results by type, or institutional/organization ownership'),
    ],
    'filterInst_desc' => [
      'title' => 'Describe inst filter',
      'default' => t('Filter according to ownership by institution or sub-institution'),
  ],
    'filterSubmit_desc' => [
      'title' => 'Filter submit description',
      'default' => t('Press to apply filters to page results.'),
    ],
    'filterSubmit' => [
      'title' => 'Filter submit label',
      'default' => t('Filter'),
    ],
    'tableTitle' => [
      'title' => t('Table title text'),
      'default' => t('Table'),
    ],
    'tableDesc' => [
      'type' => 'textarea',
      'title' => t('Explanatory text for results table'),
      'default' => t('Content from institutions and collections, can be
  filtered by choosing from drop downs below, and pressing the "Filter" button.
  (not every choice combination will return results). Sub-institution totals are added to the total of the parent institution.
  Sort by id, institution/owner, content Model, count, or
  timestamp by clicking headers below.'),
    ],
    'table-collapse_table_#empty' => [
      'title' => t('Display text when no results'),
      'default' => t('Table is empty'),
    ],
    'dlSubmit_desc' => [
      'type' => 'textarea',
      'title' => "Title above download button.",
      'default' => t('Downloads results for use in a spreadsheet.
 (always applies current filters)'),
    ],
    'dlSubmit' => [
      'title' => "Label in download button.",
      'default' => t('Download History as CSV')
    ],
  ];
}

function lang($key) {
  $fullkey = "islandora_content_stats_language_$key";
  $val = variable_get($fullkey);
  if (!$val) {
    $values = islandora_content_stats_language();
    if (!array_key_exists($key, $values)) {
      return '[missing string key]';
    }
    return $values[$key]['default'];
  }
  return $val;
}
