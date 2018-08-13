<?php
function islandora_content_stats_language() {
  return [

    'pageHeader' => [
      'title' => 'Page title',
      'default' => ('Content Statistics'),
    ],
    'pageDesc' => [
      'title' => 'Page description',
      'default' => t('Page was last updated: '),
    ],
    'globalHeader' => [
      'title' => 'Global totals title',
      'default' => t('Overall'),
    ],
    'globalDesc' => [
      'title' => "Global totals description",
      'default' => t('Totals for each content model accross all collections')
    ],
    'instHeader' => [
      'title' => 'Institution totals title',
      'default' => t('By Institution'),
    ],
    'instDesc' => [
      'title' => "Institution totals description",
      'default' => t('Totals for each content model, sub institutions of a given institution count towards the parent institution/organization')
    ],
    'tableTitle' => [
      'title' => t('Table title'),
      'default' => t('Table'),
    ],
    'tableDesc' => [
      'type' => 'textarea',
      'title' => t('Table Description'),
      'default' => t('Content from institutions and collections, can be
      filtered by choosing from drop downs below, and pressing the "Filter" button.
      (not every choice combination will return results). Sub-institution totals are added to the total of the parent institution.
      Sort by id, institution/owner, content Model, count, or
      timestamp by clicking headers below.'),
    ],
    'tableEmpty' => [
      'title' => t('Display text when no results'),
      'default' => t('Table is empty'),
    ],
    'filterCmodel' => [
      'title' => 'Type filter description',
      'default' => t('Filter by type of object'),
    ],
    'filterInst' => [
      'title' => 'Institution filter descritpion',
      'default' => t('Filter according to ownership by institution or sub-institution'),
  ],
    'filterSubmitDesc' => [
      'title' => 'Filter button description',
      'default' => t('Press to apply filters to page results.'),
    ],
    'filterSubmit' => [
      'title' => 'Filter button title',
      'default' => t('Filter'),
    ],
    'dlSubmitDesc' => [
      'type' => 'textarea',
      'title' => "Download button description.",
      'default' => t('Downloads results for use in a spreadsheet.
 (always applies current filters)'),
    ],
    'dlSubmit' => [
      'title' => "Download button title.",
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
