<?php 

// $Id: wurfl.admin.inc,v 1.1.2.8 2010/06/26 23:43:28 kharbat Exp $

function wurfl_overview() {
	if(module_exists('views')) {
		return views_embed_view('wurfl');
	} else {
		drupal_set_message(t('You need install and enable Views module so you view this page.'));
		return '';
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
		'#description' => t('You can try some of these') . ': <br />'.
		'<ul>
		  <li><pre>SonyEricssonK700i/R2AC SEMC-Browser/4.0.2 Profile/MIDP-2.0 Configuration/CLDC-1.1</pre></li>
		  <li><pre>MOT-T720/S_G_05.30.0CR MIB/2.0 Profile/MIDP-1.0 Configuration/CLDC-1.0</pre></li>
		  <li><pre>SAGEM-myX5-2/1.0 Profile/MIDP-2.0 Configuration/CLDC-1.0 UP.Browser/6.2.2.6.d.2 (GUI) MMP/1.0</pre></li>
		  <li><pre>NokiaN90-1/3.0541.5.2 Series60/2.8 Profile/MIDP-2.0 Configuration/CLDC-1.1</pre></li>
		</ul>',
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
			'path' => 'admin/build/wurfl/test/callback',
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

function wurfl_test() {
	global $wurfl_device;
	$image = theme('image', $wurfl_device->device_image) . '<br />';
	$name = $wurfl_device->brand . ' ' . $wurfl_device->model . '<br />';
	$agent = $wurfl_device->user_agent;
	print $image . $name . $agent;
	exit;
}

function wurfl_update_form() {
	$form = array();

	// Check if there are old entries in the database
	$results = db_result(db_query('SELECT COUNT(*) FROM {wurfl_devices} WHERE 1'));

	if($results > 0) {
	
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
	}

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
			$file = file_save_data($request->data, $file);
			$file = $file->filename;
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
	
	foreach($wurfl['devices'] as $device) {
		$device['brand_name'] = $device['product_info']['brand_name'];
		$device['model_name'] = $device['product_info']['model_name'];
  	$device['capabilities'] = serialize($device);
  	
		if($values['old_entries'] == 'update') {
			
			$exists = db_result(db_query("SELECT COUNT(*) as found FROM {wurfl_devices} WHERE id = '%s'", $device['id']));
	  	$record = ((int)$exists > 0 ? drupal_write_record('wurfl_devices', $device, 'id') : drupal_write_record('wurfl_devices', $device));  		
  	} 
  	else if($values['old_entries'] === 'keep') {
  		
  		$exists = db_result(db_query("SELECT COUNT(*) as found FROM {wurfl_devices} WHERE id = '%s'", $device['id']));
  		$record = ((int)$exists > 0 ? false : drupal_write_record('wurfl_devices', $device));
  		
  	} else {
			$record = drupal_write_record('wurfl_devices', $device); 		
  	}
  	
  	if($record == SAVED_NEW) {
  		$new_entries++;
  	} else if($record == SAVED_UPDATED) {
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
	
  /* TODO: use batch api for parsing wurfl and updating database..
   * 
   * $batch = array(
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

// TODO: complete those forms

function wurfl_add_form() {
	return _wurfl_prepare_form();
}

function wurfl_add_form_validate($form, &$form_state) {
	
}

function wurfl_edit_form(&$form_state, $id) {
	return _wurfl_prepare_form($id);
}

function wurfl_save($form, &$form_state) {
	$device = $form_state['values'];
	$exists = db_result(db_query("SELECT COUNT(*) as found FROM {wurfl_devices} WHERE id = '%s'", $device['id']));
  $record = ((int)$exists > 0 ? drupal_write_record('wurfl_devices', $device, 'id') : drupal_write_record('wurfl_devices', $device)); 	
	if($record) {
		drupal_set_message(t('Device has been saved'));
	}
}

function wurfl_delete_form(&$form_state, $id) {
	$query = db_query("DELETE FROM {wurfl_devices} WHERE id = '%s'", $id);
	drupal_set_message(t('Device has been deleted'));	
}

function _wurfl_prepare_form($id = NULL) {
	module_load_include('inc', 'wurfl', 'includes/wurfl.template');
	$template = _wurfl_entry_template();
	
	$wurfl = new wurfl('generic');
	$cap = array();
	
	if($id) {
		$cap = $wurfl->_getDeviceCapabilitiesFromId($id);
	} else {
		$cap = $wurfl->capabilities;			
	}

	$array = array_merge($template, $cap);
	$form = _wurfl_build_form($array);
	
	$form['submit'] = array(
		'#type' => 'submit',
		'#value' => t('Submit'),
	);
			
	$form['#submit'][] = 'wurfl_save';	

	return $form;
}

function _wurfl_build_form($array) {
	$form = array();

	foreach($array as $key => $value) {
		$type = 'markup';
		
		switch(gettype($value)) {
			case 'array':				
				$type = 'fieldset';
				
				$form[$key] = array(
					'#type' => $type,
					'#title' => $key,
				);
				
				$form[$key] += _wurfl_build_form($value);
							
				break;
				
			case 'boolean':
			case '':
			case '0':
			case '1':
			case NULL:
				$type = 'checkbox';
				$form[$key] = array(
					'#type' => $type,
					'#title' => $key,
					'#default_value' => $value,
				);					
				break;
				
			case 'string':
			case 'integer':
				$type = 'textfield';
				$form[$key] = array(
					'#type' => $type,
					'#title' => $key,
					'#default_value' => $value,
				);					
				break;	
		}		
	}
	
	return $form;
}