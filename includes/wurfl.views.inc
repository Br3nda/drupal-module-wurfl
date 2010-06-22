<?php 

// $Id  $

/**
 * Implementation of hook_views_data().
 */
function wurfl_views_data() {
	$data = array();
	
	$data['wurfl_devices']['table']['group']  = t('WURFL');
	
	$data['wurfl_devices']['table']['base'] = array(
	  'field' => 'id',
	  'title' => t('WURFL'),
	);

	$data['wurfl_devices']['id'] = array(
   'title' => t('Device ID'),
	 'help' => t('The device raw id'),
   'field' => array(
     'handler' => 'views_handler_field',
		 'click sortable' => TRUE,
   ),
   'argument' => array(
     'handler' => 'views_handler_argument_string',
   ),
   'filter' => array(
     'handler' => 'views_handler_filter_string',
   ),
   'sort' => array(
     'handler' => 'views_handler_sort',
   ),
 );

	$data['wurfl_devices']['user_agent'] = array(
   'title' => t('User Agent'),
	 'help' => t('The device agent string'),
   'field' => array(
     'handler' => 'views_handler_field',
		 'click sortable' => TRUE,
   ),
   'argument' => array(
     'handler' => 'views_handler_argument_string',
   ),
   'filter' => array(
     'handler' => 'views_handler_filter_string',
   ),
   'sort' => array(
     'handler' => 'views_handler_sort',
   ),
 );
 
	$data['wurfl_devices']['brand_name'] = array(
   'title' => t('Brand Name'),
	 'help' => t('The device brand name'),
   'field' => array(
     'handler' => 'views_handler_field',
		 'click sortable' => TRUE,
   ),
   'argument' => array(
     'handler' => 'views_handler_argument_string',
   ),
   'filter' => array(
     'handler' => 'views_handler_filter_string',
   ),
   'sort' => array(
     'handler' => 'views_handler_sort',
   ),
 );

	$data['wurfl_devices']['model_name'] = array(
   'title' => t('Model Name'),
 	 'help' => t('The device model'),
   'field' => array(
     'handler' => 'views_handler_field',
		 'click sortable' => TRUE,
   ),
   'argument' => array(
     'handler' => 'views_handler_argument_string',
   ),
   'filter' => array(
     'handler' => 'views_handler_filter_string',
   ),
   'sort' => array(
     'handler' => 'views_handler_sort',
   ),
 ); 
 
	$data['wurfl_devices']['series'] = array(
   'title' => t('Series'),
 	 'help' => t('The device series'),
   'field' => array(
     'handler' => 'views_handler_field',
		 'click sortable' => TRUE,
   ),
   'argument' => array(
     'handler' => 'views_handler_argument_string',
   ),
   'filter' => array(
     'handler' => 'views_handler_filter_string',
   ),
   'sort' => array(
     'handler' => 'views_handler_sort',
   ),
 );

	$data['wurfl_devices']['image'] = array(
   'title' => t('Image'),
 	 'help' => t('The device image'),
   'field' => array(
     'handler' => 'wurfl_views_handler_field_image',
	   'sort' => array(
       'handler' => 'views_handler_sort',
     ),
   ),
   
 ); 

 return $data;
}

/**
 * Implementation of hook_views_handlers().
 */
function wurfl_views_handlers() {
  return array(
    'info' => array(
      'path' => drupal_get_path('module', 'wurfl') . '/includes',
    ),
    'handlers' => array(
      // field handlers
      'wurfl_views_handler_field_image' => array(
        'parent' => 'views_handler_field_markup',
      ),
    ),
  );
}
?>