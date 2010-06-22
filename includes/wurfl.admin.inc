<?php 

function wurfl_overview() {
	if(module_exists('views')) {
		return views_embed_view('wurfl');
	} else {
		drupal_set_message(t('You need install and enable Views module so you view this page.'));
	}
}

function wurfl_test_form($form) {
	$form = array();
	
	$form['wurfl_test'] = array(
		'#type' => 'fieldset',
		'#title' => t('Test WURFL detection'),
	);
	
	$form['wurfl_test']['user_agent'] = array(
		'#type' => 'textfield',
		'#title' => t('User agent'),
	);
	
	$form['wurfl_capabilities'] = array(
		'#type' => 'fieldset',
		'#title' => t('Capabilities'),
	);
	
	$form['wurfl_capabilities']['results'] = array(
		'#value' => '<div id="wurfl-results"></div>',
	);

	$form['test'] = array(
		'#type' => 'submit',
		'#value' => t('Get Capabilities'),
		'#ahah' => array(
			'path' => 'admin/build/wurfl/callback',
			'wrapper' => 'wurfl-results',
			'method' => 'replace',
			'effect' => 'fade',
		),
	);	
	
	return $form;	
}


function wurfl_test_callback() {
	$user_agent = $_POST['user_agent'];
	$wurfl = new wurfl();
	$matched = $wurfl->getDeviceCapabilities($user_agent);
	
	if($matched) {
		$output = '<pre>' . print_r($wurfl->capabilities, TRUE) . '</pre>';
	} else {
		$output = '<strong>' . t('This device could not be identified!') . '</strong>';
	}

	drupal_json($output);
	exit;
}

function wurfl_update_form() {
	$form = array();
	
	$form['wurfl_update_options'] = array(
		'#type' => 'fieldset',
		'#title' => t('WURFL update options'),
	);
			
	$form['wurfl_update_options']['old_entries'] = array(
	  '#type' => 'radios',
	  '#title' => t('What do you want to do with existing entries ?'),
		'#required' => TRUE,
	  '#options' => array(
			'truncate' => ('Delete all entries'), 
			'update' => t('Modify existing entries'),
			'keep' => t('Keep existing entries untouched'),
		),
	);

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
	
	if($values['old_entries'] == 'truncate') { // truncate table
		db_query('TRUNCATE TABLE {wurfl_devices}');
		drupal_set_message(t('Old entries have been deleted'));
	}
	
	switch($values['method']) {
		case 'CVS':
			$headers = array();
			$method = 'GET';
			$request = drupal_http_request(WURFL_CVS, $headers, $method);
			$file = file_directory_path() . '/wurfl_update.xml';
			file_save_data($request->data, $file);
			break;
		case 'local':
		default:
			$file = WURFL_XML;			
			break;			
	}
	
	$wurfl = wurfl_parse($file);
	$new_entries = 0;
	$updates = 0;
	$errors = 0;
	
	/*
	 * TODO: needs review !!!! 
	 */
	
	foreach($wurfl['devices'] as $device) {
		$device['brand_name'] = $device['product_info']['brand_name'];
		$device['model_name'] = $device['product_info']['model_name'];
		$device['series'] = $device['product_info']['nokia_series'];
  	$device['capabilities'] = serialize($device);
  	
		$exists = db_result(db_query("SELECT COUNT(*) as found FROM {wurfl_devices} WHERE id = '%s'", $device['id']));  	
		
		if($values['old_entries'] == 'tuncate') {
			$record = drupal_write_record('wurfl_devices', $device);
		}	else if($values['old_entries'] == 'update') {
	  	$record = ($exists > 0 ? drupal_write_record('wurfl_devices', $device, 'id') : drupal_write_record('wurfl_devices', $device));  		
  	} else if($values['old_entries'] === 'keep') {
  		$record = ($exists > 0 ? false : drupal_write_record('wurfl_devices', $device));
  	}
  	
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
