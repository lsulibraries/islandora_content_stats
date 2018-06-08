<?php

/**
 * @file
 * place to store utility functions for mapping readable lables
 */

/**
 * Maps a machine name to a human readable $label
 * @param string $model
 *  String of model's machine name
 * @return string
 *  String of model's label stripped of repetitous words
 */

function map_cmodels($model) {
  module_load_include('inc', 'islandora', 'includes/utilities');
  $not_cmobj = $model != 'fedora-system:ContentModel-3.0';
  $not_fobj = $model != 'fedora-system:FedoraObject-3.0';
  if ( ($not_fobj) && ($not_cmobj) ) {
    $cmodel_map = islandora_get_content_models();
    if ($model != '' && array_key_exists($model, $cmodel_map)) {
      $label = $cmodel_map[$model];
      $replace = $label['label'];
      $l_replace =  str_replace('Large ', '', $replace);
      $i_replace = str_replace('Islandora ', '', $l_replace);
      $short_label = str_replace(' Content Model', '', $i_replace);
    return $short_label;
    }
  }
}

/**
 * Maps a collection pid to a collection label
 * @param string $pid
 * String of collection pid
 * @param string $context
 *  string indicates function is used for display in a form, or csv download.
 * @return string
 * String of collection title with link to item
 * If you plan to run this module form a local server:
 * add your port number after $_SERVER['SERVER_NAME']
 * IE: $_SERVER['SERVER_NAME'] . '8000' . $iobj_str
 */

function map_collection($pid, $context = FALSE) {
  $iobj_str = '/islandora/object/';
  if ($pid != '' ) {
    $obj = islandora_object_load($pid);
    if (isset($obj)) {
      $label = $obj->label;
      if ($context == 'csv') {
        //add port number after $_SERVER['SERVER_NAME'] if running locally
        $link = 'http://' . $_SERVER['SERVER_NAME'] . $iobj_str . $pid;
      }
      elseif ($context == 'form') {
        $link = '<a href = "../..' . $iobj_str . $pid . '">' . $label . '</a>';
      }
      else{
        $link = $label;
      }
      return $link;
    }
  }
}
