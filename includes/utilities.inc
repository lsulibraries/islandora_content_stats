<?php
module_load_include('inc','islandora','/includes/utilities');

function map_cmodels($model){
  if($model == 'fedora-system:ContentModel-3.0' || $model == 'fedora-system:FedoraObject-3.0'){}
  else{
    $cmodel_map = islandora_get_content_models();
    if($model != '' && array_key_exists($model, $cmodel_map)){
      $label = $cmodel_map[$model];
      $label_f = str_replace(' Content Model','',str_replace('Islandora ','',str_replace('Large ','', $label['label'])));
    return $label_f.'s';
    }
  }
}

function map_collection($pid, $context = FALSE){
  if($pid != '' ){
    $obj = islandora_object_load($pid);
    $label = $obj->label;
    if($context == 'csv'){
      $link = 'http://'.$_SERVER['SERVER_NAME'].':8000/islandora/object/'.$pid;
    }
    elseif($context == 'form'){
      $link = '<a href = "../../islandora/object/'.$pid.'">'.$label.'</a>';
    }
    else{
      $link = $label;
    }
    return $link;
  }
}

//Block Related functions
function islandora_content_stats_post_collection_stats(){
  if(strpos(request_uri(),'collection')){
    $str = request_uri();
    $coll = str_replace('/islandora/object/','',$str);
    return islandora_content_stats_page_cmodels_coll($coll);

  }
}

function check_path(){
  $ns_prefixes = update_namespace_prefixes_cache();
  $page = str_replace('/','',request_uri());
      foreach($ns_prefixes as $prefix){
        if($prefix == $page){
        return true;
      }

    }
}
