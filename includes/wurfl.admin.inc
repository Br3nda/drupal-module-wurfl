<?php 

function wurfl_overview() {
	if(module_exists('views')) {
		return views_embed_view('wurfl');
	}
}

function wurfl_test_form() {
	$form = array();
	
	$form['wurfl_test'] = array(
		'#type' => 'fieldset',
		'#title' => t('Test WURFL detection'),
	);
	
	$form['wurfl_test']['user_agent'] = array(
		'#type' => 'textfield',
		'#title' => t('User agent'),
	);

	$form['submit'] = array(
		'#type' => 'submit',
		'#value' => t('Submit'),
	);
	
	return $form;	
}

function wurfl_test_form_submit($form, &$form_state) {
	$values = $form_state['values'];
	$user_agent = $values['user_agent'];
	$wurfl = new wurfl();
	$matched = $wurfl->getDeviceCapabilities($user_agent);

	if($matched){
		echo "<pre>";
		print_r($wurfl->capabilities);
		echo "</pre>";
	}else{
		echo "<strong>This device could not be identified!</strong>";
	}	
}

function wurfl_update_form() {
	$form = array();
	
	$form['wurfl_update_method'] = array(
		'#type' => 'fieldset',
		'#title' => t('Select WURFL update method'),
	);
	
	$form['wurfl_update_method']['method'] = array(
		'#type' => 'select',
		'#title' => '',
		'#options' => array('local' => 'Local File', 'CVS' => 'CVS'),
	);

	$form['submit'] = array(
		'#type' => 'submit',
		'#value' => t('Update WURFL'),
	);
	
	return $form;
}

function wurfl_update_form_submit($form, &$form_state) {
	module_load_include('inc', 'wurfl', 'wurfl.parser');
	$values = $form_state['values'];
	$devices = array();
	
	switch($values['method']) {
		case 'CVS':
			$headers = array();
			$method = 'GET';
			$url = 'http://wurfl.cvs.sourceforge.net/viewvc/wurfl/xml/wurfl.xml';
			$request = drupal_http_request($url, $headers, $method);
			$file = file_directory_temp() . '/wurfl_update.xml';
			file_save_data($request->data, $file);
			break;
		case 'local':
		default:
			$file = drupal_get_path('module', 'wurfl') . '/wurfl.xml';			
			break;			
	}
	
	$wurfl = wurfl_parse($file);
	$new_entries = 0;
	$updates = 0;
	$errors = 0;
	
	foreach($wurfl['devices'] as $device) {
		$device['brand_name'] = $device['product_info']['brand_name'];
		$device['model_name'] = $device['product_info']['model_name'];
		$device['series'] = $device['product_info']['nokia_series'];
  	$device['capabilities'] = serialize($device);
  	$record = drupal_write_record('wurfl_devices', $device /*, 'id'*/);
  	
  	if($record == SAVED_NEW) {
  		$new_entries++;
  	} else if($record = SAVED_UPDATED) {
  		$updates++;
  	}	else {
  		// some error happened..
  		$errors++;
  	}			
	}
	
	if($wurfl['ver'] && ($new_entries || $updates)) {
		variable_set('wurfl_version', $wurfl['ver']);
		variable_set('wurfl_last_update', $wurfl['last_updated']);
	}
	
	drupal_set_message("{$new_entries} new entries.");
	drupal_set_message("{$updates} updates on existing entries.");	
	
  /* TODO: use batch api for parsing and updating database..
   * 
   * $batch = array(
    //'operations' => $operations,
    'operations' => array(
  		array('wurfl_batch_process', array($file)),
  	),
    'finished' => 'wurfl_batch_finished',
    'title' => t('Processing WURFL Batch'),
    'init_message' => t('WURFL Batch is starting.'),
    'progress_message' => t('Processed @current out of @total.'),
    'error_message' => t('WURFL Batch has encountered an error.'),
    'file' => drupal_get_path('module', 'wurfl') . '/wurfl.batch.inc',
  );
  
  batch_set($batch);*/
}

function wurfl_add_form() {
	return '';	
}

function wurfl_edit_form() {
	return '';	
}

function wurfl_delete_form() {
	return '';	
}
