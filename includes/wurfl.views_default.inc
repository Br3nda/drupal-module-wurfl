<?php

/**
 * @file
 * View for listing devices by wurfl.
 */

function wurfl_views_default_views() {
	$view = new view;
	$view->name = 'wurfl';
	$view->description = '';
	$view->tag = '';
	$view->view_php = '';
	$view->base_table = 'wurfl_devices';
	$view->is_cacheable = FALSE;
	$view->api_version = 2;
	$view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */
	$handler = $view->new_display('default', 'Defaults', 'default');
	$handler->override_option('fields', array(
	  'id' => array(
	    'label' => 'Device ID',
	    'alter' => array(
	      'alter_text' => 0,
	      'text' => '',
	      'make_link' => 0,
	      'path' => '',
	      'link_class' => '',
	      'alt' => '',
	      'prefix' => '',
	      'suffix' => '',
	      'target' => '',
	      'help' => '',
	      'trim' => 0,
	      'max_length' => '',
	      'word_boundary' => 1,
	      'ellipsis' => 1,
	      'html' => 0,
	      'strip_tags' => 0,
	    ),
	    'empty' => '',
	    'hide_empty' => 0,
	    'empty_zero' => 0,
	    'exclude' => 0,
	    'id' => 'id',
	    'table' => 'wurfl_devices',
	    'field' => 'id',
	    'override' => array(
	      'button' => 'Override',
	    ),
	    'relationship' => 'none',
	  ),
	  'user_agent' => array(
	    'label' => 'User Agent',
	    'alter' => array(
	      'alter_text' => 0,
	      'text' => '',
	      'make_link' => 0,
	      'path' => '',
	      'link_class' => '',
	      'alt' => '',
	      'prefix' => '',
	      'suffix' => '',
	      'target' => '',
	      'help' => '',
	      'trim' => 0,
	      'max_length' => '128',
	      'word_boundary' => 1,
	      'ellipsis' => 1,
	      'html' => 0,
	      'strip_tags' => 0,
	    ),
	    'empty' => '',
	    'hide_empty' => 0,
	    'empty_zero' => 0,
	    'exclude' => 0,
	    'id' => 'user_agent',
	    'table' => 'wurfl_devices',
	    'field' => 'user_agent',
	    'override' => array(
	      'button' => 'Override',
	    ),
	    'relationship' => 'none',
	  ),
	  'brand_name' => array(
	    'label' => 'Brand Name',
	    'alter' => array(
	      'alter_text' => 0,
	      'text' => '',
	      'make_link' => 0,
	      'path' => '',
	      'link_class' => '',
	      'alt' => '',
	      'prefix' => '',
	      'suffix' => '',
	      'target' => '',
	      'help' => '',
	      'trim' => 0,
	      'max_length' => '',
	      'word_boundary' => 1,
	      'ellipsis' => 1,
	      'html' => 0,
	      'strip_tags' => 0,
	    ),
	    'empty' => '',
	    'hide_empty' => 0,
	    'empty_zero' => 0,
	    'exclude' => 0,
	    'id' => 'brand_name',
	    'table' => 'wurfl_devices',
	    'field' => 'brand_name',
	    'override' => array(
	      'button' => 'Override',
	    ),
	    'relationship' => 'none',
	  ),
	  'model_name' => array(
	    'label' => 'Model Name',
	    'alter' => array(
	      'alter_text' => 0,
	      'text' => '',
	      'make_link' => 0,
	      'path' => '',
	      'link_class' => '',
	      'alt' => '',
	      'prefix' => '',
	      'suffix' => '',
	      'target' => '',
	      'help' => '',
	      'trim' => 0,
	      'max_length' => '',
	      'word_boundary' => 1,
	      'ellipsis' => 1,
	      'html' => 0,
	      'strip_tags' => 0,
	    ),
	    'empty' => '',
	    'hide_empty' => 0,
	    'empty_zero' => 0,
	    'exclude' => 0,
	    'id' => 'model_name',
	    'table' => 'wurfl_devices',
	    'field' => 'model_name',
	    'override' => array(
	      'button' => 'Override',
	    ),
	    'relationship' => 'none',
	  ),
	));
	$handler->override_option('sorts', array(
	  'id' => array(
	    'order' => 'ASC',
	    'id' => 'id',
	    'table' => 'wurfl_devices',
	    'field' => 'id',
	    'override' => array(
	      'button' => 'Override',
	    ),
	    'relationship' => 'none',
	  ),
	));
	$handler->override_option('filters', array(
	  'id' => array(
	    'operator' => 'contains',
	    'value' => '',
	    'group' => '0',
	    'exposed' => TRUE,
	    'expose' => array(
	      'use_operator' => 0,
	      'operator' => 'id_op',
	      'identifier' => 'id',
	      'label' => 'Device ID',
	      'optional' => 1,
	      'remember' => 0,
	    ),
	    'case' => 0,
	    'id' => 'id',
	    'table' => 'wurfl_devices',
	    'field' => 'id',
	    'override' => array(
	      'button' => 'Override',
	    ),
	    'relationship' => 'none',
	  ),
	  'user_agent' => array(
	    'operator' => 'contains',
	    'value' => '',
	    'group' => '0',
	    'exposed' => TRUE,
	    'expose' => array(
	      'use_operator' => 0,
	      'operator' => 'user_agent_op',
	      'identifier' => 'user_agent',
	      'label' => 'User Agent',
	      'optional' => 1,
	      'remember' => 0,
	    ),
	    'case' => 0,
	    'id' => 'user_agent',
	    'table' => 'wurfl_devices',
	    'field' => 'user_agent',
	    'override' => array(
	      'button' => 'Override',
	    ),
	    'relationship' => 'none',
	  ),
	  'brand_name' => array(
	    'operator' => 'contains',
	    'value' => '',
	    'group' => '0',
	    'exposed' => TRUE,
	    'expose' => array(
	      'use_operator' => 0,
	      'operator' => 'brand_name_op',
	      'identifier' => 'brand_name',
	      'label' => 'Brand Name',
	      'optional' => 1,
	      'remember' => 0,
	    ),
	    'case' => 0,
	    'id' => 'brand_name',
	    'table' => 'wurfl_devices',
	    'field' => 'brand_name',
	    'override' => array(
	      'button' => 'Override',
	    ),
	    'relationship' => 'none',
	  ),
	  'model_name' => array(
	    'operator' => 'contains',
	    'value' => '',
	    'group' => '0',
	    'exposed' => TRUE,
	    'expose' => array(
	      'use_operator' => 0,
	      'operator' => 'model_name_op',
	      'identifier' => 'model_name',
	      'label' => 'Model Name',
	      'optional' => 1,
	      'remember' => 0,
	    ),
	    'case' => 0,
	    'id' => 'model_name',
	    'table' => 'wurfl_devices',
	    'field' => 'model_name',
	    'override' => array(
	      'button' => 'Override',
	    ),
	    'relationship' => 'none',
	  ),
	));
	$handler->override_option('access', array(
	  'type' => 'none',
	));
	$handler->override_option('cache', array(
	  'type' => 'none',
	));
	$handler->override_option('use_ajax', TRUE);
	$handler->override_option('items_per_page', 20);
	$handler->override_option('use_pager', '1');
	$handler->override_option('style_plugin', 'table');
	
	

  $views[$view->name] = $view;
  return $views;
}