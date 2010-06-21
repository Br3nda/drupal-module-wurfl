<?php

function wurfl_batch_process($file, &$context) {
  if (!isset($context['sandbox']['progress'])) {
  	module_load_include('inc', 'wurfl', 'wurfl.parser');
    $context['sandbox']['progress'] = 0;
    $context['sandbox']['devices'] = wurfl_parse($file);
    $context['sandbox']['max'] = count($context['sandbox']['devices']) ;
  }
  
  $limit = 1000;
	$devices_slice = array_slice($context['sandbox']['devices'], $context['sandbox']['progress'], $limit); 

  foreach($devices_slice  as $device) {
  	$device['capabilities'] = serialize($device);
  	drupal_write_record('wurfl_devices', $device);
  			
    $context['sandbox']['progress']++;
    $context['message'] = t('Now processing %device', array('%device' => $device['deviceID']));
  	// Store some result for post-processing in the finished callback.
    $context['results'][] = check_plain($device['deviceID']);    
  }
  
  if ($context['sandbox']['progress'] != $context['sandbox']['max']) {
    $context['finished'] = $context['sandbox']['progress'] / $context['sandbox']['max'];
  }  
}

function wurfl_batch_finished($success, $results, $operations)  {
  if ($success) {
    // Here we do something meaningful with the results.
    $message = count($results) .' processed.';
    $message .= theme('item_list', $results);
  }
  else {
    // An error occurred.
    // $operations contains the operations that remained unprocessed.
    $error_operation = reset($operations);
    $message = t('An error occurred while processing %error_operation with arguments: @arguments', array('%error_operation' => $error_operation[0], '@arguments' => print_r($error_operation[1], TRUE)));
  }
  
  drupal_set_message($message);
}