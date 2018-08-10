<?php
function islandora_content_stats_language() {
  return [
    'totals_item_#title' => [
      'title' => "Label for global item counts",
      'default' => t('Totals for each content model')
    ],
    'data_description' => [
        'type' => 'textarea',
        'title' => "Description for data page",
        'default' => t("Page describes item totals for content models at different levels of hierarchy."),
    ],
    'filter_cmodel#title' => [
      'title' => 'cmodel filter label',
      'default' => t('Filter by type of object'),
  ],
    'filter_fieldset_#title' => [
      'title' => "Label filter fieldset",
      'default' => t('Filter results by type, ownership, or collection name'),
    ],
    'filter_cmodel#description' => [
      'title' => 'Describe cmodel filter',
      'default' => t('Shows results to that match a single content model.'),
    ],
    'filter_inst#title' => [
      'title' => 'Label inst filter',
      'default' => t('Filter according to ownership by institution or sub-institution'),
  ],
    'filter_inst#description' => [
      'type' => 'textarea',
      'title' => 'Describe inst filter',
      'default' => t('Shows results for any items owned by chosen institution or sub-institution.'),
    ],
    'filter_submit#title' => [
      'title' => 'Filter submit description',
      'default' => t('Press to apply filters to page results.'),
    ],
    'filter_submit#value' => [
      'title' => 'Filter submit label',
      'default' => t('Filter'),
    ],
    'table_#title' => [
      'title' => t('Fieldset title for results table'),
      'default' => t('Table'),
    ],
    'table-collapse_explain' => [
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
    'download_#title' => [
      'type' => 'textarea',
      'title' => "Title above download button.",
      'default' => t('Downloads results for use in a spreadsheet.
 (always applies current filters)'),
    ],
    'download_#submit' => [
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
