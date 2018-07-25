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
    'filter_fieldset_#title' => [
      'title' => "Label filter fieldset",
      'default' => t('Filter results by type, ownership, or collection name')
    ],
    'table-collapse_fieldset_#title' => [
      'title' => t('Fieldset title for results table'),
      'default' => t('Table of results'),
    ],
    'table-collapse_explain' => [
      'type' => 'textarea',
      'title' => t('Explanatory text for results table'),
      'default' => t('Content from institutions and collections, can be
  filtered by choosing from drop downs above, and pressing the "Filter" button.
  (not every choice combination will return results)
  Sort by id, institution/owner, collection, content Model, count, or
  timestamp by clicking headers below.'),
    ],
    'table-collapse_table_#empty' => [
      'title' => t('Display text when no results'),
      'default' => t('Table is empty'),
    ],
    'download_#title' => [
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
